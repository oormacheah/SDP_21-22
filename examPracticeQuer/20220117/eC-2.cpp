/*
 * Created by Stefano Quert 01.06.2022
 * Alternative solution:
 * Instead of running two threads to control the parking slot, we create one thread for each car.
 * We specify the number of cars entering the parking slot instead of the time.
 * To make the simulation more realistic every car need TIME_IN seconds to enter and TIME_OUT seconds
 * to leave the parking slot.
*/

#include <thread>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <vector>

std::mutex m_inout;
std::condition_variable cv_wait;

int TIME_T1_I = 1;
int TIME_T1_D = 3;
int TIME_T2_I = 4;
int TIME_T2_D = 4;
int TIME_IN   = 1;
int TIME_OUT  = 2;

int PLACES_MAX  = 5;
int CARS_N      = 10;
int cars_tmp    = 0;
int cars_total  = 0;

void my_print (int i) {
    for (int j=0; j<2*i; j++)
        std::cout << " ";
    return;
}

void car (int i) {
    int t;

    // Waiting before entering
    t = TIME_T1_I + rand() % TIME_T1_D;
    std::this_thread::sleep_for(std::chrono::seconds(t));

    // Entering
    {
        std::unique_lock<std::mutex> enter{m_inout};

        // Wait for initialization on the conditional variable
        while (cars_tmp >= PLACES_MAX) {
            my_print (i);
            std::cout << "W: " << i << " (waiting, parking full)" << std::endl;
            cv_wait.wait(enter);
        }

        cars_tmp++;
        cars_total++;
        my_print (i);
        std::cout << "E. " << i << " (entering)" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(TIME_IN));
        my_print (i);
        std::cout << "P. " << i << " (parking; tmp=" << cars_tmp << ", total=" << cars_total << ")" << std::endl;
    }

    // Waiting (in the parking slot)
    t = TIME_T1_I + rand() % TIME_T1_D;
    std::this_thread::sleep_for(std::chrono::seconds(t));

    // Leaving
    m_inout.lock();
    cars_tmp--;
    my_print (i);
    std::cout << "L. " << i << " (leaving; tmp=" << cars_tmp << ", total=" << cars_total << ")" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(TIME_OUT));
    // There may be many car waiting (I can also count the waiting ones)
    cv_wait.notify_all();
    m_inout.unlock();

    return;
}

int main() {
   std::vector<std::thread> cars;

   std::cout << "PROGRAM START" <<  std::endl;

   for(int i=0; i < CARS_N; i++) {
       cars.emplace_back (std::thread (car, i));
   }

  for(auto& i : cars) {
      i.join();
  }

  std::cout << "FINISHED" << std::endl;
  return 0;
}
