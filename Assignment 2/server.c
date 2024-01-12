#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>

int main()
{
    int sockfd,newsockfd;
    struct sockaddr_in cli_addr, serv_addr;

    // cannot read more than 1000 bytes at a time
    char buffer[101];

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
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
					sizeof(serv_addr)) < 0) {
		perror("Binding failed\n");
		exit(0);
	}

    // listening limit
    listen(sockfd, 10);


    // start accepting
    for(;;)
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


        // get the file data
    }
}