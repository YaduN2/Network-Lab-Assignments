#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 6960

// Driver code
int main()
{
    int client_socket;
    char server_response[1000], client_message[1000];

    // printf("Enter the input(MAX:1000):");
    // scanf("%[^\n]s", input);
    scanf("%[^\n]%*c", client_message);

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
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // printf("Client- Input:%s\n", client_message);
    printf("%s\n", client_message);
    sendto(client_socket, client_message, strlen(client_message), 0, (struct sockaddr *)&server_address, sizeof(server_address));
    // printf("Message sent!\n");

    int server_response_size;
    unsigned int server_address_size;
    server_response_size = recvfrom(client_socket, server_response, 1000, 0, (struct sockaddr *)&server_address, &server_address_size);
    // printf("Message receieved!\n");

    server_response[server_response_size] = '\0';
    // printf("Client- Output:%s\n", server_response);
    printf("%s\n", server_response);

    close(client_socket);
    return 0;
}
