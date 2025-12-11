
template<typename uniqueDistT>
class batchPQ { // batch priority queue, implemented as in Lemma 3.3
    template<typename K, typename V>
    using hash_map = std::unordered_map<K, V>;

    using elementT = std::pair<int,uniqueDistT>;
    
    struct CompareUB {
        template <typename It>
        bool operator()(const std::pair<uniqueDistT, It>& a, const std::pair<uniqueDistT, It>& b) const {
            if (a.first != b.first) return a.first < b.first;
            return  addressof(*a.second) < addressof(*b.second);
        }
    };
    
    typename std::list<std::list<elementT>>::iterator it_min;
    
    std::list<std::list<elementT>> D0,D1;
    std::set<std::pair<uniqueDistT,typename std::list<std::list<elementT>>::iterator>,CompareUB> UBs;
    
    int M,size_;
    uniqueDistT B;
    
    hash_map<int, uniqueDistT> actual_value;
    hash_map<int, std::pair<typename std::list<std::list<elementT>>::iterator, typename std::list<elementT>::iterator>> where_is0, where_is1;
    
public:

    batchPQ(int M_, uniqueDistT B_): M(M_), B(B_) {
        D1.push_back(std::list<elementT>());
        UBs.insert(make_pair(B_,D1.begin()));
        size_ = 0;
    }

    int size(){
        return size_;
    }
    
    void insert(uniqueDistT x){ // O(lg(Block Numbers))         
        uniqueDistT b = x;
        int a = get<2>(b);
    
        // checking if exists
        auto it_exist = actual_value.find(a);
        int exist = (it_exist != actual_value.end()); 
    
        if(exist && it_exist->second > b){
            delete_(x);
        }else if(exist){
            return;
        }
        
        // Searching for the first block with UB which is > 
        auto it_UB_block = UBs.lower_bound({b,it_min});
        auto [ub,it_block] = (*it_UB_block);
        
        // Inserting key/value (a,b)
        auto it = it_block->insert(it_block->end(),{a,b});
        where_is1[a] = {it_block, it};
        actual_value[a] = b;
    
        size_++;
    
        // Checking if exceeds the sixe limit M
        if((*it_block).size() > M){
            split(it_block);
        }
    }
    
    void batchPrepend(const std::vector<uniqueDistT> &v){
        std::list<elementT> l;
        for(auto x: v){
            l.push_back({get<2>(x),x});
        }
        batchPrepend(l);
    }

    std::pair<uniqueDistT, std::vector<int>> pull(){ // O(M)
        std::vector<elementT> s0,s1;
        s0.reserve(2 * M); s1.reserve(M);
    
        auto it_block = D0.begin();
        while(it_block != D0.end() && s0.size() <= M){ // O(M)   
            for (const auto& x : *it_block) s0.push_back(x);
            it_block++;
        }
    
        it_block = D1.begin();
        while(it_block != D1.end() && s1.size() <= M){   //O(M)
            for (const auto& x : *it_block) s1.push_back(x);
            it_block++;
        }
    
        if(s1.size() + s0.size() <= M){
            std::vector<int> ret;
            ret.reserve(s1.size()+s0.size());
            for(auto [a,b] : s0) {
                ret.push_back(a);
                delete_({b});
            }
            for(auto [a,b] : s1){
                ret.push_back(a);
                delete_({b});
            } 
            
            return {B, ret};
        }else{  
            std::vector<elementT> &l = s0;
            l.insert(l.end(), s1.begin(), s1.end());

            uniqueDistT med = selectKth(l, M);
            std::vector<int> ret;
            ret.reserve(M);
            for(auto [a,b]: l){
                if(b < med) {
                    ret.push_back(a);
                    delete_({b});
                }
            }
            return {med,ret};
        }
    }
    inline void erase(int key) {
        if(actual_value.find(key) != actual_value.end())
            delete_({-1, -1, key, -1});
    }
    
private:
    void delete_(uniqueDistT x){    
        int a = get<2>(x);
        uniqueDistT b = actual_value[a];
        
        auto it_w = where_is1.find(a);
        if((it_w != where_is1.end())){
            auto [it_block,it] = it_w->second;
            
            (*it_block).erase(it);
            where_is1.erase(a);
    
            if((*it_block).size() == 0){
                auto it_UB_block = UBs.lower_bound({b,it_block});  
                
                if((*it_UB_block).first != B){
                    UBs.erase(it_UB_block);
                    D1.erase(it_block);
                }
            }
        }else{
            auto [it_block,it] = where_is0[a];
            (*it_block).erase(it);
            where_is0.erase(a);
            if((*it_block).size() == 0) D0.erase(it_block); 
        }
    
        actual_value.erase(a);
        size_--;
    }
    
    uniqueDistT selectKth(std::vector<elementT> &v, int k) {
        std::nth_element(v.begin(), v.begin() + k, v.end());
        return v[k].second;
    }

        
    void split(std::list<std::list<elementT>>::iterator it_block){ // O(M) + O(lg(Block Numbers))
        int sz = (*it_block).size();
        
        std::vector<elementT> v((*it_block).begin() , (*it_block).end());
        uniqueDistT med = selectKth(v,(sz/2)); // O(M)
        
        auto pos = it_block;
        pos++;

        auto new_block = D1.insert(pos,std::list<elementT>());
        auto it = (*it_block).begin();
    
        while(it != (*it_block).end()){ // O(M)
            if((*it).second >= med){
                (*new_block).push_back(move(*it));
                auto it_new = (*new_block).end(); it_new--;
                where_is1[(*it).first] = {new_block, it_new};
    
                it = (*it_block).erase(it);
            }else{
                it++;
            }
        }
    

        // Updating UBs   
        // O(lg(Block Numbers))
        uniqueDistT UB1 = {get<0>(med),get<1>(med),get<2>(med),get<3>(med)-1};
        auto it_lb = UBs.lower_bound({UB1,it_min});
        auto [UB2,aux] = (*it_lb);
        
        UBs.insert({UB1,it_block});
        UBs.insert({UB2,new_block});
        
        UBs.erase(it_lb);
    }
    
    void batchPrepend(const std::list<elementT> &l) { // O(|l| log(|l|/M) ) 
        int sz = l.size();
        
        if(sz == 0) return;
        if(sz <= M){
    
            D0.push_front(std::list<elementT>());
            auto new_block = D0.begin();
            
            for(auto &x : l){ 
                auto it = actual_value.find(x.first);
                int exist = (it != actual_value.end()); 
    
                if(exist && it->second > x.second){
                    delete_(x.second);
                }else if(exist){
                    continue;
                }
    
                (*new_block).push_back(x);
                auto it_new = (*new_block).end(); it_new--;
                where_is0[x.first] = {new_block, it_new};
                actual_value[x.first] = x.second;
                size_++;
            }
            if(new_block->size() == 0) D0.erase(new_block);
            return;
        }

        std::vector<elementT> v(l.begin(), l.end());
        uniqueDistT med = selectKth(v, sz/2);
    
        std::list<elementT> less,great;
        for(auto [a,b]: l){
            if(b < med){
                less.push_back({a,b});
            }else if(b > med){
                great.push_back({a,b});
            }
        }
        
        great.push_back({get<2>(med),med});

        batchPrepend(great);
        batchPrepend(less);
    }
};
