#include <bits/stdc++.h>
using namespace std;

#ifdef LOCAL
#include "cp-lib/helpers/debug.cpp"
#else
#define debug(...) 0
#endif

#include "bmssp.hpp"
#include "dijkstra.hpp"
#include "helpers.hpp"

signed main(int argc, char **argv) {
    if(argc < 1) return 1;
    string graph_path = argv[1];
    string algorithm = argv[2];
    
    int s = 1;
    timerT timer;
    using distT = long long;
    vector<distT> d;
    auto adj = readGraph(graph_path);
    debug(adj.size());

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