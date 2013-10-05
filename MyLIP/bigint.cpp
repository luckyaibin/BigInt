#include "bigint.h"
#include<fstream>
#include <stdarg.h>
const  bool BigInt::NeedCarry( uint64 result )
{
	return result>CARDINAL;
}

int BigInt::Length()const
{
	return m_bits.size();
}
int BigInt::ValidLength() const
{
	return (m_bits.size()>0)?(GetNonZeroIdx()+1):(0); 
}

void BigInt::TrimHiZeros()
{
	int v = 0;
	while ( (v=m_bits[m_bits.size()-1]) == 0 )
	{
		m_bits.erase(m_bits.begin()+m_bits.size()-1);
	}
	if (m_bits.size()==0)
	{
		m_bits.push_back(0);
	}
}




void BigInt::AddRadixBits( uint32 val,uint32 pos )
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

	//�ѽ�λ����ȥ
	AddRadixBits(carry,pos+1);
}

//��֤����
void BigInt::MinusRadixBits(uint32 val,uint32 pos)
{
	if (val==0)
	{
		return;
	}

	uint64 v = m_bits[pos];

	//������
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

	//���λ�н�λ
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
	//�ӵ����λ
	uint64 v = m_bits[0];
	v = v + ui32;

	carry = v / RADIX;
	v = v % RADIX;

	m_bits[0] = (uint32)v;

	int pos = 1;
	//��������λ����Ľ�λ
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
		printf("%d	ʮ����	:	%u \n", i,m_bits[i]);
		printf("%d	ʮ������:	0x%x \n",i,m_bits[i]);
		std::string s;
		for (int j=0;j<32;j++)
		{
			int v = (m_bits[m_bits.size() - 1 - i]>>(31-j))&1;
			char c = v + '0';
			s +=c;
		}
		printf("%d	������:	%s	\n",m_bits.size() - i,s.c_str());
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
		uint64 quotient =0;//��
		uint64 remainder=0;//����


		//ÿһ����ɣ��õ�һ����������10���������� remainder
		//��RADIX���Ƶ���תΪ10�����ַ���
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

		//һ�ֽ����������һ������
		char cc = remainder+'0';
		numbers =  cc + numbers;
		//��ʼ��һ��
		BigInt next_round(quotient_str.c_str());
		index = 0;
		curr_number_bits = next_round.m_bits;
	}
	return numbers;
}


int32 BigInt::GetNonZeroBitIdx() /*get_non_zero_bit_idx */
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
int32 BigInt::GetNonZeroIdx() const
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
BigInt BigInt::GetBitRangBigInt(int bit_idx_hi,int bit_idx_lo) const
{
	//assert(bit_idx_hi>=0 && bit_idx_lo>=0 &&"invalid index for GetBitRangeBigInt()!");
	if ( bit_idx_hi< 0 ||  bit_idx_lo<0 )
	{
		printf("stop.");
	}
	BigInt res;

	int highest_bit_idx = this->Length()*32 - 1;// this->GetNonZeroBitIdx();

	res = (*this)<< (highest_bit_idx-bit_idx_hi);// ���ƣ��Ѹ���bit_idx_hi��bit���ӵ�
	res = (res)>>( (highest_bit_idx-bit_idx_hi) + bit_idx_lo);
	return res;
}
BigInt operator+(const BigInt& X,const BigInt& Y)
{
	BigInt Z;
	BigInt Smaller;
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
		//uint32 x = X.GetRadixBits(i);
		//uint32 y = Y.GetRadixBits(i);
		//uint64 z = (uint64)x + y + carry;
		//carry = (uint32)(z/X.RADIX);
		//z = z % X.RADIX;
		Z.AddRadixBits((uint32)Smaller.GetRadixBits(i),i);
		//Z.AddRadixBits(carry,i+1);
	}
	return Z;
}

/*
��������ʽ�ĳ���
*/
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
		//a.Dump("a��ʼֵ:");
		//b.Dump("b��ʼֵ,�²�ƫ��diff=%d:",diff);
		BigInt shift_b = b<<diff;
		while (diff>0 && a < shift_b)
			diff--,shift_b = b<<diff;

		//if ( a >= shift_b )
		{
			//( b<<diff ).Dump("b�ƶ���ʵ��ƫ��diff=%d",diff);
			Result = Result + ( One <<diff );
			//Result.Dump("�����");
		}
		
		a = a - shift_b;
		loopcnt++;
		//a.Dump("֮��a:");
	}

	Q = Result;
	R = a;
	return Result;
}
/*
0x100000000���Ƶĳ���
�����õ��Ĺ�ֵ�㷨Ҫ�������صĵڶ�������Ҳδ��ȫ���
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

 ���ڰ�b���루��29�����λ>=5����ͨ����λ(����2^n)

        9..
    =======
58..| 4758..
      522 > ��һ�ε����̣�q = 47/5 == 9����������,��ôq--;
	 -----
	
(          
      =======
 58.. | 5858  ��һ�����̣�q=58 / 5 == 
)

        82
    =======
 58 | 4758
      464 > ��һ�ε����������㣬�����
	 -----
	   118  �ڶ��ε�����(q = 11/5 == 2),����
	   116
    -------
	     2
*/

BigInt Fast_BigDiv(const BigInt& X,const BigInt& Y,BigInt&Q,BigInt&R)
{
	BigInt Result("0");
	BigInt a = X;
	BigInt b = Y;
	int adjust_shift = 0;
	//����b,��b�����λ >= RADIX/2
	while ( b.GetRadixBits(b.GetNonZeroIdx()) < (BigInt::RADIX/2))
	{
		b=(b<<1);
		adjust_shift++;
	}
	//����b��ͬʱ��ҲҪ����a
	if(adjust_shift>0)
	{
		a = a<<adjust_shift;
	} 
	int a_begin_idx = a.GetNonZeroIdx();
	int a_valid_len = a.ValidLength();
	int b_begin_idx = b.GetNonZeroIdx();
	int b_valid_len = b.ValidLength();

	//a�ĳ��ȵ���b�ĳ��ȣ�Ҫ��a����ǰ�����0����֤a�ĳ��ȱ�b���ٳ�1
	if(a_valid_len==b_valid_len)
	{
		a.m_bits.push_back(0);
		a_begin_idx++;//ǰ��һλ����ǰ���0��ʼ
	}
	while(a>=b)
	{
		//��a��λȡ����b����+1��uint32�� ���λ����uit32�����bit������λ����uint32�����bit
		//����a�����Ƕ�����ģ�����a�����λ��uint32������0���������ܶ�ط�������Length��������ValidLength
		BigInt a_part = a.GetBitRangBigInt(a_begin_idx*32+31,(a_begin_idx-b_valid_len)*32);

		//ȡ��a�ĸ�2λ��2��uint32����b�ĸ�1λ��1��uint32�������̹�ֵ
		uint64 a_part_hi_2 = (uint64)a_part.GetRadixBits(a_part.Length() - 1) << 32 | (uint64)a_part.GetRadixBits(a_part.Length() - 2);

		uint64 b_part_hi_1 = b.GetRadixBits(b.GetNonZeroIdx());

		uint64 guess_v = a_part_hi_2 / b_part_hi_1;

		int guess_time = 0;
		while (a_part< b*guess_v)//guess_v������ʵֵ<2,���� guess_v-2<= real_v <= guess_v;��ε��Ƶ�Ҫ�����ɵµڶ���
		{
			guess_v--;
			guess_time++;
		}
		//�˺��guess_v������ʵ��ֵ		
		assert(guess_time<=2);
		
		uint32 carry = guess_v / BigInt::RADIX;
		uint32 value = guess_v % BigInt::RADIX;
		
		Result.AddRadixBits(value,a_begin_idx-b_valid_len);
		Result.AddRadixBits(carry,a_begin_idx-b_valid_len+1);
		
		//����carry�п���>=0��Ҫ��uint64λ�Ĺ��캯��
		a = a - b*(BigInt(guess_v)<<(a_begin_idx-b_valid_len)*32);

		a_begin_idx--;
	}

	Q = Result;
	R = a>>adjust_shift;
	return Result;
}
//ŷ������㷨����X��Y�����Լ��
BigInt GCD(const BigInt& X,const BigInt& Y)
{
	BigInt Zero("0");
	BigInt a;
	BigInt b;
	BigInt r;
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
	else //��ȣ�ֱ�ӷ���
	{
		return X;
	}

	while (!( b == Zero) ) //a=41606343 b=40144455
	{
		BigInt tmp = b;
		b = Mod(a,b);
		a = tmp;
	}
	return a;		
}
/*

	gcd(a,b) = gcd(b,a%b); // ����

	�裺
    a*x1 + b*y1 = gcd(a,b)
    
	//�� a= b, b = a % b; ��һ�εݹ�
	b * x2 + ( a % b ) * y2 = gcd(b,a%b);
	


*/

void ExEuclid( BigInt a, BigInt b,BigInt& x,BigInt&y )
{
	BigInt old_a = a;
	BigInt old_b = b;

	//r Ϊ a % b ��ģ��Ҳ��a / b ������
	//q Ϊ a / b ����
	//����һ���Զ������
	BigInt r;
	BigInt q;
	BigInt tmp = b;

	Fast_BigDiv(a,b,q,r);

	b = r;
	a = tmp;

	if (b == BigInt("0"))
	{
		x = BigInt("1");
		y = BigInt("0");

		BigInt tmp = x;
		x = y;
		y = tmp - q*y;
		return;
	}

	ExEuclid(a,b,x,y);

	BigInt old_x = x;
	x = y;
	y = old_x - q*x;
}

BigInt ExpMod(const BigInt& a,const BigInt& b,const BigInt& m)
{
	BigInt res("1");;
	BigInt multiplizer = a;
	BigInt exp = b;
	BigInt Zero("0");
	while (exp > Zero)
	{
		//ȡ��һ�������ֽ�
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