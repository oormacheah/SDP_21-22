#include <iostream>
#include <pthread.h>

std::mutex m;
std::condition_variable
adminCV, adderCV;
int var = 0;
void
admin_f(){
    std::unique_lock<std::mutex> admin_lock{m}; // acquire the mutex
    var = 10;
    adderCV.notify_all();
    adminCV.wait(admin_lock);
    std::cout << "the sum is: " << var << std::endl;
}
void adder_f() {
    std::unique_lock<std::mutex> adder_lock{m};
    if (var == 0) {
        adderCV.wait(adder_lock);
    }
    if (var >= 15) {
        adminCV.notify_one();
        return;
    }

    int n = rand() % 5;
    var += n;
    std::cout << "var: " << var << " added: " << n << std::endl; 
    return;
}
int main() {
    int var=0;
    std::vector<std::thread> adders;
    std::thread admin(admin_f);

    for(int i = 0; i < 3; i++){
        srand((unsigned)time(NULL)); //makes seed different for calling rand()
        adders.emplace_back(std::thread(adder_f));
    }

    for(auto& i : adders) {
        i.join();
    }

    adminCV.notify_one(); // useless in case the adders reached 15 before
    admin.join();

    return 0;
}