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
	//����0�������ж�
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
	else if (v1.m_sign == v2.m_sign && v1.m_sign == 0)//��������
	{
		return v1.m_value < v2.m_value;
	}
	else if (v1.m_sign == v2.m_sign && v1.m_sign == 1)//���Ǹ���
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
	//����ֵ��Ϊ0
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

 ����:
 ���Ų�ͬ��������ŵ��ڱ������ķ���

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
	sign 0 0 0 ��*�� ��
		 0 1 1 ��*�� ��
		 1 0 1 ��*�� ��
		 1 1 0 ��*�� ��
	*/
	BigInt val;
	val.m_sign = N1.m_sign ^ N2.m_sign; //���
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
 Q = ����ȡ��(X/Y);
 X = Y * ����ȡ��(X/Y) + R; ==>> R = X - Y * ����ȡ��(X/Y)�������ʽ���Ƶ����������ţ�

 ����:
 X Y Q R
 0 0 0 0
 0 1 1 0
 1 0 1 1
 1 1 0 1 

 �ó�
 Q�ķ��� =  X.m_sign ^ Y.m_sign
 R�ķ��� =  X.m_sign; 
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

	//r Ϊ a % b ��ģ��Ҳ��a / b ������
	//q Ϊ a / b ����
	//����һ���Զ������
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


//ŷ������㷨����X��Y�����Լ��
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
	else //��ȣ�ֱ�ӷ���
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