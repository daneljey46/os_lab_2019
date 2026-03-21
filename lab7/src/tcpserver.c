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

    int lfd, cfd;
    int nread;
    int port = atoi(argv[1]);
    int bufsize = atoi(argv[2]);
    char *buf = malloc(bufsize);
    struct sockaddr_in servaddr, cliaddr;

    if ((lfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    memset(&servaddr, 0, sizeof(struct sockaddr_in));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    if (bind(lfd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in)) < 0) {
        perror("bind");
        exit(1);
    }

    if (listen(lfd, 5) < 0) {
        perror("listen");
        exit(1);
    }

    while (1) {
        unsigned int clilen = sizeof(struct sockaddr_in);
        if ((cfd = accept(lfd, (struct sockaddr *)&cliaddr, &clilen)) < 0) {
            perror("accept");
            continue;
        }
        printf("connection established\n");

        while ((nread = read(cfd, buf, bufsize)) > 0) {
            write(1, buf, nread);
        }
        close(cfd);
    }
    free(buf);
    return 0;
}