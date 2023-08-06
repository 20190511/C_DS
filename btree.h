#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#ifndef KEY_SIZE
#define KEY_SIZE        5
#define MIN_KEY_SIZE    KEY_SIZE/2
#define PATH_SIZE       256
#define MAX(_A,_B)      _A >= _B ? _A : _B
#define MIN(_A,_B)      _A <= _B ? _A : _B
#endif

#ifndef true
#define true            1
#define false           0
#endif

typedef struct {
    char path[256];
    int inode;
}Node;

typedef struct bnode_{
    Node* node_key[KEY_SIZE+1];     //key_ptr
    struct bnode_* node_ptr[KEY_SIZE+2];        //B트리 Pointer
    struct bnode_* parent_ptr;                  //B트리 부모 Pointer
    int count;
}BNode;

typedef struct  {
    int total_count;
    BNode* root;
}Header;

Node*   node(const char* p, int i);
Header* header();
BNode*  bnode();
Node*   btree_search(Header* h, const char* f, BNode** b);
void    btree_print(Header* h);
int     btree_append(Header* h, Node* n);
Node*   btree_delete(Header* h, const char* s); 

int btree_insert_index(BNode* bn, Node* n, int idx);

#ifdef DEBUG
int main(void)
{
    const char* test_list[] = {"junhyeong", "ayax", "danke", "brae", "ab",
    "abab", "alpha", "alke", "ssuso", "sexo", "pento", "donsu", "dulbt",
    "slayer", "LGG", "targetNum", "sti", "truman", "go2", "go", "omg", "oska",
    "omsg", "oriental", "ytsiof", "kusk"};

    
    Header* btree = header();
    for (int i = 0 ; i < (sizeof(test_list) / sizeof(test_list[0])) ; i++) {
        btree_append(btree, node(test_list[i], 30));
        btree_print(btree);
    }
    btree_print(btree);
    for (int i = 0 ; i < (sizeof(test_list) / sizeof(test_list[0])) ; i++) {
        if (!strcmp("sti", test_list[i]))
            printf("??\n");
        printf("\n\n---> delete : %s\n", test_list[i]);
        btree_delete(btree, test_list[i]);
        btree_print(btree);
    }
    exit(0);
}
#endif

/**
 * strcmp(A,B) : A>B 이면 1, A<B 이면 -1
*/
int btree_find_index(BNode* bn, const char *s)
{
    int i = 0;
    for (i = 0 ; i < bn->count ; i++) {
        if (strcmp(bn->node_key[i]->path, s) >= 0)
            break;
    }
    //만약 i가 마지막부분을 고려하는지 체킹
    return i;
}

Node* btree_search(Header* h, const char* f, BNode** b)
{
    int idx;
    BNode* ptr = h->root;
    if (h == NULL || h->root == NULL) {
        if (b != NULL)
            b = NULL;
        return NULL;
    }

    while(true) {
        idx = btree_find_index(ptr, f);
        if (ptr->node_key[idx] != NULL && !strcmp(ptr->node_key[idx]->path, f)) { 
            if (b != NULL)
                *b = ptr;
            return ptr->node_key[idx];
        }
        else if (ptr->node_ptr[idx] != NULL) 
            ptr = ptr->node_ptr[idx];
        else {
            if (b != NULL)
                *b = NULL;
            return NULL;
        }
    }
    
}

int btree_delete_index(BNode* bn, int del_idx)
{
    if (bn->count <= del_idx) {
        fprintf(stderr, "btree_delete_index error : OutOfIndex\n");
        return false;
    }
    BNode tmp;
    int offset = bn->count - del_idx - 1;
    memcpy(&(tmp.node_key[0]), &(bn->node_key[del_idx + 1]), offset * sizeof(Node *));
    memcpy(&(tmp.node_ptr[0]), &(bn->node_ptr[del_idx + 2]), offset * sizeof(BNode *));

    memcpy(&(bn->node_key[del_idx]), &(tmp.node_key[0]), offset * sizeof(Node *));
    memcpy(&(bn->node_ptr[del_idx+1]), &(tmp.node_ptr[0]), offset * sizeof(BNode *));

    bn->node_key[bn->count - 1] = NULL;
    bn->node_ptr[bn->count] = NULL;
    bn->count--;
    return true;
}

int btree_insert_index(BNode* bn, Node* n, int idx)
{
    BNode cpy__;    //복사를 하기위한 임시노드
    if (bn->count >= KEY_SIZE + 1) {
        fprintf(stderr, "btree_insert_index error\n");
        return false;
    }

    int cpy_size = bn->count - idx; //복사해야하는 칸 수 + 1 (더미노드)
    // node_key 는 idx 부터 cpy_size만큼 
    // node_idx 는 idx+1 부터 cpy_size만큼
    if (cpy_size >= 0) {
        memcpy(&(cpy__.node_key[0]), &(bn->node_key[idx]), sizeof(BNode *) * cpy_size);
        memcpy(&(cpy__.node_ptr[0]), &(bn->node_ptr[idx]), sizeof(Node *) * (cpy_size + 1));
    }
    //복사, 다음 노드는 NULL로 만듦
    bn->node_key[idx] = n;
    //bn->node_ptr[idx] = NULL;

    if (cpy_size >= 0) {
        memcpy(&(bn->node_key[idx + 1]), &(cpy__.node_key[0]), sizeof(BNode *) * cpy_size);
        memcpy(&(bn->node_ptr[idx + 1]), &(cpy__.node_ptr[0]), sizeof(Node *) * (cpy_size + 1));
    }
    bn->count++;
    return true;
}
 
Node*   btree_delete(Header* h, const char* s)
{
    if (!strcmp(s, "olle"))
        printf("\n");
    BNode* find_bnode;
    //btree 어느 Bnode 인지 찾고 + 루트노드가 아니면 스와핑(왼or오)
    Node*  find_node = btree_search(h, s, &find_bnode), *ret_node;
    if (find_bnode == NULL) {
        fprintf(stderr, "%s is not in B-tree\n", s);
        return NULL;
    }

    int idx = btree_find_index(find_bnode, s);
    // 리프노드가 아니면 swaping을 해서 맨 밑으로 끌어내려야함
    if (find_bnode->node_ptr[0]) {
        int lcnt = 0, rcnt = 0, lscnt = 0, rscnt = 1;
        BNode *blptr = find_bnode->node_ptr[idx], *brptr = find_bnode->node_ptr[idx+1];

        //왼쪽 자식으로 내려가면 바로 삭제가능? 왼쪽 자식 중 가장 큰 노드로 계속..
        while(blptr->node_ptr[0]) {
            blptr = blptr->node_ptr[blptr->count];
        }
        //오른쪽 자식으로 내려가면 바로 삭제가능?
        while(brptr->node_ptr[0]) {
            brptr = brptr->node_ptr[0];
        }
        lcnt = blptr->count, rcnt = brptr->count;
        BNode *child, *parent;
        Node  *tmp = find_node;
        //swapping만 하면됨 --> 수정! : swap이 아니라 바꾸기만하면됨!
        if (lcnt >= rcnt) { //왼쪽
            find_bnode->node_key[idx] = blptr->node_key[lcnt-1];
            blptr->node_key[lcnt-1] = tmp;
            find_bnode = blptr;
            idx = lcnt-1;
        }
        else { //오른쪽
            find_bnode->node_key[idx] = brptr->node_key[0];
            brptr->node_key[0] = tmp;
            find_bnode = brptr;
            idx = 0;
        }
    }

    //일단 지우고 보자
    if (!btree_delete_index(find_bnode, idx)) {
        fprintf(stderr, "[btree_delete] btree_delete_index(front) error\n");
        return NULL;
    }
    h->total_count--;
    //Underflow 발생! 재분배 --> 병합 시도
    while (find_bnode != h->root && find_bnode->count < MIN_KEY_SIZE) { //underflow
        //btree_find_index는 s가 해당하는 링크노드 자체를의미 
        //[긴급수정] --> Swap하고나서 못 찾는 경우가생김 shit
        int par_idx = -1;
        for (int i = 0 ; find_bnode->parent_ptr->node_ptr[i] ; i++)
            if (find_bnode->parent_ptr->node_ptr[i] == find_bnode) {
                par_idx = i;
                break;
            }
        //왼쪽 재분배 가능? 찾기
        if (par_idx-1 >= 0) {
            BNode* l_sib = find_bnode->parent_ptr->node_ptr[par_idx-1];
            if (l_sib->count > MIN_KEY_SIZE) { 
                Node* mv_node = l_sib->node_key[l_sib->count-1];
                Node* mv_parnode = find_bnode->parent_ptr->node_key[par_idx-1];
                BNode* mv_node_right = l_sib->node_ptr[l_sib->count]; //mv노드의 오른쪽 끝의 포인터 *중요!
                find_bnode->parent_ptr->node_key[par_idx-1] = mv_node;
                if (!btree_delete_index(l_sib, l_sib->count-1)) {
                    fprintf(stderr, "[btree_delete] btree_delete_index(left) error\n");
                    return NULL;
                }
                // 왼자식 -> 부모, 부모->현재노드
                if(!btree_insert_index(find_bnode, mv_parnode, 0)) {
                    fprintf(stderr, "[btree_delete] btree_insert_index(left) error\n");
                    return NULL;
                } 
                else {
                    find_bnode->node_ptr[0] = mv_node_right;  
                    if (mv_node_right)
                        mv_node_right->parent_ptr = find_bnode;
                    find_bnode = find_bnode->parent_ptr;
                    continue;
                }
                //포인터 옮기기 ...
            }
        }

        //오른쪽 재분배 가능? 찾기
        if (par_idx+1 <= find_bnode->parent_ptr->count) {
            BNode* r_sib = find_bnode->parent_ptr->node_ptr[par_idx+1];
            if (r_sib->count > MIN_KEY_SIZE) {
                Node* mv_node = r_sib->node_key[0];
                BNode* mv_node_left = r_sib->node_ptr[0]; //mv노드의 오른쪽 끝의 포인터 *중요!
                Node* mv_parnode = find_bnode->parent_ptr->node_key[par_idx];
                find_bnode->parent_ptr->node_key[par_idx] = mv_node;
                if (!btree_delete_index(r_sib, 0)) {
                    fprintf(stderr, "[btree_delete] btree_delete_index(right) error\n");
                    return NULL;
                }
                if(!btree_insert_index(find_bnode, mv_parnode, find_bnode->count)) {
                    fprintf(stderr, "[btree_delete] btree_insert_index(right) error\n");
                    return NULL;
                }
                else {
                    find_bnode->node_ptr[find_bnode->count] = mv_node_left;
                    if (mv_node_left)
                        mv_node_left->parent_ptr = find_bnode;
                    find_bnode = find_bnode->parent_ptr;
                    continue;
                }
            }
        }

            // 왼쪽부터 병합가능하면 병합시도
            BNode *pBnode = find_bnode->parent_ptr;
            int del_par_idx;
            if (par_idx - 1 >= 0) {
                // 왼쪽 노드에 왼+부모+나 병합해야함, --> 병합하고 idx를 지우고 한칸씩 땡겨야함
                // 그리고 리프라서 링크노드없음!
                del_par_idx = par_idx - 1;
                BNode *l_sib = pBnode->node_ptr[par_idx - 1];
                pBnode->node_ptr[par_idx] = l_sib;
                l_sib->node_key[l_sib->count++] = pBnode->node_key[del_par_idx];
                memcpy(&(l_sib->node_key[l_sib->count]), &(find_bnode->node_key[0]), find_bnode->count * sizeof(Node *));
                memcpy(&(l_sib->node_ptr[l_sib->count]), &(find_bnode->node_ptr[0]), (find_bnode->count+1) * sizeof(BNode *));
                for (int i = 0 ; find_bnode->node_ptr[i] ; i++) {
                    find_bnode->node_ptr[i]->parent_ptr = l_sib;
                }
                l_sib->count += find_bnode->count;
                memset(find_bnode, 0, sizeof(BNode));
                free(find_bnode);
                find_bnode = l_sib;
                // 부모 B노드에 연결
            }
            else if (par_idx + 1 <= find_bnode->count) {
                // 오른쪽 노드에 나+부모+왼 병합해야함
                del_par_idx = par_idx;
                BNode *r_sib = pBnode->node_ptr[par_idx + 1];
                pBnode->node_ptr[par_idx + 1] = find_bnode;
                find_bnode->node_key[find_bnode->count++] = pBnode->node_key[del_par_idx];
                memcpy(&(find_bnode->node_key[find_bnode->count]), &(r_sib->node_key[0]), r_sib->count * sizeof(Node *));
                memcpy(&(find_bnode->node_ptr[find_bnode->count]), &(r_sib->node_ptr[0]), (r_sib->count+1) * sizeof(BNode *));
                for (int i = 0 ; r_sib->node_ptr[i] ; i++) {
                    r_sib->node_ptr[i]->parent_ptr = find_bnode;
                }
                find_bnode->count += r_sib->count;
                memset(r_sib, 0, sizeof(BNode));
                free(r_sib);
            }

            // 병합이 끝났으니까 부모노드 정리
            if (!btree_delete_index(pBnode, del_par_idx)) {
                fprintf(stderr, "[btree_delete] btree_delete_index error\n");
                return NULL;
            }
            //루트가 병합되면서 ptr 하나만 남는 문제 해결
            if (pBnode->count == 0) {
                BNode* del_root = pBnode;
                pBnode = pBnode->node_ptr[0];
                h->root = pBnode;
                pBnode->parent_ptr = pBnode;
                free(del_root);
            }
            find_bnode = find_bnode->parent_ptr;
    }
    return ret_node;
}



int btree_append(Header* h, Node* n)
{
    BNode* root = h->root;
    //root처리 : total_count 가 KEY_COUNT + 1 에 도달할 때까지 그냥 삽입
    //Overflow가 넘어가면 그때부터 분할
    if (h->total_count < KEY_SIZE) {
        int idx = btree_find_index(root, n->path);
        if (!btree_insert_index(root, n, idx)) {
            fprintf(stderr, "btree_insert error\n");
            return false;
        }
        h->total_count++;
    }
    else
    {
        // 연결된 node_ptr 이 NULL 인지 체크 + NULL이 아니면 따라감. NULL이면 insert시도 --> 막힌다? Overflow처리
        int idx;
        BNode* ptr = root;
        while (true) {
            idx = btree_find_index(ptr, n->path);
            if (ptr->node_ptr[idx] == NULL)
                break;
            else
                ptr = ptr->node_ptr[idx];
        }
            //일단 노드삽입
        if (!btree_insert_index(ptr, n, idx)) {
            fprintf(stderr, "btree_append error\n");
            return false;
        }

        h->total_count++;
        do {
            //2차 체크
            if (ptr->count > KEY_SIZE) {
                BNode *pp = ptr->parent_ptr, *sibling = bnode(), *left = ptr;
                sibling->parent_ptr = ptr->parent_ptr;
                // 일단 Node하나 만들고 복사하고 중간노드 위로 올리는데 올리고보니까 KEY_SIZE+1 이다 이러면 또 분할 하는걸로.
                int mid = ptr->count / 2, offset = ptr->count - mid;
                Node *up = ptr->node_key[mid];
                memcpy(&(sibling->node_key[0]), &(ptr->node_key[mid + 1]), (offset - 1) * sizeof(Node*));
                memcpy(&(sibling->node_ptr[0]), &(ptr->node_ptr[mid + 1]), (offset) * sizeof(BNode*));
                memset(&(ptr->node_key[mid]), 0, (offset) * sizeof(Node*));
                memset(&(ptr->node_ptr[mid + 1]), 0, (offset) * sizeof(BNode*));
                ptr->count -= offset;
                sibling->count += offset - 1;
                for (int i = 0 ; sibling->node_ptr[i] ; i++) {
                    sibling->node_ptr[i]->parent_ptr = sibling;
                }
                //부모가 루트면 부모 갱신
                if (ptr == ptr->parent_ptr) {
                    BNode* new_root = bnode();
                    ptr->parent_ptr = sibling->parent_ptr = h->root = new_root;
                }
                ptr = ptr->parent_ptr; // 부모 위치에 이동
                int idx2 = btree_find_index(ptr, up->path);
                if (!btree_insert_index(ptr, up, idx2)) {
                    fprintf(stderr, "btree_append_error (in) \n");
                    return false;
                }
                ptr->node_ptr[idx2] = left;
                ptr->node_ptr[idx2+1] = sibling;
            }
        } while (ptr->count > KEY_SIZE);
    }
    return true;
}

Node* node(const char* p, int i)
{
    Node* newNode = (Node*)malloc(sizeof(Node));
    strncpy(newNode->path, p, PATH_SIZE);
    newNode->inode = i;
    return newNode;
}

BNode* bnode() 
{
    BNode* newBNode = (BNode*)malloc(sizeof(BNode));
    memset(newBNode->node_key, 0, KEY_SIZE * sizeof(int));
    memset(newBNode->node_ptr, (int)NULL, (KEY_SIZE+1) * sizeof(BNode*));
    newBNode->parent_ptr = newBNode;        //초기의 자신의 부모 포인터는 자신을 가리키게함
    newBNode->count = 0;
    return newBNode;
}

Header* header()
{
    Header *container = (Header*)malloc(sizeof(Header));
    container->root = bnode();
    container->total_count = 0;
    return container;
}

// 순회 전용임시큐
typedef struct bqueue{
    int depth;
    BNode* ptr;
    struct bqueue* next; 
}bqueue__;

//BFS 방식으로 노드 순회
void    btree_print(Header* h)
{
    int i,j;
    bqueue__ *head, *tail; 
    bqueue__ *saved_head;
    setbuf(stdout, NULL);
    if (h == NULL || h->root == NULL) {
        return;
    }
    bqueue__* init = (bqueue__*)malloc(sizeof(bqueue__));
    init->depth = 0;
    init->next = NULL;
    init->ptr = h->root;
    saved_head = head = tail = init;
     
    j = init->depth; 
    printf ("\n ----- started print b-tree ------\n");
    while (head != NULL) {
        BNode* ptr = head->ptr;
        for (i = 0 ; i < ptr->count ; i++) {
            if (j != head->depth) {
                printf("\n");
                j = head->depth;
            } 
            if (i == 0)
                printf("[");
            
            printf("%s", ptr->node_key[i]->path);
            
            if (i == ptr->count - 1)
                printf("] ");
            else
                printf(", ");
        }
        for (i = 0 ; ptr->node_ptr[i] != NULL ; i++) {
            bqueue__ *newNode = (bqueue__*)malloc(sizeof(bqueue__));
            newNode->depth = head->depth + 1;
            newNode->ptr = ptr->node_ptr[i];
            newNode->next = NULL;
            tail->next = newNode;
            tail = newNode;
        } 
        head = head->next;
    }
    printf ("\n ----- finished print b-tree ------\n");
    while (saved_head) {
        bqueue__* del;
        del = saved_head;
        saved_head = saved_head->next;
        free(del);
    }
}
