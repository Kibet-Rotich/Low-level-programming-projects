#include<atomic>
#include<iostream>
#include<vector>
#include<thread>

std::atomic<int> shared_val{10};
int sharedcounter = 10;

void lockFreeMultiplyByTwo(){
    int expected = shared_val.load();
    int desired = expected*2;

    while (!shared_val.compare_exchange_weak(expected,desired))
    {
        desired = expected*2;
    }

    // // 1. Read the value
    // int expected = shared_val.load();
    
    // // 2. ARTIFICIAL GAP: Tell the OS to let another thread run right now!
    // std::this_thread::yield(); 
    
    // // 3. Do the math and store it
    // int desired = expected * 2;
    // shared_val.store(desired);
    // std::this_thread::yield();
    // sharedcounter = sharedcounter *2;
    
}

int main(){

    {
        std::vector<std::jthread> workers;

        for(int i = 0;i<10;++i){
            workers.emplace_back(lockFreeMultiplyByTwo );
        }
    }

    
    
    std::cout << "Expected: 10240\n";
    std::cout << "Actual:   " << shared_val << "\n";
    return 0;
}