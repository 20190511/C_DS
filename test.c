#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "deque.h"
#include "btree.h"
#include "heap.h"

void test1()
{
    List* li = init(NULL);    
    append_head(li, 10);
    append_head(li, 20);
    append_head(li, 30);
    append_head(li, 40);
    printn(li);

    //Node* t = deque(li, -2);
    append(li, -3, 10);
    printn(li);

    Node* pops = pop(li, 2);
    printn(li);
    printf("tail : %d\n", li->tail->prev->value);
}

void test2()
{
    List* li = init(NULL);
    time_t t = time(NULL);
    int idx, i;
    srand((unsigned int)t);

    //인덱스 무작위 배치
    for (i = 1 ; i <= 10 ; i++) {
        idx = rand() % i;
        append(li, idx, i);
    }

    printn(li);

    //인덱스 무작위 삭제
    for (i = 10 ; i > 0 ; i--) {
        idx = -(rand() % i);
        Node* delNode = pop(li, idx);
        printf("del index = %d, delNode >> [%s] %d\n", idx, delNode->path, delNode->value);
        printn(li);
    }

    //삭제 에러 뜨는지 확인
    pop(li, 1);
    return;
}

//삭제 잘되는지 확인
void test3()
{
    List* li = init(NULL);
    time_t t = time(NULL);
    int idx, i;
    srand((unsigned int)t);

    for (i = 1 ; i <= 10 ; i++) {
        idx = rand() % i;
        append(li, idx, i);
    }

    printn(li);
    Node* pops = pop(li, 3);
    printn(li);
    freeList(li);
    printn(li); 
}

void test4()
{
    List li;
    init(&li);
    time_t t = time(NULL);
    int idx, i;
    srand((unsigned int)t);

    for (i = 1 ; i <= 10 ; i++) {
        idx = rand() % i;
        append(&li, idx, i);
    }

    printn(&li);
    Node* pops = pop(&li, 3);
    printn(&li);
    freeList(&li);
    printn(&li); 
}

void test5()
{
    const char* test_list[] = {"junhyeong", "ayax", "danke", "brae", "ab",
    "abab", "alpha", "alke", "ssuso", "sexo", "pento", "donsu", "dulbt",
    "slayer", "LGG", "targetNum", "sti", "truman", "go2", "go", "omg", "oska",
    "omsg", "oriental", "ytsiof", "kusk", "kok", "sto", "toj", "ojkl", "baba",
    "yeto", "olle", "strwa", "nai", "kai", "stai", "stoi", "totoi", "minoi",
    "yougin", "gangnam", "toili", "loiacp", "llo", "you", "dfk", "ksie", "kdfue",
    "dkfie", "dkfeij", "dkjfsel", "askdfjlsd", "klkvjl", "kxlcvkc", "zlkxcjlkzcj",
    "sldkfjlsdkfj", "kvcjl", "lep", "strr", "kuu", "desire", "view", "ill", "far",
    "away", "galaxy", "genre", "fantasy", "big", "stage", "true", "who", "deep",
    "load", "fly", "glri", "girl", "fire", "ahh"};

    
    Header* btree = header();
    for (int i = 0 ; i < (sizeof(test_list) / sizeof(test_list[0])) ; i++) {
        btree_append(btree, node(test_list[i], 30));
        btree_print(btree);
    }
    btree_print(btree);
    for (int i = 0 ; i < (sizeof(test_list) / sizeof(test_list[0])) ; i++) {
        printf("\n\n---> delete : %s\n", test_list[i]);
        if (!strcmp("sto", test_list[i]))
            printf("test\n");
        btree_delete(btree, test_list[i]);
        btree_print(btree);
    }
}

void test6()
{
    Heap* he = heap();
    Node* tmp;
    for (int i = 0 ; i < 16 ; i++) {
        min_heap_append(he, i, NULL);
    }
    heap_print(he);
    heap_clear(he);
    /*
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
    */
}

int main(void)
{
#ifdef NON_PRINT_BLOCK
    test1();    // Deque 에제 1
    test2();    // Deque 에제 2
    test3();    // Deque 예제 3
    test4();    // Deque 예제 4
#endif
    test5();    // B_Tree 예제
    test6();    // Heap 예제
    exit(0);
}