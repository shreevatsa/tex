

#define odd(x)     ((x) & 1)
#define negate(x)  x  = (-x)
#define incr(a) ++(a);
#define decr(a) --(a);
#define num_error(arg) { arith_error = true; arg = 0 ;}

/* changed macros: */

#define NX_PLUS_Y(vara,varb,varc)  mult_and_add (vara,varb,varc,1073741823 ) 
#define MULT_INTEGERS(vara,varb)   mult_and_add (vara,varb,0 ,2147483647 )

#define FLOAT_CONSTANT(arg)        arg


EXTERN boolean arith_error;
EXTERN scaled tex_remainder;


EXTERN integer half (int x);
EXTERN scaled round_decimals (small_number k);
EXTERN scaled mult_and_add (int n, scaled x, scaled y, scaled max_answer);
EXTERN scaled x_over_n (scaled x, int n);
EXTERN scaled xn_over_d (scaled x, int n, int d);
EXTERN halfword badness (scaled t, scaled s);
EXTERN scaled divide_scaled (scaled s, scaled m, int dd);
EXTERN scaled round_xn_over_d (scaled x, int n, int d);
EXTERN integer add_or_sub (int x, int y, int max_answer, boolean negative); 
EXTERN integer quotient (int n, int d); 
EXTERN integer fract (int x, int n, int d, int max_answer); 
EXTERN integer fix_int (int val, int min, int max);

