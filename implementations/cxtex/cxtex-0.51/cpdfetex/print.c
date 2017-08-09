
#include "types.h"
#include "c-compat.h"


#include "globals.h"



unsigned char selector; /* where to print a message */ 
unsigned char dig[22]; /* digits in a number being output */ 
int tally; /* the number of characters recently printed */ 
unsigned int term_offset; /* the number of characters on the current  terminal line */ 
unsigned int file_offset; /* the number of characters on the current file line */ 

integer max_print_line; /* width of longest text lines output; should be at least 60 */ 

/* module 243 */
/*  Set variable |c| to the current escape character */
#define set_c_escape  c = escape_char

/* module 57 */
/* To end a line of text output, we call |print_ln|. */
void
print_ln (void) {  /* prints an end-of-line */
  switch (selector) {
  case term_and_log: 
	wterm_cr;
	wlog_cr;
	term_offset = 0;
	file_offset = 0;
	break;
  case log_only:
	wlog_cr;
	file_offset = 0;
	break;
  case term_only:
	wterm_cr;
	term_offset = 0;
	break;
  case no_print:
  case pseudo:
  case new_string:
	do_something;
	break;
  default:
	fprintf (write_file[selector],"\n");
  };
}; /* |tally| is not affected */

/* module 58 */

/* The |print_visible_char| procedure sends one character to the desired
 * destination, using the |Xchr| macro to map it into an external character
 * compatible with |input_ln|. It assumes that it is always called with a
 * visible ASCII character and that the special handling for the new-line
 * character is done in |print_char|. All printing comes through |print_ln|
 * or |print_char|, which ultimately calls |print_visible_char|.
 */
void
print_visible_char (ASCII_code s) {	/* prints a single character */
  /* label is not used but nonetheless kept (for other changes?) */
  switch (selector) {
  case term_and_log: 
	wterm_char (Xchr(s));
	wlog_char (Xchr(s));
	incr (term_offset);
	incr (file_offset);
	if (term_offset == (unsigned)max_print_line) {
	  wterm_cr;
	  term_offset = 0;
	};
	if (file_offset == (unsigned)max_print_line) {
	  wlog_cr;
	  file_offset = 0;
	};
	break;
  case log_only:
	wlog_char (Xchr(s));
	incr (file_offset);
	if (file_offset == (unsigned)max_print_line)
	  print_ln();
	break;
  case term_only:
	wterm_char (Xchr(s));
	incr (term_offset);
	if (term_offset == (unsigned)max_print_line)
	  print_ln();
	break;
  case no_print:
	do_something;
	break;
  case pseudo:
	if (tally < trick_count)
	  trick_buf[tally % error_line] = s;
	break;
  case new_string:
	if (pool_ptr < pool_size)
	  append_char (s);
	break;
  default:
	fprintf (write_file[selector], "%c", Xchr(s));
  };
  incr (tally);
};


/* module 59 */

/* The |print_char| procedure sends one character to the desired destination.
 * Control sequence names, file names and string constructed with
 * \.{\\string} might contain |ASCII_code| values that can't
 * be printed using |print_visible_char|. These characters will be printed
 * in three- or four-symbol form like `\.{\^\^A}' or `\.{\^\^e4}'.
 * Output that goes to the terminal and/or log file is treated differently
 * when it comes to determining whether a character is printable.
 */
#define print_lc_hex( arg )  l   =  arg ;                       \
       if (  l  < 10  )   { print_visible_char ( l  +  '0' ); } \
       else  { print_visible_char ( l  - 10  +  'a' ); }

void
print_char (ASCII_code s) {	/* prints a single character */
  ASCII_code k;
  unsigned char l;		/* small indices or counters */
  if (selector > pseudo) {
	print_visible_char (s);
	return;
  };
  if (s == new_line_char) {
	if (selector < pseudo)
	  {
		print_ln();
		return;
	  };
  };
  k = s;
  if (((selector <= no_print) && 
	   /* module 49 */
	   (char_cannot_be_printed)) 
	  || ((selector > no_print) && (!isprint (Xchr(k))))) {
	print_visible_char ('^');
	print_visible_char ('^');
	if (s < 64) {
	  print_visible_char (s + 64);
	} else {
	  if (s < 128) {
		print_visible_char (s - 64);
	  } else {
		print_lc_hex (s / 16);
		print_lc_hex (s % 16);
	  };
	}
  } else {
	print_visible_char (s);
  }
};

void
zprint_string (char *s) {/* prints string |s| */
  while (*s) {
    print_char (str_pool[(integer)*s]);
	s++;
  };
};


/* module 60 */

/* An entire string is output by calling |zprint|. Note that if we are outputting
 * the single standard ASCII character \.c, we could call |zprint("c")|, since
 * |"c"=99| is the number of a single-character string, as explained above. But
 * |print_char("c")| is quicker, so \TeX\ goes directly to the |print_char|
 * routine when it knows that this is safe. (The present implementation
 * assumes that it is always safe to zprint a visible ASCII character.)
 * 
 * Old versions of \TeX\ needed a procedure called |slow_print| whose function
 * is now subsumed by |zprint| and the new functionality of |print_char| and
 * |print_visible_char|. We retain the old name |slow_print| here as a
 * possible aid to future software arch\ae ologists.
 */
void
zprint (int s) { /* prints string |s| */
  pool_pointer j;	/* current character code position */
  if (s >= str_ptr) {
	zprint_string ("???");		/* this can't happen */
	return;
  } else {
	if (s < 256) {
	  if (s < 0) {
		zprint_string ("???"); /* can't happen */
		return;
	  } else {
		print_char (s);
		return;
	  };
	};
  };
  j = str_start[s];
  while (j < str_start[s + 1]) {
	print_char (str_pool[j]);
	incr (j);
  };
};

/* module 62 */

/* The procedure |print_nl| is like |zprint|, but it makes sure that the
 * string appears at the beginning of a new line.
 */

void
print_nl (str_number s) {	
  /* prints string |s| at beginning of line */
  if (((term_offset > 0) && (odd (selector)))
      || ((file_offset > 0) && (selector >= log_only)))
    print_ln();
  zprint (s);
};

void
print_nl_string (char *s) {	
  /* prints string |s| at beginning of line */
  if (((term_offset > 0) && (odd (selector)))
      || ((file_offset > 0) && (selector >= log_only)))
    print_ln();
  zprint_string (s);
};


/* module 63 */

/* The procedure |print_esc| prints a string that is preceded by
 * the user's escape character (which is usually a backslash).
 */
void
print_esc (str_number s) { /* prints escape character, then |s| */
  int c; /* the escape character code */
  /* module 243 */
  set_c_escape ;
  if (c >= 0)
    if (c < 256)
      zprint (c);
  slow_print (s);
};

void
print_esc_string (char * s) { /* prints escape character, then |s| */
  int c; /* the escape character code */
  /* module 243 */
  set_c_escape ;
  if (c >= 0)
    if (c < 256)
      zprint (c);
  zprint_string (s);
};


/* module 64 */

/* 
 * An array of digits in the range |0..15| is printed by |print_the_digs|.
 */
void
print_the_digs (eight_bits k) { 	/* prints |dig[k-1]|$\,\ldots\,$|dig[0]| */
  while (k > 0) {
	decr (k);
	if (dig[k] < 10) {
	  print_char ('0' + dig[k]);
	} else {
	  print_char ('A' - 10 + dig[k]);
	};
  };
};

/* module 65 */

/* The following procedure, which prints out the decimal representation of a
 * given integer |n|, has been written carefully so that it works properly
 * if |n=0| or if |(-n)| would cause overflow. It does not apply |mod| or |div|
 * to negative arguments, since such operations are not implemented consistently
 * by all \PASCAL\ compilers.
 */

void
print_int (int n) {	/* prints an integer in decimal form */
  unsigned char k; /* index to current digit; we assume that $|n|<10^{23}$ */ 
  int m; /* used to negate |n| in possibly dangerous cases */
  k = 0;
  if (n < 0) {
	print_char ('-');
	if (n > -100000000) {
	  negate (n);
	} else {
	  m = -1 - n;
	  n = m / 10;
	  m = (m % 10) + 1;
	  k = 1;
	  if (m < 10) {
		dig[0] = m;
	  } else {
		dig[0] = 0;
		incr (n);
	  };
	};
  };
  do {
	dig[k] = n % 10;
	n = n / 10;
	incr (k);
  } while (!(n == 0));
  print_the_digs (k);
};

/* module 246 */

/* Of course we had better declare another global variable, if the next
 * routines are going to work.
 */

unsigned char old_setting;

/* module 245 */
/*
 * \TeX\ is occasionally supposed to print diagnostic information that
 * goes only into the transcript file, unless |tracing_online| is positive.
 * Here are two routines that adjust the destination of zprint commands:
 */
void
begin_diagnostic (void) { /* prepare to do some tracing */
  old_setting = selector;
  if ((tracing_online <= 0) && (selector == term_and_log)) {
	decr (selector);
	if (history == spotless)
	  history = warning_issued;
  };
}

void
end_diagnostic (boolean blank_line) { /* restore proper conditions after tracing */
  print_nl_string("");
  if (blank_line)
    print_ln();
  selector = old_setting;
};



/* module 262 */

/* Single-character control sequences do not need to be looked up in a hash
 * table, since we can use the character code itself as a direct address.
 * The procedure |print_cs| prints the name of a control sequence, given
 * a pointer to its address in |eqtb|. A space is printed after the name
 * unless it is a single nonletter or an active character. This procedure
 * might be invoked with invalid data, so it is ``extra robust.'' The
 * individual characters must be printed one at a time using |zprint|, since
 * they may be unprintable.
 */
void
print_cs (int p) { /* prints a purported control sequence */
  if (p < hash_base) { /* single character */
	if (p >= single_base) {
	  if (p == null_cs) {
		print_esc_string ("csname");
		print_esc_string ("endcsname");
	  } else {
		print_esc (p - single_base);
		if (cat_code (p - single_base) == letter)
		  print_char (' ');
	  }
	} else {
	  if (p < active_base) {
		print_esc_string ("IMPOSSIBLE.");
	  } else {
		zprint (p - active_base);
	  }
	}
  } else {
	if ((p >= undefined_control_sequence) && (p <= eqtb_size)) {
	  print_esc_string ("IMPOSSIBLE.");
	} else {
	  if ((text (p) >= str_ptr)) {
		print_esc_string ("NONEXISTENT.");
	  } else {
		print_esc (text (p));
		print_char (' ');
	  };
	}
  }
};

/* module 263 */

/* Here is a similar procedure; it avoids the error checks, and it never
 * prints a space after the control sequence.
 */
void
sprint_cs (pointer p) {	/* prints a control sequence */
  if (p < hash_base) {
	if (p < single_base) {
	  zprint (p - active_base);
	} else {
	  if (p < null_cs) {
		print_esc (p - single_base);
	  } else {
		print_esc_string ("csname");
		print_esc_string ("endcsname");
	  }
	}
  } else {
	print_esc (text (p));
  }
};

/* module 518 */

/* Conversely, here is a routine that takes three strings and prints a file
 * name that might have produced them. (The routine is system dependent, because
 * some operating systems put the file area last instead of first.)
 */

void
print_file_name (int n, int a, int e) {
  slow_print (a);
  slow_print (n);
  slow_print (e);
};

/* module 843 */

/* 
 * In order to convert mlists to hlists, i.e., noads to nodes, we need several
 * subroutines that are conveniently dealt with now.
 * 
 * Let us first introduce the macros that make it easy to get at the parameters and
 * other font information. A size code, which is a multiple of 16, is added to a
 * family number to get an index into the table of internal font numbers
 * for each combination of family and size. (Be alert: Size codes get
 * larger as the type gets smaller.)
 */

void
print_size (int s) {
  if (s == 0) {
	print_esc_string ("textfont");
  } else if (s == script_size) {
	print_esc_string ("scriptfont");
  } else {
	print_esc_string ("scriptscriptfont");
  };
};

/* module 1815 */

/* When debugging a macro package, it can be useful to see the exact
 * control sequence names in the format file. For example, if ten new
 * csnames appear, it's nice to know what they are, to help pinpoint where
 * they came from. (This isn't a truly ``basic'' printing procedure, but
 * that's a convenient module in which to put it.)
 */

void
print_csnames (int hstart, int hfinish) {
  int c, h;
  fprintf (stderr, "%s%ld%s%ld%c\n", "fmtdebug:csnames from ", 
	   (long)hstart, " to ", (long)hfinish, ':');
  for (h = hstart; h <= hfinish; h++) {
	if (text (h) > 0) { /* if have anything at this position */
	  for (c = str_start[text (h)]; c <= str_start[text (h) + 1] - 1; c++) {
		putc (str_pool[c],stderr);	/* print the characters */
	  };
	  putc ('|',stderr);
	};
  };
};



/* module 66 */

/* Here is a trivial procedure to print two digits; it is usually called with
 * a parameter in the range |0<=n<=99|.
 */
void
print_two (int n) {	/* prints two least significant digits */
  n = abs (n) % 100;
  print_char ('0' + (n / 10));
  print_char ('0' + (n % 10));
};


/* module 67 */

/* Hexadecimal printing of nonnegative integers is accomplished by |print_hex|. */
void
print_hex (int n) {	/* prints a positive integer in hexadecimal form */
  unsigned char k; /* index to current digit; we assume that $0\L n<16^{22}$ */ 
  k = 0;
  print_char ('"');
  do {
	dig[k] = n % 16;
	n = n / 16;
	incr (k);
  } while (!(n == 0));
  print_the_digs (k);
};


/* module 69 */

/* Roman numerals are produced by the |print_roman_int| routine. Readers
 * who like puzzles might enjoy trying to figure out how this tricky code
 * works; therefore no explanation will be given. Notice that 1990 yields
 * \.{mcmxc}, not \.{mxm}.
 */
void
print_roman_int (int n) {
  int j, k;           /* mysterious indices into |str_pool| */
  nonnegative_integer u, v;     /* mysterious numbers */
  char *jj;
  jj = "m2d5c2l5x2v5i";
  j = 0;
  k = 0;
  v = 1000;
  loop {
	while (n >= (int)v) {
	  print_char (jj[j]);
	  n = n - v;
	};
	if (n <= 0) /* nonpositive input produces no output */
	  return; 
	k = j + 2;
	u = v / (jj[k - 1] - '0');
	if (jj[k - 1] == '2') {
	  k = k + 2;
	  u = u / (jj[k - 1] - '0');
 	};
    if (n + u >= v) {
 	  print_char (jj[k]);
  	  n = n + u;
	} else {
	  j = j + 2;
  	  v = v / (jj[j - 1] - '0');
 	};
  };
};

/* module 103 */

/* Conversely, here is a procedure analogous to |print_int|. If the output
 * of this procedure is subsequently read by \TeX\ and converted by the
 * |round_decimals| routine above, it turns out that the original value will
 * be reproduced exactly; the ``simplest'' such decimal number is output,
 * but there is always at least one digit following the decimal point.
 * 
 * The invariant relation in the \&{repeat} loop is that a sequence of
 * decimal digits yet to be printed will yield the original number if and only if
 * they form a fraction~$f$ in the range $s-\delta\L10\cdot2^{16}f<s$.
 * We can stop if and only if $f=0$ satisfies this condition; the loop will
 * terminate before $s$ can possibly become zero.
 */
void
print_scaled (scaled s) { /* prints scaled real, rounded to five digits */
  scaled delta;			/* amount of allowable inaccuracy */
  if (s < 0) {
	print_char ('-');
	negate (s);		/* print the sign, if negative */
  };
  print_int (s / unity); /* print the integer part */
  print_char ('.');
  s = 10 * (s % unity) + 5;
  delta = 10;
  do {
	if (delta > unity)
	  s = s + 32768 - 50000; /* round the last digit */
	print_char ('0' + (s / unity));
	s = 10 * (s % unity);
	delta = delta * 10;
  } while (s > delta);
};

/* module 114 */

/* When debugging, we may want to print a |memory_word| without knowing
 * what type it is; so we print it in all modes.
 */

#ifdef TEXMF_DEBUG
void
print_word (memory_word w) { /* prints |w| in all ways */
  print_int (w.cint);
  print_char (' ');
  print_scaled (w.sc);
  print_char (' ');
  print_scaled (round (unity * zfloat (w.gr)));
  print_ln();
  print_int (w.hh.lhfield);
  print_char ('=');
  print_int (w.hh.b0);
  print_char (':');
  print_int (w.hh.b1);
  print_char (';');
  print_int (w.hh.rh);
  print_char (' ');
  print_int (w.qqqq.b0);
  print_char (':');
  print_int (w.qqqq.b1);
  print_char (':');
  print_int (w.qqqq.b2);
  print_char (':');
  print_int (w.qqqq.b3);
};
#endif /* TEXMF_DEBUG */


/* module 55 */

void
print_initialize (void) {
  selector = term_only;
  tally = 0;
  term_offset = 0;
  file_offset = 0;
}
