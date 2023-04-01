#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_LENGTH 50
#define ROOT_SERVER_IP "1.1.1.1"
#define ROOT_PORT 6900
#define LOCAL_SERVER_IP "2.2.2.2"
#define LOCAL_PORT 6901
#define TOP_SERVER_IP "3.3.3.3"
#define TOP_PORT 6902
#define AUTH_SERVER_IP "4.4.4.4"
#define AUTH_PORT 6903
#define QUIT_COMMAND "***"

#define CACHE_SIZE 10
struct node
{
    int used;
    char url[MAX_LENGTH];
    char IP[MAX_LENGTH];
};

struct node cache[CACHE_SIZE];

int checkCache(char *url)
{
    for (int i = 0; i < CACHE_SIZE; i++)
    {
        if ((cache[i].used))
        {
            if (!(strcmp(cache[i].url, url)))
            {
                return i;
            }
        }
    }
    return -1;
}

void addName(char *domainName, char *IP)
{
    // for (int i = 0; i < CACHE_SIZE; i++)
    // {
    //     if (!(cache[i].used))
    //     {
    //         strcpy(cache[i].url, domainName);
    //         strcpy(cache[i].IP, IP);
    //         cache[index].used = 1;
    //         return;
    //     }
    // }
    int index = rand() % CACHE_SIZE;
    strcpy(cache[index].url, domainName);
    strcpy(cache[index].IP, IP);
    cache[index].used = 1;
    return;
}

int main()
{
    char client_message[1000], server_response[1000];

    char command[MAX_LENGTH];
    char url[MAX_LENGTH];
    char copy_url[MAX_LENGTH];
    char full_command[MAX_LENGTH * 2];

    for (int i = 0; i < CACHE_SIZE; i++)
        cache[i].used = 0;

    int server_socket;
    server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket < 0)
        return 0;
    // else
    // printf("socket created!\n");

    struct sockaddr_in client_address;
    memset(&client_address, 0, sizeof(client_address));

    struct sockaddr_in local_server_address;
    memset(&local_server_address, 0, sizeof(local_server_address));
    local_server_address.sin_family = AF_INET;
    local_server_address.sin_addr.s_addr = INADDR_ANY;
    // local_server_address.sin_addr.s_addr = inet_addr(LOCAL_SERVER_IP);
    local_server_address.sin_port = htons(LOCAL_PORT);

    struct sockaddr_in root_server_address;
    memset(&root_server_address, 0, sizeof(root_server_address));
    root_server_address.sin_family = AF_INET;
    root_server_address.sin_addr.s_addr = INADDR_ANY;
    // root_server_address.sin_addr.s_addr = inet_addr(ROOT_SERVER_IP);
    root_server_address.sin_port = htons(ROOT_PORT);

    if (bind(server_socket, (struct sockaddr *)&local_server_address,
             sizeof(local_server_address)) < 0)
        return 0;
    // else
    // printf("bind successful!\n");

    unsigned int length_client = sizeof(client_address);
    unsigned int length_root = sizeof(root_server_address);
    printf("awaiting connection!\n");

    char hostIP[16];

    while (1)
    {
        int command_size = recvfrom(server_socket, (char *)full_command, MAX_LENGTH, 0, (struct sockaddr *)&client_address, &length_client);
        full_command[command_size] = '\0';

        if (!strcmp(full_command, QUIT_COMMAND))
        {
            sendto(server_socket, full_command, strlen(command), 0, (const struct sockaddr *)&root_server_address, length_root);
            printf("Quitting Server\n");
            break;
        }

        char *token = strtok(full_command, " ");
        strcpy(command, token);
        token = strtok(NULL, " ");
        strcpy(url, token);
        printf("client- command:%s url:%s\n", command, url);

        strcpy(copy_url, url);
        char *token2 = strtok(copy_url, ".");
        if (strcmp(token2, "www"))
            strcpy(url, copy_url);

        while (1)
        {
            int i = checkCache(url);
            if (i != -1)
            {
                printf("from cache\n");
                strcpy(hostIP, cache[i].IP);
                break;
            }

            sendto(server_socket, url, strlen(url), 0, (const struct sockaddr *)&root_server_address, length_root);

            int hostIP_size = recvfrom(server_socket, (char *)hostIP, MAX_LENGTH, 0, (struct sockaddr *)&root_server_address, &length_root);
            hostIP[hostIP_size] = '\0';
            if (hostIP_size >= 0)
            {
                addName(url, hostIP);
                break;
            }
            printf("something went wrong :( retrying!\n");
        }

        printf("url:%s hostIP:%s\n", url, hostIP);

        // if (!strcmp(url, "nice.com"))
        // {
        //     strcpy(hostIP, "69.69.69.69");
        // }
        // else
        // {
        //     strcpy(hostIP, "169.169.169.169");
        // }

        sendto(server_socket, hostIP, strlen(hostIP), 0, (const struct sockaddr *)&client_address, length_client);
        // printf("Message sent!\n");
    }

    close(server_socket);
    return 0;
}
