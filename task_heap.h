#ifndef TASK_HEAP_H
#define TASK_HEAP_H

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#define MAX_TASKS 10000

// Structure to store each element in the heap
struct TaskElement
{
    char taskId[32];
    std::string commands;
    std::string status;
    int32_t curr_capacity;
    int32_t priority;
    char clientIp[32];
};

// Structure to store the heap data
struct TaskHeapData
{
    struct TaskElement data[MAX_TASKS];
    size_t size;
    pthread_mutex_t mutex;
};

// Function prototypes
struct TaskHeapData *initialize_heap(void *shm_addr);
void swap(struct TaskElement *a, struct TaskElement *b);
void heapify_up(struct TaskHeapData *heap, size_t index);
void heapify_down(struct TaskHeapData *heap, size_t index);
void heap_push(struct TaskHeapData *heap, const char *taskId, const char *timestamp, int32_t priority);
struct TaskElement heap_pop(struct TaskHeapData *heap);
ssize_t find_index_by_id(struct TaskHeapData *heap, const char *taskId);
bool remove_node_by_id(struct TaskHeapData *heap, const char *taskId);

#endif // TASK_HEAP_H