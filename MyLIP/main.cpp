#include "biguint.h"
#include "bigint.h"
/*
	int a = 7;
	int b = 11; 1011

	a^b = a^( bn * 2^n + bn-1 * 2^(n-1) + ... +  b1 * 2^1 + b0 * 2^0 )
		= a^( bn * 2^n)    *    a^(bn-1 * 2^(n-1)    * ...  * a^(b1 * 2^1) *  a^(b0 * 2^0)

	bn 为 0，或者1
	对于 a ^ b :
	a^11 = a^(2^3) * a^(2^1) * a^(2^0)

	设 cn = a^(2^n);
	则 cn+1 = a^(2^(n+1)) 
	= a^(2^(n+1)) 
	= a^(2^n * 2) 
	= a^(2^n + 2^n) 
	= a^(2^n) *  a^(2^n)  
	= ( a^(2^n) ) ^ 2
	= (cn)^2
	可见，cn+1 和 cn形成了这种递推关系

	那么 a^b = cn * cn-1 ... c1 * c0
		

http://blog.csdn.net/hkdgjqr/article/details/5381292 
http://blog.csdn.net/hkdgjqr/article/details/5381028
*/
int modular_exp(int a,int b,int m=0)
{
	int res = 1;
	int multiplier = a;
	while(b!=0)
	{
		if(b & 1)
		{
			res = (res * multiplier)%m;
		}
		multiplier = (multiplier * multiplier)%m;
		b >>=1;
	}
	return res;
}
//1024 = 2^10
//递归版，分治法，次数更少，应该比modular_exp快，待测
int rec_modular_exp(int a,int b,int m=0)
{
	if (b==0)
	{
		return 1;
	}
	if (b%2)
	{
		return (a*rec_modular_exp(a,b-1,m))%m;
	}
	int tmp = rec_modular_exp(a,b/2,m);
	return (tmp*tmp)%m;
}

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

/*
  v = 1894780912
  d = 1044720
  v/d = 1813.6734359446

  1813 二进制为 11100010101

  v : 01110000 11110000 00001111 11110000 na = 30  : 1894780912
  d :              1111 11110000 11110000 nb = 19  :    1044720
  d':  1111111 10000111 10000000 00000000  d<<11   : 2139586560
	  -----------------------------------  不够减
	  
	  c = 0;

  v : 01110000 11110000 00001111 11110000 na = 30 : 1894780912
  d :              1111 11110000 11110000 nb = 19 :    1044720
  d':   111111 11000011 11000000 00000000   d<<10 : 1069793280
	  -----------------------------------  够减   :  
	  c = 1;

  v :   110001 00101100 01001111 11110000 na = 29 :  824987632
  d :              1111 11110000 11110000 nb = 19 :    1044720		
  d':   111111 11000011 11000000 00000000 d<<10   : 1069793280
	  -----------------------------------  不够减 :        
	 c = 0;

  v :   110001 00101100 01001111 11110000 na = 29 :  824987632
  d :              1111 11110000 11110000 nb = 19 :    1044720		
  d':    11111 11100001 11100000 00000000 d<<9   :   534896640
  -----------------------------------  够减 :    
	c = 1;

  v :    10001 01001010 01101111 11110000 na = 28 :  290090992
  d :              1111 11110000 11110000 nb = 19 :    1044720		
  d':     1111 11110000 11110000 00000000 d<<9   :   534896640
  -----------------------------------  不够减 :    
    c = 0;

  v :    10001 01001010 01101111 11110000 na = 28 :  290090992
  d :              1111 11110000 11110000 nb = 19 :    1044720		
  d':      111 11111000 01111000 00000000 d<<8   :   267448320
  -----------------------------------  够减 :    
    c = 1;

  v :        1 01011001 01111111 11110000 na = 24 :   22642672
  d :              1111 11110000 11110000 nb = 19 :    1044720		
  d':        1 11111110 00011110 00000000  d<<5   :   33431040
  -----------------------------------  不够减 :    
    c = 0;

  v :        1 01011001 01111111 11110000 na = 24 :   22642672
  d :              1111 11110000 11110000 nb = 19 :    1044720		
  d':          11111111 00001111 00000000  d<<4   :   16715520
  -----------------------------------  够减 :   
  c = 1;

  v :           1011010 01110000 11110000 na = 22 :    5927152
  d :              1111 11110000 11110000 nb = 19 :    1044720		
  d':           1111111 10000111 10000000  d<<3   :    8357760
  -----------------------------------  不够减 :  
  c = 0;

  v :           1011010 01110000 11110000 na = 22 :    5927152
  d :              1111 11110000 11110000 nb = 19 :    1044720		
  d':            111111 11000011 11000000  d<<2   :    4178880
  -----------------------------------  够减 :  
  c = 1;

  v :             11010 10101101 00110000 na = 20 :    1748272
  d :              1111 11110000 11110000 nb = 19 :    1044720		
  d':             11111 11100001 11100000  d<<1   :    2089440
  -----------------------------------  不够减 :  
  c = 0;

  v :             11010 10101101 00110000 na = 20 :    1748272
  d :              1111 11110000 11110000 nb = 19 :    1044720		
  d':              1111 11110000 11110000  d<<0   :    1044720
  -----------------------------------  够减 :  
  c = 1;

  v :              1010 10111100 01000000 na = 19 :     703552
  d :              1111 11110000 11110000 nb = 19 :    1044720		
  d':              1111 11110000 11110000  d<<0   :    1044720
  -----------------------------------  不够减 :  
  c = 0;

*/
//v 被除数 d 除数 q 商 r 余数
uint64  binary_div(int64 a,int64 b,uint64 &q,uint64&r)
{
	int na = 0;
	int nb = 0;
	int c = 0;
	na = get_non_zero_bit_idx(a);
	nb = get_non_zero_bit_idx(b);

	while (a>=b) // so that diff >= 0
	{
		na = get_non_zero_bit_idx(a);
		int diff = na - nb;
		
		while(diff>0 && a<(b<<diff)) //so that a >= b<<diff
			diff--;
		if (a >= (b<<diff) )
		{
			c = c + (1<<diff);	
		}				
		a = a - (b<<diff);
	}
	q = c;
	r = a;
	return c;
}

//http://blog.csdn.net/wangjian8006/article/details/7833228
//http://blog.sbw.so/Article/index/title/%E6%89%A9%E5%B1%95%E6%AC%A7%E5%87%A0%E9%87%8C%E5%BE%B7%E7%AE%97%E6%B3%95%E7%9A%84%E5%8E%9F%E7%90%86%E4%B8%8E%E5%AE%9E%E7%8E%B0.html

int g_x,g_y;
//euclid算出来的是 a*x + b*y = gcd(a,b)的解
//对于a 和 b互质的情况，就是a*x + b*y = gcd(a,b) === 1的解
void euclid(int a,int b)
{
	int old_a = a;
	int old_b = b;
	int tmp = b;
	b = a % b;
	a = tmp;
	if(b == 0)
	{
		g_x = 1;
		g_y = 0;

		//最后一次递归返回时，顺带把出了赋予初始值之外的
		//第一层x y之间的递推也计算出来。
		int tmp = g_x;
		g_x = g_y;
		g_y = tmp - (old_a/old_b)*g_y;
		return;
	}
	
	euclid(a,b);
	
	//计算第一次
	int old_x = g_x;
	g_x = g_y;
	g_y = old_x - (old_a/old_b)*g_y;
}
int main()
{
	BigUInt e_a = "3490529510847650949147849619903898133417764638493387843990820577";
	BigUInt e_b = "32769132993266709549961988190834461413177642967992942539798288533";

	
	BigUInt rsa_129 = e_a*e_b;
	DumpBits(rsa_129,"rsa_129:");

	BigUInt rsa_1290 = "114381625757888867669235779976146612010218296721242362562561842935706935245733897830597123563958705058989075147599290026879543541";
	DumpBits(rsa_1290,"rsa_1290:");

	std::string rsa_129_str = "114381625757888867669235779976146612010218296721242362562561842935706935245733897830597123563958705058989075147599290026879543541";


	BigUInt test = rsa_129 % BigUInt("10000000000");
	DumpBits(test,"使用bit除法:%s",test.ToString().c_str());
	 for (int i=1;i<120;i++)
	{
		std::string ten_NPower = "1";

		ten_NPower.insert(ten_NPower.begin()+1,i,'0');

		BigUInt tmp(ten_NPower.c_str());
		
		BigUInt r = rsa_129 % tmp;

		DumpBits(r,"对%s取模之后\n余数 %s\n",ten_NPower.c_str(),r.ToString().c_str());
	}
	 
	return 0;
}

