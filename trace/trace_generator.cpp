#include<iostream>
#include<time.h>
/*
 * trace format:<PUT key value>
 * */
#define ModePut "PUT"
// get
#define ModeGet "GET"
// putget
#define ModePutGet "PUTGET"
#define ll long long

ll GenerateRandomLL(){
    ll numLeft = rand();
    ll numRight = rand();
    ll num = ((numLeft<<32)+numRight)%p;
    return num;
}

/*
 * p: max key
 * */
void GenerateTrace(char *filename, int n, ll p, char* mode){
    std::ofstream output;
    output.open(filename, ios::out);
    time_t t;
    srand((unsigned) time(&t));
    for(int i=0;i<n;i++){
        char* traceItem = (char*)malloc(50);
        if(strcmp(mode,ModePut)==0){
            ll key = GenerateRandomLL();
            ll value = GenerateRandomLL();
            sprintf(traceItem,"<PUT %lld %lld>",key,value);
        }else if(strcmp(mode,ModeGet)==0){
            ll key = GenerateRandomLL();
            sprintf(traceItem,"<GET %lld>",key);
        }else{
            if(rand()%p<i){
                // get
                ll key = GenerateRandomLL();
                sprintf(traceItem,"<GET %lld>",key);
            }else {
                // put
                ll key = GenerateRandomLL();
                ll value = GenerateRandomLL();
                printf(traceItem,"<PUT %lld %lld>",key,value);
            }
        }
        output<<traceItem<<endl;
        free(traceItem);
    }
    output.close();
}