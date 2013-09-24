
--a > = b
function gcd(a,b)

	--b<=aһֱ����
	while b~=0 do
		local t = a % b; --t �� a �� b �Ĳ��0<=t<=b;b>0
		a = b;
		b = t;
	end
	return a;
end

--[[

 ŷ����÷���:

		(A*B) % N = 1,��֪A��N����B��ʹ�÷��̳���
���� 	(A*B) - N * M = 1


�� �� A Ϊ a��BΪx��NΪb��MΪ y������Ϊ:
		a * x - b * y = 1(��B�������x)

  a * x - b * y = 1
 �� (a * x) mod b = 1


   �Ƶ��ǵݹ��㷨:
1. �� a*x1 + b*y1 = 1 = mod(a,b)

	��Ϊmod(a,b) === mod(b,a%b) ���������ĳ�εݹ�����У�������
	b ����a���� a%b ����b����:

    b*x2 + (a%b)*y2 = mod(b,a%b)


   ���ԣ� a*x1 + b*y1 === b*x2 + (a%b)*y2  = b*x2 + ( a - b*(a/b))* y2 �����

   a*x1 + b*y1 === b*x2 + a*y2 - b*(a/b)*y2

   ��Ϊ��ȣ�������:
   x1 = y2;
   y1 = x2 - (a/b)*y2;

   ��a=b ʱ����һ��ѭ��ʱb=0ʱ:
   a*xn + 0 = a;( mod(a,0) === a);
   (��ʵ���һ�Σ�a�͵���1)
   ��ʱxn = 1



 --]]
 -- http://www.cnblogs.com/zhtxwd/archive/2012/02/09/2344165.html
 -- http://www.cnblogs.com/frog112111/archive/2012/08/19/2646012.html
 -- http://www.cnblogs.com/void/archive/2011/04/18/2020357.html
function ex_euc(a,b)

	local x0 = 1;
	local y0 = 0;

	local x = x0;
	local y = y0;

	while b ~= 0 do

		local old_x = x;
		x = y;
		y = old_x - ( a/b) * y;

		local old_a;
		old_a = a;
		a = b % a;
		b = old_a;

	end

	if x < 0 then
		x = x + b;
	end

	return x,y;

end


print( gcd(100,8));


print( 1%1);

print(ex_euc(20,7));






















