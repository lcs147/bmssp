#include "bmssp.hpp"
#include "dijkstra.hpp"

#ifdef LOCAL
#include "debug.cpp"
#else
#define debug(...) 0
#endif

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
    vector<int> times;
    for(int i = 0; i < reps; i++) {
        if(algorithm == "bmssp") {
            spp::bmssp<distT> spp(adj);
            spp.prepare_graph(false); //true = tranform graph to have out-degree <= 2.
            timer.start();
            d = spp.execute(s);
            timer.stop();
        } else {
            spp::dijkstra<distT> spp(adj);
            timer.start();
            d = spp.execute(s);
            timer.stop();
        }
        times.push_back(timer.elapsed());
    }

    cout << fixed << setprecision(0);
    cout << algorithm << " on " << graph_path << " source: " << s << " reps: " << reps << endl;
    cout << "time: " << ceil(calculateMean(times)) << " us" << endl;
    cout << "std: " << ceil(calculatePopulationSD(times)) << " us" << endl;
    cout << "checksum: " << check_sum(d) << endl;
    return 0;
}