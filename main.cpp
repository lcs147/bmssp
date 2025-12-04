#include "bmssp.hpp"
#include "helpers/common.hpp"

#include <bits/stdc++.h>
using namespace std;

long long check_sum(auto &v) {
    return accumulate(v.begin(), v.end(), 0ll);
}

signed main(int argc, char **argv) {
    string graph_path = argv[1];
    int s = 1;

    using distT = double;
    auto [adj, m] = readGraph<distT>(graph_path);
    spp::bmssp<distT> spp(adj);
    spp.prepare_graph(false);

    auto [d, pred] = spp.execute(s);

    if(d.back() == spp.oo) { // if 0-indexed, the last vertice is not necessary
        d.pop_back();
        pred.pop_back();
    } else { // if 1-indexed, the first vertex is not necessary
        d.erase(d.begin());
        pred.erase(pred.begin());
    }

    cout << "sum of all distances: " << check_sum(d) << endl;
    return 0;
}