#include <thread>
#include <iostream>
#include <vector>
#include <mutex>
#include <condition_variable>

std::mutex m;
std::condition_variable cvw;
int var=0;
bool finished = false;
void do_work(int i){
std::unique_lock<std::mutex> worker_lock{m};
std::cout << "Worker " << i << " started" << std::endl;
while(var<=0) {
std::cout << "Worker " << i << " waits" << std::endl;
cvw.wait(worker_lock);
}
var=var * i;
std::cout << "Worker " << i << " finished, var = " << var << std::endl;
worker_lock.unlock();
}
void assign_work(){
std::this_thread::sleep_for (std::chrono::seconds(1));
std::unique_lock<std::mutex> manager_lock{m};
std::cout << "Manager started" << std::endl;
var=var+1;
std::cout << "Manager assigned work, var = " << var << std::endl;
cvw.notify_all();
manager_lock.unlock();
}
int main() {
std::thread manager(assign_work);
std::vector<std::thread> workers;
for(int i=0; i<2; i++){
workers.emplace_back(std::thread(do_work,i));
}
for(auto& w : workers) {
w.join();
}
manager.join();
std::cout << "FINISHED. VAR = " << var << std::endl;
return 0;
}