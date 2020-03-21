#include "so_stdio.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

typedef struct _so_file{
	char *buf;
	int fd;
	int cursor;
}SO_FILE;


SO_FILE *so_fopen(const char *pathname, const char *mode){

	SO_FILE *file = (SO_FILE *)calloc(1,sizeof(SO_FILE));
	file->buf = (char*)calloc(4096 , sizeof(char));
	file->cursor = 0;
	int ret = 0;

	if(!strcmp(mode, "r")) {
		ret = open(pathname, O_RDONLY, 0666);
	}
	else if(!strcmp(mode, "r+")) {
		ret = open(pathname, O_RDWR, 0666);
	}
	else if(!strcmp(mode, "w")) {
		ret = open(pathname, O_WRONLY| O_CREAT| O_TRUNC, 0666);
	}
	else if(!strcmp(mode, "w+")) {
		ret = open(pathname, O_RDWR| O_CREAT| O_TRUNC, 0666);
	}
	else if(!strcmp(mode, "a")) {
		ret = open(pathname, O_APPEND| O_CREAT, 0666);
	}
	else if(!strcmp(mode, "a+")) {
		ret = open(pathname, O_RDONLY| O_APPEND| O_CREAT, 0666);
	}
	if(ret > 0) {
		file->fd = ret;
		return file;
	}
	else {
		free(file->buf);
		free(file);
		return NULL;
	}
}
int so_fclose(SO_FILE *stream){

	int ret = close(stream->fd);
	free(stream->buf);
	free(stream);
	if(ret == 0){
		return 0;
	}
	return SO_EOF;
}

int so_fileno(SO_FILE *stream){
	return 0;
}

int so_fflush(SO_FILE *stream){
	return 0;
}

int so_fseek(SO_FILE *stream, long offset, int whence){
	return 0;
}

long so_ftell(SO_FILE *stream){
	return 0;
}

size_t so_fread(void *ptr, size_t size, size_t nmemb, SO_FILE *stream){
	return 0;
}

size_t so_fwrite(const void *ptr, size_t size, size_t nmemb, SO_FILE *stream){
	return 0;
}

int so_fgetc(SO_FILE *stream){
	
	int ret = 0;
	if((stream->buf[0] == 0 && stream->cursor == 0) || stream->buf[4095] != 0 ){
		ret = read(stream->fd, stream->buf, 4096);
	}
	if(ret != -1){
		stream->cursor++;
		return (int)(stream->buf[stream->cursor - 1]);
	}
	else {
		free(stream->buf);
		free(stream);
		return SO_EOF;
	}
}
int so_fputc(int c, SO_FILE *stream){
	

	read(stream->fd,stream->buf,4096);
	//printf("inainte de memset= %s",stream->buf);
	memset(stream->buf + stream->cursor, (unsigned char)c, 1);
	//printf("\nstring = \n%s\n",stream->buf);
	int ret = write(stream->fd, stream->buf, 4096);
	if(ret == -1) {
		return SO_EOF;
	}
	stream->cursor++;
	return c;	
}
int so_feof(SO_FILE *stream){
	return 0;
}
int so_ferror(SO_FILE *stream){
	return 0;
}
SO_FILE *so_popen(const char *command, const char *type){
	return NULL;
}
int so_pclose(SO_FILE *stream){
	return 0;
}
