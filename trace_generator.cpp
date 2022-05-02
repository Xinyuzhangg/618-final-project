#include<iostream>
#include<time.h>
#include"trace_generator.hpp"
#include<fstream>
#include<cstring>
/*
 * trace format:<PUT key value>
 * */
#define ModePut "PUT"
// get
#define ModeGet "GET"
// putget
#define ModePutGet "PUTGET"
#define ll long long
#define mod 1000003
using namespace std;


ll GenerateRandomLL(){
    ll numLeft = rand();
    ll numRight = rand();
    ll num = ((numLeft<<32)+numRight);
    return num;
}

/*
 * p: max key
 * */
void GenerateTrace(char *filename, int n, ll p, char* mode){
    std::ofstream output;
    cout<<"ofstream"<<endl;
    output.open(filename, ios::out);
    output<<n<<endl;
    time_t t;
    srand((unsigned) time(&t));
    ll *keyList = new ll[n];
    int ptr =0;
    for(int i=0;i<n;i++){
        char* traceItem = (char*)malloc(50);
        if(strcmp(mode,ModePut)==0){
            ll key = GenerateRandomLL();
            int value = rand();
            sprintf(traceItem,"PUT %lld %d",key,value);
        }else if(strcmp(mode,ModeGet)==0){
            ll key = GenerateRandomLL();
            sprintf(traceItem,"GET %lld %d",key,0);
        }else{
            if(rand()%n<i){
                // get
                // ll key = GenerateRandomLL();
                int keyId = rand()%ptr;
                ll key = keyList[keyId];
                sprintf(traceItem,"GET %lld %d",key,0);
            }else {
                // put
                ll key = GenerateRandomLL();
                int value = rand();
                sprintf(traceItem,"PUT %lld %d",key,value);
                //keyList[i] = key;
                //keyList.push_back(key);
                keyList[ptr++] = key;
            }
        }
        output<<traceItem<<endl;
        free(traceItem);
    }
    delete[] keyList;
    output.close();
}