#define ANKERL_NANOBENCH_IMPLEMENT
#include "../helpers/external/nanobench.h"

// #include "../helpers/debug.cpp"
#include "../include/bmssp.hpp"

#include "../helpers/dijkstra.hpp"
#include "../helpers/common.hpp"

template<typename distT>
void runOnGraph(std::string path) {
    ankerl::nanobench::Bench bench;
    bench.relative(true);
    bench.title(path);
    bench.output(nullptr);
    
    auto [adj, m] = readGraph<distT>(path);
    int n = adj.size();


    auto run = [&](auto &spp) {
        auto [dist, pred] = spp.execute(0);
        auto path = spp.get_shortest_path(n - 1, pred);
        ankerl::nanobench::doNotOptimizeAway(dist);
        ankerl::nanobench::doNotOptimizeAway(pred);
        ankerl::nanobench::doNotOptimizeAway(path);
    };
    spp::dijkstra<distT> dijkstra(adj);
    bench.run("Dijkstra", [&] { run(dijkstra); });
    
    spp::bmssp<distT> bmssp(adj);
    bmssp.prepare_graph(false);
    bench.run("BMSSP-WC", [&] { run(bmssp); });
    spp_expected::bmssp<distT> bmssp_ex(adj);
    bmssp_ex.prepare_graph(false);
    bench.run("BMSSP-EXPECTED", [&] { run(bmssp_ex); });

    auto const& results = bench.results();
    if (results.empty()) return;

    // Use the first result (Dijkstra) as the baseline
    double baseline_ns = results[0].median(ankerl::nanobench::Result::Measure::elapsed);

    std::cout << "Graph Path: " << path << std::endl;
    std::cout << "Number of Vertices: " << n << " | Number of Edges: " << m << std::endl;
    std::cout << "--------------------------------------------------------------------------------\n";
    std::cout << "| Algorithm        | Time per Run (ns) | Relative | Total Runs (Samples) |\n";
    std::cout << "| :---             |              ---: |     ---: |                 ---: |\n";
for (auto const& r : results) {
        // ERROR WAS HERE: nanobench gives Seconds, not Nanoseconds.
        // Multiply by 1e9 to get Nanoseconds.
        double time_sec = r.median(ankerl::nanobench::Result::Measure::elapsed);
        double time_ns = time_sec * 1e9; 
        
        // Calculate ratio using the seconds (or ns, doesn't matter as long as units match)
        double ratio = time_sec / results[0].median(ankerl::nanobench::Result::Measure::elapsed);
        
        uint64_t total_runs = r.sum(ankerl::nanobench::Result::Measure::iterations);

        printf("| %-16s | %17.0f | %7.2fx | %20lu |\n", 
               r.config().mBenchmarkName.c_str(), 
               time_ns, // Now this is 54000000.0, not 0.054
               ratio, 
               total_runs);
    }
    std::cout << "--------------------------------------------------------------------------------\n";
}
int main() {
    runOnGraph<long long>("../tests/graphs/random16384D3.gr");
    runOnGraph<long long>("../tests/graphs/USA-road-t.NY.gr");
}