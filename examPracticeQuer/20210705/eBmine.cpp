#include <iostream>
#include <thread>
#include <vector>
#include <future>

using namespace std;

void take_value(promise<int> &p_input) {
    int val;
    cout << "Enter a value:" << endl;
    cin >> val;
    p_input.set_value(val);
    return;
}

void check_if_prime(promise<bool> &p_result) {
    int val;
    bool result = true;
    promise<int> p_value;
    future<int> f_value = p_value.get_future();
    thread t1 = thread(take_value, ref(p_value));
    val = f_value.get();
    
    // check if prime ...
    for (int i=2; i<val; i++) {
        if ((val % i) == 0) {
            result = false;
        }
    }
    
    p_result.set_value(result);
    t1.join();
    return;
}

void give_answer() {
    promise<bool> p_answer;
    future<bool> f_answer = p_answer.get_future();
    thread t = thread(check_if_prime, ref(p_answer));
    bool answer = f_answer.get();
    string yes_no(" ");
    if (!answer) {
        yes_no = " NOT ";
    }
    cout << "The number is" << yes_no << "a prime\n";
    t.join();
    return;
}

int main() {
    thread t1(give_answer);
    t1.join();
    return 0;
}