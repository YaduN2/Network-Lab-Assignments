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
#define AUTH_PORT_COM 6903
#define AUTH_PORT_NET 6904
#define QUIT_COMMAND "***"

int main()
{
    char client_message[1000], server_response[1000];

    char url[MAX_LENGTH];
    char domainName[MAX_LENGTH];
    char domain[MAX_LENGTH];
    struct hostent *host;

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
    auth_server_address.sin_port = htons(AUTH_PORT_COM);

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

        sprintf(url, "%s.com", domainName);

        host = gethostbyname(url);
        if (!host)
            strcpy(hostIP, "NO IP FOUND");

        strcpy(hostIP, inet_ntoa(*(struct in_addr *)host->h_addr));

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
