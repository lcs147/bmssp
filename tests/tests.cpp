
#include "../bmssp.hpp"
#include "../helpers/dijkstra.hpp"
#include "../helpers/common.hpp"

#include <vector>
#include <string>
using namespace std;

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../helpers/external/doctest.h"

int n;

template<typename distT>
auto checkReturns(int source, auto &dijkstra, auto &bmssp) {
    auto [dist_expected, pred_expected] = dijkstra.execute(source);
    auto [dist_realized, pred_realized] = bmssp.execute(source);
    CHECK(dist_expected == dist_realized);
    
    auto distance = [&](const vector<int> &path) {
        distT res = distT();
        for(int i = 0; i + 1 < path.size(); i++) {
            int u = path[i], v = path[i + 1];
            distT cur_w = -1;
            for(auto [x, w]: dijkstra.adj[u]) {
                if(x == v) {
                    cur_w = w;
                    break;
                }
            }
            CHECK(cur_w != -1);
            res += cur_w;
        }
        return res;
    };
    
    for(int destination: {1, n / 3, n / 2, n / 3 * 2, n - 1}) {
        auto path1 = dijkstra.get_shortest_path(destination);
        if(path1.size()) {
            CHECK(path1[0] == source);
            CHECK(path1.back() == destination);
        }

        auto path2 = bmssp.get_shortest_path(destination);
        if(path2.size()) {
            CHECK(path2[0] == source);
            CHECK(path2.back() == destination);
        }

        CHECK(distance(path1) == distance(path2));
    }
}

TEST_CASE("King graphs with a wall") {
    using distT = double;
    vector<string> paths = {"../tests/graphs/grid5.gr", "../tests/graphs/grid25.gr", "../tests/graphs/grid100.gr"};
    for(string path: paths) {
        auto [adj, m] = readGraph<distT>(path);

        spp::dijkstra<distT> dijkstra(adj);
        spp::bmssp<distT> bmssp(adj);
        bmssp.prepare_graph(false);

        n = adj.size();
        int grid_sz = sqrt(n);
        for(int source: {0, grid_sz - 1, grid_sz * (grid_sz - 1), n - 2, n / 2}) {
            checkReturns<distT>(source, dijkstra, bmssp);
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

        n = adj.size();
        for(int source: {1, n / 3, n / 2, n / 3 * 2, n - 1}) {
            checkReturns<distT>(source, dijkstra, bmssp);
        }
    }
}