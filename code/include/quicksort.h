#ifndef QUICKSORT_H
#define QUICKSORT_H

#include <pcosynchro/pcoconditionvariable.h>
#include <pcosynchro/pcomutex.h>
#include <pcosynchro/pcosemaphore.h>
#include <pcosynchro/pcothread.h>

#include <iostream>
#include <queue>

#include "multithreadedsort.h"

/**
 * @brief The Quicksort class implements the multi-threaded Quicksort algorithm.
 */
template <typename T>
class Quicksort : public MultithreadedSort<T> {
   public:
    Quicksort(unsigned int nbThreads)
        : MultithreadedSort<T>(nbThreads)/*, sem(1)*/ {
        // Check for invalid number of threads
        if (nbThreads == 0) {
            throw std::invalid_argument("Number of threads must not be 0.");
        } else if (nbThreads > MAX_THREADS) {
            throw std::invalid_argument(
                "Number of threads must not be greater than " +
                std::to_string(MAX_THREADS));
        }
    }

    /**
     * @brief sort Manages the threads to sort the given sequence.
     * @param array is the sequence to sort
     */
    void sort(std::vector<T> &array) override {
        // Create the list of workers/threads
        for (unsigned int i = 0; i < this->nbThreads; i++) {
            workers.emplace_back(&Quicksort::workerThread, this);
        }

        // Add the entire array to the list of tasks and notify a worker
        put(Task{&array, 0, (int)(array.size() - 1)});

        // Wait for the array to be sorted
        waitForCompletion();
    }

   private:
    /**
     * @brief The Task struct represents the tasks that are given to a thread
     */
    struct Task {
        std::vector<T> *array;
        int lo;
        int hi;

        Task(std::vector<T> *array, int lo, int hi)
            : array(array), lo(lo), hi(hi) {}
    };

    const int MAX_THREADS =
        1000;  // Maximum number of threads so as not to overload the system
    std::vector<std::thread> workers;  // List of workers/threads
    std::queue<Task> tasks;            // List of tasks to be completed
    PcoMutex mutex;
    PcoConditionVariable cv, cvFinished;
    PcoConditionVariable isFree, isNotEmpty;
    unsigned int nbThreadsActive = 0;
    bool stop = false;

    /**
     * @brief quicksort is the actual sorting algorithm
     * @param array is the array to be sorted
     * @param lo is the index of the beginning of the portion for the current
     * thread
     * @param hi is the index of the end of the portion for the current thread
     */
    void quicksort(std::vector<T> &array, int lo, int hi) {
        // Check if invalid indexes
        if (lo >= hi || lo < 0) return;

        // Partition the correct portion
        int p = partition(array, lo, hi);

        put(Task{&array, lo, p - 1});
        put(Task{&array, p + 1, hi});
    };

    /**
     * @brief partition devides the array into two partitions
     * @param array from which the portion will be chosen
     * @param lo first index of portion
     * @param hi last index of portion
     * @return
     */
    int partition(std::vector<T> &array, int lo, int hi) {
        T pivot = array[hi];  // Define the last element as the pivot
        int i = lo;

        for (int j = lo; j < hi; j++) {
            // Check if the current element is less than or equal to the pivot
            if (array[j] <= pivot) {
                std::swap(array[i], array[j]);
                i++;
            }
        }
        // Swap the pivot and the last element
        std::swap(array[i], array[hi]);
        return i;
    };

    /**
     * @brief workerThread is the function executed by the threads that handle
     * the tasks
     */
    void workerThread() {
        while (true) {
            Task task(nullptr, 0, 0);

            mutex.lock();
            while(tasks.empty()) {
                if (stop) {
                    mutex.unlock();
                    return;
                }
                cv.wait(&mutex);
            }

            task = get();
            nbThreadsActive++;
            mutex.unlock();

            quicksort(*task.array, task.lo, task.hi);

            mutex.lock();

            nbThreadsActive--;
            
            mutex.unlock();

            mutex.lock();
            if (tasks.empty() && nbThreadsActive == 0) {
                cvFinished.notifyOne();
            }
            mutex.unlock();
        }
    };

    /**
     * @brief put is used to add a task to the buffer
     * @param task is the task to be added
     */
    void put(Task task) {
        mutex.lock();
        tasks.push(task);
        isNotEmpty.notifyOne();
        mutex.unlock();
    }

    /**
     * @brief get is to get the next task in the queue
     */
    Task get() {
        Task task(nullptr, 0, 0);
        while(tasks.empty()) {
            isNotEmpty.wait(&mutex);
        }
        task = tasks.front();
        tasks.pop();
        isFree.notifyOne();
        return task;
    }

    /**
     * @brief waitForCompletion is executed by the main process to wait until
     * the array is sorted
     */
    void waitForCompletion() {
        mutex.lock();
        // While threads are working, wait
        while (!tasks.empty()) {
            cvFinished.wait(&mutex);
        }

        // Once all threads have finished, stop
        stop = true;
        cv.notifyAll();

        mutex.unlock();

        // Wait for thread termination
        for (auto &worker : workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    };
};

#endif  // QUICKSORT_H
