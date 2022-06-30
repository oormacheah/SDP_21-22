#include "Queue.h"
#include <iostream>

Queue::Queue() {
    _queue = nullptr;
    _size = 0;
}

enum Client Queue::pop() {
    if(_size == 0)
        throw QueueEmptyException();
    enum Client val = _queue[0];
    enum Client* oldQueue = _queue;
    _size--;
    if(_size == 0) {
        _queue = nullptr;
    }
    else{
        _queue = new enum Client[_size];
        for(int i = 0; i < _size; i++)
            _queue[i] = oldQueue[i+1];
        delete[] oldQueue;
    }
    return val;
}

bool Queue::empty() {
    return (_size==0);
}

void Queue::_pushAt(enum Client value, int position) {
    enum Client* oldQueue = _queue;
    ++_size;
    _queue = new enum Client[_size];
    _queue[position] = value;
    for(int i=0; i<position; i++)
        _queue[i] = oldQueue[i];
    for(int i=position+1; i<_size; i++)
        _queue[i] = oldQueue[i-1];
    delete[] oldQueue;
}

void Queue::printQueue() {
    for(int i=0; i < _size; i++)
        std::cout << i << " " << _queue[i] << std::endl;

}

int Queue::push(enum Client client) {
    int i, position;
    switch(client) {
        case PRIORITY:
            for (i = 0; i < _size; i++) {
                if (_queue[i] != PRIORITY)
                    break;
            }
            position = i;
            _pushAt(client, position);
            break;
        case MONEY:
            position = _size;
            _pushAt(client, position);
            break;
        case POSTAL:
            int nMoney = 0;
            for(i = 0; i < _size; i++) {
                if (_queue[i] == MONEY)
                    ++nMoney;
                if (nMoney > 3)
                    break;
            }
            position = i;
            _pushAt(client, position);
            break;
            }
        return position;
    }




