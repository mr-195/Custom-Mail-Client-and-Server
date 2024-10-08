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
#define POP_PORT 115
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
int main()
{

    // take username and passsword from user
    while (1)
    {
        int client_socket;
        struct sockaddr_in server_addr;
        char buffer[MAX_BUFFER_SIZE];

        // Create socket
        client_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (client_socket == -1)
        {
            perror("Error creating client socket");
            exit(EXIT_FAILURE);
        }

        // Set up server address structure
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = inet_addr(POP_SERVER_IP);
        server_addr.sin_port = htons(POP_PORT);

        // Connect to the server
        if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
        {
            perror("Error connecting to server");
            close(client_socket);
            exit(EXIT_FAILURE);
        }
        // Authorization State
        char username[100] = "Manaswi";
        char password[100] = "Linux";
        // printf("Enter username: ");
        // scanf("%s", username);
        // printf("Enter password: ");
        // scanf("%s", password);
        // Receive welcome message
        recv(client_socket, buffer, sizeof(buffer), 0);
        printf("Server: %s", buffer);
        // Send username
        // send USER : username
        char msg[100];
        sprintf(msg, "USER : %s\r\n", username);
        printf("Sending Message: %s\n", msg);
        send(client_socket, msg, strlen(msg), 0);
        // Receive response
        char *rec_msg = receive_message(client_socket);
        printf("%s\n", rec_msg);
        // check if username is valid
        if (strncmp(rec_msg, "-ERR", 4) == 0)
        {
            printf("Username is incorrect\n");
            continue;
        }

        // send password
        char pass[100];
        sprintf(pass, "PASS : %s\r\n", password);
        printf("Sending Message: %s\n", pass);
        send(client_socket, pass, strlen(pass), 0);
        // Receive response
        rec_msg = receive_message(client_socket);
        printf("%s\n", rec_msg);
        // check if password is valid
        if (strncmp(rec_msg, "-ERR", 4) == 0)
        {
            printf("Password is incorrect\n");
            continue;
        }
        // Transaction State
        while (1)
        {
            // send STAT command
            char stat[100];
            sprintf(stat, "STAT\r\n");
            // send to server
            send(client_socket, stat, strlen(stat), 0);
            // recieve +OK num
            char *rec_msg = receive_message(client_socket);
            printf("%s\n", rec_msg);
            // extract the number of messages
            int num;
            sscanf(rec_msg, "+OK %d", &num);
            // error check
            if (strncmp(rec_msg, "-ERR", 4) == 0)
            {
                printf("Error in STAT command\n");
                break;
            }
            // send LIST command
            char list[100];
            sprintf(list, "LIST\r\n");
            // send to server
            send(client_socket, list, strlen(list), 0);
            // recieve the list of messages
            // recieve +OK number of messages
            rec_msg = receive_message(client_socket);
            printf("%s\n", rec_msg);
            // error check
            if (strncmp(rec_msg, "-ERR", 4) == 0)
            {
                printf("Error in LIST command\n");
                break;
            }
            // recieve in the format of Sl. No. <Sender’s email id> <When received, in date : hour : minute> <Subject>
            while (1)
            {
                char *rec_msg = receive_message(client_socket);
                printf("%s\n", rec_msg);
                if (strstr(rec_msg, ".\r\n") != NULL)
                {
                    break;
                }
            }
            // for(int i=0; i<num; i++){
            //     char *rec_msg = receive_message(client_socket);
            //     printf("%s", rec_msg);
            // }
            printf("Enter the message number to read: ");
            int choice;
            scanf("%d", &choice);
            // send RETR command
            char retr[100];
            sprintf(retr, "RETR %d\r\n", choice);
            // send to server
            send(client_socket, retr, strlen(retr), 0);
            // recieve the message
            rec_msg = receive_message(client_socket); // +OK message follows
            printf("%s\n", rec_msg);
            // error check
            if (strncmp(rec_msg, "-ERR", 4) == 0)
            {
                printf("Error in RETR command\n");
                break;
            }
            else
            {
                // clear rec_msg
                // rec_msg = "";
                // rec_msg = receive_message(client_socket);
                // printf("%s", rec_msg);
            }
            // printf("Here \n");
            char cl;
            // DELETE message
            printf("Enter a character to delete the message: \n");
            // scanf("%c",&cl);
            getchar();
            cl = getchar();
            if (cl == 'd')
            {
                // send DELE command
                char dele[100];
                sprintf(dele, "DELE %d\r\n", choice);
                // send to server
                send(client_socket, dele, strlen(dele), 0);
                // recieve the message
                rec_msg = receive_message(client_socket);
                printf("%s\n", rec_msg);
                // error check
                if (strncmp(rec_msg, "-ERR", 4) == 0)
                {
                    printf("Error in DELE command\n");
                    break;
                }
                printf("Message deleted\n");
            }
            else
            {
                printf("Enter again\n");
                continue;
            }

            // send DELE command
            char quit[100];
            sprintf(quit, "QUIT\r\n");
            // send to server
            send(client_socket, quit, strlen(quit), 0);
            // recieve the message
            rec_msg = receive_message(client_socket);
            printf("Recieved Message: %s\n", rec_msg);
            // error check
            if (strncmp(rec_msg, "-ERR", 4) == 0)
            {
                printf("Error in QUIT command\n");
                break;
            }

            // }
        }
        // Close the client socket
        close(client_socket);
    }
    return 0;
}
