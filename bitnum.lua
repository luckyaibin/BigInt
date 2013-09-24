
--a > = b
function gcd(a,b)

	--b<=a一直成立
	while b~=0 do
		local t = a % b; --t 是 a 和 b 的差，且0<=t<=b;b>0
		a = b;
		b = t;
	end
	return a;
end

--[[

 欧几里得方程:

		(A*B) % N = 1,已知A和N，求B，使得方程成立
即求 	(A*B) - N * M = 1


设 另 A 为 a，B为x，N为b，M为 y，方程为:
		a * x - b * y = 1(求B就是求出x)

  a * x - b * y = 1
 即 (a * x) mod b = 1


   推导非递归算法:
1. 设 a*x1 + b*y1 = 1 = mod(a,b)

	因为mod(a,b) === mod(b,a%b) 恒成立，在某次递归过程中，可以用
	b 代替a，用 a%b 代替b，即:

    b*x2 + (a%b)*y2 = mod(b,a%b)


   所以： a*x1 + b*y1 === b*x2 + (a%b)*y2  = b*x2 + ( a - b*(a/b))* y2 恒成立

   a*x1 + b*y1 === b*x2 + a*y2 - b*(a/b)*y2

   因为恒等，所以有:
   x1 = y2;
   y1 = x2 - (a/b)*y2;

   当a=b 时，下一个循环时b=0时:
   a*xn + 0 = a;( mod(a,0) === a);
   (其实最后一次，a就等于1)
   此时xn = 1



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






















