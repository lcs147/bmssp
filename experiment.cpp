#include <bits/stdc++.h>
using namespace std;

#ifdef LOCAL
#include "cp-lib/helpers/debug.cpp"
#else
#define debug(...) 0
#endif

#include "bmssp.hpp"
#include "dijkstra.hpp"

struct timerT {
    chrono::_V2::system_clock::time_point begin, end;
    timerT() {
        start();
    }
    void start() {
        begin = chrono::high_resolution_clock::now();
    }
    void stop() {
        end = chrono::high_resolution_clock::now();
    }
    int elapsed() {
        return chrono::duration_cast<chrono::milliseconds>(end - begin).count();
    }
};

long long check_sum(auto &v) {
    return accumulate(v.begin(), v.end(), 0ll);
}

auto readGraph(string path) {
    vector<vector<pair<int, long long>>> adj;

    int n = -1, m = 0, c = 0;
    string line, tmp;
    ifstream in(path);

    while(getline(in, line)) {
        stringstream ss(line);
        if(line[0] == 'p') {
            string tmp;
            ss >> tmp >> tmp >> n >> m;
            adj.assign(n + 1, {});
        } else if(line[0] == 'a') {
            int a, b, w; ss >> tmp >> a >> b >> w;
            adj[a].emplace_back(b, w);
            c = max(c, w);
        }
    }
    
    return adj;
}

signed main(int argc, char **argv) {
    if(argc < 1) return 1;
    string graph_path = argv[1];
    string algorithm = argv[2];
    
    int s = 1;
    timerT timer;
    using distT = long long;
    vector<distT> d;
    auto adj = readGraph(graph_path);

    const int reps = 5;
    long long tot_time = 0;
    for(int i = 0; i < reps; i++) {
        if(algorithm == "bmssp") {
            bmssp<distT> spp(adj);
            spp.prepare_graph();
            timer.start();
            d = spp.execute(s);
            timer.stop();
        } else {
            dijkstra<distT> spp(adj);
            timer.start();
            d = spp.execute(s);
            timer.stop();
        }
        tot_time += timer.elapsed();
    }

    cout << algorithm << " on " << graph_path << " source: " << s << " reps: " << reps << endl;
    cout << "time: " << tot_time / reps << " ms" << endl;
    cout << "checksum: " << check_sum(d) << endl;
    return 0;
}