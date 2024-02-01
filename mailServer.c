#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define MAX_BUFFER_SIZE 1024

void handleClient(int clientSocket);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("[-] Usage : server [PORTNO]\n");
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);

    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        perror("[-] Error creating socket");
        exit(EXIT_FAILURE);
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        perror("[-] Error binding socket");
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocket, 5) == -1)
    {
        perror("Error listening on socket");
        exit(EXIT_FAILURE);
    }
    printf("==========SMTP Server Running=============\n");

    while (1)
    {
        printf("[+] SMTP Service Ready iitkgp.edu\n");
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (clientSocket == -1)
        {
            perror("[-] Error accepting connection");
            continue;
        }

        if (fork() == 0)
        {
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

void handleClient(int clientSocket)
{
    send(clientSocket, "220 iitkgp.edu Service ready\r\n", 31, 0);
    char buffer[MAX_BUFFER_SIZE];
    char msg[MAX_BUFFER_SIZE];
    memset(buffer, '\0', sizeof(buffer));
    memset(msg, '\0', sizeof(msg));
    while (1)
    {
        memset(buffer, '\0', sizeof(buffer));
        int n = recv(clientSocket, buffer, sizeof(buffer), 0);
        // break when EOF is reached
        if (n == 0)
        {
            break; // Connection closed by the remote peer
        }
        else if (n < 0)
        {
            perror("[-] Error in receiving");
            exit(1);
        }

        // printf("Received: %s\n", buffer);

        // Check for the end of a line (CRLF)
        if (strstr(buffer, "\r\n") != NULL)
        {
            strcat(msg, buffer);
            break;
        }

        strcat(msg, buffer);
    }
    printf("%s\n", msg);
    if (strncmp(msg, "HELO", 4) == 0)
    {
        send(clientSocket, "250 OK Hello iitkgp.edu\r\n", 30, 0);
    }
    else
    {
        send(clientSocket, "500 Error: bad syntax\r\n", 23, 0);
        exit(EXIT_SUCCESS);
    }

    // MAIL FROM

    memset(buffer, '\0', sizeof(buffer));
    memset(msg, '\0', sizeof(msg));
    while (1)
    {
        memset(buffer, '\0', sizeof(buffer));
        int n = recv(clientSocket, buffer, sizeof(buffer), 0);
        // break when EOF is reached
        if (n == 0)
        {
            break; // Connection closed by the remote peer
        }
        else if (n < 0)
        {
            perror("Error in receiving");
            exit(1);
        }

        // Check for the end of a line (CRLF)
        if (strstr(buffer, "\r\n") != NULL)
        {
            strcat(msg, buffer);
            break;
        }

        strcat(msg, buffer);
    }
    printf("%s\n", msg);
    if (strncmp(msg, "MAIL", 4) == 0)
    {
        send(clientSocket, "250 sender OK\r\n", 16, 0);
    }
    else
    {
        send(clientSocket, "500 Error: bad syntax\r\n", 23, 0);
        exit(EXIT_SUCCESS);
    }

    // MAIL FROM

    memset(buffer, '\0', sizeof(buffer));
    memset(msg, '\0', sizeof(msg));
    while (1)
    {
        memset(buffer, '\0', sizeof(buffer));
        int n = recv(clientSocket, buffer, sizeof(buffer), 0);
        // break when EOF is reached
        if (n == 0)
            break;
        if (n < 0)
        {
            perror("[-] Error in receiving\n");
            exit(1);
        }
        if (n >= 2 && buffer[n - 1] == '\n' && buffer[n - 2] == '\r')
        {
            buffer[n - 1] = '\0';
            strcat(msg, buffer);
            break;
        }
        strcat(msg, buffer);
    }
    printf("%s\n", msg);
    if (strncmp(msg, "RCPT To", 7) == 0)
    {
        send(clientSocket, "250 root .. Recipient Ok\r\n", 26, 0);
    }
    else
    {
        send(clientSocket, "500 Error: bad syntax\r\n", 23, 0);
        exit(EXIT_SUCCESS);
    }

    // RCPT TO

    memset(buffer, 0, sizeof(buffer));
    memset(msg, 0, sizeof(msg));
    while (1)
    {
        memset(buffer, 0, sizeof(buffer));
        int n = recv(clientSocket, buffer, sizeof(buffer), 0);
        // break when EOF is reached
        if (n == 0)
            break;
        if (n < 0)
        {
            perror("[-] Error in receiving\n");
            exit(1);
        }
        if (n >= 2 && buffer[n - 1] == '\n' && buffer[n - 2] == '\r')
        {
            buffer[n - 1] = '\0';
            strcat(msg, buffer);
            break;
        }
        strcat(msg, buffer);
    }
    printf("%s\n", msg);
    if (strncmp(msg, "DATA", 4) == 0)
    {
        send(clientSocket, "354 Enter mail, end with . on a line by itself\r\n", 50, 0);
    }
    else
    {
        send(clientSocket, "500 Error: bad syntax\r\n", 23, 0);
        exit(EXIT_SUCCESS);
    }

    // DATA

    memset(buffer, '\0', sizeof(buffer));
    memset(msg, '\0', sizeof(msg));
    while (1)
    {
        memset(buffer, '\0', sizeof(buffer));
        int n = recv(clientSocket, buffer, sizeof(buffer), 0);
        // break when EOF is reached
        if (n == 0)
            break;
        if (n < 0)
        {
            perror("[-] Error in receiving\n");
            exit(1);
        }
        if (buffer[n - 1] == '\n' && buffer[n - 2] == '.')
        {
            buffer[n - 1] = '\0';
            strcat(msg, buffer);
            break;
        }
        strcat(msg, buffer);
    }

    char username[100];
    int i = 0;
    while (msg[i] != 'T' || msg[i + 1] != 'o' || msg[i + 2] != ':' || msg[i + 3] != ' ')
        i++;
    i += 4;
    int j = 0;
    while (msg[i] != '@')
    {
        username[j] = msg[i];
        i++;
        j++;
    }
    username[j] = '\0';

    strcat(username, "/mymailbox");
    FILE *fp = fopen(username, "a");
    // write msg in file
    if (fp != NULL)
    {
        // fprintf(fp, "%s\n", msg);
        // char msg[] = "This is a\nsample string\nwith newline characters.";
        const char delimiter[2] = "\n";

        // Get the first token
        char *token = strtok(msg, delimiter);
        int cnt = 0;
        // Continue getting tokens until there are no more
        time_t rawtime;
        struct tm *timeinfo;
        char buffer[80];

        // Get current time
        time(&rawtime);
        timeinfo = localtime(&rawtime);

        // Format time as a string
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
        // printf("BUFFER %s\n",buffer);
        while (token != NULL)
        {
            fprintf(fp, "%s\n", token);
            cnt++;
            if (cnt == 3)
            {
                fprintf(fp, "Recieved at: %s\n", buffer);
            }

            //  Get the next token
            token = strtok(NULL, delimiter);
        }
        // send 250 Ok Message accepted for delivery
        fclose(fp);
        send(clientSocket, "250 OK Message accepted for delivery\r\n", 38, 0);
    }
    else
    {
        send(clientSocket, "236 user not found\r\n", 23, 0);
    }
    // recieve Quit
    memset(buffer, '\0', sizeof(buffer));
    memset(msg, '\0', sizeof(msg));
    while (1)
    {
        memset(buffer, '\0', sizeof(buffer));
        int n = recv(clientSocket, buffer, sizeof(buffer), 0);
        // break when EOF is reached
        if (n == 0)
            break;
        if (n < 0)
        {
            perror("[-] Error in receiving\n");
            exit(1);
        }
        if (n >= 2 && buffer[n - 1] == '\n' && buffer[n - 2] == '\r')
        {
            buffer[n - 1] = '\0';
            strcat(msg, buffer);
            break;
        }
        strcat(msg, buffer);
    }
    // send 221 iitkgp.edu closing connection
    send(clientSocket, "221 iitkgp.edu closing connection\r\n", 35, 0);
}