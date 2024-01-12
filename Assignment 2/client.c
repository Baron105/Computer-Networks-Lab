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
    struct sockaddr_in serv_addr;

    // cannot read more than 1000 bytes at a time
    char buffer[101]={'\0'};

    // creating socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation failed\n");
        exit(0);
    }

    // setting params
    serv_addr.sin_family = AF_INET;
	inet_aton("127.0.0.1", &serv_addr.sin_addr);
	serv_addr.sin_port = htons(6969);

    char filename[101]; int fd;
    while(1)
    {
        printf("Enter filename: ");
        scanf("%s", filename);

        // open the file if it exists
        fd = open("foo.txt", O_RDONLY);
        if (fd) break;

        // else print error and ask again
        printf("File doesnt exist, please try again\n");
    }

    // filename accessed
    // ask for key
    int k;
    printf("Enter the key: ");
    scanf("%d", &k);

    // connection establishing
    if ((connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr))) < 0)
    {
		perror("Connection failed\n");
		exit(0);
	}

    // send key


    // read file
    int rd = 1;
    while (rd)
    {
        // read 100 bytes into buffer
        rd = read(fd, buffer, 100);

        // transfer buffer data to server via send
        send(sockfd, buffer, 100, 0);

        // exit if rd == 0 ie EOF reached
    }
}