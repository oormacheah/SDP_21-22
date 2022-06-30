#include <iostream>
#include <thread>
#include <condition_variable>
#include <chrono>

#define N 5
#define T 30

std::mutex m;
int occupied_spaces = 0;
int countdown = T;

void car_coming() {
    int n;
    while (true) {
        std::unique_lock<std::mutex> cc_lock{m}; 
        // while loop defines a scope, so you can declare/define variables at every iteration
        if (countdown < 0) {
            cc_lock.unlock();
            break;
        }
        if (occupied_spaces == N) {
            std::cout << "Car arrived but parking full\n";
        }
        else {
            occupied_spaces++;
            std::cout << "Car arrived and parked, number of cars: " << occupied_spaces << std::endl;
            if (occupied_spaces == N) std::cout << "Parking is now full\n";
        }

        // generate number from 1 to 3
        n = (rand() % 3) + 1;
        std::cout << "Next car will arrive in " << n << " seconds\n"; 
        countdown -= n;
        
        cc_lock.unlock();
        std::this_thread::sleep_for (std::chrono::seconds(n));
    }
}

void car_leaving() {
    int n;
    while (true) {
        std::unique_lock<std::mutex> cl_lock{m};
        if (countdown < 0) {
            cl_lock.unlock();
            break;
        }
        if (occupied_spaces == 0) {
            std::cout << "There's no cars in the parking lot\n";
        }
        else {
            occupied_spaces--;
            std::cout << "Car fucked off somewhere else: number of cars: " << occupied_spaces << std::endl;
            if (occupied_spaces == 0) std::cout << "Parking lot is now empty. Gonna cry?\n";
        }
        n = (rand() % 4) + 4;
        std::cout << "Next car will leave in " << n << " seconds\n";
        countdown -= n;
        cl_lock.unlock();
        std::this_thread::sleep_for (std::chrono::seconds(n));
    }
}

int main() {

    std::cout << "PROGRAM START\n";

    std::thread t_coming(car_coming);
    std::thread t_leaving(car_leaving);

    t_coming.join();
    t_leaving.join();

    std::cout << "PROGRAM ENDED\n";

    return 0;
}