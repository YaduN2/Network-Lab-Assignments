#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>

#define MAX_CLIENTS 10
#define MAX_LENGTH 2048
#define NAME_LENGTH 15
#define IP "0.0.0.0"
#define PORT "6969"

int client_count = 0;
int ID = 10;

typedef struct
{
    struct sockaddr_in client_address;
    int client_socket;
    int ID;
    char client_name[NAME_LENGTH];
} client_structure;

client_structure *clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
int server_socket = 0;
int server_stop = 0;

void addClient(client_structure *cl)
{
    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < MAX_CLIENTS; ++i)
        if (!clients[i])
        {
            clients[i] = cl;
            break;
        }

    pthread_mutex_unlock(&clients_mutex);
}

void removeClient(int ID)
{
    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < MAX_CLIENTS; ++i)
        if (clients[i] && clients[i]->ID == ID)
        {
            clients[i] = NULL;
            break;
        }

    pthread_mutex_unlock(&clients_mutex);
}

void sendMessage(char *message, int ID)
{
    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < MAX_CLIENTS; ++i)
        if (clients[i] && clients[i]->ID != ID)
        {
            if (write(clients[i]->client_socket, message, strlen(message)) < 0)
            {
                perror("ERROR: write to descriptor failed");
                break;
            }
        }

    pthread_mutex_unlock(&clients_mutex);
}

void *handleClient(void *client_p)
{
    char message[MAX_LENGTH];
    char temp_message[MAX_LENGTH];
    char client_name[NAME_LENGTH];
    int abort = 0;

    client_count += 1;
    client_structure *client = (client_structure *)client_p;

    if (recv(client->client_socket, client_name, NAME_LENGTH, 0) <= 0)
    {
        abort = 1;
    }
    else
    {
        strcpy(client->client_name, client_name);
        sprintf(message, "[+]%s has joined\n", client->client_name);
        printf("%s", message);
        sendMessage(message, client->ID);
    }

    bzero(message, MAX_LENGTH);

    while (1)
    {
        if (abort)
            break;

        int message_size = recv(client->client_socket, message, sizeof(message), 0);
        if (message_size > 0)
        {
            if (!strcmp(message, "***"))
            {
                sendMessage("***", client->ID);
                printf("[-]%s has left\n", client->client_name);
                sleep(1);
                close(server_socket);
                server_stop = 1;
                abort = 1;
            }
            else if (!strcmp(message, "/") || !strcmp(message, "goodbyecruelworld") || !strcmp(message, "/exit"))
            {
                sprintf(message, "[-]%s has left\n", client->client_name);
                printf("%s", message);
                sendMessage(message, client->ID);
                abort = 1;
            }
            else if (strlen(message) > 0)
            {
                sprintf(temp_message, "%s: %s", client->client_name, message);
                sendMessage(temp_message, client->ID);
                printf("%s\n", temp_message);
            }
        }
        else if (!message_size)
        {
            sprintf(message, "[-]%s has left\n", client->client_name);
            printf("%s", message);
            sendMessage(message, client->ID);
            abort = 1;
        }
        else
        {
            abort = 1;
        }
        bzero(message, MAX_LENGTH);
    }

    client_count -= 1;
    close(client->client_socket);

    removeClient(client->ID);
    free(client);

    pthread_detach(pthread_self());
    return NULL;
}

int main()
{

    int client_socket = 0;
    pthread_t thread_id;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address, client_address;
    server_address.sin_family = AF_INET;
    // server_address.sin_addr.s_addr = inet_addr(IP);
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(atoi(PORT));
    /* Bind */
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("ERROR: Socket binding failed!");
        return 0;
    }

    if (listen(server_socket, MAX_CLIENTS) < 0)
    {
        perror("ERROR: Socket listening failed!");
        return 0;
    }

    printf("######## Chat ########\n");

    while (1)
    {
        if (server_stop)
        {
            pthread_exit(thread_id);
            break;
        }

        socklen_t client_address_length = sizeof(client_address);
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_length);

        if ((client_count + 1) == MAX_CLIENTS)
        {
            close(client_socket);
            continue;
        }

        client_structure *client = (client_structure *)malloc(sizeof(client_structure));
        client->client_address = client_address;
        client->client_socket = client_socket;
        client->ID = ID++;

        addClient(client);
        pthread_create(&thread_id, NULL, &handleClient, (void *)client);

        sleep(1);
    }
    close(server_socket);
    printf("Server Shutting Down!\n");
    return 1;
}