template<typename wT>
struct set_pq {
    set<pair<wT, int>> s;
    set_pq(){};
    void clear() {
        s.clear();
    }
    void insert(int u, int du) {
        s.insert({du, u});
    }
    pair<wT, int> extract_min() {
        auto ret = *s.begin();
        s.erase(s.begin());
        return ret; // d, v
    }
    bool empty() {
        return s.size() == 0;
    }
};

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
        set_pq<wT> q;
        d[s] = 0;
        q.insert(s, 0);
        while(!q.empty()) {
            auto [du, u] = q.extract_min();
            for(auto [v, w] : adj[u]) {
                if(d[u] + w < d[v]) {
                    p[v] = u;
                    d[v] = d[u] + w;
                    q.insert(v, d[v]);
                }
            }
        }
        return d;
    };
};