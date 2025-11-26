#include "../bmssp.hpp"
#include "../dijkstra.hpp"
#include "../helpers/common.hpp"

#include <vector>
#include <string>
#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>

TEST_CASE("King graphs with a wall") {

    // cout << "Local directory: " << std::filesystem::current_path().string() << endl;
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
            REQUIRE(expected == realized);
        }
    }
}