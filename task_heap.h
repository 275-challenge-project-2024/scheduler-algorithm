#ifndef TASK_HEAP_H
#define TASK_HEAP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#define MAX_TASKS 10000

// Structure to store each element in the heap
struct HeapElement
{
    char taskId[32];
    char timestamp[32];
    int32_t priority;
};

// Structure to store the heap data
struct HeapData
{
    struct HeapElement data[MAX_TASKS];
    bool lock;
    size_t size;
};

// Function prototypes
struct HeapData *initialize_heap(void *shm_addr);
void swap(struct HeapElement *a, struct HeapElement *b);
void heapify_up(struct HeapData *heap, size_t index);
void heapify_down(struct HeapData *heap, size_t index);
void heap_push(struct HeapData *heap, const char *taskId, const char *timestamp, int32_t priority);
struct HeapElement heap_pop(struct HeapData *heap);
ssize_t find_index_by_id(struct HeapData *heap, const char *taskId);
bool remove_node_by_id(struct HeapData *heap, const char *taskId);

#endif // TASK_HEAP_H