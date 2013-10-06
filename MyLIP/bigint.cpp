#include "bigint.h"
#include<fstream>
#include <stdarg.h>
const  bool BigUInt::NeedCarry( uint64 result )
{
	return result>CARDINAL;
}

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
	while ( (v=m_bits[m_bits.size()-1]) == 0 )
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

	//�ѽ�λ����ȥ
	AddRadixBits(carry,pos+1);
}

//��֤����
void BigUInt::MinusRadixBits(uint32 val,uint32 pos)
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

	//���λ�н�λ
	if (carry > 0)
	{
		//m_bits.push_front(0);
		//m_bits.insert(m_bits.begin(),0);
		m_bits.push_back(0);
		m_bits[m_bits.size()-1] = (uint32)carry;
	}
}

void BigUInt::Add( const uint32& ui32 )
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

void BigUInt::Dump(const char *fmt,...) const
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

std::string BigUInt::ToString() const
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
		BigUInt next_round(quotient_str.c_str());
		index = 0;
		curr_number_bits = next_round.m_bits;
	}
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
	//assert(bit_idx_hi>=0 && bit_idx_lo>=0 &&"invalid index for GetBitRangeBigInt()!");
	if ( bit_idx_hi< 0 ||  bit_idx_lo<0 )
	{
		printf("stop.");
	}
	BigUInt res;

	int highest_bit_idx = this->Length()*32 - 1;// this->GetNonZeroBitIdx();

	res = (*this)<< (highest_bit_idx-bit_idx_hi);// ���ƣ��Ѹ���bit_idx_hi��bit���ӵ�
	res = (res)>>( (highest_bit_idx-bit_idx_hi) + bit_idx_lo);
	return res;
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
		//a.Dump("a��ʼֵ:");
		//b.Dump("b��ʼֵ,�²�ƫ��diff=%d:",diff);
		BigUInt shift_b = b<<diff;
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

BigUInt Fast_BigDiv(const BigUInt& X,const BigUInt& Y,BigUInt&Q,BigUInt&R)
{
	BigUInt Result("0");
	BigUInt a = X;
	BigUInt b = Y;
	int adjust_shift = 0;
	//����b,��b�����λ >= RADIX/2
	while ( b.GetRadixBits(b.GetNonZeroIdx()) < (BigUInt::RADIX/2))
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
		BigUInt a_part = a.GetBitRangBigInt(a_begin_idx*32+31,(a_begin_idx-b_valid_len)*32);

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
		//����carry�п���>=0��Ҫ��uint64λ�Ĺ��캯��
		//a = a - b*(BigInt(guess_v)<<(a_begin_idx-b_valid_len)*32);
		a = a - (b <<(a_begin_idx-b_valid_len)*32) * guess_v;

		a_begin_idx--;
	}

	Q = Result;
	R = a>>adjust_shift;
	return Result;
}
//ŷ������㷨����X��Y�����Լ��
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
	else //��ȣ�ֱ�ӷ���
	{
		return X;
	}

	while (!( b == Zero) ) //a=41606343 b=40144455
	{
		BigUInt tmp = b;
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

void ExEuclid( BigUInt a, BigUInt b,BigUInt& x,BigUInt&y )
{
	BigUInt old_a = a;
	BigUInt old_b = b;

	//r Ϊ a % b ��ģ��Ҳ��a / b ������
	//q Ϊ a / b ����
	//����һ���Զ������
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
	BigUInt res("1");;
	BigUInt multiplizer = a;
	BigUInt exp = b;
	BigUInt Zero("0");
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