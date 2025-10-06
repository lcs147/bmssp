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
    using elementT = pair<wT, int>;
    vector<list<elementT>> D0,D1;
    set<pair<int,int>> UBs;
    vector<int> blocks_UB;
    int M, B;
    unordered_map<wT,int> actual_value;
    unordered_map<wT, pair<int, typename list<elementT>::iterator>> where_is;
    
    // Initialize
    batchPQ(int M_, int B_): M(M_), B(B_) {
        D1.push_back(list<elementT>());
        UBs.insert({B,0});
        blocks_UB.push_back(B);
    }

    void delete_(elementT x){
        auto [a,b] = x;
        auto [id_block,it] = where_is[a];
        D1[id_block].erase(it);
        where_is.erase(a);

        if(D1[id_block].size() == 0){
            UBs.erase({blocks_UB[id_block],id_block});
        }
    }

    void insert(elementT x){
        auto [a,b] = x;

        int exist = actual_value.contains(a); 

        if(exist && actual_value[a] > b){
            delete_({a, actual_value[a]});
        }else if(exist){
            return;
        }

        auto it_block = UBs.lower_bound({b,-1});
        auto [ub,id_block] = (*it_block);

        D1[id_block].push_back(x);
        auto it = D1[id_block].end(); it--;
        where_is[a] = {id_block, it};
        actual_value[a] = b;

        if(D1[id_block].size() > M){
            //split();
        }
    }   

    void print(){
        cout <<  D1.size() << "\n";
        cout << D1[0].size() << "\n";
        cout << UBs.size() << "\n";

        for(auto[k,v]: D1[0]){
            cout << k << " " << v << "\n";
        }
        cout << "\n";
    }

    // unsigned size() {
    //     return s.size();
    // }

};

#endif
