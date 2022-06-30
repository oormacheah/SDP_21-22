//
// Created by Antonio Vetrò on 24/05/22.
//

#include <thread>
#include <iostream>
#include <chrono>
#include <mutex>

std::mutex m;

double targetTemp = 18.0;
double currentTemp = 18.5;
bool heating = false;


void f_targetTemp(){

    double temp=18.0;
    while(true) {

        std::cin >> temp ;
        std::unique_lock<std::mutex> tt_lock{m};
        targetTemp = temp;
        if(targetTemp == -1){
            tt_lock.unlock();
            break;
        }
        std::cout << "New target temperature set: " << targetTemp << std::endl;

        tt_lock.unlock();
        std::this_thread::sleep_for (std::chrono::seconds(5));
    }
    std::cout << "+++ target_temp thread finished +++"<< std::endl;

}


void f_currentTemp(){
    while(true) {

        std::unique_lock<std::mutex> h_lock{m};

        if(targetTemp == -1){
            h_lock.unlock();
            break;
        }

        if(heating==true){
            currentTemp= currentTemp+0.3;
            std::cout << "Temperature from " << currentTemp-0.3 << " to  " << currentTemp  << std::endl;
        }else{

            currentTemp= currentTemp-0.3;
            std::cout << "Temperature from " << currentTemp+0.3 << " to " << currentTemp  << std::endl;

        }
        h_lock.unlock();
        std::this_thread::sleep_for (std::chrono::seconds(5));

    }
    std::cout << "+++ current_temp thread finished +++"<< std::endl;

}

void admin(){
    while(true) {
        std::unique_lock<std::mutex> a_lock{m};
        if(targetTemp == -1){
            a_lock.unlock();
            break;
        }

        if(heating==true){
            if(currentTemp - targetTemp > 0.2){
                std::cout << "Target temp: " << targetTemp << " - Current temp " << currentTemp  << std::endl;
                heating = false;
                std::cout << "Heating system switched OFF "<< std::endl;
            }
        }else{ //heating is off
            if(currentTemp - targetTemp <= 0.2){
                std::cout << "Target temp: " << targetTemp << " - Current temp " << currentTemp  << std::endl;
                heating = true;
                std::cout << "Heating system switched ON "<< std::endl;

            }
        }

        a_lock.unlock();
        std::this_thread::sleep_for (std::chrono::seconds(3));
    }
    std::cout << "+++ admin thread finished +++"<< std::endl;
}

int main() {
    std::cout << "PROGRAM START" <<  std::endl;

    std::thread a(f_targetTemp);
    std::thread b(f_currentTemp);
    std::thread c(admin);

    a.join();
    b.join();
    c.join();

    std::cout << "FINISHED" << std::endl;
    return 0;
}



