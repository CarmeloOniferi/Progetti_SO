#include "../include/user.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

User *users = NULL;

User* findUser(char* username)
{
    User* current = users;
    while (current != NULL) {
        if (strcmp(username, current->username) == 0)
            return current;
        current = current->next;
    }
    return NULL;
}

void addUser(char* username, char* password)
{
    User* newUser = (User*) malloc(sizeof(User));
    if (!newUser) {
        perror("Failed to allocate memory for new user");
        exit(EXIT_FAILURE);
    }

    newUser->username = malloc(strlen(username) + 1);
    newUser->password = malloc(strlen(password) + 1);
    if (!newUser->username || !newUser->password) {
        perror("Failed to allocate memory for user credentials");
        free(newUser);
        exit(EXIT_FAILURE);
    }

    strcpy(newUser->username, username);
    strcpy(newUser->password, password);
    newUser->next = NULL;

    if (users == NULL)
        users = newUser;
    else {
        User* last_node = users;
        while (last_node->next != NULL) last_node = last_node->next;
        last_node->next = newUser;
    }
}