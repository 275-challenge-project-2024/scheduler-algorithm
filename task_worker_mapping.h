#ifndef TASK_WORKER_MAPPING_H
#define TASK_WORKER_MAPPING_H

#include <stdint.h>
#include "task_heap.h" // Assuming task_heap.h defines TaskElement

#define MAX_MAPPINGS 256

struct TaskWorkerMapping {
    char taskId[32];
    char workerId[32];
    char timestamp[32];
    int32_t priority;
    int32_t execution_time;
    char clientIp[64];  // Added clientIp field
    char status[16]; // Status field added
};

struct TaskWorkerMappingList {
    TaskWorkerMapping mappings[MAX_MAPPINGS];
    size_t size;
    pthread_mutex_t mutex;
};

#endif // TASK_WORKER_MAPPING_H