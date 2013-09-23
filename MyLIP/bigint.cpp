#include "bigint.h"
#include<fstream>
#include <stdarg.h>
const  bool BigInt::NeedCarry( uint64 result )
{
	return result>CARDINAL;
}

int BigInt::Length() const
{
	return m_bits.size();
}
//下标是 n n-1 n-2 ... 2 1 0 对应uint32数组
//由于uint32 数组 是 0 1 2 ... n-1 n，并且为了使用vector时的效率（在头部插入）
uint32 BigInt::GetRadixBits( uint32 pos ) const
{
	if(pos<0 || pos >=m_bits.size() )
		return 0;
	return m_bits[pos];
}

void BigInt::SetRadixBits( uint32 v,uint32 pos )
{
	//assert(pos);
	while (pos>=m_bits.size())
	{
		//m_bits.push_front(0);
		//m_bits.insert(m_bits.begin(),0);
		m_bits.push_back(0);
	}
	m_bits[pos] = v;
}

void BigInt::AddRadixBits( uint32 val,uint32 pos )
{
	if (val==0)
	{
		return;
	}
	//assert(pos);
	while (pos>=m_bits.size())
	{
		//m_bits.push_front(0);
		//m_bits.insert(m_bits.begin(),0);
		m_bits.push_back(0);
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
void BigInt::MinusRadixBits(uint32 val,uint32 pos)
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

void BigInt::Mul( const uint32& ui32 ) 
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

void BigInt::Add( const uint32& ui32 )
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

void BigInt::Dump(const char *fmt,...) const
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
	for (int i=0;i<m_bits.size();i++)
	{
		printf("%d	十进制	:	%u \n", i,m_bits[i]);
		printf("%d	十六进制:	0x%x \n",i,m_bits[i]);
		std::string s;
		for (int j=0;j<32;j++)
		{
			int v = (m_bits[m_bits.size() - 1 - i]>>(31-j))&1;
			char c = v + '0';
			s +=c;
		}
		printf("%d	二进制:	%s	\n",m_bits.size() - i,s.c_str());
		fs.write(s.c_str(),s.length());
	}
	printf("\n");
	fs.write("\n\r",1);
	fs.close();
}

std::string BigInt::ToString() const
{
	std::string numbers;

	BigIntBitType curr_number_bits;
	curr_number_bits = m_bits;

	for(uint64 index = 0;index <curr_number_bits.size();) 
	{
		std::string quotient_str; 
		uint64 v = 0;
		uint64 quotient =0;//商
		uint64 remainder=0;//余数


		//每一轮完成，得到一个大数除以10的最终余数 remainder
		//把RADIX进制的数转为10进制字符串
		do
		{		
			v = curr_number_bits[index];		
			v = v + remainder * RADIX;

			quotient = v / 10;
			remainder = v % 10;	

			char c = quotient + '0';
			quotient_str = (c+quotient_str);
			index++;
		}
		while(remainder||index < curr_number_bits.size());

		//一轮结束，求出了一个余数
		char cc = remainder+'0';
		numbers =  cc + numbers;
		//开始下一轮
		BigInt next_round(quotient_str.c_str());
		index = 0;
		curr_number_bits = next_round.m_bits;
	}
	return numbers;
}

uint32 BigInt::GetNonZeroBitIdx() /*get_non_zero_bit_idx */
{
	int hightest_idx = -1;
	int len = Length();
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
BigInt operator+(const BigInt& X,const BigInt& Y)
{
	BigInt Z;
	uint32 carry=0;
	uint32 xlen = X.Length();
	uint32 ylen = Y.Length();
	for (uint32 i=0;i<std::max<uint32>(xlen,ylen);i++)
	{
		uint32 x = X.GetRadixBits(i);
		uint32 y = Y.GetRadixBits(i);
		uint64 z = (uint64)x + y + carry;
		carry = (uint32)(z/X.RADIX);
		z = z % X.RADIX;
		Z.AddRadixBits((uint32)z,i);
		Z.AddRadixBits(carry,i+1);
	}
	return Z;
}

BigInt BigDiv( const BigInt& X,const BigInt& Y,BigInt &Q,BigInt&R )
{
	BigInt One("1");
	BigInt Result("0");
	BigInt a = X;
	BigInt b = Y;

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
		BigInt shift_b = b<<diff;
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