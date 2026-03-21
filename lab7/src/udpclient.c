#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc < 4) {
        printf("Usage: %s <IP> <PORT> <BUFSIZE>\n", argv[0]);
        exit(1);
    }

    int sockfd, n;
    int port = atoi(argv[2]);
    int bufsize = atoi(argv[3]);
    char *sendline = malloc(bufsize);
    char *recvline = malloc(bufsize + 1);
    struct sockaddr_in servaddr;

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    printf("Input message:\n");
    while (fgets(sendline, bufsize, stdin) != NULL) {
        sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
        if ((n = recvfrom(sockfd, recvline, bufsize, 0, NULL, NULL)) < 0) {
            perror("recvfrom");
            exit(1);
        }
        recvline[n] = 0;
        printf("ECHO FROM SERVER: %s", recvline);
    }

    free(sendline); free(recvline);
    close(sockfd);
}