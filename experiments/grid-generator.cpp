#include<bits/stdc++.h>
#include <random>
#define int long long
using namespace std;

signed main(signed argc, char **argv) {
    if(argc < 3) {
        cout << "must have 4 arguments: n_row n_col C seed" << endl;
        return 1;
    }

    int n_row = atoi(argv[1]);
    int n_col = atoi(argv[2]);
    int C = atoi(argv[3]); // max_weight. If C == 0, euclidean distance
    int seed = atoi(argv[4]);
    
    mt19937_64 gen(seed);
    auto random_integer = [&](int l, int r) {
        uniform_int_distribution<uint64_t> dis(l, r);
        return dis(gen);
    };

    int s = 0;

    int n = n_row * n_col, m = 0;
    cout << n << "\n";
    vector<vector<pair<int, double>>> adj(n + 1);

    int wallcol = n_col / 2;
    auto add = [&](int i, int j, int ii, int jj, double w) {
        if(max({i, ii}) >= n_row || max({j, jj}) >= n_col || min({i, ii, j, jj}) < 0) return;
        int id1 = i * n_col + j;
        int id2 = ii * n_col + jj;
        if(jj == wallcol && ii >= 5) w = (n_col+n_row+1) * (C+1);  
        adj[id1].push_back({id2, w});
        adj[id2].push_back({id1, w});
        m += 2;
    };

    for(int i = 0; i < n_row; i++) {
        for(int j = 0; j < n_col; j++) {
            if(C){
                int w = random_integer(1,C);
                add(i, j, i, j + 1, w);
                w = random_integer(1,C);
                add(i, j, i + 1, j, w);
                w = random_integer(1,C);
                add(i, j, i + 1, j + 1, w);
                w = random_integer(1,C);
                add(i, j, i + 1, j - 1, w);
            }else{
                add(i, j, i, j + 1, 1);
                add(i, j, i + 1, j, 1);
                add(i, j, i + 1, j + 1, sqrt(2));
                add(i, j, i + 1, j - 1, sqrt(2));
            }
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