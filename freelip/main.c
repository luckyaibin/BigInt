#include "lip.h"
//#pragma comment(lib,"ws2_32.lib") 
//#pragma comment(lib,"wsock32.lib") 

change_verylong(
				verylong verylong_a
				)
{
	zwriteln(verylong_a);
	zsadd(verylong_a,1,&verylong_a);
	zwriteln(verylong_a);
}

change_long(
			long long_a
			)
{
	printf("%ld\n",long_a);
	long_a ++;
	printf("%ld\n",long_a);
}

main () {
	long a = 5;
	verylong b = 0;
	zintoz(5,&b);
	change_long(a);
	printf("%ld\n",a);
	change_verylong(b);
	zwriteln(b);
}