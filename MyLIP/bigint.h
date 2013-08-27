#include <vector>
#include <string>
#include <iostream>
#include <assert.h>

typedef unsigned int uint32;
typedef int int32;
typedef unsigned long long uint64;
typedef long long int64;


//将大数表示为n^32 进制（即0x1 0000 0000）的数组

// 0xFFFFFFFF == 4294967295
// 0xFFFFFFFF **2 == FFFFFFFE00000001 ==  4294967295**2 == 18446744065119617025

// 0xFFFFFFFF FFFFFFFF == 18446744073709551615 ~= 4294967295.9999999998835846781731**2

struct BigInt
{
	BigInt():m_Sign(true)//true 为正，false为负
	{
		if (m_bits.size() == 0)
		{
			m_bits.insert(m_bits.begin(),0);
		}
	}
	BigInt(const uint32& ui32)
	{
		m_bits.push_back(ui32);
	}
	BigInt(const char * intstr)
	{
		Reset();
		FromString(*this,intstr);
	}
	


	
	std::string ToString() const;

	bool IsPositive() { return m_Sign;}
	const static uint64 RADIX =		0x0000000100000000;
	const static uint64 CARDINAL =	0x00000000ffffffff;

	const static bool NeedCarry(uint64 result);
	
	int Length() const;
	int Reset()
	{
		m_bits.clear();
		m_bits.push_back(0);
	}

	//乘以一个小整数
	// a b c d
	//*      v
	//---------
	         
	void Mul(const uint32& ui32) ;
	void Mul(const BigInt& N)
	{

	}
	void Add(const uint32& ui32) ;
	
	//
	uint32 Mod(const uint32& ui32)
	{
		uint32 index = 0;
		
		uint64 v = 0;
		uint64 quotient = 0;
		uint64 remainder = 0;
		do 
		{
			v = remainder*0x100000000 + m_bits[index];
			
			quotient = v / ui32;
			remainder = v % ui32;
		} while (++index<m_bits.size());
		return remainder;
	}
	void Dump() const;
	//获取pos位的uint32的数
	uint32 GetRadixBits(uint32 pos) const ;

	void SetRadixBits(uint32 v,uint32 pos);

	void AddRadixBits(uint32 v,uint32 pos);

	void MinusRadixBits(uint32 v,uint32 pos);
	

	typedef std::vector<uint32> BigIntBitType;

	//位置:  4 3 2 1 0
	//数字：| e...e | d...d | c...c | b...b | a...a |
	BigIntBitType m_bits;
	bool m_Sign;


	friend	void FromString(BigInt& N,const std::string& numbers)
	{
		int length = 0;
		for(;(!numbers.empty())&&numbers[length]!='\0';length++);

		for (int i=0;i<length;i++)
		{
			N.Dump();
			int n = numbers[i]-'0';
			N.Mul(10);
			N.Dump();
			N.Add(n);
			N.Dump();
		}
	}
	friend BigInt Add(const BigInt& N1,const BigInt& N2)
	{
		BigInt R;
		

		uint64 carry=0;
		uint32 index=0;
		for (;index<N1.Length() || index < N2.Length();index++)
		{
			uint32 n1 = N1.GetRadixBits(index);
			uint32 n2 = N2.GetRadixBits(index);
			uint64 v = n1+n2+carry;

			carry = v / RADIX;
			v = v % RADIX;
			R.AddRadixBits((uint32)v,index);
			R.AddRadixBits((uint32)carry,index+1);
		}
		return R;
	}


//N1 > N2,那么N1长度大于等于N2长度
friend BigInt Minus(const BigInt& N1,const BigInt& N2)
{
	BigInt R;
	R = N1;
	
	for (int index=0;index<N2.Length();index++)
	{
		uint32 v = N2.GetRadixBits(index);
		R.MinusRadixBits(v,index);
	}
	return R;
}


/*
						a		b		c   :index1 0~2
				  d		e		f		g   :index2 0~3
				  ------------------------
					   a*g     b*g     c*g
				 a*f   b*f     c*f           
         a*e     b*e   c*e 
 a*d     b*d     c*d

 */
friend  BigInt Mul(const BigInt& N1,const BigInt& N2)
{
		BigInt R;
		
		uint32 carry = 0;

		uint64 v = 0;
		uint32 index1 = 0;
		uint32 index2 = 0;
		for (;index1<N1.Length();index1++)
		{
			for (;index2<N2.Length();index2++)
			{
				uint32 n1 = N1.GetRadixBits(index1);
				uint32 n2 = N2.GetRadixBits(index2);

				v = n1*n2 ;
				carry = v / RADIX;
				v = v % RADIX;

				uint32 r = R.GetRadixBits(index1+index2);
				//把值和进位加到结果中
				R.AddRadixBits(v,index1+index2);
				R.AddRadixBits(carry,index1+index2+1);
			}
		}
	return R;

};






BigInt operator+(const BigInt& X,const BigInt& Y);
