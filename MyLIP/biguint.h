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
//ǰ������
int FromString(BigUInt& N,const std::string& numbers);
void DumpBits(const BigUInt& N,const char * fmt="",...);
int32 Compare(const BigUInt & X,const BigUInt & Y);
int operator<(const BigUInt& v1,const BigUInt& v2);
int operator==(const BigUInt& v1,const BigUInt& v2);
int operator>(const BigUInt& v1,const BigUInt& v2);
int operator<= (const BigUInt& v1,const BigUInt& v2);
int operator>= (const BigUInt& v1,const BigUInt& v2);
int operator!=(const BigUInt& v1,const BigUInt& v2);
//��������
BigUInt operator+(const BigUInt& X,const BigUInt& Y);
BigUInt operator-(const BigUInt& N1,const BigUInt& N2);
BigUInt operator*(const BigUInt& N1,const BigUInt& N2);
BigUInt operator/ (const BigUInt& X,const BigUInt& Y);
BigUInt operator%(const BigUInt& X,const BigUInt& M);
BigUInt operator>>(const BigUInt& X,int bits);
BigUInt operator<<(const BigUInt& X,int bits);
BigUInt operator&(const BigUInt&X,uint32 mask);

//����������
//�������������������̣�ͬʱQ��Ϊ�̣�RΪ����
BigUInt BigDiv(const BigUInt& X,const BigUInt& Y,BigUInt &Q,BigUInt&R);
BigUInt Fast_BigDiv(const BigUInt& X,const BigUInt& Y,BigUInt&Q,BigUInt&R);
//����2**N��ʽ��������Ҫ�����ɸ������㷨�С�
BigUInt BigDiv2N(const BigUInt&X,const BigUInt& Y,BigUInt&Q,BigUInt&R);

//ŷ������㷨����X��Y�����Լ��
BigUInt GCD(const BigUInt& X,const BigUInt& Y);

//��չŷ������㷨����ģ��Ԫ��
void ExEuclid( BigUInt a,BigUInt b,BigUInt& x,BigUInt&y );

//��ģ�ݣ�a^b % m
BigUInt ExpMod(const BigUInt& a,const BigUInt& b,const BigUInt& m);

//Montgomery �㷨����ģ��: a^b % m
BigUInt MExpMod(const BigUInt&a,const BigUInt&b,const BigUInt& m);


//����
BigUInt Exp(const BigUInt& a,const BigUInt& b);

//��������ʾΪn^32 ���ƣ���0x1 0000 0000��������

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

	//uint32�����ܳ��ȣ��������λ��0����Ŀ
	int Length()const;

	//uint32�����ܳ��ȣ����������λ��0
	int ValidLength() const;

	//ȥ�������λ������Ϊ0��Ԫ��
	void TrimHiZeros();

	//���
	void Reset();
	
	//������Ϊ1�ı��ص����� N N-1 ... 3 2 1 0
	int32 GetNonZeroBitIdx() const; //get_non_zero_bit_idx;

	//��ȡ��λ��һ����Ϊ���uint32�������
	int32 GetNonZeroIdx()const;

	//��ȡ��idx_hi,��idx_lo֮������ݹ��ɵ�BigInt�����ڳ������Գ�
	BigUInt GetBitRangBigInt(int bit_idx_hi,int bit_idx_lo) const;

	//��ȡbit_idx������bitֵ��false��ʾ0��true��ʾΪ1
	bool GetBitByIdx(int bit_idx) const;
	
	//����һ��С������for FromString��
	void Add(const uint32& ui32) ;

	//����һ��С���� ��for FromString��
	void Mul(const uint32& ui32) ;

	//��С����ȡģ����ʱû�ã�
	uint32 Mod(const uint32& ui32);

	//����10���̺�����(����һ����uint32���Ա�ʾ�ģ���Ϊ����һ����0~9��
	BigUInt DivBy10(BigUInt& Q,uint32 &R);

	//��posλ����v�����λ��
	void AddRadixBits(uint32 v,uint32 pos);
	//��posλ��ȥv�����λ�ģ�һ��Ҫ��֤������
	void MinusRadixBits(uint32 v,uint32 pos);

	//��ȡposλ��uint32����	
	inline uint32 GetRadixBits(uint32 pos) const
	{
		if(pos<0 || pos >=m_bits.size() )
			return 0;
		return m_bits[pos];
	}

	//����posλ��ֵΪv��û�и�λ�õĻ���ֱ�Ӱ���������posλ������
	inline void SetRadixBits(uint32 v,uint32 pos)
	{
		if (pos>=m_bits.size())
		{
			m_bits.resize(pos+1,0);
		}
		m_bits[pos] = v;
	}
	
	
	//��ʾ����
	friend int FromString(BigUInt& N,const std::string& numbers);
	friend void DumpBits(const BigUInt& N,const char * fmt,...);

	//�Ƚϴ�С 
	friend int32 Compare(const BigUInt & X,const BigUInt & Y);
	friend int operator<(const BigUInt& v1,const BigUInt& v2);
	friend int operator==(const BigUInt& v1,const BigUInt& v2);
	friend int operator>(const BigUInt& v1,const BigUInt& v2);
	friend int operator<= (const BigUInt& v1,const BigUInt& v2);
	friend int operator>= (const BigUInt& v1,const BigUInt& v2);
	friend int operator!=(const BigUInt& v1,const BigUInt& v2);
	//��������
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
	//λ��:  4 3 2 1 0
	//���֣�| e...e | d...d | c...c | b...b | a...a |
	BigIntBitType m_bits;
	

	const static uint64 RADIX =		0x0000000100000000;
	const static uint64 CARDINAL =	0x00000000ffffffff;
};


#endif