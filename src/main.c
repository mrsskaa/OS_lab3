#include "minmax.h"

#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int parse_number(const char *text, int *value) {
    char *end;
    errno = 0;
    long number = strtol(text, &end, 10);
    if (errno != 0 || text == end || *end != '\0' || number <= 0 || number > INT_MAX) {
        return 1;
    }
    *value = (int)number;
    return 0;
}

int main(int argc, char **argv) {
    if ((argc != 5 && argc != 6) || strcmp(argv[1], "-n") != 0 ||
        strcmp(argv[3], "-t") != 0 || (argc == 6 && strcmp(argv[5], "-d") != 0)) {
        fprintf(stderr, "Usage: %s -n SIZE -t THREADS [-d]\n", argv[0]);
        return 1;
    }

    int size;
    int threads;
    if (parse_number(argv[2], &size) != 0 || parse_number(argv[4], &threads) != 0 ||
        (size_t)size > SIZE_MAX / sizeof(int)) {
        fprintf(stderr, "Invalid arguments\n");
        return 1;
    }
    if (threads > size) {
        threads = size;
    }

    int *array = malloc((size_t)size * sizeof(int));
    if (array == NULL) {
        perror("malloc");
        return 1;
    }
    srand(42);
    for (int i = 0; i < size; i++) {
        array[i] = rand() % 200001 - 100000;
    }
    if (size < 30) {
        printf("Array: ");
        for (int i = 0; i < size; i++) {
            printf("%d ", array[i]);
        }
        printf("\n");
    }

    struct timespec start;
    struct timespec finish;
    if (clock_gettime(CLOCK_MONOTONIC, &start) == -1) {
        perror("clock_gettime");
        free(array);
        return 1;
    }
    MinMax result;
    find_minmax(array, size, threads, argc == 6, &result);
    if (clock_gettime(CLOCK_MONOTONIC, &finish) == -1) {
        perror("clock_gettime");
        free(array);
        return 1;
    }

    double seconds = finish.tv_sec - start.tv_sec;
    seconds += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("Elements: %d\nThreads limit: %d\n", size, threads);
    printf("Min: %d\nMax: %d\nTime: %.9f seconds\n", result.min, result.max, seconds);
    free(array);
    return 0;
}
