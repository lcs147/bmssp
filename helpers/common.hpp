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
        }
    }
    
    return std::make_pair(adj, m);
}