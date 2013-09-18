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
	

	return 0;
}