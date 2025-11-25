#include<bits/stdc++.h>
#include <random>
#define int long long
using namespace std;

signed main(signed argc, char **argv) {
    if(argc < 3) {
        cout << "must have 2 arguments: grid_size seed" << endl;
        return 1;
    }

    int grid_sz = atoi(argv[1]);
    int seed = atoi(argv[2]);
    
    mt19937_64 gen(seed);
    auto random_integer = [&](int l, int r) {
        uniform_int_distribution<uint64_t> dis(l, r);
        return dis(gen);
    };

    int s = 0;

    int n = grid_sz * grid_sz, m = 0;
    vector<vector<pair<int, double>>> adj(n + 1);

    int wallcol = grid_sz / 2;
    auto add = [&](int i, int j, int ii, int jj, double w) {
        if(max({i, ii, j, jj}) >= grid_sz || min({i, ii, j, jj}) < 0) return;
        int id1 = i * grid_sz + j % grid_sz;
        int id2 = ii * grid_sz + jj % grid_sz;
        if(jj == wallcol && ii >= 5) w = grid_sz * 10;
        adj[id1].push_back({id2, w});
        adj[id2].push_back({id1, w});
        m += 2;
    };

    for(int i = 0; i < grid_sz; i++) {
        for(int j = 0; j < grid_sz; j++) {
            add(i, j, i, j + 1, 1);
            add(i, j, i + 1, j + 1, sqrt(2));
            add(i, j, i + 1, j - 1, sqrt(2));
        }
    }

    cout << "p sp " << n << " " << m << endl;
    for(int i = 0; i < adj.size(); i++) {
        for(auto [j, w]: adj[i]) {
            cout << "a "<< i << " " << j << " " << w << endl;
        }
    }
    
    vector<bool> vis(n + 1); // check reachability from 1
    queue<int> q;
    q.push(s);
    vis[s] = true;
    while(q.size()) {
        int u = q.front();
        q.pop();
        // assert(adj[u].size() <= average_outdegree + 1); // check degree
        for(auto [v, w]: adj[u]) {
            if(!vis[v]){
                vis[v] = true;
                q.push(v);
            }
        }
    }
    assert(accumulate(vis.begin(), vis.end(), 0ll) == n);
    return 0;
}