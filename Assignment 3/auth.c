#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

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

    char url[MAX_LENGTH];
    char domainName[MAX_LENGTH];
    char domain[MAX_LENGTH];
    struct hostent *host;

    for (int i = 0; i < CACHE_SIZE; i++)
        cache[i].used = 0;

    int server_socket;
    server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket < 0)
        return 0;
    // else
    // printf("socket created!\n");

    struct sockaddr_in auth_server_address;
    memset(&auth_server_address, 0, sizeof(auth_server_address));
    auth_server_address.sin_family = AF_INET;
    auth_server_address.sin_addr.s_addr = INADDR_ANY;
    // auth_server_address.sin_addr.s_addr = inet_addr(LOCAL_SERVER_IP);
    auth_server_address.sin_port = htons(AUTH_PORT);

    struct sockaddr_in top_server_address;
    memset(&top_server_address, 0, sizeof(top_server_address));
    top_server_address.sin_family = AF_INET;
    top_server_address.sin_addr.s_addr = INADDR_ANY;
    // root_server_address.sin_addr.s_addr = inet_addr(ROOT_SERVER_IP);
    top_server_address.sin_port = htons(TOP_PORT);

    if (bind(server_socket, (struct sockaddr *)&auth_server_address,
             sizeof(auth_server_address)) < 0)
    {
        // printf("bind unsuccessful!\n");
        return 0;
    }
    // else
    //     printf("bind successful!\n");

    unsigned int length_auth = sizeof(auth_server_address);
    unsigned int length_top = sizeof(top_server_address);

    printf("awaiting connection!\n");

    char hostIP[16];

    while (1)
    {
        int domainName_size = recvfrom(server_socket, (char *)domainName, MAX_LENGTH, 0, (struct sockaddr *)&top_server_address, &length_top);
        domainName[domainName_size] = '\0';

        printf("top- DomainName:%s\n", domainName);

        if (!strcmp(domainName, QUIT_COMMAND))
        {
            printf("Quitting Server\n");
            break;
        }
        int i = checkCache(domainName);
        if (i != -1)
        {
            printf("from cache\n");
            strcpy(hostIP, cache[i].IP);
        }
        else
        {
            printf("NOT from cache\n");
            host = gethostbyname(domainName);
            if (!host)
            {
                strcpy(hostIP, "NO IP FOUND");
            }
            else
            {
                strcpy(hostIP, inet_ntoa(*(struct in_addr *)host->h_addr));
            }
            addName(domainName, hostIP);
        }

        // if (!strcmp(domainName, "nice"))
        // {
        //     strcpy(hostIP, "69.69.69.69");
        // }
        // else
        // {
        //     strcpy(hostIP, "169.169.169.169");
        // }

        printf("DomainName:%s hostIP:%s\n", domainName, hostIP);

        sendto(server_socket, hostIP, strlen(hostIP), 0, (const struct sockaddr *)&top_server_address, length_top);
        // printf("Message sent!\n");
    }

    close(server_socket);
    return 0;
}
