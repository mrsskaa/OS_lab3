#ifndef MINMAX_H
#define MINMAX_H

typedef struct {
    int min;
    int max;
} MinMax;

void find_minmax(const int *array, int size, int threads, int demo, MinMax *result);

#endif
