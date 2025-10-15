// Generate a random sparse graph in DIMACS format

#include<bits/stdc++.h>
#include <random>
#define int long long
using namespace std;

signed main(signed argc, char **argv) {
    if(argc < 3) {
        cout << "must have 4 arguments: number_of_vertices average_outdegree max_weight seed" << endl;
        return 1;
    }

    int n = atoi(argv[1]);
    int average_outdegree = atoi(argv[2]);
    int max_weight = atoi(argv[3]);
    int seed = atoi(argv[4]);
    
    mt19937_64 gen(seed);
    auto random_integer = [&](int l, int r) {
        uniform_int_distribution<uint64_t> dis(l, r);
        return dis(gen);
    };

    set<pair<int, int>> edges;
    auto add = [&](int i, int j, int w) {
        if(edges.count({i, j}) || i == j) return false; // no duplicated edges and no self-loops

        edges.insert({i, j});
        cout << "a "<< i << " " << j << " " << w << endl;

        return true;
    };

    const int oo = 1e18;
    int m = n * average_outdegree;
    cout << "p " <<  n << " " << m << endl;
    for(int i = 2; i <= n; i++) { // make 1 reach all vertices, but with infinite cost
        add(random_integer(1, i - 1), i, random_integer(oo / 10, oo));
    }

    m -= n - 1;
    while(m--) {
        while(add(random_integer(1, n), random_integer(1, n), random_integer(1, max_weight)) == false);
    }
    
    return 0;
}