#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

std::queue<int> data_queue;
std::mutex mtx;
std::condition_variable cv;
bool finished = false;

void consumer() {
    std::unique_lock<std::mutex> lock(mtx);
    
    // cv.wait() atomically UNLOCKS the mutex and goes to sleep.
    // When notified, it wakes up, RELOCKS the mutex, and checks the lambda.
    // If the lambda is false (spurious wakeup), it goes back to sleep.
    cv.wait(lock, [] { 
        return !data_queue.empty() || finished; 
    });

    while (!data_queue.empty()) {
        std::cout << "Processed: " << data_queue.front() << "\n";
        data_queue.pop();
    }
}

void producer() {
    {
        std::lock_guard<std::mutex> lock(mtx);
        data_queue.push(42);
        data_queue.push(99);
        finished = true;
    } // Mutex unlocked here
    
    // Wake up the sleeping consumer thread
    cv.notify_one(); 
}

int main() {
    std::jthread t1(consumer);
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate delay
    std::jthread t2(producer);
    return 0;
}