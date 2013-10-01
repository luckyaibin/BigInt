#include <vector>
#include <string>
#include <iostream>
#include <assert.h>

typedef unsigned int uint32;
typedef int int32;
typedef unsigned long long uint64;
typedef long long int64;

struct BigInt;
//前向声明
BigInt BigDiv(const BigInt& X,const BigInt& Y,BigInt &Q,BigInt&R);
BigInt GCD(const BigInt& X,const BigInt& Y);
BigInt ExEuc(const BigInt&a,const BigInt& b,BigInt& x,BigInt& y);
BigInt ExpMod(const BigInt& a,const BigInt& b,const BigInt& m);

void ExEuclid( BigInt a,BigInt b,BigInt& x,BigInt&y );

//将大数表示为n^32 进制（即0x1 0000 0000）的数组

// 0xFFFFFFFF == 4294967295
// 0xFFFFFFFF **2 == FFFFFFFE00000001 ==  4294967295**2 == 18446744065119617025

// 0xFFFFFFFF FFFFFFFF == 18446744073709551615 ~= 4294967295.9999999998835846781731**2

struct BigInt
{
	BigInt():m_Sign(true)//true 为正，false为负
	{
		m_bits.resize(100);
		//if (m_bits.size() == 0)
		//{
			//m_bits.insert(m_bits.begin(),0);
		//	m_bits.push_back(0);
		//}
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
	
	//获得最高为1的比特的索引 N N-1 ... 3 2 1 0
	uint32 GetNonZeroBitIdx(); //get_non_zero_bit_idx;

	//比较大小 
	/*
		00000000 00001111 11110000 0000111
		         00000001 00001110 0011001
	*/
	int32 Compare(const BigInt & N) const
	{
		int32 idx1 = this->Length() - 1;
		int32 idx2 = N.Length() - 1;

		uint32 v1 = 0;
		uint32 v2 = 0;
		
		//找到第一个不为0 的高位的值和索引
		while( !(v1=this->GetRadixBits(idx1)) && idx1>0)
		{ 
			idx1--;
		}

		while( !(v2 = N.GetRadixBits(idx2)) && idx2>0 )
		{ 
			idx2--;
		}
		int32 result = 0;
		while (idx1>=0 || idx2 >=0)
		{	
			if (idx1>idx2)
			{
				result = 1;
				break;
			}
			else if (idx2>idx1)
			{
				result = -1;
				break;
			}
			else  //idx1 == idx2
			{
				if (v1>v2)
				{
					result = 1;
					break;
				}
				else if (v2>v1)
				{
					result = -1;
					break;
				}
				else // idx1 == idx2 并且 v1 == v2，要判断下一位
				{
					idx1--;
					idx2--;
					v1 = this->GetRadixBits(idx1);
					v2 = N.GetRadixBits(idx2);
				}
			}
		}
		return result;
	}

	friend int operator < (const BigInt& v1,const BigInt& v2)
	{
		return v1.Compare(v2) == -1;
	}

	friend int operator == (const BigInt& v1,const BigInt& v2)
	{
		return v1.Compare(v2) == 0;
	}

	friend int operator > (const BigInt& v1,const BigInt& v2)
	{
		return v1.Compare(v2) == 1;
	}

	friend int operator <= (const BigInt& v1,const BigInt& v2)
	{
		return !(v1>v2);
	}

	friend int operator >= (const BigInt& v1,const BigInt& v2)
	{
		return !(v1<v2);
	}

	friend int operator !=(const BigInt& v1,const BigInt& v2)
	{
		return !(v1==v2);
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
		uint32 index = m_bits.size() - 1;
		
		uint64 v = 0;
		uint64 quotient = 0;
		uint64 remainder = 0;
		do 
		{
			v = remainder*0x100000000 + m_bits[index];
			
			quotient = v / ui32;
			remainder = v % ui32;
		} while (--index>=0);
		return remainder;
	}
	void Dump(const char * msg="",...) const;
	//获取pos位的uint32的数
	inline uint32 GetRadixBits(uint32 pos) const ;

	inline void SetRadixBits(uint32 v,uint32 pos);

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
	
	friend BigInt operator+(const BigInt& X,const BigInt& Y);

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

	friend BigInt operator-(const BigInt&N1,const BigInt& N2)
	{
		return Minus(N1,N2);
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
			
			for (uint32 index1 = 0;index1<N1.Length();index1++)
			{
				for (	uint32 index2 = 0;index2<N2.Length();index2++)
				{
					uint64 n1 = N1.GetRadixBits(index1);
					uint64 n2 = N2.GetRadixBits(index2);
					//注意这里转型操作，否则v里的结果是32位的
					v = (uint64)n1*(uint64)n2 ;
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

	friend BigInt operator*(const BigInt& N1,const BigInt& N2)
	{
		return ::Mul(N1,N2);
	}


	friend BigInt Mod(const BigInt& X,const BigInt& M)
	{
		BigInt q;//商
		BigInt r;//余
		BigDiv(X,M,q,r);
		return r;
	}

	friend BigInt operator% (const BigInt&X,const BigInt& Y)
	{
		return ::Mod(X,Y);
	}


	

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

		//右移一位
		// 01111000 01111000 01111000 01111000

		//向右移动remaind位，hi_mask是 31 ~ remaind ，lo_mask 是 remaind ~ 0位
		uint32 hi_mask = 0xffffffff << remaind;
		uint32 lo_mask = 0xffffffff >>(32 - remaind);

		for (int j=0;j<result.Length();j++)
		{
			uint32 hi_v=result.GetRadixBits(j+1);

			uint32 bits_from_hi = hi_v & lo_mask; // 高一位的

			uint32 v = result.GetRadixBits(j);


			uint32 bits_from_lo = v &  hi_mask;

			//高位的bits移到低位的 高bit部分，再和原来低位的搞bit部分组成新的值
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
		
		//这里可以优化掉，当remaind	> 0 才进行下面的操作，也就不存在int移动32bit之后期待为0，但是值不变的现象

		// 11110000 11110000 11110000 11110000 
		
		//右移一位
		// 01111000 01111000 01111000 01111000

		//向右移动remaind位，hi_mask是 31 ~ remaind ，lo_mask 是 remaind ~ 0位
		uint32 hi_mask = 0xffffffff << remaind;
		uint32 lo_mask = (remaind == 0) ? 0 : 0xffffffff >>(32 - remaind);


		
		for (int j=0;remaind && j<result.Length();j++)
		{
			uint32 hi_v=result.GetRadixBits(j+1);
			
			uint32 bits_from_hi = hi_v & lo_mask; // 高一位的

			uint32 v = result.GetRadixBits(j);


			uint32 bits_from_lo = v &  hi_mask;

			//高位的bits移到低位的 高bit部分，再和原来低位的搞bit部分组成新的值
			uint32 new_v = (bits_from_hi << (32-remaind) )  | (bits_from_lo>>remaind);

			result.SetRadixBits(new_v,j);
		}
		return result;

	}

	// 11111111000000001111111100000000 11110000
	//左移一位后:
	// 11111110000000011111111000000001 11100000
	friend BigInt operator<<(const BigInt& X,int bits)
	{
		BigInt result=X;

		int complete_ints = bits/32;
		int remaind = bits%32; //remaind 0~31
		
		//32位整数这样移动
		//for(int idx=0;idx<X.Length();idx++)
		//{
		//	uint32 v = X.GetRadixBits(idx);
		//	result.SetRadixBits(v,idx+complete_ints);
		//}

		if (complete_ints>0)
		{
			//在vector头部（相当于在数的右边）加上complete_ints个0，也就是左移了complete_ints*32位
			result.m_bits.insert(result.m_bits.begin(),complete_ints,0);
		}
		/*
		if (complete_ints>0)
		{
		//先插入0，在插入真正的数据，要比先插入数据，然后在头部插入0要快
		result.m_bits.insert(result.m_bits.begin(),complete_ints,0);
		result.m_bits.insert(result.m_bits.end(),X.m_bits.begin(),X.m_bits.end());
		}
		*/

		//移动零散的bit:
		uint32 hi_mask = (remaind==0) ? 0 : (0xffffffff << (32 - remaind)); //因为remaind为0时，值v左移32位仍然是v
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
	
	//大数除法：
	//返回两个大数除法的商，同时Q置为商，R为余数
	friend BigInt BigDiv(const BigInt& X,const BigInt& Y,BigInt &Q,BigInt&R);

	friend BigInt operator/ (const BigInt& X,const BigInt& Y)
	{
		BigInt Q;
		BigInt R;
		return BigDiv(X,Y,Q,R);
	}

	//欧几里德算法，求X和Y的最大公约数
	friend BigInt GCD(const BigInt& X,const BigInt& Y);

	//扩展欧几里德算法，求模反元素
	friend void ExEuclid(BigInt a, BigInt b,BigInt& x,BigInt&y); 
};
