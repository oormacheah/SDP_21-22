#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

mutex m;
int var = 0;
bool end_program = false;

void plus3() {
    while (true) {
        unique_lock<mutex> plus3lock{m};
        if (end_program) {
            plus3lock.unlock();
            break;
        }
        var += 3;
        plus3lock.unlock();

        this_thread::sleep_for (chrono::seconds(3));
    }
}

void minus1() {
    while (true) {
        unique_lock<mutex> minus1lock{m};
        if (end_program) {
            minus1lock.unlock();
            break;
        }
        var -= 1;
        minus1lock.unlock();

        this_thread::sleep_for (chrono::seconds(2));
    }
    
}

void check10() {
    while (true) {
        unique_lock<mutex> check10lock{m};
        if (var >= 10) {
            end_program = true;
            check10lock.unlock();
            break;
        }

        this_thread::sleep_for (chrono::seconds(10));
    }
}

int main() {
    thread t1{check10};
    thread t2{plus3};
    thread t3{minus1};

    t1.join();
    t2.join();
    t3.join();
    return 0;
}