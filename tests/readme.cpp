#include "../include/bmssp.hpp"
#include <iostream>
#include <vector>

using T = long long;

int main() {
    // 1. Define the graph
    int n = 5;
    std::vector<std::tuple<int, int, T>> edges = {
        {0, 1, 10}, {0, 2, 3}, {1, 3, 2}, {2, 1, 4},
        {2, 3, 8}, {2, 4, 2}, {3, 4, 5}
    };
    int source_node = 0;

    // 2. Initialize and build the graph
    spp::bmssp<T> solver(n);
    // You can also use the version with average-case guarantees that uses less memory
    // spp_expected::bmssp<T> solver(n);
    for (const auto& [u, v, weight] : edges) {
        solver.addEdge(u, v, weight);
    }

    // 3. Prepare the graph (must be called once)
    solver.prepare_graph(false);
    // use solver.prepare_graph(true) if the graph does not have contant out-degree

    // 4. Compute shortest paths
    auto [distances, predecessors] = solver.execute(source_node);

    // 5. Print results
    std::cout << "Shortest distance from source " << source_node << ":" << std::endl;
    for (int i = 0; i < n; ++i) {
        std::cout << "  To " << i << ": ";
        if (distances[i] == solver.oo) {
            std::cout << "unreachable" << std::endl;
        } else {
            std::cout << distances[i] << std::endl;
        }
    }
    
    std::cout << "Shortest path from source " << source_node << ":" << std::endl;
    for (int i = 0; i < n; ++i) {
        std::cout << "  To " << i << ": ";
        auto path = solver.get_shortest_path(i, predecessors);
        for(int x: path) std::cout << x << " ";
        std::cout << std::endl;
    }

    return 0;
}