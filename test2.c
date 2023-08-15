#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "deque.h"

#define MAX_PATH_SIZE	1024
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
		exit(1);
	}

	*ptr++ = '/';
	*ptr = '\0';

	int count = 1;	
	while((dt=readdir(dp))) {
		if (strcmp(dt->d_name, ".") && strcmp(dt->d_name, "..")) {
			strcpy(ptr, dt->d_name);
			append_tail(li, count++, cur_path);
		}
	}
	return;
}
int main(void)
{
	List* l = init(NULL);
	char *cwd = getcwd(NULL,0);
	search(l, cwd);
	Node* n = deque(l, -2);
	printn(l);
	printf("%s\n", n->path);
	exit(0);
}

