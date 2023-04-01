#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 6960
#define MAX_LENGTH 50
#define ROOT_SERVER_IP "1.1.1.1"
#define ROOT_PORT 6900
#define LOCAL_SERVER_IP "2.2.2.2"
#define LOCAL_PORT 6901
#define QUIT_COMMAND "***"

// Driver code
int main()
{
    int client_socket;
    char client_message[1000], server_response[1000];

    char command[MAX_LENGTH];
    char url[MAX_LENGTH];
    char full_command[MAX_LENGTH * 2];

    // printf("Enter the input(MAX:1000):");
    // scanf("%[^\n]s", input);

    client_socket = socket(AF_INET, SOCK_DGRAM, 0);

    if (client_socket < 0)
    {
        // printf("socket creation failed");
        return 0;
    }
    // else
    // printf("socket created!\n");

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(LOCAL_PORT);
    // server_address.sin_addr.s_addr = inet_addr(LOCAL_SERVER_IP);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // printf("Client- Input:%s\n", client_message);

    while (1)
    {
        scanf("%s", command);
        if (!strcmp(command, QUIT_COMMAND))

        {
            sendto(client_socket, command, strlen(command), 0, (struct sockaddr *)&server_address, sizeof(server_address));
            break;
        }

        if (!(!(strcmp(command, "nslookup")) || !(strcmp(command, "/"))))
            continue;

        scanf("%s", url);

        sprintf(full_command, "%s %s", command, url);

        unsigned int server_address_size;
        int server_response_size;

        while (1)
        {
            sendto(client_socket, full_command, strlen(full_command), 0, (struct sockaddr *)&server_address, sizeof(server_address));

            server_response_size = recvfrom(client_socket, server_response, MAX_LENGTH, 0, (struct sockaddr *)&server_address, &server_address_size);
            // printf("Message receieved!\n");
            if (server_response_size >= 0)
                break;
            printf("something went wrong :( retrying!\n");
        }
        server_response[server_response_size] = '\0';
        // printf("Client- Output:%s\n", server_response);
        printf("%s\n", server_response);
        // printf("Message sent!\n");
    }

    close(client_socket);
    return 0;
}
