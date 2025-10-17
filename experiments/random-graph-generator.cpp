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

    vector<vector<int>> adj(n + 1);
    auto add = [&](int i, int j, int w) {
        if(adj[i].size() >= average_outdegree + 1 || find(adj[i].begin(), adj[i].end(), j) != adj[i].end() || i == j) return false; // no duplicated edges, no self-loops, and not too big of a degree
        adj[i].push_back(j);
        cout << "a "<< i << " " << j << " " << w << endl;

        return true;
    };

    const int oo = 1e18;
    int m = n * average_outdegree;
    cout << "p " <<  n << " " << m << endl;
    for(int i = 2; i <= n; i++) { // make 1 reach all vertices, but with infinite cost
        while(add(random_integer(1, i - 1), i, random_integer(oo / 10, oo)) == false);
    }

    // m -= n - 1;
    while(m > 0) {
        m--;
        while(add(random_integer(1, n), random_integer(1, n), random_integer(1, max_weight)) == false);
    }
    
    vector<bool> vis(n + 1); // check reachability from 1
    int s = 1;
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