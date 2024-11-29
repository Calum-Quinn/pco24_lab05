#ifndef QUICKSORT_H
#define QUICKSORT_H

#include <pcosynchro/pcoconditionvariable.h>
#include <pcosynchro/pcomutex.h>
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
    Quicksort(unsigned int nbThreads) : MultithreadedSort<T>(nbThreads) {
        // Check for invalid number of threads
        if (nbThreads == 0) {
            throw std::invalid_argument("Number of threads must not be 0.");
        }
        else if (nbThreads > MAX_THREADS) {
            throw std::invalid_argument("Number of threads must not be greater than " + std::to_string(MAX_THREADS));
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

        // Add the entire array to the list of taks and notify a worker
        mutex.lock();
        tasks.push(Task{&array, 0, (int)(array.size() - 1)});
        nbThreadsActive++;
        cv.notifyOne();
        mutex.unlock();

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

        Task(std::vector<T> *array, int lo, int hi) : array(array), lo(lo), hi(hi) {}
    };

    const int MAX_THREADS = 1000; // Maximum number of threads so as not to overload the system
    std::vector<std::thread> workers; // List of workers/threads
    std::queue<Task> tasks; // List of tasks to be completed
    PcoMutex mutex;
    PcoConditionVariable cv, cvFinished;
    unsigned int nbThreadsActive = 0;
    bool stop = false;

    /**
     * @brief quicksort is the actual sorting algorithm
     * @param array is the array to be sorted
     * @param lo is the index of the beginning of the portion for the current thread
     * @param hi is the index of the end of the portion for the current thread
     */
    void quicksort(std::vector<T> &array, int lo, int hi) {
        // Check if invalid indexes
        if (lo >= hi || lo < 0) return;

        // Partition the correct portion
        int p = partition(array, lo, hi);

        // Check if there are available threads
        if(nbThreadsActive < this->nbThreads) {
            mutex.lock();
            // Split the portion in 2 and add these portions to the list of tasks
            tasks.push(Task{&array, lo, p - 1});
            tasks.push(Task{&array, p + 1, hi});

            nbThreadsActive += 2; // DUBIOUS, SHOULD PROBABLY BE CHANGED TO CHECK IF 2 ARE AVAILABLE

            cv.notifyAll();
            mutex.unlock();
        } else {
            // Split the portion in 2 and recursively call the function
            quicksort(array, lo, p - 1);
            quicksort(array, p + 1, hi);
        }
    };

    /**
     * @brief partition devides the array into two partitions
     * @param array from which the portion will be chosen
     * @param lo first index of portion
     * @param hi last index of portion
     * @return
     */
    int partition(std::vector<T> &array, int lo, int hi) {
        T pivot = array[hi]; // Define the last element as the pivot
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
     * @brief workerThread is the function executed by the threads that handle the tasks
     */
    void workerThread() {
        while (true) {
            Task task(nullptr, 0, 0);

            mutex.lock();
            // If there are no tasks to be completed
            while (tasks.empty()) {
                if (stop) {
                    mutex.unlock();
                    return;
                }
                cv.wait(&mutex);
            }

            // Select the first available task
            task = tasks.front();
            tasks.pop();
            mutex.unlock();

            // Start the sorting process for the selected task
            quicksort(*task.array, task.lo, task.hi);

            mutex.lock();
            nbThreadsActive--;
            // Check if there are no active threads to notify of finished
            if (tasks.empty() && nbThreadsActive == 0) {
                cvFinished.notifyOne();
            }
            mutex.unlock();
        }
    };

    /**
     * @brief waitForCompletion is executed by the main process to wait until the array is sorted
     */
    void waitForCompletion() {
        mutex.lock();
        // While threads are working, wait
        while (nbThreadsActive > 0) {
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
