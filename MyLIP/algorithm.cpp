#include "algorithm.h"

//get the index bit of Number,1 or 0
int GetIndexBit(int index,int Number)
{
	if (Number>>index)
	{
		return 1;
	}
	else
		return 0;
}


int other_gcd(int a,int b,int&x,int &y)
{
	int t,d;
	if(b==0)
	{
		x=1;
		y=0;   //不明处1
		return a;
	}
	d=other_gcd(b,a%b,x,y);
	t=x;
	x=y;
	y=t-(a/b)*y;  //不明处2
	return d;
}

/*
相求 a*x + b*y = 1的解
因为	gcd(a,b) = gcd(b,a%b) == 1
->设	a*x + b*y = 1 （1）
所以下一次递归有：
		b*x' + (a%b)*y' = 1,
	即  b*x' + (a- a/b * b) * y' = 1 
	即  a*y' + b(1-a/b)*y' = 1      （2）

由 （1） 和（2）恒成立得到：
	    x = y'
		y = (1 - a/b) * y'
即，本地递归的x等于下一次递归的y，本次递归的y等于下一次递归的 (1-a/b)*y。

因为gcd(a,b) = gcd(b,a%b)
所以，最后一次递归，就是 a==b，即 a%b==0时，
对于gcd(a,b,x,y)
{
	if(b==0)//此时b为0， b = a_upper % b_upper ,即a_upper == b_upper,此时等式为 a*x + 0*y == 1,可以令y=1，
	{
		y=1
	}
		
}


*/


void ex_euclid( int a,int b,int& g_x,int & g_y )
{
	int old_a = a;
	int old_b = b;
	int tmp = b;
	b = a % b;
	a = tmp;
	if(b == 0)
	{
		g_x = 1;
		g_y = 0;

		//最后一次递归返回时，顺带把出了赋予初始值之外的
		//第一层x y之间的递推也计算出来。
		int tmp = g_x;
		g_x = g_y;
		g_y = tmp - (old_a/old_b)*g_y;
		return;
	}

	ex_euclid(a,b,g_x,g_y);

	//计算第一次
	int old_x = g_x;
	g_x = g_y;
	g_y = old_x - (old_a/old_b)*g_y;
}
int GetR( int n )
{
	int i = 0;
	while (n>0)
	{
		i++;
		n=n>>1;
	}
	return 1<<i;
}
int MonPro( int a,int b,int r,int n,int n$ )
{
	int t = a*b;
	int m = t*n$ % r;
	int u = (t+m*n) / r;
	if (u>= n) 
		return u - n;
	else
		return u;
}
//r 128 n 97 r$ 25 n$ 33

int ModMul( int a,int b,int r,int n )
{
	int r$,n$;
	int rr,nn;
	ex_euclid(r,n,r$,n$);
	other_gcd(r,n,rr,nn);
	int a_ = (a*r) % n;
	int b_ = (b*r) % n;

	int x_ = MonPro(a_,b_,r,n,-n$);//扩展欧几里得算法求出来的n$ 是 r*r-1 + n*n$ =1 的解，蒙哥马利算法要的是r*r-1 - n*n$ =1  的 n$,所以n$要取负
	int x = MonPro(x_,1,r,n,-n$);
	return x;
}

int ModMul2(int a,int b,int r,int n)
{
	int r$,n$;
	ex_euclid(r,n,r$,n$);
	int a_ = (a*r) % n;
	int x = MonPro(a_,b,r,n,n$);
	return x;
}
/*

*/
int ModExp(int M,int e,int r,int n)//n is odd
{
	int r$,n$;
	ex_euclid(r,n,r$,n$);//16*-4  + 13*5 = 1   ->>>   16*9 - 13* 11 =  144 - 143 = 1
	
	//因为扩展欧几里得算法求出来的不一定满足 r*r$ - n*n$ = 1,并且
	while (r$ < 0)
	{
		r$ = r$ + n;
	}
	n$ = (r*r$ - 1) / n;
	//计算第一次
	int M_ = M*r % n;
	int x_ = 1*r % n;
	int nn = n;//13 == 1101
	int len = 0;

	while (nn)
	{
		nn = nn >> 1;
			len++;
	}
	while (len--)
	{
		x_ = MonPro(x_,x_,r,n,n$);
		int bit = n>>len;
		if (bit & 1)
		{
			x_ = MonPro(M_,x_,r,n,n$);
		}
		
	}
	x_ = MonPro(x_,1,r,n,n$);
	return x_;
}
int modular_exp_test(int a,int b,int m=1)
{
	int res = 1;
	int multiplier = a;
	while(b!=0)
	{
		if(b & 1)
		{
			res = (res * multiplier)%m;
		}
		multiplier = (multiplier * multiplier)%m;
		b >>=1;
	}
	return res;
}
/*
原理是：
gcd(a,b) == gcd(b,a%b) == 
*/
int test_gcd(int a,int b)
{
	while (a)
	{
		int t = a;
		a = a % b;
		b = t;
	}
	return b;
}
int main()
{
	int gcd_v = test_gcd(24,24);
	int a = 7,b=10,n=13; //a*b mod n = 14
	int r = GetR(n);
	int rr = r;
	int len = 0;
	

	//int v = ModMul(a,b,r,n);
	//int vv = ModMul2(a,b,r,n);
	int vvv = ModExp(a,b,r,n);
	int vvvv = modular_exp_test(a,b,n);
	return 0;
}