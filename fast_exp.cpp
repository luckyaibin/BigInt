/*
	int a = 7;
	int b = 11; 1011

	a^b = a^( bn * 2^n + bn-1 * 2^(n-1) + ... +  b1 * 2^1 + b0 * 2^0 )
		= a^( bn * 2^n)    *    a^(bn-1 * 2^(n-1)    * ...  * a^(b1 * 2^1) *  a^(b0 * 2^0)

	bn 为 0，或者1
	对于 a ^ b :
	a^11 = a^(2^3) * a^(2^1) * a^(2^0)

*/

int modular_exp(int a,int b,int n=0)
{
	int res = 0;
	int multiplier = a;
	while(b!=0)
	{
		if(b & 1)
		{
			
		}
		b >>=1;
	}
	return a;
}
int main()
{
	modular_exp(7,11);
}
