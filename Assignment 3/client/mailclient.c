#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>

void set_nonblocking(int sockfd)
{
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1)
    {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }

    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }
}

char buf[2048];


int main()
{
    char server_ip[100];
    char smtp_port[100];
    char pop3_port[100];

    printf("Enter the server ip: ");
    scanf("%s", server_ip);

    printf("Enter the smtp port: ");
    scanf("%s", smtp_port);

    // printf("Enter the pop3 port: ");
    // scanf("%s", pop3_port);

    // char username[100];
    // char password[100];

    // printf("Enter the username: ");
    // scanf("%s", username);

    // printf("Enter the password: ");
    // scanf("%s", password);

    int client_socket;
    struct sockaddr_in server_addr, client_addr;

    // opening a socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Unable to create socket\n");
        exit(0);
    }

    // binding the client (optional in this case)

    // server info
    server_addr.sin_family = AF_INET;
    inet_aton(server_ip, &server_addr.sin_addr);
    server_addr.sin_port = htons(atoi(smtp_port));

    while (1)
    {
        printf("\n1.Manage mails\n2.Send mail\n3.Quit\n");

        int choice;
        scanf("%d", &choice);

        if (choice == 3)
        {
            // send receiver that quit is requested

            // also print the message response from server

            // break any connection is open
            return 0;
        }

        else if (choice == 1)
        {
            
        }

        else if (choice == 2)
        {
            // connect to the server
            if ((connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr))) < 0)
            {
                perror("Unable to connect to server");
                exit(0);
            }

            // if the message is not 220 , then close the connection and exit
            
            memset(buf, 0, sizeof(buf));

            int len ;

            while(1)
            {
                len = recv(client_socket, buf, sizeof(buf), 0);
                if(buf[len-1] == '\n' && buf[len-2] == '\r')break;
            }
            

            if (strncmp(buf, "220", 3) != 0)
            {
                printf("Error in connection\n");
                close(client_socket);
                exit(0);
            }

            printf("%s\n", buf);

            char msg[2048] = "HELO <Domain_name>\r\n";
            send(client_socket, msg, strlen(msg), 0);

            //recv 250
            memset(buf, 0, sizeof(buf));
            while(1)
            {
                len = recv(client_socket, buf, sizeof(buf), 0);
                if(buf[len-1] == '\n' && buf[len-2] == '\r')break;
            }

            if (strncmp(buf, "250", 3) != 0)
            {
                printf("Error in connection\n");
                close(client_socket);
                exit(0);
            }

            printf("%s\n", buf);

            // ask user to enter the sender's mail id

            char sender[100];
            printf("Enter the sender's mail id: ");
            scanf("%s", sender);

            // send MAIL FROM
            memset(msg, 0, sizeof(msg));
            strcpy(msg, "MAIL FROM: ");
            strcat(msg, sender);
            strcat(msg, "\r\n");

            send(client_socket, msg, strlen(msg), 0);

            // recv 250
            memset(buf, 0, sizeof(buf));
            while(1)
            {
                len = recv(client_socket, buf, sizeof(buf), 0);
                if(buf[len-1] == '\n' && buf[len-2] == '\r')break;
            }

            if (strncmp(buf, "250", 3) != 0)
            {
                printf("Error in connection\n");
                close(client_socket);
                exit(0);
            }

            printf("%s\n", buf);

            // ask user to enter the receiver's mail id

            char receiver[100];
            printf("Enter the receiver's mail id: ");
            scanf("%s", receiver);

            // send RCPT TO
            memset(msg, 0, sizeof(msg));
            strcpy(msg, "RCPT TO: ");
            strcat(msg, receiver);
            strcat(msg, "\r\n");

            send(client_socket, msg, strlen(msg), 0);

            // recv 250
            memset(buf, 0, sizeof(buf));
            while(1)
            {
                len = recv(client_socket, buf, sizeof(buf), 0);
                if(buf[len-1] == '\n' && buf[len-2] == '\r')break;
            }

            if (strncmp(buf, "250", 3) != 0)
            {
                printf("Error in connection\n");
                close(client_socket);
                exit(0);
            }

            printf("%s\n", buf);

            // send DATA
            memset(msg, 0, sizeof(msg));
            strcpy(msg, "DATA\r\n");

            send(client_socket, msg, strlen(msg), 0);

            // recv 354
            memset(buf, 0, sizeof(buf));
            while(1)
            {
                len = recv(client_socket, buf, sizeof(buf), 0);
                if(buf[len-1] == '\n' && buf[len-2] == '\r')break;
            }

            if (strncmp(buf, "354", 3) != 0)
            {
                printf("Error in connection\n");
                close(client_socket);
                exit(0);
            }

            printf("%s\n", buf);

            printf("Enter the message:(strictly adhere to the mail format) \n");


            // taking msg input from user line by line and sending
            while(1)
            {
                memset(msg, 0, sizeof(msg));
                scanf("\n%[^\n]s", msg);
                strcat(msg, "\r\n");
                if(strncmp(msg, ".\r\n", 3)==0)
                {
                    char * msg2 = "\r\n.\r\n";
                    send(client_socket, msg2, strlen(msg2), 0);
                    break;

                }
                send(client_socket, msg, strlen(msg), 0);
            }

            // recv 250
            memset(buf, 0, sizeof(buf));
            while(1)
            {
                len = recv(client_socket, buf, sizeof(buf), 0);
                if(buf[len-1] == '\n' && buf[len-2] == '\r')break;
            }

            if (strncmp(buf, "250", 3) != 0)
            {
                printf("Error in connection\n");
                close(client_socket);
                exit(0);
            }

            printf("%s\n", buf);


            close(client_socket);
        }
    }
}