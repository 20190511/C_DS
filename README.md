# C_DS
DataStructure For C (Cpp Ds) --> C 언어 전용 자료구조(data Structure) 헤더 

## 1. Deque.h

1. Deque.h
- 메소드
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
