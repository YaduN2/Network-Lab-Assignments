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

int main()
{
    char client_message[1000], server_response[1000];

    char url[MAX_LENGTH];
    char domainName[MAX_LENGTH];
    char domain[MAX_LENGTH] = "x";

    int server_socket;
    server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket < 0)
        return 1;
    // else
    // printf("socket created!\n");

    struct sockaddr_in root_server_address;
    memset(&root_server_address, 0, sizeof(root_server_address));
    root_server_address.sin_family = AF_INET;
    root_server_address.sin_addr.s_addr = INADDR_ANY;
    root_server_address.sin_port = htons(ROOT_PORT);
    // root_server_address.sin_addr.s_addr = inet_addr(ROOT_SERVER_IP);

    struct sockaddr_in local_server_address;
    memset(&local_server_address, 0, sizeof(local_server_address));
    local_server_address.sin_family = AF_INET;
    local_server_address.sin_addr.s_addr = INADDR_ANY;
    local_server_address.sin_port = htons(LOCAL_PORT);
    // local_server_address.sin_addr.s_addr = inet_addr(LOCAL_SERVER_IP);

    struct sockaddr_in top_server_address;
    memset(&top_server_address, 0, sizeof(top_server_address));
    top_server_address.sin_family = AF_INET;
    top_server_address.sin_addr.s_addr = INADDR_ANY;
    top_server_address.sin_port = htons(TOP_PORT);
    // root_server_address.sin_addr.s_addr = inet_addr(ROOT_SERVER_IP);

    if (bind(server_socket, (struct sockaddr *)&root_server_address, sizeof(root_server_address)) < 0)
    {
        printf("bind unsuccessful!\n");
        return 1;
    }
    // else
    //     printf("bind successful!\n");

    unsigned int length_local = sizeof(local_server_address);
    unsigned int length_root = sizeof(root_server_address);
    unsigned int length_top = sizeof(top_server_address);

    printf("awaiting connection!\n");

    char hostIP[16];

    while (1)
    {
        int url_size = recvfrom(server_socket, (char *)url, MAX_LENGTH, 0, (struct sockaddr *)&local_server_address, &length_local);
        url[url_size] = '\0';

        if (!strcmp(url, QUIT_COMMAND))
        {
            sendto(server_socket, url, strlen(url), 0, (const struct sockaddr *)&top_server_address, length_top);
            printf("Quitting Server\n");
            break;
        }

        // char *token = strtok(url, ".");
        // strcpy(domainName, token);
        // token = strtok(NULL, " ");
        // strcpy(domain, token);
        // printf("root- domainName:%s domain:%s\n", domainName, domain);
        printf("local- url:%s\n", url);

        if (!strcmp(domain, "com"))
        {
            // printf(".com\n");
            while (1)
            {
                sendto(server_socket, url, strlen(url), 0, (const struct sockaddr *)&top_server_address, length_top);

                int hostIP_size = recvfrom(server_socket, (char *)hostIP, MAX_LENGTH, 0, (struct sockaddr *)&top_server_address, &length_top);
                hostIP[hostIP_size] = '\0';
                if (hostIP_size >= 0)
                    break;
                printf("something went wrong :( retrying!\n");
            }
        }
        else if (!strcmp(domain, "net"))
        {
            // printf(".net\n");

            while (1)
            {
                sendto(server_socket, url, strlen(url), 0, (const struct sockaddr *)&top_server_address, length_top);

                int hostIP_size = recvfrom(server_socket, (char *)hostIP, MAX_LENGTH, 0, (struct sockaddr *)&top_server_address, &length_top);
                hostIP[hostIP_size] = '\0';
                if (hostIP_size >= 0)
                    break;
                printf("something went wrong :( retrying!\n");
            }
        }
        else
        {
            // printf(".other\n");

            while (1)
            {
                sendto(server_socket, url, strlen(url), 0, (const struct sockaddr *)&top_server_address, length_top);

                int hostIP_size = recvfrom(server_socket, (char *)hostIP, MAX_LENGTH, 0, (struct sockaddr *)&top_server_address, &length_top);
                hostIP[hostIP_size] = '\0';
                if (hostIP_size >= 0)
                    break;
                printf("something went wrong :( retrying!\n");
            }
        }

        // printf("after: url:%s hostIP:%s\n", url, hostIP);

        // if (!strcmp(domainName, "nice"))
        // {
        //     strcpy(hostIP, "69.69.69.69");
        // }
        // else
        // {
        //     strcpy(hostIP, "169.169.169.169");
        // }

        sendto(server_socket, hostIP, strlen(hostIP), 0, (const struct sockaddr *)&local_server_address, length_local);
        // printf("Message sent!\n");
    }

    close(server_socket);
    return 0;
}
