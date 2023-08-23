#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "deque.h"

#ifndef OBJ_TYPE
#define OJB_TYPE
#define OBJ_INT         1
#define OBJ_DOUBLE      2
#define OBJ_CHAR        3
#endif

#ifndef OBJ_SETTING
#define OBJ_SETTING
#define OBJ_DEFAULT_ARR_SIZE    10
#endif
typedef struct _obj{
    void*   ptr[OBJ_DEFAULT_ARR_SIZE];
    int     type[OBJ_DEFAULT_ARR_SIZE];
}Obj;

typedef struct __n { 
    struct __n  *next;
    Obj         *obj;    
}_n;


Obj* set_obj(Obj** obj_ptr, void* value, int mode);
int*    set_int(int i);
double* set_double(double d);
char*   set_char (char c);
void*   set_index(Obj* obj, int idx);

int  printo (Obj* obj);

#ifndef DEBUG
#define DEBUG
int main(void)
{
    Obj* o = NULL;
    set_obj(&o, (void*)set_int(40), OBJ_INT);
    set_obj(&o, (void*)set_int(30), OBJ_INT);
    set_obj(&o, (void*)set_double(3.14), OBJ_DOUBLE);
    set_obj(&o, (void*)set_char('c'), OBJ_CHAR);
    printo(o);
    double a = 2.5;
    double b = *((double*)o->ptr[1]);
    printf("%f\n", a+b);
    printf("%s\n", "Hello");
    exit(0);
}
#endif

Obj* init_obj()
{

    Obj* n = (Obj*)malloc(sizeof(Obj));
    memset(n->ptr, 0, sizeof(void*) * OBJ_DEFAULT_ARR_SIZE); 
    memset(n->type, 0, sizeof(int)  * OBJ_DEFAULT_ARR_SIZE);
    return n;
}

int*    set_int(int i)
{
    int* n = (int*)malloc(sizeof(int));
    *n = i;
    return n;
}
double* set_double(double d)
{
    double* n = (double*)malloc(sizeof(double));
    *n = d;
    return n;
}
char*   set_char (char c) 
{
    char* n = (char*)malloc(sizeof(char));
    *n = c;
    return n;
}

Obj* set_obj(Obj** obj_ptr, void* value, int mode)
{
    int i = 0;
    if (obj_ptr == NULL)
        return NULL;
    Obj* obj = *obj_ptr;
    if(obj == NULL) {    
        obj = *obj_ptr = init_obj();
    }

    for (i = 0 ; obj->type[i] ; i++)
        ; 
    obj->type[i] = mode;
    obj->ptr[i] = value;
    return obj;
}

int  printo (Obj* obj)
{
    if (!obj) {
        return false;
    }
    for (int i = 0 ; obj->type[i] ; i++) {
        switch(obj->type[i]) {
            case OBJ_INT:
                printf("int array ptr : %p, ", (int*)obj->ptr[i]);
                printf("value : %d\n", *((int*)obj->ptr[i]));
                break;
            case OBJ_DOUBLE:
                printf("double array ptr : %p, ", (double*)obj->ptr[i]);
                printf("value : %f\n", *((double*)obj->ptr[i]));
                break;
            case OBJ_CHAR:
                printf("char array ptr : %p, ", (char*)obj->ptr[i]);
                printf("value : %c\n", *((char*)obj->ptr[i]));
                break;
        }
    }
    return true;
}