#pragma once

#include<bits/stdc++.h>
using namespace std;

template<typename uniqueDistT>
struct batchPQ { 
    // using uniqueDistT = tuple<wT, int, int, int>; // dist, hops, u, pred[u]
    using elementT = pair<int,uniqueDistT>;
    
    struct CompareUB {
        template <typename It>
        bool operator()(const std::pair<uniqueDistT, It>& a, const std::pair<uniqueDistT, It>& b) const {
            if (a.first != b.first) return a.first < b.first;
            return  std::addressof(*a.second) < std::addressof(*b.second);
        }
    };
    
    typename std::list<std::list<elementT>>::iterator it_min;
    
    list<list<elementT>> D0,D1;
    set<pair<uniqueDistT,typename list<list<elementT>>::iterator>,CompareUB> UBs; // (UB, it_block)
    
    int M,size_;
    uniqueDistT B;
    
    unordered_map<int, uniqueDistT> actual_value;
    unordered_map<int, pair< typename list<list<elementT>>::iterator , typename list<elementT>::iterator> > where_is[2];
    
    // Initialize
    batchPQ(int M_, uniqueDistT B_): M(M_), B(B_) { // O(1)
        D1.push_back(list<elementT>());
        UBs.insert({B_,D1.begin()});
        size_ = 0;
    }
    
    int size(){
        return size_;
    }
    
    void delete_(uniqueDistT x){    
        int a = get<2>(x);
        uniqueDistT b = actual_value[a];
        
        auto it_w = where_is[1].find(a);
        if((it_w != where_is[1].end())){
            auto [it_block,it] = it_w->second;
            
            (*it_block).erase(it);
            where_is[1].erase(a);
    
            if((*it_block).size() == 0){
                auto it_UB_block = UBs.upper_bound({b,it_block});  
                
                if((*it_UB_block).first != B){
                    UBs.erase(it_UB_block);
                    D1.erase(it_block);
                }
            }
        }else{
            auto [it_block,it] = where_is[0][a];
            (*it_block).erase(it);
            where_is[0].erase(a);
            if((*it_block).size() == 0) D0.erase(it_block); 
        }
    
        actual_value.erase(a);
        size_--;
    }
    
    void insert(uniqueDistT x){ // O(lg(Block Numbers))         
        uniqueDistT b = x;
        int a = get<2>(b);
    
        // checking if exists
        auto it_exist = actual_value.find(a);
        int exist = (it_exist != actual_value.end()); 
    
        if(exist && it_exist->second > b){
            delete_(x);
        }else if(exist){
            return;
        }
        
        // Searching for the first block with UB which is > 
        auto it_UB_block = UBs.upper_bound({b,it_min});
        auto [ub,it_block] = (*it_UB_block);
                
        // Inserting key/value (a,b)
        auto it = it_block->insert(it_block->end(),{a,b});
        where_is[1][a] = {it_block, it};
        actual_value[a] = b;
    
        size_++;
    
        // Checking if exceeds the sixe limit M
        if((*it_block).size() > M){
            split(it_block);
        }
    }   
    uniqueDistT medianOfMedians(vector<elementT> l) {
        while (true) {
            int n = l.size();

            if (n <= 5) {
                sort(l.begin(), l.end(), [](const auto& a, const auto& b) {
                    return a.second < b.second;
                });
                return l[l.size() / 2].second;
            }

            vector<elementT> medians;
            medians.reserve((n + 4) / 5);

            auto it = l.begin();
            while (it != l.end()) {
                vector<elementT> group;
                group.reserve(5);
                for (int j = 0; j < 5 && it != l.end(); ++j, ++it)
                    group.push_back(*it);

                sort(group.begin(), group.end(), [](const auto& a, const auto& b) {
                    return a.second < b.second;
                });

                medians.push_back(group[group.size() / 2]);
            }

            l = std::move(medians);
        }
    }
    uniqueDistT selectMedian(vector<elementT> l, int k){
        uniqueDistT p = medianOfMedians(l);
        vector<elementT> less,great;

        for (auto& e : l) {
            if (e.second < p) {
                less.push_back(e);   // adiciona a cópia
            } else if (e.second >= p) {
                great.push_back(e);
            }
        }

        int sz_less = less.size();

        if (sz_less > k){ 
            return selectMedian(less, k);
        }else if (sz_less < k){
            return selectMedian(great, k - sz_less - 1);
        }else{
            return p;
        }    
    }
        
    void split(list<list<elementT>>::iterator it_block){ // O(M) + O(lg(Block Numbers))
        int sz = (*it_block).size();
        
        vector<elementT> v((*it_block).begin() , (*it_block).end());
        uniqueDistT med = selectMedian(v,(sz/2)); // O(M)
        
        auto pos = it_block;
        pos++;


        auto new_block = D1.insert(pos,list<elementT>());
        auto it = (*it_block).begin();
    
        while(it != (*it_block).end()){ // O(M)
            if((*it).second >= med){
                // (*new_block).push_back((*it));
                (*new_block).push_back(std::move(*it));
                auto it_new = (*new_block).end(); it_new--;
                where_is[1][(*it).first] = {new_block, it_new};
    
                it = (*it_block).erase(it);
            }else{
                it++;
            }
        }
    

        // Updating UBs   
        // O(lg(Block Numbers))
        uniqueDistT UB1 = {get<0>(med),get<1>(med),get<2>(med),get<3>(med)-1};
        auto it_lb = UBs.upper_bound({UB1,it_min});
        auto [UB2,aux] = (*it_lb);
        
        UBs.insert({UB1,it_block});
        UBs.insert({UB2,new_block});
        
        UBs.erase(it_lb);
    }
    
    void batchPrepend(list<elementT> l) { // O(|l| log(|l|/M) ) 
        int sz = l.size();
        
        if(sz == 0) return;
        if(sz <= M){
    
            D0.push_front(list<elementT>());
            auto new_block = D0.begin();
            
            for(auto &x : l){ 
                auto it = actual_value.find(x.first);
                int exist = (it != actual_value.end()); 
    
                if(exist && it->second > x.second){
                    delete_(x.second);
                }else if(exist){
                    continue;
                }
    
                (*new_block).push_back(x);
                auto it_new = (*new_block).end(); it_new--;
                where_is[0][x.first] = {new_block, it_new};
                actual_value[x.first] = x.second;
                size_++;
            }
    
            return;
        }

        vector<elementT> v(l.begin(), l.end());
        uniqueDistT med = selectMedian( v, sz/2);
    
        list<elementT> less,great;
        for(auto [a,b]: l){
            if(b < med){
                less.push_back({a,b});
            }else if(b > med){
                great.push_back({a,b});
            }
        }
        
        great.push_back({get<2>(med),med});

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
    
    pair<uniqueDistT, vector<uniqueDistT>> pull(){ // O(M)
        list<elementT> s0,s1;
    
        auto it_block = D0.begin();
        while(it_block != D0.end() && s0.size() <= M){ // O(M)   
            for (const auto& x : *it_block) s0.push_back(x);
            it_block++;
        }
    
        it_block = D1.begin();
        while(it_block != D1.end() && s1.size() <= M){   //O(M)
            for (const auto& x : *it_block) s1.push_back(x);
            it_block++;
        }
    
        if(s1.size() + s0.size() <= M){
            vector<uniqueDistT> ret;
            ret.reserve(s1.size()+s0.size());
            for(auto [a,b] : s0) {
                ret.push_back(b);
                delete_({b});
            }
            for(auto [a,b] : s1){
                ret.push_back(b);
                delete_({b});
            } 
    
            return {B, ret};
        }else{  
            vector<elementT> l;
            for(auto x : s0) l.push_back(x);
            for(auto x : s1) l.push_back(x);
            uniqueDistT med = selectMedian(l, M);
            vector<uniqueDistT> ret;
            ret.reserve(M);
            for(auto [a,b]: l){
                if(b < med) {
                    ret.push_back(b);
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
        for(auto &block : D1){   
            cout << "Block " << i++ << " UB: " << std::addressof(block) << "\n";
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