#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define POP_PORT 110
#define MAX_BUFFER_SIZE 1024

void handle_client(int client_socket) {
    char buffer[MAX_BUFFER_SIZE];

    // Send welcome message
    send(client_socket, "+OK POP3 server ready\r\n", 24, 0);

    while (1) {
        // Receive client command
        recv(client_socket, buffer, sizeof(buffer), 0);

        // Implement your POP server logic here based on the client's command
        // For simplicity, let's assume the client will send "QUIT" to terminate

        if (strstr(buffer, "QUIT")) {
            send(client_socket, "+OK Bye!\r\n", 10, 0);
            break;
        } else {
            send(client_socket, "-ERR Unknown command\r\n", 21, 0);
        }
    }

    // Close the client socket
    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating server socket");
        exit(EXIT_FAILURE);
    }

    // Set up server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(POP_PORT);

    // Bind the socket to the specified address and port
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding server socket");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) == -1) {
        perror("Error listening for connections");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("POP server listening on port %d...\n", POP_PORT);

    while (1) {
        // Accept incoming connection
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_socket == -1) {
            perror("Error accepting connection");
            continue;
        }

        printf("Client connected: %s\n", inet_ntoa(client_addr.sin_addr));

        // Handle client communication
        handle_client(client_socket);
    }

    // Close the server socket (this part is not reachable in this example)
    close(server_socket);

    return 0;
}
