#include "../bmssp.hpp"
#include "../helpers/dijkstra.hpp"
#include "../helpers/common.hpp"

#include <vector>
#include <string>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../helpers/external/doctest.h"

TEST_CASE("King graphs with a wall") {
    using distT = double;
    vector<string> paths = {"../tests/graphs/grid5.gr", "../tests/graphs/grid25.gr", "../tests/graphs/grid100.gr"};
    for(string path: paths) {
        auto [adj, m] = readGraph<distT>(path);

        spp::dijkstra<distT> dijkstra(adj);
        spp::bmssp<distT> bmssp(adj);
        bmssp.prepare_graph(false);

        int n = adj.size();
        int grid_sz = sqrt(n);
        for(int source: {0, grid_sz - 1, grid_sz * (grid_sz - 1), n - 2, n / 2}) {
            auto expected = dijkstra.execute(source); 
            auto realized = bmssp.execute(source);
            CHECK(expected == realized);
        }
    }
}

TEST_CASE("Small random graphs") {
    using distT = long long;
    vector<string> paths = {"../tests/graphs/random32D3.gr", "../tests/graphs/random256D3.gr", "../tests/graphs/random1024D3.gr"};
    for(string path: paths) {
        auto [adj, m] = readGraph<distT>(path);

        spp::dijkstra<distT> dijkstra(adj);
        spp::bmssp<distT> bmssp(adj);
        bmssp.prepare_graph(false);

        int n = adj.size();
        for(int source: {1, n / 3, n / 2, n / 3 * 2, n - 1}) {
            auto expected = dijkstra.execute(source);
            auto realized = bmssp.execute(source);
            REQUIRE(expected == realized);
        }
    }
}