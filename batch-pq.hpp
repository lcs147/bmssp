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
    unordered_map<int, pair< typename list<list<elementT>>::iterator , typename list<elementT>::iterator> > where_is[2];
    
    // Initialize
    batchPQ(int M_, int B_): M(M_), B(B_) { // O(1)
        D1.push_back(list<elementT>());
        UBs.insert({{B,oo,-1,-1},D1.begin()});
    }

    void delete_(uniqueDistT x){    // THINK MORE ABOUT THE DELETION
        uniqueDistT b = x;
        int a = get<2>(b);

        if(where_is[1].contains(a)){
            auto [it_block,it] = where_is[1][a];
            
            (*it_block).erase(it);
            where_is[1].erase(a);

            if((*it_block).size() == 0){
                auto it_UB_block = UBs.lower_bound({b,it_min});    
                UBs.erase(it_UB_block);
                D1.erase(it_block);
            }
        }else{
            auto [it_block,it] = where_is[0][a];
            (*it_block).erase(it);
            where_is[0].erase(a);
            if((*it_block).size() == 0) D0.erase(it_block); 
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

        where_is[1][a] = {it_block, it};
        actual_value[a] = b;

        // Checking if exceeds the sixe limit M
        if((*it_block).size() > M){
            split(it_block);
        }
    }   

    uniqueDistT medianOfMedians(list<elementT> l){
        int n = l.size();
        vector<elementT> v;
        v.reserve(5);
        
        auto it = l.begin();
        list<elementT> medians;

        while(it != l.end()){
            for(int j=0;j<5;j++){
                v.push_back(*it);
                it++;

                if(it == l.end()) break;
            }

            sort(v.begin(), v.end(), [](const auto& x, const auto& y) {
                return x.second < y.second;
            });

            medians.push_back(v[(v.size()/2)]);
            v.clear();
        }

        if(medians.size() == 1){
            return medians.front().second;
        }else{
            return medianOfMedians(medians);
        }
    }

    uniqueDistT selectMedian(list<elementT> &l, int k){

        uniqueDistT p = medianOfMedians(l);
        list<elementT> less,great;

        for(auto [a,b]: l){
            if(b < p){
                less.push_back({a,b});
            }else if(b > p){
                great.push_back({a,b});
            }
        }

        if(less.size() > k){
            return selectMedian(less,k);
        }else if(less.size() < k){
            return selectMedian(great, k - less.size() - 1);
        }else{
            return p;
        }
    }

    void split(list<list<elementT>>::iterator it_block){ // O(M) + O(lg(Block Numbers))
        int sz = (*it_block).size();
        uniqueDistT med = selectMedian((*it_block),(sz/2)); // O(M)
        
        auto pos = it_block;
        pos++;
        
        auto new_block = D1.insert(pos,list<elementT>());
        auto it = (*it_block).begin();

        while(it != (*it_block).end()){ // O(M)
            if((*it).second > med){
                (*new_block).push_back((*it));
                auto it_new = (*new_block).end(); it_new--;
                where_is[1][(*it).first] = {new_block, it_new};
    
                it = (*it_block).erase(it);
            }else{
                it++;
            }
        }
        
        // Updating UBs   
        // O(lg(Block Numbers))
        auto UB1 = (*new_block).front().second;
        auto it_lb = UBs.lower_bound({UB1,it_block});
        auto [UB2,aux] = (*it_lb);
        UBs.erase({UB2,it_block});

        UBs.insert({UB2,new_block});
        UBs.insert({UB1,it_block}); 
    }

    void batchPrepend(list<elementT> &l) { // O(|l| log(|l|/M) ) 
        int sz = l.size();
        
        if(sz <= M){
            
            D0.push_front(list<elementT>());
            auto new_block = D0.begin();

            for(auto x : l){
                int exist = actual_value.contains(x.first); 

                if(exist && actual_value[x.first] > x.second){
                    delete_(x.second);
                }else if(exist){
                    continue;
                }

                (*new_block).push_back(x);
                auto it_new = (*new_block).end(); it_new--;
                where_is[0][x.first] = {new_block, it_new};
                actual_value[x.first] = x.second;
            }

            return;
        }

        uniqueDistT med = selectMedian(l, sz/2);

        list<elementT> less,great;
        for(auto [a,b]: l){
            if(b <= med){
                less.push_back({a,b});
            }else if(b > med){
                great.push_back({a,b});
            }
        }

        batchPrepend(great);
        batchPrepend(less);
    }

    void batchPrepend(const vector<uniqueDistT> &v){
        list<elementT> l;
        int sz = v.size();
        for(auto x: v){
            l.push_back({get<2>(x),x});
        }

        batchPrepend(l);
    }

    pair<uniqueDistT, vector<int>> pull(){ // O(M)
        list<elementT> s0,s1;

        auto it_block = D0.begin();
        while(it_block != D0.end() && s0.size() < M){ // O(M)   
            for(auto x: (*it_block) ) s0.push_back(x);
            it_block++;
        }

        it_block = D1.begin();
        while(it_block != D1.end() && s1.size() < M){   //O(M)
            for(auto x: (*it_block) ) s1.push_back(x);
            it_block++;
        }

        if(s1.size() + s0.size() <= M){
            vector<int> ret;
            ret.reserve(s1.size()+s0.size());
            for(auto [a,b] : s0) {
                ret.push_back(get<2>(b));
                delete_({b});
            }
            for(auto [a,b] : s1){
                ret.push_back(get<2>(b));
                delete_({b});
            } 

            return {{B,oo,-1,-1}, ret};
        }else{
            list<elementT> l;
            for(auto x : s0) l.push_back(x);
            for(auto x : s1) l.push_back(x);
            
            uniqueDistT med = selectMedian(l, M);
            vector<int> ret;
            ret.reserve(M);
            for(auto [a,b]: l){
                if(b < med) {
                    ret.push_back(a);
                    delete_({b});
                }
            }
 
            return {med,ret};
        }
    }

    void print(uniqueDistT x){
        cout <<  get<0>(x) << " " << get<1>(x) << " " << get<2>(x) << " " << get<3>(x) << "\n";
    }

    void print(){
        cout <<  D1.size() << "\n";
        cout << UBs.size() << "\n";

        cout << "Sequence D1\n";
        int i = 0;
        for(auto block : D1){   
            cout << "Block " << i++ << " UB: " << "\n";
            for(auto [a,b]: block){
                cout << a << " " << get<0>(b) << " " << get<1>(b) << " " << get<2>(b) << " " << get<3>(b) << "\n";
            }
            cout << "\n";
        }


        cout << "Sequence D0\n";
        i = 0;
        for(auto block : D0){   
            cout << "Block " << i++ << " UB: " << "\n";
            for(auto [a,b]: block){
                cout << a << " " << get<0>(b) << " " << get<1>(b) << " " << get<2>(b) << " " << get<3>(b) << "\n";
            }
            cout << "\n";
        }

    }

};

#endif
