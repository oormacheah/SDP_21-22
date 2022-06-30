#include <thread>
#include <iostream>
#include <condition_variable>
#include <mutex>

int wallet = 3;
bool recharged = false; 

std::mutex m;
std::condition_variable rechargeCV;

void pay(int pay_amount) {
    std::unique_lock<std::mutex> pay_lock{m};
    std::cout << "Current balance: " << wallet << std::endl;
    while (!recharged) {
        std::cout << "Haven't recharged bitch\n";
        rechargeCV.wait(pay_lock);
    }
    std::cout << "New balance: " << wallet << std::endl;
    pay_lock.unlock();
    std::cout << "Paying...\n";
    wallet -= pay_amount;
}

void top_up_wallet() {
    std::unique_lock<std::mutex> recharge_lock{m};
    std::this_thread::sleep_for (std::chrono::seconds(5));
    wallet += 10;
    recharged = true;
    std::cout << "Recharged\n";
    rechargeCV.notify_one();
    recharge_lock.unlock();
}

int main() {
    std::thread a(pay,5);
    std::thread b(top_up_wallet);
    a.join();
    b.join();
    return 0;
}