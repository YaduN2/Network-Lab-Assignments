#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_CLIENTS 10
#define MAX_LENGTH 2048
#define NAME_LENGTH 15
#define IP "0.0.0.0"
#define PORT "6969"

// Global variables
int flag = 0;
int network_socket = 0;
char name[NAME_LENGTH];

void start_symbol()
{
    printf("%s", "> ");
    fflush(stdout);
}

void sendHandler()
{
    char message[MAX_LENGTH];

    while (1)
    {
        start_symbol();
        scanf("%[^\n]%*c", message);

        send(network_socket, message, strlen(message), 0);
        if (!strcmp(message, "***") || !strcmp(message, "/") || !strcmp(message, "goodbyecruelworld") || !strcmp(message, "/exit"))
        {
            printf("Disconnected from sever!\n");
            break;
        }

        bzero(message, MAX_LENGTH);
    }
    flag = 1;
}

void recvHandler()
{
    char message[MAX_LENGTH];

    while (1)
    {
        int receive = recv(network_socket, message, MAX_LENGTH, 0);
        if (receive > 0)
        {
            if (!strcmp(message, "***"))
            {
                printf("Disconnected from sever!\n");
                strcpy(message, "/");
                send(network_socket, message, strlen(message), 0);
                flag = 1;
                break;
            }
            printf("%s\n", message);
            start_symbol();
        }
        else if (receive == 0)
        {
            break;
        }
        memset(message, 0, sizeof(message));
    }
}

int main()
{

    printf("Username:");
    scanf("%[^\n]%*c", name);

    if (strlen(name) > NAME_LENGTH)
    {
        printf("Name must be less than %d.\n", NAME_LENGTH);
        return 1;
    }

    network_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    // server_addr.sin_addr.s_addr = inet_addr(IP);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(atoi(PORT));

    int connection_status = connect(network_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (connection_status == -1)
    {
        printf("ERROR: connect\n");
        return 1;
    }

    send(network_socket, name, NAME_LENGTH, 0);
    printf("######## Chat ########\n");

    pthread_t thread_sendHandler;
    if (pthread_create(&thread_sendHandler, NULL, (void *)sendHandler, NULL) != 0)
    {
        printf("ERROR: pthread\n");
        return 1;
    }

    pthread_t thread_recvHandler;
    if (pthread_create(&thread_recvHandler, NULL, (void *)recvHandler, NULL) != 0)
    {
        printf("ERROR: pthread\n");
        return 1;
    }

    while (1)
        if (flag)
            break;

    close(network_socket);

    return 0;
}