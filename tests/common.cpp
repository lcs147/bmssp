#ifndef BMSSP_COMMON_TESTS
#define BMSSP_COMMON_TESTS

int n;

const double eps = 1e-6;
template<typename distT>
auto checkReturns(int source, auto &dijkstra, auto &bmssp, bool transformed) {
    auto [dist_expected, pred_expected] = dijkstra.execute(source);
    auto [dist_realized, pred_realized] = bmssp.execute(source);
    CHECK(dist_expected.size() == dist_realized.size());
    for(int i = 0; i < dist_expected.size(); i++) {
        REQUIRE_MESSAGE(dist_expected[i] == doctest::Approx(dist_realized[i]).epsilon(eps), "vertex ", i, " has wrong distance");
    }
    auto distance = [&](const std::vector<int> &path) {
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
        auto path1 = dijkstra.get_shortest_path(destination, pred_expected);
        if(path1.size()) {
            CHECK(path1[0] == source);
            CHECK(path1.back() == destination);
        }

        auto path2 = bmssp.get_shortest_path(destination, pred_realized);
        if(path2.size()) {
            CHECK(path2[0] == source);
            CHECK(path2.back() == destination);
        }

        CHECK(distance(path1) == doctest::Approx(distance(path2)).epsilon(eps));
    }
}

#endif