#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

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
    char sever_response[1000];
    int server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(6969);

    bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));

    listen(server_socket, 3);

    int client_socket;
    client_socket = accept(server_socket, NULL, NULL);

    char client_input[1000];
    recv(client_socket, &client_input, sizeof(client_input), 0);
    // printf("Server- recieved_input:%s\n", client_input);
    printf("%s\n", client_input);

    strcpy(sever_response, revstr(client_input));

    // printf("Server- Output:%s\n", sever_response);
    printf("%s\n", sever_response);
    send(client_socket, sever_response, sizeof(sever_response), 0);

    close(server_socket);

    return 0;
}