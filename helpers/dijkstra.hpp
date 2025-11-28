#include<vector>
#include<utility>
#include<limits>
#include<set>
#include<queue>

namespace spp {
using namespace std;

template<typename wT>
struct dijkstra {
    int n;
    vector<wT> d, p;
    vector<vector<pair<int, wT>>> adj;
    const wT oo = numeric_limits<wT>::max() / 10;

    dijkstra(const auto &adj): adj(adj), n(adj.size()), p(n, -1), d(n) {}

    vector<wT> execute(int s) {
        fill(d.begin(), d.end(), oo);
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
        return d;
    };
};
}