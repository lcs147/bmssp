#include "batch-pq.hpp"
#include <iostream>

using namespace std;

int main() {
    batchPQ<int> pq(2, 10);

    pq.insert({1,2});
    pq.insert({2,3});
    pq.insert({2,5});
    pq.insert({2,1});
     pq.insert({3,4});


    pq.print();
    return 0;
}
