
#ifdef LOCAL
#include "debug.cpp"
#else
#define debug(...) 0
#endif

#include "bmssp.hpp"
#include "dijkstra.hpp"

#include <bits/stdc++.h>
using namespace std;

double calculateMean(auto data) {
    if (data.empty()) {
        return 0.0;
    }
    double sum = accumulate(data.begin(), data.end(), 0.0);
    return sum / data.size();
}
double calculatePopulationSD(auto data) {
    if (data.size() <= 1) {
        return 0.0;
    }
    double mean = calculateMean(data);
    double squaredDifferenceSum = 0.0;
    for (double value : data) {
        squaredDifferenceSum += pow(value - mean, 2);
    }
    double variance = squaredDifferenceSum / data.size();
    return sqrt(variance);
}

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
        return chrono::duration_cast<chrono::microseconds>(end - begin).count();
    }
};

long long check_sum(auto &v) {
    return accumulate(v.begin(), v.end(), 0ll);
}

using distT = double;
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
            adj.assign(n, {});
        } else if(line[0] == 'a') {
            int a, b;
            distT w;
            ss >> tmp >> a >> b >> w;
            adj[a].emplace_back(b, w);
            c = max(c, w);
        }
    }
    
    return adj;
}

signed main(int argc, char **argv) {
    if(argc < 3) return 1;

    string graph_path = argv[1];
    string algorithm = argv[2];
    int reps = 1;
    if(argc >= 4) reps = atoi(argv[3]);

    timerT timer;
    vector<distT> d;
    auto adj = readGraph(graph_path);

    debug(adj.size());
    int s = 130;
    if(s >= adj.size()) s = 1;

    vector<int> times;
    if(algorithm == "bmssp") {
        spp::bmssp<distT> spp(adj);
        spp.prepare_graph(false); //true = tranform graph to have out-degree <= 2.
        
        for(int i = 0; i < reps; i++) {
            timer.start();
            d = spp.execute(s);
            timer.stop();
            times.push_back(timer.elapsed());
        }
    } else {
        spp::dijkstra<distT> spp(adj);
        for(int i = 0; i < reps; i++) {
            timer.start();
            d = spp.execute(s);
            timer.stop();
            times.push_back(timer.elapsed());
        }
    }

    string graph_name = graph_path.substr(graph_path.find("graphs/") + 7);
    cout << fixed << setprecision(0);
    cout << algorithm << " on " << graph_name << " source: " << s << " reps: " << reps << " size: " << adj.size() << endl;
    cout << "time: " << ceil(calculateMean(times)) << " us" << endl;
    cout << "std: " << ceil(calculatePopulationSD(times)) << " us" << endl;
    cout << "checksum: " << check_sum(d) << endl;
    return 0;
}