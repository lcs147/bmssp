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
#include <limits>

using namespace std;

#define int long long


template<typename wT>
struct batchPQ {
    
    const int oo = 1e18;
    
    int n;
    
    using uniqueDistT = tuple<wT, int, int, int>; // dist, hops, u, pred[u]
    using elementT = pair<int,uniqueDistT>;
    
    
    struct CompareUB {
        template <typename It>
        bool operator()(const std::pair<uniqueDistT, It>& a, const std::pair<uniqueDistT, It>& b) const {
            return a.first < b.first;
        }
    };


    typename std::list<std::list<elementT>>::iterator it_min;

    list<list<elementT>> D0,D1;
    set<pair<uniqueDistT,typename list<list<elementT>>::iterator>,CompareUB> UBs; // (UB, it_block)
    
    int M, B;
    unordered_map<int, uniqueDistT> actual_value;
    unordered_map<int, pair< typename list<list<elementT>>::iterator , typename list<elementT>::iterator> > where_is;
    
    // Initialize
    batchPQ(int M_, int B_): M(M_), B(B_) { // O(1)
        D1.push_back(list<elementT>());
        UBs.insert({{B,oo,-1,-1},D1.begin()});
    }

    void delete_(uniqueDistT x){    // THINK MORE ABOUT THE DELETION
        uniqueDistT b = x;
        int a = get<2>(b);

        auto [it_block,it] = where_is[a];
        
        (*it_block).erase(it);
        where_is.erase(a);

        if((*it_block).size() == 0){
            auto it_UB_block = UBs.lower_bound({b,it_min});    
            UBs.erase(it_UB_block);
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
        auto it_UB_block = UBs.lower_bound({b,it_min});
        auto [ub,it_block] = (*it_UB_block);

        // Inserting key/value (a,b)
        (*it_block).push_back({a,b});
        auto it = (*it_block).end(); it--;

        where_is[a] = {it_block, it};
        actual_value[a] = b;

        // Checking if exceeds the sixe limit M
        if((*it_block).size() > M){
            cout << "Estorou\n";
            split(it_block);
        }
    }   

    void split(list<list<elementT>>::iterator it_block){ // O(M) + O(lg(Block Numbers))

        (*it_block).sort([](const auto& x, const auto& y) {
            return x.second < y.second;
        }); // O(M log M) WRONG WAY, CHANGE HERE

        
        int sz = (*it_block).size();
        
        
        auto pos = it_block;
        pos++;
        
        auto new_block = D1.insert(pos,list<elementT>());
        
        auto it = (*it_block).begin();

        for(int i=0;i<sz/2;i++){it++;} // O(M)

        auto UB1 = it->second;
        for(int i=sz/2;i<sz;i++){ // O(M)
            (*new_block).push_back((*it));
            auto it_new = (*new_block).end(); it_new--;
            where_is[(*it).first] = {new_block, it_new};

            it = (*it_block).erase(it);
        }

        // Updating UBs   
        // O(lg(Block Numbers))
        auto it_lb = UBs.lower_bound({UB1,it_block});
        auto [UB2,aux] = (*it_lb);
        UBs.erase({UB2,it_block});

        UBs.insert({UB2,new_block});
        UBs.insert({UB1,it_block}); 
    }

    void batchPrepend(const vector<uniqueDistT> &v) {
        
    }

    void print(){
        cout <<  D1.size() << "\n";
        cout << UBs.size() << "\n";

        int i = 0;
        for(auto block : D1){   
            cout << "Block " << i++ << " UB: " << "\n";
            for(auto [a,b]: block){
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
