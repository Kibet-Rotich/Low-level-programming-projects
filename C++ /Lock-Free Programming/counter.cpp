#include<atomic>
#include<iostream>
#include<thread>
#include<vector>

std::atomic<int> shared_counter{0};
// int shared_counter = 0;
void lockfreeincrement(){
    for(int i = 0; i<100000;++i){
        shared_counter++;
    }
}


int main(){
    std::cout << "Spawning threads...\n";
    
    {
        std::vector<std::jthread> workers;
        for (int i = 0; i < 10; ++i) {
            workers.emplace_back(lockfreeincrement);
        }
    }
    std::cout << "Expected: 1000000\n";
    std::cout << "Actual:   " << shared_counter << "\n";


}