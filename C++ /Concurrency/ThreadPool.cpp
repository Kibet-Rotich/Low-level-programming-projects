#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

class ThreadPool {
private:
    // 1. THE STATE
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    
    // std::atomic guarantees this boolean can be safely read/written 
    // by multiple threads simultaneously without needing a mutex.
    std::atomic<bool> stop;

    // 2. THE SYNCHRONIZATION PRIMITIVES
    std::mutex queue_mutex;
    std::condition_variable condition;

public:
    // --- THE CONSTRUCTOR (The Factory) ---
    explicit ThreadPool(size_t num_threads) : stop(false) {
        for (size_t i = 0; i < num_threads; ++i) {
            // We spawn the threads immediately.
            // Each thread is given a lambda function containing an infinite loop.
            workers.emplace_back([this]() {
                while (true) {
                    std::function<void()> task;

                    { // --- CRITICAL SECTION START ---
                        // 1. The thread tries to grab the lock.
                        std::unique_lock<std::mutex> lock(this->queue_mutex);

                        // 2. THE ALARM CLOCK
                        // condition.wait() automatically UNLOCKS the mutex and puts the thread to sleep 
                        // IF the queue is empty AND we aren't stopping.
                        // When it wakes up, it automatically RE-LOCKS the mutex.
                        this->condition.wait(lock, [this]() {
                            return this->stop || !this->tasks.empty();
                        });

                        // 3. THE EXIT CONDITION
                        // If the server is shutting down and the queue is empty, kill the thread.
                        if (this->stop && this->tasks.empty()) {
                            return; 
                        }

                        // 4. GRAB THE WORK
                        // We pop the task off the queue while safely holding the lock.
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                        
                    } // --- CRITICAL SECTION END (lock goes out of scope and releases) ---

                    // 5. EXECUTE THE WORK
                    // We execute the task OUTSIDE the lock. 
                    // If we kept the lock, only 1 thread could work at a time!
                    task();
                }
            });
        }
    }

    // --- THE INTERFACE (The Drop-off Point) ---
    void enqueue(std::function<void()> task) {
        {
            // 1. Grab the lock before touching the shared queue.
            std::unique_lock<std::mutex> lock(queue_mutex);
            
            // 2. Push the job onto the board.
            tasks.push(std::move(task));
        } // 3. Drop the lock.

        // 4. Ring the alarm clock! Wake up EXACTLY ONE sleeping thread.
        condition.notify_one(); 
    }

    // --- THE DESTRUCTOR (The Shutdown Sequence) ---
    ~ThreadPool() {
        // 1. Flip the kill switch.
        stop = true;

        // 2. Sound the alarm for EVERY sleeping thread so they wake up, 
        // see the stop flag, and gracefully exit their infinite loops.
        condition.notify_all();

        // 3. Wait for every thread to officially cross the finish line 
        // before we allow the program memory to be destroyed.
        for (std::thread &worker : workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }
};