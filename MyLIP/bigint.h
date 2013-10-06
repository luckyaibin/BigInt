#ifndef __BIG_INT_H__
#define __BIG_INT_H__
#include "biguint.h"

struct BigInt;


BigInt operator+(const BigInt& N1,const BigInt& N2);
BigInt operator-(const BigInt&N1,const BigInt& N2);
BigInt operator*(const BigInt& N1,const BigInt& N2);
BigInt operator/ (const BigInt& X,const BigInt& Y);
BigInt operator% (const BigInt&X,const BigInt& Y);
BigInt operator>>(const BigInt& X,int bits);
BigInt operator<<(const BigInt& X,int bits);

int FromString2(BigInt& N,const std::string& numbers);
BigInt Fast_BigDiv2(const BigInt& X,const BigInt& Y,BigInt&Q,BigInt&R);

struct BigInt
{
	BigInt():m_sign(0)
	{
		
	}
	BigInt(const std::string& numbers)
	{
		FromString2(*this,numbers);
	}
	int		m_sign;//符号，0为正，非0为负
	BigUInt m_value;
	
	friend BigInt operator+(const BigInt& N1,const BigInt& N2);
	friend BigInt operator-(const BigInt&N1,const BigInt& N2);
	friend BigInt operator*(const BigInt& N1,const BigInt& N2);
	friend BigInt operator/ (const BigInt& X,const BigInt& Y);
	friend BigInt operator% (const BigInt&X,const BigInt& Y);
	friend BigInt operator>>(const BigInt& X,int bits);
	friend BigInt operator<<(const BigInt& X,int bits);

	friend int FromString2(BigInt& N,const std::string& numbers);
	friend BigInt Fast_BigDiv2(const BigInt& X,const BigInt& Y,BigInt&Q,BigInt&R);
	
};

#endif



