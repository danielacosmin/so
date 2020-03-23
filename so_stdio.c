#include "so_stdio.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFSIZE 4096

typedef struct _so_file {
	char *buf;
	int fd;
	int cursor;
	char last_op;
	int err;
	int bytes_added;
} SO_FILE;

SO_FILE *so_fopen(const char *pathname, const char *mode)
{
	SO_FILE *file = (SO_FILE *)malloc(sizeof(SO_FILE));

	file->buf = (char *)calloc(BUFSIZE + 1, sizeof(char));
	file->cursor = 0;
	file->last_op = 'o';
	file->err = 0;
	file->bytes_added = 0;
	int ret = 0;

	if (!strcmp(mode, "r"))
		ret = open(pathname, O_RDONLY, 0666);
	else if (!strcmp(mode, "r+"))
		ret = open(pathname, O_RDWR, 0666);
	else if (!strcmp(mode, "w"))
		ret = open(pathname, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	else if (!strcmp(mode, "w+"))
		ret = open(pathname, O_RDWR | O_CREAT | O_TRUNC, 0666);
	else if (!strcmp(mode, "a"))
		ret = open(pathname, O_WRONLY | O_APPEND | O_CREAT, 0666);
	else if (!strcmp(mode, "a+"))
		ret = open(pathname, O_RDWR | O_APPEND | O_CREAT, 0666);
	else {
		free(file->buf);
		free(file);
		return NULL;
	}
	if (ret != -1) {
		file->fd = ret;
		return file;
	}
	file->err = 1;
	free(file->buf);
	free(file);
	return NULL;
}
int so_fclose(SO_FILE *stream)
{
	int ret;

	if (stream->last_op == 'w') {
		ret = write(stream->fd, stream->buf, stream->bytes_added);
		stream->bytes_added = 0;
		if (ret == -1) {
			free(stream->buf);
			free(stream);
			return SO_EOF;
		}
	}
	ret = close(stream->fd);
	if (ret != -1) {
		free(stream->buf);
		free(stream);
		return 0;
	}
	free(stream->buf);
	free(stream);
	return SO_EOF;
}
int so_fileno(SO_FILE *stream)
{
	return stream->fd;
}
int so_fflush(SO_FILE *stream)
{	int ret;

	if (stream->last_op == 'w') {
		ret = write(stream->fd, stream->buf, stream->bytes_added);
		stream->bytes_added = 0;
	}
	if (ret != -1)
		return 0;
	stream->err = 1;
	return SO_EOF;
}
int so_fseek(SO_FILE *stream, long offset, int whence)
{
	int ret;

	if (stream->fd != -1) {
		if (stream->last_op == 'w') {
			ret = write(stream->fd, stream->buf,
					stream->bytes_added);
			if (ret != -1) {
				stream->bytes_added = 0;
				stream->cursor =
					lseek(stream->fd, offset, whence);
				return 0;
			}
		} else if (stream->last_op == 'r')
			memset(stream->buf, 0, BUFSIZE);
		stream->bytes_added = 0;
		stream->cursor = lseek(stream->fd, offset, whence);
		return 0;
	}
	stream->err = 1;
	return -1;
}
long so_ftell(SO_FILE *stream)
{
	if (stream->fd == -1) {
		stream->err = 1;
		return -1;
	}
	return (long)stream->cursor;
}
size_t so_fread(void *ptr, size_t size, size_t nmemb, SO_FILE *stream)
{
	size_t buf_val = 0;
	size_t no_elems_read = 0;

	for (long i = 0; i < size * nmemb; i++) {
		buf_val = (int)so_fgetc(stream);
		if (stream->err != 1) {
			*(char *)(ptr + no_elems_read) = (char)buf_val;
			no_elems_read++;
		} else {
			stream->err = 1;
			return 0;
		}
	}
	stream->last_op = 'r';
	return no_elems_read / size;
}
size_t so_fwrite(const void *ptr, size_t size, size_t nmemb, SO_FILE *stream)
{
	size_t elem = 0;
	size_t no_elems_written = 0;

	for (long i = 0 ; i < size * nmemb; i++) {
		elem = *(size_t *)(ptr + i);
		if (so_fputc(elem, stream) != SO_EOF)
			no_elems_written++;
		else {
			stream->err = 1;
			return 0;
		}
	}
	stream->last_op = 'w';
	return no_elems_written / size;
}
int so_fgetc(SO_FILE *stream)
{
	int ret = 0;

	if (stream->bytes_added == BUFSIZE || stream->bytes_added == 0) {
		memset(stream->buf, 0, BUFSIZE);
		ret = read(stream->fd, stream->buf, BUFSIZE);
		if (stream->bytes_added == BUFSIZE)
			stream->bytes_added = 0;
	}
	if (ret != -1)  {
		stream->bytes_added++;
		stream->cursor++;
		stream->last_op = 'r';
		return stream->buf[stream->bytes_added - 1];
	}
	stream->err = 1;
	return SO_EOF;
}
int so_fputc(int c, SO_FILE *stream)
{
	int ret = 0;

	if (stream->bytes_added < BUFSIZE) {
		stream->buf[stream->bytes_added] = (unsigned char)c;
		stream->bytes_added++;
		stream->cursor++;
	} else {
		ret = write(stream->fd, stream->buf, BUFSIZE);
		if (ret != -1) {
			memset(stream->buf, 0, BUFSIZE);
			stream->bytes_added = 0;
			stream->buf[stream->bytes_added] = (unsigned char) c;
			stream->bytes_added++;
			stream->cursor++;
		} else {
			stream->err = 1;
			return SO_EOF;
		}
	}
	stream->last_op = 'w';
	return c;
}
int so_feof(SO_FILE *stream)
{
	return 0;
}
int so_ferror(SO_FILE *stream)
{
	return stream->err;
}
SO_FILE *so_popen(const char *command, const char *type)
{
	return NULL;
}
int so_pclose(SO_FILE *stream)
{
	return 0;
}
