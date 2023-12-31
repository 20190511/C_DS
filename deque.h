#ifndef DEQUE
#define DEQUE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "node.h"
#ifndef PATH_SIZE
#define PATH_SIZE        256
#endif
#define INDEX_ERROR     "EndexOutOfError"


#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

#ifndef NODE
#define NODE
typedef struct node {
    int inode;
    int value;
    char path[PATH_SIZE];
    struct node* next;
    struct node* prev; 
    struct node* par;
}Node;
#endif

typedef struct {
   Node* head;
   Node* tail;
   int mode;    //0 : local 변수, 1 : 동적할당
   int count;
}List;

/** Init function*/
List* init(List* li);
Node* node (int data);
Node* nodeObj (Obj* Obj);

// For Insert Method
void append_tail(List* l, int data, const char* name = "null");       // Deque 앞에 연결
void append_head(List* l, int data, const char* name = "NULL");       // Deque 끝에 연결
int append(List* l, int idx, int data, const char* name = "NULL");    // Deque index 요소 연결

void append_tail_obj(List* l, Obj* obj, const char* name = "NULL");       // Deque 앞에 연결
void append_head_obj(List* l, Obj* obj, const char* name = "NULL");       // Deque 끝에 연결
int append_obj(List* l, int idx, Obj* obj, const char* name = "NULL");    // Deque index 요소 연결

// For Delete Method
Node* pop_front(List* l);           // Deque 끝 삭제
Node* pop_rear(List* l);            // Deque 앞 삭제
Node* pop(List* l, int idx);        // Deque 요소 삭제

void printn (List* list);       //Deque 순회

/** free 함수들 (Method)*/
void freeNode (Node* node);
void freeList (List* list);

/**  Utility */
Node* deque (List* li, int index); // Deqeue 인덱스 접근
int deque_count (List* li);


#ifdef DEBUG
int main(void)
{
    List* li = init();    
    append_head(li, 10);
    append_head(li, 20);
    append_head(li, 30);
    append_head(li, 40);
    printn(li);

    //Node* t = deque(li, -2);
    append(li, -2, 10);
    printn(li);

    Node* pops = pop(li, 2);
    printn(li);
    printf("tail : %d\n", li->tail->prev->value);
    exit(0);
}
#endif

//clear(DeAllocate) node
void freeNode (Node* node)
{
    //del element
    memset(node->path, 0, PATH_SIZE);
    node->next = node->prev = NULL; //안전하게 삭제
    free(node);
    return; 
}

// clear all elements
void freeList (List* list)
{
    Node* node, *delNode;
    for (node = list->head ; node ; ) {
        delNode = node;
        node = node->next;
        freeNode(delNode);
    }

    // 안전하게 삭제
    list->head = list->tail = NULL;
    list->count = 0;
    if (list->mode) 
        free(list);
}

int deque_count (List* li) 
{
    return li->count;
}

Node* pop(List* l, int idx)        // Deque 요소 삭제
{
    Node* ptr = deque(l, idx);
    if (ptr == NULL || ptr == l->head) //삭제할 때 head 나오면 에러로 간주
        return false;

    l->count--;
    ptr->prev->next = ptr->next;
    ptr->next->prev = ptr->prev;
    ptr->next = ptr->prev = NULL;
    
    return ptr;
}

/**
 * 파이썬처럼 append(l, data, name, -1) 이렇게하면 마지막 부분에 연결하고
 * append(l, data, name, 1) 두 번째 요소에 연결함 : 
 * 만약 요소 idx가 범위를 벗어나면 에러처리
*/
int append(List* l, int idx, int data, const char* name)
{
    //idx가 가리키는 위치로 삽입이 되도록 설정
    Node* ptr = deque(l, idx);
    //노드가 없는 경우
    if (ptr == NULL) 
        return false;
    
    Node* newNode = node(data);
    if (!strcmp(name, "NULL"))
        snprintf(newNode->path, PATH_SIZE, "%d", l->count); 
    else
        strncpy(newNode->path, name, PATH_SIZE);

    l->count++;
    if (idx >= 0) {
        newNode->next = ptr;
        newNode->prev = ptr->prev;
    }
    else {
        newNode->prev = ptr;
        newNode->next = ptr->next;
    }
    newNode->prev->next = newNode->next->prev = newNode;
    return true;
}


Node* pop_front(List* l)
{
    Node* ret;
    if (l->count < 1) {
        printf("[Pop Error] : %s", INDEX_ERROR);
        return NULL;
    }
    
    l->count--;
    ret = l->head->next;
    ret->prev->next = ret->next;
    ret->next->prev = ret->prev;
    ret->prev = ret->next = NULL;
    return ret;
}
Node* pop_rear(List* l)
{
    Node* ret;
    if (l->count < 1) {
        printf("[Pop Error] : %s", INDEX_ERROR);
        return NULL;
    }
    
    l->count--;
    ret = l->tail->prev;
    ret->prev->next = ret->next;
    ret->next->prev = ret->prev;
    ret->prev = ret->next = NULL;
    return ret;
}

/**
 *  init(NULL)  사용 시 동적할당으로 list 할당
 *  init(&li) 사용 시 local 변수로 list 할당
*/
List* init(List* li)
{
    List* list = li;
    if (!li) {
        list = (List*)malloc(sizeof(List));
        list->mode = 1;
    }
    else
        list->mode = 0;
    Node* h = (Node*)malloc(sizeof(Node));
    Node* t = (Node*)malloc(sizeof(Node));

    h->prev = t->next = NULL;
    h->next = t; t->prev = h;
    list->head = h; list->tail = t;
    strncpy(h->path, "dummy_head", 20);
    strncpy(t->path, "dummy_tail", 20);
    h->value = t->value = -1;
    list->count = 0;
    return list;
}

Node* node (int data)
{
    Node* newNode = (Node*)malloc(sizeof(Node));
    //strncpy(newNode->path, name, NODE_NAME);
    newNode->value = data;
    newNode->prev = newNode->next = NULL;
    newNode->obj = NULL;
    return newNode;
}

/**
 * Object 리스트, (다양한 자료구조를 deque에 연결할 수 있음)
*/
Node* nodeObj (Obj* obj)
{
    Node* newNode = node(0);
    if (obj != NULL)
        newNode->obj = obj;
    /*
    switch (mode) {
    case OBJ_INT:
        set_int(newNode->obj, *((int*)ptr))
        break;
    case OBJ_CHAR:
        set_char(newNode->obj, *((char*)ptr))
        break;
    case OBJ_DOUBLE:
        set_double(newNode->obj, *((double*)ptr))
        break;
    case OBJ_VOID:
        set_obj(newNode->obj, ptr, OBJ_VOID);
        break;
    }
    */
    return newNode;
}

void append_tail(List* l, int data, const char* path )
{
    if (l == NULL)
        return;
    Node* ns = node(data);
    if (!strcmp(path, "NULL"))
        snprintf(ns->path, PATH_SIZE, "%d", l->count); 
    else
        strncpy(ns->path, path, PATH_SIZE);

    l->count++;
    ns->prev = l->tail->prev;
    ns->next = l->tail;
    ns->prev->next = ns->next->prev = ns;
}

void append_head(List* l, int data, const char* path )
{
    if (l == NULL || l->head == NULL)
        return;

    Node* ns = node(data);
    if (!strcmp(path, "NULL"))
        snprintf(ns->path, PATH_SIZE, "%d", l->count); 
    else
        strncpy(ns->path, path, PATH_SIZE);

    l->count++;
    ns->next = l->head->next;
    ns->prev = l->head;
    ns->next->prev = ns->prev->next = ns;
}

void append_tail_obj(List* l, Obj* obj, const char* path)       // Deque 앞에 연결
{
    if (l == NULL || obj == NULL)
        return;
    Node* ns = nodeObj(obj);
    if (!strcmp(path, "NULL"))
        snprintf(ns->path, PATH_SIZE, "%d", l->count); 
    else
        strncpy(ns->path, path, PATH_SIZE);

    l->count++;
    ns->prev = l->tail->prev;
    ns->next = l->tail;
    ns->prev->next = ns->next->prev = ns;
}

void append_head_obj(List* l, Obj* obj, const char* path)       // Deque 끝에 연결
{
    if (l == NULL || l->head == NULL || obj == NULL)
        return;

    Node* ns = nodeObj(obj);
    if (!strcmp(path, "NULL"))
        snprintf(ns->path, PATH_SIZE, "%d", l->count); 
    else
        strncpy(ns->path, path, PATH_SIZE);

    l->count++;
    ns->next = l->head->next;
    ns->prev = l->head;
    ns->next->prev = ns->prev->next = ns;
}

int append_obj(List* l, int idx, Obj* obj, const char* name)    // Deque index 요소 연결
{
    //idx가 가리키는 위치로 삽입이 되도록 설정
    Node* ptr = deque(l, idx);
    //노드가 없는 경우
    if (ptr == NULL || obj == NULL) 
        return false;
    
    Node* newNode = nodeObj(obj);
    if (!strcmp(name, "NULL"))
        snprintf(newNode->path, PATH_SIZE, "%d", l->count); 
    else
        strncpy(newNode->path, name, PATH_SIZE);

    l->count++;
    if (idx >= 0) {
        newNode->next = ptr;
        newNode->prev = ptr->prev;
    }
    else {
        newNode->prev = ptr;
        newNode->next = ptr->next;
    }
    newNode->prev->next = newNode->next->prev = newNode;
    return true;

}

void printn (List* list)
{
    if (list == NULL || list->head == NULL || list->count == 0) {
        printf("Deque is empty\n");
        return;
    }

    for (Node* a = list->head ; a ; a = a->next) {
        if (a->obj == NULL) 
            printf("[%s] : %d", a->path, a->value);
        else
            printo(a->obj);
        if (a->next != NULL)
            printf(" <=> ");
        else
            printf("\n");
    }
}

/** : 파이썬 처럼 지원 deque(k) : k 번 요소 접근
 *  --> k가 음수 (라면 뒤에서부터 접근)
 *  --> k가 양수 (처음부터 접근)
 */
Node* deque (List* li, int index)
{
    Node* return_addr;
    int i, idx;
    if ((index >= 0 && li->count < index) ||
        (index < 0 && li->count < -index)) {
        printf("[OutOfIndexError]\n");
        return NULL;
    }

    if (index >= 0) {
        idx = index + 1;
        for (return_addr = li->head, i = 0 ; i < idx ; i++, return_addr = return_addr->next);
    }
    else {
        idx = -index;
        for (return_addr = li->tail, i = 0 ; i < idx ; i++, return_addr = return_addr->prev);
    }
    
    return return_addr;
}


#endif