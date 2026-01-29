// Generate a random sparse graph in DIMACS format

#include<bits/stdc++.h>
#include <random>
#define int long long
using namespace std;

signed main(signed argc, char **argv) {
    if(argc < 4) {
        cout << "must have 4 arguments: number_of_vertices C density seed" << endl;
        return 1;
    }

    int n = atoi(argv[1]);
    int C = atoi(argv[2]);
    int density = atoi(argv[3]);
    int seed = atoi(argv[4]);
    
    mt19937_64 gen(seed);
    auto random_integer = [&](int l, int r) {
        uniform_int_distribution<uint64_t> dis(l, r);
        return dis(gen);
    };

    int m = min(n*density,n*(n-1));
    int create_m = m - n;
    m = n+max(0LL, create_m);
    cout << "p sp " << n << " " << m << endl;

    int s = 1;
    vector<vector<int>> adj(n + 1);
    auto canAddEdge = [&](int i, int j) -> bool {
        if(find(adj[i].begin(), adj[i].end(), j) != adj[i].end() 
        || i == j) return false; 
        return true;
    };

    
    {   // hamiltonian cycle
        vector<int> v;
        for(int i=1;i<=n;i++) v.push_back(i);
        shuffle(v.begin(), v.end(), gen);

        for(int i=0;i<n;i++){
            adj[v[i]].push_back(v[(i+1)%n]);
        }
    } 

    for(int i = 1; i <= n; i++) {
        for(int j: adj[i]) {
            cout << "a "<< i << " " << j << " " << random_integer(0, C) << endl;
        }
    }

    auto add = [&](int i, int j, int w) {
        if(canAddEdge(i, j) == false) return false;

        adj[i].push_back(j);
        cout << "a "<< i << " " << j << " " << w << endl;

        return true;
    };

    while(create_m > 0) {
        create_m--;
        while(add(random_integer(1, n), random_integer(1, n), random_integer(0, C)) == false);
    }
    
    vector<bool> vis(n + 1); // check reachability from 1
    queue<int> q;
    q.push(s);
    vis[s] = true;
    while(q.size()) {
        int u = q.front();
        q.pop();
       
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