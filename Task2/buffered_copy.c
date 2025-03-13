#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>

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

    FILE *src_fp = fopen(src_file, "rb");
    if(src_fp == NULL){
        fprintf(stderr, "Error opening source file: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    FILE *dest_fp = fopen(dest_file, "wb");
    if(dest_fp == NULL){
        fprintf(stderr, "Error opening destination file: %s\n", strerror(errno));
        fclose(src_fp);
        return EXIT_FAILURE;
    }

    char buf[BUF_SIZE];
    size_t bytes_read, total_bytes = 0;

    while((bytes_read = fread(buf, 1, BUF_SIZE, src_fp)) > 0){
        size_t bytes_written = fwrite(buf, 1, bytes_read, dest_fp);
        if(bytes_written != bytes_read){
            fprintf(stderr, "Error writing to destination file: %s\n", strerror(errno));
            fclose(src_fp);
            fclose(dest_fp);
            return EXIT_FAILURE;
        }
        total_bytes += bytes_written;
    }

    if (ferror(src_fp)){
        fprintf(stderr, "Error reading from source file: %s\n", strerror(errno));
        fclose(src_fp);
        fclose(dest_fp);
        return EXIT_FAILURE;
    }

    fclose(src_fp);
    fclose(dest_fp);

    gettimeofday(&end, NULL);
    double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;

    printf("Buffered copy completed\n");
    printf("Total bytes copied: %ld\n", total_bytes);
    printf("Time taken: %.6f seconds\n", time_taken);

    return EXIT_SUCCESS;
}
