#include <bits/stdc++.h>
using namespace std;
 
#define fastio ios::sync_with_stdio(false); cin.tie(0); cout.tie(0);
 
#ifdef LOCAL
#include "cp-lib/helpers/debug.cpp"
#else
#define debug(...) 0
#endif
 
void solve() {
    int n, m; cin >> n >> m;
    newspp<long long> spp(n);
    while(m--) {
        int a, b, w; cin >> a >> b >> w;
        a--; b--;
        spp.addEdge(a, b, w);
    }
    spp.prepare_graph();
    auto d = spp.execute(0);
    for(auto x: d) cout << x << " ";
    cout << endl;
}
 
signed main() {
    fastio;
 
    int t = 1;
    // in(t);
    for(int i = 0; i < t; i++) solve();
    
    return 0;
}