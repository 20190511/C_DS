#ifndef OBJ_H
#define OBJ_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef OBJ_TYPE
#define OJB_TYPE
#define OBJ_INT         1
#define OBJ_DOUBLE      2
#define OBJ_CHAR        3
#define OBJ_VOID        4
#endif

#ifndef true
#define true    1
#define false   0
#endif

#ifndef OBJ_SETTING
#define OBJ_SETTING
#endif

typedef struct _obj{
    void*   ptr;
    int     type;
    int     alloc_check;
}Obj;

typedef struct __n { 
    struct __n  *next;
    Obj         *obj;    
}_n;


Obj*    set_obj     (Obj** obj_ptr, void* value, int mode=OBJ_VOID);
Obj*    set_int     (Obj** old_ptr, int i);         //int i 로 Obj 하나 생성 (이전 값 존재시 할당해제하고 생성)
Obj*    set_double  (Obj** old_ptr, double d);      //double 로 Obj 하나 생성
Obj*    set_char    (Obj** old_ptr, char c);        //char 로 Obj 하나  
void*   set_get     (Obj* obj);        // obj value를 리턴받음

void    set_free    (Obj* obj);         //할당 해제함수.
int  printo (Obj* obj);

#ifdef DEBUG
int main(void)
{
    Obj* o = set_int(NULL, 50);
    o = set_int(&o, 0);
    printo(o);
    o = set_obj(&o, (void*)"Hello World", OBJ_CHAR);
    char* v = (char*)set_get(o);
    typedef struct {
        int v;
        int k;
    }data ;
    data a = {1,2};
    o = set_obj(&o, (void*)&a, OBJ_VOID);

    data *b = (data*)set_get(o);
    printf("%d, %d\n", b->k, b->v);
    
    printf("%s\n", v);
    //printo(o);
    exit(0);
}
#endif

void*   set_get     (Obj* obj)        // obj value를 리턴받음
{
    if (obj == NULL)
        return NULL;
    return obj->ptr;
}


Obj* init_obj()
{
    Obj* n = (Obj*)malloc(sizeof(Obj));
    n->ptr = NULL;
    n->type = n->alloc_check = 0;
    return n;
}

Obj*    set_int(Obj** old_ptr, int i)
{
    int* n = (int*)malloc(sizeof(int));
    *n = i;
    Obj* ret = set_obj(old_ptr, (void*)n, OBJ_INT);
    ret->alloc_check = true;
    return ret;
}
Obj* set_double(Obj** old_ptr, double d)
{
    double* n = (double*)malloc(sizeof(double));
    *n = d;
    Obj* ret = set_obj(old_ptr, (void*)n, OBJ_DOUBLE);
    ret->alloc_check = true;
    return ret;
}
Obj*   set_char (Obj** old_ptr, char c) 
{
    char* n = (char*)malloc(sizeof(char));
    *n = c;
    Obj* ret = set_obj(old_ptr, (void*)n, OBJ_CHAR);
    ret->alloc_check = true;
    return ret;
}

Obj* set_obj(Obj** obj_ptr, void* value, int mode)
{
    int i = 0;
    Obj* obj;
    if (obj_ptr == NULL) 
        obj = init_obj();
    else
        Obj* obj = *obj_ptr;
    if(obj == NULL) {    
        obj = *obj_ptr = init_obj();
    }
    else if (obj->alloc_check){
        free(obj->ptr);
    }
    obj->type = mode;
    obj->ptr = value;
    return obj;
}

int  printo (Obj* obj)
{
    if (!obj) {
        return false;
    }
    switch (obj->type) {
    case OBJ_INT:
        //printf("int array ptr : %p, ", (int *)obj->ptr);
        printf("int value : %d", *((int *)obj->ptr));
        break;
    case OBJ_DOUBLE:
        //printf("double array ptr : %p, ", (double *)obj->ptr);
        printf("double value : %f", *((double *)obj->ptr));
        break;
    case OBJ_CHAR:
        //printf("char array ptr : %p, ", (char *)obj->ptr);
        printf("char value : %c", *((char *)obj->ptr));
        break;
    case OBJ_VOID:
        printf("void ptr : %p, ", obj->ptr);
        break;
    }
    return true;
}

void    set_free    (Obj* obj)
{
    if (obj != NULL) {
        if (obj->alloc_check)
            free(obj->ptr);
        obj->type = obj->alloc_check = 0;
        obj->ptr = NULL;
        free(obj);
        obj->type = obj->alloc_check = 0;
        obj->ptr = NULL;
    }
}

#endif