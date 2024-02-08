#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#define POP_PORT 110
#define MAX_BUFFER_SIZE 1024

char *receive_message(int client_socket)
{
    char buffer[1024];        // Adjust buffer size as needed
    char *msg = malloc(1024); // Allocate memory for message buffer

    if (msg == NULL)
    {
        perror("[-] Error in memory allocation");
        exit(1);
    }

    memset(buffer, '\0', sizeof(buffer));

    while (1)
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

    return msg;
}
void handle_client(int client_socket) {


    // Send welcome message
    send(client_socket, "+OK POP3 server ready\r\n", 24, 0);

    // Receive username
    char *rec_msg=receive_message(client_socket);
    printf("%s\n",rec_msg);
    // msg : USER : username
    // check if username is valid
    // extract username from msg
    char username[100];
    sscanf(rec_msg, "USER : %s\r\n", username);
    // printf("Username: %s\n", username);
    // check if username is valid
    // find the username in the file where it is the first word of some line 
    FILE *fp = fopen("user.txt", "r");
    if (fp == NULL)
    {
        perror("[-] Error in opening file");
        exit(1);
    }
    char line[100];
    int found = 0;
    // compare the first word of each line 
    while(fgets(line, sizeof(line), fp))
    {
        char * token = strtok(line, " ");
        if (strcmp(token, username) == 0)
        {
            found = 1;
            break;
        }
    }
    fclose(fp);
    if(found)
    {
        send(client_socket, "+OK user found\r\n", 17, 0);
    }
    else
    {
        send(client_socket, "-ERR user not found\r\n", 21, 0);
        close(client_socket);
        exit(1);
    }
    // receive password
   
    rec_msg=receive_message(client_socket);
    printf("%s\n", rec_msg);  
    // extract password from msg
    char password[100];
    sscanf(rec_msg, "PASS : %s\r\n", password);
    // printf("Password: %s\n", password);
    // check if password is valid for the username
    // find the username in the file where it is the first word of some line
    fp = fopen("user.txt", "r");
    if (fp == NULL)
    {
        perror("[-] Error in opening file");
        exit(1);
    }
    found = 0;
    // compare the first word of each line
    while(fgets(line, sizeof(line), fp))
    {
        char * token = strtok(line, " ");
        if (strcmp(token, username) == 0)
        {
            token = strtok(NULL, " ");
            token[strlen(token) - 1] = '\0';
            if (strcmp(token, password) == 0)
            {
                found = 1;
                break;
            }
        }
    }
    fclose(fp);
    if(found)
    {
        send(client_socket, "+OK password correct\r\n", 23, 0);
    }
    else
    {
        send(client_socket, "-ERR password incorrect\r\n", 25, 0);
        close(client_socket);
        exit(1);
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

   // concurrent server
   printf("==========POP3 Server Running=============\n");
   while(1)
   {
         printf("[+] POP3 Service Ready iitkgp.edu\n");
         client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
         if (client_socket == -1)
         {
              perror("[-] Error accepting connection");
              continue;
         }
    
         if (fork() == 0)
         {
              close(server_socket);
              handle_client(client_socket);
              close(client_socket);
              exit(EXIT_SUCCESS);
         }
         close(client_socket);
   }
   

    return 0;
}
