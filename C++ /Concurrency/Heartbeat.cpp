#include<iostream>
#include<thread>
#include<chrono>

void heartbeat(std::stop_token st) {
    while (!st.stop_requested()) {
      
        std::cout << "Heartbeat [OK]\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
    }

    std::cout << "Heartbeat stopped.\n";
}

int main() {
    std::jthread hb_thread(heartbeat);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(400));
    
    std::cout << "Main shutting down\n";
    hb_thread.request_stop();
    
    // // Wait for the heartbeat thread to finish
    // hb_thread.join();
    
    std::cout << "Main thread exiting.\n";
    return 0;
}