These changes were made to the quicksort.h file:

## Deadlock in `workerThread`
- Problem:
Threads were stuck in a loop because the condition to exit on stop was not checked properly. When tasks was empty and stop was true, threads continued waiting indefinitely.

- Correction:
Modified the loop to exit immediately when stop is true:

```
while (tasks.empty()) {
    if (stop) {
        mutex.unlock();
        return;
    }
    cv.wait(&mutex);
}
```

## Threads not joined
- Problem:
The threads in workers were not joined before the function completed, leading to undefined behavior when the Quicksort object was destroyed.

- Correction:
Ensured all threads are joined after sorting is complete:

```
for (auto &worker : workers) {
    if (worker.joinable()) {
        worker.join();
    }
}
```

## Task queue management
- Problem:
Updates to tasks and nbThreadsActive were not synchronized properly, potentially leading to race conditions.

- Correction:
Added proper locking when modifying tasks and nbThreadsActive:

```
mutex.lock();
tasks.push(Task{&array, lo, p - 1});
tasks.push(Task{&array, p + 1, hi});
nbThreadsActive += 2;
cv.notifyAll();
mutex.unlock();
```

## Edge case in `waitForCompletion`
- Problem:
The condition in waitForCompletion relied on nbThreadsActive being updated correctly. Any synchronization issue in decrementing this counter could cause the loop to never exit.

- Correction:
Verified and protected updates to nbThreadsActive, ensuring they occur within locked sections. Also added stop = true to signal threads to terminate:

```
mutex.lock();
while (nbThreadsActive > 0) {
    cvFinished.wait(&mutex);
}
stop = true;
cv.notifyAll();
mutex.unlock();
```