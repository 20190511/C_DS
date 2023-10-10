#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include "deque.h"

#define IGNORE_TXT  "ignorelist.txt"

List* updateIgnorelist();
void cleaner(char* path);
char* pathConverter(char* path);
int main(int argc ,char* argv[])
{
    if (argc != 2) {
        fprintf(stderr, "usage : %s <ERASE_DIRNAME>\n", argv[0]);
        exit(1);
    }
    printf("Erase Dir name : %s\n", argv[1]);
    char* path = pathConverter(argv[1]);
    cleaner(path);
    free(path);
    exit(0);
}

char* pathConverter(char* path)
{
    List* li = init(NULL);
    char converterPath[1024] = {0,};
    if (path[0] == '~')
        sprintf(converterPath, "%s/%s", getenv("HOME"), path+2);
    else if (path[0] != '/')
        sprintf(converterPath, "%s/%s", getcwd(NULL, 0), path+2);
    else
        strcpy(converterPath, path);

    char *tok = strtok(converterPath, "/");
    while(tok) {
        if (!strcmp(tok, ".")) {
            tok = strtok(NULL, "/");
            continue;
        }
        else if (!strcmp(tok, ".."))  {
            tok = strtok(NULL, "/");
            pop_rear(li);
            continue;
        }

        append_tail(li, true, tok);
        tok = strtok(NULL, "/");
    }
    if (li->count == 0) {
        fprintf(stderr, "your path is error : %s\n", path);
        return NULL;
    }
    strcpy(converterPath, "/");
    for (Node* s = li->head->next ; s->value == true ; s = s->next) {
        strcat(converterPath, s->path);
        strcat(converterPath, "/");
    }
    if (strcmp(converterPath, "/"))
        *(converterPath + strlen(converterPath) - 1) = '\0';

    char* erase_path = "/SHELL=/bin/bash", *erase_p = strstr(converterPath, erase_path);
    if (erase_p)
        *erase_p = '\0';
    char* retPath = (char*)malloc(strlen(converterPath) + 1);
    strcpy(retPath, converterPath);
    freeList(li);
    return retPath;
}

List* updateIgnorelist()
{
    FILE* fp;
    List* li = init(NULL);
    char filename[1024] = {0,};
    if (!(fp = fopen(IGNORE_TXT, "r"))) {
        fprintf(stderr, "no file : %s\n", IGNORE_TXT);
        return NULL;
    }

    while(!feof(fp)){
        fgets(filename, 1024, fp);
        if (*(filename + strlen(filename) - 1) == '\n')
            *(filename + strlen(filename) - 1) = '\0';
        append_tail(li, true, filename);
    }
    return li;  
}

void cleaner(char* path)
{
    char pathS[1024], *ptr;
    strcpy(pathS, path);
    ptr = pathS + strlen(pathS);
    *ptr++ = '/';
    *ptr = '\0';

    struct dirent *dt;
    DIR* dir;
    List* li = init(NULL); 
    if (!(dir = opendir(pathS))) {
        fprintf(stderr, "opendir error for %s\n", pathS);
        return;
    }

    while((dt = readdir(dir))) {
        if (strcmp(dt->d_name, ".") && strcmp(dt->d_name, "..")) {
            char *tail = strrchr(dt->d_name, '.');
            if (tail == NULL) {
                strcpy(pathS, dt->d_name);
                append_tail(li, true, pathS);
            }
        }
    }

    List* ignoList = updateIgnorelist();
    int notErasedFlag = false;
    for (Node* s = li->head->next ; s->value != -1 ; s = s->next) {
        notErasedFlag = false;
        for (Node *c = ignoList->head->next ; c->value != -1 ; c = c->next) {
            if (!strcmp(s->path, c->path)) {
                printf("cannot be erased : %s\n", s->path);
                notErasedFlag = true;
                break;
            }
        }

        if (!notErasedFlag) {
            printf("erase : %s\n", s->path);
            unlink(s->path); 
        }
    }

    freeList(ignoList);
    freeList(li);
    //append_tail(li, true, );
}