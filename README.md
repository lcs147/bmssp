# BMSSP: A C++ Shortest Path Algorithm

A header-only C++ implementation of the single-source shortest path (SSSP) algorithm for sparse directed graphs with non-negative weights, based on the 2025 paper by Duan et al. This algorithm is asymptotically faster than Dijkstra's on sparse graphs.

**Paper:**
> Ran Duan, Jiayi Mao, Xiao Mao, Xinkai Shu, and Longhui Yin. "Breaking the Sorting Barrier for Directed Single-Source Shortest Paths." *STOC 2025*. [https://doi.org/10.1145/3717823.3718179](https://doi.org/10.1145/3717823.3718179)

---

## Features
*   **Faithfull:** Implements the `O(m log^(2/3) n)` time SSSP algorithm as described in the paper.
*   **Header-Only:** Simply include `bmssp.hpp` in your project.
*   **Flexible:** Supports all C++ standard numeric types (`long long`, `double`, etc.) for edge weights.

## Performance Benchmarks

This implementation is a proof of concept for a theoretical algorithm. While BMSSP offers a superior asymptotic complexity, this is outweighed in practice by high constant factors and the overhead of its more complex data structures.

As a result, a standard implementation of Dijkstra's algorithm remains significantly faster for typical real-world scenarios, as shown in the benchmarks with random graphs below.

| Number of Vertices    | Number of Edges   | Dijkstra Time (ms)    | BMSSP Time (ms)| Time Ratio BMSSP / Dijkstra
| :---                  | :---              | :---                  | :---            | :---            |
|2<sup>12</sup>|3 * 2<sup>12</sup>|0.431|2.244|5.206|
|2<sup>15</sup>|3 * 2<sup>15</sup>|4.769|20.455|4.289|
|2<sup>20</sup>|3 * 2<sup>20</sup>|418.830|1450.863|3.464|
|2<sup>25</sup>|3 * 2<sup>25</sup>|21765.062|85382.069|3.923|

A more detailed performance benchmark can be found in [https://arxiv.org/abs/2511.03007](https://arxiv.org/abs/2511.03007)
## Requirements

* C++20 or later.

## Usage

### 1. Installation

Copy the `bmssp.hpp` header into your project and include it.

```cpp
#include "path/to/bmssp.hpp"
```

### 2. Example

The following demonstrates how to compute shortest paths. Nodes must be numbered from `0` to `n-1`.

```cpp
#include "../bmssp.hpp"
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
        auto path = solver.get_shortest_path(i);
        for(int x: path) std::cout << x << " ";
        std::cout << std::endl;
    }

    return 0;
}
```

**Output:**
```
Shortest distance from source 0:
  To 0: 0
  To 1: 7
  To 2: 3
  To 3: 9
  To 4: 5
Shortest path from source 0:
  To 0: 0 
  To 1: 0 2 1 
  To 2: 0 2 
  To 3: 0 2 1 3 
  To 4: 0 2 4 
```

## API

*   `bmssp<T>(int n)`: Constructor for a graph with `n` vertices.
*   `void addEdge(int u, int v, T weight)`: Adds a directed edge.
*   `void prepare_graph(bool exec_const_degree_transformation)`: Prepares the graph for computation. Must be called once after adding all edges.
*   `std::pair<std::vector<T>, std::vector<int>> execute(int s)`: Computes shortest paths from a source node `s`.
*   `std::vector<int> get_shortest_path(int destination)`: Returns a shortest path from `s` to `destination`

## How to Cite This Implementation in Your Research?

Directly:
> Lucas Castro, Thailsson Clementino, and Rosiane de Freitas. 2025. Implementation and brief experimental analysis of the Duan et al. (2025) algorithm for single-source shortest paths. [https://doi.org/10.48550/arXiv.2511.03007](https://doi.org/10.48550/arXiv.2511.03007)


Bibtex:
```bibtex
@misc{castro_implementation_2025,
  title = {Implementation and Brief Experimental Analysis of the {{Duan}} et al. (2025) Algorithm for Single-Source Shortest Paths},
  author = {Castro, Lucas and Clementino, Thailsson and {de Freitas}, Rosiane},
  year = 2025,
  publisher = {arXiv},
  doi = {10.48550/arXiv.2511.03007}
}
```
