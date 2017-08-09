

#include "types.h"
#include "c-compat.h"
#include "globals.h"

/* module 256 */

/* 
 * Control sequences are stored and retrieved by means of a fairly standard hash
 * table algorithm called the method of ``coalescing lists'' (cf.\ Algorithm 6.4C
 * in {\sl The Art of Computer Programming\/}). Once a control sequence enters the
 * table, it is never removed, because there are complicated situations
 * involving \.{\\gdef} where the removal of a control sequence at the end of
 * a group would be a mistake preventable only by the introduction of a
 * complicated reference-count mechanism.
 * 
 * The actual sequence of letters forming a control sequence identifier is
 * stored in the |str_pool| array together with all the other strings. An
 * auxiliary array |hash| consists of items with two halfword fields per
 * word. The first of these, called |next(p)|, points to the next identifier
 * belonging to the same coalesced list as the identifier corresponding to~|p|;
 * and the other, called |text(p)|, points to the |str_start| entry for
 * |p|'s identifier. If position~|p| of the hash table is empty, we have
 * |text(p)=0|; if position |p| is either empty or the end of a coalesced
 * hash list, we have |next(p)=0|. An auxiliary pointer variable called
 * |hash_used| is maintained in such a way that all locations |p>=hash_used|
 * are nonempty. The global variable |cs_count| tells how many multiletter
 * control sequences have been defined, if statistics are being kept.
 * 
 * A global boolean variable called |no_new_control_sequence| is set to
 * |true| during the time that new hash table entries are forbidden.
 */

two_halves *hash; /* the hash table */ 
pointer hash_used; /* allocation pointer for |hash| */ 
boolean global_no_new_control_sequence; /* are new identifiers legal? */ 
int cs_count; /* total number of known identifiers */

#define hash_is_full ( hash_used  ==  hash_base )

const int hash_prime=850009;

/* module 259 */

/* Here is the subroutine that searches the hash table for an identifier
 * that matches a given string of length |l>1| appearing in |buffer[j..
 * (j+l-1)]|. If the identifier is found, the corresponding hash table address
 * is returned. Otherwise, if the global variable |no_new_control_sequence|
 * is |true|, the dummy address |undefined_control_sequence| is returned.
 * Otherwise the identifier is inserted into the hash table and its location
 * is returned.
 */
pointer
id_lookup (int j, int l) { /* search the hash table */
  int h;			/* hash code */
  int d;			/* number of characters in incomplete current string */
  pointer p;			/* index in |hash| array */
  pointer k;			/* index in |buffer| array */
  /* begin expansion of Compute the hash code |h| */
  /* module 261 */
  /* The value of |hash_prime| should be roughly 85\pct! of |hash_size|, and it
   * should be a prime number. The theory of hashing tells us to expect fewer
   * than two table probes, on the average, when the search is successful.
   * [See J.~S. Vitter, {\sl Journal of the ACM\/ \bf30} (1983), 231--258.]
   */
  h = buffer[j];
  for (k = j + 1; k <= j + l - 1; k++) {
	h = h + h + buffer[k];
	while (h >= hash_prime)
	  h = h - hash_prime;
  };
  /* end expansion of Compute the hash code |h| */
  p = h + hash_base;
  /* we start searching here; note that |0<=h<hash_prime| */
  loop {
    if (text (p) > 0)
      if (length (text (p)) == l)
		if (str_eq_buf (text (p), j))
		  goto FOUND;
    if (next (p) == 0) {
      if (is_no_new_control_sequence()) {
	    p = undefined_control_sequence;
	  } else {
	    /* begin expansion of Insert a new control sequence after |p|, then make |p| point to it */
	    /* module 260 */
	    if (text (p) > 0) {
	      do {
		if (hash_is_full)
		  overflow ("hash size", HASH_SIZE);
		decr (hash_used);
	      } while (text (hash_used) != 0);	/* search for an empty location in |hash| */
	      next (p) = hash_used;
	      p = hash_used;
	    };
	    str_room (l);
	    d = cur_length;
	    while (pool_ptr > str_start[str_ptr]) {
		  decr (pool_ptr);
		  str_pool[pool_ptr + l] = str_pool[pool_ptr];
		};
	    /* move current string up to make room for another */
	    for (k = j; k <= j + l - 1; k++)
	      append_char (buffer[k]);
	    text (p) = make_string ();
	    pool_ptr = pool_ptr + d;
	    incr (cs_count);
	  };
	/* end expansion of Insert a new control sequence after |p|, then make |p| point to it */
	goto FOUND;
	};
    p = next (p);
  };
 FOUND:
  return p;
}

/* Here is a similar subroutine that searches the hash table for an identifier,
 * accepting a TeX string instead of offsets into |buffer|. Since the string
 * is already made, this routine is shorter than |id_lookup|
 */
pointer
id_lookup_string (str_number string) { /* search the hash table */
  integer j, l; /* str_pool index */
  int h;			/* hash code */
  pointer p;			/* index in |hash| array */
  integer k;			/* index in |str_pool|*/
  /* begin expansion of Compute the hash code |h| */
  /* module 261 */
  /* The value of |hash_prime| should be roughly 85\pct! of |hash_size|, and it
   * should be a prime number. The theory of hashing tells us to expect fewer
   * than two table probes, on the average, when the search is successful.
   * [See J.~S. Vitter, {\sl Journal of the ACM\/ \bf30} (1983), 231--258.]
   */
  j = str_start[string];
  l = str_start[(string+1)];
  h = str_pool[j];
  for (k = j + 1; k < l ; k++) {
	h = h + h + str_pool[k];
	while (h >= hash_prime)
	  h = h - hash_prime;
  };
  /* end expansion of Compute the hash code |h| */
  p = h + hash_base;
  /* we start searching here; note that |0<=h<hash_prime| */
  loop {
    if (text (p) > 0)
      if (length (text (p)) == (l-j))
		if (str_eq_str (text (p), string))
		  goto FOUND;
    if (next (p) == 0) {
      if (is_no_new_control_sequence()) {
	    p = undefined_control_sequence;
	  } else {
	    /* Insert a new control sequence after |p|, then make |p| point to it */
	    /* module 260 */
	    if (text (p) > 0) {
	      do {
		if (hash_is_full)
		  overflow ("hash size", HASH_SIZE);
		decr (hash_used);
	      } while (text (hash_used) != 0);	/* search for an empty location in |hash| */
	      next (p) = hash_used;
	      p = hash_used;
	    };
	    text (p) = string;
	    incr (cs_count);
	  };
	/* Insert a new control sequence after |p|, then make |p| point to it */
	goto FOUND;
	};
    p = next (p);
  };
 FOUND:
  return p;
}

/* module 264 */

/* We need to put \TeX's ``primitive'' control sequences into the hash
 * table, together with their command code (which will be the |eq_type|)
 * and an operand (which will be the |equiv|). The |primitive| procedure
 * does this, in a way that no \TeX\ user can. The global value |cur_val|
 * contains the new |eqtb| pointer after |primitive| has acted.
 */

void
primitive (str_number s, quarterword c, halfword o) {
  pool_pointer k;		/* index into |str_pool| */
  unsigned int j;		/* index into |buffer| */
  small_number l;		/* length of the string */
  if (s < 256) {
	cur_val = s + single_base;
  } else {
	k = str_start[s];
	l = str_start[s + 1] - k; /* we will move |s| into the (possibly non-empty) |buffer| */
	if (first + l > (unsigned)buf_size + 1)
	  realloc_buffer();
	for (j = 0; j <= (unsigned)l - 1; j++)
	  buffer[first + j] = str_pool[k + j];
	cur_val = id_lookup (first, l); /* |no_new_control_sequence| is |false| */ 
	flush_string;
	text (cur_val) = s; /* we don't want to have the string twice */ 
  };
  eq_level (cur_val) = level_one;
  eq_type (cur_val) = c;
  equiv (cur_val) = o;
};

/* smallest index in hash array, i.e., |hash_base|, used in malloc()
   Use |hash_offset=0| for compilers which cannot decrement pointer.
   (then the lowest 514 or so entries in |hash| will remain unused). */
#define hash_offset ( hash_base )

/* auxiliary pointer for the hash */ 
two_halves *yhash; 

void
hash_initialize (void) {
  pointer hash_top; /* maximum of the hash array */ 
  set_no_new_control_sequence(true);
  hash_top = undefined_control_sequence;
  yhash = xmalloc_array (two_halves, 1 + hash_top - hash_offset);
  hash = yhash - hash_offset; /* Some compilers require |hash_offset=0| */
  /* TH: using memset() is a lot faster than the original code. On my
   * machine, time spent in hash_initialize dropped from 0.51sec to 0.01sec 
   */
  hash_used = hash_top;
  memset((void *)yhash,0,sizeof(two_halves)*((hash_top-hash_offset)+1));
	/*
	  next (hash_base) = 0;
	  text (hash_base) = 0;
	  for (hash_used = hash_base + 1; hash_used <= hash_top; hash_used++)
	  hash[hash_used] = hash[hash_base]; 
	*/
}

/* module 258 */
void
hash_initialize_init (void) {
  hash_used = frozen_control_sequence;	/* nothing is used */
  cs_count = 0;
  eq_type (frozen_dont_expand) = dont_expand;
  primitive_text (frozen_dont_expand,"notexpanded:");
}

/* module 1463 */
/* A different scheme is used to compress the hash table, since its lower
 * region is usually sparse. When |text(p)<>0| for |p<=hash_used|, we output
 * two words, |p| and |hash[p]|. The hash table is, of course, densely packed
 * for |p>=hash_used|, so the remaining entries are output in a~block.
 */
void
dump_hash (void) {
  int p;
  dump_int (hash_used);
  cs_count = frozen_control_sequence - 1 - hash_used ;
  for (p = hash_base; p <= hash_used; p++)
	if (text (p) != 0) {
	  dump_int (p);
	  dump_hh (hash[p]);
	  incr (cs_count);
	};
  dump_things (hash[hash_used + 1], undefined_control_sequence - 1 - hash_used);
  dump_int (cs_count);
  print_ln();
  print_int (cs_count);
  zprint_string (" multiletter control sequences");
}

/* module 1465 */
/* this version is slightly less secure than the original, because |dump_int|
 * is called directly instead of through the |undump| macro.
 */
void
undump_hash (void) {
  int p;
  /* module 1464 */
  undump_int (hash_used);
  p = hash_base - 1;
  do {
    undump_int (p);
    undump_hh (hash[p]);
  } while (p != hash_used);
  undump_things (hash[hash_used + 1],undefined_control_sequence - 1 -hash_used);
  if (debug_format_file){
	print_csnames (hash_base,undefined_control_sequence - 1);
  };
  undump_int (cs_count);
}
