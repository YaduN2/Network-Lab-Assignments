#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

int main()
{
    char input[1000];
    // printf("Enter the input(MAX:1000):");
    // scanf("%[^\n]s", input);
    scanf("%[^\n]%*c", input);
    // gets(input);
    // fgets(input, 1000, stdin);

    // printf("Client- input:%s\n", input);
    printf("%s\n", input);

    int network_socket;
    network_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(6969);

    int connection_status = connect(network_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    if (connection_status == -1)
    { // printf("Connection unsuccesssful!\n");
        return 0;
    }

    send(network_socket, input, sizeof(input), 0);

    char server_response[1000];
    recv(network_socket, server_response, sizeof(server_response), 0);

    // printf("Client- Output:%s\n", server_response);
    printf("%s\n", server_response);

    close(network_socket);
    return 0;
}
