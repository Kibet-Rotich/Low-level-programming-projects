#include <iostream>
#include <thread>
#include <mutex>



std::mutex mts;

void thread_function(int id) {
    for (int i = 0; i < 5; ++i) {
        std::lock_guard<std::mutex> lock(mts);
        std::cout << "Thread " << id << " is working. Iteration: " << i + 1 << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int shared_counter = 0;
void increment_counter(int id) {
    for (int i = 0; i < 5; ++i) {
        std::lock_guard<std::mutex> lock(mts);
        ++shared_counter;
        std::cout << "Thread " << id << " incremented counter to: " << shared_counter << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void flexibleUpdate(){
    std::unique_lock<std::mutex> lock(mts);
    shared_counter ++;
    lock.unlock(); // Unlock before doing some work
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate some work
    lock.lock(); // Lock again to update shared resource
    shared_counter += 2;
    std::cout << "Flexible update done. Counter is now: " << shared_counter << std::endl;   

}


std::mutex mtx1, mtx2;
void thread1() {
    std::lock(mtx1, mtx2); // Lock both mutexes without risk of deadlock
    std::lock_guard<std::mutex> lock1(mtx1, std::adopt_lock);
    std::lock_guard<std::mutex> lock2(mtx2, std::adopt_lock);
    std::cout << "Thread 1 has locked both mutexes.\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}


int main() {
    std::cout << "--- Basic Mutex Locking ---\n";
    std::jthread t1(thread_function, 1);
    std::jthread t2(thread_function, 2);
  

    std::cout << "\n--- Shared Counter Increment ---\n";
    std::thread t3(increment_counter, 1);
    std::thread t4(increment_counter, 2);
    t3.join();
    t4.join();

    std::cout << "\n--- Flexible Update with Unique Lock ---\n";
    std::thread t5(flexibleUpdate);
    std::thread t6(flexibleUpdate);
    t5.join();
    t6.join();

    std::cout << "\n--- Deadlock Prevention with std::lock ---\n";
    std::thread t7(thread1);
    std::thread t8(thread1); // Both threads will try to lock the same mutexes
    t7.join();
    t8.join();

    return 0;
}