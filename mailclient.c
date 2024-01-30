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

#define SMTP_SERVER "smtp.example.com"
#define SMTP_PORT 25

int main() {
    int sock;
    struct sockaddr_in server;
    char message[1024], server_reply[2000];

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("Could not create socket");
        return 1;
    }

    server.sin_addr.s_addr = inet_addr(SMTP_SERVER);
    server.sin_family = AF_INET;
    server.sin_port = htons(SMTP_PORT);

    // Connect to remote server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Connect failed. Error");
        return 1;
    }

    // Receive the server response
    if (recv(sock, server_reply, 2000, 0) < 0) {
        printf("Receive failed");
        return 1;
    }

    printf("Server reply: %s\n", server_reply);

    // Send EHLO command
    strcpy(message, "EHLO example.com\r\n");
    if (send(sock, message, strlen(message), 0) < 0) {
        printf("Send failed");
        return 1;
    }

    // Receive the server response
    if (recv(sock, server_reply, 2000, 0) < 0) {
        printf("Receive failed");
        return 1;
    }

    printf("Server reply: %s\n", server_reply);

    // Send MAIL FROM command
    strcpy(message, "MAIL FROM: <sender@example.com>\r\n");
    if (send(sock, message, strlen(message), 0) < 0) {
        printf("Send failed");
        return 1;
    }

    // Receive the server response
    if (recv(sock, server_reply, 2000, 0) < 0) {
        printf("Receive failed");
        return 1;
    }

    printf("Server reply: %s\n", server_reply);

    // Send RCPT TO command
    strcpy(message, "RCPT TO: <recipient@example.com>\r\n");
    if (send(sock, message, strlen(message), 0) < 0) {
        printf("Send failed");
        return 1;
    }

    // Receive the server response
    if (recv(sock, server_reply, 2000, 0) < 0) {
        printf("Receive failed");
        return 1;
    }

    printf("Server reply: %s\n", server_reply);

    // Send DATA command
    strcpy(message, "DATA\r\n");
    if (send(sock, message, strlen(message), 0) < 0) {
        printf("Send failed");
        return 1;
    }

    // Receive the server response
    if (recv(sock, server_reply, 2000, 0) < 0) {
        printf("Receive failed");
        return 1;
    }

    printf("Server reply: %s\n", server_reply);

    // Send email content
    strcpy(message, "Subject: Test Email\r\n");
    strcat(message, "From: sender@example.com\r\n");
    strcat(message, "To: recipient@example.com\r\n");
    strcat(message, "\r\n");
    strcat(message, "This is the body of the email.\r\n");
    strcat(message, ".\r\n");
    if (send(sock, message, strlen(message), 0) < 0) {
        printf("Send failed");
        return 1;
    }

    // Receive the server response
    if (recv(sock, server_reply, 2000, 0) < 0) {
        printf("Receive failed");
        return 1;
    }

    printf("Server reply: %s\n", server_reply);

    // Send QUIT command
    strcpy(message, "QUIT\r\n");
    if (send(sock, message, strlen(message), 0) < 0) {
        printf("Send failed");
        return 1;
    }

    // Receive the server response
    if (recv(sock, server_reply, 2000, 0) < 0) {
        printf("Receive failed");
        return 1;
    }

    printf("Server reply: %s\n", server_reply);

    close(sock);
    return 0;
}   
