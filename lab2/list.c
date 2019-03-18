#include "list.h"
#include <stdlib.h>

#define true 1
#define false 0


Node* list_create() {
    Node *el = (Node*) malloc(sizeof(Node));
    el->next = NULL;
    return el;
}

int list_isEmpty(Node *head) {
    return (head == NULL) || (head->next == NULL) ? true : false;
}


int list_size(Node *head) {
    if(list_isEmpty(head)) return 0;

    Node *tmp = head->next;

    int size = 0;
    while(tmp) {
        size++;
        tmp = tmp->next;
    }

    return size;
}



NodeInfo list_get(Node* head, int index) {
    if (list_isEmpty(head) || (index < 0)) exit(-1);

    int i = 0;
    Node *tmp = head->next;
    while((i++ < index) && tmp) {
        tmp = tmp->next;
    }
    if(tmp == NULL) exit(-1);

    return tmp->info;
}



Node* list_createElement(NodeInfo data) {
    Node *el = (Node*) malloc(sizeof(Node));
    el->info = data;
    el->next = NULL;
    return el;
}

void list_insertLast(Node *head, NodeInfo data) {
    Node *tmp = head;
    while(tmp->next) {
        tmp = tmp->next;
    }

    tmp->next = NULL;
    tmp->next = list_createElement(data);
}


void list_insert(Node* head, int index, NodeInfo data) {
    int i = 0;
    Node *tmp = head->next;
    while((i++ < index-1) && tmp) {
        tmp = tmp->next;
    }
    if((tmp == NULL) || (tmp->next == NULL)) exit(-1);

    // tmp - element after which you want to insert an element
    Node *newEl = list_createElement(data);
    newEl->next = tmp->next;
    tmp->next = newEl;
}

