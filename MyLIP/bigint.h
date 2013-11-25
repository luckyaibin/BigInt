#ifndef __BIG_INT_H__
#define __BIG_INT_H__
#include "biguint.h"

struct BigInt;


int operator<(const BigInt& v1,const BigInt& v2);
int operator==(const BigInt& v1,const BigInt& v2);
int operator>(const BigInt& v1,const BigInt& v2);
int operator<=(const BigInt& v1,const BigInt& v2);
int operator>=(const BigInt& v1,const BigInt& v2);
int operator!=(const BigInt& v1,const BigInt& v2);

BigInt operator+(const BigInt& N1,const BigInt& N2);
BigInt operator-(const BigInt&N1,const BigInt& N2);
BigInt operator*(const BigInt& N1,const BigInt& N2);
BigInt operator/ (const BigInt& X,const BigInt& Y);
BigInt operator% (const BigInt&X,const BigInt& Y);
BigInt operator>>(const BigInt& X,int bits);
BigInt operator<<(const BigInt& X,int bits);

int FromString2(BigInt& N,const std::string& numbers);
BigInt Fast_BigDiv2(const BigInt& X,const BigInt& Y,BigInt&Q,BigInt&R);
BigInt BigDiv2N2(const BigInt&X,const BigInt& Y,BigInt&Q,BigInt&R);//�����Ƴ������Ż���Y����Ϊ2������ʽ

void ExEuclid2( BigInt a, BigInt b,BigInt& x,BigInt&y );
BigInt GCD2(const BigInt& X,const BigInt& Y);
struct BigInt
{
	BigInt():m_sign(0)
	{
		
	}
	BigInt(const char* numbers)
	{
		FromString2(*this,numbers);
	}
	BigInt(const BigUInt& bui)
	{
		m_sign = 0;
		m_value = bui;
	}

	//����
	BigInt operator-() const
	{
		BigInt val = *this;
		val.m_sign = !val.m_sign;
		return val;
	}
	
	std::string ToString() const;

	int		m_sign;//���ţ�0Ϊ������0Ϊ��
	BigUInt m_value;
	
	friend int operator<(const BigInt& v1,const BigInt& v2);
	friend int operator==(const BigInt& v1,const BigInt& v2);
	friend int operator>(const BigInt& v1,const BigInt& v2);
	friend int operator<=(const BigInt& v1,const BigInt& v2);
	friend int operator>=(const BigInt& v1,const BigInt& v2);
	friend int operator!=(const BigInt& v1,const BigInt& v2);

	friend BigInt operator+(const BigInt& N1,const BigInt& N2);
	friend BigInt operator-(const BigInt&N1,const BigInt& N2);
	friend BigInt operator*(const BigInt& N1,const BigInt& N2);
	friend BigInt operator/ (const BigInt& X,const BigInt& Y);
	friend BigInt operator% (const BigInt&X,const BigInt& Y);
	friend BigInt operator>>(const BigInt& X,int bits);
	friend BigInt operator<<(const BigInt& X,int bits);

	friend int FromString2(BigInt& N,const std::string& numbers);
	friend BigInt Fast_BigDiv2(const BigInt& X,const BigInt& Y,BigInt&Q,BigInt&R);

	friend BigInt BigDiv2N2(const BigInt&X,const BigInt& Y,BigInt&Q,BigInt&R);
	friend void ExEuclid2( BigInt a, BigInt b,BigInt& x,BigInt&y );

	BigInt GCD2(const BigInt& X,const BigInt& Y);
};

#endif



