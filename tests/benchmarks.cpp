#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../helpers/external/doctest.h"

#define ANKERL_NANOBENCH_IMPLEMENT
#include "../helpers/external/nanobench.h"

#include "../bmssp.hpp"
#include "../bmssp-expected.hpp"
#include "common.cpp"

#include "../helpers/dijkstra.hpp"
#include "../helpers/common.hpp"

TEST_CASE("Benchmark WC") {
    using distT = long long;
    std::vector<std::string> paths = {"../tests/graphs/random32D3.gr",
        // "../tests/graphs/random256D3.gr", "../tests/graphs/random1024D3.gr", "../tests/graphs/random4096D3.gr"
    };
    for(std::string path: paths) {
        auto [adj, m] = readGraph<distT>(path);

        spp::dijkstra<distT> dijkstra(adj);
        spp::bmssp<distT> bmssp(adj);
        bmssp.prepare_graph(false);

        n = adj.size();
        ankerl::nanobench::Bench bench;
        // bench.output(nullptr);

        bench.run(path, [&] {
            auto [dist, pred] = dijkstra.execute(0);
            auto path1 = dijkstra.get_shortest_path(n - 1);
            ankerl::nanobench::doNotOptimizeAway(dist);
            ankerl::nanobench::doNotOptimizeAway(pred);
            ankerl::nanobench::doNotOptimizeAway(path);
        });
        bench.render(ankerl::nanobench::templates::json(), std::cout);
    }
}