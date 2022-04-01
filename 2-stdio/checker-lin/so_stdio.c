#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include "so_stdio.h"
#include <unistd.h>

#define BUFFERSIZE 4096

struct _so_file
{
	int fd;
	int offset;
	int bffIndex;
	char *buffer;
	char *path;
	char *mode;

};


SO_FILE *so_fopen(const char *pathname, const char *mode) {
	SO_FILE *file = malloc(sizeof(SO_FILE));

	file->buffer = malloc(BUFFERSIZE);
	file->bffIndex = 0;


	if (strcmp(mode, "r") == 0)
		file->fd = open(pathname, O_RDONLY);
	else if (strcmp(mode, "r+") == 0)
		file->fd = open(pathname, O_RDWR);
	else if (strcmp(mode, "w") == 0)
		file->fd = open(pathname, O_WRONLY|O_CREAT|O_TRUNC, 0644);
	else if (strcmp(mode, "w+") == 0)
		file->fd = open(pathname, O_RDWR|O_TRUNC, 0644);
	else if (strcmp(mode, "a") == 0)
		file->fd = open(pathname, O_APPEND|O_CREAT, 0644);
	else if (strcmp(mode, "a+") == 0)
		file->fd = open(pathname, O_APPEND|O_RDONLY|O_CREAT, 0644);
	else {
		free(file->buffer);
		free(file);
		return NULL;
		}

	if (file->fd < 0) {
		free(file->buffer);
		free(file);
		return NULL;
	}
	return file;
}

int so_fclose(SO_FILE *stream)
{
	int rc = close(stream->fd);
	free(stream->buffer);
	free(stream);
	if (rc < 0)
		return SO_EOF;
	return 0;
}




int so_fgetc(SO_FILE *stream){
	if (strlen(stream->buffer) > 0) {
		return stream->buffer[stream->bffIndex++];
	}
	if (read(stream->fd, stream->buffer, BUFFERSIZE)) {
		return stream->buffer[stream->bffIndex++];
	}
	return SO_EOF;
}

int so_fileno(SO_FILE *stream) {
	return stream->fd;
}


int so_fflush(SO_FILE *stream)
{
	return 0;
}

int so_fseek(SO_FILE *stream, long offset, int whence)
{
	return 0;
}
long so_ftell(SO_FILE *stream)
{
	return 0;
}

size_t so_fread(void *ptr, size_t size, size_t nmemb, SO_FILE *stream)
{
	return 0;
}

size_t so_fwrite(const void *ptr, size_t size, size_t nmemb, SO_FILE *stream)
{
	return 0;
}

int so_fputc(int c, SO_FILE *stream)
{
	return 0;
}

int so_feof(SO_FILE *stream)
{
	return 0;
}
int so_ferror(SO_FILE *stream)
{
	return 0;
}

SO_FILE *so_popen(const char *command, const char *type)
{

	SO_FILE *f;
	f->fd = 0;
	return f;
}
int so_pclose(SO_FILE *stream)
{
	return 0;
}











// FUNC_DECL_PREFIX int so_fgetc(SO_FILE *stream)
// {
// 	char bf[1];
// 	char res;

// 	if (strlen(stream->buffer) > 0) {
// 		res = stream->buffer[0];
// 		stream->buffer[strlen(stream->buffer)-1] = '\0';
// 		memmove(stream->buffer, stream->buffer+1, strlen(stream->buffer));
// 		return res;
// 	}
// 	int c = read(stream->fd, bf, 1);

// 	strcat(stream->buffer, bf);
// 	if (c != 0) {
// 		res = stream->buffer[0];
// 		memmove(stream->buffer, stream->buffer+1, strlen(stream->buffer));
// 		return res;
// 	}
// 	return SO_EOF;
// }

// size_t so_fread(void *ptr, size_t size, size_t nmemb, SO_FILE *stream)
// {
//     char elem[size];
// 	int numberOfElements;
// 	int i, j;

//     numberOfElements = 0;
// 	for (i = 0; i < nmemb; i++) {
// 		if (strlen(stream->buffer) > size) {
// 			for (j = 0; j < size; j++) {
// 				strcat(ptr, )
// 			}
// 		}
// 	}
// }
