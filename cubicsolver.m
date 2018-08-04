function result = cubicsolver(a, b, c, d)

%	result = -b/(3*a) +	nthroot( -b*b*b/(27*a*a*a) + b*c/(6*a*a) - d/(2*a) + sqrt( (-b*b*b/(27*a*a*a) + b*c/(6*a*a) - d/(2*a))^2 + (c/(3*a) - b*b/(9*a*a))^3 ),3) + nthroot( -b*b*b/(27*a*a*a) + b*c/(6*a*a) - d/(2*a) - sqrt( (-b*b*b/(27*a*a*a) + b*c/(6*a*a) - d/(2*a))^2 + (c/(3*a) - b*b/(9*a*a))^3 ),3);
	
	x1 = -b/(3*a);
	x2 = -b*b*b/(27*a*a*a) + b*c/(6*a*a) - d/(2*a);
	x3 = sqrt( x2^2 + (c/(3*a) - b*b/(9*a*a))^3 );
	
	result = x1 + nthroot(x2 + x3, 3) + nthroot(x2 - x3, 3);