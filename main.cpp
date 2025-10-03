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

    vector<unordered_map<int, int>> neig;
    newspp(int n_): n(n_) {
        ori_adj.assign(n, {});
        neig.assign(n, {});
        k = floor(pow(log2(n), 1.0 / 3.0));
        t = floor(pow(log2(n), 2.0 / 3.0));
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
        d.assign(cnt, {});
        dad.assign(cnt, {});
        root.assign(cnt, {});

        for(int i = 0; i < n; i++) { // create 0-weight cycles
            for(auto cur = neig[i].begin(); cur != neig[i].end(); cur++) {
                auto nxt = next(cur);
                if(nxt == neig[i].end()) nxt = neig[i].begin();
                adj[cur->second].emplace_back(nxt->second, wT());
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
    }
    
    int toAnyCustomNode(int real_id) {
        return neig[real_id].begin()->second;
    }

    vector<wT> execute(int s) {
        s = toAnyCustomNode(s);
        fill(d.begin(), d.end(), oo);
        d[s] = 0;
        
        const int l = ceil(log2(n) / t);
        bmssp(l, oo, {s});

        vector<wT> res(n);
        for(int i = 0; i < n; i++) res[i] = d[toAnyCustomNode(i)];
        return res;
    }
    
    // ===================================================================
    
    struct batchPQ {
        int n;
        using elementT = pair<wT, int>;
        set<elementT> s;
        map<int, wT> inv;
        int M, B;

        batchPQ(int M_, int B_): M(M_), B(B_) {}
        unsigned size() {
            return s.size();
        }
        void insert(elementT x) {
            s.insert(x);
            inv[x.second] = x.first;
        }
        void batchPrepend(const vector<elementT> &v) {
            for(auto &x: v) {
                auto it = inv.find(x.second);
                if(it != s.end() && it->second > x.first) {
                    s.erase({it->second, it->first});
                    inv.erase(it);
                }
                s.insert(x);
                inv[x.second] = x.first;
            }
        }
        pair<wT, vector<int>> pull() {
            wT bigKey;
            vector<int> res;
            while(s.size() && res.size() < M) {
                res.push_back(s.begin()->second);
                bigKey = s.begin()->first;
                s.erase(s.begin());
            }
            int x = B;
            if(s.size()) {
                x = s.begin()->first;
                assert(x > bigKey);
            }
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
        unordered_set<T> s(v.begin(), v.end());
        v.clear();
        append(v, s);
    }
    template<typename T>
    bool isUnique(vector<T> v) {
        auto v2 = v;
        sort(v.begin(), v.end());
        v.erase(unique(v.begin(), v.end()), v.end());
        return v2.size() == v.size();
    }

    // ===================================================================

    vector<int> dad, root;
    pair<vector<int>, vector<int>> findPivots(wT B, vector<int> S) {
        unordered_set<int> w(S.begin(), S.end());
        vector<int> active = S;
        for(int x: S) root[x] = x;
        for(int i = 1; i <= k; i++) {
            vector<int> nw_active;
            for(int u: active) {
                for(auto [v, w]: adj[u]) {
                    if(d[u] + w <= d[v]) {
                        d[v] = d[u] + w;
                        if(d[v] < B) {
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
        vector<int> P;
        for(int x: active) P.push_back(root[x]);
        removeDuplicates(P);

        return {P, vector<int>(w.begin(), w.end())};
    }

    pair<wT, vector<int>> baseCase(wT B, int x) {
        vector<int> found = {x};

        set<pair<wT, int>> heap;
        heap.insert({d[x], x});
        while(heap.size() && found.size() < k + 1) {
            auto [du, u] = *heap.begin();
            found.push_back(u);
            for(auto [v, w]: adj[u]) {
                if(du + w <= d[v] && du + w < B) {
                    heap.erase({d[v], v});
                    d[v] = du + w;
                    heap.insert({d[v], v});
                }
            }
        }
        if(found.size() <= k) return {B, found};

        int nB = -1;
        for(int u: found) nB = max(nB, d[u]);
        vector<int> U;
        for(int u: found) if(d[u] < nB) U.push_back(u);
        return {B, U};
    }

    pair<wT, vector<int>> bmssp(int l, wT B, vector<int> S) {
        assert(S.size() <= (2ll << (l * t)));
        debug(l, B, S);
        if(l == 0) return baseCase(B, S[0]);

        auto [P, W] = findPivots(B, S);

        const int M = (2ll << ((l - 1) * t));
        batchPQ D(M, B);
        for(int p: P) D.insert({d[p], p});

        int nB_ = oo;
        for(int p: P) nB_ = min(nB_, d[p]);

        vector<int> found; // found
        while(found.size() < k * (2ll << ((long long)l * t)) && D.size()) {
            auto [nB, S] = D.pull();
            auto ret = bmssp(l - 1, B, S);
            nB_ = ret.first;
            vector<int> cur_found = ret.second;
            append(found, cur_found);
            
            vector<pair<wT, int>> promissedButNotFound;
            for(int u: cur_found) {
                for(auto [v, w]: adj[u]) {
                    if(d[u] + w <= d[v]) {
                        d[v] = d[u] + w;
                        if(nB <= d[v] && d[v] < B) {
                            D.insert({d[v], v});
                        } else if( nB_ <= d[v] && d[v] < nB) {
                            promissedButNotFound.emplace_back(d[v], v);
                        }
                    }
                }
            }
            for(int x: S) {
                if(nB_ <= d[x] && d[x] < nB) promissedButNotFound.emplace_back(d[x], x);
            }
            // removeDuplicates(promissedButNotFound);
            assert(isUnique(promissedButNotFound));
            D.batchPrepend(promissedButNotFound);
        }
        wT retB = min(nB_, B);
        
        for(int x: W) if(d[x] < retB) found.push_back(x);
        removeDuplicates(found);
        return {retB, found};
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
    fastio;
 
    int t = 1;
    // in(t);
    for(int i = 0; i < t; i++) solve();
    
    return 0;
}