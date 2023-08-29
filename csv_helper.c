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
int         insert_row(FILE_CSV* f, List* li, int row=MAX_CELL, int insert=1);
int         insert_col(FILE_CSV* f, List* li, int col=MAX_CELL, int insert=1);

/** 출력함수 **/
void        print_csvinfo (FILE_CSV* c);
void        print_csvall ();

int         cflush(FILE_CSV* c);
int main(void)
{
    FILE_CSV* t = cfopen("passwd.csv");

    //cell(t, 4, 2, "Hello");
    List* v = init();
    append_tail(v, 10);
    append_tail(v, 20);
    append_tail(v, 30);
    insert_col(t, v, 3, true);
    print_csvall();
    cflush(t);
    exit(0);
}

/**
 * filename으로 csv파일 스트림 FILE_CSV를 생성하는 함수, 
 * 해당 csv파일의 라인별로 출력하여 FILE_CSV->line 리스트로 저장함
 * @param   filename    :csv 파일 파일 경로
 * @return  파일 스트림 FILE_CSV 리턴
*/
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
    
    //파일 존재유무 체크 -> r+, w+ 결정
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
        if (!strcmp(tmp_buf, "\n") || !strlen(tmp_buf))
            continue;

        //라인 읽어들이면서 연결
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

/**
 * Node n 을 csv파일스트림 FILE_CSV로 바꿔주는 함수
 * @param n : node.h 의  node
*/
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

/**
 *  csv파일의 정보를 출력하는 함수
 *  @param c    : FILE_CSV 파일 스트림
*/
void        print_csvinfo (FILE_CSV* c)
{
    if(c == NULL) {
        printf("Empty csv\n");
        return;
    }
    printf("filename : %s\n", c->filename);
    printf("maxrow = %d, maxcol = %d\n", c->max_row, c->max_col);
    Node* ss = c->line->head;
    for (ss = ss->next ; ss->value != -1 ; ss = ss->next) {
        csv_line* val = (csv_line*)set_get(ss->obj);
        printf("[%d] %s", val->max_col, val->line);
    }
}

/**
 * 전역변수 file_list 에 연결된 파일 리스트의 모든 파일을 출력하는 함수
*/
void        print_csvall ()
{
    for (Node* s = file_list->head->next ; s->value != -1 ; s = s->next) {
        printf("--------\n");
        FILE_CSV* ret = (FILE_CSV*)set_get(s->obj);
        print_csvinfo(ret);
        printf("--------\n");
    }
}


/**
 * csv 행 위치에서 열 위치 찾는 함수
 * @author Junhyeong(230829)
 * @param line  : 행 문자열
 * @param col   : 찾으려는 열 번호
 * @return 해당 열 위치 값이 존재하면 line에서 열 위치, 없으면 NULL
*/
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

/**
 * in_cell 안에서 cell 데이터를 추가하는 함수.
 * @param s     : 해당 행의 문자열
 * @param value : 삽입하거나 수정할 문자열
 * @return 성공시 true, 실패시 false;
*/
int csv_make_value (char* s, char* value, int insert)
{
    if (s == NULL) {
        s = value;
        return true;
    }

    char tmp_line[BUF_SIZE] = {0, };
    char* e = strchr(s, ',');
    if (e == NULL) {
        char* enter_check = s + strlen(s) - 1;
        if (*enter_check == '\n') {
            *enter_check = 0;
        }
        if (insert) 
            sprintf(tmp_line, ",%s", s);
        memset(s, 0, strlen(s) + 1);
        strcat(s, value);
        strcat(s, tmp_line);
        strcat(s, "\n");
        return true;
    }
    else {
        if (insert) 
            snprintf(tmp_line, BUF_SIZE, ",%s", s);  
        else
            strncpy(tmp_line, e, strlen(e) + 1);
        strncpy(s, value, strlen(value) + 1); 
        strcat(s, tmp_line);
        return true;
    }
    return false;
}

/**
 * 셀에 데이터를 넣기위한 내부 함수
 *  @param f       : FILE_CSV 파일 스트림
 *  @param col     : 열 번호 (최대값 : MAX_CELL)
 *  @param value   : 저장할 값.
 *  @param n       : node.h 의 Node* (저장된 csv_line* obj 를 얻기 위함)
 *  @param insert  : 1 : 삽입, 0 : 수정
 * 
*/
int         in_cell(FILE_CSV* f, int col, char* value, Node* n, int insert)
{
    int new_transaction = 0, j = 1, over_col=0;
    csv_line* v;
    //n->value에 값 추가
    if (n->value == -1) { // 행 추가케이스
        new_transaction = 1; // 아래에서 처리
        //csv 라인 추가하고
        v = (csv_line*)malloc(sizeof(csv_line)); 
        f->max_row++;
        //리스트에 연결
        append_tail_obj(f->line, set_obj(NULL, (void*)v));    
    }
    else {
        v = (csv_line*)set_get(n->obj); 
        char* s = get_col_ptr(v->line, col);
        if (s == NULL) { // ',' 가 없는 경우 열이 1개 뿐인 경우
            new_transaction = over_col = 1;
            for (char* c = v->line ; *c != 0 ; c++)
                if (*c == ',') j++;
        } //아래에서 처리
        else { // 다중 열을 가지고 있는 csv 파일
            csv_make_value(s, value, insert);
            v->max_col = insert ? v->max_col + 1 : v->max_col;
            f->max_col = v->max_col > f->max_col ? v->max_col : f->max_col;
            return true;
        }
    }

    // col 위치에 열이 없는 경우 처리 --> 수정/삽입 동일하게 작동
    if (new_transaction) {
        // ,(열 추가) + line 열 개수 갱신 + 데이터 추가(삽입/수정)
        char *enter = v->line + strlen(v->line) - 1;
        if (strlen(v->line) && *enter == '\n') 
            *enter = 0;
        //열 개수 결정
        v->max_col = (over_col && col <= v->max_col) ? v->max_col + 1 : col;
        f->max_col = v->max_col > f->max_col ? v->max_col : f->max_col;
        for (; j < col ; j++) {     
            strcat(v->line, ",");
        }
        strcat(v->line, value);
        strcat(v->line, "\n");        
        return true;
    }
    return false;
}

/**
 *  csv파일에 자유로운 셀 위치에 (row,col) 데이터를저장 (csv 셀 함수)
 *  @param f       : FILE_CSV 파일 스트림
 *  @param row     : 행 번호 (최대값 : MAX_CELL)
 *  @param col     : 열 번호 (최대값 : MAX_CELL)
 *  @param value   : 저장할 값.
*/
int         cell(FILE_CSV* f, int row, int col, char* value)
{
    if (f == NULL || f->fp == NULL || row < -1 || col < -1) {
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

    if(!in_cell(f, col, value, n, false)) {
        fprintf(stderr, "cell error for %s : [%s]\n", f->filename, value);
        return false;
    }

    return true;
}

/**
 * obj로부터 char* 경로를 추출하는 insert_row/col 임시함수
 * @param obj : deqeue의 Node  (원소)
 * @return 성공시 문자열 포인터, 에러시 NULL
*/
char *csv_returnNode(Node *n) {

    char *value;
    if (n == NULL)
        return NULL;

    if (n->obj) {
        value = (char *)set_get(n->obj);
    }
    else {
        if (!strlen(n->path))
            value = n->path;
        else {
            value = n->path + strlen(n->path);
            sprintf(value, "%d", n->value);
        }
    }
    return value;
}

/**
 *  csv 파일에 행 단위로 데이터 저장 : 
 *  @param f       : FILE_CSV 파일 스트림
 *  @param li      : 저장할 값(리스트: deque)
 *  @param row     : 저장할 행 위치
 *  @param insert    : 모드(0: 수정, 1: 삽입) -> default : 삽입
 *  @return 성공시 true, 실패시 false
*/
int         insert_row(FILE_CSV* f, List* li, int row, int insert)
{
    if (f == NULL || li == NULL || li->count < 1 || row < -1 || (insert != 1 && insert != 0)) {
        fprintf(stderr, "insert_row error\n");
        return false;
    }

    int new_row = insert;
    if (row > f->max_row || row == MAX_CELL)  {
        row = f->max_row + 1;
        new_row = true;
    }


    //삽입 모드 시 빈 obj를 추가하고 in_cell 호출 -> insert 모드로 호출 주의!
    if (new_row) {     
        csv_line* new_line = (csv_line*)malloc(sizeof(csv_line));
        if(!append_obj(f->line, row-1, set_obj(NULL, (void*)new_line)))  {
            append_tail_obj(f->line, set_obj(NULL, (void*)new_line));
        } 
        f->max_row++;
    }
    int i;
    Node* s;
    for (i = 1, s = f->line->head->next ; i < row ; s = s->next, i++);
    
    if (s == NULL) {
        fprintf(stderr, "insert_row error(can't find row) : %s\n", f->filename);
        return false;
    }
    csv_line* cur_line = (csv_line*)set_get(s->obj);
    cur_line->max_col = 0;
    memset(cur_line->line, 0, BUF_SIZE);
    //데이터 추가, 추가라인
    for (s = li->head->next ; strcmp(s->path, "dummy_tail") ; s = s->next) {
        char* value = csv_returnNode(s);
        strcat(cur_line->line, value);
        cur_line->max_col++;
        if (strcmp(s->next->path, "dummy_tail"))
            strcat(cur_line->line, ",");
        else
            strcat(cur_line->line, "\n");
    }

    f->max_col = cur_line->max_col > f->max_col ? cur_line->max_col : f->max_col;
    //만약 list 크기가 부족하면 마지막 값 복사해서 저장
    return true;
}


/**
 *  csv 파일에 열 단위로 데이터 저장 : 
 *  @category 출력
 *  @param f       : FILE_CSV 파일 스트림
 *  @param li      : 저장할 값(리스트: deque)
 *  @param col     : 저장할 열 위치
 *  @param insert    : 모드(0: 수정, 1: 삽입) -> default : 삽입
 *  @return 성공 시 true, 실패시 false
*/
int         insert_col(FILE_CSV* f, List* li, int col, int insert)
{
    if (f == NULL || li == NULL || col < -1 || (insert != true && insert != false)) {
        fprintf(stderr, "insert_col error\n");
        return false;
    }

    if (col > f->max_col || col == MAX_CELL) {
        col = f->max_col++;
    }

    Node *n, *v = li->head->next;
    for (n = f->line->head->next ; n->value != -1 ; n = n->next) {
        char* val = csv_returnNode(v); 
        if(!in_cell(f, col, val, n, insert)) {
            fprintf(stderr, "insert_col append line error for %s [%s]\n", f->filename, val);
            return false;
        }

        if (strcmp(v->next->path, "dummy_tail")) 
            v = v->next; 
    }    
    return true;
}

/**
 *  excel 파일에 변경된 트렌젝션 저장
 *  @param  c   : FILE_CSV 파일 스트림
 *  @return 성공 시 true, 실패시 false
*/
int        cflush(FILE_CSV* c)
{
    if (c->fp == NULL)
        return false;

    //c->fp close --> re open
    fclose(c->fp);

    if((c->fp=fopen(c->filename, "w+")) == NULL) {
        fprintf (stderr, "cflush fopen error for %s\n", c->filename);
        return false;
    }
    c->fd = fileno(c->fp);

    //Data Print to File
    for (Node* s = c->line->head->next ; s->value != -1 ; s = s->next) {
        csv_line* n = (csv_line*)set_get(s->obj);
        fprintf(c->fp, "%s", n->line);
    }
    printf("--- complete fflush : %s (row: %d, col: %d)\n", c->filename, c->max_row, c->max_col);
    return true;     
}