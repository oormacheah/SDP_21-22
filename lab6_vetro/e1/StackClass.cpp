#include "StackClass.h"
#include <iosfwd>
#include <iostream>
#include <exception>

template<typename T>
StackClass<T>::StackClass() {
    _dataContainer = nullptr;
    _size = 0;
}

template<typename T>
void StackClass<T>::push(const T &val) {
    T* oldDataContainer = _dataContainer;
    _size++;
    _dataContainer = new T [_size]; // classic array definition
    _dataContainer[0] = val;
    for(int i=1; i < _size; i++){
        _dataContainer[i] = oldDataContainer[i-1];
    }
    delete[] oldDataContainer;
}



template<typename T>
T StackClass<T>::pop() {
    if(_size == 0) {
        throw StackEmptyException();
    }
    T val = _dataContainer[0];
    T* oldDataContainer = _dataContainer;
    _size--;
    if(_size == 0) {
        _dataContainer = nullptr;
    }
    else {
        _dataContainer = new T[_size];
        for (int i = 0; i < _size; i++)
            _dataContainer[i] = oldDataContainer[i + 1];
    }
    delete[] oldDataContainer;
    return val;
}

template<typename T>
bool StackClass<T>::empty() const {
    return (_size==0);
}

template<typename T>
T* StackClass<T>::getDataContainer() {
    return _dataContainer;
}

template<typename T>
std::vector<T> StackClass<T>::getStackAsVector() {
    std::vector<T> dataVector;
    for(int i=0; i<_size; i++){
        dataVector.push_back(_dataContainer[i]);
    }
    return dataVector;
}

template<typename T>
int StackClass<T>::getSize() const {
    return _size;
}

template<typename T>
StackClass<T>::~StackClass() {
    delete[] _dataContainer;
    _dataContainer = nullptr;
    _size = 0;
}

//copy constructor
template<typename T>
StackClass<T>::StackClass(const StackClass<T> & copyStack) {
    _size = copyStack._size;
    _dataContainer = new T[_size];
    for(int i=0; i<_size; i++)
        _dataContainer[i] = copyStack._dataContainer[i];
}

//copy assignment
template<typename T>
StackClass<T>& StackClass<T>::operator=(const StackClass<T> & copyStack) {
    if(this == &copyStack) // this is a POINTER to this object itself, compare to the address operator of the
                           // passed reference, if it is literally equal, return the object contained by "this"

        return *this;
    delete[] _dataContainer;
    _size = copyStack._size;
    _dataContainer = new T[_size];
    for(int i=0; i<_size; i++)
        _dataContainer[i] = copyStack._dataContainer[i];
    return *this;
}

//move constructor
template<typename T>
StackClass<T>::StackClass(StackClass<T> && other) noexcept {
    _size = other._size;
    _dataContainer = other._dataContainer;
    other._size = 0;
    other._dataContainer = nullptr;
}

//move assignment
template<typename T>
StackClass<T>& StackClass<T>::operator=(StackClass<T> &&other) noexcept {
    if(this == &other) // if passed itself to the move assignment
        return *this;
    delete[] _dataContainer;
    _size = other._size;
    _dataContainer = other._dataContainer;
    other._size = 0;
    other._dataContainer = nullptr;
    return *this;
}

template<typename T>
void StackClass<T>::reverse() {
    StackClass<T> copy = *this; // call copy constructor
    for(int i=0, j=_size-1; i<_size; i++, j--){
        _dataContainer[i] = copy._dataContainer[j];
    }
}

template<typename T>
StackClass<T> StackClass<T>::operator+(const StackClass<T> &toAdd) {
    StackClass<T> output;
    output._size = _size + toAdd._size;
    output._dataContainer = new T[output._size];
    int i;
    for(i=0; i<_size; i++){
        output._dataContainer[i] = _dataContainer[i];
    }
    for(int j=0; j<toAdd._size; j++, i++){
        output._dataContainer[i] = toAdd._dataContainer[j];
    }
    return output;
}

template<typename T>
std::ostream & operator<<(std::ostream &os, const StackClass<T> &stack) {
    for(int i=0; i < stack._size; i++)
        os << stack._dataContainer[i] << std::endl;
    return os;
}
