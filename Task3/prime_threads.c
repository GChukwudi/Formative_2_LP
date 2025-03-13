#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <stdbool.h>

#define NUM_THREADS 20
#define MAX 10000

typedef struct {
    int start;
    int end;
    int prime_count;
} thread_data;

int global_prime_count = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

bool is_prime(int n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;

    int sqrt_n = (int)sqrt(n);

    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }

    return true;
}

void *count_primes(void *arg) {
    thread_data *data = (thread_data *)arg;
    data->prime_count = 0;

    printf("Thread %ld: start = %d, end = %d\n", pthread_self(), data->start, data->end - 1);

    for (int i = data->start; i < data->end; i++) {
        if (is_prime(i)) {
            data->prime_count++;
        }
    }

    pthread_mutex_lock(&mutex);
    global_prime_count += data->prime_count;
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    thread_data data[NUM_THREADS];

    int range = MAX / NUM_THREADS;
    for (int i = 0; i < NUM_THREADS; i++) {
        data[i].start = i * range;
        
        if (i == NUM_THREADS - 1) {
            data[i].end = MAX;
        } else {
            data[i].end = (i + 1) * range;
        }
        
        pthread_create(&threads[i], NULL, count_primes, (void *)&data[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(threads[i], NULL)) {
            perror("pthread_join");
            exit(EXIT_FAILURE);
        }
    }

    printf("Total number of prime numbers from 0 to 10,000: %d\n", global_prime_count);

    pthread_mutex_destroy(&mutex);

    return 0;
}
