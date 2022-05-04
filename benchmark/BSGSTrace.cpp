#include <iostream>
#include<map>
#include<cmath>
#include<fstream>
#include<vector>
#define ll long long
using namespace std;

//
// Created by zhi lin on 4/10/22.
//
#define ll long long

struct Req{
    ll key;
    int value;
    int comm;
};

vector<Req> trace[8];

ll fastPow(ll a, ll b, ll p){
    ll res = 1;
    while(b){
        if(b&1)res = res*a%p;
        b=b>>1;
        a = a*a%p;
    }
    return res;
}

ll benchmarkBSGS(ll a, ll b, ll c){
    map<ll,int> mp;
    if(a%c==0){
        return 0;
    }
    ll m = ceil(sqrt(c));
    ll ans;
    ll base = 1;
    // parallel
    for(int i=0;i<m;i++){
        ll newKey = base*b%c;
        mp[newKey] = i+1;
        Req r;
        r.comm = 0;
        r.key = newKey;
        r.value = i+1;
        trace[i%8].push_back(r);
        base = base * a % c;
    }
    base = 1;
    ll stride = fastPow(a,m,c);
    // barrier
    // parallel
    for(int i=0;i<m;i++){
        ll findKey = base;
        if(mp[findKey]){
            //return i*m-(mp[findKey]-1);
        }
        Req r;
        r.comm = 1;
        r.key = findKey;
        r.value = 0;
        trace[i%8].push_back(r);
        base = base*stride%c;
    }
    return -1;
}

void generateBSGSTrace(){
    benchmarkBSGS(65537,5555777797,5555777813);
    for(int i=0;i<8;i++){
        std::ofstream output;
        char *filename = (char *)malloc(8);
        sprintf(filename,"%d.txt",i);
        output.open(filename, ios::out);
        output<<trace[i].size()<<endl;
        for(int j=0;j<trace[i].size();j++){
            Req r = trace[i][j];
            if(r.comm==0){
                output<<"PUT ";
            }else{
                output<<"GET ";
            }
            output<<r.key<<" ";
            output<<r.value<<endl;
        }
        output.close();
    }
}