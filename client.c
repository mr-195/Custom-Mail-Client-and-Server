#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 1024

void sendMail(char *serverIP, int smtpPort, char *username);

void manageMail(char *serverIP, int pop3Port, char *username, char *password);

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s <server_IP> <smtp_port> <pop3_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *serverIP = argv[1];
    int smtpPort = atoi(argv[2]);
    int pop3Port = atoi(argv[3]);

    int choice;
    char username[50];
    char password[50];

    printf("Enter username: ");
    scanf("%s", username);
    // Remove newline character

    printf("Enter password: ");
    scanf("%s", password);
    do
    {
        // ... (same as before)
        printf("\nOptions:\n1. Manage Mail\n2. Send Mail\n3. Quit\nEnter your choice (1-3): ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:

            // Manage Mail (POP3 commands)
            // Implement POP3 functionality to retrieve and manage emails
            // You will need to implement functions for STAT, LIST, RETR, DELE, RSET commands
            manageMail(serverIP, pop3Port, username, password);

            break;
        case 2:
            // Send Mail (SMTP command)
            sendMail(serverIP, smtpPort, username);
            break;
        case 3:
            // Quit
            printf("Quitting the program.\n");
            break;
        default:
            printf("Invalid choice. Please enter a number between 1 and 3.\n");
        }
    } while (choice != 3);

    return 0;
}

void sendMail(char *serverIP, int smtpPort, char *username)
{
    char buffer[MAX_BUFFER_SIZE];
    int clientSocket;

    // Assuming you get user input for mail here
    // ...

    // Assuming user input is stored in the following variables
    char from[100], to[100], subject[100], message[4096];

    strcpy(from, username);
    getchar();
    printf("Enter To: ");
    fgets(to, sizeof(to), stdin);
    to[strcspn(to, "\n")] = '\0';
    printf("Enter Subject: ");
    fgets(subject, sizeof(subject), stdin);
    subject[strcspn(subject, "\n")] = '\0';

    printf("Enter Message (terminate with a line containing only a fullstop character):\n");
    strcpy(message, ""); // Clear previous content
    while (1)
    {
        char line[1024];
        fgets(line, sizeof(line), stdin);
        if (strcmp(line, ".\n") == 0)
        {
            break;
        }
        strcat(message, line);
    }

    // Connect to SMTP server
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(smtpPort);
    inet_pton(AF_INET, serverIP, &(serverAddr.sin_addr));

    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        perror("Error connecting to SMTP server");
        close(clientSocket);
        exit(EXIT_FAILURE);
    }

    // Receive greeting from SMTP server
    memset(buffer, 0, sizeof(buffer));
    recv(clientSocket, buffer, sizeof(buffer), 0);
    printf("%s", buffer);

    // Send HELO command
    sprintf(buffer, "HELO %s\r\n", serverIP);
    send(clientSocket, buffer, strlen(buffer), 0);

    // Receive response from SMTP server
    memset(buffer, 0, sizeof(buffer));
    recv(clientSocket, buffer, sizeof(buffer), 0);
    printf("%s", buffer);

    // Send MAIL FROM command
    sprintf(buffer, "MAIL FROM: <%s>\r\n", from);
    send(clientSocket, buffer, strlen(buffer), 0);

    // Receive response from SMTP server
    memset(buffer, 0, sizeof(buffer));
    recv(clientSocket, buffer, sizeof(buffer), 0);
    printf("%s", buffer);

    // Send RCPT TO command
    sprintf(buffer, "RCPT TO: <%s>\r\n", to);
    send(clientSocket, buffer, strlen(buffer), 0);

    // Receive response from SMTP server
    memset(buffer, 0, sizeof(buffer));
    recv(clientSocket, buffer, sizeof(buffer), 0);
    printf("%s", buffer);

    // Send DATA command
    sprintf(buffer, "DATA\r\n");
    send(clientSocket, buffer, strlen(buffer), 0);

    // Receive response from SMTP server
    memset(buffer, 0, sizeof(buffer));
    recv(clientSocket, buffer, sizeof(buffer), 0);
    printf("%s", buffer);

    // Send mail content
    // Send mail content
    sprintf(buffer, "From: <%s>\r\n", from);
    send(clientSocket, buffer, strlen(buffer), 0);

    sprintf(buffer, "To: <%s>\r\n", to);
    send(clientSocket, buffer, strlen(buffer), 0);

    sprintf(buffer, "Subject: %s\r\n", subject);
    send(clientSocket, buffer, strlen(buffer), 0);

    // End of headers, start message body
    sprintf(buffer, "%s.END\r\n", message);
    send(clientSocket, buffer, strlen(buffer), 0);

    // Receive response from SMTP server
    memset(buffer, 0, sizeof(buffer));
    recv(clientSocket, buffer, sizeof(buffer), 0);
    printf("%s", buffer);

    // Send QUIT command
    sprintf(buffer, "QUIT\r\n");
    send(clientSocket, buffer, strlen(buffer), 0);

    // Receive response from SMTP server
    memset(buffer, 0, sizeof(buffer));
    recv(clientSocket, buffer, sizeof(buffer), 0);
    printf("%s", buffer);

    // Close the connection
    close(clientSocket);
}

void manageMail(char *serverIP, int pop3Port, char *username, char *password)
{
    char buffer[MAX_BUFFER_SIZE];
    int clientSocket;

    // Connect to POP3 server
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(pop3Port);
    inet_pton(AF_INET, serverIP, &(serverAddr.sin_addr));

    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        perror("Error connecting to POP3 server");
        close(clientSocket);
        exit(EXIT_FAILURE);
    }

    // Receive greeting from POP3 server
    memset(buffer, 0, sizeof(buffer));
    recv(clientSocket, buffer, sizeof(buffer), 0);
    printf("%s", buffer);

    // Send USER command
    sprintf(buffer, "USER %s\r\n", username);
    send(clientSocket, buffer, strlen(buffer), 0);

    // Receive response from POP3 server
    memset(buffer, 0, sizeof(buffer));
    recv(clientSocket, buffer, sizeof(buffer), 0);
    printf("%s", buffer);

    // Process USER response
    if (strstr(buffer, "+OK") != NULL)
    {
        // Send PASS command
        sprintf(buffer, "PASS %s\r\n", password);
        send(clientSocket, buffer, strlen(buffer), 0);

        // Receive response from POP3 server
        memset(buffer, 0, sizeof(buffer));
        recv(clientSocket, buffer, sizeof(buffer), 0);
        printf("%s", buffer);

        // Process PASS response
        if (strstr(buffer, "+OK") != NULL)
        {
            printf("Authentication successful.\n");

            // Enter command loop
            while (1)
            {
                // Get user command
                int arg;
                printf("Enter POP3 command (or QUIT to exit): ");
                scanf("%s", buffer);
                if (strstr(buffer, "RETR") != NULL || strstr(buffer, "DELE") != NULL)
                {
                    scanf("%d", &arg);

                    // Concatenating string s with integer d using sprintf
                    sprintf(buffer, "%s %d", buffer, arg);
                }

                // Remove trailing newline character

                // Send user command to POP3 server
                send(clientSocket, buffer, strlen(buffer), 0);

                if (strstr(buffer, "LIST") != NULL || strstr(buffer, "RETR") != NULL)
                {
                    while (1)
                    {

                        memset(buffer, 0, sizeof(buffer));
                        recv(clientSocket, buffer, sizeof(buffer), 0);

                        // Check if the received data contains ".\n"
                        printf("%s", buffer);
                        if (buffer[0] == '.')
                        {
                            printf("end of reading\n");
                            break;
                        }

                        if (strstr(buffer, "END") != NULL)
                        {

                            break;
                        }

                        buffer[strcspn(buffer, "\r\n")] = '\0';
                    }
                    //     // Process or print the received data as needed
                }
                else if (strstr(buffer, "QUIT") != NULL)
                {
                    memset(buffer, 0, sizeof(buffer));
                    printf("Exiting POP3 command loop.\n");
                    break;
                }
                else if (strstr(buffer, "DELE") != NULL || strstr(buffer, "STAT") != NULL || strstr(buffer, "RSET") != NULL)
                {
                    memset(buffer, 0, sizeof(buffer));
                    recv(clientSocket, buffer, sizeof(buffer), 0);
                    printf("%s\n", buffer);
                }
                else
                {
                    printf("invalid command\n ");
                }

                // Check if the user entered the QUIT command
            }
        }
        else
        {
            printf("Authentication failed. Quitting.\n");
        }
    }
    else
    {
        printf("Username not recognized. Quitting.\n");
    }

    // Close the connection to POP3 server
    close(clientSocket);
}