// Generate a random sparse graph in DIMACS format

#include<bits/stdc++.h>
#include <random>
#define int long long
using namespace std;

signed main(signed argc, char **argv) {
    if(argc < 3) {
        cout << "must have 4 arguments: number_of_vertices average_outdegree max_weight seed" << endl;
        return 1;
    }

    int n = atoi(argv[1]);
    int average_outdegree = atoi(argv[2]);
    int max_weight = atoi(argv[3]);
    int seed = atoi(argv[4]);
    
    mt19937_64 gen(seed);
    auto random_integer = [&](int l, int r) {
        uniform_int_distribution<uint64_t> dis(l, r);
        return dis(gen);
    };

    int m = n * average_outdegree;
    cout << "p sp " << n << " " << m << endl;

    int s = 1;
    vector<vector<int>> adj(n + 1);
    auto canAddEdge = [&](int i, int j) -> bool {
        if(adj[i].size() >= average_outdegree + 1 
        || find(adj[i].begin(), adj[i].end(), j) != adj[i].end() 
        || i == j) return false; 
        return true;
    };

    {   // prufer generator
        
        auto adj2 = adj;
        for(int i = 2; i <= n; i++) {
            int j;
            do {
                j = random_integer(1, i - 1);
            } while(canAddEdge(i, j) == false || canAddEdge(j, i) == false);
            adj[i].push_back(j);
            adj[j].push_back(i);
        }

        auto dfs = [&](auto &self, int u, int dad) -> int {
            int cnt = 1;
            for(int v: adj[u]) {
                if(v == dad) continue;
                adj2[u].push_back(v);
                cnt += self(self, v, u);
            }
            return cnt;
        };
        
        int olds = random_integer(1, n);
        int found = dfs(dfs, olds, -1);
        assert(found == n);

        adj = move(adj2);
        for(int u = 1; u <= n; u++) {
            for(int &v: adj[u]) {
                if(v == olds) v = s;
                else if(v == s) v = olds;
            }
        }
        swap(adj[s], adj[olds]);

        adj2.assign(n + 1, {});
        assert(dfs(dfs, 1, -1) == n);
    } // adj is a tree, and 1 reaches all vertices

    for(int i = 1; i <= n; i++) {
        for(int j: adj[i]) {
            cout << "a "<< i << " " << j << " " << random_integer(0, max_weight) << endl;
        }
    }

    auto add = [&](int i, int j, int w) {
        if(canAddEdge(i, j) == false) return false;

        adj[i].push_back(j);
        cout << "a "<< i << " " << j << " " << w << endl;

        return true;
    };

    m -= n - 1;
    while(m > 0) {
        m--;
        while(add(random_integer(1, n), random_integer(1, n), random_integer(0, max_weight)) == false);
    }
    
    vector<bool> vis(n + 1); // check reachability from 1
    queue<int> q;
    q.push(s);
    vis[s] = true;
    while(q.size()) {
        int u = q.front();
        q.pop();
        assert(adj[u].size() <= average_outdegree + 1); // check degree
        for(int v: adj[u]) {
            if(!vis[v]){
                vis[v] = true;
                q.push(v);
            }
        }
    }
    assert(accumulate(vis.begin(), vis.end(), 0ll) == n);
    return 0;
}