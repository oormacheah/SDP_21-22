#include <iostream>
#include <thread>
#include <mutex>

// Heating system problem

std::mutex m;
bool on_off = false;
float des_temp = 18;
float current_temp = 18.5;

void targetTemp() {
    while (true) {
        std::cin >> des_temp; // blocking, so won't attempt to acquire the mutex until you prompt
        std::unique_lock<std::mutex> targetTLock{m};
        if (des_temp == -1) {
            targetTLock.unlock();
            break;
        }
        std::cout << "Set new target temp: " << des_temp << std::endl;

        targetTLock.unlock();
        std::this_thread::sleep_for (std::chrono::seconds(5));
    }
    return;
}

void currentTemp() {
    std::cout << "Starting temp: " << current_temp << std::endl;
    while (true) {
        std::unique_lock<std::mutex> currentTLock{m};
        if (des_temp == -1) {
            currentTLock.unlock();
            break;
        }
        if (on_off) {
            current_temp += 0.3;
            std::cout << "Current temp increased to " << current_temp << std::endl;
        }
        else {
            current_temp -= 0.3;
            std::cout << "Current temp decreased to " << current_temp << std::endl;
        }

        currentTLock.unlock();

        std::this_thread::sleep_for (std::chrono::seconds(5));
    }
    return;
}

void admin() {
    while (true) {
        std::unique_lock<std::mutex> adminLock{m};
        if (des_temp == -1) {
            adminLock.unlock();
            break;
        }

        if (on_off) {
            if ((current_temp - des_temp) > 0.2) {
                on_off = false;
            }
        }
        else {
            if ((des_temp - current_temp) >= 0.2) {
                on_off = true;
            }
        }

        adminLock.unlock();
        std::this_thread::sleep_for (std::chrono::seconds(3));
    }
    return;
}

int main() {

    std::cout << "PROGRAM STARTED\n";

    std::thread tTarget(targetTemp);
    std::thread tCurrent(currentTemp);
    std::thread tAdmin(admin);

    tTarget.join();
    tCurrent.join();
    tAdmin.join();

    std::cout << "PROGRAM ENDED\n";
}