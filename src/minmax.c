#include "minmax.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
    const int *array;
    int size;
    int threads;
    int demo;
    MinMax result;
} Task;

void search(const int *array, int size, MinMax *result) {
    result->min = array[0];
    result->max = array[0];
    for (int i = 1; i < size; i++) {
        if (array[i] < result->min) {
            result->min = array[i];
        }
        if (array[i] > result->max) {
            result->max = array[i];
        }
    }
}

void *worker(void *argument) {
    Task *task = argument;
    find_minmax(task->array, task->size, task->threads,
                task->demo, &task->result);
    return NULL;
}

void find_minmax(const int *array, int size, int threads,
                 int demo, MinMax *result) {
    if (threads == 1) {
        if (demo) {
            printf("PID=%ld TID=%ld elements=%d\n",
                   (long)getpid(), (long)gettid(), size);
            fflush(stdout);
            sleep(10);
        }
        search(array, size, result);
        return;
    }

    int half = size / 2;
    Task left = {array, half, threads / 2, demo, {0, 0}};
    pthread_t thread;
    int error = pthread_create(&thread, NULL, worker, &left);
    if (error != 0) {
        fprintf(stderr, "pthread_create: %s; searching sequentially\n",
                strerror(error));
        search(array, size, result);
        return;
    }

    find_minmax(array + half, size - half, threads - threads / 2,
                demo, result);
    error = pthread_join(thread, NULL);
    if (error != 0) {
        fprintf(stderr, "pthread_join: %s\n", strerror(error));
        exit(EXIT_FAILURE);
    }

    if (left.result.min < result->min) {
        result->min = left.result.min;
    }
    if (left.result.max > result->max) {
        result->max = left.result.max;
    }
}
