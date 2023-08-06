#ifndef HEAP_H
#define HEAP_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "node.h"

#ifndef PATH_SIZE
#define PATH_SIZE        256
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

#ifndef HEAP_STRUCT
#define HEAP_STRUCT   
#define HEAP_SWITCH(_MODE, _A, _B)    _MODE == 0 ? _A > _B : _A < _B
typedef struct heap {
    Node* root;
    int total_cnt;
}Heap;
int heap_counter_ = 0;
#endif

#ifndef INT_STR
#define INT_STR
char int_str_[PATH_SIZE];
#endif

/** 성공 시 true(0), 에러시 false(0)*/
int     max_heap_append(Heap* h, int v, const char* name);  // 최소 Heap 추가, name이 NULL 이면 Heap 생성순서대로 name 설정
int     min_heap_append(Heap* h, int v, const char* name);  // 최대 heap 추가, name이 NULL 이면 Heap 생성순서대로 name 설정
Node*   min_heap_pop (Heap* h);                             // 최소 Heap Pop -> Pop 한 최소 Key값 노드 리턴
Node*   max_heap_pop (Heap* h);                             // 최대 Heap Pop -> Pop 한 최대 Key값 노드 리턴
Heap*   heap();                                             // Heap 생성자
int     heap_clear(Heap* h);                                // 재귀호출이 아닌 while 후위탐색으로 전 노드 삭제
void    heap_print(Heap* h);                                // heap 자료구조 전체 출력 (트리형태로 출력 : 임시 큐 구현해서 사용)

#ifdef DEBUG
int main(void)
{
    Heap* he = heap();
    Node* tmp;
    for (int i = 0 ; i < 16 ; i++) {
        min_heap_append(he, i, NULL);
    }
    heap_print(he);
    heap_clear(he);
#ifdef DEBUG2
    for (int i = 0 ; i < 16 ; i++) {
        if(!(tmp = min_heap_pop(he))) {
            fprintf(stderr, "Heap Delete Error : %dth idx\n", i);
            exit(1);
        }
        printf("\n\n [%d]----> [delete_data : %d]\n",i, tmp->value);
        heap_print(he);
        free(tmp);
        tmp = NULL;
    }
#endif
    exit(0);
}
#endif

/** 정수 --> 문자열화*/
char* itos(int i) 
{
    memset(int_str_, 0, PATH_SIZE);
    snprintf(int_str_, PATH_SIZE, "%d", i);
    return int_str_;
}

Node* heap_node(int value, const char* path)
{
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->value = value;
    if (path == NULL)
        strncpy(newNode->path, itos(heap_counter_), PATH_SIZE);
    else
        strncpy(newNode->path, path, PATH_SIZE);

    newNode->prev = newNode->next = NULL;
    newNode->par = newNode; //초기 세팅은 지자신
    heap_counter_++;
    return newNode;
}

Heap* heap() 
{
    Heap* newHeap = (Heap*)malloc(sizeof(Heap));
    newHeap->root = NULL;
    newHeap->total_cnt = 0;
    return newHeap;
}

/** heap 의 현재 노드 개수로부터 heap 값 꼬리에 연결하기
 * 원리 : 2로 나눌 수 없을 때까지 나눈 나머지의 역수를 따라가면 노드 위치가나옴.
 *  ex) 11 --> 5 (나머지:1) --> 2 (1) --> 1 (0) / 이제 못 나눔
 *      루트기준으로 0(왼) -> 1(오) -> 0(왼) 으로 구할 수 있음.
*/
#ifndef HSTACK
#define HSTACK
typedef struct hstack {
    int r;
    struct hstack* next;
}Hstack;
#endif
typedef enum {h_append, h_delete} heap_mode__;
Node* heap_hang_node(Heap* h, int v, const char* name, heap_mode__ mode)
{

    if (h == NULL) {
        fprintf(stderr, "empty heap\n");
        return NULL;
    }

    if (h->root == NULL) {
        h->root = heap_node(v, name);
        h->total_cnt++;
        return h->root;
    }

    if (mode == h_delete && h->total_cnt == 1)  {
        return h->root;
    }

    Hstack* head = (Hstack*)malloc(sizeof(Hstack));
    Hstack* ptr = head;
    head->r = -1;
    int idx_num = h->total_cnt, pos = -1;
    if (mode == h_append)
        idx_num = ++(h->total_cnt);

    Node* nd_ptr = h->root;

    do {
        if (idx_num < 2)
            break;
        Hstack* n = (Hstack*)malloc(sizeof(Hstack));
        n->r = idx_num % 2;
        n->next = ptr;
        ptr = n;
    } while((idx_num /= 2)); 

    // prev : 왼(0), next : 오(1)
    while(ptr->r != -1) {
        Hstack* del = ptr;
        if (ptr->next->r != -1) {
            if (ptr->r == 0) 
                nd_ptr = nd_ptr->prev;
            else if (ptr->r == 1)
                nd_ptr = nd_ptr->next;
        }
        else
            pos = ptr->r;
        ptr = ptr->next;
        free(del);
    }
    free(head);

    if (nd_ptr == NULL || pos == -1) {
        fprintf(stderr, "[heap_hang_node] empty nd_ptr\n");
        h->total_cnt--;
        return NULL;
    }

    if (mode == h_append) {
        Node *newNode = heap_node(v, name);
        newNode->par = nd_ptr;
        if (pos == 0)
            nd_ptr->prev = newNode;
        else
            nd_ptr->next = newNode;
        return newNode;
    }
    else if (mode == h_delete) {
        if (pos == 0)
            nd_ptr = nd_ptr->prev;
        else 
            nd_ptr = nd_ptr->next;
        return nd_ptr;
    }
    else
        return NULL;
}

/**
 * Node의 연결구조는 그대로 두되, 내용만 바꿈
 * 내용이란 Node* 이외의 데이터를 의미함.
*/
int heap_swap_text (Node* a, Node* b)
{
    if (!a || !b) {
        fprintf(stderr, "[heap_swap_text] NULL pointer error\n");
        return false;
    }

    Node temp;
    temp.inode = a->inode;
    temp.value = a->value;
    strncpy(temp.path, a->path, PATH_SIZE);
    
    a->inode = b->inode;
    a->value = b->value;
    strncpy(a->path, b->path, PATH_SIZE);
    
    b->inode = temp.inode;
    b->value = temp.value;
    strncpy(b->path, temp.path, PATH_SIZE);
    return true;

}
int ha___(Heap* h, int v, const char* name, int mode)
{
    Node* curNode;
    if (!(curNode=heap_hang_node(h, v, name, h_append))) {
        return false;
    }
    //swapping 시작
    while (curNode != h->root && HEAP_SWITCH(mode, curNode->value, curNode->par->value)) {
        if(!heap_swap_text(curNode, curNode->par))
            return false;
        curNode = curNode->par;
    }
    return true;
}

int max_heap_append(Heap* h, int v, const char* name)
{
    if (!ha___(h, v, name, 0))
        return false;
    return true;
}

int min_heap_append(Heap* h, int v, const char* name)
{
    if (!ha___(h, v, name, 1))
        return false;
    return true;
}

Node* hp___ (Heap* h, int mode)
{
    if (h == NULL || h->root == NULL) {
        fprintf(stderr, "[heap_pop] Heap index is empty\n");
        return NULL;
    }

    Node* popNode = h->root, *endNode = heap_hang_node(h, -1, NULL, h_delete);
    if (!heap_swap_text(endNode, popNode)) 
        return NULL;

    //root노드만 있는경우 
    if (endNode == popNode) {
        h->root = NULL;
        h->total_cnt--;
        return endNode;
    }

    //항상 next -> prev 순으로 삭제됨 
    if (endNode->par->next) 
        endNode->par->next = NULL;
    else if (endNode->par->prev)
        endNode->par->prev = NULL;        
    else {
        fprintf(stderr, "[min_heap_pop] Finding End Node error\n");
        return NULL;
    }

    endNode->par = NULL; //안정성을 위해 부모 노드 끊어버림
    h->total_cnt--;
    while(popNode != NULL) {
        int left, right;
        if (popNode->prev == NULL && popNode->next == NULL)
            break;
        else if (popNode->prev != NULL && popNode->next == NULL) { 
            if (HEAP_SWITCH(mode, popNode->prev->value, popNode->value)) {
                if (!heap_swap_text(popNode, popNode->prev))
                    return false;
                popNode = popNode->prev;
                continue;
            }
        } 
        else {
            Node* check_node;
            if (HEAP_SWITCH(mode, popNode->prev->value, popNode->next->value))
                check_node = popNode->prev;
            else
                check_node = popNode->next;

            if (HEAP_SWITCH(mode, check_node->value, popNode->value)) {
                if (!heap_swap_text(popNode, check_node))
                    return false;
                popNode = check_node;
                continue;
            }
        }
        break;
    }
    return endNode;
}

Node* max_heap_pop (Heap* h) 
{
    Node* retNode = NULL;
    if(!(retNode=hp___(h, 0)))
        return NULL;
    return retNode;
}

Node* min_heap_pop (Heap* h) 
{
    Node* retNode = NULL;
    if(!(retNode=hp___(h, 1)))
        return NULL;
    return retNode;
}

#ifndef HQUEUE
#define HQUEUE
typedef struct hqueue{
    int depth;
    Node* ptr;
    struct hqueue* next; 
}hqueue__;
#endif

//BFS 방식으로 노드 순회
void heap_print(Heap *h)
{
    int i,j;
    hqueue__ *head, *tail; 
    hqueue__ *saved_head;
    setbuf(stdout, NULL);
    if (h == NULL) {
        return;
    }
    if (h->total_cnt == 0) {
        printf("----- !!! Heap is empty !!! -----\n");
        return;
    }
    hqueue__* init = (hqueue__*)malloc(sizeof(hqueue__));
    init->depth = 0;
    init->next = NULL;
    init->ptr = h->root;
    saved_head = head = tail = init;
     
    j = init->depth; 
    printf ("\n ----- started print Heap ------\n");
    while (head != NULL) {
        Node* ptr = head->ptr;
        if (j != head->depth) {
            printf("\n");
            j = head->depth;
        } 

        printf("[%d]", ptr->value);            
        Node* pn[3] = {ptr->prev, ptr->next, NULL};
        for (i = 0 ; pn[i] ; i++) {
            hqueue__ *newNode = (hqueue__*)malloc(sizeof(hqueue__));
            newNode->depth = head->depth + 1;
            newNode->ptr = pn[i];
            newNode->next = NULL;
            tail->next = newNode;
            tail = newNode;
        } 
        head = head->next;
    }
    printf ("\n ----- finished print Heap ------\n");
    while (saved_head) {
        hqueue__* del;
        del = saved_head;
        saved_head = saved_head->next;
        free(del);
    }
}

/**
 * 후위순회 방식으로 전 노드 삭제
 *  후위노드인데 재귀호출이 아니라 while로 구현함
*/
int     heap_clear(Heap* h)
{
    if (h == NULL || h->root == NULL)
        return false;

    Node* tmp = (Node*)malloc(sizeof(Node)); 
    tmp->inode = -2;
    Node* s = h->root;
    h->root->par = tmp;
    setbuf(stdout, NULL);
    printf("delete heap[key num] : ");
    int mode = -1;
    while(s != NULL && s->inode != -2) {
        Node* del;
        if (s->prev != NULL) {
            s = s->prev;
            continue;
        }
        s->inode = -1;  //삭제표시
        printf("%d ", s->value);
        s = s->par;
        if (s->prev != NULL && s->prev->inode) {
            free(s->prev);
            s->prev = NULL;
        }
        if (s->next != NULL && s->next->inode) {
            free(s->next); 
            s->next = NULL;
        }
        

        if (s->next != NULL) {
            s = s->next;
            continue;
        }
    }
    free(tmp);
    h->root = NULL;
    h->total_cnt = 0;
    free(h);
    h = NULL;
    printf("\nALL CLEAR!\n");
    return true;
}
#endif