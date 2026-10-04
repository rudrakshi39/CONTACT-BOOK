/*
 * Contact Book - a simple command-line program in C
 * Features: add, view, search, delete contacts. Data is saved to contacts.txt
 *
 * Compile: gcc contact_book.c -o contact_book
 * Run:     ./contact_book
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_CONTACTS 100
#define NAME_LEN 50
#define PHONE_LEN 15
#define FILE_NAME "contacts.txt"

typedef struct {
    char name[NAME_LEN];
    char phone[PHONE_LEN];
} Contact;

Contact contacts[MAX_CONTACTS];
int count = 0;

/* Safely read a line of text from the user and remove the newline. */
void read_line(const char *prompt, char *buf, int size) {
    printf("%s", prompt);
    if (fgets(buf, size, stdin) == NULL) {
        buf[0] = '\0';
        return;
    }
    char *nl = strchr(buf, '\n');
    if (nl) {
        *nl = '\0';
    } else {
        /* Input was too long: throw away the rest of the line. */
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }
}

/* Load contacts from the file when the program starts. */
void load_contacts(void) {
    FILE *fp = fopen(FILE_NAME, "r");
    if (fp == NULL) return; /* No file yet: first run */

    char line[128];
    while (count < MAX_CONTACTS && fgets(line, sizeof line, fp)) {
        if (sscanf(line, "%49[^|]|%14[^\n]",
                   contacts[count].name, contacts[count].phone) == 2) {
            count++;
        }
    }
    fclose(fp);
}

/* Write all contacts to the file (one per line as name|phone). */
void save_contacts(void) {
    FILE *fp = fopen(FILE_NAME, "w");
    if (fp == NULL) {
        printf("Error: could not save contacts.\n");
        return;
    }
    for (int i = 0; i < count; i++) {
        fprintf(fp, "%s|%s\n", contacts[i].name, contacts[i].phone);
    }
    fclose(fp);
}

void add_contact(void) {
    if (count >= MAX_CONTACTS) {
        printf("Contact book is full.\n");
        return;
    }
    Contact c;
    read_line("Name: ", c.name, NAME_LEN);
    read_line("Phone: ", c.phone, PHONE_LEN);

    if (strlen(c.name) == 0 || strlen(c.phone) == 0) {
        printf("Name and phone cannot be empty.\n");
        return;
    }
    contacts[count++] = c;
    save_contacts();
    printf("Contact added.\n");
}

void view_contacts(void) {
    if (count == 0) {
        printf("No contacts yet.\n");
        return;
    }
    printf("\n%-4s %-30s %s\n", "No.", "Name", "Phone");
    printf("------------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        printf("%-4d %-30s %s\n", i + 1, contacts[i].name, contacts[i].phone);
    }
}

/* Case-insensitive check: does text contain word? */
int contains_ignore_case(const char *text, const char *word) {
    int n = strlen(text), m = strlen(word);
    for (int i = 0; i + m <= n; i++) {
        int j = 0;
        while (j < m &&
               tolower((unsigned char)text[i + j]) == tolower((unsigned char)word[j])) {
            j++;
        }
        if (j == m) return 1;
    }
    return 0;
}

void search_contact(void) {
    char key[NAME_LEN];
    read_line("Search for name: ", key, NAME_LEN);

    int found = 0;
    for (int i = 0; i < count; i++) {
        if (contains_ignore_case(contacts[i].name, key)) {
            printf("%s - %s\n", contacts[i].name, contacts[i].phone);
            found = 1;
        }
    }
    if (!found) printf("No match found.\n");
}

void delete_contact(void) {
    if (count == 0) {
        printf("No contacts to delete.\n");
        return;
    }
    view_contacts();

    char buf[16];
    read_line("Enter the number to delete: ", buf, sizeof buf);
    int n = atoi(buf);

    if (n < 1 || n > count) {
        printf("Invalid number.\n");
        return;
    }
    /* Shift everything after the deleted contact one place left. */
    for (int i = n - 1; i < count - 1; i++) {
        contacts[i] = contacts[i + 1];
    }
    count--;
    save_contacts();
    printf("Contact deleted.\n");
}

int main(void) {
    load_contacts();

    char choice[8];
    do {
        printf("\n===== CONTACT BOOK =====\n");
        printf("1. Add contact\n");
        printf("2. View all contacts\n");
        printf("3. Search contact\n");
        printf("4. Delete contact\n");
        printf("5. Exit\n");
        read_line("Choose an option: ", choice, sizeof choice);

        switch (atoi(choice)) {
            case 1: add_contact();    break;
            case 2: view_contacts();  break;
            case 3: search_contact(); break;
            case 4: delete_contact(); break;
            case 5: printf("Goodbye!\n"); break;
            default: printf("Invalid option, try again.\n");
        }
    } while (atoi(choice) != 5);

    return 0;
}

