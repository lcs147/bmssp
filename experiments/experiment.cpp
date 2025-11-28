
#ifdef LOCAL
#include "../helpers/debug.cpp"
#else
#define debug(...) 0
#endif

#include "../bmssp.hpp"
#include "../helpers/dijkstra.hpp"

#include <bits/stdc++.h>
#include "../helpers/common.hpp"
#include "../helpers/external/json.hpp"
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

signed main(int argc, char **argv) {
    if(argc < 3) return 1;

    string graph_path = argv[1];
    string algorithm = argv[2];

    int reps = 1;
    if(argc >= 4) reps = atoi(argv[3]);
    
    int s = 1;
    if(argc >= 5) s = atoi(argv[4]);

    timerT timer;
    vector<distT> d;
    vector<int> pred;
    auto [adj, m] = readGraph<distT>(graph_path);

    vector<int> times;
    if(algorithm == "bmssp") {
        spp::bmssp<distT> spp(adj);
        spp.prepare_graph(false);
        
        for(int i = 0; i < reps; i++) {
            timer.start();
            tie(d, pred) = spp.execute(s);
            timer.stop();
            times.push_back(timer.elapsed());
        }
    } else {
        spp::dijkstra<distT> spp(adj);
        for(int i = 0; i < reps; i++) {
            timer.start();
            tie(d, pred) = spp.execute(s);
            timer.stop();
            times.push_back(timer.elapsed());
        }
    }

    nlohmann::json j;
    j["algorithm"] = algorithm;
    j["graph_name"] = graph_path.substr(graph_path.find("graphs/") + 7);
    j["source"] = s;
    j["reps"] = reps;
    j["n"] = adj.size();
    j["m"] = m;
    j["time_us"] = ceil(calculateMean(times));
    j["std_us"] = ceil(calculatePopulationSD(times));
    j["checksum"] = check_sum(d);

    std::cout << j.dump(4) << std::endl;
    return 0;
}