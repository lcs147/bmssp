#include<bits/stdc++.h>
template<typename wT>
struct dijkstra {
    int n;
    vector<wT> d, p;
    vector<vector<pair<int, wT>>> adj;
    const wT oo = numeric_limits<wT>::max() / 10;

    dijkstra(const auto &adj): adj(adj), n(adj.size()), p(n, -1) {
        d = vector<wT>(n, oo);
    }

    vector<wT> execute(int s) {
        set<pair<wT, int>> q;
        d[s] = 0;
        q.insert({0, s});
        while(!q.empty()) {
            auto [du, u] = *q.begin();
            q.erase(q.begin());
            for(auto [v, w] : adj[u]) {
                if(d[u] + w < d[v]) {
                    q.s.erase({d[v], v});
                    p[v] = u;
                    d[v] = d[u] + w;
                    q.insert(v, d[v]);
                }
            }
        }
        return d;
    };
};