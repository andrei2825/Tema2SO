#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include "so_stdio.h"
#include <unistd.h>

struct _so_file
{
    int fd;
    int offset;
    char *buffer;
    char *path;
    char *mode;

};


FUNC_DECL_PREFIX SO_FILE *so_fopen(const char *pathname, const char *mode) {
    SO_FILE *file = malloc(sizeof(SO_FILE));
    file->buffer = malloc(4096);
    if(strcmp(mode, "r") == 0)
        file->fd = open(pathname, O_RDONLY);
    else if(strcmp(mode, "r+") == 0)
        file->fd = open(pathname, O_RDWR);
    else if(strcmp(mode, "w") == 0)
        file->fd = open(pathname, O_WRONLY|O_CREAT|O_TRUNC, 0644);
    else if(strcmp(mode, "w+") == 0)
        file->fd = open(pathname, O_RDWR|O_TRUNC, 0644);
    else if(strcmp(mode, "a") == 0)
        file->fd = open(pathname, O_APPEND|O_CREAT, 0644);
    else if(strcmp(mode, "a+") == 0)
        file->fd = open(pathname, O_APPEND|O_RDONLY|O_CREAT, 0644);
        
    if(file->fd < 0) {
        return NULL;
    }
    return file;
}

FUNC_DECL_PREFIX int so_fclose(SO_FILE *stream) {
    int rc = close(stream->fd);
    if(rc == 0) {
        return rc;
    }
    return SO_EOF;
}


FUNC_DECL_PREFIX int so_fgetc(SO_FILE *stream) {
    char bf[1];
    char res;
    if(strlen(stream->buffer) > 0) {
        res = stream->buffer[strlen(stream->buffer)-1];
        stream->buffer[strlen(stream->buffer)-1] = '\0';
        return res;
    } else {
        int c = read(stream->fd, bf, 1);
        strcat(stream->buffer, bf);
        if(c != 0) {
            res = stream->buffer[strlen(stream->buffer)-1];
            stream->buffer[strlen(stream->buffer)-1] = '\0';
            return res;
        }
    }
    return SO_EOF;
}


// FUNC_DECL_PREFIX int so_fputc(int c, SO_FILE *stream) {
//     int p = putc(c, stream);
//     if(p != EOF) {
//         return p;
//     }
//     return SO_EOF;
// }


// size_t so_fread(void *ptr, size_t size, size_t nmemb, SO_FILE *stream) {
//     int n = size * nmemb;
//     int numOfElements = 0;
//     ptr = (char*) malloc(sizeof(char) * n);
//     for(int i = 0; i < nmemb; i++) {
//         for(int j = 0; j < size; j++) {
//             ptr = so_fgetc(stream);
//             if(ptr == SO_EOF) {
//                 return 0;
//             }
//             ptr++;
//         }
//         numOfElements++;
//     }
//     return numOfElements;
// }
