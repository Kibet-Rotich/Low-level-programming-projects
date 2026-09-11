#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <random>

std::atomic<int> global_max{0};
void update_maximum(int val){
    int current_max = global_max.load();
    if(val<=current_max){
        return;
    }

    while(val>current_max &&(!global_max.compare_exchange_weak(current_max,val))){
            // current_max = val;
            
        }
}
void worker(int seed) {
    std::mt19937 gen(seed);
    std::uniform_int_distribution<int> dist(1, 100000);

    for (int i = 0; i < 10000; ++i) {
        int val = dist(gen);
        update_maximum(val);
    }
}

int main() {
    {
        std::vector<std::jthread> threads;
        for (int i = 0; i < 8; ++i) {
            threads.emplace_back(worker, i + 1);
        }
    } // All threads join here

    std::cout << "Observed Global Max: " << global_max.load() << "\n";
    return 0;
}

