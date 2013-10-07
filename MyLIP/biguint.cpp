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

		result_ = (v / 10); //result ÿ��������ӡ����ǡ�9*RADIX 
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

	res = (*this)<< (highest_bit_idx-bit_idx_hi);// ���ƣ��Ѹ���bit_idx_hi��bit���ӵ�
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

	//�ҵ���һ����Ϊ0 �ĸ�λ��ֵ������
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
			else // idx1 == idx2 ���� v1 == v2��Ҫ�ж���һλ
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


//N1 > N2,��ôN1���ȴ��ڵ���N2����
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
			//ע������ת�Ͳ���������v��Ľ����32λ��
			v = (uint64)n1*(uint64)n2 ;
			carry = v / BigUInt::RADIX;
			v = v % BigUInt::RADIX;

			//��ֵ�ͽ�λ�ӵ������
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
	BigUInt q;//��
	BigUInt r;//��
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
	//��������Ż�������remaind	> 0 �Ž�������Ĳ�����Ҳ�Ͳ�����int�ƶ�32bit֮���ڴ�Ϊ0������ֵ���������

	// 11110000 11110000 11110000 11110000 

	//����һλ
	// 01111000 01111000 01111000 01111000

	//�����ƶ�remaindλ��hi_mask�� 31 ~ remaind ��lo_mask �� remaind ~ 0λ
	uint32 hi_mask = 0xffffffff << remaind;
	uint32 lo_mask = (remaind == 0) ? 0 : 0xffffffff >>(32 - remaind);

	for (int j=0;remaind && j<result.ValidLength();j++)
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

BigUInt operator<<( const BigUInt& X,int bits )
{
	BigUInt result=X;

	int complete_ints = bits/32;
	int remaind = bits%32; //remaind 0~31

	if (complete_ints>0)
	{
		//��vectorͷ�����൱���������ұߣ�����complete_ints��0��Ҳ����������complete_ints*32λ
		result.m_bits.insert(result.m_bits.begin(),complete_ints,0);
	}
	//�ƶ���ɢ��bit:
	uint32 hi_mask = (remaind==0) ? 0 : (0xffffffff << (32 - remaind)); //��ΪremaindΪ0ʱ��ֵv����32λ��Ȼ��v
	uint32 lo_mask = 0xffffffff >> remaind;

	for(int idx = result.ValidLength();remaind && idx>=0;idx--)
	{
		uint32 hi_v = result.GetRadixBits(idx);
		uint32 lo_v = result.GetRadixBits(idx-1);

		uint32 new_hi_v = ( (hi_v & lo_mask) << remaind ) | ( ( lo_v & hi_mask ) >> (32 - remaind) );
		if (new_hi_v) //��������Ӷ���Ŀհ�uint32
		{
			result.SetRadixBits(new_hi_v,idx);
		}
	}
	return result;
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
	a.TrimHiZeros();
	BigUInt b = Y;
	b.TrimHiZeros();
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
		b =  a%b;
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
	BigUInt res("1");
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


BigUInt Exp(const BigUInt& a,const BigUInt& b)
{
	BigUInt res("1");
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
		sign = 1; //1��ʾ��
		numbers.erase(numbers.begin());//ɾ������
	}
	else if(numbers[0]=='+')
	{
		sign = 0;
		numbers.erase(numbers.begin());//ɾ������
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
				if (n==' ')//�����հ�
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
		printf("%d	ʮ����	:	%u \n", i,N.m_bits[i]);
		printf("%d	ʮ������:	0x%x \n",i,N.m_bits[i]);
		std::string s;
		for (int j=0;j<32;j++)
		{
			int v = (N.m_bits[N.m_bits.size() - 1 - i]>>(31-j))&1;
			char c = v + '0';
			s +=c;
		}
		printf("%d	������:	%s	\n",N.m_bits.size() - i,s.c_str());
		fs.write(s.c_str(),s.length());
	}
	printf("\n");
	fs.write("\n\r",1);
	fs.close();
}
