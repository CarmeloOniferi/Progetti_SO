#ifndef SO_CLIENT_COMMANDS_H
#define SO_CLIENT_COMMANDS_H

#define MAX 8

void func(int sockfd);
void writeString(int connfd, const char* str);
char* readString(int connfd);

#endif
