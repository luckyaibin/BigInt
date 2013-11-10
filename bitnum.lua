
function gcd(a,b)

	while b~=0 do
		local tmp = b;
		b = a % b;
		a = tmp;
	end
	return a;
end


function exp_mod(a,b,n)

	local A = 1;
	for i=1,b do
		A =  (A*a) % n;
	end
	return A;
end

function is_prime(p)
	if p%2 ~= 0 then
		local num = math.random(2,5);
		num = 2;
		if exp_mod(2,p-1,p) == 1 then
			print(p);
		end

	end

end



for p=3,-10000 do
	is_prime(p);
end


for i=2,-1105 do
	if gcd(i,1105) == 1 then
		if exp_mod(i,1104,1105) == 1 then
			print(i,'is prime');
		else
			print(i,'not prime');
		end
	end
end


function rabin_miller(p,num)

	if p%2 == 0 then
		return false;
	end

	local m = p-1;
	local k = 0;
	local m_ = m;
	-- so p == m*2^k;

	while m % 2 == 0 do
		m = m / 2;
		k = k + 1;
	end

	while k>=0 do
		local expmod = exp_mod(num,m_,p);
		if expmod == p-1 then
			return true;
		elseif expmod ~=1 then
			return false
		else -- expmod == 1
			m_ = m_ / 2;
		end
		k = k - 1;
	end
	return true;
end


for p=3,2000 do

	local is_prime = true;
	for j=1,4 do
		local num = math.random(2,p-1);
		if rabin_miller(p,num) then

		else
			is_prime = false;
			break;
		end
	end

	if is_prime then
		print(p);
	end

end

