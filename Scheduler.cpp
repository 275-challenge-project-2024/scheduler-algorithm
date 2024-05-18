#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <pthread.h>
#include <tuple>

#include "../registry/worker_heap.h"
#include "../registry/task_heap.h"
#include "task_worker_mapping.h"

#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include "task.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using taskscheduler::TaskStatus;
using taskscheduler::TaskScheduler;
using TASK_HEAP_H::task_heap_pop;
using TASK_HEAP_H::task_heapify_down;

#define verbose 1

extern key_t worker_heap_k;  // external key
extern key_t task_heap_k;
extern key_t asgnd_tasks_k;

class TaskSchedulerClient {
public:
    TaskSchedulerClient(std::shared_ptr<Channel> channel)
        : stub_(TaskScheduler::NewStub(channel)) {}

    Status SubmitTask(TaskElement task) {
        Status status;
        ClientContext context;

        Status grpcStatus = stub_->SubmitTask(&context, task, &status);

        return status;
    }
 
private:
    std::unique_ptr<TaskScheduler::Stub> stub_;
};

class Scheduler {

public:
    int worker_shm_id;
    int task_shm_id;
    int asgnd_tasks_shm_id;
    WorkerHeapData* worker_heap;
    TaskHeapData* task_heap;
    TaskWorkerMappingList* asgnd_tasks_list;
   Scheduler() {
        worker_shm_id = shmget(worker_heap_k, sizeof(WorkerHeapData), 0666);
        if (worker_shm_id == -1) {
            perror("shmget for worker queue failed in Scheduler constructor");
            exit(1); 
        }
        worker_heap = (WorkerHeapData*)shmat(worker_shm_id, NULL, 0);
        if (worker_heap == (void*) -1) {
            perror("shmat for worker queue failed in Scheduler constructor");
            exit(1);
        }

        task_shm_id = shmget(task_heap_k, sizeof(TaskHeapData), 0666);
        if (task_shm_id == -1) {
            perror("shmget for task queue failed in Scheduler constructor");
            exit(1); 
        }
        task_heap = (TaskHeapData*)shmat(task_shm_id, NULL, 0);
        if (task_heap == (void*) -1) {
            perror("shmat for task queue failed in Scheduler constructor");
            exit(1);
        }

        asgnd_tasks_shm_id = shmget(asgnd_tasks_k, sizeof(TaskWorkerMappingList), 0666);
        if (asgnd_tasks_shm_id == -1) {
            perror("shmget for assigned tasks queue failed in Scheduler constructor");
            exit(1); 
        }
        asgnd_tasks_list = (TaskWorkerMappingList*)shmat(asgnd_tasks_shm_id, NULL, 0);
        if (asgnd_tasks_list == (void*) -1) {
            perror("shmat for assigned tasks queue failed in Scheduler constructor");
            exit(1);
        }

    }

    // ~Scheduler() {
    //     if (shmdt(heap) == -1) {
    //         perror("shmdt failed in Scheduler destructor");
    //     }
    // }

    WorkerHeapElement getWorkerWithMaxCapacity() {
        pthread_mutex_lock(&this->worker_heap->mutex);
        if(worker_heap->size == 0) {
            pthread_mutex_unlock(&this->worker_heap->mutex);
            return NULL;
        }
        WorkerHeapElement topWorker = worker_heap->data[0];
        pthread_mutex_unlock(&this->worker_heap->mutex);
        return topWorker;
    }

    TaskElement getTaskWithMaxPriority(){
        pthread_mutex_lock(&this->task_heap->mutex);
        if(task_heap->size == 0) {
            pthread_mutex_unlock(&this->task_heap->mutex);
            return NULL;
        }
        TaskElement topTask = task_heap_pop(this->task_heap);
        task_heapify_down(this->task_heap, 0);
        pthread_mutex_unlock(&this->task_heap->mutex);
        return topTask;
    }

    void pushAssignedTaskToWorker(TaskElement task, WorkerHeapElement worker){
        TaskWorkerMapping taskMap;
        taskMap.clientIp = &task.clientIp;
        taskMap.priority = task.priority;
        taskMap.taskId = &task.taskId;
        taskMap.workerId = &worker.workerId;
        TaskSchedulerClient client(grpc::CreateChannel(worker.workerId, grpc::InsecureChannelCredentials()));
        Status response = client.SubmitTask(task);
        if (response.ok()){
            pthread_mutex_lock(&this->asgnd_tasks_list->mutex);
            this->asgnd_tasks_list[this->asgnd_tasks_list->size++] = taskMap;
            pthread_mutex_unlock(&this->asgnd_tasks_list->mutex);
        }
    }
    
};

int main(){
    if (verbose > 1){
        std::cout << "Starting Scheduler Process" << std::endl;
    }
    Scheduler sch = Scheduler();
    while(1){
        long sleepTime = 10000L+(long)((1e5-1e4)*rand()/(RAND_MAX+1.0));  
        TaskElement topTask = sch.getTaskWithMaxPriority();
        while(topTask == nullptr){
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
            topTask = sch.getTaskWithMaxPriority();
        }
        WorkerHeapElement worker = sch.getWorkerWithMaxCapacity();
        while(worker == nullptr){
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
            worker = sch.getWorkerWithMaxCapacity();
        }
        sch.pushAssignedTaskToWorker(task, worker);
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime)); 
    }
}
