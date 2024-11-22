#ifndef COMMANDS_H
#define COMMANDS_H

#define MAX 1024

void processCommands(int connfd);
char* readString(int connfd);
void writeString(int connfd, const char* str);

#endif
