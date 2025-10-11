#include <bits/stdc++.h>
using namespace std;
 
#define fastio ios::sync_with_stdio(false); cin.tie(0); cout.tie(0);
 
#ifdef LOCAL
#include "cp-lib/helpers/debug.cpp"
#else
#define debug(...)
#endif
 
// #include <boost/unordered_map.hpp>
// #include <boost/unordered_set.hpp>
// template<typename K, typename V>
// using hash_map = boost::unordered_map<K, V>;
// template<typename K>
// using hash_set = boost::unordered_set<K>;
 
#include <ext/pb_ds/assoc_container.hpp>
using namespace __gnu_pbds;
 
template<typename K, typename V>
using hash_map = gp_hash_table<K, V>;
template<typename K>
using hash_set = hash_map<K, null_type>;
 
// template<typename K, typename V>
// using hash_map = map<K, V>;
// template<typename K>
// using hash_set = set<K>;
 
template<typename wT>
struct newspp {
    int n, k, t;
    const wT oo = numeric_limits<wT>::max() / 1000;
    // const wT oo = 1e9 / 10;
 
    vector<vector<pair<int, wT>>> ori_adj;
    vector<vector<pair<int, wT>>> adj;
    vector<wT> d;
    vector<int> pred, path_sz;
    vector<int> rev_map;
 
    vector<hash_map<int, int>> neig;
    newspp(int n_): n(n_) {
        ori_adj.assign(n, {});
        neig.assign(n, {});
    }
    void addEdge(int a, int b, wT w) { // deduplicating can be worst-case
        ori_adj[a].emplace_back(b, w);
    }
 
    void prepare_graph() {
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
            ori_adj[i].shrink_to_fit();
        }
        tmp_edges.clear();
        tmp_edges.shrink_to_fit();
        // Make graph become constant degree
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
        root.resize(cnt);
        
        rev_map.resize(cnt);
        d.resize(cnt);
        path_sz.resize(cnt, 0);
        pred.resize(cnt);
 
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
                neig[i][n] = cnt;
            }
        }
        k = floor(pow(log2(cnt), 1.0 / 3.0));
        t = floor(pow(log2(cnt), 2.0 / 3.0));
        debug(n, cnt);
        debug(k, t);
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
        for(int i = 0; i < (int) pred.size(); i++) pred[i] = i;
 
        s = toAnyCustomNode(s);
        d[s] = 0;
        path_sz[s] = 0;
        
        const int l = ceil(log2(adj.size()) / t);
        debug(l);
        bmssp(l, make_tuple(oo, 0, 0, 0), {s});
 
        vector<wT> res(n);
        for(int i = 0; i < n; i++) res[i] = d[toAnyCustomNode(i)];
        return res;
    }
    
    // ===================================================================
 
    using uniqueDistT = tuple<wT, int, int, int>;
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

        uniqueDistT medianOfMedians(vector<elementT> l){
            int n = l.size();
            
            if (n <= 5) {
                // Convert small list to vector and sort directly
                vector<elementT> v(l.begin(), l.end());
                sort(v.begin(), v.end(), [](const auto& a, const auto& b) {
                    return a.second < b.second;
                });
                return v[v.size() / 2].second;
            }
            
            vector<elementT> group;
            group.reserve(5);
            vector<elementT> medians;

            auto it = l.begin();
            while (it != l.end()) {
                group.clear();
                for (int j = 0; j < 5 && it != l.end(); ++j, ++it)
                    group.push_back(*it);

                sort(group.begin(), group.end(), [](const auto& a, const auto& b) {
                    return a.second < b.second;
                });

                medians.push_back(group[group.size() / 2]);
            }

            return medianOfMedians(medians);
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

            auto mid = std::partition(l.begin(), l.end(), [&](auto& x) {
                return x.second < med;
            });
            
            list<elementT> less, great;
            less.splice(less.end(), l, l.begin(), mid);
            great.splice(great.end(), l, mid, l.end());
     
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
                    ret.push_back(get<2>(b));
                    delete_({b});
                }
                for(auto [a,b] : s1){
                    ret.push_back(get<2>(b));
                    delete_({b});
                } 
     
                return {B, ret};
            }else{  
                vector<elementT> l;
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
                
                return {med,std::move(ret)};
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
 
    // set stuff
    template<typename T>
    void append(vector<T> &a, auto &b) {
        a.insert(a.end(), b.begin(), b.end());
    }
 
    template<typename T>
    void removeDuplicates(vector<T> &v) { // sort is faster
        // hash_set<T> s(v.begin(), v.end());
        // v.clear();
        // append(v, s);
        sort(v.begin(), v.end());
        v.erase(unique(v.begin(), v.end()), v.end());
    }
    // template<typename T>
    // bool isUnique(const vector<T> &v) {
    //     auto v2 = v;
    //     sort(v.begin(), v.end());
    //     v.erase(unique(v.begin(), v.end()), v.end());
    //     return v2.size() == v.size();
    // }
    inline uniqueDistT getDist(int u, int v, int w) { // for unique paths assumption
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
    pair<vector<int>, set<int>> findPivots(uniqueDistT B, const vector<int> &S) {
        set<int> w(S.begin(), S.end());
        vector<int> active = S;
        for(int x: S) root[x] = x;
        for(int i = 1; i <= k; i++) {
            vector<int> nw_active;
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
            for(const auto &x: nw_active) w.insert(x);
            if(w.size() > k * S.size()) {
                return {S, w};
            }
            active = move(nw_active);
        }
        hash_map<int, int> sz;
        for(int u: w) sz[root[u]]++;
 
        vector<int> P;
        for(auto [u, trsize]: sz) if(trsize >= k) P.push_back(u);
 
        return {P, w};
    }
 
    pair<uniqueDistT, vector<int>> baseCase(uniqueDistT B, int x) { // find k closest to x | d[x] < B
        vector<int> complete;
 
        set<uniqueDistT> heap;
        heap.insert(getDist(x));
        while(heap.size() && (int) complete.size() < k + 1) {
            int u = get<2>(*heap.begin());
            heap.erase(heap.begin());
            complete.push_back(u);
            for(auto [v, w]: adj[u]) {
                auto new_dist = getDist(u, v, w);
                auto old_dist = getDist(v);
                if(new_dist <= old_dist && new_dist < B) {
                    heap.erase(old_dist);
                    updateDist(u, v, w);
                    heap.insert(new_dist);
                }
            }
        }
        if(complete.size() <= k) return {B, complete};
 
        uniqueDistT nB = getDist(complete.back());
        complete.pop_back();
        return {nB, complete};
    }
 
    pair<uniqueDistT, vector<int>> bmssp(int l, uniqueDistT B, const vector<int> &S) {
        // debug(l, B);
        // for(int u: S) assert(getDist(u) < B);
        // assert(S.size() <= (1 << (l * t)));
        if(l == 0) {
            // assert(S.size() == 1);
            return baseCase(B, S[0]);
        }
 
        auto [P, W] = findPivots(B, S);
 
        const int M = (1 << ((l - 1) * t));
        batchPQ D(M, B);
        for(int p: P) D.insert(getDist(p));
 
        uniqueDistT complete_B = B;
        for(int p: P) complete_B = min(complete_B, getDist(p));
 
        // int its = 0;
        vector<int> complete;
        const int cota = k * (1ll << (l * t));
        while((int) complete.size() < cota && (int) D.size()) {
            // its++;
            auto [trying_B, S] = D.pull();
 
            auto ret = bmssp(l - 1, trying_B, S);
            
            // debug("ON", l, B);
            // if(complete_B > ret.first) {
            //     debug("wtf?");
            //     debug(complete_B, ret.first, l, B);
            //     debug(trying_B, S.size());
            // }
            // assert(complete_B <= ret.first);
            complete_B = ret.first;
            auto &nw_complete = ret.second;
            // int old_sz = complete.size();
            // for(int x: nw_complete) complete.push_back(x);
            append(complete, nw_complete);
 
            // if(!isUnique(complete)) {
            //     debug(l, B);
            //     debug(trying_B, S.size());
            //     debug(complete_B, nw_complete.size(), complete.size());
            //     removeDuplicates(complete);
            //     debug(old_sz, complete.size());
            //     debug(its);
            // }
            // assert(isUnique(complete)); // point 6, page 10
            // assert(complete_B <= trying_B);
            // assert(trying_B <= B);
 
            vector<uniqueDistT> new_frontier;
            for(int u: nw_complete) {
                // assert(getDist(u) < complete_B);
                for(auto [v, w]: adj[u]) {
                    auto new_dist = getDist(u, v, w);
                    if(new_dist <= getDist(v)) {
                        updateDist(u, v, w);
                        if(trying_B <= new_dist && new_dist < B) {
                            D.insert(new_dist); // d[v] can be greater equal than min(D)
                        } else if(complete_B <= new_dist && new_dist < trying_B) {
                            new_frontier.emplace_back(new_dist); // d[v] is less than all in D
                        }
                    }
                }
            }
            for(int x: S) {
                if(complete_B <= getDist(x) && getDist(x) < trying_B) new_frontier.emplace_back(getDist(x));
            }
            // new_frontier is not necessarily all unique
            D.batchPrepend(new_frontier);
        }   
        uniqueDistT retB;
        if(D.size() == 0) retB = B; // successful
        else retB = complete_B;     // partial
 
        for(int x: W) if(getDist(x) < retB) complete.push_back(x); // this get the completed vertices from 
        // for(int x: W) if(getDist(x) < retB) complete.push_back(x); // this get the completed vertices from belman-ford, it has P in it as well
        removeDuplicates(complete);
 
        // assert(P.size() <= complete.size() / k); // point 4, page 10
        return {retB, complete};
    }
};
 
void solve() {
    int n, m; cin >> n >> m;
    newspp<long long> spp(n);
    while(m--) {
        int a, b, w; cin >> a >> b >> w;
        a--; b--;
        spp.addEdge(a, b, w);
    }
    spp.prepare_graph();
    auto d = spp.execute(0);
    for(auto x: d) cout << x << " ";
    cout << endl;
}
 
signed main() {
    fastio;
 
    int t = 1;
    // in(t);
    for(int i = 0; i < t; i++) solve();
    
    return 0;
}