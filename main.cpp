#include <bits/stdc++.h>
using namespace std;
 
#define int long long
#define fastio ios::sync_with_stdio(false); cin.tie(0); cout.tie(0);
 
#ifdef LOCAL
#include "../cp-lib/helpers/debug.cpp"
#else
#define debug(...)
#endif

template<typename wT>
struct newspp {
    int n;
    const int oo = numeric_limits<wT>::max{} / 10;

    vector<vector<pair<int, wT>>> ori_adj;
    vector<vector<pair<int, wT>>> adj;
    vector<wT> d;

    vector<unordered_map<int, int>> neig;
    newspp(int n_): n(n_) {
        ori_adj.assign(n, {});
        neig.assign(n, {});
    }

    int toAnyCustomNode(int real_id) {
        return neig[real_id].begin()->second;
    }
    vector<wT> execute(int s) {
        s = toAnyCustomNode(s);
        fill(d.begin(), d.end(), oo);
        bmssp(l, oo, s);

        vector<wT> res(n);
        for(int i = 0; i < n; i++) res[i] = d[toAnyCustomNode(i)];
        return res;
    }

    void add(int a, int b, wT w) {
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

        for(int i = 0; i < n; i++) { // create 0 weight cycles
            for(auto cur = neig[i].begin(); cur != neig[i].end(); cur++) {
                auto nxt = next(cur);
                if(nxt == neig[i].end()) nxt = neig[i].begin();
                adj[cur->second].emplace_back(nxt->second);
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
    vector<int> dad, root;
    pair<vector<int>, vector<int>> findPivots(wT B, vector<int> S) {
        vector<int> w = S;
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
            append(w, nw_active);
            if(w.size() > k * S.size()) {
                return {S, W};
            }
            swap(active, nw_active);
        }
        vector<int> P;
        for(int x: active) P.push_back(root[x]);
        return {P, w};
    }
    pair<wT, vector<int>> baseCase(wT B, int x) {
        vector<int> found = {x};

        set<wT> h;
        h.insert({d[x], x});
        while(h.size()) {
            auto [u, du] = *h.begin();
            found.insert(u);
            for(auto [v, w]: adj[u]) {
                if(du + w <= d[v] && du + w <= B) {
                    s.erase({d[v], v});
                    d[v] = du + w;
                    s.insert({d[v], v});
                }
            }
        }
        if(found.size() <= k) return {B, found};

        int nB = -1;
        for(int u: found) nB = max(nb, d[u]);
        vector<int> U;
        for(int u: found) if(d[u] != nB) U.insert(u);
        return {B, U};
    }

    struct batchPQ {

    };

    template<typename T>
    void append(vector<T> &a, auto &b) {
        insert(a.end(), b.begin(), b.end());
    }
    template<typename T>
    void removeDuplicates(vector<T> &v) {
        unordered_set<T> s(v.begin(), v.end());
        v.clear();
        append(v, s);
    }
    template<typename T>
    bool isUnique(const vector<T> &vec) {
        unordered_set<T> unique_set(vec.begin(), vec.end());
        return vec.size() == unique_set.size();
    }
    pair<wT, vector<int>> bmssp(int l, wT B, vector<int> S) {
        if(l == 0) return baseCase(B, S);
        auto [P, W] = findPivots(B, S);

        const int M = (2ll << ((long long)(l) - 1) * t);
        batchPQ D(M, B);
        for(int p: P) D.insert({p, d[p]});

        int nB_ = oo;
        for(int p: P) nB_ = min(nb_, d[p]);

        vector<int> found; // found
        while(found.size() < k * (2ll << ((long long)l * t))) {
            auto [nB, S] = D.pull();
            auto ret = bmssp(l - 1, B, S);
            nB_ = ret.first;
            vector<int> cur_found = ret.second;
            append(found, cur_found);
            
            for(int u: cur_found) {
                for(auto [v, w]: adj[u]) {
                    if(d[u] + w <= d[v]) {
                        d[v] = d[u] + w;
                        if(nB <= d[v] && d[v] < B) {
                            D.insert({v, d[v]});
                        }
                    }
                }
            }
            vector<int> promissedButNotFound;
            for(int x: S) {
                if(nB_ <= d[x] && d[x] < nB) promissedButNotFound.push_back({x, d[x]});
            }
            D.batchPrepend(promissedButNotFound);
        }
        wT retB = min(nB_, b);
        
        for(int x: W) if(d[x] < retB) found.push_back(x);
        removeDuplicates(found);
        return {min(nB_, B), found};
    }
}

void solve() {
    int n, m; cin >> n >> m;
    newspp<int> spp(n);
    while(m--) {
        int a, b, w; cin >> a >> b >> w;
        spp.add(a, b, w);
    }
    spp.prepare_graph();
    auto d = spp.execute();
    for(auto x: d) cout << x << " ";
    cout << endl;
}
 
signed main() {
    fastio;
 
    int t = 1;
    // in(t);
    L(i, 0, t - 1) solve();
    
    return 0;
}