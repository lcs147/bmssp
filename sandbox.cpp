#include "batch-pq.hpp"
#include <iostream>

using namespace std;

int main() {
    batchPQ<int> pq(2, 10);

     // dist, hops, u, pred[u]

    pq.insert({2,3,1,-1});
    pq.insert({3,2,2,1});
    pq.insert({5,2,2,2});
    pq.insert({1,2,2,3});
    pq.insert({4,2,3,2});

    pq.print();

    return 0;
}
