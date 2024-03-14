#include "node.h"

struct node* head = NULL;

void insert(char* str) {
    struct node* new_node = (struct node*)malloc(sizeof(struct node));
    if (new_node == NULL) {
        perror("Memory allocation failed for new_node");
        return;
    }
    
    new_node->str = (char*)malloc(sizeof(char) * (strlen(str) + 1));
    if (new_node->str == NULL) {
        perror("Memory allocation failed for new_node->str");
        free(new_node);
        return;
    }
    strcpy(new_node->str, str);
    if (head == NULL) {
        new_node->prev = NULL;
        new_node->next = NULL;
        head = new_node;
    }
    else {
        struct node* curr = head;
        while (curr != NULL && strcmp(curr->str, str) > 0) {
            curr = curr->next;
        }
        if (curr == head) {
            new_node->next = head;
            new_node->prev = NULL;
            head->prev = new_node;
            head = new_node;
        }
        else if (curr == NULL) {
            struct node* last = head;
            while (last->next != NULL) {
                last = last->next;
            }
            last->next = new_node;
            new_node->prev = last;
            new_node->next = NULL;
        }
        else {
            new_node->next = curr;
            new_node->prev = curr->prev;
            curr->prev->next = new_node;
            curr->prev = new_node;
        }
    }
}

void traverse() {
    struct node* temp = head;
    while (temp != NULL) {
        printf("%s\n", temp->str);
        temp = temp->next;
    }
    printf("\n");
}

void erase(unsigned int i) {
    struct node* temp = head;
    while (temp != NULL && i > 0) {
        temp = temp->next;
        i--;
    }

    if (temp != NULL) {
        if (temp->prev != NULL)
            temp->prev->next = temp->next;
        else
            head = temp->next;

        if (temp->next != NULL)
            temp->next->prev = temp->prev;

        free(temp->str);
        free(temp);
    }
}

void free_list() {
    struct node* temp;
    struct node* next;

    temp = head;
    while (temp != NULL) {
        next = temp->next;
        free(temp->str);
        free(temp);
        temp = next;
    }
}
