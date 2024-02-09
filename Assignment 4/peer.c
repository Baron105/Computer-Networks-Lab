// Barun Parua
// 21CS10014
// Assignment 4

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

// user_info table
struct user_info_table
{
    char name[100];
    char ipaddr[100];
    int port;
} user_info[3];

int main(int argc, char *argv[])
{
    int sockfd, newsockfd; /* Socket descriptors */
    int clilen;
    struct sockaddr_in cli_addr, serv_addr;

    strcpy(user_info[0].name, "user_1");
    strcpy(user_info[0].ipaddr, "127.0.0.1");
    user_info[0].port = 50000;

    strcpy(user_info[1].name, "user_2");
    strcpy(user_info[1].ipaddr, "127.0.0.1");
    user_info[1].port = 50001;

    strcpy(user_info[2].name, "user_3");
    strcpy(user_info[2].ipaddr, "127.0.0.1");
    user_info[2].port = 50002;

    // get port number from command line
    if (argc < 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        exit(0);
    }
    int port = atoi(argv[1]);

    // create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Cannot create socket\n");
        exit(0);
    }

    // set server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    // bind server address to socket
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Unable to bind local address\n");
        exit(0);
    }

    // three file descriptors for clients
    int client_fds[3] = {0};
    int connection[3] = {0};

    // 1 is 50000, 2 is 50001, 3 is 50002
    // set own connection array to -1
    if (port == 50000)
    {
        connection[0] = -1;
        client_fds[0] = sockfd;
    }
    else if (port == 50001)
    {
        connection[1] = -1;
        client_fds[1] = sockfd;
    }
    else if (port == 50002)
    {
        connection[2] = -1;
        client_fds[2] = sockfd;
    }

    // listen for incoming connections
    listen(sockfd, 3);

    printf("Welcome to Chat Service\nMessage Format: (receiver/sender/message)\n");

    // concurrent server
    while (1)
    {
        // use select to handle multiple clients and stdin at the same time
        struct timeval tv;
        fd_set readfds;

        tv.tv_sec = 300;
        // 300 seconds

        // clear the set and add the socket file descriptor and stdin to the set
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        FD_SET(0, &readfds);

        // add client file descriptors to the set
        for (int i = 0; i < 2; i++)
        {
            if (client_fds[i] > 0)
            {
                FD_SET(client_fds[i], &readfds);
            }
        }

        // wait for activity on the socket, stdin, or client file descriptors
        int activity = select(FD_SETSIZE, &readfds, NULL, NULL, &tv);

        // if there is activity on the socket
        if (FD_ISSET(sockfd, &readfds))
        {
            // accept the incoming connection
            clilen = sizeof(cli_addr);
            newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

            // add the newsockfd to readfds
            FD_SET(newsockfd, &readfds);

            // receive the message from the client
            char msg[300], extracted_msg[300];
            recv(newsockfd, msg, 300, 0);

            // message of format receiver/sender/message

            // look at the message to determine the user
            char sender[10];
            int i = 0, k = 0;

            while (msg[i] != '/')
            {
                i++;
            }
            i++;

            while (msg[i] != '/')
            {
                sender[k++] = msg[i];
                i++;
            }
            sender[i++] = '\0';

            // add the newsockfd to the client_fds by checking sender
            if (strncmp(sender, "user_1", 6) == 0)
            {
                client_fds[0] = newsockfd;
                connection[0] = 1;
            }
            else if (strncmp(sender, "user_2", 6) == 0)
            {
                client_fds[1] = newsockfd;
                connection[1] = 1;
            }
            else if (strncmp(sender, "user_3", 6) == 0)
            {
                client_fds[2] = newsockfd;
                connection[2] = 1;
            }

            // extract the message
            int j = 0;
            while (msg[i] != '\0')
            {
                extracted_msg[j] = msg[i];
                i++;
                j++;
            }
            extracted_msg[j] = '\0';

            printf("Message from %s: %s\n", sender, extracted_msg);
        }

        // if there is activity on stdin
        if (FD_ISSET(0, &readfds))
        {
            // read from stdin
            char msg[300];
            fgets(msg, 300, stdin);

            // message format is receiver/sender/message
            char receiver[10], sender[10], message[300];
            int i = 0;

            // extract the receiver
            while (msg[i] != '/')
            {
                receiver[i] = msg[i];
                i++;
            }

            receiver[i++] = '\0';

            // get the port number of the receiver
            int receiver_port = 0;
            for (int i = 0; i < 3; i++)
            {
                if (strncmp(receiver, user_info[i].name, strlen(user_info[i].name)) == 0)
                {
                    receiver_port = user_info[i].port;
                    break;
                }
            }

            // now send the message to the receiver
            // create a new socket
            int newclisockfd;
            struct sockaddr_in newcli_addr;

            // create socket
            if ((newclisockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            {
                perror("Cannot create socket\n");
                exit(0);
            }

            // set server address
            newcli_addr.sin_family = AF_INET;
            newcli_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
            newcli_addr.sin_port = htons(receiver_port);

            // connect to the receiver
            if (connect(newclisockfd, (struct sockaddr *)&newcli_addr, sizeof(newcli_addr)) < 0)
            {
                perror("Unable to connect to server\n");
                exit(0);
            }

            // send the message
            send(newclisockfd, msg, strlen(msg) + 1, 0);

            // close the socket
            close(newclisockfd);

            // printf("Message %s sent to %s\n", msg, receiver);
        }

        // timeout
        if (activity == 0)
        {
            printf("Timeout\nClosing client connections\n");
            
            // close all client connections
            for (int i = 0; i < 3; i++)
            {
                if (client_fds[i] > 0 && client_fds[i] != sockfd)
                {
                    close(client_fds[i]);
                    client_fds[i] = 0;
                    connection[i] = 0;
                }
            }
        }

        if (activity < 0)
        {
            perror("select error\n");
        }
    }
}