struct timerT {
    chrono::_V2::system_clock::time_point begin, end;
    timerT() {
        start();
    }
    void start() {
        begin = chrono::high_resolution_clock::now();
    }
    void stop() {
        end = chrono::high_resolution_clock::now();
    }
    int elapsed() {
        return chrono::duration_cast<chrono::milliseconds>(end - begin).count();
    }
};

long long check_sum(auto &v) {
    return accumulate(v.begin(), v.end(), 0ll);
}

auto readGraph(string path) {
    vector<vector<pair<int, long long>>> adj;

    int n = -1, m = 0, c = 0;
    string line, tmp;
    ifstream in(path);

    while(getline(in, line)) {
        stringstream ss(line);
        if(line[0] == 'p') {
            string tmp;
            ss >> tmp >> tmp >> n >> m;
            adj.assign(n + 1, {});
        } else if(line[0] == 'a') {
            int a, b, w; ss >> tmp >> a >> b >> w;
            adj[a].emplace_back(b, w);
            c = max(c, w);
        }
    }
    
    return adj;
}