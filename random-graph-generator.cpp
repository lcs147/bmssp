// Generate a random sparse graph in DIMACS format

#include<bits/stdc++.h>
#include <random>
#define int long long
using namespace std;

signed main(signed argc, char **argv){

    std::mt19937_64 gen(atoi(argv[1]));
    auto rand = [&](int l, int r) {
        std::uniform_int_distribution<uint64_t> dis(l, r);
        return dis(gen);
    };

    set<pair<int, int>> s;
    auto add = [&](int i, int j, int w) {
        if(s.count({i, j}) || i == j) return false;

        s.insert({i, j});
        s.insert({j, i});

        cout << "a "<< i << " " << j << " " << w << endl;
        cout << "a "<< j << " " << i << " " << w << endl;

        return true;
    };

    int n = 100000, m = n * 4;
    cout << "p " <<  n << " " << m << endl;
    for(int i = 2; i <= n; i++) {
        add(rand(1, i - 1), i, rand(1, 5));
    }

    m -= n - 1;
    while(m--) {
        while(add(rand(1, n), rand(1, n), rand(1, 50)) == false);
    }
    
    return 0;
}