#include "../include/commands.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

/**
 * @brief Write a string to the client, dynamically allocated memory for the string
 * @param connfd The connection file descriptor
 * @param str  The string to write
 */

void writeString(int connfd, const char* str) {
    int length = (int) strlen(str);
    if (write(connfd, &length, sizeof(length)) != sizeof(length)) {
        perror("Failed to write length");
    }
    if (write(connfd, str, length) != length) {
        perror("Failed to write string");
    }
}

/**
 * @brief Read a string from the client
 * @param connfd The connection file descriptor
 * @return The string read from the client, dynamically allocated memory for the string
 */

char* readString(int connfd) {
    int length;
    if (read(connfd, &length, sizeof(length)) != sizeof(length)) {
        perror("Failed to read length");
        return NULL;
    }
    char* buffer = (char*)malloc(length + 1);
    if (buffer == NULL) {
        perror("Failed to allocate memory");
        return NULL;
    }
    if (read(connfd, buffer, length) != length) {
        perror("Failed to read string");
        free(buffer);
        return NULL;
    }
    buffer[length] = '\0';
    return buffer;
}

void func(int sockfd) {
    char buff[MAX];
    while (1) {
        bzero(buff, sizeof(buff));
        printf("Commands:\n"
               "1. login\n"
               "2. register\n"
               "3. add\n"
               "4. modify\n"
               "5. delete\n"
               "6. print\n"
               "7. close\n"
               "Enter the command: ");
        scanf("%s", buff);
        writeString(sockfd, buff);

        if (strcmp(buff, "add") == 0) {
            char* response = readString(sockfd);
            if (response == NULL) {
                continue;
            }
            if (strcmp(response, "Authentication required") == 0) {
                printf("Authentication required\n");
                free(response);
                continue;
            }
            free(response);

            printf("Enter name: ");
            scanf("%s", buff);
            writeString(sockfd, buff);

            response = readString(sockfd);
            if (response == NULL) {
                continue;
            }
            if (strcmp(response, "Contact already exists") == 0) {
                printf("Contact already exists\n");
                free(response);
            } else {
                free(response);
                printf("Enter number: ");
                scanf("%s", buff);
                writeString(sockfd, buff);
            }
        } else if (strcmp(buff, "delete") == 0) {
            char* response = readString(sockfd);
            if (response == NULL) {
                continue;
            }
            if (strcmp(response, "Authentication required") == 0) {
                printf("Authentication required\n");
                free(response);
                continue;
            }
            free(response);

            bzero(buff, sizeof(buff));
            printf("Enter name: ");
            scanf("%s", buff);
            writeString(sockfd, buff);

            response = readString(sockfd);
            if (response == NULL) {
                continue;
            }
            if (strcmp(response, "Contact not found") == 0) {
                printf("Contact not found\n");
            } else if (strcmp(response, "Contact deleted") == 0) {
                printf("Contact deleted\n");
            }
            free(response);
        } else if (strcmp(buff, "print") == 0) {
            char* contacts = readString(sockfd);
            if (contacts != NULL) {
                printf("Contacts:\n%s\n", contacts);
                free(contacts);
            }
        } else if (strcmp(buff, "modify") == 0) {
            char* response = readString(sockfd);
            if (response == NULL) {
                continue;
            }
            if (strcmp(response, "Authentication required") == 0) {
                printf("Authentication required\n");
                free(response);
                continue;
            }
            free(response);

            bzero(buff, sizeof(buff));
            printf("Enter name: ");
            scanf("%s", buff);
            writeString(sockfd, buff);

            bzero(buff, sizeof(buff));
            printf("Enter new number: ");
            scanf("%s", buff);
            writeString(sockfd, buff);

            response = readString(sockfd);
            if (response == NULL) {
                continue;
            }
            if (strcmp(response, "Contact not found") == 0) {
                printf("Contact not found\n");
            } else if (strcmp(response, "Contact modified") == 0) {
                printf("Contact modified\n");
            }
            free(response);
        } else if (strcmp(buff, "register") == 0) {
            printf("Enter username: ");
            scanf("%s", buff);
            writeString(sockfd, buff);

            printf("Enter password: ");
            scanf("%s", buff);
            writeString(sockfd, buff);

            char* response = readString(sockfd);
            if (response != NULL) {
                if (strcmp(response, "Username already exists") == 0) {
                    printf("Username already exists\n");
                } else if (strcmp(response, "User registered successfully") == 0) {
                    printf("User registered successfully\n");
                }
                free(response);
            }
        } else if (strcmp(buff, "login") == 0) {
            printf("Enter username: ");
            scanf("%s", buff);
            writeString(sockfd, buff);

            printf("Enter password: ");
            scanf("%s", buff);
            writeString(sockfd, buff);

            char* response = readString(sockfd);
            if (response != NULL) {
                if (strcmp(response, "User authenticated successfully") == 0) {
                    printf("User authenticated successfully\n");
                } else if (strcmp(response, "Authentication failed") == 0) {
                    printf("Authentication failed\n");
                }
                free(response);
            }
        } else if (strcmp(buff, "close") == 0) {
            writeString(sockfd, buff);
            break;
        }
    }
    printf("Client Exit...\n");
}
