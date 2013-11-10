#ifndef __BIG_U_INT_H__
#define __BIG_U_INT_H__
#include <vector>
#include <string>
#include <iostream>
#include <assert.h>
#include <algorithm>

typedef unsigned int uint32;
typedef int int32;
typedef unsigned long long uint64;
typedef long long int64;

struct BigUInt;
//前向声明
int FromString(BigUInt& N,const std::string& numbers);
void DumpBits(const BigUInt& N,const char * fmt="",...);
int32 Compare(const BigUInt & X,const BigUInt & Y);
int operator<(const BigUInt& v1,const BigUInt& v2);
int operator==(const BigUInt& v1,const BigUInt& v2);
int operator>(const BigUInt& v1,const BigUInt& v2);
int operator<= (const BigUInt& v1,const BigUInt& v2);
int operator>= (const BigUInt& v1,const BigUInt& v2);
int operator!=(const BigUInt& v1,const BigUInt& v2);
//算数运算
BigUInt operator+(const BigUInt& X,const BigUInt& Y);
BigUInt operator-(const BigUInt& N1,const BigUInt& N2);
BigUInt operator*(const BigUInt& N1,const BigUInt& N2);
BigUInt operator/ (const BigUInt& X,const BigUInt& Y);
BigUInt operator%(const BigUInt& X,const BigUInt& M);
BigUInt operator>>(const BigUInt& X,int bits);
BigUInt operator<<(const BigUInt& X,int bits);
BigUInt operator&(const BigUInt&X,uint32 mask);

//大数除法：
//返回两个大数除法的商，同时Q置为商，R为余数
BigUInt BigDiv(const BigUInt& X,const BigUInt& Y,BigUInt &Q,BigUInt&R);
BigUInt Fast_BigDiv(const BigUInt& X,const BigUInt& Y,BigUInt&Q,BigUInt&R);
//除以2**N形式的数。主要用于蒙哥马利算法中。
BigUInt BigDiv2N(const BigUInt&X,const BigUInt& Y,BigUInt&Q,BigUInt&R);

//欧几里德算法，求X和Y的最大公约数
BigUInt GCD(const BigUInt& X,const BigUInt& Y);

//扩展欧几里德算法，求模反元素
void ExEuclid( BigUInt a,BigUInt b,BigUInt& x,BigUInt&y );

//求模幂：a^b % m
BigUInt ExpMod(const BigUInt& a,const BigUInt& b,const BigUInt& m);

//Montgomery 算法计算模幂: a^b % m
BigUInt MExpMod(const BigUInt&a,const BigUInt&b,const BigUInt& m);


//求幂
BigUInt Exp(const BigUInt& a,const BigUInt& b);

//将大数表示为n^32 进制（即0x1 0000 0000）的数组

// 0xFFFFFFFF == 4294967295
// 0xFFFFFFFF **2 == FFFFFFFE00000001 ==  4294967295**2 == 18446744065119617025

// 0xFFFFFFFF FFFFFFFF == 18446744073709551615 ~= 4294967295.9999999998835846781731**2

struct BigUInt
{
	BigUInt()
	{
		m_bits.resize(1);
	}
	BigUInt(const uint32& ui32)
	{
		m_bits.push_back(ui32);
	}
	BigUInt(const uint64& ui64)
	{
		uint32 carry = ui64 / RADIX;
		uint32 v = ui64 % RADIX;
		m_bits.push_back(v);
		m_bits.push_back(carry);
	}
	BigUInt(const char * intstr)
	{
		FromString(*this,intstr);
	}
	
	BigUInt( const BigUInt& other)
	{
		m_bits = other.m_bits;
	}

	BigUInt& operator=(const BigUInt& other)
	{
		this->m_bits = other.m_bits;
		return *this;
		FormRealBits();
	}
	std::string ToString() const;

	//uint32数组总长度，包括最高位的0的数目
	int Length()const;

	//uint32数组总长度，不包括最高位的0
	int ValidLength() const;

	//去掉从最高位的连续为0的元素
	void TrimHiZeros();

	//清除
	void Reset();
	
	//获得最高为1的比特的索引 N N-1 ... 3 2 1 0
	int32 GetNonZeroBitIdx() const; //get_non_zero_bit_idx;

	//获取高位第一个不为零的uint32块的索引
	int32 GetNonZeroIdx()const;

	//获取从idx_hi,到idx_lo之间的数据构成的BigInt，用于除法的试除
	BigUInt GetBitRangBigInt(int bit_idx_hi,int bit_idx_lo) const;

	//获取bit_idx索引的bit值，false表示0，true表示为1
	bool GetBitByIdx(int bit_idx) const;
	
	//加上一个小整数（for FromString）
	void Add(const uint32& ui32) ;

	//乘以一个小整数 （for FromString）
	void Mul(const uint32& ui32) ;

	//对小整数取模（暂时没用）
	uint32 Mod(const uint32& ui32);

	//除以10的商和余数(余数一定是uint32可以表示的，因为余数一定是0~9）
	BigUInt DivBy10(BigUInt& Q,uint32 &R);

	//在pos位加上v，会进位的
	void AddRadixBits(uint32 v,uint32 pos);
	//从pos位减去v，会借位的（一定要保证够减）
	void MinusRadixBits(uint32 v,uint32 pos);

	//获取pos位的uint32的数	
	inline uint32 GetRadixBits(uint32 pos) const
	{
		if(pos<0 || pos >=m_bits.size() )
			return 0;
		return m_bits[pos];
	}

	//设置pos位的值为v，没有该位置的话，直接把数组扩大到pos位再设置
	inline void SetRadixBits(uint32 v,uint32 pos)
	{
		if (pos>=m_bits.size())
		{
			m_bits.resize(pos+1,0);
		}
		m_bits[pos] = v;
	}
	
	
	//显示函数
	friend int FromString(BigUInt& N,const std::string& numbers);
	friend void DumpBits(const BigUInt& N,const char * fmt,...);

	//比较大小 
	friend int32 Compare(const BigUInt & X,const BigUInt & Y);
	friend int operator<(const BigUInt& v1,const BigUInt& v2);
	friend int operator==(const BigUInt& v1,const BigUInt& v2);
	friend int operator>(const BigUInt& v1,const BigUInt& v2);
	friend int operator<= (const BigUInt& v1,const BigUInt& v2);
	friend int operator>= (const BigUInt& v1,const BigUInt& v2);
	friend int operator!=(const BigUInt& v1,const BigUInt& v2);
	//算数运算
	friend BigUInt operator+(const BigUInt& X,const BigUInt& Y);
	friend BigUInt operator-(const BigUInt& N1,const BigUInt& N2);
	friend BigUInt operator*(const BigUInt& N1,const BigUInt& N2);
	friend BigUInt operator/ (const BigUInt& X,const BigUInt& Y);
	friend BigUInt operator%(const BigUInt& X,const BigUInt& M);

	friend BigUInt operator>>(const BigUInt& X,int bits);
	friend BigUInt operator<<(const BigUInt& X,int bits);
	friend BigUInt operator&(const BigUInt&X,uint32 mask);
	
	friend BigUInt BigDiv(const BigUInt& X,const BigUInt& Y,BigUInt &Q,BigUInt&R);
	friend BigUInt Fast_BigDiv(const BigUInt& X,const BigUInt& Y,BigUInt&Q,BigUInt&R);
	friend BigUInt BigDiv2N(const BigUInt&X,const BigUInt& Y,BigUInt&Q,BigUInt&R);
	
	friend BigUInt GCD(const BigUInt& X,const BigUInt& Y);

	friend void ExEuclid(BigUInt a, BigUInt b,BigUInt& x,BigUInt&y); 
	
private:

	struct len
	{
		len(){ _01_=0;}
		int operator()(char c)
		{
			if (c == '0' || c == '1')
			{
				_01_++;
				return _01_;
			}
		}
		int _01_;
	};

public:
	void FormRealBits()
	{
		m_real_bits.clear();
		for (int i= 0;i<=Length()*32-1;i++)
		{
			m_real_bits.insert(m_real_bits.begin(),'0'+GetBitByIdx(i));
			 
			len L = std::for_each(m_real_bits.begin(),m_real_bits.end(),len());
			int l = L._01_;
			if(l %32 == 0) // (m_real_bits.size() % 32 == 0)
			{
				m_real_bits.insert(m_real_bits.begin(),',' );
			}
		}
		
	}
	typedef std::vector<uint32> BigIntBitType;
	std::string   m_real_bits;
	//位置:  4 3 2 1 0
	//数字：| e...e | d...d | c...c | b...b | a...a |
	BigIntBitType m_bits;
	

	const static uint64 RADIX =		0x0000000100000000;
	const static uint64 CARDINAL =	0x00000000ffffffff;
};


#endif