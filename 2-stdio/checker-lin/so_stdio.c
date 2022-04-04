#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include "so_stdio.h"
#include <unistd.h>

#define BUFFERSIZE 4096

// Structura 
struct _so_file {
	int fd;
	int offset;
	int bffIndex;
	// retine ultima operatie, 1 pentru write, 2 pentru read, 0 pentru idle
	int lastOp;
	// retine nr de bytes cititi de ultimul read
	int lastRead;
	// retin modul de deschidere a fisierului 0 - r, 1 - r+, 2 - w, 3 - w+, 4 - a, 5 - a+
	int mode;
	int eof;
	int cursorPos;
	char *buffer;
	int errFlag;
};

SO_FILE *so_fopen(const char *pathname, const char *mode)
{
	SO_FILE *file = malloc(sizeof(SO_FILE));

	file->bffIndex = 0;
	file->eof = 0;
	file->lastOp = 0;
	file->lastRead = BUFFERSIZE;
	file->errFlag = 0;
	file->cursorPos = 0;
// deschid si/sau creez un fisier si retin modul in care e deschis
	if (strcmp(mode, "r") == 0) {
		file->fd = open(pathname, O_RDONLY);
		file->mode = 0;
	} else if (strcmp(mode, "r+") == 0) {
		file->fd = open(pathname, O_RDWR);
		file->mode = 1;
	} else if (strcmp(mode, "w") == 0) {
		file->fd = open(pathname, O_WRONLY|O_CREAT|O_TRUNC, 0644);
		file->mode = 2;
	} else if (strcmp(mode, "w+") == 0) {
		file->fd = open(pathname, O_RDWR|O_TRUNC, 0644);
		file->mode = 3;
	} else if (strcmp(mode, "a") == 0) {
		file->fd = open(pathname, O_APPEND|O_CREAT, 0644);
		file->mode = 4;
		so_fseek(file, 0, SEEK_END);
	} else if (strcmp(mode, "a+") == 0) {
		file->fd = open(pathname, O_APPEND|O_RDONLY|O_CREAT, 0644);
		file->mode = 5;
	} else {
		free(file);
		return NULL;
		}
	file->buffer = calloc(BUFFERSIZE, sizeof(char));
	if (file->fd < 0) {
		free(file->buffer);
		free(file);
		return NULL;
	}
	return file;
}


// eliberez bufferul, dezaloc memoria si inchid fisierul
int so_fclose(SO_FILE *stream)
{
	int err = 0;
	int rc;

	if (stream->lastOp == 1)
		err = so_fflush(stream);
	rc = close(stream->fd);
	free(stream->buffer);
	free(stream);
	if (rc != 0 || err != 0)
		return SO_EOF;
	return 0;
}

// returnez un caracter din buffer sau populez bufferul
int so_fgetc(SO_FILE *stream)
{
	int r;

	stream->lastOp = 2;
// daca bufferul este gol sau daca am citit deja tot contunutul acestuia, il repopulez
	if ((stream->bffIndex == 0 || stream->bffIndex == BUFFERSIZE) && stream->lastRead == BUFFERSIZE) {
		r  = read(stream->fd, stream->buffer, BUFFERSIZE);
		stream->lastRead = r;
		if (r <= 0) {
			stream->eof = SO_EOF;
			stream->errFlag = 1;
			return SO_EOF;
		}
		stream->bffIndex = 0;
	}
	stream->cursorPos++;
	int c;

// extrag caracterul de la indexul curent din buffer sau verific daca am ajuns la EOF
	if (stream->bffIndex < stream->lastRead)
		c = (unsigned char)stream->buffer[stream->bffIndex++];
	else {
		if (stream->lastRead < BUFFERSIZE) {
			r =  read(stream->fd, stream->buffer, BUFFERSIZE);
			if (r <= 0) {
				stream->errFlag = 1;
				stream->eof = SO_EOF;
				return SO_EOF;
			}
		}
	}
	return c;

}

// citestc nmemb * size elemente
size_t so_fread(void *ptr, size_t size, size_t nmemb, SO_FILE *stream)
{
	stream->lastOp = 2;
	unsigned int c;

	for (int i = 0; i < nmemb*size; i++) {
		c =  so_fgetc(stream);
		if (so_feof(stream)) {
			stream->errFlag = 1;
			stream->eof = SO_EOF;
// in caz de eroare returnez nr de elemente citite
			return i / size;
		}
		*(char *)ptr = c;
		ptr++;
	}
	return nmemb;
}

// scriu un caracter in buffer, daca acesta este plin ii dau flush
int so_fputc(int c, SO_FILE *stream)
{
	stream->lastOp = 1;
	if (stream->bffIndex == BUFFERSIZE)
		so_fflush(stream);

	if (stream->bffIndex < BUFFERSIZE) {
		stream->cursorPos++;
		stream->buffer[stream->bffIndex] = c;
		stream->bffIndex++;
		return c;
	}
	stream->errFlag = 1;
	stream->eof = SO_EOF;
	return SO_EOF;

}


// scriu nmemb * size caractere in fisier
size_t so_fwrite(const void *ptr, size_t size, size_t nmemb, SO_FILE *stream)
{
	for (int i = 0; i < nmemb * size; i++) {
		so_fputc(*(int *)ptr, stream);
		if (so_feof(stream)) {
			stream->errFlag = 1;
			stream->eof = SO_EOF;
			return 0;
		}
		ptr += 1;
	}

	return nmemb;
}

// returnez file descriptorul fisierului deschis
int so_fileno(SO_FILE *stream)
{
	return stream->fd;
}

// golesc bufferul daca acesta are ceva in el
int so_fflush(SO_FILE *stream)
{
	int r;

	if (stream->bffIndex > 0) {
		r = write(stream->fd, stream->buffer, stream->bffIndex);
		if (r > 0) {
			free(stream->buffer);
			stream->buffer = calloc(BUFFERSIZE, sizeof(char));
			stream->bffIndex = 0;
			return 0;
		}
	}
	stream->errFlag = 1;
	return SO_EOF;
}

// mut cursorul la pozitia data in whence + offset
int so_fseek(SO_FILE *stream, long offset, int whence)
{
	if (whence == SEEK_CUR && stream->lastOp == 2)
		offset = offset - (stream->lastRead - stream->bffIndex);

	// daca ultima operatie a fost citire, golesc bufferul
	if (stream->lastOp == 1) {
		so_fflush(stream);
	} else if (stream->lastOp == 2) {
		stream->bffIndex = 0;
		stream->lastRead = BUFFERSIZE;
	}
	int res = lseek(stream->fd, offset, whence);

	if (res < 0) {
		stream->errFlag = 1;
		return -1;
	}
	stream->cursorPos = res;
	return 0;
}

// returnez pozitia cursorului
long so_ftell(SO_FILE *stream)
{
	return stream->cursorPos;
}

// daca s-a ajuusn la finalul fisierului returnez SO_EOF
int so_feof(SO_FILE *stream)
{
	if (stream->eof == SO_EOF)
		return SO_EOF;
	return 0;
}
// returnez 1 in caz de eroare si 0 altfel
int so_ferror(SO_FILE *stream)
{
	if (stream->errFlag)
		return 1;
	return 0;
}

SO_FILE *so_popen(const char *command, const char *type)
{

	return NULL;
}
int so_pclose(SO_FILE *stream)
{
	return 0;
}
