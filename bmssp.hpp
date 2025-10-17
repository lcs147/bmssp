#pragma once

#include<vector>
#include<list>
#include<utility>
#include<limits>
#include<set>
#include<iostream>
#include<queue>
#include<unordered_map>
#include<unordered_set>
#include<random>
#include<ext/pb_ds/assoc_container.hpp>

namespace spp {

using namespace std;
using namespace __gnu_pbds;

template<typename K, typename V>
using hash_map = unordered_map<K, V>;
template<typename K>
using hash_set = unordered_set<K>;

template<typename uniqueDistT>
struct batchPQ { // batch priority queue, implemented as in Lemma 3.3
    using elementT = pair<int,uniqueDistT>;
    
    struct CompareUB {
        template <typename It>
        bool operator()(const pair<uniqueDistT, It>& a, const pair<uniqueDistT, It>& b) const {
            if (a.first != b.first) return a.first < b.first;
            return  addressof(*a.second) < addressof(*b.second);
        }
    };
    
    typename list<list<elementT>>::iterator it_min;
    
    list<list<elementT>> D0,D1;
    set<pair<uniqueDistT,typename list<list<elementT>>::iterator>,CompareUB> UBs; // (UB, it_block)
    
    int M,size_;
    uniqueDistT B;
    
    hash_map<int, uniqueDistT> actual_value;
    hash_map<int, pair< typename list<list<elementT>>::iterator , typename list<elementT>::iterator> > where_is[2];
    
    // Initialize
    batchPQ(int M_, uniqueDistT B_): M(M_), B(B_) { // O(1)
        D1.push_back(list<elementT>());
        UBs.insert({B_,D1.begin()});
        size_ = 0;
    }
    
    int size(){
        return size_;
    }
    
    inline void erase(int key) {
        if(actual_value.find(key) != actual_value.end())
            delete_({-1, -1, key, -1});
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
                auto it_UB_block = UBs.lower_bound({b,it_block});  
                
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
        auto it_UB_block = UBs.lower_bound({b,it_min});
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

    uniqueDistT medianOfMedians(vector<elementT>::iterator bg, vector<elementT>::iterator en) {
        vector<elementT> l(bg, en);
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

            l = move(medians);
        }
    }
    
    uniqueDistT selectKth(vector<elementT> &v, int k) { 
        using std::swap;
        int l=0, r=v.size()-1;
        
        for(;l<=r;){
            uniqueDistT p = medianOfMedians(v.begin() + l, v.begin() + r + 1);
            
            int i = l, j = r, m = l;
            
            while(m <= j){
                if(v[m].second < p) {
                    swap(v[m++],v[i++]);
                }else if(v[m].second > p){
                    swap(v[m],v[j--]);
                }else{
                    m++;
                }
            }
        
            if (k < i) r = i - 1;     // k está na parte menor
            else if (k > j) l = j + 1;     // k está na parte maior
            else return v[k].second; // k está entre os iguais ao pivot
        } 
        
        return v[k].second;
    }

        
    void split(list<list<elementT>>::iterator it_block){ // O(M) + O(lg(Block Numbers))
        int sz = (*it_block).size();
        
        vector<elementT> v((*it_block).begin() , (*it_block).end());
        uniqueDistT med = selectKth(v,(sz/2)); // O(M)
        
        auto pos = it_block;
        pos++;


        auto new_block = D1.insert(pos,list<elementT>());
        auto it = (*it_block).begin();
    
        while(it != (*it_block).end()){ // O(M)
            if((*it).second >= med){
                // (*new_block).push_back((*it));
                (*new_block).push_back(move(*it));
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
        auto it_lb = UBs.lower_bound({UB1,it_min});
        auto [UB2,aux] = (*it_lb);
        
        UBs.insert({UB1,it_block});
        UBs.insert({UB2,new_block});
        
        UBs.erase(it_lb);
    }
    
    void batchPrepend(const list<elementT> &l) { // O(|l| log(|l|/M) ) 
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
        uniqueDistT med = selectKth(v, sz/2);
    
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
        for(auto x: v){
            l.push_back({get<2>(x),x});
        }
        batchPrepend(l);
    }
    
    pair<uniqueDistT, vector<int>> pull(){ // O(M)
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
            vector<int> ret;
            ret.reserve(s1.size()+s0.size());
            for(auto [a,b] : s0) {
                ret.push_back(a);
                delete_({b});
            }
            for(auto [a,b] : s1){
                ret.push_back(a);
                delete_({b});
            } 
    
            return {B, ret};
        }else{  
            vector<elementT> l;
            l.reserve(s0.size() + s1.size());
            for(auto x : s0) l.push_back(x);
            for(auto x : s1) l.push_back(x);
            uniqueDistT med = selectKth(l, M);
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
};

template<typename wT>
struct bmssp { // bmssp class
    int n, k, t;
    const wT oo = numeric_limits<wT>::max() / 10;
 
    vector<vector<pair<int, wT>>> ori_adj;
    vector<vector<pair<int, wT>>> adj;
    vector<wT> d;
    vector<int> pred, path_sz;
    vector<int> rev_map;
    vector<short int> last_complete_lvl;
 
    vector<hash_map<int, int>> neig;
    bmssp(int n_): n(n_) {
        ori_adj.assign(n, {});
        neig.assign(n, {});
    }
    bmssp(const auto &adj) {
        n = adj.size();
        ori_adj = adj;
        neig.assign(n, {});
    }
    void addEdge(int a, int b, wT w) {
        ori_adj[a].emplace_back(b, w);
    }

    // if the graph already has constant degree, prepage_graph(false)
    // else, prepage_graph(true)
    void prepare_graph(bool exec_constant_degree_trasnformation = false) {
        // erase duplicated edges
        vector<pair<int, int>> tmp_edges(n, {-1, -1});
        for(int i = 0; i < n; i++) {
            vector<pair<int, wT>> nw_adj;
            nw_adj.reserve(ori_adj[i].size());
            for(auto [j, w]: ori_adj[i]) {
                if(tmp_edges[j].first != i) {
                    nw_adj.emplace_back(j, w);
                    tmp_edges[j] = {i, nw_adj.size() - 1};
                } else {
                    int id = tmp_edges[j].second;
                    nw_adj[id].second = min(nw_adj[id].second, w);
                }
            }
            ori_adj[i] = move(nw_adj);
        }
        tmp_edges.clear();

        if(exec_constant_degree_trasnformation == false) {
            adj = move(ori_adj);
            ori_adj.clear();
            d.resize(n);
            root.resize(n);
            pred.resize(n);
            treesz.resize(n);
            rev_map.resize(n);
            path_sz.resize(n, 0);
            last_complete_lvl.resize(n);
            
            for(int i = 0; i < n; i++) {
                neig[i][i] = i;
                rev_map[i] = i;
            }

            k = floor(pow(log2(n), 1.0 / 3.0));
            t = floor(pow(log2(n), 2.0 / 3.0));
        } else {
            // Make the graph become constant degree
            int cnt = 0;
            for(int i = 0; i < n; i++) {
                for(auto [j, w]: ori_adj[i]) {
                    if(neig[i].find(j) == neig[i].end()) {
                        neig[i][j] = cnt++;
                        neig[j][i] = cnt++;
                    }
                }
            }

            cnt++;
            adj.assign(cnt, {});
            
            d.resize(cnt);
            root.resize(cnt);
            pred.resize(cnt);
            treesz.resize(cnt);
            rev_map.resize(cnt);
            path_sz.resize(cnt, 0);
            last_complete_lvl.resize(cnt);
    
            for(int i = 0; i < n; i++) { // create 0-weight cycles
                for(auto cur = neig[i].begin(); cur != neig[i].end(); cur++) {
                    auto nxt = next(cur);
                    if(nxt == neig[i].end()) nxt = neig[i].begin();
                    adj[cur->second].emplace_back(nxt->second, wT());
                    rev_map[cur->second] = i;
                }
            }
            for(int i = 0; i < n; i++) { // add edges
                for(auto [j, w]: ori_adj[i]) {
                    adj[neig[i][j]].emplace_back(neig[j][i], w);
                }
                if(neig[i].size() == 0) { // for vertices without edges
                    neig[i][n] = cnt - 1;
                }
            }
            k = floor(pow(log2(cnt), 1.0 / 3.0));
            t = floor(pow(log2(cnt), 2.0 / 3.0));
            
            ori_adj.clear();
        }
    }
    
    int toAnyCustomNode(int real_id) {
        return neig[real_id].begin()->second;
    }
    int customToReal(int id) {
        return rev_map[id];
    }
 
    vector<wT> execute(int s) {
        fill(d.begin(), d.end(), oo);
        fill(path_sz.begin(), path_sz.end(), oo);
        fill(last_complete_lvl.begin(), last_complete_lvl.end(), -1);
        for(int i = 0; i < pred.size(); i++) pred[i] = i;
        
        s = toAnyCustomNode(s);
        d[s] = 0;
        path_sz[s] = 0;
        
        const int l = ceil(log2(adj.size()) / t);
        const uniqueDistT inf_dist = make_tuple(oo, 0, 0, 0);
        bmsspRec(l, inf_dist, {s});
 
        vector<wT> res(n);
        for(int i = 0; i < n; i++) res[i] = d[toAnyCustomNode(i)];
        return res;
    }
    
    // ===================================================================
 
    // set stuff
    // template<typename T>
    // void removeDuplicates(vector<T> &v) { // sort is faster
    //     hash_set<T> s(v.begin(), v.end());
    //     v = vector<T>(s.begin(), s.end());
    //     // sort(v.begin(), v.end());
    //     // v.erase(unique(v.begin(), v.end()), v.end());
    // }
    template<typename T>
    bool isUnique(const vector<T> &v) {
        auto v2 = v;
        sort(v2.begin(), v2.end());
        v2.erase(unique(v2.begin(), v2.end()), v2.end());
        return v2.size() == v.size();
    }

    using uniqueDistT = tuple<wT, int, int, int>;
    inline uniqueDistT getDist(int u, int v, int w) {
        return {d[u] + w, path_sz[u] + 1, v, u};
    }
    inline uniqueDistT getDist(int u) {
        return {d[u], path_sz[u], u, pred[u]};
    }
    void updateDist(int u, int v, wT w) {
        pred[v] = u;
        d[v] = d[u] + w;
        path_sz[v] = path_sz[u] + 1;
    }
    // ===================================================================
    vector<int> root;
    vector<short int> treesz;
    pair<vector<int>, hash_set<int>> findPivots(uniqueDistT B, const vector<int> &S) { // Algorithm 1
        hash_set<int> vis;
        vis.reserve(S.size() * k);
        vis.insert(S.begin(), S.end());

        vector<int> active = S;
        for(int x: S) root[x] = x, treesz[x] = 0;
        for(int i = 1; i <= k; i++) {
            vector<int> nw_active;
            nw_active.reserve(active.size() * 4);
            for(int u: active) {
                for(auto [v, w]: adj[u]) {
                    if(getDist(u, v, w) <= getDist(v)) {
                        updateDist(u, v, w);
                        if(getDist(v) < B) {
                            root[v] = root[u];
                            nw_active.push_back(v);
                        }
                    }
                }
            }
            for(const auto &x: nw_active) {
                vis.insert(x);
            }
            if(vis.size() > k * S.size()) {
                return {S, vis};
            }
            active = move(nw_active);
        }

        vector<int> P;
        P.reserve(vis.size() / k);
        for(int u: vis) treesz[root[u]]++;
        for(int u: S) if(treesz[u] >= k) P.push_back(u);
        
        // assert(P.size() <= vis.size() / k);
        return {P, vis};
    }
 
    pair<uniqueDistT, vector<int>> baseCase(uniqueDistT B, int x) { // Algorithm 2
        vector<int> complete;
        complete.reserve(k + 1);
 
        priority_queue<uniqueDistT, vector<uniqueDistT>, greater<uniqueDistT>> heap;
        heap.push(getDist(x));
        while(heap.empty() == false && complete.size() < k + 1) {
            int u = get<2>(heap.top());
            int du = get<0>(heap.top());
            heap.pop();
            if(du > d[u]) continue;

            complete.push_back(u);
            for(auto [v, w]: adj[u]) {
                auto new_dist = getDist(u, v, w);
                auto old_dist = getDist(v);
                if(new_dist <= old_dist && new_dist < B) {
                    updateDist(u, v, w);
                    heap.push(new_dist);
                }
            }
        }
        if(complete.size() <= k) return {B, complete};
 
        uniqueDistT nB = getDist(complete.back());
        // {   // sanity check
        //     int cntbig = 0;
        //     for(int u: complete) if(getDist(u) >= nB) cntbig++;
        //     assert(cntbig == 1);
        //     for(int u: complete) assert(getDist(u) < B);
        //     assert(complete.size() == k + 1);
        // }
        complete.pop_back();
        return {nB, complete};
    }
 
    pair<uniqueDistT, vector<int>> bmsspRec(short int l, uniqueDistT B, const vector<int> &S) { // Algorithm 3
        if(l == 0) return baseCase(B, S[0]);
 
        auto [P, bellman_vis] = findPivots(B, S);
 
        const long long batch_size = (1ll << ((l - 1) * t));
        batchPQ<uniqueDistT> D(batch_size, B);
        for(int p: P) D.insert(getDist(p));
 
        uniqueDistT last_complete_B = B;
        for(int p: P) last_complete_B = min(last_complete_B, getDist(p));
 
        vector<int> complete;
        const long long quota = k * (1ll << (l * t));
        complete.reserve(quota + bellman_vis.size());
        while(complete.size() < quota && D.size()) {
            auto [trying_B, miniS] = D.pull();
            // all with dist < trying_B, can be reached by miniS <= req 2, alg 3
 
            auto [complete_B, nw_complete] = bmsspRec(l - 1, trying_B, miniS);
            
            // all new complete_B are greater than the old ones <= point 6, page 10
            // assert(last_complete_B < complete_B);
 
            complete.insert(complete.end(), nw_complete.begin(), nw_complete.end());
            // point 6, page 10 => complete does not intersect with nw_complete
            // assert(isUnique(complete));
 
            vector<uniqueDistT> can_prepend;
            can_prepend.reserve(nw_complete.size() * 5 + miniS.size());
            for(int u: nw_complete) {
                D.erase(u); // priority queue fix
                last_complete_lvl[u] = l;
                for(auto [v, w]: adj[u]) {
                    auto new_dist = getDist(u, v, w);
                    if(new_dist <= getDist(v)) {
                        updateDist(u, v, w);
                        if(trying_B <= new_dist && new_dist < B) {
                            D.insert(new_dist); // d[v] can be greater equal than min(D), occur 1x per vertex
                        } else if(complete_B <= new_dist && new_dist < trying_B) {
                            can_prepend.emplace_back(new_dist); // d[v] is less than all in D, can occur 1x at each level per vertex
                        }
                    }
                }
            }
            for(int x: miniS) {
                if(complete_B <= getDist(x)) can_prepend.emplace_back(getDist(x));
                // second condition is not necessary
            }
            // can_prepend is not necessarily all unique
            D.batchPrepend(can_prepend);
 
            last_complete_B = complete_B;
        }
        uniqueDistT retB;
        if(D.size() == 0) retB = B;     // successful
        else retB = last_complete_B;    // partial
 
        for(int x: bellman_vis) if(last_complete_lvl[x] != l && getDist(x) < retB) complete.push_back(x); // this get the completed vertices from bellman-ford, it has P in it as well
        // get only the ones not in complete already, for it to become disjoint
 
        return {retB, complete};
    }
};
}