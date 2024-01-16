#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

int main()
{
    int sockfd,newsockfd;
    struct sockaddr_in cli_addr, serv_addr;

    // cannot read more than 100 bytes at a time
    char buffer[100] = {'\0'};

    // creating socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation failed\n");
        exit(0);
    }

    // setting params
    serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(6969);

    // binding
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("Binding failed\n");
		exit(0);
	}

    // listening limit
    listen(sockfd, 10);


    // start accepting
    while(1)
    {
        int cli_len = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr*) &cli_addr, &(cli_len));

        if (newsockfd < 0)
        {
            perror("Accepting failed\n");
            exit(0);
        }

        // now start communicating

        // receive the value of key
        int k;
        // it is a string, so we need to convert it to int
        recv(newsockfd, buffer, sizeof(buffer), 0);
        k = atoi(buffer);

        printf("Key received: %d\n", k);

        // get the filename from client
        char filename[100];
        recv(newsockfd, filename, sizeof(buffer), 0);
        memset(buffer, '\0', sizeof(buffer));

        printf("Filename received: %s\n", filename);

        // now create a new file with name <IP.port of client>.txt
        char tempFilename[256];
        sprintf(tempFilename, "%s.%d.txt", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
        int tempFile = open(tempFilename, O_CREAT | O_WRONLY | O_TRUNC, 0666);

        printf("Temporary file created: %s\n", tempFilename);

        // receive and write file data to the temporary file
        int br = 1;
        while (1)
        {
            br = recv(newsockfd, buffer, sizeof(buffer), 0);
            // printf("br: %d\n", br);
            if (br < 50 && br > 0)
            {
                write(tempFile, buffer, br);
                break;
            }
            else if (br == 0 || br == -1) break;
            else write(tempFile, buffer, br);
        }

        printf("File data received\n");

        close(tempFile);

        // encrypt the file
        // encrypted file will be named <IP.port of client>.txt.enc
        char encFilename[256];
        sprintf(encFilename, "%s.%d.txt.enc", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
        int encFile = open(encFilename, O_CREAT | O_WRONLY | O_TRUNC, 0666);

        printf("Encrypted file created: %s\n", encFilename);

        // write encrypted data to the new file
        int fd = open(tempFilename, O_RDONLY);
        while ((br = read(fd, buffer, sizeof(buffer))) > 0)
        {
            for (int i = 0; i < br; i++)
            {
                // caesar cipher
                // lowercase
                if (buffer[i] >= 'a' && buffer[i] <= 'z')
                {
                    buffer[i] = (buffer[i] - 'a' + k) % 26 + 'a';
                }

                // uppercase
                else if (buffer[i] >= 'A' && buffer[i] <= 'Z')
                {
                    buffer[i] = (buffer[i] - 'A' + k) % 26 + 'A';
                }

                // else do nothing
            }
            write(encFile, buffer, br);
        }

        close(fd);

        printf("File encrypted\n");

        // now send the encrypted file back to the client
        int encFd = open(encFilename, O_RDONLY);
        while ((br = read(encFd, buffer, sizeof(buffer))) > 0)
        {
            send(newsockfd, buffer, br, 0);
        }

        close(encFd);

        printf("Encrypted file sent\n");

        // close the connection
        close(newsockfd);
    }
}