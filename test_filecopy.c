#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "deque.h"

#ifndef MAX_PATH_SIZE
#define MAX_PATH_SIZE	1024
#endif

void search (List* li, const char* path);		// path 경로에 있는 전 파일 li Deque 로 복사.
void take_ch(List* li, const char* dir_path);   // li Deque에 있는 파일 중 .c, .h 파일만 복사하는 함수
void fcopy(const char* from, const char* to);   // File 복사함수

int main(void)
{
	List* l = init(NULL);
	char *cwd = getcwd(NULL,0);
	search(l, "/home/junhyeong/ds");
	Node* n = deque(l, -2);
	//printn(l);
	printf("%s\n", n->path);

	take_ch(l, "/home/junhyeong/ds/file");	

	exit(0);
}

/** file 복사 함수
 * 	from : 복사할 파일
 * 	to : 경로
*/
void fcopy(const char* from, const char* to)
{
	if (!strcmp(from, to)) {
		return;
	}
	struct stat st;
	char copy[MAX_PATH_SIZE];
	strncpy(copy, to, MAX_PATH_SIZE);
	char* mother = strrchr(copy, '/');
	if (mother == NULL) {
		fprintf(stderr, "%s is not directory\n", copy);
		return;
	}

	if (access(from, F_OK) != 0) {
		fprintf(stderr, "file copy error for %s\n", from);
		return;
	}

	if (stat(from, &st) != 0) {
		fprintf(stderr, "stat error for %s\n", from);
		return;
	}	
	if (S_ISDIR(st.st_mode)) {
		fprintf(stderr, "file %s is directory file {can't copy}", from);
		return;
	}

	*mother = 0;
	if (stat(copy, &st) != 0) {
		fprintf(stderr, "stat error for %s\n", copy);
		return;
	}	
	if (!S_ISDIR(st.st_mode)) {
		fprintf(stderr, "file %s is not directory file", copy);
		return;
	}


	int fd_from, fd_to, rd_count;
	char buf[4096];

	if ((fd_from = open(from, O_RDONLY)) < 0) {
		fprintf(stderr,  "open error for %s\n", from);
		return;
	}
	
	if ((fd_to = open(to, O_CREAT | O_WRONLY | O_TRUNC, 0644)) < 0) {
		fprintf(stderr,  "open error for %s\n", to);
		return;
	}
	
	while ((rd_count = read(fd_from, buf, 4096)) > 0) {
		if (write(fd_to, buf, rd_count) != rd_count) {
			fprintf(stderr, "write error : %s\n", to);
			return;
		}
	}
	
	close(fd_from);
	close(fd_to);
}

/** .c, .h 파일만 추출하여 dir_path 경로 디렉토리로 복사하는 함수
 *  List* li	   : 경로 연결리스트(Deque)
 *  char* dir_path : 복사할 디렉토리 경로
*/
void take_ch(List* li, const char* dir_path)
{
	if (li == NULL || li->count == 0) {
		fprintf(stderr, "[Dir Error] : Empty Directory\n");
		return;
	}

	List* c_h = init(NULL);
	for (Node* s = li->head ; s != NULL ; s = s->next) {
		char *f_type;
		if (!(f_type = strrchr(s->path, '.'))) 
			continue;
		if (!strcmp(f_type, ".c") || !strcmp(f_type, ".h")) {
			append_tail(c_h, c_h->count+1, s->path);
		}
	}

	if (c_h->count == 0) {
		printf("This directory has not c, h file\n");
		return;	
	}	

	struct stat st;
	if (access(dir_path, F_OK) != 0) {
		//Direcotory 생성
		mkdir(dir_path, 0755);
		
	} else {
		if (stat(dir_path, &st) != 0){ 
			fprintf(stderr, "stat access error for %s\n", dir_path);
			return;
		}	
		if (!S_ISDIR(st.st_mode)) {	
			unlink(dir_path);
			mkdir(dir_path, 0755);
		}
	}

	//파일 복사
	char dirs[MAX_PATH_SIZE];
	char *ptr;
	Node* s;
	int f_cnt;

	strncpy(dirs, dir_path, MAX_PATH_SIZE);
	ptr = dirs + strlen(dirs);
	*ptr++ = '/';
	*ptr = 0;
	printn(c_h);
	for (f_cnt = 1, s = c_h->head->next ; s != NULL ; s = s->next, f_cnt++) {
		if (s->value == -1)
			break;
		char* only_file = strrchr(s->path, '/');
		if (only_file == NULL)
			continue;
		strcpy(ptr, only_file+1);
		printf("[%d] copy %s --> %s\n", f_cnt, s->path, dirs);
		fcopy(s->path, dirs);	
	}

	freeList(c_h);
	return;
}

/** path 경로에 있는 전 파일을 Deque에 연결
 *  -> li Deque에 연결함.
*/
void search (List* li, const char* path)
{
	DIR* dp;
	struct dirent *dt;
	struct stat st;
	char cur_path[MAX_PATH_SIZE];
	char* ptr;

	if (stat(path, &st) != 0) {
		fprintf(stderr, "stat error for %s\n", path);
		exit(1);
	}
	
	if (!S_ISDIR(st.st_mode)) {
		fprintf(stderr, "not directory error for %s\n", path);
		exit(1);
	}

	strncpy(cur_path, path, MAX_PATH_SIZE);
	ptr = cur_path + strlen(cur_path);

	if ((dp=opendir(path)) == NULL) {
		fprintf(stderr, "opendir error for %s\n", path);
		return;
	}

	*ptr++ = '/';
	*ptr = '\0';

	int count = 1;	
	while((dt=readdir(dp))) {
		if (strcmp(dt->d_name, ".") && strcmp(dt->d_name, "..")) {
			strcpy(ptr, dt->d_name);
			if (stat(cur_path, &st) != 0) {
				fprintf(stderr, "stat error for %s\n", cur_path);
				exit(1);
			}

			append_tail(li, count++, cur_path);
			if (S_ISDIR(st.st_mode)) {
				search(li, cur_path);
			}
		}
	}
	return;
}
