#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include "task_heap.h"

#include "worker_heap.h"  // assuming heap.h is available for import
#include "queue.h"

#define verbose 1

extern key_t worker_heap_k = "worker_heap";  // external key
extern key_t task_heap_k = "task_heap";
extern key_t asgnd_tasks_q_k = "assigned_tasks_queue";

class Scheduler {

public:
    int worker_shm_id;
    int task_shm_id;
    int asgnd_tasks_shm_id;
    Heap* worker_heap;
    HeapData* task_heap;
    Queue* asgnd_tasks_q;
    Scheduler() {
        worker_shm_id = shmget(worker_heap_k, sizeof(Heap) + INITIAL_CAPACITY * sizeof(HeapItem), 0666);
        if (worker_shm_id == -1) {
            perror("shmget for worker queue failed in Scheduler constructor");
            exit(1); 
        }
        worker_heap = (Heap*)shmat(worker_shm_id, NULL, 0);
        if (worker_heap == (void*) -1) {
            perror("shmat for worker queue failed in Scheduler constructor");
            exit(1);
        }

        task_shm_id = shmget(task_heap_k, sizeof(Heap) + INITIAL_CAPACITY * sizeof(HeapItem), 0666);
        if (task_shm_id == -1) {
            perror("shmget for task queue failed in Scheduler constructor");
            exit(1); 
        }
        task_heap = (Heap*)shmat(task_shm_id, NULL, 0);
        if (task_heap == (void*) -1) {
            perror("shmat for task queue failed in Scheduler constructor");
            exit(1);
        }

        asgnd_tasks_shm_id = shmget(asgnd_tasks_q_k, sizeof(Queue), 0666);
        if (asgnd_tasks_shm_id == -1) {
            perror("shmget for assigned tasks queue failed in Scheduler constructor");
            exit(1); 
        }
        asgnd_tasks_q = (Heap*)shmat(asgnd_tasks_shm_id, NULL, 0);
        if (asgnd_tasks_q == (void*) -1) {
            perror("shmat for assigned tasks queue failed in Scheduler constructor");
            exit(1);
        }

    }

    ~Scheduler() {
        if (shmdt(heap) == -1) {
            perror("shmdt failed in Scheduler destructor");
        }
    }

    HeapItem getWorkerWithMaxCapacity() {
        while(sch.worker_heap->lock == true){

        }
        sch.worker_heap->lock = true;
        HeapItem topWorker = worker_heap->data[0];
        sch.worker_heap->lock = false;
        return topWorker;
    }

    HeapElement getTaskWithMaxPriority(){
        while(sch.task_heap->lock == true){

        }
        sch.task_heap->lock = true;
        HeapElement topTask = heap_pop(sch->task_heap);
        heapify_down(task_heap, 0);
        sch.task_heap->lock = false;
        return topTask;
    }

    void pushAssignedTaskToQueue(HeapElement task, int workerId){
        queue_push(sch->asgnd_tasks_q, (task, workerId))
    }
};

int main(){
    if (verbose > 1){
        std::cout << "Starting Scheduler Process" << std::endl;
    }
    Scheduler sch = new Scheduler();
    while(1){
        HeapElement topTask = sch.getTaskWithMaxPriority();
        int workerId = sch.getWorkerWithMaxCapacity()->workerId;
        sch.pushAssignedTaskToQueue(task, workerId);

    }
}
