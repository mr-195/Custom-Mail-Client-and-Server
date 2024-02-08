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
    // take username and passsword from user
    char username[100];
    char password[100];
    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);
    // Receive welcome message
    recv(client_socket, buffer, sizeof(buffer), 0);
    printf("Server: %s", buffer);
    // Send username
    // send USER : username
    char msg[100];
    sprintf(msg, "USER : %s\r\n", username);
    send(client_socket, msg, strlen(msg), 0);
    // Receive response

    
    memset(buffer, '\0', sizeof(buffer));
    memset(msg, '\0', sizeof(msg));
    while(1)
    {
         memset(buffer, '\0', sizeof(buffer));
        int n = recv(client_socket, buffer, sizeof(buffer), 0);
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
        // Check for the end of a line (CRLF)
        if (strstr(buffer, "\r\n") != NULL)
        {
            strcat(msg, buffer);
            break;
        }

        strcat(msg, buffer);
    }
    printf("%s\n", msg);
    // send password 
    char pass[100];
    sprintf(pass,"PASS : %s\r\n", password);
    send(client_socket, pass, strlen(pass), 0);
    // Receive response
    memset(buffer, '\0', sizeof(buffer));
    memset(msg, '\0', sizeof(msg));
    while(1)
    {
         memset(buffer, '\0', sizeof(buffer));
        int n = recv(client_socket, buffer, sizeof(buffer), 0);
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
        // Check for the end of a line (CRLF)
        if (strstr(buffer, "\r\n") != NULL)
        {
            strcat(msg, buffer);
            break;
        }

        strcat(msg, buffer);
    }
    printf("%s\n", msg);

    // // Send a command (e.g., QUIT)
    // send(client_socket, "QUIT\r\n", 6, 0);

    // // Receive response
    // recv(client_socket, buffer, sizeof(buffer), 0);
    // printf("Server: %s", buffer);

    // Close the client socket
    close(client_socket);

    return 0;
}
