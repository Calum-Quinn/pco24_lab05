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
    Quicksort(unsigned int nbThreads) : MultithreadedSort<T>(nbThreads) {}

    /**
     * @brief sort Manages the threads to sort the given sequence.
     * @param array is the sequence to sort
     */
    void sort(std::vector<T> &array) override {
        for(unsigned int i = 0; i < this->nbThreads; i++) {
            workers.emplace_back(&Quicksort::workerThread, this);
        }

        mutex.lock();
        tasks.push(Task{&array, 0, (int)( array.size() - 1)});
        nbThreadsActive++;
        cv.notifyOne();

        waitForCompletion();
    }

   private:
    struct Task {
        std::vector<T> *array;
        int lo;
        int hi;

        Task(std::vector<T> *array, int lo, int hi) : array(array), lo(lo), hi(hi) {}
    };

    std::vector<std::thread> workers;
    std::queue<Task> tasks;
    PcoMutex mutex;
    PcoConditionVariable cv, cvFinished;
    unsigned int nbThreadsActive = 0;
    bool stop = false;

    void quicksort(std::vector<T> &array, int lo, int hi) {
        if (lo >= hi || lo < 0) return;

        int p = partition(array, lo, hi);

        // On donne le travail à faire à un autre thread tant qu'il reste des threads disponibles
        if(nbThreadsActive < this->nbThreads) {
            
            tasks.push(Task{&array, lo, p - 1});
            tasks.push(Task{&array, p + 1, hi});
            nbThreadsActive += 2;
            cv.notifyAll();
        } else {
            quicksort(array, lo, p - 1);
            quicksort(array, p + 1, hi);
        }
    };

    int partition(std::vector<T> &array, int lo, int hi) {
        T pivot = array[hi];
        int i = lo;

        for (int j = lo; j < hi; j++) {
            if (array[j] < pivot) {
                std::swap(array[i], array[j]);
                i++;
            }
        }

        std::swap(array[i], array[hi]);

        return i;
    };

    void workerThread() {
        while (true) {
            Task task(nullptr, 0, 0);

            mutex.lock();
            while (tasks.empty() && !stop) {
                cv.wait(&mutex);
            }

            if (stop) {
                mutex.unlock();
                return;
            }

            task = tasks.front();

            tasks.pop();

            mutex.unlock();

            quicksort(*task.array, task.lo, task.hi);

            mutex.lock();
            nbThreadsActive--;
            if(tasks.empty() && nbThreadsActive == 0) {
                cvFinished.notifyOne();
            }
        }
    };

    void waitForCompletion() {
        mutex.lock();
        while (nbThreadsActive > 0) {
            cvFinished.wait(&mutex);
        }
        mutex.unlock();
    };
};

#endif  // QUICKSORT_H
