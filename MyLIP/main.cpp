#include "bigint.h"


int main__()
{
	// | 0000 0001 | ffff ffff | 
	//uint64 i = 0x1ffffffff;		//8589934591;
	
	uint32 number[] = {0x00000001,0x00000000,0x00000000};//18446744073709551616(10进制)
  //uint32 number[] = {0x00000000,0xffffffff,0xffffffff};//18446744073709551615(10进制)
	uint32 number_size= sizeof(number)/sizeof(uint32);
 
	uint64 quotient =0;//商
	uint64 remainder=0;//余数
	uint32 index = 0;

	uint32 number2[3] = {0};
	do
	{
		uint64 v = 0;
		do 
		{
			v = number[index];
			v = v + remainder * 0x0000000100000000;
			quotient = v / 10;
			remainder =  v % 10;

			printf("%lld,%lld	\n",quotient,remainder);

			number[index] = quotient;

		} while ( ++index< number_size );//remainder>0);

		index = 0;

	}while(remainder);

	return 0;
}

//获得最高位不为0的bit的序数: 63 ~ 0
uint32 get_non_zero_bit_idx(uint64 v)
{
	for (int idx=63;idx>=0;idx--)
	{
		if ( (v>>idx) & 1)
		{
			return idx;
		}
	}
	return -1;
}

//v : 01110000 11110000 00001111 11110000
//d :          00001111 11110000 11110000 
/*
 110
   1
-----
 110
*/
//v 被除数 d 除数 q 商 r 余数
uint64  binary_div(uint64 a,uint64 b,uint64 &q,uint64&r)
{
	int i = 0;
	int na = 0;
	int nb = 0;
	uint64 c = 0;
	i = get_non_zero_bit_idx(a);
	nb = get_non_zero_bit_idx(b);
	while (i>=0)
	{	
		c = c << 1;
		
		na = get_non_zero_bit_idx(a);
		int diff = na - nb;
		if (diff<0)
		{
			diff = 0;
		}
		if (a >= (b<<diff) )
		{
			c = c + 1;
			a = a - (b<<diff);
		}
		i--;
	} ;
	q = c;
	r = a;
	return c;
}

int main()
{
	

	//BigInt ii("4294967295");
	//BigInt iii("4294967296");
	BigInt   iiii("18446744065119617025");
	BigInt  iiiii("18446744073709551615");
	BigInt iiiiii("18446744073709551616");
	BigInt iiiiiii("184467440737095516161844674407370955161618446744073709551616184467440737095516161844674407370955161618446744073709551616");

	iiiiiii.Dump();
	iiiiiii = (iiiiiii<<32);
	iiiiiii.Dump();
	

	uint64 u64 = 1;
	u64 = u64 << 33;

	uint32 u32 = 1;
	u32 = u32 << 33;

	uint64 q,r;

	for ( int i=0;i<=100;i++)
	{
		int v = rand()%1000 + 1;
		int d = rand() % 100 + 1;
		std::cout<<v<<" "<<d<<" "<< (v / d) <<" : "<< binary_div(v,d,q,r)<<std::endl;
	}
	binary_div(310,17,q,r);

	return 0;
}