#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <PORT> <BUFSIZE>\n", argv[0]);
        exit(1);
    }

    int sockfd, n;
    int port = atoi(argv[1]);
    int bufsize = atoi(argv[2]);
    char *mesg = malloc(bufsize);
    char ipadr[16];
    struct sockaddr_in servaddr, cliaddr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket problem");
        exit(1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind problem");
        exit(1);
    }
    printf("UDP SERVER starts on port %d...\n", port);

    while (1) {
        unsigned int len = sizeof(cliaddr);
        if ((n = recvfrom(sockfd, mesg, bufsize, 0, (struct sockaddr *)&cliaddr, &len)) < 0) {
            perror("recvfrom");
            continue;
        }
        mesg[n] = 0;

        printf("REQUEST %s FROM %s : %d\n", mesg,
               inet_ntop(AF_INET, (void *)&cliaddr.sin_addr.s_addr, ipadr, 16),
               ntohs(cliaddr.sin_port));

        if (sendto(sockfd, mesg, n, 0, (struct sockaddr *)&cliaddr, len) < 0) {
            perror("sendto");
        }
    }
    free(mesg);
}