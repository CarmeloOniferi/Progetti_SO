#ifndef CONTACT_H
#define CONTACT_H

typedef struct Contact {
    char* number;
    char* name;
    struct Contact* next;
} Contact;
extern Contact *contacts;

void addContact(char*, char*);
Contact* deleteContact(const char*);
Contact* findContact(char*);
Contact* changeContactNumber(const char*, char*);

#endif
