#include "biguint.h"
#include<fstream>
#include <stdarg.h>
#include <algorithm>
int BigUInt::Length()const
{
	return m_bits.size();
}
int BigUInt::ValidLength() const
{
	return (m_bits.size()>0)?(GetNonZeroIdx()+1):(0); 
}

void BigUInt::TrimHiZeros()
{
	int v = 0;
	while (m_bits.size() >=1 && (v=m_bits[m_bits.size()-1]) == 0 )
	{
		m_bits.erase(m_bits.begin()+m_bits.size()-1);
	}
	if (m_bits.size()==0)
	{
		m_bits.push_back(0);
	}
}




void BigUInt::AddRadixBits( uint32 val,uint32 pos )
{
	if (val==0)
	{
		return;
	}
	//assert(pos);
	if (pos>=m_bits.size())
	{
		//m_bits.push_front(0);
		//m_bits.insert(m_bits.begin(),0);
		//m_bits.push_back(0);
		m_bits.resize(pos+1,0);
	}
	uint32 carry = 0;
	uint64 v = m_bits[pos];
	v = v + val;
	
	carry = v / RADIX;
	v = v % RADIX;
	m_bits[pos] = v;

	//把进位加上去
	AddRadixBits(carry,pos+1);
}

//保证够减
void BigUInt::MinusRadixBits(uint32 val,uint32 pos)
{
	if (val==0)
	{
		return;
	}

	uint64 v = m_bits[pos];

	//不够减
	if(v<val)
	{
		MinusRadixBits(1,pos+1);
		v = RADIX + v;
	}
	v = v - val;

	m_bits[pos] = v;
}



void BigUInt::Add( const uint32& ui32 )
{
	uint64 carry = 0;
	//加到最低位
	uint64 v = m_bits[0];
	v = v + ui32;

	carry = v / RADIX;
	v = v % RADIX;

	m_bits[0] = (uint32)v;

	int pos = 1;
	//处理除最低位以外的进位
	while (carry>0)
	{
		if ((int)m_bits.size()-1-pos<0)
		{
			//m_bits.push_front(0);
			//m_bits.insert(m_bits.begin(),0);
			m_bits.push_back(0);
		}
		uint64 v = m_bits[pos];

		v += carry;

		carry = v / RADIX;
		v = v % RADIX;

		m_bits[pos] = (uint32)v;
		++pos;
	}
}
void BigUInt::Mul( const uint32& ui32 ) 
{
	uint64 carry = 0;
	for (uint32 i=0;i<m_bits.size();i++)
	{
		uint64 v = m_bits[i];	

		v *= ui32;
		v += carry;

		carry = v / RADIX;
		v = v % RADIX;
		m_bits[i] = (uint32)v;
	}

	//最高位有进位
	if (carry > 0)
	{
		//m_bits.push_front(0);
		//m_bits.insert(m_bits.begin(),0);
		m_bits.push_back(0);
		m_bits[m_bits.size()-1] = (uint32)carry;
	}
}
uint32 BigUInt::Mod( const uint32& ui32 )
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
BigUInt BigUInt::DivBy10( BigUInt& Q,uint32 &R )
{
	BigUInt res;
	int32 idx = this->GetNonZeroIdx();
	uint64 result_ = 0;
	uint64 remainder = 0;
	
	while (idx>=0)
	{
		uint64 v = this->GetRadixBits(idx)+remainder*RADIX;

		result_ = (v / 10); //result 每次最多增加　的是　9*RADIX 
		remainder   = v % 10;

		if(result_>0)
			res = res + (BigUInt(result_)<<(idx*32));		
		idx--;
	}
	Q = res;
	R = remainder;
	return res;
}

std::string BigUInt::ToString() const
{
	std::string numbers;

	BigUInt Q = *this;
	BigUInt  R;
	while(Q>BigUInt("0"))
	{
		BigDiv(Q,BigUInt("10"),Q,R);
		numbers.insert(numbers.begin(),R.m_bits[0]+'0');
		std::string b_right = "114381625757888867669235779976146612010218296721242362562561842935706935245733897830597123563958705058989075147599290026879543541";
		char a = numbers[0];
		char b = b_right[b_right.size() - numbers.size()];

		if (numbers == "3563958705058989075147599290026879543541")
		{
			printf("next step will be wrong..");
		}
		if (a!=b)
		{
			printf("stop..");
		}
	}
	//std::reverse(numbers.begin(),numbers.end());
	
	return numbers;
}


int32 BigUInt::GetNonZeroBitIdx() /*get_non_zero_bit_idx */
{
	int hightest_idx = -1;
	int len = ValidLength();
	for (int i=len-1;i>=0;i--)
	{
		uint32 v = GetRadixBits(i);
		if (v)
		{
			for (int idx=31;idx>=0;idx--)
			{
				if ( (v>>idx) & 1)
				{
					hightest_idx = idx + 32*i;
					goto RETURN;
				}
			}
		}
	}
RETURN: 
	return hightest_idx;
}
int32 BigUInt::GetNonZeroIdx() const
{
	int hightest_idx = -1;
	int len = m_bits.size();
	for (int i=len-1;i>=0;i--)
	{
		uint32 v = GetRadixBits(i);
		if (v)
		{
			hightest_idx =i;
			goto RETURN;	 
		}
	}
RETURN: 
	return hightest_idx;
}
/*
      
	idx_id                   idx_lo
	  |                        |
 11110000 00001111 11001100 00110011
 
*/
BigUInt BigUInt::GetBitRangBigInt(int bit_idx_hi,int bit_idx_lo) const
{
	if( !(bit_idx_hi>=0 && bit_idx_lo>=0))
	{
		printf("stop.");
	}
	assert(bit_idx_hi>=0 && bit_idx_lo>=0 &&"invalid index for GetBitRangeBigInt()!");
	
	BigUInt res;
	int highest_bit_idx = this->Length()*32 - 1;// this->GetNonZeroBitIdx();

	res = (*this)<< (highest_bit_idx-bit_idx_hi);// 左移，把高于bit_idx_hi的bit都扔掉
	res = (res)>>( (highest_bit_idx-bit_idx_hi) + bit_idx_lo);
	return res;
}
bool BigUInt::GetBitByIdx(int bit_idx) const
{
	uint32 v = GetRadixBits(bit_idx/32);

	uint32 inner_idx = bit_idx % 32;

	uint32 mask = 1<< (inner_idx);

	uint32 is_on = v & mask;
	return is_on;
}
void BigUInt::Reset()
{
	m_bits.clear();
	m_bits.push_back(0);
}


BigUInt operator+(const BigUInt& X,const BigUInt& Y)
{
	BigUInt Z;
	BigUInt Smaller;
	uint32 carry=0;
	int32 xlen = X.GetNonZeroIdx()+1;//X.Length();//
	int32 ylen = Y.GetNonZeroIdx()+1;//Y.Length();//
	if (xlen>=ylen)
	{
		Z = X;
		Smaller = Y;
	}
	else
	{
		Z = Y;
		Smaller = X;
	}
	for (uint32 i=0;i<std::min<uint32>(xlen,ylen);i++)
	{
		Z.AddRadixBits((uint32)Smaller.GetRadixBits(i),i);
	}
	return Z;
}

int32 Compare( const BigUInt & X,const BigUInt & Y )
{
	int32 idx1 = X.ValidLength() - 1;
	int32 idx2 = Y.ValidLength() - 1;

	uint32 v1 = 0;
	uint32 v2 = 0;

	//找到第一个不为0 的高位的值和索引
	while( !(v1=X.GetRadixBits(idx1)) && idx1>0)
	{ 
		idx1--;
	}

	while( !(v2 = Y.GetRadixBits(idx2)) && idx2>0 )
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
				v1 = X.GetRadixBits(idx1);
				v2 = Y.GetRadixBits(idx2);
			}
		}
	}
	return result;
}

int operator<(const BigUInt& v1,const BigUInt& v2)
{
	return Compare(v1,v2) == -1;
}

int operator==(const BigUInt& v1,const BigUInt& v2)
{
	return Compare(v1,v2) == 0;
}

int operator>(const BigUInt& v1,const BigUInt& v2)
{
	return Compare(v1,v2) == 1;
}

int operator<= (const BigUInt& v1,const BigUInt& v2)
{
	return !(v1>v2);
}

int operator>= (const BigUInt& v1,const BigUInt& v2)
{
	return !(v1<v2);
}

int operator!=(const BigUInt& v1,const BigUInt& v2)
{
	return !(v1==v2);
}


//N1 > N2,那么N1长度大于等于N2长度
BigUInt operator-( const BigUInt&N1,const BigUInt& N2 )
{
	BigUInt R=N1;
	for (int index=0;index<N2.ValidLength();index++)
	{
		uint32 v = N2.GetRadixBits(index);
		R.MinusRadixBits(v,index);
	}
	R.TrimHiZeros();
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
BigUInt operator*(const BigUInt& N1,const BigUInt& N2)
{
	BigUInt R;
	uint32 carry = 0;

	uint64 v = 0;
	int len1 = N1.ValidLength();
	int len2 = N2.ValidLength();
	for (uint32 index1 = 0;index1<len1;index1++)
	{
		uint64 n1 = N1.GetRadixBits(index1);
		if (n1==0)
		{
			continue;
		}
		for (uint32 index2 = 0;index2<len2;index2++)
		{
			uint64 n2 = N2.GetRadixBits(index2);
			if (n2==0)
			{
				continue;
			}
			//注意这里转型操作，否则v里的结果是32位的
			v = (uint64)n1*(uint64)n2 ;
			carry = v / BigUInt::RADIX;
			v = v % BigUInt::RADIX;

			//把值和进位加到结果中
			R.AddRadixBits(v,index1+index2);
			R.AddRadixBits(carry,index1+index2+1);
		}
	}
	R.TrimHiZeros();
	return R;
}
BigUInt operator/ (const BigUInt& X,const BigUInt& Y)
{
	BigUInt Q;
	BigUInt R;
	return Fast_BigDiv(X,Y,Q,R);
}

BigUInt operator% (const BigUInt&X,const BigUInt& M)
{
	BigUInt q;//商
	BigUInt r;//余
	BigDiv(X,M,q,r);
	return r;
}

BigUInt operator>>( const BigUInt& X,int bits )
{
	BigUInt result = X,zero;
	if (bits>=X.ValidLength()*4*8)
	{
		return zero;
	}
	int complete_ints = bits/32;
	int remaind = bits%32;		
	if (complete_ints>0)
	{
		result.m_bits.erase(result.m_bits.begin(),result.m_bits.begin() + complete_ints);
	}
	//这里可以优化掉，当remaind	> 0 才进行下面的操作，也就不存在int移动32bit之后期待为0，但是值不变的现象

	// 11110000 11110000 11110000 11110000 

	//右移一位
	// 01111000 01111000 01111000 01111000

	//向右移动remaind位，hi_mask是 31 ~ remaind ，lo_mask 是 remaind ~ 0位
	uint32 hi_mask = 0xffffffff << remaind;
	uint32 lo_mask = (remaind == 0) ? 0 : 0xffffffff >>(32 - remaind);

	for (int j=0;remaind && j<result.ValidLength();j++)
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

BigUInt operator<<( const BigUInt& X,int bits )
{
	BigUInt result=X;

	int complete_ints = bits/32;
	int remaind = bits%32; //remaind 0~31

	if (complete_ints>0)
	{
		//在vector头部（相当于在数的右边）加上complete_ints个0，也就是左移了complete_ints*32位
		result.m_bits.insert(result.m_bits.begin(),complete_ints,0);
	}
	//移动零散的bit:
	uint32 hi_mask = (remaind==0) ? 0 : (0xffffffff << (32 - remaind)); //因为remaind为0时，值v左移32位仍然是v
	uint32 lo_mask = 0xffffffff >> remaind;

	for(int idx = result.ValidLength();remaind && idx>=0;idx--)
	{
		uint32 hi_v = result.GetRadixBits(idx);
		uint32 lo_v = result.GetRadixBits(idx-1);

		uint32 new_hi_v = ( (hi_v & lo_mask) << remaind ) | ( ( lo_v & hi_mask ) >> (32 - remaind) );
		if (new_hi_v) //不随意添加额外的空白uint32
		{
			result.SetRadixBits(new_hi_v,idx);
		}
	}
	return result;
}
/*
二进制形式的除法
*/
BigUInt BigDiv( const BigUInt& X,const BigUInt& Y,BigUInt &Q,BigUInt&R )
{
	BigUInt One("1");
	BigUInt Result("0");
	BigUInt a = X;
	BigUInt b = Y;

	int32 na = 0;
	int32 nb = 0;

	na = a.GetNonZeroBitIdx();
	nb = b.GetNonZeroBitIdx();
	int loopcnt = 0;
	while(a>=b)
	{
		na = a.GetNonZeroBitIdx();
		int diff = na - nb;
		//a.Dump("a初始值:");
		//b.Dump("b初始值,猜测偏移diff=%d:",diff);
		BigUInt shift_b = b<<diff;
		while (diff>0 && a < shift_b)
			diff--,shift_b = b<<diff;

		//if ( a >= shift_b )
		{
			//( b<<diff ).Dump("b移动后，实际偏移diff=%d",diff);
			Result = Result + ( One <<diff );
			//Result.Dump("结果：");
		}
		
		a = a - shift_b;
		loopcnt++;
		//a.Dump("之后a:");
	}

	Q = Result;
	R = a;
	return Result;
}
/*
0x100000000进制的除法

这里用到的估值算法要看唐纳特的第二卷，本人也未完全理解
  2379../29.. :

      0082..
    =======
29..| 2379..
      232
	 -----
	    59
		58
	 -----
	     1

 现在把b对齐（令29的最高位>=5），通过移位(乘以2^n)

        9..
    =======
58..| 4758..
      522 > 第一次的试商（q = 47/5 == 9），不满足,那么q--;
	 -----
	
(          
      =======
 58.. | 5858  第一次试商，q=58 / 5 == 
)

        82
    =======
 58 | 4758
      464 > 第一次的修正，满足，则继续
	 -----
	   118  第二次的试商(q = 11/5 == 2),满足
	   116
    -------
	     2
*/

BigUInt Fast_BigDiv(const BigUInt& X,const BigUInt& Y,BigUInt&Q,BigUInt&R)
{
	BigUInt Result("0");
	BigUInt a = X;
	a.TrimHiZeros();
	BigUInt b = Y;
	b.TrimHiZeros();
	int adjust_shift = 0;
	//调整b,让b的最高位 >= RADIX/2
	while ( b.GetRadixBits(b.GetNonZeroIdx()) < (BigUInt::RADIX/2))
	{
		b=(b<<1);
		adjust_shift++;
	}
	//调整b的同时，也要调整a
	if(adjust_shift>0)
	{
		a = a<<adjust_shift;
	} 
	int a_begin_idx = a.GetNonZeroIdx();
	int a_valid_len = a.ValidLength();
	int b_begin_idx = b.GetNonZeroIdx();
	int b_valid_len = b.ValidLength();

	//a的长度等于b的长度，要在a的最前面插入0，保证a的长度比b至少长1
	if(a_valid_len==b_valid_len)
	{
		a.m_bits.push_back(0);
		a_begin_idx++;//前移一位，从前面的0开始
	}
	while(a>=b)
	{
		//从a高位取出比b长度+1个uint32。 最高位索引uit32的最高bit，到低位索引uint32的最低bit
		//由于a可能是对齐过的，所以a的最高位的uint32可能是0，因此下面很多地方都用了Length，而不是ValidLength
		BigUInt a_part = a.GetBitRangBigInt(a_begin_idx*32+31,(a_begin_idx-b_valid_len)*32);

		//取出a的高2位（2个uint32），b的高1位（1个uint32），对商估值
		uint64 a_part_hi_2 = (uint64)a_part.GetRadixBits(a_part.Length() - 1) << 32 | (uint64)a_part.GetRadixBits(a_part.Length() - 2);

		uint64 b_part_hi_1 = b.GetRadixBits(b.GetNonZeroIdx());

		uint64 guess_v = a_part_hi_2 / b_part_hi_1;

		int guess_time = 0;
		while (a_part< b*guess_v)//guess_v最多比真实值<2,即， guess_v-2<= real_v <= guess_v;这段的推导要看唐纳德第二卷
		{
			guess_v--;
			guess_time++;
		}
		//此后的guess_v就是真实的值		
		assert(guess_time<=2);
		
		uint32 carry = guess_v / BigUInt::RADIX;
		uint32 value = guess_v % BigUInt::RADIX;
		
		Result.AddRadixBits(value,a_begin_idx-b_valid_len);
		Result.AddRadixBits(carry,a_begin_idx-b_valid_len+1);
		
		/*
		a = a - b * ( res << N)
		  = a - b * ( res * 2^^N)
		  = a - b * res * 2^^N
		  = a - 
		*/
		//这里carry有可能>=0，要用uint64位的构造函数
		//a = a - b*(BigInt(guess_v)<<(a_begin_idx-b_valid_len)*32);
		a = a - (b <<(a_begin_idx-b_valid_len)*32) * guess_v;

		a_begin_idx--;
	}

	Q = Result;
	R = a>>adjust_shift;
	return Result;
}
//欧几里德算法，求X和Y的最大公约数
BigUInt GCD(const BigUInt& X,const BigUInt& Y)
{
	BigUInt Zero("0");
	BigUInt a;
	BigUInt b;
	BigUInt r;
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
		BigUInt tmp = b;
		b =  a%b;
		a = tmp;
	}
	return a;		
}
/*

	gcd(a,b) = gcd(b,a%b); // 定理

	设：
    a*x1 + b*y1 = gcd(a,b)
    
	//另 a= b, b = a % b; 下一次递归
	b * x2 + ( a % b ) * y2 = gcd(b,a%b);
	


*/

void ExEuclid( BigUInt a, BigUInt b,BigUInt& x,BigUInt&y )
{
	BigUInt old_a = a;
	BigUInt old_b = b;

	//r 为 a % b 的模，也是a / b 的余数
	//q 为 a / b 的商
	//这里一次性都求出来
	BigUInt r;
	BigUInt q;
	BigUInt tmp = b;

	Fast_BigDiv(a,b,q,r);

	b = r;
	a = tmp;

	if (b == BigUInt("0"))
	{
		x = BigUInt("1");
		y = BigUInt("0");

		BigUInt tmp = x;
		x = y;
		y = tmp - q*y;
		return;
	}

	ExEuclid(a,b,x,y);

	BigUInt old_x = x;
	x = y;
	y = old_x - q*x;
}

BigUInt ExpMod(const BigUInt& a,const BigUInt& b,const BigUInt& m)
{
	BigUInt res("1");
	BigUInt multiplizer = a;
	BigUInt exp = b;
	BigUInt Zero("0");
	while (exp > Zero)
	{
		//取出一个整数字节
		uint32 lowestbit = exp.GetRadixBits(0);
		while (lowestbit!=0)
		{
			if(lowestbit&1)
			{
				res = (res * multiplizer)%m;
			}
			multiplizer = (multiplizer * multiplizer)%m;
			lowestbit = lowestbit >> 1;
		}
		exp = exp >> 32;
	}
	return res;
}


BigUInt Exp(const BigUInt& a,const BigUInt& b)
{
	BigUInt res("1");
	BigUInt multiplizer = a;
	BigUInt exp = b;
	BigUInt Zero("0");
	while (exp > Zero)
	{
		//取出一个整数字节
		uint32 lowestbit = exp.GetRadixBits(0);
		while (lowestbit!=0)
		{
			if(lowestbit&1)
			{
				res = (res * multiplizer) ;
			}
			multiplizer = (multiplizer * multiplizer) ;
			lowestbit = lowestbit >> 1;
		}
		exp = exp >> 32;
	}
	return res;
}
int FromString( BigUInt& N,const std::string& numbers_ )
{
	std::string numbers = numbers_;
	int sign = 0;
	N.Reset();
	int length = 0;
	for(;(!numbers.empty())&&numbers[length]!='\0';length++);

	if (numbers[0] == '-')
	{
		sign = 1; //1表示负
		numbers.erase(numbers.begin());//删掉符号
	}
	else if(numbers[0]=='+')
	{
		sign = 0;
		numbers.erase(numbers.begin());//删掉符号
	}

	int base = 10;
	if (numbers[0]=='0' && (numbers[1]== 'x' || numbers[1]== 'X'))
	{
		base = 16;
	}

	//N.Dump();
	switch (base)
	{
	case 10:
		{
			for (int i=0;i<length;i++)
			{
				int n = numbers[i]-'0';
				N.Mul(10);
				//N.Dump();
				N.Add(n);
			}
		}
		break;
	case 16:
		{
			for (int i=2;i<length;i++)
			{
				int n = numbers[i];
				if (n==' ')//跳过空白
				{
					continue;
				}
				N.Mul(16);

				switch(n)
				{
				case 'a':
				case 'A':
					N.Add(10);
					break;
				case 'b':
				case 'B':
					N.Add(11);
					break;
				case 'c':
				case 'C':
					N.Add(12);
					break;
				case 'd':
				case 'D':
					N.Add(13);
					break;
				case 'e':
				case 'E':
					N.Add(14);
					break;
				case 'f':
				case 'F':
					N.Add(15);
					break;
					//for 0~9
				default:
					{
						n = n -'0';
						assert(n>=0 && n<=9);
						N.Add(n);
					}
				}
			}
		}
		break;
	}
	return sign;
}


void DumpBits(const BigUInt& N,const char *fmt,...)
{
	static int idx = 0;
	idx++;

	std::ofstream fs;
	fs.open("bits.txt",std::ios_base::binary | std::ios_base::out | std::ios_base::app);
	std::string index;
	char buf[100]  = {0};
	index = itoa(idx,buf,10);

	char out[1024]={0};

	va_list ap;
	va_start(ap, fmt);

	vsnprintf(out, 1024, fmt, ap);

	va_end(ap);

	index +=out;
	index += ":\n";

	fs.write(index.c_str(),index.length());
	for (int i=0;i<N.m_bits.size();i++)
	{
		printf("%d	十进制	:	%u \n", i,N.m_bits[i]);
		printf("%d	十六进制:	0x%x \n",i,N.m_bits[i]);
		std::string s;
		for (int j=0;j<32;j++)
		{
			int v = (N.m_bits[N.m_bits.size() - 1 - i]>>(31-j))&1;
			char c = v + '0';
			s +=c;
		}
		printf("%d	二进制:	%s	\n",N.m_bits.size() - i,s.c_str());
		fs.write(s.c_str(),s.length());
	}
	printf("\n");
	fs.write("\n\r",1);
	fs.close();
}
