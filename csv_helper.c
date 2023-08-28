#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include "deque.h"

#ifndef NAME_SIZE
#define NAME_SIZE   1024
#endif

#ifndef BUF_SIZE    
#define BUF_SIZE    4096
#endif

#ifndef MAX_CELL
#define MAX_CELL    -1
#endif

typedef struct {
    int max_col;
    char line[BUF_SIZE];
}csv_line;

typedef struct ___io{
    FILE*   fp;
    List*   line;
    int     fd;
    char    filename[NAME_SIZE];
    int     max_row;
    int     max_col; 
}FILE_CSV;


List* file_list = init();
FILE_CSV*   convert_filecsv(Node* n);
FILE_CSV*   cfopen(const char* filename);
void        cfclose(FILE_CSV* fc);

char*       get_col_ptr(char* line, int col);
int         cell(FILE_CSV* f, int row, int col, char* value);
int         insert_row(FILE_CSV* f, char* value, int row=MAX_CELL);
int         insert_col(FILE_CSV* f, char* value, int col=MAX_CELL);

/** 출력함수 **/
void        print_csvinfo (FILE_CSV* c);
void        print_csvall ();

int         cflush(FILE_CSV* c);
int main(void)
{
    FILE_CSV* t = cfopen("passwd.csv");
    cell(t, 4, 3, "Hello");
    print_csvall();
    cflush(t);
    exit(0);
}

FILE_CSV* cfopen(const char* filename)
{
    FILE_CSV* new_f = (FILE_CSV*)malloc(sizeof(FILE_CSV));
    new_f->max_col = new_f->max_row = 0;
    new_f->line = init();
    strncpy(new_f->filename, filename, NAME_SIZE); 
    char* typecheck = strrchr(new_f->filename, '.');
    if (!strcmp(new_f->filename, ".csv")) {
        free(new_f);
        fprintf(stderr, "your file type is not csv file : %s\n", filename);
        return NULL;
    }
    

    if (access(filename, F_OK) == 0) {
        if ((new_f->fp = fopen(filename, "r+")) == NULL) {
            fprintf(stderr, "fopen error for %s\n", filename);
            free(new_f);
            return NULL;
        }
    }
    else {
        if ((new_f->fp = fopen(filename, "w+")) == NULL) {
            fprintf(stderr, "fopen error for %s\n", filename);
            free(new_f);
            return NULL;
        }
    }

    // max_row/col 체크 라인
    char tmp_buf [BUF_SIZE] = {0,};
    int length, col_count = 1, last_enter_check = 0;

    while(!feof(new_f->fp)) {
        fgets(tmp_buf, BUF_SIZE, new_f->fp);
        if (!strcmp(tmp_buf, "\n"))
            continue;

        csv_line* line = (csv_line*)malloc(sizeof(csv_line));
        append_tail_obj(new_f->line, set_obj(NULL, (void*)line));
        memset(line->line, 0, BUF_SIZE);
        line->max_col = 1;
        char* last_enter = tmp_buf + strlen(tmp_buf) - 1;
        if (*last_enter != '\n')
            *(++last_enter) = '\n';
        strncpy(line->line, tmp_buf, BUF_SIZE);
        for (char *s = tmp_buf ; *s != '\0' ;  s++) {
            if (*s == ',')
                line->max_col++;
        }
        
        new_f->max_row++;
        new_f->max_col = line->max_col > new_f->max_col ? line->max_col : new_f->max_col;
        if(feof(new_f->fp))
            break;
        memset(tmp_buf, 0, BUF_SIZE);
    }
    fseek(new_f->fp, 0, SEEK_SET); 
    append_tail_obj(file_list, set_obj(NULL, (void*)new_f));
    return new_f;
}

FILE_CSV* convert_filecsv(Node* n)
{
    if (n == NULL || n->obj == NULL)
        return NULL;
    return (FILE_CSV*)set_get(n->obj);
}

void      cfclose(FILE_CSV* fc)
{
    fclose(fc->fp);
    free(fc);
    fc->fd = fc->max_col = fc->max_row = -1;
    fc->fp = 0;
    memset(fc->filename, 0 , NAME_SIZE);
}

void        print_csvinfo (FILE_CSV* c)
{
    if(c == NULL) {
        printf("Empty csv\n");
        return;
    }
    printf("filename : %s\n", c->filename);
    printf("maxrow = %d, maxcol = %d\n", c->max_row, c->max_col);
}

void        print_csvall ()
{
    for (Node* s = file_list->head->next ; s->value != -1 ; s = s->next) {
        printf("--------\n");
        FILE_CSV* ret = (FILE_CSV*)set_get(s->obj);
        print_csvinfo(ret);
        Node* ss = ret->line->head;
        for (ss = ss->next ; ss->value != -1 ; ss = ss->next) {
            csv_line* val = (csv_line*)set_get(ss->obj);
            printf("[%d] %s\n", val->max_col, val->line);
        }
        printf("--------\n");
    }
}


char*       get_col_ptr(char* line, int col)
{
    if (line == NULL)
        return NULL;

    if (col == 1)
        return line;
    int find_col = col - 1;
    for (char* s = line ; *s != 0 ; s++) {
        if (*s == ',') {
            find_col--;
            if (find_col == 0)  { 
                return s+1; 
            }
        }
    }
    return NULL;
}

int csv_make_value (char* s, char* value)
{
    if (s == NULL) {
        s = value;
        return true;
    }

    char tmp_line[BUF_SIZE];
    char* e = strchr(s, ',');
    if (e == NULL) {
        char* enter_check = s + strlen(s) - 1;
        if (*enter_check == '\n') {
            *enter_check = 0;
        }
        memset(s, 0, strlen(s) + 1);
        strcpy(s, value);
        strcat(s, "\n");
        return true;
    }
    else {
        strncpy(tmp_line, e, strlen(e) + 1);
        strncpy(s, value, strlen(value) + 1); 
        strcat(s, tmp_line);
        return true;
    }
}

int         cell(FILE_CSV* f, int row, int col, char* value)
{
    if (f == NULL || f->fp == NULL) {
        return false;
    }
    // 최대 범위 설정
    if (row > f->max_row || row == MAX_CELL) {
        row = f->max_row+1;
    }
    if (col > f->max_col || col == MAX_CELL) {
        col = f->max_col+1;
    }

    int i;
    Node* n;

    // row 만큼 이동
    for (i = 1, n = f->line->head->next ; i < row ; i++, n = n->next);

    //n->value에 값 추가
    if (n->value == -1) { //최대범위
        csv_line* v = (csv_line*)malloc(sizeof(csv_line));
        v->max_col = col;
        f->max_col = v->max_col > f->max_col ? v->max_col : f->max_col;
        f->max_row++;
        for (int j = 1 ; j < col ; j++) {     
            strcat(v->line, ",");
        }
        strcat(v->line, value);
        strcat(v->line, "\n");
        //리스트에 연결
        append_tail_obj(f->line, set_obj(NULL, (void*)v));
        
        return true;
    }
    else {
        csv_line* v = (csv_line*)set_get(n->obj); 
        char* s = get_col_ptr(v->line, col);
        if (s == NULL) { //열 추가 (부분)
            char *enter = v->line + strlen(v->line) - 1;
            if (*enter == '\n') 
                *enter = 0;
            strcat (v->line, ",");
            strcat (v->line, value);
            strcat (v->line, "\n");
            v->max_col++;
            f->max_col = v->max_col > f->max_col ? v->max_col : f->max_col;
            return true;
        } else {
            csv_make_value(s, value);
            return true;
        }
    }
    return false;
}

int         insert_row(FILE_CSV* f, const char* value, int row)
{

    return 1;
}

int         insert_col(FILE_CSV* f, const char* value, int col)
{
    return 1;
}


int        cflush(FILE_CSV* c)
{
    if (c->fp == NULL)
        return false;

    fclose(c->fp);

    if((c->fp=fopen(c->filename, "w+")) == NULL) {
        fprintf (stderr, "cflush fopen error for %s\n", c->filename);
        return false;
    }


    for (Node* s = c->line->head->next ; s->value != -1 ; s = s->next) {
        csv_line* n = (csv_line*)set_get(s->obj);
        fprintf(c->fp, "%s", n->line);
    }
    printf("--- complete fflush : %s (row: %d, col: %d)\n", c->filename, c->max_row, c->max_col);
    return true;    
        
}