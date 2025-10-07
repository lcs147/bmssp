#include <bits/stdc++.h>
using namespace std;
 
#define int long long
#define fastio ios::sync_with_stdio(false); cin.tie(0); cout.tie(0);
 
#ifdef LOCAL
#include "cp-lib/helpers/debug.cpp"
#else
#define debug(...)
#endif

template<typename wT>
struct newspp {
    int n, k, t;
    // const wT oo = numeric_limits<wT>::max() / 10;
    const wT oo = 1e9 / 10;

    vector<vector<pair<int, wT>>> ori_adj;
    vector<vector<pair<int, wT>>> adj;
    vector<wT> d;
    vector<int> pred, path_sz;
    vector<int> rev_map;

    vector<unordered_map<int, int>> neig;
    newspp(int n_): n(n_) {
        ori_adj.assign(n, {});
        neig.assign(n, {});
    }
    void addEdge(int a, int b, wT w) {
        ori_adj[a].emplace_back(b, w);
    }

    void prepare_graph() {
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
        debug(n, cnt);
        k = floor(pow(log2(cnt), 1.0 / 3.0));
        t = floor(pow(log2(cnt), 2.0 / 3.0));
        debug(k, t);
        debug("custom nodes");
        for(int i = 0; i < n; i++) {
            for(auto [j, id]: neig[i]) debug(i + 1, id);
        }
        for(int i = 0; i < cnt; i++) {
            debug(customToReal(i) + 1, i, adj[i]);
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
        for(int i = 0; i < pred.size(); i++) pred[i] = i;

        s = toAnyCustomNode(s);
        d[s] = 0;
        path_sz[s] = 0;
        
        const int l = ceil(log2(adj.size()) / t);
        bmssp(l, make_tuple(oo, 0, 0, 0), {s});

        vector<wT> res(n);
        debug(d);
        for(int i = 0; i < n; i++) res[i] = d[toAnyCustomNode(i)];
        return res;
    }
    
    // ===================================================================

    using uniqueDistT = tuple<wT, int, int, int>;
    struct batchPQ {
        int n;
        set<uniqueDistT> s;
        map<int, uniqueDistT> inv;
        int M;
        uniqueDistT B;

        batchPQ(int M_, uniqueDistT B_): M(M_), B(B_) {}
        unsigned size() {
            return s.size();
        }

        void insert(uniqueDistT x) {
            auto it = inv.find(get<2>(x));
            if(it != inv.end()) {
                if(it->second > x) {
                    s.erase(it->second);
                    inv.erase(it);

                    s.insert(x);
                    inv[get<2>(x)] = x;
                }
            } else {
                s.insert(x);
                inv[get<2>(x)] = x;
            }
            assert(*prev(s.end()) < B);
        }
        void batchPrepend(const vector<uniqueDistT> &v) {
            for(auto &x: v) {
                insert(x);
            }
            if(v.size()) assert(*prev(s.end()) < B);
        }
        pair<uniqueDistT, vector<int>> pull() {
            vector<int> res;
            while(s.size() && res.size() < M) {
                res.push_back(get<2>(*s.begin()));
                s.erase(s.begin());
                inv.erase(res.back());
            }
            uniqueDistT x = B;
            if(s.size()) x = *s.begin();

            if(s.size()) assert(*prev(s.end()) < B);

            return {x, res};
            
        }
    };

    // set stuff
    template<typename T>
    void append(vector<T> &a, auto &b) {
        a.insert(a.end(), b.begin(), b.end());
    }
    template<typename T>
    void removeDuplicates(vector<T> &v) {
        // unordered_set<T> s(v.begin(), v.end());
        // v.clear();
        // append(v, s);
        // this is n log n, which is not cool. Change later
        sort(v.begin(), v.end());
        v.erase(unique(v.begin(), v.end()), v.end());
    }
    template<typename T>
    bool isUnique(vector<T> v) {
        auto v2 = v;
        sort(v.begin(), v.end());
        v.erase(unique(v.begin(), v.end()), v.end());
        return v2.size() == v.size();
    }
    uniqueDistT getDist(int u, int v, int w) { // for unique paths assumption
        return {d[u] + w, path_sz[u] + 1, v, u};
    }
    uniqueDistT getDist(int u) {
        return {d[u], path_sz[u], u, pred[u]};
    }
    void updateDist(int u, int v, wT w) {
        pred[v] = u;
        d[v] = d[u] + w;
        path_sz[v] = path_sz[u] + 1;
    }
    // ===================================================================
    vector<int> root;
    pair<vector<int>, vector<int>> findPivots(uniqueDistT B, const vector<int> &S) {
        unordered_set<int> w(S.begin(), S.end());
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
            w.insert(nw_active.begin(), nw_active.end());
            if(w.size() > k * S.size()) {
                return {S, vector<int>(w.begin(), w.end())};
            }
            swap(active, nw_active);
        }
        unordered_map<int, int> sz;
        for(int u: w) sz[root[u]]++;

        vector<int> P;
        for(auto [u, trsize]: sz) if(trsize >= k) P.push_back(u);

        return {P, vector<int>(w.begin(), w.end())};
    }

    pair<uniqueDistT, vector<int>> baseCase(uniqueDistT B, int x) { // find k closest to x | d[x] < B
        vector<int> complete;

        set<uniqueDistT> heap;
        heap.insert(getDist(x));
        while(heap.size() && complete.size() < k + 1) {
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
        debug("baseCase complete", nB, complete);
        return {nB, complete};
    }

    pair<uniqueDistT, vector<int>> bmssp(int l, uniqueDistT B, const vector<int> &S) {
        for(int u: S) assert(getDist(u) < B);
        debug(l, B, S);
        assert(S.size() <= (1 << (l * t)));
        if(l == 0) {
            assert(S.size() == 1);
            return baseCase(B, S[0]);
        }

        auto [P, W] = findPivots(B, S);

        const int M = (1 << ((l - 1) * t));
        batchPQ D(M, B);
        for(int p: P) D.insert(getDist(p));

        uniqueDistT complete_B = B;
        for(int p: P) complete_B = min(complete_B, getDist(p));

        vector<int> complete;
        while(complete.size() < k * (1ll << (l * t)) && D.size()) {
            auto [trying_B, S] = D.pull();

            auto ret = bmssp(l - 1, trying_B, S);
            assert(complete_B <= ret.first);
            complete_B = ret.first;
            vector<int> nw_complete = ret.second;
            append(complete, nw_complete);

            assert(isUnique(complete)); // point 6, page 10
            assert(complete_B <= trying_B);
            assert(trying_B <= B);
            
            debug("ON", l, B, S);
            debug("complete", nw_complete);

            vector<uniqueDistT> new_frontier;
            for(int u: nw_complete) {
                assert(getDist(u) < complete_B);
                for(auto [v, w]: adj[u]) {
                    auto new_dist = getDist(u, v, w);
                    if(new_dist <= getDist(v)) {
                        updateDist(u, v, w);
                        if(trying_B <= new_dist && new_dist < B) {
                            debug("new frontier", v);
                            D.insert(new_dist); // d[v] can be greater equal than min(D)
                        } else if(complete_B <= new_dist && new_dist < trying_B) {
                            debug("new frontier", v);
                            new_frontier.emplace_back(new_dist); // d[v] is less than all in D
                        }
                    }
                }
            }
            for(int x: S) {
                if(complete_B <= getDist(x) && getDist(x) < trying_B) new_frontier.emplace_back(getDist(x)), debug("new frontier", x);
            }
            // new_frontier is not necessarily all unique
            D.batchPrepend(new_frontier);
        }
        uniqueDistT retB;
        if(D.size() == 0) retB = B; // successful
        else retB = complete_B;     // partial
        
        for(int x: W) if(getDist(x) < retB) complete.push_back(x); // this get the completed vertices from belman-ford, it has P in it as well
        removeDuplicates(complete);

        assert(P.size() <= complete.size() / k); // point 4, page 10
        debug("ON", l, B, S);
        debug(P, B, retB, complete);
        return {retB, complete};
    }
};

void solve() {
    int n, m; cin >> n >> m;
    newspp<int> spp(n);
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
    // fastio;
 
    int t = 1;
    // in(t);
    for(int i = 0; i < t; i++) solve();
    
    return 0;
}