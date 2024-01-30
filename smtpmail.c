#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/stat.h>

#define MAX_BUFFER_SIZE 1024

int main(int argc,int *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <my_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddress, clientAddress;
    char buffer[MAX_BUFFER_SIZE];

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    // Set server address
    // recieve on port mentioned in command line
    int port = atoi(argv[1]);
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);

    // Bind socket to address
    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Failed to bind socket");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(serverSocket, 5) < 0) {
        perror("Failed to listen for connections");
        exit(EXIT_FAILURE);
    }

    printf("SMTP server listening on port 25...\n");

    while (1) {
        socklen_t clientAddressLength = sizeof(clientAddress);

        // Accept incoming connection
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLength);
        if (clientSocket < 0) {
            perror("Failed to accept connection");
            exit(EXIT_FAILURE);
        }

        printf("New connection accepted\n");

        // Receive data from client
        memset(buffer, 0, sizeof(buffer));
        if (recv(clientSocket, buffer, sizeof(buffer), 0) < 0) {
            perror("Failed to receive data from client");
            exit(EXIT_FAILURE);
        }

        printf("Received data from client:\n%s\n", buffer);

        // Process received data and send response

        // TODO: Implement SMTP protocol logic here

        // Close client socket
        close(clientSocket);
    }

    // Close server socket
    close(serverSocket);

    return 0;
}
