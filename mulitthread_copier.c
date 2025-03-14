#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define BUFFER_SIZE 8192

typedef struct {
    int thread_id;
    const char *src;
    const char *dst;
    off_t start_offset;
    size_t chunk_size;
} ThreadData;

void *copy_chunk(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    char *buffer;
    int src_fd, dst_fd;
    ssize_t bytes_to_read, bytes_read, bytes_written;
    off_t offset;

    buffer = (char *)malloc(BUFFER_SIZE);
    if (buffer == NULL) {
        fprintf(stderr, "Failed to allocate buffer in thread %d\n", data->thread_id);
        pthread_exit(NULL);
    }

    src_fd = open(data->src, O_RDONLY);
    if (src_fd == -1) {
        fprintf(stderr, "Failed to open source file in thread %d\n", data->thread_id);
        free(buffer);
        pthread_exit(NULL);
    }

    dst_fd = open(data->dst, O_WRONLY);
    if (dst_fd == -1) {
        fprintf(stderr, "Failed to open destination file in thread %d\n", data->thread_id);
        free(buffer);
        close(src_fd);
        pthread_exit(NULL);
    }

    if (lseek(src_fd, data->start_offset, SEEK_SET) == -1) {
        fprintf(stderr, "Failed to seek source file in thread %d\n", data->thread_id);
        free(buffer);
        close(src_fd);
        close(dst_fd);
        pthread_exit(NULL);
    }

    if (lseek(dst_fd, data->start_offset, SEEK_SET) == -1) {
        fprintf(stderr, "Failed to seek destination file in thread %d\n", data->thread_id);
        free(buffer);
        close(src_fd);
        close(dst_fd);
        pthread_exit(NULL);
    }

    size_t bytes_left = data->chunk_size;

    while (bytes_left > 0) {
        bytes_to_read = (bytes_left < BUFFER_SIZE) ? bytes_left : BUFFER_SIZE;
        
        bytes_read = read(src_fd, buffer, bytes_to_read);
        if (bytes_read <= 0) {
            if (bytes_read < 0) {
                fprintf(stderr, "Thread %d failed to read from source file: %s\n", data->thread_id, strerror(errno));
            }
            break;
        }

        char *ptr = buffer;
        bytes_left -= bytes_read;

        while (bytes_read > 0) {
            bytes_written = write(dst_fd, ptr, bytes_read);
            if (bytes_written <= 0) {
                fprintf(stderr, "Thread %d failed to write to destination file: %s\n", data->thread_id, strerror(errno));
                close(src_fd);
                close(dst_fd);
                free(buffer);
                pthread_exit(NULL);
            }

            ptr += bytes_written;
            bytes_read -= bytes_written;
        }
        bytes_left -= bytes_read;
    }

    printf("Thread %d finished copying %ld bytes\n", data->thread_id, data->chunk_size - bytes_left);

    free(buffer);
    close(src_fd);
    close(dst_fd);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <source> <destination> <num_threads>\n", argv[0]);
        return 1;
    }

    const char *src = argv[1];
    const char *dst = argv[2];
    int num_threads = atoi(argv[3]);

    if(num_threads <= 0) {
        fprintf(stderr, "Number of threads must be greater than 0\n");
        return 1;
    }

    struct stat st;
    if (stat(src, &st) == -1) {
        fprintf(stderr, "Failed to stat source file: %s\n", strerror(errno));
        return 1;
    }

    off_t file_size = st.st_size;

    struct timeval start, end;
    gettimeofday(&start, NULL);

    int dst_fd = open(dst, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (dst_fd == -1) {
        fprintf(stderr, "Failed to open destination file: %s\n", strerror(errno));
        return 1;
    }

    if (ftruncate(dst_fd, file_size) == -1) {
        fprintf(stderr, "Failed to truncate destination file: %s\n", strerror(errno));
        close(dst_fd);
        return 1;
    }

    close(dst_fd);

    size_t chunk_size = file_size / num_threads;
    size_t last_chunk_size = chunk_size + file_size % num_threads;

    printf("File size: %ld\n", file_size);
    printf("Using %d threads\n", num_threads);
    printf("Chunk size: %ld\n", chunk_size);
    printf("Last chunk size: %ld\n", last_chunk_size);

    pthread_t *threads = (pthread_t *)malloc(num_threads * sizeof(pthread_t));
    ThreadData *thread_data = (ThreadData *)malloc(num_threads * sizeof(ThreadData));

    if (!threads || !thread_data) {
        fprintf(stderr, "Failed to allocate memory for threads\n");
        free(threads);
        free(thread_data);
        return 1;
    }

    for (int i = 0; i < num_threads; i++) {
        thread_data[i].thread_id = i;
        thread_data[i].src = src;
        thread_data[i].dst = dst;
        thread_data[i].start_offset = i * chunk_size;
        
        if (i == num_threads - 1) {
            thread_data[i].chunk_size = last_chunk_size;
        } else {
            thread_data[i].chunk_size = chunk_size;
        }

        if (pthread_create(&threads[i], NULL, copy_chunk, &thread_data[i]) != 0) {
            fprintf(stderr, "Failed to create thread %d\n", i);
            for (int j = 0; j < i; j++) {
                pthread_cancel(threads[j]);
                pthread_join(threads[j], NULL);
            }
            free(threads);
            free(thread_data);
            return 1;
        }
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    gettimeofday(&end, NULL);
    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;

    printf("Copy took %.2f seconds\n", elapsed_time);

    free(threads);
    free(thread_data);

    return 0;
}
