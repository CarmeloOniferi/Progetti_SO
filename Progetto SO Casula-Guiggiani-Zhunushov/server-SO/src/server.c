#include "../include/server.h"
#include "../include/commands.h"

#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>

#define PORT 8080
#define SA struct sockaddr

void setupServer() {
    int sockfd;
    socklen_t len;
    struct sockaddr_in servaddr, cli;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");

    if ((listen(sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
    len = sizeof(cli);

    while (1) {
        int *connfd = malloc(sizeof(int));
        *connfd = accept(sockfd, (SA *) &cli, &len);
        if (*connfd < 0) {
            printf("server accept failed... retrying\n");
            sleep(1);
            continue;
        } else printf("\nserver accept the client...\n");

        pthread_t clientThread;
        if (pthread_create(&clientThread, NULL, clientHandler, connfd) != 0) {
            printf("Failed to create thread\n");
            return;
        }
    }
}

void* clientHandler(void *arg) {
    int connfd = *((int *)arg);
    free(arg);
    processCommands(connfd);
    if (close(connfd) < 0) {
        perror("Failed to close connection");
    }
    return NULL;
}