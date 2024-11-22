#include "../include/contact.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Contact *contacts = NULL;

void addContact(char* name, char* number)
{
    Contact* newContact = (Contact*) malloc(sizeof(Contact));
    if (!newContact) {
        perror("Failed to allocate memory for new contact");
        exit(EXIT_FAILURE);
    }

    newContact->name = malloc(strlen(name) + 1);
    newContact->number = malloc(strlen(number) + 1);
    if (!newContact->name || !newContact->number) {
        perror("Failed to allocate memory for contact name or number");
        free(newContact);
        exit(EXIT_FAILURE);
    }

    strcpy(newContact->name, name);
    strcpy(newContact->number, number);
    newContact->next = NULL;

    if (contacts == NULL)
        contacts = newContact;
    else {
        Contact* last_node = contacts;
        while (last_node->next != NULL) last_node = last_node->next;
        last_node->next = newContact;
    }
}

Contact* deleteContact(const char* name)
{
    Contact* current = contacts;
    Contact* previous = NULL;

    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            if (previous == NULL)
                contacts = current->next;
            else
                previous->next = current->next;
            free(current);
            return current;
        }
        previous = current;
        current = current->next;
    }
    return NULL;
}

Contact* findContact(char* name)
{
    Contact* current = contacts;
    while (current != NULL) {
        if (strcmp(name, current->name) == 0)
            return current;
        current = current->next;
    }
    return NULL;
}

Contact* changeContactNumber(const char* name, char* number)
{
    Contact* current = contacts;
    while (current != NULL) {
        if (strcmp(name, current->name) == 0) {
            strcpy(current->number, number);
            return current;
        }
        current = current->next;
    }
    return NULL;
}
