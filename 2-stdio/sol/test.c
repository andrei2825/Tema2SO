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



void main() {
    SO_FILE *file = malloc(sizeof(SO_FILE));
    file->buffer = malloc(4096);
    char b[1];
    file->fd = open("test.txt", O_RDONLY);
    int c = read(file->fd,b, 1);
    // strcat(file->buffer, b);
    while(c != 0) {
        strcat(file->buffer, b);
        c = read(file->fd, b, 1);
    }
    file->buffer[3] = '\0';
    strcat(file->buffer, b);
    printf("%s ", file->buffer);
    printf("%d ", c);
}