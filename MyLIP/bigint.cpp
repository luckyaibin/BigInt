#include "bigint.h"



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