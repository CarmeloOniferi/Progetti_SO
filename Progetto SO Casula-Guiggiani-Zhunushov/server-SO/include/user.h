#ifndef USER_H
#define USER_H

typedef struct User {
    char* username;
    char* password;
    struct User* next;
} User;
extern User *users;

User* findUser(char*);
void addUser(char*, char*);

#endif
