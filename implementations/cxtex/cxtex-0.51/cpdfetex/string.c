
#include "types.h"
#include "c-compat.h"
#include "globals.h"

integer max_strings; /* maximum number of strings; must not exceed  |max_halfword| */ 
integer strings_free; /* strings available after format loaded */ 
integer string_vacancies; /* the minimum number of characters that should be 
							 available for the user's control sequences and 
							 font names, after \TeX's own error messages 
							 are stored */
integer pool_size; /* maximum number of characters in strings, including all 
					  error messages and help texts, and the names of all 
					  fonts and control sequences; must exceed |string_vacancies| 
					  by the total length of \TeX's own strings, which is 
					  currently about 23000 */
integer pool_free; /* pool space free after format loaded */ 

/* module 39 */
packed_ASCII_code *str_pool; /* the characters */ 
pool_pointer *str_start; /* the starting pointers */ 
pool_pointer pool_ptr; /* first unused position in |str_pool| */
str_number str_ptr; /* number of the current string being created */
pool_pointer init_pool_ptr; /* the starting value of |pool_ptr| */
str_number init_str_ptr; /* the starting value of |str_ptr| */

/* module 43 */

/* Once a sequence of characters has been appended to |str_pool|, it
 * officially becomes a string when the function |make_string| is called.
 * This function returns the identification number of the new string as its
 * value.
 */
str_number
make_string (void) {  /* current string enters the pool */
  strings_room(1);
  str_ptr++;
  str_start[str_ptr] = pool_ptr;
  return str_ptr-1;
};

/* module 45 */

/* The following subroutine compares string |s| with another string of the
 * same length that appears in |buffer| starting at position |k|;
 * the result is |true| if and only if the strings are equal.
 * Empirical tests indicate that |str_eq_buf| is used in such a way that
 * it tends to return |true| about 80 percent of the time.
 */
boolean
str_eq_buf (str_number s, int k) { /* test equality of strings */
  pool_pointer j; /* running index */ 
  j = str_start[s];
  while (j < str_start[s + 1]) {
	if (str_pool[j] != buffer[k]) {
	  return false;
	};
	j++;
	k++;
  };
  return true;
}

/* module 46 */

/* Here is a similar routine, but it compares two strings in the string pool,
 * and it does not assume that they have the same length.
 */
boolean
str_eq_str (str_number s, str_number t) { /* test equality of strings */
  pool_pointer j, k; /* running indices */ 
  if (length (s) != length (t))
	return false;
  j = str_start[s];
  k = str_start[t];
  while (j < str_start[s + 1]) {
	if (str_pool[j] != str_pool[k])
	  return false;
	j++;
	k++;
  };
  return true;
}

/* module 47 */

/* The initial values of |str_pool|, |str_start|, |pool_ptr|,
 * and |str_ptr| are computed by the \.{INITEX} program, based in part
 * on the information that \.{WEB} has output while processing \TeX.
 */

/* module 50 */

/* When the \.{WEB} system program called \.{TANGLE} processes the \.{TEX.WEB}
 * description that you are now reading, it outputs the \PASCAL\ program
 * \.{TEX.PAS} and also a string pool file called \.{TEX.POOL}. The \.{INITEX}
 * program reads the latter file, where each string appears as a two-digit decimal
 * length followed by the string itself, and the information is recorded in
 * \TeX's string memory.
 */


boolean 
get_strings_started (void) { 
  /* initializes the string pool, but returns |false| if something goes wrong */
  int k;		/* small indices or counters */
  str_number g;			/* garbage */
  pool_ptr = 0;
  str_ptr = 0;
  str_start[0] = 0;
  /* begin expansion of Make the first 256 strings */
  /* module 48 */
  /* The first 256 strings will consist of a single character only. */
  for (k = 0; k <= 255; k++) {
      append_char (k);
      g = make_string ();
    };
  /* end expansion of Make the first 256 strings */
  g = make_string(); /* this makes the null string */
  return true;
};

/* module 70 */

/* The |zprint| subroutine will not print a string that is still being
 * created. The following procedure will.
 */
void
print_current_string (void) { /* prints a yet-unmade string */
  pool_pointer j;		/* points to current character code */
  j = str_start[str_ptr];
  while (j < pool_ptr) {
	print_char (str_pool[j++]);
  };
};

/* module 1817 */

/* 
 * \TeX{} uses 2 upto 4 {\it new\/} strings when scanning a filename in an 
 * \.{\\input}, \.{\\openin}, or \.{\\openout} operation. These strings are 
 * normally lost because the reference to them are not saved after finishing 
 * the operation. |search_string| searches through the string pool for the
 * given string and returns either 0 or the found string number.
 */
str_number
search_string (str_number search) {
  str_number s;			/* running index */
  int len;			/* length of searched string */
  len = length (search);
  if (len == 0) /* trivial case */
	return null_string;
  s = search - 1;
  /* start search with newest string below |s|; |search>1|! */
  /* first 256 strings depend on implementation!! */
  while (s > 255) {
	if (length (s) == len) {
	  if (str_eq_str (s, search)) {
		return s;
	  };
	};
	s--;
  };
  return 0;
}

/* module 1818 */

/* The following routine is a variant of |make_string|. It searches
 * the whole string pool for a string equal to the string currently built
 * and returns a found string. Otherwise a new string is created and
 * returned. Be cautious, you can not apply |flush_string| to a replaced
 * string!
 */
str_number
slow_make_string (void) {
  str_number s;			/* result of |search_string| */
  str_number t;			/* new string */
  t = make_string ();
  s = search_string (t);
  if (s > 0) {
	flush_string;
	return s;
  };
  return t;
}

str_number
slow_make_tex_string (char *ss) {
  size_t len;
  char *s;
  len = strlen(ss);
  if(len==0) 
    return null_string;
  checkpoolpointer (pool_ptr, len);
  s=ss;
  while (len-- > 0)
      str_pool[pool_ptr++] = *s++;
  return (slow_make_string());
}

string
gettexstring (str_number s) {
  pool_pointer len;
  string xname;
  len = str_start[s + 1] - str_start[s];
  xname = (string)xmalloc (len + 1);
  strncpy (xname, (string)&str_pool[str_start[s]], len);
  xname[len] = 0;
  return xname;
}

/* FIXME -- some (most?) of this can/should be moved to the Pascal/WEB side. */
void
checkpoolpointer (pool_pointer pool_ptrj, size_t len) {
  if (pool_ptrj + (integer)len >= pool_size) {
    fprintf (stderr, "\nstring pool overflow [%i bytes]\n",  (int)pool_size); /* fixme */
    exit(1);
  }
}

void 
flush_str (str_number s) {			
  /* flush a string if possible */
  if (flushable (s))
    flush_string;
};
		
boolean 
str_less_str (str_number s1, str_number s2) { /* compare two strings */
  pool_pointer j1, j2;
  int l, i;
  j1 = str_start[s1];
  j2 = str_start[s2];
  if (length (s1) < length (s2)) {
	l = length (s1);
  } else {
	l = length (s2);
  }
  i = 0;
  while ((i < l) && (str_pool[j1 + i] == str_pool[j2 + i]))
	incr (i);
  if (((i < l) && (str_pool[j1 + i] < str_pool[j2 + i]))
	  || ((i == l) && (length (s1) < length (s2))))
	return true;
  return false;
}

boolean
str_in_str (str_number s, char * rr, int i) { /* test equality of strings */
  pool_pointer j, k;		/* running indices */
  str_number r;
  r = slow_make_tex_string(rr);
  if (length (s) < i + length (r))
    return false;
  j = i + str_start[s];
  k = str_start[r];
  while ((j < str_start[s + 1]) && (k < str_start[r + 1])) {
	if (str_pool[j] != str_pool[k])
	  return false;
	incr (j);
	incr (k);
  };
  return true;
};


boolean 
substr_of_str (char *s, char *t) {
  char *a;
  a = strstr(t,s);
  if (a == NULL)
	return false;
  return true;
}

/* module 1831 */

/* This function used to be in pdftex, but is useful in tex too. */
str_number 
get_nullstr (void) {
  return null_string; 
};


