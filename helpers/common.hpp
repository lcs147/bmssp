#ifndef CASTRO_THAILSSON_BMSSP_COMMON
#define CASTRO_THAILSSON_BMSSP_COMMON

#include<string>
#include<iostream>
#include<fstream>
#include<sstream>

template<typename distT>
auto readGraph(std::string path) {
    std::vector<std::vector<std::pair<int, distT>>> adj;

    int n = -1, m = 0;
    distT c = 0;
    std::string line, tmp;
    std::ifstream in(path);

    bool has_0 = false;
    while(std::getline(in, line)) {
        std::stringstream ss(line);
        if(line[0] == 'p') {
            std::string tmp;
            ss >> tmp >> tmp >> n >> m;
            adj.assign(n + 1, {});
        } else if(line[0] == 'a') {
            int a, b;
            distT w;
            ss >> tmp >> a >> b >> w;
            adj[a].emplace_back(b, w);
            c = std::max(c, w);
            has_0 = has_0 || (a == 0) || (b == 0);
        }
    }
    if(has_0 == false) {
        for(int i = 0; i + 1 < adj.size(); i++) {
            adj[i] = move(adj[i + 1]);
            for(auto &[j, w]: adj[i]) j--;
        }
    }
    adj.pop_back();
    return std::make_pair(adj, m);
}

#endif