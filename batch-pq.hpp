#ifndef BATCHPQ_HPP
#define BATCHPQ_HPP

#include<list>
#include <vector>
#include <set>
#include <map>
#include <cassert>
#include<iostream>
#include <unordered_map>
#include <utility>

using namespace std;

template<typename wT>
struct batchPQ {
    int n;
    
    using uniqueDistT = tuple<wT, int, int, int>; // dist, hops, u, pred[u]
    using elementT = pair<int,uniqueDistT>;

    vector<list<elementT>> D0,D1;
    set<pair<int,int>> UBs; // (UB, id_block)
    vector<int> blocks_UB;
    int M, B;
    unordered_map<int, uniqueDistT> actual_value;
    unordered_map<int, pair<int, typename list<elementT>::iterator>> where_is;
    
    // Initialize
    batchPQ(int M_, int B_): M(M_), B(B_) { // O(1)
        D1.push_back(list<elementT>());
        UBs.insert({B,0});
        blocks_UB.push_back(B);
    }

    void delete_(uniqueDistT x){        // THINK MORE ABOUT THE DELETION
        uniqueDistT b = x;
        int a = get<2>(b);

        auto [id_block,it] = where_is[a];
        D1[id_block].erase(it);
        where_is.erase(a);

        if(D1[id_block].size() == 0){
            UBs.erase({blocks_UB[id_block],id_block});
        }
    }

    void insert(uniqueDistT x){ // O(lg(Block Numbers))
        uniqueDistT b = x;
        int a = get<2>(b);

        // checking if exists
        int exist = actual_value.contains(a); 

        if(exist && actual_value[a] > b){
            delete_(x);
        }else if(exist){
            return;
        }

        // Searching for the first block with UB which is >= 
        auto it_block = UBs.lower_bound({get<0> (b),-1});
        auto [ub,id_block] = (*it_block);

        // Inserting key/value (a,b)
        D1[id_block].push_back({a,b});
        auto it = D1[id_block].end(); it--;

        where_is[a] = {id_block, it};
        actual_value[a] = b;

        // Checking if exceeds the sixe limit M
        if(D1[id_block].size() > M){
            cout << "Estorou\n";
            split(id_block);
        }
    }   

    void split(int id_block){ // O(M) + O(lg(Block Numbers))
        D1[id_block].sort([](const auto& x, const auto& y) {
            return x.second < y.second;
        }); // O(M log M) WRONG WAY, CHANGE HERE

        int sz = D1[id_block].size();

        D1.push_back(list<elementT>());
        int new_block = D1.size() - 1;    

        auto it = D1[id_block].begin();

        for(int i=0;i<sz/2;i++){it++;} // O(M)

        int UB1 = get<0>(it->second);
        for(int i=sz/2;i<sz;i++){ // O(M)
            D1[new_block].push_back((*it));
            auto it_new = D1[new_block].end(); it_new--;
            where_is[(*it).first] = {new_block, it_new};

            it = D1[id_block].erase(it);
        }

        // Updating UBs   
        // O(lg(Block Numbers))
        int UB2 = blocks_UB[id_block];

        UBs.insert({UB2,new_block});
        blocks_UB.push_back(UB2);

        UBs.erase({blocks_UB[id_block],id_block});
        UBs.insert({UB1,id_block});
        blocks_UB[id_block] = UB1;
    }

    void print(){
        cout <<  D1.size() << "\n";
        cout << UBs.size() << "\n";

        for(int i=0;i<D1.size();i++){
            
            cout << "Block " << i << " UB: " <<  blocks_UB[i] << "\n";
            for(auto [a,b]: D1[i]){
                cout << a << " " << get<0>(b) << " " << get<1>(b) << " " << get<2>(b) << " " << get<3>(b) << "\n";
            }
            cout << "\n";
        }
    }

    // unsigned size() {
    //     return s.size();
    // }

};

#endif
