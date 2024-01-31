#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 1024
#define PORTNO 1234

void handleClient(int clientSocket);

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen, Len = sizeof(clientAddr);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORTNO);

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocket, 5) == -1) {
        perror("Error listening on socket");
        exit(EXIT_FAILURE);
    }

    printf("SMTP Service Ready iitkgp.edu\n");

    while (1) {
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (clientSocket == -1) {
            perror("Error accepting connection");
            continue;
        }

        if (fork() == 0) {
            close(serverSocket);
            handleClient(clientSocket);
            close(clientSocket);
            exit(EXIT_SUCCESS);
        }

        close(clientSocket);
    }

    close(serverSocket);

    return 0;
}

void handleClient(int clientSocket) {
    char buffer[MAX_BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));

    send(clientSocket, "220 iitkgp.edu Service ready\n", 29, 0);

    while (1) {
        recv(clientSocket, buffer, MAX_BUFFER_SIZE, 0);
        printf("C: %s", buffer);

        if (strncmp(buffer, "HELO", 4) == 0) {
            send(clientSocket, "250 OK Hello iitkgp.edu\n", 24, 0);
        } else if (strncmp(buffer, "MAIL FROM:", 10) == 0) {
            send(clientSocket, "250 Sender ok\n", 15, 0);
        } else if (strncmp(buffer, "RCPT TO:", 8) == 0) {
            send(clientSocket, "250 Recipient ok\n", 18, 0);
        } else if (strncmp(buffer, "DATA", 4) == 0) {
            send(clientSocket, "354 Enter mail, end with \".\" on a line by itself\n", 50, 0);

            while (1) {
                recv(clientSocket, buffer, MAX_BUFFER_SIZE, 0);
                printf("C: %s", buffer);

                if (strncmp(buffer, ".", 1) == 0) {
                    send(clientSocket, "250 OK Message accepted for delivery\n", 38, 0);
                    break;
                }
            }
        } else if (strncmp(buffer, "QUIT", 4) == 0) {
            send(clientSocket, "221 iitkgp.edu closing connection\n", 36, 0);
            break;
        }

        memset(buffer, 0, sizeof(buffer));
    }
}
