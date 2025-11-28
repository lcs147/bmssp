#include<vector>
#include<utility>
#include<limits>
#include<set>
#include<queue>
#include<algorithm>

namespace spp {
using namespace std;

template<typename wT>
struct dijkstra {
    int n;
    vector<wT> d;
    vector<int> p;
    vector<vector<pair<int, wT>>> adj;
    const wT oo = numeric_limits<wT>::max() / 10;

    dijkstra(const auto &adj): adj(adj), n(adj.size()), p(n, -1), d(n) {}

    std::pair<std::vector<wT>, std::vector<int>> execute(int s) {
        fill(d.begin(), d.end(), oo);
        fill(p.begin(), p.end(), -1);
        priority_queue<pair<wT, int>, vector<pair<wT, int>>, greater<pair<wT, int>>> heap;

        d[s] = 0;
        heap.push({0, s});
        while(!heap.empty()) {
            auto [du, u] = heap.top();
            heap.pop();
            if(du > d[u]) continue;
            
            for(auto [v, w] : adj[u]) {
                if(d[u] + w < d[v]) {
                    p[v] = u;
                    d[v] = d[u] + w;
                    heap.push({d[v], v});
                }
            }
        }
        
        return {d, p};
    };
    
    vector<int> get_shortest_path(int u) {
        if(d[u] == oo) return {};

        vector<int> path;
        path.reserve(n);
        while(u != -1) {
            path.push_back(u);
            u = p[u];
        }
        reverse(path.begin(), path.end());

        return path; // {source, ..., real_u}
    }
};
}