#include<bits/stdc++.h>
using namespace std;

template<typename distT>
auto readGraph(string path) {
    vector<vector<pair<int, distT>>> adj;

    int n = -1, m = 0;
    distT c = 0;
    string line, tmp;
    ifstream in(path);

    while(getline(in, line)) {
        stringstream ss(line);
        if(line[0] == 'p') {
            string tmp;
            ss >> tmp >> tmp >> n >> m;
            adj.assign(n + 1, {});
        } else if(line[0] == 'a') {
            int a, b;
            distT w;
            ss >> tmp >> a >> b >> w;
            adj[a].emplace_back(b, w);
            c = max(c, w);
        }
    }
    
    return make_pair(adj, m);
}