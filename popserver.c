#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#define POP_PORT 117
#define MAX_BUFFER_SIZE 1024
// define a structure to hold the messages of the user
typedef struct
{
    int num;
    char from[100];
    char to[100];
    char received_at[100];
    char subject[100];
    char body[1000];
} Email;

void parseMailbox(const char *filename, Email emails[], int *num_emails)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        perror("[-] Error in opening file");
        exit(1);
    }
    Email current_email;
    char line[1000];
    int emailindex = 0;
    strcpy(current_email.body, "");
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        if (strncmp(line, "From: ", 6) == 0)
        {
            sscanf(line, "From: %[^\n]", current_email.from);
            // add CRLF to the end of the line
            // strcat(current_email.from, "\r\n");
        }
        else if (strncmp(line, "To: ", 4) == 0)
        {
            sscanf(line, "To: %[^\n]", current_email.to);
            // strcat(current_email.to, "\r\n");
        }
        else if (strncmp(line, "Recieved at: ", 13) == 0)
        {
            sscanf(line, "Recieved at: %[^\n]", current_email.received_at);
            // strcat(current_email.received_at, "\r\n");
        }
        else if (strncmp(line, "Subject: ", 9) == 0)
        {
            sscanf(line, "Subject: %[^\n]", current_email.subject);
            // strcat(current_email.subject, "\r\n");
        }
        else if (strncmp(line, ".\n", 2) == 0)
        {
            // end of email reached add to array
            strcat(current_email.body, line);
            strcpy(emails[emailindex].from, current_email.from);
            strcpy(emails[emailindex].to, current_email.to);
            strcpy(emails[emailindex].received_at, current_email.received_at);
            strcpy(emails[emailindex].subject, current_email.subject);
            strcpy(emails[emailindex].body, current_email.body);
            current_email.num = emailindex + 1;
            emails[emailindex].num = current_email.num;
            emailindex++;
            // set current_email.body to empty
            strcpy(current_email.body, "");
        }
        else
        {
            strcat(current_email.body, line);
        }
    }
    *num_emails = emailindex;

    fclose(fp);
}

char *receive_message(int client_socket)
{
    char buffer[1024];                // Adjust buffer size as needed
    char *msg = (char *)malloc(1024); // Allocate memory for message buffer
    memset(msg, '\0', 1024);
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

    printf("Message Recieved=> %s\n", msg);

    return msg;
}
void handle_client(int client_socket)
{

    // Send welcome message
    send(client_socket, "+OK POP3 server ready\r\n", 24, 0);

    // Receive username
    char *rec_msg = receive_message(client_socket);
    printf("Username Recieved : %s\n", rec_msg);
    // msg : USER : username
    // check if username is valid
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
    while (fgets(line, sizeof(line), fp))
    {
        char *token = strtok(line, " ");
        printf("Token is: %s\n", token);
        if (strcmp(token, username) == 0)
        {
            found = 1;
            break;
        }
    }
    fclose(fp);
    if (found)
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

    rec_msg = receive_message(client_socket);
    printf("password is => %s\n", rec_msg);
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
    while (fgets(line, sizeof(line), fp))
    {
        char *token = strtok(line, " ");
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
    if (found)
    {
        send(client_socket, "+OK password correct\r\n", 23, 0);
    }
    else
    {
        send(client_socket, "-ERR password incorrect\r\n", 25, 0);
        close(client_socket);
        exit(1);
    }
    // open username/mymailbox
    strcat(username, "/mymailbox");
    Email emails[100];
    int num_emails = 0;

    // recieve STAT
    while (1)
    {
        parseMailbox(username, emails, &num_emails);
        rec_msg = receive_message(client_socket);
        printf("STAT : %s\n", rec_msg);
        if (strcmp(rec_msg, "STAT\r\n") == 0)
        {
            char msg[100];
            sprintf(msg, "+OK %d %d\r\n", num_emails, 1000);
            send(client_socket, msg, strlen(msg), 0);
        }
        else
        {
            send(client_socket, "-ERR invalid command\r\n", 23, 0);
            close(client_socket);
            exit(1);
        }

        // recieve LIST
        rec_msg = receive_message(client_socket);
        printf("LIST : %s\n", rec_msg);
        if (strcmp(rec_msg, "LIST\r\n") == 0)
        {
            char msg[100];
            sprintf(msg, "+OK %d messages .\r\n", num_emails);
            send(client_socket, msg, strlen(msg), 0);
            int cnt = 0;
            for (int i = 0; i < num_emails; i++)
            {
                
                cnt++;
                emails[i].num = cnt;
                sprintf(msg, "%d %s %s %s \n", emails[i].num, emails[i].from, emails[i].received_at, emails[i].subject);
                send(client_socket, msg, strlen(msg), 0);
            }
            // num_emails=cnt;
            send(client_socket, ".\r\n", 3, 0);
        }
        else
        {
            send(client_socket, "-ERR invalid command\r\n", 23, 0);
            close(client_socket);
            exit(1);
        }

        // recieve RETR
        rec_msg = receive_message(client_socket);
        printf("RETR : %s\n", rec_msg);
        int email_num;
        sscanf(rec_msg, "RETR %d\r\n", &email_num);
        printf("Email number: %d\n", email_num);
        if (email_num > num_emails)
        {
            send(client_socket, "-ERR invalid email number\r\n", 27, 0);
            close(client_socket);
            exit(1);
        }
        else
        {
            // send +OK message follows
            // send(client_socket, "+OK message follows\r\n", 22, 0);
            char *msg;
            int msg_length = strlen(emails[email_num - 1].from) + strlen(emails[email_num - 1].to) +
                             strlen(emails[email_num - 1].received_at) + strlen(emails[email_num - 1].subject) +
                             strlen(emails[email_num - 1].body) + 5; // 5 extra characters for delimiters and termination

            msg = (char *)malloc(msg_length * sizeof(char));
            if (msg == NULL)
            {
                perror("Failed to allocate memory for message");
                exit(EXIT_FAILURE);
            }

            // Construct the message
            strcpy(msg, emails[email_num - 1].from);
            strcat(msg, "\n");
            strcat(msg, emails[email_num - 1].to);
            strcat(msg, "\n");
            strcat(msg, emails[email_num - 1].received_at);
            strcat(msg, "\n");
            strcat(msg, emails[email_num - 1].subject);
            strcat(msg, "\n");
            strcat(msg, emails[email_num - 1].body);

            // Remove newline character from the end of the message, if present
            if (strlen(msg) > 0 && msg[strlen(msg) - 1] == '\n')
            {
                msg[strlen(msg) - 1] = '\0';
            }

            // Send the message
            // send(client_socket, "+OK message follows\r\n", 22, 0);
            strcat(msg, "\r\n");
            send(client_socket, msg, strlen(msg), 0);

            // Free dynamically allocated memory
            free(msg);
        }
        printf("Message sent \n");

        //    recieve DELETE
        int old = num_emails;
        rec_msg = receive_message(client_socket);
        printf("DELE : %s\n", rec_msg);
        sscanf(rec_msg, "DELE %d\r\n", &email_num);
        if (email_num == 0)
        {
            send(client_socket, "-ERR invalid email number\r\n", 27, 0);
            // break;
            // close(client_socket);
        }
        if (email_num > num_emails && email_num!=0)
        {
            send(client_socket, "-ERR invalid email number\r\n", 27, 0);
            close(client_socket);
            exit(1);
        }
        else if(email_num!=0)
        {
            emails[email_num - 1].num = 0;
            // num_emails--;
            send(client_socket, "+OK message deleted\r\n", 23, 0);
        }

        // // recieve QUIT
        rec_msg = receive_message(client_socket);
        printf("QUIT : %s\n", rec_msg);

        printf("Number of emails: %d\n", num_emails);
        if (strcmp(rec_msg, "QUIT\r\n") == 0)
        {
            fp = fopen(username, "w");
            for (int i = 0; i < num_emails; i++)
            {
                if (emails[i].num != 0)
                {
                    printf("Email %d\n", emails[i].num);
                    // write the email to the file after deleting the entire file

                    if (fp == NULL)
                    {
                        perror("[-] Error in opening file");
                        exit(1);
                    }

                    fprintf(fp, "From: %s\n", emails[i].from);
                    fprintf(fp, "To: %s\n", emails[i].to);
                    fprintf(fp, "Recieved at: %s\n", emails[i].received_at);
                    fprintf(fp, "Subject: %s\n", emails[i].subject);
                    fprintf(fp, "%s", emails[i].body);
                }
            }
            fclose(fp);
            send(client_socket, "+OK POP3 server signing off\r\n", 30, 0);
        }
        else
        {
            send(client_socket, "You Deleted Message\r\n",30,0);
        }
    }

    // Close the client socket
}

int main()
{
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("Error creating server socket");
        exit(EXIT_FAILURE);
    }

    // Set up server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(POP_PORT);

    // Bind the socket to the specified address and port
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Error binding server socket");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) == -1)
    {
        perror("Error listening for connections");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // concurrent server
    printf("==========POP3 Server Running=============\n");
    while (1)
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
    close(server_socket);

    return 0;
}
