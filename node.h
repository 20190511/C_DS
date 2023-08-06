#ifndef NODE_H
#define NODE_H

#ifndef PATH_SIZE
#define PATH_SIZE        256
#endif

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
#endif