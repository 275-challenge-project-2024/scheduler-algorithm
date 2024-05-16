#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "heap.h"
// add any needed grpc headers here

class Scheduler {
public:
    Scheduler();
    ~Scheduler();

    // insert any grpc declarations here 
    
private:
    int shm_id;
    Heap* heap;

    HeapItem getWorkerWithMaxCapacity();
};

#endif 
