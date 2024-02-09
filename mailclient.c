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

#define SMTP_SERVER "iitkgp.edu"
#define SMTP_PORT 25
#define MAX_BUFFER_SIZE 4000
#define MAX_SIZE 100

char *receive_message(int sockfd_pop3)
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
        int n = recv(sockfd_pop3, buffer, sizeof(buffer), 0);
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
int isvalidmail(char from_line[], char to_line[], char subject_line[])
{
    // printf("%s\n", from_line);
    // printf("%s\n", to_line);
    // printf("%s\n", subject_line);
    if (strncmp(from_line, "From:", 5) != 0 || strncmp(to_line, "To:", 3) != 0 || strncmp(subject_line, "Subject:", 8) != 0)
    {
        printf("[-] Incorrect Format \n");
        return 0;
    }

    // check X@Y
    int cnt = 0;
    // count no of @
    for (int i = 0; i < strlen(from_line); i++)
    {
        if (from_line[i] == '@')
            cnt++;
    }
    if (cnt != 1)
    {
        printf("[-] Incorrect Format \n");
        return 0;
    }
    // check first letter of X@Y
    if (from_line[6] == '@')
    {
        printf("[-] Incorrect Format \n");
        return 0;
    }
    cnt = 0;
    for (int i = 0; i < strlen(to_line); i++)
    {
        if (to_line[i] == '@')
            cnt++;
    }
    if (cnt != 1)
    {
        printf("[-] Incorrect Format \n");
        return 0;
    }
    if (to_line[4] == '@')
    {
        printf("[-] Incorrect Format \n");
        return 0;
    }

    return 1;
}
void clearInputBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
    // fflush(stdin);
    // fflush(stdout);
}

int main(int argc, char *argv[])
{
    // the program takes 3 command line arguments
    // server IP,smtp port,pop3port
    if (argc != 4)
    {
        printf("[-] Usage: %s <server IP> <smtp port> <pop3 port>\n", argv[0]);
        exit(1);
    }

    while (1)
    {

        //  create a socket for pop3 server as well
        char username[100];
        char password[100];
        printf("Enter username: ");
        scanf("%s", username);
        printf("Enter password: ");
        scanf("%s", password);

        printf("Enter option:\n");
        printf("1. Manage Mail\n");
        printf("2. Send Mail\n");
        printf("3. Quit\n");
        int option;
        scanf("%d", &option);

        if (option == 1)
        {
            // to be implemented later
            // connect to pop3 server
            int sockfd_pop3;
            struct sockaddr_in serv_addr_pop3;
            char buffer[MAX_BUFFER_SIZE];
            if ((sockfd_pop3 = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            {
                perror("[-] Unable to create socket\n");
                exit(0);
            }
            memset(&serv_addr_pop3, 0, sizeof(serv_addr_pop3));
            serv_addr_pop3.sin_family = AF_INET;
            inet_aton(argv[1], &serv_addr_pop3.sin_addr);
            serv_addr_pop3.sin_port = htons(atoi(argv[3]));
            if (connect(sockfd_pop3, (struct sockaddr *)&serv_addr_pop3,
                        sizeof(serv_addr_pop3)) < 0)
            {
                perror("[-] Error in connecting to server");
                exit(1);
            };
            // char username[100];
            // char password[100];
            // printf("Enter username: ");
            // scanf("%s", username);
            // printf("Enter password: ");
            // scanf("%s", password);
            // Receive welcome message
            recv(sockfd_pop3, buffer, sizeof(buffer), 0);
            printf("Server: %s", buffer);
            // Send username
            // send USER : username
            char msg[100];
            sprintf(msg, "USER : %s\r\n", username);
            printf("Sending Message: %s\n", msg);
            send(sockfd_pop3, msg, strlen(msg), 0);
            // Receive response
            char *rec_msg = receive_message(sockfd_pop3);
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
            send(sockfd_pop3, pass, strlen(pass), 0);
            // Receive response
            rec_msg = receive_message(sockfd_pop3);
            printf("%s\n", rec_msg);
            // check if password is valid
            if (strncmp(rec_msg, "-ERR", 4) == 0)
            {
                printf("Password is incorrect\n");
                continue;
            }
            // Transaction State
            while (1) // command loop
            {
                // send STAT command
                char stat[100];
                sprintf(stat, "STAT\r\n");
                // send to server
                send(sockfd_pop3, stat, strlen(stat), 0);
                // recieve +OK num
                char *rec_msg = receive_message(sockfd_pop3);
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
                send(sockfd_pop3, list, strlen(list), 0);
                // recieve the list of messages
                // recieve +OK number of messages
                rec_msg = receive_message(sockfd_pop3);
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
                    char *rec_msg = receive_message(sockfd_pop3);
                    printf("%s\n", rec_msg);
                    if (strstr(rec_msg, ".\r\n") != NULL)
                    {
                        break;
                    }
                }
                printf("Enter the message number to read: ");
                int choice;
                scanf("%d", &choice);
                if (choice == -1)
                {
                    // go back to main menu
                    break;
                }
                // send RETR command
                char retr[100];
                sprintf(retr, "RETR %d\r\n", choice);
                // send to server
                send(sockfd_pop3, retr, strlen(retr), 0);
                // recieve the message
                rec_msg = receive_message(sockfd_pop3); // +OK message follows
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
                    // rec_msg = receive_message(sockfd_pop3);
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
                    send(sockfd_pop3, dele, strlen(dele), 0);
                    // recieve the message
                    rec_msg = receive_message(sockfd_pop3);
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
                send(sockfd_pop3, quit, strlen(quit), 0);
                // recieve the message
                rec_msg = receive_message(sockfd_pop3);
                printf("Recieved Message: %s\n", rec_msg);
                // error check
                if (strncmp(rec_msg, "-ERR", 4) == 0)
                {
                    printf("Error in QUIT command\n");
                    break;
                }
            }

            // Close the client socket
            close(sockfd_pop3);
        }
        else if (option == 2)
        {
            // connect to server
            int sockfd;
            struct sockaddr_in serv_addr;

            /* Opening a socket is exactly similar to the server process */
            if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            {
                perror("[-] Unable to create socket\n");
                exit(0);
            }
            memset(&serv_addr, 0, sizeof(serv_addr));
            serv_addr.sin_family = AF_INET;
            inet_aton(argv[1], &serv_addr.sin_addr);
            serv_addr.sin_port = htons(atoi(argv[2]));

            if (connect(sockfd, (struct sockaddr *)&serv_addr,
                        sizeof(serv_addr)) < 0)
            {
                perror("[-] Error in connecting to server");
                exit(1);
            };
            clearInputBuffer();
            char from_line[100];
            char to_line[100];
            char subject_line[100];
            char message_lines[100][100]; // Assuming a maximum of 4 lines for the message

            // take input from line
            fgets(from_line, sizeof(from_line), stdin);

            fgets(to_line, sizeof(to_line), stdin);

            fgets(subject_line, sizeof(subject_line), stdin);

            for (int i = 0; i < 50; ++i)
            {
                fgets(message_lines[i], sizeof(message_lines[i]), stdin);
                // printf("MESSAGE LINE :: %s\n", message_lines[i]);
                if (strcmp(message_lines[i], ".\n") == 0)
                    break;
            }
            // printf("INPUT TAKEN\n");
            // check for format of mail
            int valid = isvalidmail(from_line, to_line, subject_line);

            if (valid == 0)
            {
                printf("[-] Invalid mail format\n");
                continue;
            }

            // printf("MAIL FORMAT IS VALID\n");
            // format is correct
            // recieve service ready message
            char buffer[MAX_BUFFER_SIZE];
            char msg[10 * MAX_BUFFER_SIZE];
            memset(msg, '\0', sizeof(msg));

            while (1)
            {
                memset(buffer, '\0', sizeof(buffer));
                int n = recv(sockfd, buffer, sizeof(buffer) - 1, 0);

                if (n == 0)
                {
                    // printf("BREAKING HERE!\n");
                    break; // Connection closed by the remote peer
                }
                else if (n < 0)
                {
                    perror("[-] Error in receiving");
                    exit(1);
                }

                // printf("Received: %s\n", buffer);

                // Check for the end of a line (CRLF)
                if (strchr(buffer, '\r') && strchr(buffer, '\n'))
                {
                    // printf("Break condition met\n");
                    strcat(msg, buffer);
                    break;
                }

                strcat(msg, buffer);
            }

            printf("%s\n", msg);
            // check for 220 <iitkgp.edu> Service ready
            if (strcmp(msg, "220 iitkgp.edu Service ready\r\n") != 0)
            {
                printf("[-] Error in connecting to server\n");
                continue;
            }

            // send HELO domain name
            char helo[100];
            strcpy(helo, "HELO ");
            strcat(helo, SMTP_SERVER);
            strcat(helo, "\r\n");
            send(sockfd, helo, strlen(helo), 0);
            // receive acknowkledgement from server "250 OK domain name"
            memset(msg, '\0', sizeof(msg));
            while (1)
            {
                memset(buffer, '\0', sizeof(buffer));
                int n = recv(sockfd, buffer, sizeof(buffer) - 1, 0);

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
                    // printf("Break condition met\n");
                    strcat(msg, buffer);
                    break;
                }

                strcat(msg, buffer);
            }
            printf("%s\n", msg);
            // check for 250 OK
            if (strncmp(msg, "250 OK", 6) != 0)
            {
                printf("[-] Error in HELO\n");
                continue;
            }
            // send MAIL+ from_line
            char mail_from[100];
            strcpy(mail_from, "MAIL ");
            strcat(mail_from, from_line);
            strcat(mail_from, "\r\n");
            send(sockfd, mail_from, strlen(mail_from), 0);
            // receive acknowkledgement from server 250 from_line ... Sender Ok
            memset(buffer, '\0', sizeof(buffer));
            memset(msg, '\0', sizeof(msg));
            while (1)
            {
                memset(buffer, '\0', sizeof(buffer));
                int n = recv(sockfd, buffer, sizeof(buffer), 0);
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
                    // printf("Break condition met\n");
                    strcat(msg, buffer);
                    break;
                }

                strcat(msg, buffer);
            }
            printf("%s\n", msg); //
            // check for 250
            if (strncmp(msg, "250", 3) != 0)
            {
                printf("[-] Error in MAIL\n");
                continue;
            }
            // send RCPT+ to_line
            char rcpt_to[100];
            strcpy(rcpt_to, "RCPT ");
            strcat(rcpt_to, to_line);
            strcat(rcpt_to, "\r\n");
            send(sockfd, rcpt_to, strlen(rcpt_to), 0);
            // receive acknowkledgement from server 250 to_line ... Recipient Ok
            memset(buffer, '\0', sizeof(buffer));
            memset(msg, '\0', sizeof(msg));
            while (1)
            {
                memset(buffer, '\0', sizeof(buffer));
                int n = recv(sockfd, buffer, sizeof(buffer), 0);
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
            // check for 250
            if (strncmp(msg, "250", 3) != 0)
            {
                printf("[-] Error in RCPT\n");
                continue;
            }
            // send DATA
            send(sockfd, "DATA\r\n", strlen("DATA\r\n"), 0);
            // receive acknowkledgement from server 354 Start mail input; end with <CRLF>.<CRLF>
            memset(buffer, '\0', sizeof(buffer));
            memset(msg, '\0', sizeof(msg));
            while (1)
            {
                memset(buffer, '\0', sizeof(buffer));
                int n = recv(sockfd, buffer, sizeof(buffer), 0);
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
                    // printf("Break condition met\n");
                    strcat(msg, buffer);
                    break;
                }

                strcat(msg, buffer);
            }
            printf("%s\n", msg);
            // check for 354
            if (strncmp(msg, "354", 3) != 0)
            {
                printf("[-] Error in DATA\n");
                continue;
            }
            // send from_line
            send(sockfd, from_line, strlen(from_line), 0);
            // send to_line
            send(sockfd, to_line, strlen(to_line), 0);
            // send subject_line
            send(sockfd, subject_line, strlen(subject_line), 0);
            // send message_lines
            for (int i = 0; i < 50; ++i)
            {
                send(sockfd, message_lines[i], strlen(message_lines[i]), 0);
                if (strcmp(message_lines[i], ".\r\n") == 0)
                    break;
            }
            // receive acknowkledgement from server 250 OK Message accepted for delivery
            memset(buffer, '\0', sizeof(buffer));
            memset(msg, '\0', sizeof(msg));
            while (1)
            {
                memset(buffer, '\0', sizeof(buffer));
                int n = recv(sockfd, buffer, sizeof(buffer), 0);
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
                    // printf("Break condition met\n");
                    strcat(msg, buffer);
                    break;
                }

                strcat(msg, buffer);
            }
            printf("%s\n", msg);
            // check for 250
            if (strcmp(msg, "250 OK Message accepted for delivery\r\n") != 0)
            {
                printf("[-] Error in sending mail\n");
                continue;
            }
            else
            {
                printf("[+] Mail sent successfully\n");
            }
            // send QUIT
            send(sockfd, "QUIT\r\n", strlen("QUIT\r\n"), 0);
            // receive acknowledgement 221 iitkgp.edu closing connection
            memset(buffer, '\0', sizeof(buffer));
            memset(msg, '\0', sizeof(msg));
            while (1)
            {
                memset(buffer, '\0', sizeof(buffer));
                int n = recv(sockfd, buffer, sizeof(buffer), 0);
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
                    // printf("Break condition met\n");
                    strcat(msg, buffer);
                    break;
                }

                strcat(msg, buffer);
            }
            printf("%s\n", msg);
            // check for 221
            if (strcmp(msg, "221 iitkgp.edu closing connection\r\n") != 0)
            {
                printf("[-] Error in QUIT\n");
                continue;
            }
            close(sockfd);
        }
        else if (option == 3)
        {
            // quit
            break;
        }
        else
        {
            printf("[-] Invalid option\n");
        }
        // close(sockfd);
    }

    return 0;
}
