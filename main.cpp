#include <iostream>
#include <bits/stdc++.h>
#include <chrono>

using namespace std;

void init();
int get_msb(int num);
int get_rand(int range);
void print_b(int num);
void print(int num);

int main(){

    init();

    u_int64_t num = 0; //get_rand(32);
    int msb = get_msb(num);
    
    print(num);
    print(msb);
    print_b(num);

    return 0;
}

void init(){
    srand(time(NULL));
}

int get_msb(int num){
    if(num < 1) return 0;
    return 1 << (31-__builtin_clz(num));
}

int get_rand(int range){
    return rand()%range;
}

void print_b(int num){
    if (num == 0) {
        cout << 0 << endl;
        return;
    }
    string b;
    while (num > 0){
        b += (num&1u) ? '1' : '0';
        num >>= 1;
    }
    reverse(b.begin(), b.end());
    cout << b << endl;
}

void print(int num){
    cout << num << endl;
}