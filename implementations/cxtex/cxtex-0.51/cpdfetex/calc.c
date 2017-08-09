
#include "types.h"
#include "c-compat.h"

#include "globals.h"

/* module 100 */

/* Here is a routine that calculates half of an integer, using an
 * unambiguous convention with respect to signed odd numbers.
 */
integer
half (int x) {
  if (odd (x)) {
	return (x + 1) / 2;
  } else {
	return x / 2;
  }
};


/* module 102 */

/* The following function is used to create a scaled integer from a given decimal
 * fraction $(.d_0d_1\ldots d_{k-1})$, where |0<=k<=17|. The digit $d_i$ is
 * given in |dig[i]|, and the calculation produces a correctly rounded result.
 */
scaled
round_decimals (small_number k) {  /* converts a decimal fraction */
  integer a;  /* the accumulator */
  a = 0;
  while (k > 0) {
	decr (k);
	a = (a + dig[k] * two) / 10;
  };
  return (a + 1) / 2;
};



/* module 104 */

/* Physical sizes that a \TeX\ user specifies for portions of documents are
 * represented internally as scaled points. Thus, if we define an `sp' (scaled
 * 
 * point) as a unit equal to $2^{-16}$ printer's points, every dimension
 * inside of \TeX\ is an integer number of sp. There are exactly
 * 4,736,286.72 sp per inch. Users are not allowed to specify dimensions
 * larger than $2^{30}-1$ sp, which is a distance of about 18.892 feet (5.7583
 * meters); two such quantities can be added without overflow on a 32-bit
 * computer.
 * 
 * The present implementation of \TeX\ does not check for overflow when
 * 
 * dimensions are added or subtracted. This could be done by inserting a
 * few dozen tests of the form `\ignorespaces|if x>= oct(10000000000) then
 * @t\\{report\_overflow}@>|', but the chance of overflow is so remote that
 * such tests do not seem worthwhile.
 * 
 * \TeX\ needs to do only a few arithmetic operations on scaled quantities,
 * other than addition and subtraction, and the following subroutines do most of
 * the work. A single computation might use several subroutine calls, and it is
 * desirable to avoid producing multiple error messages in case of arithmetic
 * overflow; so the routines set the global variable |arith_error| to |true|
 * instead of reporting errors directly to the user. Another global variable,
 * |remainder|, holds the remainder after a division.
 */

boolean arith_error; /* has arithmetic overflow occurred recently? */ 
scaled tex_remainder; /* amount subtracted to get an exact division */

/* module 105 */

/* The first arithmetical subroutine we need computes $nx+y$, where |x|
 * and~|y| are |scaled| and |n| is an integer. We will also use it to
 * multiply integers.
 */
scaled
mult_and_add (int n, scaled x, scaled y, scaled max_answer) {
  if (n < 0) {
	negate (x);
	negate (n);
  };
  if (n == 0) {
	return y;
  } else {
	if (((x <= (max_answer - y) / n) && (-x <= (max_answer + y) / n))) {
	  return n * x + y;
	} else {
	  arith_error = true;
	  return 0;
	};
  };
};


/* module 106 */

/* We also need to divide scaled dimensions by integers.
 */
scaled
x_over_n (scaled x, int n) {
  boolean negative;		/* should |tex_remainder| be negated? */
  scaled x_over_n_ret;
  negative = false;
  if (n == 0) {
	arith_error = true;
	x_over_n_ret = 0;
	tex_remainder = x;
  } else {
	if (n < 0) {
	  negate (x);
	  negate (n);
	  negative = true;
	};
	if (x >= 0) {
	  x_over_n_ret = x / n;
	  tex_remainder = x % n;
	} else {
	  x_over_n_ret = -((-x) / n);
	  tex_remainder = -((-x) % n);
	};
  };
  if (negative)
    negate (tex_remainder);
  return x_over_n_ret;
};


/* module 107 */

/* Then comes the multiplication of a scaled number by a fraction |n/d|,
 * where |n| and |d| are nonnegative integers |<=@t$2^{16}$@>| and |d| is
 * positive. It would be too dangerous to multiply by~|n| and then divide
 * by~|d|, in separate operations, since overflow might well occur; and it
 * would be too inaccurate to divide by |d| and then multiply by |n|. Hence
 * this subroutine simulates 1.5-precision arithmetic.
 */
scaled
xn_over_d (scaled x, int n, int d) {
  boolean positive;		/* was |x>=0|? */
  nonnegative_integer t, u, v;	/* intermediate quantities */
  if (x >= 0) {
	positive = true;
  } else {
	negate (x);
	positive = false;
  };
  t = (x % 32768) * n;
  u = (x / 32768) * n + (t / 32768);
  v = (u % d) * 32768 + (t % 32768);
  if (u / d >= 32768) {
	arith_error = true;
  } else {
	u = 32768 * (u / d) + (v / d);
  }
  if (positive) {
	tex_remainder = v % d;
	return u;
  } else {
	tex_remainder = -(v % d);
	return -u;
  };
};


/* module 108 */

/* The next subroutine is used to compute the ``badness'' of glue, when a
 * total~|t| is supposed to be made from amounts that sum to~|s|. According
 * to {\sl The \TeX book}, the badness of this situation is $100(t/s)^3$;
 * however, badness is simply a heuristic, so we need not squeeze out the
 * last drop of accuracy when computing it. All we really want is an
 * approximation that has similar properties.
 * 
 * The actual method used to compute the badness is easier to read from the
 * program than to describe in words. It produces an integer value that is a
 * reasonably close approximation to $100(t/s)^3$, and all implementations
 * of \TeX\ should use precisely this method. Any badness of $2^{13}$ or more is
 * treated as infinitely bad, and represented by 10000.
 * 
 * It is not difficult to prove that $$\hbox{|badness(t+1,s)>=badness(t,s)
 * >=badness(t,s+1)|}.$$ The badness function defined here is capable of
 * computing at most 1095 distinct values, but that is plenty.
 */
halfword
badness (scaled t, scaled s) { 	/* compute badness, given |t>=0| */
  int r;  /* approximation to $\alpha t/s$, where $\alpha^3\approx 100\cdot2^{18}$ */
  if (t == 0) {
	return 0;
  } else if (s <= 0) {
	return inf_bad;
  } else if (t <= 7230584) {
	r = (t * 297) / s;	/* $297^3=99.94\times2^{18}$ */
  } else if (s >= 1663497) {
	r = t / (s / 297);
  } else {
	r = t;
  }
  if (r > 1290) {
	return inf_bad;		/* $1290^3<2^{31}<1291^3$ */
  } else {
	return (r * r * r + 131072) / 262144;	/* that was $r^3/2^{18}$, rounded to the nearest integer */
  };
};


/* module 656 */
integer fix_int (int val, int min, int max) {
  if (val < min) {
	return min;
  } else if (val > max) {
	return max;
  } else {
	return val;
  };
};


/* module 663 */

/* The following function divides |s| by |m|. |dd| is number of decimal digits.
 */

scaled
divide_scaled (scaled s, scaled m, int dd) {
  scaled q, r;
  int sign, i;
  sign = 1;
  if (s < 0) {
	sign = -sign;
	s = -s;
  };
  if (m < 0) {
	sign = -sign;
	m = -m;
  };
  if (m == 0) {
	pdf_error_string("arithmetic","divided by zero");
  } else {
	if (m >= (max_integer / 10))
	pdf_error_string("arithmetic", "number too big");
  };
  q = s / m;
  r = s % m;
  for (i = 1; i <= dd; i++) {
	q = 10 * q + (10 * r) / m;
	r = (10 * r) % m;
  };
  if (2 * r >= m) {
	incr (q);
	r = r - m;
  };
  scaled_out = sign * (s - (r / ten_pow[dd]));
  return sign * q;
};

scaled
round_xn_over_d (scaled x, int n, int d) {
  boolean positive;		/* was |x>=0|? */
  nonnegative_integer t, u, v;	/* intermediate quantities */
  if (x >= 0) {
	positive = true;
  } else {
	negate (x);
	positive = false;
  };
  t = (x % 32768) * n;
  u = (x / 32768) * n + (t / 32768);
  v = (u % d) * 32768 + (t % 32768);
  if (u / d >= 32768) {
	arith_error = true;
  } else {
	u = 32768 * (u / d) + (v / d);
  };
  v = v % d;
  if (2 * v >= (unsigned)d)
    incr (u);
  if (positive) {
	return u;
  } else {
	return -u;
  };
};

/* module 1738 */

/* The function |add_or_sub(x,y,max_answer,negative)| computes the sum
 * (for |negative=false|) or difference (for |negative=true|) of |x| and
 * |y|, provided the absolute value of the result does not exceed
 * |max_answer|.
 */

integer
add_or_sub (int x, int y, int max_answer, boolean negative) {
  int a;			/* the answer */
  if (negative)
    negate (y);
  if (x >= 0) {
	if (y <= max_answer - x) {
	  a = x + y;
	} else {
	  num_error (a);
	};
  } else if (y >= -max_answer - x) {
	a = x + y;
  } else {
	num_error (a);
  }
  return a;
};

/* module 1742 */

/* The function |quotient(n,d)| computes the rounded quotient
 * $q=\lfloor n/d+{1\over2}\rfloor$, when $n$ and $d$ are positive.
 */
integer
quotient (int n, int d) {
  boolean negative; /* should the answer be negated? */
  int a; /* the answer */ 
  if (d == 0) {
    num_error (a) ;
  } else {
	if (d > 0) {
	  negative = false;
	} else {
	  negate (d);
	  negative = true;
	};
	if (n < 0) {
	  negate (n);
	  negative = !negative;
	};
	a = n / d;
	n = n - a * d;
	d = n - d; /* avoid certain compiler optimizations! */
	if (d + n >= 0)
	  incr (a);
	if (negative)
	  negate (a);
  };
  return a;
};

/* module 1744 */

/* Finally, the function |fract(x,n,d,max_answer)| computes the integer
 * $q=\lfloor xn/d+{1\over2}\rfloor$, when $x$, $n$, and $d$ are positive
 * and the result does not exceed |max_answer|. We can't use floating
 * point arithmetic since the routine must produce identical results in all
 * cases; and it would be too dangerous to multiply by~|n| and then divide
 * by~|d|, in separate operations, since overflow might well occur. Hence
 * this subroutine simulates double precision arithmetic, somewhat
 * analogous to \MF's |make_fraction| and |take_fraction| routines.
 */
integer
fract (int x, int n, int d, int max_answer) {
  boolean negative; /* should the answer be negated? */
  int a; /* the answer */ 
  int f; /* a proper fraction */ 
  int h; /* smallest integer such that |2*h>=d| */ 
  int r; /* intermediate remainder */ 
  int t; /* temp variable */ 
  if (d == 0)
    goto TOO_BIG;
  a = 0;
  if (d > 0)  {
	negative = false;
  } else {
	negate (d);
	negative = true;
  };
  if (x < 0) {
	negate (x);
	negative = !negative;
  } else if (x == 0) {
    goto DONE;
  }
  if (n < 0) {
	negate (n);
	negative = !negative;
  };
  t = n / d;
  if (t > max_answer / x)
    goto TOO_BIG;
  a = t * x;
  n = n - t * d;
  if (n == 0)
    goto FOUND;
  t = x / d;
  if (t > (max_answer - a) / n)
    goto TOO_BIG;
  a = a + t * n;
  x = x - t * d;
  if (x == 0)
    goto FOUND;
  if (x < n) {
	t = x;
	x = n;
	n = t;
  }; /* now |0<n<=x<d| */
  /* begin expansion of Compute \(f)$f=\lfloor xn/d+{1\over2}\rfloor$ */
  /* module 1745 */
  /* The loop here preserves the following invariant relations
   * between |f|, |x|, |n|, and~|r|:
   * (i)~$f+\lfloor(xn+(r+d))/d\rfloor=\lfloor x_0n_0/d+{1\over2}\rfloor$;
   * (ii)~|-d<=r<0<n<=x<d|, where $x_0$, $n_0$ are the original values of~$x$
   * and $n$.
   * 
   * Notice that the computation specifies |(x-d)+x| instead of |(x+x)-d|,
   * because the latter could overflow.
   */
  f = 0;
  r = (d / 2) - d;
  h = -r;
  loop {
    if (odd (n)) {
	  r = r + x;
	  if (r >= 0) {
	    r = r - d;
	    incr (f);
	  };
	};
    n = n / 2;
    if (n == 0)
      goto FOUND1;
    if (x < h) {
	  x = x + x;
	} else {
	  t = x - d;
	  x = t + x;
	  f = f + n;
	  if (x < n) {
	    if (x == 0)
	      goto FOUND1;
	    t = x;
	    x = n;
	    n = t;
	  };
	};
  };
 FOUND1:
  /* end expansion of Compute \(f)$f=\lfloor xn/d+{1\over2}\rfloor$ */
  if (f > (max_answer - a))
    goto TOO_BIG;
  a = a + f;
 FOUND:
  if (negative)
	negate (a);
  goto DONE;
 TOO_BIG:
  num_error (a);
 DONE:
  return a;
};

