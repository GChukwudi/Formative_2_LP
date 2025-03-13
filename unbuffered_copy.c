#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/stat.h>

#define BUF_SIZE 4096

void print_usage(char *prog_name){
    fprintf(stderr, "Usage: %s <source_file> <destination_file>\n", prog_name);
    exit(1);
}

int main(int argc, char *argv[]){
    if(argc != 3){
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    const char *src_file = argv[1];
    const char *dest_file = argv[2];

    struct timeval start, end;
    gettimeofday(&start, NULL);

    int src_fd = open(src_file, O_RDONLY);
    if(src_fd == -1){
        fprintf(stderr, "Error opening source file: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    int dest_fd = open(dest_file, O_CREAT | O_WRONLY, 0644);
    if(dest_fd == -1){
        fprintf(stderr, "Error opening destination file: %s\n", strerror(errno));
        close(src_fd);
        return EXIT_FAILURE;
    }

    char buf[BUF_SIZE];
    ssize_t bytes_read, bytes_written;
    size_t total_bytes = 0;

    while((bytes_read = read(src_fd, buf, BUF_SIZE)) > 0){
        char *buf_ptr = buf;
        
    }
}
