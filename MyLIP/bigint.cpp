#include "bigint.h"
#include<algorithm>


std::string BigInt::ToString() const
{
	std::string numbers;

	BigUInt Q = this->m_value;
	BigUInt  R;
	while(Q>BigUInt("0"))
	{
		Fast_BigDiv(Q,BigUInt("10"),Q,R);
		//Q.DivBy10(Q,R);
		numbers.push_back(R.GetRadixBits(0)+'0');
	}
	if (this->m_sign==1)
	{
		numbers.push_back('-');
	}
	std::reverse(numbers.begin(),numbers.end());

	return numbers;
}

int operator<(const BigInt& v1,const BigInt& v2)
{
	//都是0在这里判断
	if (v1==v2)
	{
		return 0;
	}
	else if (v1.m_sign > v2.m_sign)
	{
		return 1;
	}
	else if (v1.m_sign < v2.m_sign)
	{
		return 0;
	}
	else if (v1.m_sign == v2.m_sign && v1.m_sign == 0)//都是正数
	{
		return v1.m_value < v2.m_value;
	}
	else if (v1.m_sign == v2.m_sign && v1.m_sign == 1)//都是负数
	{
		return v1.m_value > v2.m_value;
	}
	else
	{
		assert(0&&"never here!!!");
	}
}
int operator==(const BigInt& v1,const BigInt& v2)
{
	//绝对值都为0
	if (v1.m_value == BigUInt("0") && v2.m_value == BigUInt("0"))
	{
		return 1;
	}
	else if(v1.m_sign == v2.m_sign && v1.m_value == v2.m_value)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
int operator>(const BigInt& v1,const BigInt& v2)
{
	return (!(v1==v2)) && (!(v1<v2));
}
int operator<=(const BigInt& v1,const BigInt& v2)
{
	return !(v1>v2);
}
int operator>=(const BigInt& v1,const BigInt& v2)
{
	return !(v1<v2);
}
int operator!=(const BigInt& v1,const BigInt& v2)
{
	return !(v1==v2);
}

BigInt operator+(const BigInt& N1,const BigInt& N2)
{
	BigInt val;
	if (N1.m_sign == N2.m_sign)
	{
		val.m_sign = N1.m_sign;
		val.m_value = N1.m_value + N2.m_value;
	}
	else if (N1.m_value >= N2.m_value)
	{
		val.m_sign = N1.m_sign;
		val.m_value = N1.m_value - N2.m_value;
	}
	else
	{
		val.m_sign = N2.m_sign;
		val.m_value = N2.m_value - N1.m_value;
	}
	return val;
}

/*
 -3 - (-5)  = 2
  3 - ( 5)  = -2
  3 - (-5)  = 8
 -3 - ( 5)  = -8

 规律:
 符号不同，结果符号等于被减数的符号

*/
BigInt operator-(const BigInt&N1,const BigInt& N2)
{
	
	if (N1.m_sign != N2.m_sign)
	{
		BigInt val;
		val.m_sign = N1.m_sign;
		val.m_value = N1.m_value + N2.m_value;
		return val;
	}
	else
	{
		BigInt N2_ = N2;
		N2_.m_sign = !N2_.m_sign;

		return N1 + N2_;
	}
}
BigInt operator*(const BigInt& N1,const BigInt& N2)
{
	/*
	sign 0 0 0 正*正 正
		 0 1 1 正*负 负
		 1 0 1 负*正 负
		 1 1 0 负*负 正
	*/
	BigInt val;
	val.m_sign = N1.m_sign ^ N2.m_sign; //异或
	val.m_value = N1.m_value * N2.m_value;
	return val;
}
BigInt operator/ (const BigInt& N1,const BigInt& N2)
{
	BigInt Q,R;
	Fast_BigDiv2(N1,N2,Q,R);
	return Q;
}
BigInt operator% (const BigInt&N1,const BigInt& N2)
{
	BigInt Q,R;
	Fast_BigDiv2(N1,N2,Q,R);
	return R;
}


BigInt operator>>(const BigInt& X,int bits)
{
	BigInt val;
	val.m_sign = X.m_sign;
	val.m_value = X.m_value>>bits;
	return val;
}
BigInt operator<<(const BigInt& X,int bits)
{
	BigInt val;
	val.m_sign = X.m_sign;
	val.m_value = X.m_value<<bits;
	return val;
}



int FromString2(BigInt& N,const std::string& numbers)
{
	int sign = FromString(N.m_value,numbers);
	N.m_sign = sign;
	return sign;
}

/*
 Q = 向下取整(X/Y);
 X = Y * 向下取整(X/Y) + R; ==>> R = X - Y * 向下取整(X/Y)，由这个式子推导出各个符号：

 符号:
 X Y Q R
 0 0 0 0
 0 1 1 0
 1 0 1 1
 1 1 0 1 

 得出
 Q的符号 =  X.m_sign ^ Y.m_sign
 R的符号 =  X.m_sign; 
*/

BigInt Fast_BigDiv2(const BigInt& X,const BigInt& Y,BigInt&Q,BigInt&R)
{
	Fast_BigDiv(X.m_value,Y.m_value,Q.m_value,R.m_value);
	Q.m_sign = X.m_sign ^ Y.m_sign;
	R.m_sign = X.m_sign;

	return Q;
}


BigInt BigDiv2N2(const BigInt&X,const BigInt& Y,BigInt&Q,BigInt&R)
{
	BigDiv2N(X.m_value,Y.m_value,Q.m_value,R.m_value);
	Q.m_sign = X.m_sign ^ Y.m_sign;
	R.m_sign = X.m_sign;
	return Q;
}


void ExEuclid2( BigInt a, BigInt b,BigInt& x,BigInt&y )
{
	BigInt old_a = a;
	BigInt old_b = b;

	//r 为 a % b 的模，也是a / b 的余数
	//q 为 a / b 的商
	//这里一次性都求出来
	BigInt r;
	BigInt q;
	BigInt tmp = b;

	if(a.m_value.Length() == 4 && a.m_value.ValidLength() == 3)
	{
		printf("stop.");
	}
	Fast_BigDiv2(a,b,q,r);

	b = r;
	a = tmp;

	if (b == BigInt("0"))
	{
		x = BigInt("1");
		y = BigInt("0");

		BigInt tmp = x;
		x = y;
		y = tmp - q*y;
		return;
	}

	ExEuclid2(a,b,x,y);

	BigInt old_x = x;
	x = y;
	y = old_x - q*x;
}


//欧几里德算法，求X和Y的最大公约数
BigInt GCD2(const BigInt& X,const BigInt& Y)
{
	BigInt Zero("0");
	BigInt a;
	BigInt b;
	BigInt r;
	if (X>Y)
	{
		a = X;
		b = Y;
	}
	else if (X<Y)
	{
		a = Y;
		b = X;
	}
	else //相等，直接返回
	{
		return X;
	}

	while (!( b == Zero) ) //a=41606343 b=40144455
	{
		BigInt tmp = b;
		b =  a%b;
		a = tmp;
	}
	return a;		
}