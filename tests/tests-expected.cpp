
#include "../helpers/external/doctest.h"

#include "../bmssp.hpp"
#include "common.cpp"

#include "../helpers/dijkstra.hpp"
#include "../helpers/common.hpp"

#include <vector>
#include <string>

TEST_CASE("King graphs with a wall") {
    using distT = double;
    std::vector<std::string> paths = {"../tests/graphs/grid5.gr", "../tests/graphs/grid25.gr", "../tests/graphs/grid100.gr"};
    for(std::string path: paths) {
        auto [adj, m] = readGraph<distT>(path);

        spp::dijkstra<distT> dijkstra(adj);
        spp::bmssp<distT> bmssp(adj);
        bmssp.prepare_graph(false);

        n = adj.size();
        int grid_sz = sqrt(n);
        for(int source: {0, grid_sz - 1, grid_sz * (grid_sz - 1), n - 2, n / 2}) {
            checkReturns<distT>(source, dijkstra, bmssp, false);
        }
    }
}

TEST_CASE("Small random graphs") {
    using distT = long long;
    std::vector<std::string> paths = {"../tests/graphs/random32D3.gr", "../tests/graphs/random256D3.gr", "../tests/graphs/random1024D3.gr"};
    for(std::string path: paths) {
        auto [adj, m] = readGraph<distT>(path);

        spp::dijkstra<distT> dijkstra(adj);
        spp::bmssp<distT> bmssp(adj);
        bmssp.prepare_graph(false);

        n = adj.size();
        for(int source: {1, n / 3, n / 2, n / 3 * 2, n - 1}) {
            checkReturns<distT>(source, dijkstra, bmssp, false);
        }
    }
}


TEST_CASE("King graphs with a wall - CD") {
    // using distT = double;
    // std::vector<std::string> paths = {"../tests/graphs/grid5.gr", "../tests/graphs/grid25.gr", "../tests/graphs/grid100.gr"};
    // for(std::string path: paths) {
    //     auto [adj, m] = readGraph<distT>(path);

    //     spp::dijkstra<distT> dijkstra(adj);
    //     spp::bmssp<distT> bmssp(adj);
    //     bmssp.prepare_graph(true);

    //     n = adj.size();
    //     int grid_sz = sqrt(n);
    //     for(int source: {0, grid_sz - 1, grid_sz * (grid_sz - 1), n - 2, n / 2}) {
    //         checkReturns<distT>(source, dijkstra, bmssp, true);
    //     }
    // }
}

TEST_CASE("Small random graphs - CD") {
    using distT = long long;
    std::vector<std::string> paths = {"../tests/graphs/random32D3.gr", "../tests/graphs/random256D3.gr", "../tests/graphs/random1024D3.gr"};
    for(std::string path: paths) {
        auto [adj, m] = readGraph<distT>(path);

        spp::dijkstra<distT> dijkstra(adj);
        spp::bmssp<distT> bmssp(adj);
        bmssp.prepare_graph(true);

        n = adj.size();
        for(int source: {1, n / 3, n / 2, n / 3 * 2, n - 1}) {
            checkReturns<distT>(source, dijkstra, bmssp, true);
        }
    }
}