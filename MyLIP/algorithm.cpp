#include "algorithm.h"

//�����ɸ������˻�����ModExp������ж�����ģ��ʱÿһ������ʹ��
BigInt MonPro(const BigInt&a,const BigInt& b,const BigInt& r,const BigInt&n,const BigInt& n$)
{
	BigInt t = a*b;
	/*BigInt m = t*n$ % r; //�ɸ������㷨�ľ��������һ�к���һ�С�����Ҫ����ļ���Ϳ��Խ��У���Ϊr��10...0��ʽ�� %r �� /r �ļ��㳬�����ף�
	BigInt u = (t+m*n)/r;  //�����Ƕ������д��뾭�������2���Ƶ�ȡģ�ͳ���
	*/
	BigInt m,u;
	BigInt __;
	BigDiv2N2(t*n$,r,__,m);
	BigDiv2N2((t+m*n),r,u,__);
	
	if (u>=n)
		return u-n;
	else
		return u;	
}

BigInt ModExp(const BigInt& M,const BigInt& e,const BigInt& r,const BigInt& n)//n is odd
{
	BigInt r$,n$;
	ExEuclid2(r,n,r$,n$);//16*-4  + 13*5 = 1   ->>>   16*9 - 13* 11 =  144 - 143 = 1

	//��Ϊ��չŷ������㷨������Ĳ�һ������ r*r$ - n*n$ = 1,����
	while (r$ < BigInt("0"))
	{
		r$ = r$ + n;
	}
	n$ = (r*r$ - BigInt("1")) / n;
	//�����һ��
	BigInt M_ = M*r % n;
	BigInt x_ = BigInt("1")*r % n;
	BigInt ee = e;//13 == 1101  1010 == 10
	int len = 0;

	while (ee>BigInt("0"))
	{
		ee = ee >> 1;
		len++;
	}

	while (--len>=0)
	{
		x_ = MonPro(x_,x_,r,n,n$);
		BigInt tmp = e>>len;
		int bit = tmp.m_value.GetRadixBits(0);
		if (bit & 1)//e�� e>>len�ǲ���1
		{
			x_ = MonPro(M_,x_,r,n,n$);
		}
	}
	x_ = MonPro(x_,BigInt("1"),r,n,n$);
	return x_;
}


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
		y=0;   //������1
		return a;
	}
	d=other_gcd(b,a%b,x,y);
	t=x;
	x=y;
	y=t-(a/b)*y;  //������2
	return d;
}

int modular_exp_test(int a,int b,int m);
// 11001
//test_v�ǲ����õ�ֵ
int rabin_miller(int p,int test_v)
{
	// ż��
	if ( (p&3) % 2 == 0)
	{
		return false;
	}
	int m = p-1;
	int k = 0;
	while(0==(m&1))
	{
		m = m >> 1;
		k++;
	}
	int m_ = p-1;
	while (k>=0)
	{
		int expmod = modular_exp_test(test_v,m_,p);
		if (expmod == p-1)
		{
			return true;
		}
		else if (expmod!=1)
		{
			return false;
		}
		else
		{
			m_ = m_ >> 1;
		}
		k = k -1;
	}
	return true;
}

//��test_v����p����һ��rabinmiller�����жϣ�test_v<p
bool rabin_miller(const BigUInt& p,const BigUInt& test_v)
{
	uint32 v = p.GetRadixBits(0);
	if( (p & 1) == BigUInt("0"))//ż��
	{
		return false;
	}

	BigUInt m = p - BigUInt("1");
	BigUInt m_ = m;
	int k = 0;

	while ( (m&1) == BigUInt("0"))
	{
		if( (m&0xffffffff) == BigUInt("0") )
		{
			int continue_uin32 = 0;
			while ( (m.GetRadixBits(continue_uin32) & 0xffffffff) == BigUInt("0"))
			{
				continue_uin32++;
			}
			m = m >> (32*continue_uin32);
			k = k + 32*continue_uin32;
		}
		else
		{
			m = m >> 1;
			k = k + 1;
		}
	}
	while (k>=0)
	{
		BigUInt expmod = ExpMod(test_v,m_,p);
		if (expmod == p - BigUInt("1"))
		{
			return true;
		}
		else if (expmod == BigUInt("1"))
		{
			m_ = m_ >> 1;
		}
		else
		{
			return false;
		}
		k = k - 1;
	}
	return true;
}

/*
���� a*x + b*y = 1�Ľ�
��Ϊ	gcd(a,b) = gcd(b,a%b) == 1
->��	a*x + b*y = 1 ��1��
������һ�εݹ��У�
		b*x' + (a%b)*y' = 1,
	��  b*x' + (a- a/b * b) * y' = 1 
	��  a*y' + b(1-a/b)*y' = 1      ��2��

�� ��1�� �ͣ�2��������õ���
	    x = y'
		y = (1 - a/b) * y'
�������صݹ��x������һ�εݹ��y�����εݹ��y������һ�εݹ�� (1-a/b)*y��

��Ϊgcd(a,b) = gcd(b,a%b)
���ԣ����һ�εݹ飬���� a==b���� a%b==0ʱ��
����gcd(a,b,x,y)
{
	if(b==0)//��ʱbΪ0�� b = a_upper % b_upper ,��a_upper == b_upper,��ʱ��ʽΪ a*x + 0*y == 1,������y=1��
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

		//���һ�εݹ鷵��ʱ��˳���ѳ��˸����ʼֵ֮���
		//��һ��x y֮��ĵ���Ҳ���������
		int tmp = g_x;
		g_x = g_y;
		g_y = tmp - (old_a/old_b)*g_y;
		return;
	}

	ex_euclid(a,b,g_x,g_y);

	//�����һ��
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

	int x_ = MonPro(a_,b_,r,n,-n$);//��չŷ������㷨�������n$ �� r*r-1 + n*n$ =1 �Ľ⣬�ɸ������㷨Ҫ����r*r-1 - n*n$ =1  �� n$,����n$Ҫȡ��
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
	
	//��Ϊ��չŷ������㷨������Ĳ�һ������ r*r$ - n*n$ = 1,����
	while (r$ < 0)
	{
		r$ = r$ + n;
	}
	n$ = (r*r$ - 1) / n;
	//�����һ��
	int M_ = M*r % n;
	int x_ = 1*r % n;
	int ee = e;//13 == 1101  1010 == 10
	int len = 0;

	while (ee)
	{
		ee = ee >> 1;
			len++;
	}

	while (--len>=0)
	{
		x_ = MonPro(x_,x_,r,n,n$);
		int bit = e>>len;
		if (bit & 1)
		{
			x_ = MonPro(M_,x_,r,n,n$);
		}
	}
	x_ = MonPro(x_,1,r,n,n$);
	return x_;
}
int modular_exp_test(int a,int b,int m)
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
ԭ���ǣ�
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
	BigUInt max_prime = "170141183460469231731687303715884105727";
	//DumpBits(max_prime);

	 
	//for (BigUInt p=BigUInt("3");p<BigUInt("2000");)
	for (BigUInt p=max_prime;p<max_prime /*+ BigUInt("2000")*/;)
	{
		int is_prime = true;
		for(BigUInt j="2";j<BigUInt("5");)
		{
			if( !rabin_miller(p,j))
			{
				is_prime = false;
				break;
			}
			j = j + BigUInt("1");
		}
		if (is_prime)
		{
			DumpBits(p);
		}
		p = p + BigUInt("1");
	}

	BigInt QQ,RR;
	BigDiv2N2(BigInt("7"),BigInt("2"),QQ,RR);
	int gcd_v = test_gcd(24,24);
	int a = 7,b=109999999,n=13; //a*b mod n = 14
	int r = GetR(n);
	int rr = r;
	int len = 0;
	
	int vvv = ModExp(a,b,r,n);
	int vvvv = modular_exp_test(a,b,n);

	BigInt A = "817263847623465716523745273645781652376548176523874658162537846518726354";
	BigInt B = "76981762398764918762398576298376495876198237645987263478562398475";
	BigInt C = "87619823764598726349865981763948765928364985762198376498576234986598768761987";
	
	//�ɸ������㷨�õ���R���Ǳ�ģCҪ�����С��2^n��ֵ��
	BigInt R = BigInt("1");
	 
	R=R<<( C.m_value.GetNonZeroBitIdx()+1 );
	 
	BigInt Result = ModExp(A,B,R,C);
	DumpBits(Result.m_value,"���:");
	return 0;
}