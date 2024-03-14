#ifndef NODE_H
#define NODE_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct node {
    struct node* prev;
    char* str;
    struct node* next;
};

void insert(char* str);
void traverse();
void erase(unsigned int i);
void free_list();

#endif // NODE_H