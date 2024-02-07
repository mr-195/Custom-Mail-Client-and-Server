#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define POP_SERVER_IP "127.0.0.1"
#define POP_PORT 110
#define MAX_BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[MAX_BUFFER_SIZE];

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Error creating client socket");
        exit(EXIT_FAILURE);
    }

    // Set up server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(POP_SERVER_IP);
    server_addr.sin_port = htons(POP_PORT);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error connecting to server");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // Receive welcome message
    recv(client_socket, buffer, sizeof(buffer), 0);
    printf("Server: %s", buffer);

    // Send a command (e.g., QUIT)
    send(client_socket, "QUIT\r\n", 6, 0);

    // Receive response
    recv(client_socket, buffer, sizeof(buffer), 0);
    printf("Server: %s", buffer);

    // Close the client socket
    close(client_socket);

    return 0;
}
