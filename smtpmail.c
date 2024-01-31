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

void recieveMail(int clientSocket)
{
    char buffer[MAX_BUFFER_SIZE];
    char username[255];
    char sender[255];
    // Read the email content from the client
    // Read the username
    // Mail is of format
    /*From: <username>@<domain_name>
 To: <username>@<domain_name>
 Subject: <subject string, max 100 characters>
 Received: <time at which received, in date : hour : minute>
 <Message body – one or more lines> */
    size_t n;
    while ((n = read(clientSocket, buffer, MAX_BUFFER_SIZE)) > 0)
    {
        buffer[n] = '\0';
        printf("%s", buffer);
        if (strncmp(buffer, "From:", 5) == 0)
        {
            char *token = strtok(buffer, " ");
            token = strtok(NULL, " ");
            strcpy(sender, token);
            printf("Sender is %s\n", sender);
        }
        else if (strncmp(buffer, "To:", 3) == 0)
        {
            char *token = strtok(buffer, " ");
            token = strtok(NULL, " ");
            strcpy(username, token);
            printf("Username is %s\n", username);
        }
        else if (strncmp(buffer, "Subject:", 8) == 0)
        {
            char *token = strtok(buffer, " ");
            token = strtok(NULL, " ");
            printf("Subject is %s\n", token);
        }
        else if (strncmp(buffer, "Received:", 9) == 0)
        {
            char *token = strtok(buffer, " ");
            token = strtok(NULL, " ");
            printf("Received at %s\n", token);
        }
        else
        {
            printf("Message is %s\n", buffer);
        }
    }
}
int main(int argc, int *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <my_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int serverSocket;
    struct sockaddr_in serverAddress, clientAddress;
    char buffer[MAX_BUFFER_SIZE];

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
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
    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        perror("Failed to bind socket");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(serverSocket, 5) < 0)
    {
        perror("Failed to listen for connections");
        exit(EXIT_FAILURE);
    }

    printf("SMTP server listening on port %d...\n", port);

    while (1)
    {
        socklen_t clientAddressLength = sizeof(clientAddress);

        // Accept incoming connection
        int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLength);
        if (clientSocket < 0)
        {
            perror("Failed to accept connection");
            exit(EXIT_FAILURE);
        }

        printf("New connection accepted\n");
        pid_t pid = fork();

        if (pid < 0)
        {
            perror("Failed to fork");
            exit(EXIT_FAILURE);
        }
        if (pid == 0)
        {
            close(serverSocket);
            recieveMail(clientSocket, );

            // Process received data and send response
        }
        else
        {
            close(clientSocket);
            continue;
        }
    }

    // Close server socket
    close(serverSocket);

    return 0;
}