#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define LOWER_NUM 1
#define UPPER_NUM 10000
#define BUFFER_SIZE 100
#define MAX_COUNT 10000

int buffer[BUFFER_SIZE];
int buffer_index = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Function proto
void* producer(void* arg);
void* even_consumer(void* arg);
void* odd_consumer(void* arg);

int main() {
    pthread_t producer_thread, even_consumer_thread, odd_consumer_thread;
    clock_t start_time, end_time;
    double execution_time;

    // Record the start time
    start_time = clock();

    // Create threads
    pthread_create(&producer_thread, NULL, producer, NULL);
    pthread_create(&even_consumer_thread, NULL, even_consumer, NULL);
    pthread_create(&odd_consumer_thread, NULL, odd_consumer, NULL);

    // Wait for threads to finish
    pthread_join(producer_thread, NULL);
    pthread_join(even_consumer_thread, NULL);
    pthread_join(odd_consumer_thread, NULL);

    // Record the end time
    end_time = clock();

    // The execution time in seconds
    execution_time = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
    printf("Execution time: %f seconds\n", execution_time);

    return 0;
}

// Producer function
void* producer(void* arg) {
    for (int i = 0; i < MAX_COUNT; ++i) {
        int num = LOWER_NUM + rand() % (UPPER_NUM - LOWER_NUM + 1);
        pthread_mutex_lock(&mutex);
        if (buffer_index < BUFFER_SIZE) {
            buffer[buffer_index++] = num;
            FILE *file = fopen("all.txt", "a");
            fprintf(file, "%d\n", num);
            fclose(file);
        }
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

// Even consumer function
void* even_consumer(void* arg) {
    char* file_name = "even.txt";
    FILE* file = fopen(file_name, "w");
    while (1) {
        pthread_mutex_lock(&mutex);
        if (buffer_index > 0) {
            int num = buffer[--buffer_index];
            if (num % 2 == 0) {
                fprintf(file, "%d\n", num);
            }
        }
        pthread_mutex_unlock(&mutex);
        if (buffer_index == 0) {
            fclose(file);
            break;
        }
    }
    return NULL;
}

// Odd consumer function
void* odd_consumer(void* arg) {
    char* file_name = "odd.txt";
    FILE* file = fopen(file_name, "w");
    while (1) {
        pthread_mutex_lock(&mutex);
        if (buffer_index > 0) {
            int num = buffer[--buffer_index];
            if (num % 2 != 0) {
                fprintf(file, "%d\n", num);
            }
        }
        pthread_mutex_unlock(&mutex);
        if (buffer_index == 0) {
            fclose(file);
            break;
        }
    }
    return NULL;
}
