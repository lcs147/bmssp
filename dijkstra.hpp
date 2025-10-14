#include<vector>
#include<utility>
#include<limits>
#include<set>

namespace spp {
template<typename wT>
struct dijkstra {
    int n;
    std::vector<wT> d, p;
    std::vector<std::vector<pair<int, wT>>> adj;
    const wT oo = std::numeric_limits<wT>::max() / 10;

    dijkstra(const auto &adj): adj(adj), n(adj.size()), p(n, -1) {
        d = std::vector<wT>(n, oo);
    }

    std::vector<wT> execute(int s) {
        std::set<pair<wT, int>> q;
        d[s] = 0;
        q.insert({0, s});
        while(!q.empty()) {
            auto [du, u] = *q.begin();
            q.erase(q.begin());
            for(auto [v, w] : adj[u]) {
                if(d[u] + w < d[v]) {
                    q.erase({d[v], v});
                    p[v] = u;
                    d[v] = d[u] + w;
                    q.insert({d[v], v});
                }
            }
        }
        return d;
    };
};
}