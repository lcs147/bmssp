// Copyright (c) 2025 Lucas Castro and Thailsson Clementino
// Licensed under the MIT License.

#ifndef CASTRO_THAILSSON_BMSSP_WC_DUAN25_H
#define CASTRO_THAILSSON_BMSSP_WC_DUAN25_H

#include<set>
#include<map>
#include<list>
#include<cmath>
#include<vector>
#include<utility>
#include<limits>
#include<queue>
#include<algorithm>

namespace spp {

template<typename dataT>
struct worst_case_hash_map {
    short int counter = 1;
    std::vector<short int> flag;
    std::vector<std::pair<int, dataT>> data;
    worst_case_hash_map(int n): data(n), flag(n) {}; // O(n)

    void erase(int id) { // O(1)
        flag[id] = counter - 1;
    }
    dataT& operator[](int id) { // O(1)
        flag[id] = counter;
        data[id].first = id;
        return data[id].second;
    }
    std::vector<std::pair<int, dataT>>::iterator find(int id) { // O(1)
        if(flag[id] != counter) return data.end();
        return data.begin() + id;
    }
    std::vector<std::pair<int, dataT>>::iterator end() { return data.end(); } // O(1)

    void clear() { counter++; } // O(1)
};

template<typename uniqueDistT>
class batchPQ { // batch priority queue, implemented as in Lemma 3.3

    template<typename V>
    using hash_map = worst_case_hash_map<V>;

    using elementT = std::pair<int,uniqueDistT>;
    
    struct CompareUB {
        template <typename It>
        bool operator()(const std::pair<uniqueDistT, It>& a, const std::pair<uniqueDistT, It>& b) const {
            if (a.first != b.first) return a.first < b.first;
            return  addressof(*a.second) < addressof(*b.second);
        }
    };
    
    typename std::list<std::list<elementT>>::iterator it_min;
    
    std::list<std::list<elementT>> D0,D1;
    std::set<std::pair<uniqueDistT,typename std::list<std::list<elementT>>::iterator>,CompareUB> UBs;
    
    int M,size_;
    uniqueDistT B;
    
    hash_map<uniqueDistT> actual_value;
    hash_map<std::pair<typename std::list<std::list<elementT>>::iterator, typename std::list<elementT>::iterator>> where_is0, where_is1;
    
public:

    batchPQ(int n): actual_value(n), where_is0(n), where_is1(n){} // O(n)

    void initialize(int M_, uniqueDistT B_) { // O(1)
        M = M_; B = B_;
        D0 = {};
        D1 = {std::list<elementT>()};
        UBs = {make_pair(B_,D1.begin())};
        size_ = 0;

        actual_value.clear();
        where_is0.clear(); where_is1.clear();
    }

    int size(){
        return size_;
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
        where_is1[a] = {it_block, it};
        actual_value[a] = b;
    
        size_++;
    
        // Checking if exceeds the sixe limit M
        if((*it_block).size() > M){
            split(it_block);
        }
    }
    
    void batchPrepend(const std::vector<uniqueDistT> &v){
        std::list<elementT> l;
        for(auto x: v){
            l.push_back({get<2>(x),x});
        }
        batchPrepend(l);
    }

    std::pair<uniqueDistT, std::vector<int>> pull(){ // O(M)
        std::vector<elementT> s0,s1;
        s0.reserve(2 * M); s1.reserve(M);
    
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
            std::vector<int> ret;
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
            std::vector<elementT> &l = s0;
            l.insert(l.end(), s1.begin(), s1.end());

            uniqueDistT med = selectKth(l, M);
            std::vector<int> ret;
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
    inline void erase(int key) {
        if(actual_value.find(key) != actual_value.end())
            delete_({-1, -1, key, -1});
    }
    
private:
    uniqueDistT medianOfMedians(std::vector<elementT>::iterator bg, std::vector<elementT>::iterator en) {
        std::vector<elementT> l(bg, en);
        while (true) {
            int n = l.size();

            if (n <= 5) {
                sort(l.begin(), l.end(), [](const auto& a, const auto& b) {
                    return a.second < b.second;
                });
                return l[l.size() / 2].second;
            }

            std::vector<elementT> medians;
            medians.reserve((n + 4) / 5);

            auto it = l.begin();
            while (it != l.end()) {
                std::vector<elementT> group;
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

    void delete_(uniqueDistT x){    
        int a = get<2>(x);
        uniqueDistT b = actual_value[a];
        
        auto it_w = where_is1.find(a);
        if((it_w != where_is1.end())){
            auto [it_block,it] = it_w->second;
            
            (*it_block).erase(it);
            where_is1.erase(a);
    
            if((*it_block).size() == 0){
                auto it_UB_block = UBs.lower_bound({b,it_block});  
                
                if((*it_UB_block).first != B){
                    UBs.erase(it_UB_block);
                    D1.erase(it_block);
                }
            }
        }else{
            auto [it_block,it] = where_is0[a];
            (*it_block).erase(it);
            where_is0.erase(a);
            if((*it_block).size() == 0) D0.erase(it_block); 
        }
    
        actual_value.erase(a);
        size_--;
    }
    
    uniqueDistT selectKth(std::vector<elementT> &v, int k) { 
        int l=0, r=v.size()-1;
        
        for(;l<=r;){
            uniqueDistT p = medianOfMedians(v.begin() + l, v.begin() + r + 1);
            
            int i = l, j = r, m = l;
            
            while(m <= j){
                if(v[m].second < p) {
                    std::swap(v[m++],v[i++]);
                }else if(v[m].second > p){
                    std::swap(v[m],v[j--]);
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

        
    void split(std::list<std::list<elementT>>::iterator it_block){ // O(M) + O(lg(Block Numbers))
        int sz = (*it_block).size();
        
        std::vector<elementT> v((*it_block).begin() , (*it_block).end());
        uniqueDistT med = selectKth(v,(sz/2)); // O(M)
        
        auto pos = it_block;
        pos++;

        auto new_block = D1.insert(pos,std::list<elementT>());
        auto it = (*it_block).begin();
    
        while(it != (*it_block).end()){ // O(M)
            if((*it).second >= med){
                (*new_block).push_back(move(*it));
                auto it_new = (*new_block).end(); it_new--;
                where_is1[(*it).first] = {new_block, it_new};
    
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
    
    void batchPrepend(const std::list<elementT> &l) { // O(|l| log(|l|/M) ) 
        int sz = l.size();
        
        if(sz == 0) return;
        if(sz <= M){
    
            D0.push_front(std::list<elementT>());
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
                where_is0[x.first] = {new_block, it_new};
                actual_value[x.first] = x.second;
                size_++;
            }
            if(new_block->size() == 0) D0.erase(new_block);
            return;
        }

        std::vector<elementT> v(l.begin(), l.end());
        uniqueDistT med = selectKth(v, sz/2);
    
        std::list<elementT> less,great;
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
};

template<typename wT>
class bmssp { // bmssp class
    int n, k, t, l;

    std::vector<std::vector<std::pair<int, wT>>> ori_adj;
    std::vector<std::vector<std::pair<int, wT>>> adj;
    std::vector<wT> d;
    std::vector<int> pred, path_sz;

    std::vector<int> node_map, node_rev_map;
    
    bool cd_transfomed;

public:
    const wT oo = std::numeric_limits<wT>::max() / 10;
    bmssp(int n_): n(n_) {
        ori_adj.assign(n, {});
    }
    bmssp(const auto &adj) {
        n = adj.size();
        ori_adj = adj;
    }
    
    void addEdge(int a, int b, wT w) {
        ori_adj[a].emplace_back(b, w);
    }

    // if the graph already has constant degree, prepage_graph(false)
    // else, prepage_graph(true)
    void prepare_graph(bool exec_constant_degree_trasnformation = false) {
        cd_transfomed = exec_constant_degree_trasnformation;
        // erase duplicated edges
        std::vector<std::pair<int, int>> tmp_edges(n, {-1, -1});
        for(int i = 0; i < n; i++) {
            std::vector<std::pair<int, wT>> nw_adj;
            nw_adj.reserve(ori_adj[i].size());
            for(auto [j, w]: ori_adj[i]) {
                if(tmp_edges[j].first != i) {
                    nw_adj.emplace_back(j, w);
                    tmp_edges[j] = {i, nw_adj.size() - 1};
                } else {
                    int id = tmp_edges[j].second;
                    nw_adj[id].second = std::min(nw_adj[id].second, w);
                }
            }
            ori_adj[i] = move(nw_adj);
        }
        tmp_edges.clear();

        if(exec_constant_degree_trasnformation == false) {
            adj = move(ori_adj);
            node_map.resize(n);
            node_rev_map.resize(n);
            
            for(int i = 0; i < n; i++) {
                node_map[i] = i;
                node_rev_map[i] = i;
            }
    
            // k = floor(pow(log2(n), 1.0 / 3.0));
            // t = floor(pow(log2(n), 2.0 / 3.0));
        } else { // Make the graph become constant degree
            int cnt = 0;
            std::vector<std::map<int, int>> edge_id(n);
            for(int i = 0; i < n; i++) {
                for(auto [j, w]: ori_adj[i]) {
                    if(edge_id[i].find(j) == edge_id[i].end()) {
                        edge_id[i][j] = cnt++;
                        edge_id[j][i] = cnt++;
                    }
                }
            }

            cnt++;
            adj.assign(cnt, {});
            node_map.resize(cnt);
            node_rev_map.resize(cnt);
    
            for(int i = 0; i < n; i++) { // create 0-weight cycles
                for(auto cur = edge_id[i].begin(); cur != edge_id[i].end(); cur++) {
                    auto nxt = next(cur);
                    if(nxt == edge_id[i].end()) nxt = edge_id[i].begin();
                    adj[cur->second].emplace_back(nxt->second, wT());
                    node_rev_map[cur->second] = i;
                }
            }
            for(int i = 0; i < n; i++) { // add edges
                for(auto [j, w]: ori_adj[i]) {
                    adj[edge_id[i][j]].emplace_back(edge_id[j][i], w);
                }
                if(edge_id[i].size()) {
                    node_map[i] = edge_id[i].begin()->second;
                } else {
                    node_map[i] = cnt - 1;
                }
            }
            
        }
        
        // ori_adj.clear();
        d.resize(adj.size());
        root.resize(adj.size());
        pred.resize(adj.size());
        treesz.resize(adj.size());
        path_sz.resize(adj.size(), 0);
        last_complete_lvl.resize(adj.size());
        pivot_vis.resize(adj.size(),0);
        k = floor(pow(log2(adj.size()), 1.0 / 3.0));
        t = floor(pow(log2(adj.size()), 2.0 / 3.0));
        l = ceil(log2(adj.size()) / t);
        Ds.assign(l, adj.size());
    }

    std::pair<std::vector<wT>, std::vector<int>> execute(int s) {
        
        fill(d.begin(), d.end(), oo);
        fill(path_sz.begin(), path_sz.end(), oo);
        fill(last_complete_lvl.begin(), last_complete_lvl.end(), -1);
        for(int i = 0; i < pred.size(); i++) pred[i] = i;
        
        s = toAnyCustomNode(s);
        //cout << d.size() <<  " " << toAnyCustomNode(0) << " " << toAnyCustomNode(1) << "\n";
        //cout << s << " " << toAnyCustomNode(2790) << "\n";
        d[s] = 0;
        path_sz[s] = 0;
        
        const int l = ceil(log2(adj.size()) / t);
        const uniqueDistT inf_dist = {oo, 0, 0, 0};
        bmsspRec(l, inf_dist, {s});
        
        if(!cd_transfomed) {
            //cout << d[0] << " " << d[2790] << " " << d.size() << "\n";
            return {d, pred};
        } else {
            std::vector<wT> ret_distance(n);
            std::vector<int> ret_pred(n);
            for(int i = 0; i < n; i++) {
                ret_distance[i] = d[toAnyCustomNode(i)];
                ret_pred[i] = customToReal(getPred(toAnyCustomNode(i)));
            }
           //cout << ret_distance[0] << " " << ret_distance[2790] << " " << ret_distance.size() << "\n";
            return {ret_distance, ret_pred};
        }
    }

    std::vector<int> get_shortest_path(int real_u) {
        if(!cd_transfomed) {
            int u = real_u;
            if(d[u] == oo) return {};

            int path_sz = get<1>(getDist(u)) + 1;
            std::vector<int> path(path_sz);
            for(int i = path_sz - 1; i >= 0; i--) {
                path[i] = u;
                u = pred[u];
            }
            return path; // {source, ..., real_u}
        } else {
            int u = toAnyCustomNode(real_u);
            if(d[u] == oo) return {};

            int max_path_sz = get<1>(getDist(u)) + 1;
            std::vector<int> path;
            path.reserve(max_path_sz);

            int oldu;
            do {
                path.push_back(customToReal(u));
                oldu = u;
                u = getPred(u);
            } while(customToReal(u) != customToReal(oldu));

            reverse(path.begin(), path.end());
            return path; // {source, ..., real_u}
        }
    }

private:
    int toAnyCustomNode(int real_id) {
        return node_map[real_id];
    }
    int customToReal(int id) {
        return node_rev_map[id];
    }
    int getPred(int u) {
        int real_u = customToReal(u);

        int dad = u;
        do dad = pred[dad];
        while(customToReal(dad) == real_u && pred[dad] != dad);

        return dad;
    }

    template<typename T>
    bool isUnique(const std::vector<T> &v) {
        auto v2 = v;
        sort(v2.begin(), v2.end());
        v2.erase(unique(v2.begin(), v2.end()), v2.end());
        return v2.size() == v.size();
    }

    // Unique distances helpers: Assumption 2.1
    struct uniqueDistT : std::tuple<wT, int, int, int> {
        static constexpr wT SCALE = 1e10;
        static constexpr wT SCALE_INV = ((wT) 1.0) / SCALE; 

        uniqueDistT() = default;
        static inline wT sanitize(wT w) {
            if constexpr (std::is_floating_point_v<wT>) {
                return std::round(w * SCALE) * SCALE_INV;
            }
            return w;
        }
        uniqueDistT(wT w, int i1, int i2, int i3) 
            : std::tuple<wT, int, int, int>(sanitize(w), i1, i2, i3) {}
    };
    inline uniqueDistT getDist(int u, int v, wT w) {
        return {d[u] + w, path_sz[u] + 1, v, u};
    }
    inline uniqueDistT getDist(int u) {
        return {d[u], path_sz[u], u, pred[u]};
    }
    void updateDist(int u, int v, wT w) {
        pred[v] = u;
        d[v] = d[u] + w;
        path_sz[v] = path_sz[u] + 1;

        // if(v == 12799 || v == 2790 || customToReal(v) == 1913 ){

        //    cout << "update " << customToReal(u) << " -> " << customToReal(v) << " "  << d[v] << "\n";
        // }
    }

    // ===================================================================
    std::vector<int> root;
    std::vector<short int> treesz;

    int counter_pivot = 0;
    std::vector<int> pivot_vis;
    std::pair<std::vector<int>, std::vector<int>> findPivots(uniqueDistT B, const std::vector<int> &S) { // Algorithm 1
        counter_pivot++;

        std::vector<int> vis;
        vis.reserve(2 * k * S.size());

        for(int x: S) {
            vis.push_back(x);
            pivot_vis[x] = counter_pivot;
        }

        std::vector<int> active = S;
        for(int x: S) root[x] = x, treesz[x] = 0;
        for(int i = 1; i <= k; i++) {
            std::vector<int> nw_active;
            nw_active.reserve(active.size() * 4);
            for(int u: active) {
                for(auto [v, w]: adj[u]) {
                    if(getDist(u, v, w) <= getDist(v)) {
                         updateDist(u, v, w);
                     //   if(v == 12799 || v == 2790 || customToReal(v) == 1913 ) cout << "FinfPivots\n";
                        if(getDist(v) < B){
                            root[v] = root[u];
                            if(pivot_vis[v] != counter_pivot) {
                                nw_active.push_back(v);
                                pivot_vis[v] = counter_pivot;
                            }
                        }
                    }
                }
            }
            for(const auto &x: nw_active) {
                vis.push_back(x);
            }
            if(vis.size() > k * S.size()) {
                return {S, vis};
            }
            active = move(nw_active);
        }

        std::vector<int> P;
        P.reserve(vis.size() / k);
        for(int u: vis) treesz[root[u]]++;
        for(int u: S) if(treesz[u] >= k) P.push_back(u);
        
        // assert(P.size() <= vis.size() / k);
        return {P, vis};
    }
 
    std::pair<uniqueDistT, std::vector<int>> baseCase(uniqueDistT B, int x) { // Algorithm 2
        std::vector<int> complete;
        complete.reserve(k + 1);
 
        std::priority_queue<uniqueDistT, std::vector<uniqueDistT>, std::greater<uniqueDistT>> heap;
        heap.push(getDist(x));
        while(heap.empty() == false && complete.size() < k + 1) {
            auto du = heap.top();
            int u = get<2>(du);
            heap.pop();

            if(du > getDist(u)) continue;

            complete.push_back(u);
            for(auto [v, w]: adj[u]) {
                auto new_dist = getDist(u, v, w);
                auto old_dist = getDist(v);
                if(new_dist <= old_dist && new_dist < B) {
                    updateDist(u, v, w);
                   // if(v == 12799 || v == 2790 || customToReal(v) == 1913 ) cout << "BaseCase\n";
                    heap.push(new_dist);
                }
            }
        }
        if(complete.size() <= k) return {B, complete};
 
        uniqueDistT nB = getDist(complete.back());
        complete.pop_back();

        return {nB, complete};
    }
 
    std::vector<batchPQ<uniqueDistT>> Ds;
    std::vector<short int> last_complete_lvl;
    std::pair<uniqueDistT, std::vector<int>> bmsspRec(short int l, uniqueDistT B, const std::vector<int> &S) { // Algorithm 3
        if(l == 0) return baseCase(B, S[0]);
        
        auto [P, bellman_vis] = findPivots(B, S);
 
        const long long batch_size = (1ll << ((l - 1) * t));
        auto &D = Ds[l - 1];
        D.initialize(batch_size, B);
        for(int p: P) D.insert(getDist(p));
 
        uniqueDistT last_complete_B = B;
        for(int p: P) last_complete_B = std::min(last_complete_B, getDist(p));
 
        std::vector<int> complete;
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
 
            std::vector<uniqueDistT> can_prepend;
            can_prepend.reserve(nw_complete.size() * 5 + miniS.size());
            for(int u: nw_complete) {
                D.erase(u); // priority queue fix
                last_complete_lvl[u] = l;
                for(auto [v, w]: adj[u]) {
                    auto new_dist = getDist(u, v, w);
                    if(new_dist <= getDist(v)) {
                        updateDist(u, v, w);
                        if(trying_B <= new_dist && new_dist < B) {
                            //if(v == 12799 || v == 2790 || customToReal(v) == 1913 ) cout << "A\n";
                            D.insert(new_dist); // d[v] can be greater equal than std::min(D), occur 1x per vertex
                        } else if(complete_B <= new_dist && new_dist < trying_B) {
                          //  if(v == 12799 || v == 2790 || customToReal(v) == 1913 ) cout << "B\n";
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

#endif