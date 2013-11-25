#ifndef __ALGORITHM__H
#define __ALGORITHM__H
#include "bigint.h"

  
//euclid算出来的是 a*x + b*y = gcd(a,b)的解
//对于a 和 b互质的情况，就是a*x + b*y = gcd(a,b) === 1的解
void ex_euclid(int a,int b,int& g_x,int & g_y);


//a 和 b都是 residue
//n$ 是 r*r-1  - n*n$ = 1 的解，用扩展欧几里得算法求得
int MonPro(int a,int b,int r,int n,int n$);



BigInt ModExp(const BigInt& M,const BigInt& e,const BigInt& r,const BigInt& n);
//用MonPro计算模乘.a和b都是原始的数据，不是residue
int ModMul(int a,int b,int r,int n);

int GetR(int n);
#endif
