# C_DS
- DataStructure For C (Cpp Ds) --> C 언어 전용 자료구조(data Structure) 헤더 
- 모든 헤더는 동시에 사용 가능하며, Node.h 의 노드를 공유하고 있는 형태로 구현됨  
- [_동작예제파일 (test.c)_](https://github.com/20190511/C_DS/blob/main/test.c)
- [Deque를 이용한 (c,h파일) 파일 복사 메커니즘]()

## [_1. Deque.h (데크)_](https://github.com/20190511/C_DS/blob/main/deque.h)

```C
/** Init function*/
List* init();
Node* node (int data);

// For Insert Method
void append_tail(List* l, int data, char* name = "NULL");       // Deque 앞에 연결
void append_head(List* l, int data, char* name = "NULL");       // Deque 끝에 연결
int append(List* l, int idx, int data, char* name = "NULL");    // Deque index 요소 연결

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
```

## [_2. Btree.h (비트리 자료구조)_](https://github.com/20190511/C_DS/blob/main/btree.h)
```C
Node*   node(const char* p, int i);                          // 노드 생성자 (내부실행파일에서 사용)
Header* header();                                            // B트리 헤더 생성자
BNode*  bnode();                                             // B트리 전용 노드 (내부실행파일에서 사용)
Node*   btree_search(Header* h, const char* f, BNode** b);   // B트리 탐색 함수
void    btree_print(Header* h);                              // B트리 출력 함수 (트리형태로 출력, Queue 이용 + BFS 순회)
int     btree_append(Header* h, Node* n);                    // B트리 데이터 추가 함수
Node*   btree_delete(Header* h, const char* s);              // B트리 삭제 함수.
```

## [_3. heap.h (힙 자료구조)_](https://github.com/20190511/C_DS/blob/main/heap.h)
```C
/** 성공 시 true(0), 에러시 false(0)*/
int     max_heap_append(Heap* h, int v, const char* name);  // 최소 Heap 추가, name이 NULL 이면 Heap 생성순서대로 name 설정
int     min_heap_append(Heap* h, int v, const char* name);  // 최대 heap 추가, name이 NULL 이면 Heap 생성순서대로 name 설정
Node*   min_heap_pop (Heap* h);                             // 최소 Heap Pop -> Pop 한 최소 Key값 노드 리턴
Node*   max_heap_pop (Heap* h);                             // 최대 Heap Pop -> Pop 한 최대 Key값 노드 리턴
Heap*   heap();                                             // Heap 생성자
int     heap_clear(Heap* h);                                // 재귀호출이 아닌 while 후위탐색으로 전 노드 삭제
void    heap_print(Heap* h);                                // heap 자료구조 전체 출력 (트리형태로 출력 : 임시 큐 구현해서 사용 (BFS))
```

