#include "../include/commands.h"
#include "../include/user.h"
#include "../include/contact.h"

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

void processCommands(int connfd) {
    int authenticated = 0;
    char* username = NULL;
    char* password = NULL;
    while (1) {
        char* command = readString(connfd);
        if (command == NULL) {
            break;
        }
        if (command[0] == 0) {
            printf("\nClient Exit...\n");
            free(command);
            break;
        }
        if (username)
            printf("From client %s: %s\n", username, command);
        else
            printf("From client: %s\n", command);

        if (strcmp(command, "login") == 0) {
            username = readString(connfd);
            password = readString(connfd);
            if (username == NULL || password == NULL) {
                writeString(connfd, "Error reading login credentials");
                free(command);
                free(username);
                free(password);
                continue;
            }

            User* user = findUser(username);
            if (user != NULL && strcmp(password, user->password) == 0) {
                authenticated = 1;
                printf("%s authenticated successfully\n", username);
                writeString(connfd, "User authenticated successfully");
            } else {
                printf("Authentication failed for client\n");
                writeString(connfd, "Authentication failed");
            }
            free(password);
        } else if (strcmp(command, "add") == 0) {
            if (!authenticated) {
                writeString(connfd, "Authentication required");
                free(command);
                continue;
            }
            writeString(connfd, "User authenticated");

            char* name = readString(connfd);
            if (name == NULL) {
                writeString(connfd, "Error reading name");
                free(command);
                continue;
            }

            if (findContact(name) != NULL) {
                printf("Contact already exists\n");
                writeString(connfd, "Contact already exists");
            } else {
                writeString(connfd, "Contact available");
                char* number = readString(connfd);
                if (number == NULL) {
                    writeString(connfd, "Error reading number");
                    free(name);
                    free(command);
                    continue;
                }
                printf("Name: %s\nNumber: %s\n", name, number);
                addContact(name, number);
                free(number);
            }
            free(name);
        } else if (strcmp(command, "register") == 0) {
            username = readString(connfd);
            password = readString(connfd);
            if (username == NULL || password == NULL) {
                writeString(connfd, "Error reading registration credentials");
                free(command);
                free(username);
                free(password);
                continue;
            }

            if (findUser(username) != NULL) {
                printf("Username already exists\n");
                writeString(connfd, "Username already exists");
            } else {
                addUser(username, password);
                printf("User registered successfully\n");
                writeString(connfd, "User registered successfully");
            }
            free(username);
            free(password);
        } else if (strcmp(command, "delete") == 0) {
            if (!authenticated) {
                writeString(connfd, "Authentication required");
                free(command);
                continue;
            }
            writeString(connfd, "User authenticated");
            char* name = readString(connfd);
            if (name == NULL) {
                writeString(connfd, "Error reading name");
                free(command);
                continue;
            }
            printf("Name: %s\n", name);
            Contact* contact = deleteContact(name);
            if (contact == NULL) {
                writeString(connfd, "Contact not found");
                printf("Contact not found\n");
            } else {
                writeString(connfd, "Contact deleted");
                printf("Contact deleted\n");
            }
            free(name);
        } else if (strcmp(command, "print") == 0) {
            char buff[MAX] = "";
            Contact* current = contacts;
            while (current != NULL) {
                char temp[MAX];
                sprintf(temp, "Name: %s - number: %s", current->name, current->number);
                strcat(buff, temp);
                strcat(buff, "\n");
                current = current->next;
            }
            writeString(connfd, buff);
        } else if (strcmp(command, "modify") == 0) {
            if (!authenticated) {
                writeString(connfd, "Authentication required");
                free(command);
                continue;
            }
            writeString(connfd, "User authenticated");
            char* name = readString(connfd);
            char* number = readString(connfd);
            if (name == NULL || number == NULL) {
                writeString(connfd, "Error reading name or number");
                free(name);
                free(number);
                free(command);
                continue;
            }
            printf("Name: %s\nNumber: %s\n", name, number);
            Contact* contact = changeContactNumber(name, number);
            if (contact == NULL) {
                writeString(connfd, "Contact not found");
                printf("Contact not found\n");
            } else {
                writeString(connfd, "Contact modified");
                printf("Contact modified\n");
            }
            free(name);
            free(number);
        } else if (strcmp(command, "close") == 0) {
            printf("\nClient Exit...\n");
            free(command);
            break;
        }
        free(command);
    }
}
