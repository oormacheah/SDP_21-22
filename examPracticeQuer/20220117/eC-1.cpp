//
// Created by Antonio Vetrò on 24/05/22.
//

#include <thread>
#include <iostream>
#include <chrono>
#include <mutex>

std::mutex m;

int MAX_PLACES = 5;
int nr_cars = 0;
bool running = true;
int countdown = 30;

void carComing(){

    while(true) {
        std::unique_lock<std::mutex> cc_lock{m};

        if(countdown<0)
            running=false;

        if(!running){
            cc_lock.unlock();
            break;
        }

        if(nr_cars < MAX_PLACES ){
            nr_cars ++;
            std::cout << "Car arrived and parked. Number of cars: " << nr_cars << std::endl;
        } else{
            std::cout << "Car arrived but parking lot is full" << std::endl;
        }

        int r = rand() %3 + 1;   /* generate  number between 1 and 3: */
        std::cout << "(next car will arrive in " << r << " seconds)" << std::endl;
        countdown-=r;
        cc_lock.unlock();

        std::this_thread::sleep_for (std::chrono::seconds(r));
    }
}


void carLeaving(){

    while(true) {
        std::unique_lock<std::mutex> cl_lock{m};

        if(!running){
            cl_lock.unlock();
            break;
        }
        if(nr_cars > 0 ) {
            nr_cars--;
            std::cout << "Car left. Number of cars: " << nr_cars << std::endl;
        } else{
            std::cout << "Parking lot empty"  << std::endl;
        }


        int r = rand() %4 + 4;   /* generate  number between 4 and 7: */
        std::cout << "(next car will leave in  " << r << " seconds)" << std::endl;
        cl_lock.unlock();
        std::this_thread::sleep_for (std::chrono::seconds(r));
    }

}


int main() {

    std::cout << "PROGRAM START" <<  std::endl;


    std::thread a(carComing);
    std::thread b(carLeaving);


    a.join();
    b.join();


    std::cout << "FINISHED" << std::endl;
    return 0;
}



