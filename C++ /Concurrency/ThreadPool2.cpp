#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <chrono>
#include <syncstream> // C++20 for synchronized output

class ThreadPool {
private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    
    std::mutex queue_mutex_;
    std::condition_variable cv_;
    bool stop_;

public:
    // --- 1. CONSTRUCTOR ---
    ThreadPool(size_t num_threads) : stop_(false) {
        for (size_t i = 0; i < num_threads; ++i) {
            
            // We give each thread a lambda function representing its infinite loop
            workers_.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    
                    {
                        // TODO: Create a std::unique_lock on queue_mutex_
                        std::unique_lock<std::mutex> lock(queue_mutex_);
                        
                        // TODO: Make the thread wait on cv_. 
                        cv_.wait(lock, [this] {
                            return stop_ || !tasks_.empty();
                        });
                        // Hint: The predicate should return true if 'stop_' is true OR if 'tasks_' is NOT empty.
                        
                        // TODO: If 'stop_' is true AND 'tasks_' is empty, break out of the while loop to kill the thread.
                        if (stop_ && tasks_.empty()) {
                            break;
                        }
                        // TODO: Grab the front task from the queue, then pop it from the queue.
                        task = std::move(tasks_.front());
                        tasks_.pop();
                        
                    } // Mutex automatically unlocks here! (Crucial so other threads can access the queue while this one works)

                    // Execute the task outside the lock
                    task();
                }
            });
        }
    }

    // --- 2. ENQUEUE (The Producer) ---
    void enqueue(std::function<void()> task) {
        {
            // TODO: Create a std::lock_guard on queue_mutex_
            std::lock_guard<std::mutex> lock(queue_mutex_);
            
            // TODO: Push the task into the tasks_ queue
            tasks_.push(std::move(task));
            
        } // Mutex unlocks here
        
        // TODO: Notify ONE waiting thread via cv_ that work is available
        cv_.notify_one();
    }

    // --- 3. DESTRUCTOR (Graceful Teardown) ---
    ~ThreadPool() {
        {
            // TODO: Create a std::lock_guard on queue_mutex_
            std::lock_guard<std::mutex> lock(queue_mutex_);
            
            // TODO: Set stop_ to true
            stop_ = true;
            
        } // Mutex unlocks here
        
        // TODO: Notify ALL waiting threads via cv_ so they wake up and see the stop_ flag.
        // Hint: cv_.notify_all()
        cv_.notify_all();
        
        // TODO: Loop through workers_ and call .join() on each one to wait for them to finish.
        for (std::thread &worker : workers_) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }
};

// --- TEST HARNESS ---
int main() {
    std::cout << "Starting Thread Pool with 4 workers...\n";
    
    {
        ThreadPool pool(4);

        // Submit 10 simulated tasks
        for (int i = 1; i <= 10; ++i) {
            pool.enqueue([i] {
                std::osyncstream(std::cout) << "Task " << i 
                            << " executing on thread " 
                            << std::this_thread::get_id() << "\n";
                // Simulate some work taking 100ms
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            });
        }
        
        // Sleep the main thread for a bit to let tasks process
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
    } // pool goes out of scope here. The destructor is called.

    std::cout << "Thread pool successfully shut down.\n";
    return 0;
}