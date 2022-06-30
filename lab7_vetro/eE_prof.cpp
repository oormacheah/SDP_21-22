//
// Created by Antonio Vetrò on 24/05/22.
//

#include <iostream>
#include <future>
#include <vector>
#include <numeric>
#include <functional>
#include <fstream>

int computeSumOfProducts(const std::vector<int>& v1, const std::vector<int>& v2){
    return std::inner_product(v1.begin(), v1.end(), v2.begin(), 0);
}

void generateRandomMatrix(std::vector<std::vector<int>>& m, int nRow, int nCol){
    for (int i = 0; i < nRow; i++){
        std::vector<int> row;
        for (int j = 0; j < nCol; j++) {
            row.push_back(rand()%3);
        }
        m.push_back(row);
    }
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
    int nRowA = 2, nColA = 3,  nRowB = 3, nColB = 2;
    std::vector<std::vector<int>> a, b;
    std::vector<std::vector<std::future<int>>> futures;
    std::ofstream outputFile;

    std::cout << "Please insert number of rows and of columns of matrix A" << std::endl;
    std::cin >> nRowA;
    std::cin >> nColA;
    std::cout << "Please insert number of rows and of columns of matrix B" <<
     " (# of rows of B must be equal to  # of columns of A)" << std::endl;
    std::cin >> nRowB;
    std::cin >> nColB;

    if(nColA != nRowB) {
        std::cout << "Number of rows of B must be equal to and number of columns of A" << std::endl;
        return -1;
    }

    generateRandomMatrix(a, nRowA, nColA);
    generateRandomMatrix(b, nColB, nRowB); //generate the transpose of B (columns -> rows) so that we can easily access the columns of B for the multiplication


    //generate the futures that compute the products of rows and columns
    for (int i = 0; i < nRowA; i++){
        std::vector<std::future<int>> futureRow;
        for (int j = 0; j < nColB; j++) {
            std::future<int> f = std::async(std::launch::async | std::launch::deferred, computeSumOfProducts, a[i], b[j]);
            futureRow.push_back(std::move(f)); //futures are not copyable so we have to use move to store them in a vector
        }
        futures.push_back(std::move(futureRow));
    }

    printMatrix(a);
    std::cout << std::endl;
    printMatrix(b);
    std::cout << std::endl;

    outputFile.open("./output-matrix.txt");
    for (auto & row : futures){
        for (std::future<int> & f : row) {
            outputFile << f.get() << " ";
        }
        outputFile << std::endl;
    }
    outputFile.close();

    return 0;
}