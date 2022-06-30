#include <iostream>
#include <numeric>
#include <future>
#include <vector>
#include <functional>
#include <fstream>

int sumOfProducts(const std::vector<int> &a, const std::vector<int> &b) {
    return std::inner_product(a.begin(), a.end(), b.begin(), 0);
}

void printMatrix(const std::vector<std::vector<int>>& m){
    for (auto & row : m){
        for (int element : row) {
            std::cout << element << " ";
        }
        std::cout << std::endl;
    }
}

int main() {
    int rA, cA, i, j, n;
    std::cout << "Enter rows and columns for A:\n";
    std::cin >> rA >> cA;
    int rB, cB;
    std::cout << "Enter rows and columns for B:\n";
    std::cin >> rB >> cB;
    std::ofstream outputFile;
    if (cA != rB) {
        std::cout << "Dimensions are incompatible\n";
        return 0;
    }
    
    std::vector<std::vector<int>> A, B;

    for (i=0; i<rA; i++) {
        std::vector<int> row;
        for (j=0; j<cA; j++) {
            n = rand() % 5;        
            row.push_back(n);
        }
        A.push_back(row);
    }

    for (i=0; i<cB; i++) {
        std::vector<int> col;
        for (j=0; j<rB; j++) {
            n = rand() % 5;        
            col.push_back(n);
        }
        B.push_back(col);
    }

    std::vector<std::vector<std::future<int>>> futures;

    for (i=0; i<rA; i++) {
        std::vector<std::future<int>> futureRow;
        for (j=0; j<cB; j++) {
            std::future<int> fut = std::async(std::launch::async | std::launch::deferred, sumOfProducts, A[i], B[j]);
            futureRow.push_back(std::move(fut));
        }
        futures.push_back(std::move(futureRow));
    }

    std::cout << "A:\n";
    printMatrix(A);
    std::cout << "B:\n";
    printMatrix(B);

    outputFile.open("./output-matrix-mine.txt");
    for (auto & row : futures){
        for (std::future<int> & f : row) {
            outputFile << f.get() << " ";
        }
        outputFile << std::endl;
    }
    outputFile.close();
    
    return 0;
}