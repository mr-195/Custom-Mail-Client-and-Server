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
#define MAX_BUFFER_SIZE 1024

int isvalidmail(char from_line[], char to_line[], char subject_line[])
{
     if (strncmp(from_line, "From:", 5) != 0 || strncmp(to_line, "To:", 3) != 0 || strncmp(subject_line, "Subject:", 8) != 0){
        printf("Incorrect Format \n");
        return 0;
    }
    // Check the presence of '@' in the "From" line
    char *from_at_symbol = strchr(from_line, '@');
    if (from_at_symbol == NULL) {
        printf("Incorrect Format \n");
        return 0;
    }

    // Check the presence of '@' in the "To" line
    char *to_at_symbol = strchr(to_line, '@');
    if (to_at_symbol == NULL) {
        printf("Incorrect Format \n");
        return 0;
    }

    return 1;
}
int main(int argc, char *argv[])
{
    // the program takes 3 command line arguments
    // server IP,smtp port,pop3port
    if (argc != 4)
    {
        printf("Usage: %s <server IP> <smtp port> <pop3 port>\n", argv[0]);
        exit(1);
    }
    int sockfd;
    struct sockaddr_in serv_addr;

    /* Opening a socket is exactly similar to the server process */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Unable to create socket\n");
        exit(0);
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    inet_aton(argv[1], &serv_addr.sin_addr);
    serv_addr.sin_port = htons(atoi(argv[2]));

    // connect with server

    char username[100];
    char password[100];
    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);

    while (1)
    {
        printf("Enter option:\n");
        printf("1. Manage Mail\n");
        printf("2. Send Mail\n");
        printf("3. Quit\n");
        int option;
        scanf("%d", &option);
        if (option == 1)
        {
        }
        else if (option == 2)
        {
            // connect to server
            connect(sockfd, (struct sockaddr *)&serv_addr,
                    sizeof(serv_addr));

            char from_line[100];
            char to_line[100];
            char subject_line[100];
            char message_lines[4][100]; // Assuming a maximum of 4 lines for the message
            // take input from line
            fgets(from_line, sizeof(from_line), stdin);
            // take input to line
            fgets(to_line, sizeof(to_line), stdin);
            // take input subject line
            fgets(subject_line, sizeof(subject_line), stdin);
            // take input message
            for (int i = 0; i < 4; ++i)
            {
                fgets(message_lines[i], sizeof(message_lines[i]), stdin);
            }
            // check for format of mail
            int valid = isvalidmail(from_line, to_line, subject_line);

            if(valid == 0)
            {
                printf("Invalid mail format\n");
                continue;
            }
            // format is correct 
            // send HELO domain name
            char helo[100];
            strcpy(helo, "HELO ");
            strcat(helo, SMTP_SERVER);
            strcat(helo, "\r\n");
            send(sockfd, helo, strlen(helo), 0);
            // receive acknowkledgement from server "250 OK domain name"
            char buffer[MAX_BUFFER_SIZE];
            char msg [MAX_BUFFER_SIZE];
            while(1)
            {
                memset(buffer, 0, sizeof(buffer));
                int n=recv(sockfd, buffer, sizeof(buffer), 0);
                // break when EOF is reached
                if(n==0)
                    break;
                if(n<0)
                {
                    perror("Error in receiving\n");
                    exit(1);
                }
                if( n>1 && buffer[n-1]=='\n')
                {
                    buffer[n-1]='\0';
                    strcat(msg, buffer);
                    break;
                }
                strcat(msg, buffer);
            }
            printf("%s\n", msg);
            // check for 250 OK
            if(strncmp(msg, "250 OK",6) != 0)
            {
                printf("Error in HELO\n");
                continue;
            }


        }
        else if (option == 3)
        {
            // quit
            break;
        }
        else
        {
            printf("Invalid option\n");
        }
    }
    close(sockfd);
    return 0;
}

