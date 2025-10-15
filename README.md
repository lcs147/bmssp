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

As a result, a standard implementation of Dijkstra's algorithm remains significantly faster for typical real-world scenarios, as shown in the benchmarks below.

| Graph         | Number of Vertices    | Number of Edges   | Dijkstra Time (ms)    | BMSSP Time (ms)|
| :---          | :---                  | :---              | :---                  | :---            |
| **NY**        |264,346|733,846|56|418|
| **CAL**       |1,890,815|4,657,742|436|2,920|
| **USA**       | 23,947,347|58,333,344|7,970|52,777|
| **Random (4x)**  |100,000|400,000|83|292|


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
#include <iostream>
#include <vector>
#include "bmssp.hpp"

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
    solver.prepare_graph();

    // 4. Compute shortest paths
    std::vector<T> distances = solver.execute(source_node);

    // 5. Print results
    std::cout << "Shortest paths from source " << source_node << ":" << std::endl;
    for (int i = 0; i < n; ++i) {
        std::cout << "  To " << i << ": ";
        if (distances[i] == solver.oo) {
            std::cout << "unreachable" << std::endl;
        } else {
            std::cout << distances[i] << std::endl;
        }
    }

    return 0;
}
```

**Output:**
```
Shortest paths from source 0:
  To 0: 0
  To 1: 7
  To 2: 3
  To 3: 9
  To 4: 5
```

## API

*   `bmssp<T>(int n)`: Constructor for a graph with `n` vertices.
*   `void addEdge(int u, int v, T weight)`: Adds a directed edge.
*   `void prepare_graph()`: Prepares the graph for computation. Must be called once after adding all edges.
*   `std::vector<T> execute(int s)`: Computes shortest paths from a source node `s`.
