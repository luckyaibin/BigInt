#include <vector>
#include <string>
#include <iostream>
#include <assert.h>

typedef unsigned int uint32;
typedef int int32;
typedef unsigned long long uint64;
typedef long long int64;


//��������ʾΪn^32 ���ƣ���0x1 0000 0000��������

// 0xFFFFFFFF == 4294967295
// 0xFFFFFFFF **2 == FFFFFFFE00000001 ==  4294967295**2 == 18446744065119617025

// 0xFFFFFFFF FFFFFFFF == 18446744073709551615 ~= 4294967295.9999999998835846781731**2

struct BigInt
{
	BigInt():m_Sign(true)//true Ϊ����falseΪ��
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
	
	BigInt( const BigInt& other)
	{
		m_bits = other.m_bits;
		m_Sign = other.m_Sign;
	}

	BigInt& operator=(const BigInt& other)
	{
		this->m_bits = other.m_bits;
		this->m_Sign = other.m_Sign;
		return *this;
	}
	
	std::string ToString() const;

	bool IsPositive() { return m_Sign;}
	const static uint64 RADIX =		0x0000000100000000;
	const static uint64 CARDINAL =	0x00000000ffffffff;

	const static bool NeedCarry(uint64 result);
	
	int Length() const;
	void Reset()
	{
		m_bits.clear();
		m_bits.push_back(0);
	}

	//����һ��С����
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
	//��ȡposλ��uint32����
	uint32 GetRadixBits(uint32 pos) const ;

	void SetRadixBits(uint32 v,uint32 pos);

	void AddRadixBits(uint32 v,uint32 pos);

	void MinusRadixBits(uint32 v,uint32 pos);
	

	typedef std::vector<uint32> BigIntBitType;

	//λ��:  4 3 2 1 0
	//���֣�| e...e | d...d | c...c | b...b | a...a |
	BigIntBitType m_bits;
	bool m_Sign;


	friend	void FromString(BigInt& N,const std::string& numbers)
	{
		int length = 0;
		for(;(!numbers.empty())&&numbers[length]!='\0';length++);

		for (int i=0;i<length;i++)
		{
			//N.Dump();
			int n = numbers[i]-'0';
			N.Mul(10);
			//N.Dump();
			N.Add(n);
			//N.Dump();
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


	//N1 > N2,��ôN1���ȴ��ڵ���N2����
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
					//��ֵ�ͽ�λ�ӵ������
					R.AddRadixBits(v,index1+index2);
					R.AddRadixBits(carry,index1+index2+1);
				}
			}
		return R;
	};







	friend BigInt operator+(const BigInt& X,const BigInt& Y);

	// 11110000 11110000        00110011 11000011 11110000 00001111  (2)

	BigInt& operator>>(int bits)
	{
		BigInt result,zero;

		if (bits>=this->Length()*4*8)
		{
			* this =  zero;
			return *this;
		}

		int complete_ints = bits/32;
		int remaind = bits%32;

		int from_hi_ints = this->Length() - complete_ints;

		for (int i=0;i<from_hi_ints;i++)
		{
			uint32 v = this->GetRadixBits(this->Length() - i - 1);
			result.SetRadixBits(v,from_hi_ints - i - 1);
		}

		// 11110000 11110000 11110000 11110000 

		//����һλ
		// 01111000 01111000 01111000 01111000

		//�����ƶ�remaindλ��hi_mask�� 31 ~ remaind ��lo_mask �� remaind ~ 0λ
		uint32 hi_mask = 0xffffffff << remaind;
		uint32 lo_mask = 0xffffffff >>(32 - remaind);

		for (int j=0;j<result.Length();j++)
		{
			uint32 hi_v=result.GetRadixBits(j+1);

			uint32 bits_from_hi = hi_v & lo_mask; // ��һλ��

			uint32 v = result.GetRadixBits(j);


			uint32 bits_from_lo = v &  hi_mask;

			//��λ��bits�Ƶ���λ�� ��bit���֣��ٺ�ԭ����λ�ĸ�bit��������µ�ֵ
			uint32 new_v = (bits_from_hi << (32-remaind) )  | (bits_from_lo>>remaind);

			result.SetRadixBits(new_v,j);
		}
		* this =  result;
		return *this;
	}
	friend BigInt operator>>(const BigInt& X,int bits)
	{
		BigInt result,zero;

		if (bits>=X.Length()*4*8)
		{
			return zero;
		}

		int complete_ints = bits/32;
		int remaind = bits%32;

		int from_hi_ints = X.Length() - complete_ints;

		for (int i=0;i<from_hi_ints;i++)
		{
			uint32 v = X.GetRadixBits(X.Length() - i - 1);
			result.SetRadixBits(v,from_hi_ints - i - 1);
		}
		
		//��������Ż�������remaind	> 0 �Ž�������Ĳ�����Ҳ�Ͳ�����int�ƶ�32bit֮���ڴ�Ϊ0������ֵ���������

		// 11110000 11110000 11110000 11110000 
		
		//����һλ
		// 01111000 01111000 01111000 01111000

		//�����ƶ�remaindλ��hi_mask�� 31 ~ remaind ��lo_mask �� remaind ~ 0λ
		uint32 hi_mask = 0xffffffff << remaind;
		uint32 lo_mask = (remaind == 0) ? 0 : 0xffffffff >>(32 - remaind);


		
		for (int j=0;remaind && j<result.Length();j++)
		{
			uint32 hi_v=result.GetRadixBits(j+1);
			
			uint32 bits_from_hi = hi_v & lo_mask; // ��һλ��

			uint32 v = result.GetRadixBits(j);


			uint32 bits_from_lo = v &  hi_mask;

			//��λ��bits�Ƶ���λ�� ��bit���֣��ٺ�ԭ����λ�ĸ�bit��������µ�ֵ
			uint32 new_v = (bits_from_hi << (32-remaind) )  | (bits_from_lo>>remaind);

			result.SetRadixBits(new_v,j);
		}
		return result;

	}

	// 11111111000000001111111100000000 11110000
	//����һλ��:
	// 11111110000000011111111000000001 11100000
	friend BigInt operator<<(const BigInt& X,int bits)
	{
		BigInt result,zero;

		int complete_ints = bits/32;
		int remaind = bits%32; //remaind 0~31
		
		//32λ���������ƶ�
		for(int idx=0;idx<X.Length();idx++)
		{
			uint32 v = X.GetRadixBits(idx);
			result.SetRadixBits(v,idx+complete_ints);
		}
		//�ƶ���ɢ��bit:
		uint32 hi_mask = (remaind==0) ? 0 : (0xffffffff << (32 - remaind)); //��ΪremaindΪ0ʱ��ֵv����32λ��Ȼ��v
		uint32 lo_mask = 0xffffffff >> remaind;
		
		for(int idx = result.Length();remaind && idx>=0;idx--)
		{
			uint32 hi_v = result.GetRadixBits(idx);
			uint32 lo_v = result.GetRadixBits(idx-1);

			uint32 new_hi_v = ( (hi_v & lo_mask) << remaind ) | ( ( lo_v & hi_mask ) >> (32 - remaind) );
			result.SetRadixBits(new_hi_v,idx);
		}


		return result;
	}
};
