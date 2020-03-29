/*********************************************************************
 *
 * Copyright (C) 2020 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without 
 * the express written permission of the copyright holder.
 *
 ***********************************************************************/

#include "fileman.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <dirent.h> 
#include <stdbool.h>
/*
 * You need to implement this function, see fileman.h for details 
 */
int fileman_read(char *fname, size_t foffset, char *buf, size_t boffset, size_t size) {
	int fd = open(fname, O_RDONLY); 
	int s;
	if (fd < 0){
		return -1;
	}
	lseek(fd, foffset, SEEK_CUR);  
	s = read(fd, buf + boffset, size);
	close(fd);
	return s;
}

/*
 * You need to implement this function, see fileman.h for details 
 */
int fileman_write(char *fname, size_t foffset, char *buf, size_t boffset, size_t size) {
	int fd = open(fname, O_RDWR); 
	int s;
	if (fd >= 0){
		return -1;
	}
	fd = creat(fname, S_IRWXU);
	lseek(fd, foffset, SEEK_CUR);
	s = write(fd, buf + boffset, size);
	close(fd);
	return s;
}

/*
 * You need to implement this function, see fileman.h for details 
 */
int fileman_append(char *fname, char *buf, size_t size) {
	int fd = open(fname, O_APPEND | O_WRONLY); 
	if (fd < 0){
		return -1;
	}
	int s = write(fd, buf, size);
	close(fd);
	return s;
}

/*
 * You need to implement this function, see fileman.h for details 
 */
int fileman_copy(char *fsrc, char *fdest) {
	int fd = open(fsrc, O_RDWR); 
	int exists = open(fdest, O_RDWR); 
	if (fd < 0 || exists >= 0){
		return -1;
	}
	int fd2 = creat(fdest, S_IRWXU);
	char buff[256];
	int count = 0;
	while(read(fd,buff,sizeof(char)) > 0)
	{
		write(fd2,buff,sizeof(char));
		count++;
	}
	close(fd);
	return count;
}

char temp[32];
int count = 0;
bool first = true;
/*
 * You need to implement this function, see fileman.h for details 
 */
void fileman_dir(int fd, char *dname)
{
	if(first){
		char path[sizeof(dname)+1];
		char* t = "\n";
		strcpy(path, dname);
		strcat(path, t);
		write(fd, path, sizeof(path));
		first = false;
	}
	else{
		for (int k = 0; k < count; k++){
			write(fd, "    ", 4);
		}
		// printf("%s\n", temp);
		char f[sizeof(temp) + 1];
		snprintf(f, sizeof(temp), "%s\n", temp);
		write(fd, f, strlen(f));
	}
	struct dirent **dir;
    int n;
	int i = 0;
	n = scandir(dname, &dir, NULL, alphasort);
    if (n < 0){
		return;
	}
    else {
		count++;
        while (i < n) {
			if (strcmp(dir[i]->d_name, ".") != 0 && strcmp(dir[i]->d_name, "..") != 0){
				char path[128];
				path[0] = '\0';
				strcpy(path, dname);
				strcat(path, "/");
				strcat(path, dir[i]->d_name);
				strcpy(temp, dir[i]->d_name);
				fileman_dir(fd, path);
			}
			i++;
        }
		count--;
    }
}
/*
 * You need to implement this function, see fileman.h for details 
 */

char temp[32];
char text[64];
int ct = 0;
void fileman_tree(int fd, char *dname)
{
	if(first){
		char path[sizeof(dname)+1];
		char* t = "\n";
		strcpy(path, dname);
		strcat(path, t);
		write(fd, path, sizeof(path));
		first = false;
	}
	else{
		// printf("%s\n", temp);
		char f[sizeof(temp) + 1];
		snprintf(f, sizeof(temp), "%s\n", temp);
		write(fd, f, strlen(f));
	}
	struct dirent **dir;
    int n;
	int i = 0;
	n = scandir(dname, &dir, NULL, alphasort);
    if (n < 0){
		return;
	}
    else {
		ct++;
        while (i < n) {
			if (strcmp(dir[i]->d_name, ".") != 0 && strcmp(dir[i]->d_name, "..") != 0){
				char path[128];
				path[0] = '\0';
				strcpy(path, dname);
				strcat(path, "/");
				strcat(path, dir[i]->d_name);
				strcpy(temp, dir[i]->d_name);
				for (int j = 0; j < ct; j++){
					if (j == ct-1 && i == n - 1){
						write(fd, "└── ", strlen("└── "));
					}
					else if (j == ct - 1){
						write(fd, "├── ", strlen("├── "));
					}
					else if (ct > 0){
						// write(fd,"      ",6);
						write(fd,"│   ",strlen("│   "));
					}
					else{
						write(fd,"      ",6);
					}
				}
				fileman_tree(fd, path);
			}
			i++;
        }
		ct--;
    }
}

