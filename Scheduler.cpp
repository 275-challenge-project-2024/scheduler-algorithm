#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#include "heap.h"  // assuming heap.h is available for import

extern key_t worker_heap;  // external key

class Scheduler {
private:
    int shm_id;
    Heap* heap;

public:
    Scheduler() {
        shm_id = shmget(worker_heap, sizeof(Heap) + INITIAL_CAPACITY * sizeof(HeapItem), 0666);

        if (shm_id == -1) {
            perror("shmget failed in Scheduler constructor");
            exit(1); 
        }

        heap = (Heap*)shmat(shm_id, NULL, 0);
        if (heap == (void*) -1) {
            perror("shmat failed in Scheduler constructor");
            exit(1);
        }
    }

    ~Scheduler() {
        if (shmdt(heap) == -1) {
            perror("shmdt failed in Scheduler destructor");
        }
    }
   
    HeapItem getWorkerWithMaxCapacity() {
        if (heap->size == 0) {
            std::cerr << "Error: Heap is empty in Scheduler::getWorkerWithMaxCapacity\n";
            return HeapItem{.id = "", .value = 0}; 
        }

        HeapItem maxItem = heap_pop(heap);
        return maxItem;
    }
    
};
