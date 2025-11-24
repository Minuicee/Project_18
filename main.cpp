#include <iostream>
#include <bits/stdc++.h>
#include <chrono>

using namespace std;

void init();
int get_msb(int num);
int get_lsb(int num);
int get_row(int num, int row_size);
int get_rand(int range);
int get_rand_print(int range);
void print_b(int num);
void print(int num);

int main(){

    init();


    //// main code ////////////////////////////////////////////////////////////////////
    //u_int32_t num = get_rand_print(32);
    print(get_row(64, 3));

    ////////////////////////////////////////////////////////////////////////////////

    cout << endl;
    return 0;
}

void init(){
    srand(time(NULL));
}

int get_msb(int num){
    if(num < 1) return 0;
    return 1 << (31-__builtin_clz(num));
}

int get_lsb(int num){
    if(num < 1) return 0;
    return 1 << __builtin_ctz(num);
}

int get_rand(int range){
    return rand()%range;
}

int get_row(int num, int row_size){
    //only 1 bit of num can be 1
    if(num < 1) return 0;
    return __builtin_ctz(num)/row_size;
}

int get_rand_print(int range){
    auto num = rand()%range;
    cout << "num:" << endl;
    print(num);
    print_b(num);
    cout << endl;
    return num;
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