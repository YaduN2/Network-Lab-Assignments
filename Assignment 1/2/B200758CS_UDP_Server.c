#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define PORT 6960

char *revstr(char *str1)
{
    int i, temp;
    int len = strlen(str1);
    char rev_str[1000];
    for (i = 0; i < strlen(str1) / 2; i++)
    {
        temp = str1[i];
        str1[i] = str1[len - i - 1];
        str1[len - i - 1] = temp;
    }
    return str1;
}

int main()
{
    char client_message[1000], server_response[1000];

    int server_socket;
    server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket < 0)
    {
        // printf("socket creation failed\n");
        return 0;
    }
    // else
    // printf("socket created!\n");

    struct sockaddr_in server_address, client_address;
    memset(&server_address, 0, sizeof(server_address));
    memset(&client_address, 0, sizeof(client_address));

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_address,
             sizeof(server_address)) < 0)
    {
        // printf("bind failed!\n");
        return 0;
    }
    // else
    // printf("bind successful!\n");

    unsigned int len = sizeof(client_address);

    // printf("awaiting connection!\n");

    int message_size = recvfrom(server_socket, (char *)client_message, 1000, 0, (struct sockaddr *)&client_address, &len);
    client_message[message_size] = '\0';
    // printf("Message receieved!\n");
    // printf("Server- Input:%s\n", client_message);
    printf("%s\n", client_message);

    strcpy(server_response, revstr(client_message));

    // printf("Server- Output:%s\n", server_response);
    printf("%s\n", server_response);
    sendto(server_socket, server_response, strlen(server_response), 0, (const struct sockaddr *)&client_address, len);
    // printf("Message sent!\n");

    close(server_socket);
    return 0;
}
