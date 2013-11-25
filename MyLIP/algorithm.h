#ifndef __ALGORITHM__H
#define __ALGORITHM__H
#include "bigint.h"

  
//euclid��������� a*x + b*y = gcd(a,b)�Ľ�
//����a �� b���ʵ����������a*x + b*y = gcd(a,b) === 1�Ľ�
void ex_euclid(int a,int b,int& g_x,int & g_y);


//a �� b���� residue
//n$ �� r*r-1  - n*n$ = 1 �Ľ⣬����չŷ������㷨���
int MonPro(int a,int b,int r,int n,int n$);



BigInt ModExp(const BigInt& M,const BigInt& e,const BigInt& r,const BigInt& n);
//��MonPro����ģ��.a��b����ԭʼ�����ݣ�����residue
int ModMul(int a,int b,int r,int n);

int GetR(int n);
#endif
