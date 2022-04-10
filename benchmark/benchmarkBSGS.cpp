//
// Created by zhi lin on 4/10/22.
//
#define ll long long

ll fastPow(ll a, ll b, ll p){
    ll res = 1;
    while(b){
        if(b&1)res = res*a%p;
        b=b>>1;
        a = a*a%p;
    }
    return res;
}

void benchmarkBSGS(ll a, ll b, ll c){
    map<ll,int> mp;
    if(a%c==0){
        return;
    }
    m = ceil(sqrt(c));
    ll ans;
    ll base = 1;
    // parallel
    for(int i=0;i<m;i++){
        ll newKey = base*b%c;
        mp[newKey] = i+1;
        base = base * a % c;
    }
    base = 1;
    ll stride = fastPow(a,m,c);
    // barrier
    // parallel
    for(int i=0;i<m;i++){
        ll findKey = base;
        if(mp[findKey]){
            return i*m-(mp[findKey]-1);
        }
        base = base*stride%c;
    }
    return -1;
}




















