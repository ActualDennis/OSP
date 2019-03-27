
typedef struct NodeInfo {
    char* fileName;
} NodeInfo;

typedef struct Node {
    NodeInfo info;
    struct Node *next;
} Node;

Node* list_create();


NodeInfo list_get(Node* head, int index);

void list_insertLast(Node *head, NodeInfo data);
void list_insertFirst(Node *head, NodeInfo data);
void list_insert(Node* head, int index, NodeInfo data);

int list_size(Node *head);

int list_isEmpty(Node *head);

