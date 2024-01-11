#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_FILENAME_LENGTH 100
#define MAX_WORD_LENGTH 100
#define PORT 8888

int main() {
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len = sizeof(cliaddr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }

    char filename[MAX_FILENAME_LENGTH];
    int n = recvfrom(sockfd, filename, MAX_FILENAME_LENGTH, 0, (struct sockaddr *)&cliaddr, &len);
    if (n == -1) {
        perror("Filename reception failed");
        exit(EXIT_FAILURE);
    }
    filename[n] = '\0';

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        char not_found_msg[MAX_FILENAME_LENGTH + 20];
        sprintf(not_found_msg, "NOTFOUND %s", filename);
        sendto(sockfd, not_found_msg, strlen(not_found_msg), 0, (const struct sockaddr *)&cliaddr, len);
        perror("File not found");
        exit(EXIT_FAILURE);
    }

    char buffer[MAX_WORD_LENGTH], response[MAX_WORD_LENGTH];
    fgets(buffer, MAX_WORD_LENGTH, file); // Read HELLO
    sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr *)&cliaddr, len);

    int word_count = 1;
    while (fgets(buffer, MAX_WORD_LENGTH, file) != NULL) {
        n = recvfrom(sockfd, response, MAX_WORD_LENGTH, 0, (struct sockaddr *)&cliaddr, &len);
        // printf("%d\n",n);
        if (n == -1) {
        perror("Filename reception failed");
        exit(EXIT_FAILURE);
        }
        response[n] = '\0';
        if (strncmp(response, "WORD", 4) == 0)
        {
            sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr *)&cliaddr, len);
        }

        ++word_count;
    }
    fclose(file);

    char end_msg[MAX_WORD_LENGTH] = "END";
    sendto(sockfd, end_msg, strlen(end_msg), 0, (const struct sockaddr *)&cliaddr, len);

    printf("File sent.\n");

    close(sockfd);
    return 0;
}
