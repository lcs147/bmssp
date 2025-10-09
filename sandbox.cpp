#include "batch-pq.hpp"
#include <iostream>

using namespace std;
using uniqueDistT = tuple<int, int, int, int>;

signed main() {
    batchPQ<int> pq(4, 20);

     // dist, hops, u, pred[u]


    pq.insert({2,3,1,-1});
    pq.insert({3,2,2,1});
    pq.insert({5,2,2,2});
    pq.insert({1,2,2,3});
    pq.insert({4,2,3,2});
    pq.insert({7,2,4,2});
    pq.insert({2,2,5,2});
    pq.insert({9,2,6,2});
    pq.insert({5,2,7,2});
    pq.insert({19,2,8,2});
    pq.insert({3,2,9,2});
    pq.insert({10,2,10,2});
    pq.insert({7,2,11,2});
    pq.insert({9,2,12,2});
    pq.insert({17,2,13,2});
    

    vector<uniqueDistT> v;
    v.push_back({2,3,14,-1});
    v.push_back({3,2,15,1});
    v.push_back({5,2,15,2});
    v.push_back({1,2,15,3});
    v.push_back({4,2,16,2});
    v.push_back({7,2,17,2});
    v.push_back({2,2,18,2});
    v.push_back({9,2,19,2});
    v.push_back({5,2,20,2});
    v.push_back({19,2,21,2});
    v.push_back({3,2,22,2});
    v.push_back({10,2,23,2});
    v.push_back({7,2,24,2});
    v.push_back({9,2,25,2});
    v.push_back({17,2,26,2});

    pq.batchPrepend(v);

    pq.print();

    auto [b,v2] = pq.pull();

    cout << "\n\n PULL\n"; 
    pq.print(b);
    for(int it: v2){
        cout << it << "\n";
    }

    cout << "\n\n";
    pq.print();

    return 0;
}
