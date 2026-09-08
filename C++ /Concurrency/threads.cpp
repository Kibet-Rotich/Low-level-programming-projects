#include<iostream>
#include<thread>

void workerTask(){
std::cout << "Worker running... \n";
}
void badFunction(){
  std::thread t1(workerTask);
  
  bool error_occured = true;
  if(error_occured){
    throw std::runtime_error("Something broke!");
    
  }
  t1.join();
}


void goodFunction(){
  std::jthread t1([]{
    std::cout<<"Worker running safely... \n";
    });
    throw std::runtime_error("Something broke!");
    
}
int main(){

//badFunction();
goodFunction();

return 0;
}
  
