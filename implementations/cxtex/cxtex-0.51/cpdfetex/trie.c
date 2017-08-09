
#include "types.h"
#include "c-compat.h"
#include "globals.h"

/* for lower |trie_op_hash| array bound; must be equal to |-trie_op_size|. */
#define neg_trie_op_size (-trie_op_size)

integer trie_size; /* space for hyphenation patterns; should be larger for 
					  \.{INITEX} than it is in production versions of \TeX.
					  50000 is needed for English, German, and Portuguese. */

/* module 1065 */

/* For more than 255 trie op codes, the three fields |trie_link|, |trie_char|,
 * and |trie_op| will no longer fit into one memory word; thus using web2c
 * we define |trie| as three array instead of an array of records.
 * The variant will be implented by reusing the opcode field later on with
 * another macro.
 */

/* We will dynamically allocate these arrays. */
trie_pointer *trie_trl; /* |trie_link| */
trie_pointer *trie_tro; /* |trie_op| */
quarterword *trie_trc; /* |trie_char| */
small_number hyf_distance[trie_op_size]; /* position |k-j| of $n_j$ */
small_number hyf_num[trie_op_size]; /* value of $n_j$ */
trie_opcode hyf_next[trie_op_size]; /* continuation code */
unsigned int op_start[257]; /* offset for current language */

/* module 1070 */
str_number *hyph_word; /* exception words */
halfword *hyph_list; /* list of hyphen positions */
hyph_pointer *hyph_link; /* link array for hyphen exceptions hash table */
int hyph_count; /* the number of words in the exception dictionary */
int hyph_next; /* next free slot in hyphen exceptions hash table */

/* module 1091 */
#define trie_root  trie_l [0 ]

/* module 1094 */
#define trie_ref  trie_hash
#define trie_back( arg )  trie_tro [ arg ]


/* module 1087 */

/* Before we discuss trie building in detail, let's consider the simpler
 * problem of creating the |hyf_distance|, |hyf_num|, and |hyf_next| arrays.
 * 
 * Suppose, for example, that \TeX\ reads the pattern `\.{ab2cde1}'. This is
 * a pattern of length 5, with $n_0\ldots n_5=0\,0\,2\,0\,0\,1$ in the
 * notation above. We want the corresponding |trie_op| code |v| to have
 * |hyf_distance[v]=3|, |hyf_num[v]=2|, and |hyf_next[v]=@t$v^\prime$@>|,
 * where the auxiliary |trie_op| code $v^\prime$ has
 * |hyf_distance[@t$v^\prime$@>]=0|, |hyf_num[@t$v^\prime$@>]=1|, and
 * |hyf_next[@t$v^\prime$@>]=min_trie_op|.
 * 
 * \TeX\ computes an appropriate value |v| with the |new_trie_op| subroutine
 * below, by setting
 * $$\hbox{|@t$v^\prime$@>:=new_trie_op(0,1,min_trie_op)|,\qquad
 * |v:=new_trie_op(3,2,@t$v^\prime$@>)|.}$$
 * This subroutine looks up its three
 * parameters in a special hash table, assigning a new value only if these
 * three have not appeared before for the current language.
 * 
 * The hash table is called |trie_op_hash|, and the number of entries it contains
 * is |trie_op_ptr|.
 */
integer trie_op_hashz[(2*trie_op_size+1)]; /* trie op codes for quadruples */
#define trie_op_hash(arg) trie_op_hashz[(arg+trie_op_size+1)]
trie_opcode trie_used[257]; /* largest opcode used so far for this language */
ASCII_code trie_op_lang[trie_op_size]; /* language part of a hashed 
										  quadruple */
trie_opcode trie_op_val[trie_op_size]; /* opcode corresponding to a 
										  hashed quadruple */
integer trie_op_ptr; /* number of stored ops so far */
trie_opcode max_op_used; /* largest opcode used for any language */

/* module 1091 */

/* The linked trie that is used to preprocess hyphenation patterns appears
 * in several global arrays. Each node represents an instruction of the form
 * ``if you see character |c|, then perform operation |o|, move to the
 * next character, and go to node |l|; otherwise go to node |r|.''
 * The four quantities |c|, |o|, |l|, and |r| are stored in four arrays
 * |trie_c|, |trie_o|, |trie_l|, and |trie_r|. The root of the trie
 * is |trie_l[0]|, and the number of nodes is |trie_ptr|. Null trie
 * pointers are represented by zero. To initialize the trie, we simply
 * set |trie_l[0]| and |trie_ptr| to zero. We also set |trie_c[0]| to some
 * arbitrary value, since the algorithm may access it.
 * 
 * The algorithms maintain the condition
 * $$\hbox{|trie_c[trie_r[z]]>trie_c[z]|\qquad
 * whenever |z<>0| and |trie_r[z]<>0|};$$ in other words, sibling nodes are
 * ordered by their |c| fields.
 */

packed_ASCII_code *trie_c; /* characters to match */
trie_opcode *trie_o; /* operations to perform */
trie_pointer *trie_l; /* left subtrie links */
trie_pointer *trie_r; /* right subtrie links */
trie_pointer trie_ptr; /* the number of nodes in the trie */
trie_pointer *trie_hash; /* used to identify equivalent subtries */

/* module 1094 */

/* The compressed trie will be packed into the |trie| array using a
 * ``top-down first-fit'' procedure. This is a little tricky, so the reader
 * should pay close attention: The |trie_hash| array is cleared to zero
 * again and renamed |trie_ref| for this phase of the operation; later on,
 * |trie_ref[p]| will be nonzero only if the linked trie node |p| is the
 * smallest character
 * in a family and if the characters |c| of that family have been allocated to
 * locations |trie_ref[p]+c| in the |trie| array. Locations of |trie| that
 * are in use will have |trie_link=0|, while the unused holes in |trie|
 * will be doubly linked with |trie_link| pointing to the next larger vacant
 * location and |trie_back| pointing to the next smaller one. This double
 * linking will have been carried out only as far as |trie_max|, where
 * |trie_max| is the largest index of |trie| that will be needed.
 * To save time at the low end of the trie, we maintain array entries
 * |trie_min[c]| pointing to the smallest hole that is greater than~|c|.
 * Another array |trie_taken| tells whether or not a given location is
 * equal to |trie_ref[p]| for some |p|; this array is used to ensure that
 * distinct nodes in the compressed trie will have distinct |trie_ref|
 * entries.
 */

boolean *trie_taken; /* does a family start here? */
trie_pointer trie_min[257]; /* the first possible slot for each character */
trie_pointer trie_max; /* largest location used in |trie| */
boolean trie_not_ready; /* is the trie still in linked form? */

/* module 1086 */

/* 
 * The trie for \TeX's hyphenation algorithm is built from a sequence of
 * patterns following a \.{\\patterns} specification. Such a specification
 * is allowed only in \.{INITEX}, since the extra memory for auxiliary tables
 * and for the initialization program itself would only clutter up the
 * production version of \TeX\ with a lot of deadwood.
 * 
 * The first step is to build a trie that is linked, instead of packed
 * into sequential storage, so that insertions are readily made.
 * After all patterns have been processed, \.{INITEX}
 * compresses the linked trie by identifying common subtries. Finally the
 * trie is packed into the efficient sequential form that the hyphenation
 * algorithm actually uses.
 */

/* module 1088 */

/* It's tempting to remove the |overflow| stops in the following procedure;
 * |new_trie_op| could return |min_trie_op| (thereby simply ignoring
 * part of a hyphenation pattern) instead of aborting the job. However, that would
 * lead to different hyphenation results on different installations of \TeX\
 * using the same patterns. The |overflow| stops are necessary for portability
 * of patterns.
 */
trie_opcode 
new_trie_op (small_number d, small_number n, trie_opcode v) {
  integer h; /* trial hash location */ 
  trie_opcode u; /* trial op code */ 
  integer l; /* pointer to stored data */ 
  h = abs (intcast (n) + 313 * intcast (d) + 361 * intcast (v) + 1009 * intcast (cur_lang)) % 
	(trie_op_size - neg_trie_op_size) + neg_trie_op_size;
  loop {
	l = trie_op_hash(h);
	if (l == 0)	{/* empty position found for a new op */
	  if (trie_op_ptr == trie_op_size)
		overflow ("pattern memory ops", trie_op_size);
	  u = trie_used[cur_lang];
	  if (u == max_trie_op)
		overflow ("pattern memory ops per language", max_trie_op - min_trie_op);
	  incr (trie_op_ptr);
	  incr (u);
	  trie_used[cur_lang] = u;
	  if (u > max_op_used)
		max_op_used = u;
	  hyf_distance[trie_op_ptr] = d;
	  hyf_num[trie_op_ptr] = n;
	  hyf_next[trie_op_ptr] = v;
	  trie_op_lang[trie_op_ptr] = cur_lang;
	  trie_op_hash(h) = trie_op_ptr;
	  trie_op_val[trie_op_ptr] = u;
	  return u;
	};
	if ((hyf_distance[l] == d) && (hyf_num[l] == n) && (hyf_next[l] == v) && (trie_op_lang[l] == cur_lang)) {
	  return trie_op_val[l];
	};
	if (h > -(integer)trie_op_size) {
	  decr (h);
	} else {
	  h = trie_op_size;
	}
  };
};


/* module 1092 */

/* Let us suppose that a linked trie has already been constructed.
 * Experience shows that we can often reduce its size by recognizing common
 * subtries; therefore another hash table is introduced for this purpose,
 * somewhat similar to |trie_op_hash|. The new hash table will be
 * initialized to zero.
 * 
 * The function |trie_node(p)| returns |p| if |p| is distinct from other nodes
 * that it has seen, otherwise it returns the number of the first equivalent
 * node that it has seen.
 * 
 * Notice that we might make subtries equivalent even if they correspond to
 * patterns for different languages, in which the trie ops might mean quite
 * different things. That's perfectly all right.
 */
trie_pointer
trie_node (trie_pointer p) {			/* converts to a canonical form */
  trie_pointer h; /* trial hash location */ 
  trie_pointer q; /* trial trie node */ 
  h = abs (intcast (trie_c[p]) + 1009 * intcast (trie_o[p]) + 2718 *
		   intcast (trie_l[p]) + 3142 * intcast (trie_r[p])
		   ) % trie_size;
  loop {
	q = trie_hash[h];
	if (q == 0) {
	  trie_hash[h] = p;
	  return p;
	};
	if ((trie_c[q] == trie_c[p]) && (trie_o[q] == trie_o[p])
		&& (trie_l[q] == trie_l[p]) && (trie_r[q] == trie_r[p])) {
	  return q;
	};
	if (h > 0) {
	  decr (h);
	} else {
	  h = trie_size;
	}
  };
};


/* module 1093 */

/* A neat recursive procedure is now able to compress a trie by
 * traversing it and applying |trie_node| to its nodes in ``bottom up''
 * fashion. We will compress the entire trie by clearing |trie_hash| to
 * zero and then saying `|trie_root:=compress_trie(trie_root)|'.
 */

trie_pointer 
compress_trie (trie_pointer p) {
  if (p == 0) {
	return 0;
  } else {
	trie_l[p] = compress_trie (trie_l[p]);
	trie_r[p] = compress_trie (trie_r[p]);
	return trie_node (p);
  };
};

/* module 1097 */

/* The |first_fit| procedure finds the smallest hole |z| in |trie| such that
 * a trie family starting at a given node |p| will fit into vacant positions
 * starting at |z|. If |c=trie_c[p]|, this means that location |z-c| must
 * not already be taken by some other family, and that |z-c+@t$c^\prime$@>|
 * must be vacant for all characters $c^\prime$ in the family. The procedure
 * sets |trie_ref[p]| to |z-c| when the first fit has been found.
 */
void 
first_fit (trie_pointer p) {			/* packs a family into |trie| */
  trie_pointer h; /* candidate for |trie_ref[p]| */ 
  trie_pointer z; /* runs through holes */ 
  trie_pointer q; /* runs through the family starting at |p| */ 
  ASCII_code c; /* smallest character in the family */ 
  trie_pointer l, r; /* left and right neighbors */ 
  short ll; /* upper limit of |trie_min| updating */ 
  c = trie_c[p];
  z = trie_min[c]; /* get the first conceivably good hole */ 
  loop {
	h = z - c;
	/* begin expansion of Ensure that |trie_max>=h+256| */
	/* module 1098 */
	/* By making sure that |trie_max| is at least |h+256|, we can be sure that
	 * |trie_max>z|, since |h=z-c|. It follows that location |trie_max| will
	 * never be occupied in |trie|, and we will have |trie_max>=trie_link(z)|.
	 */
	if (trie_max < h + 256) {
	  if (trie_size <= h + 256)
		overflow ("pattern memory", trie_size);
	  do {
		incr (trie_max);
		trie_taken[trie_max] = false;
		trie_link (trie_max) = trie_max + 1;
		trie_back (trie_max) = trie_max - 1;
	  } while (!(trie_max == h + 256));
	};
	/* end expansion of Ensure that |trie_max>=h+256| */
	if (trie_taken[h])
	  goto NOT_FOUND;
	/* begin expansion of If all characters of the family fit relative to |h|,
	   then |goto found|,\30\ otherwise |goto not_found| */
	/* module 1099 */
	q = trie_r[p];
	while (q > 0) {
	  if (trie_link (h + trie_c[q]) == 0)
		goto NOT_FOUND;
	  q = trie_r[q];
	};
	do_something;
	goto FOUND;
	/* end expansion of If all characters of the family fit relative to |h|, .. */
  NOT_FOUND:
	z = trie_link (z); /* move to the next hole */ 
  };
 FOUND:
  /* begin expansion of Pack the family into |trie| relative to |h| */
  /* module 1100 */
  trie_taken[h] = true;
  trie_ref[p] = h;
  q = p;
  do {
	z = h + trie_c[q];
	l = trie_back (z);
	r = trie_link (z);
	trie_back (r) = l;
	trie_link (l) = r;
	trie_link (z) = 0;
	if (l < 256) {
	  if (z < 256) {
		ll = z;
	  } else {
		ll = 256;
	  }
	  do {
		trie_min[l] = r;
		incr (l);
	  } while (l != ll);
	};
	q = trie_r[q];
  }  while (q != 0);
  /* end expansion of Pack the family into |trie| relative to |h| */
}

/* module 1101 */

/* To pack the entire linked trie, we use the following recursive procedure.  */
void 
trie_pack (trie_pointer p) { /* pack subtries of a family */
  trie_pointer q; /* a local variable that need not be saved on recursive calls */
  do {
	q = trie_l[p];
	if ((q > 0) && (trie_ref[q] == 0)) {
	  first_fit (q);
	  trie_pack (q);
	};
	p = trie_r[p];
  } while (p != 0);
};

/* module 1102 */
#define clear_trie { trie_link ( r )  = 0 ; \
            trie_op ( r )  =  min_trie_op ; \
            trie_char ( r )  =  min_quarterword ;}

/* module 1103 */

/* The fixing-up procedure is, of course, recursive. Since the linked trie
 * usually has overlapping subtries, the same data may be moved several
 * times; but that causes no harm, and at most as much work is done as it
 * took to build the uncompressed trie.
 */
void 
trie_fix (trie_pointer p) {	/* moves |p| and its siblings into |trie| */
  trie_pointer q; /* a local variable that need not be saved on recursive calls */
  ASCII_code c; /* another one that need not be saved */ 
  trie_pointer z; /* |trie| reference; this local variable must be saved */ 
  z = trie_ref[p];
  do {
	q = trie_l[p];
	c = trie_c[p];
	trie_link (z + c) = trie_ref[q];
	trie_char (z + c) = qi (c);
	trie_op (z + c) = trie_o[p];
	if (q > 0)
	  trie_fix (q);
	p = trie_r[p];
  } while (p != 0);
};

/* module 1108 */
/* Insert a new trie node between |q| and |p|, and make |p| point to it */
#define insert_trie_node                             \
		if (trie_ptr == trie_size)                   \
		  overflow ("pattern memory", trie_size);    \
		incr (trie_ptr);                             \
		trie_r[trie_ptr] = p;                        \
		p = trie_ptr;                                \
		trie_l[p] = 0;                               \
		if (first_child) {                           \
		  trie_l[q] = p;                             \
		} else {                                     \
		  trie_r[q] = p;                             \
		}                                            \
		trie_c[p] =      c;                          \
		trie_o[p] = min_trie_op             

/* module 1104 */

/* Now let's go back to the easier problem, of building the linked
 * trie. When \.{INITEX} has scanned the `\.{\\patterns}' control
 * sequence, it calls on |new_patterns| to do the right thing.
 */
void 
new_patterns (void) { /* initializes the hyphenation pattern data */
  unsigned char k, l; /* indices into |hc| and |hyf|; not always in |small_number| range */
  boolean digit_sensed; /* should the next digit be treated as a letter? */
  trie_opcode v; /* trie op code */ 
  trie_pointer p, q; /* nodes of trie traversed during insertion */ 
  boolean first_child; /* is |p=trie_l[q]|? */ 
  ASCII_code c; /* character being inserted */ 
  int cal;
  if (trie_not_ready) {
	set_cur_lang;
	scan_left_brace(); /* a left brace must follow \.{\\patterns} */
	/* begin expansion of Enter all of the patterns into a linked trie, until coming to a right brace */
	/* module 1105 */
	/* Novices are not supposed to be using \.{\\patterns}, so the error
	 * messages are terse. (Note that all error messages appear in \TeX's string
	 * pool, even if they are used only by \.{INITEX}.)
	 */
	k = 0;
	hyf[0] = 0;
	digit_sensed = false;
	loop {
	  get_x_token();
	  switch (cur_cmd) {
	  case letter:
	  case other_char:
		/* begin expansion of Append a new letter or a hyphen level */
		/* module 1106 */
		if (digit_sensed || (cur_chr < '0') || (cur_chr > '9')) {
		  if (cur_chr == '.') {
			cur_chr = 0;	/* edge-of-word delimiter */
		  } else {
			cur_chr = lc_code (cur_chr);
			if (cur_chr == 0) {
			  print_err ("Nonletter");
			  help1 ("(See Appendix H.)");
			  error();
			};
		  };
		  if (k < 63) {
			incr (k);
			hc[k] = cur_chr;
			hyf[k] = 0;
			digit_sensed = false;
		  };
		} else if (k < 63) {
		  hyf[k] = cur_chr - '0';
		  digit_sensed = true;
		};
		/* end expansion of Append a new letter or a hyphen level */
		break;
	  case spacer:
	  case right_brace:
		{
		  if (k > 0) {
			/* begin expansion of Insert a new pattern into the linked trie */
			/* module 1107 */
			/* When the following code comes into play, the pattern $p_1\ldots p_k$
			 * appears in |hc[1..k]|, and the corresponding sequence of numbers $n_0\ldots
			 * n_k$ appears in |hyf[0..k]|.
			 */
			/* begin expansion of Compute the trie op code, |v|, and set |l:=0| */
			/* module 1109 */
			if (hc[1] == 0)
			  hyf[0] = 0;
			if (hc[k] == 0)
			  hyf[k] = 0;
			l = k;
			v = min_trie_op;
			loop {
			  if (hyf[l] != 0)
				v = new_trie_op (k - l, hyf[l], v);
			  if (l > 0) {
				decr (l);
			  } else {
				goto DONE1;
			  };
			};
		  DONE1:
			/* end expansion of Compute the trie op code, |v|, and set |l:=0| */
			q = 0;
			hc[0] = cur_lang;
			while (l <= k) {
			  c = hc[l];
			  incr (l);
			  p = trie_l[q];
			  first_child = true;
			  while ((p > 0) && (c > trie_c[p])) {
				q = p;
				p = trie_r[q];
				first_child = false;
			  };
			  if ((p == 0) || (c < trie_c[p])) {
				/* Insert a new trie node between |q| and |p|, and make |p| point to it */
                insert_trie_node;
			  };
			  q = p;  /* now node |q| represents $p_1\ldots p_{l-1}$ */
			};
			if (trie_o[q] != min_trie_op)  {
			  print_err ("Duplicate pattern");
			  help1 ("(See Appendix H.)");
			  error();
			};
			trie_o[q] = v;
		  };
		  /* end expansion of Insert a new pattern into the linked trie */
		  if (cur_cmd == right_brace)
			goto DONE;
		  k = 0;
		  hyf[0] = 0;
		  digit_sensed = false;
		};
		break;
	  default:
		{
		  print_err  ("Bad ");
		  print_esc_string ("patterns");
		  help1 ("(See Appendix H.)");
		  error();
		};
	  };
	};
  DONE:
	/* end expansion of Enter all of the patterns into a linked trie, until coming to a right brace */
	if (saving_hyph_codes > 0) {
	  /* begin expansion of Store hyphenation codes for current language */
	  /* module 1800 */
	  c = cur_lang;
	  first_child = false;
	  p = 0;
	  do {
		q = p;
		p = trie_r[q];
	  } while (!((p == 0) || (c <= trie_c[p])));
	  if ((p == 0) || (c < trie_c[p])) {
		/* Insert a new trie node between |q| and |p|, and make |p| point to it */
		insert_trie_node;
	  }
	  q = p;
	  /* now node |q| represents |cur_lang| */
	  /* begin expansion of Store all current |lc_code| values */
	  /* module 1801 */
	  /* We store all nonzero |lc_code| values, overwriting any previously
	   * stored values (and possibly wasting a few trie nodes that were used
	   * previously and are not needed now). We always store at least one
	   * |lc_code| value such that |hyph_index| (defined below) will not be zero.
	   */
	  p = trie_l[q];
	  first_child = true;
	  for (cal = 0; cal <= 255; cal++) {
		c= cal;
		if ((lc_code (c) > 0) || ((c == 255) && first_child)) {
		  if (p == 0) {
			/* begin expansion of Insert a new trie node between |q| and |p|, and make |p| point to it */
			insert_trie_node;
		  } else {
			trie_c[p] = c;
		  }
		  trie_o[p] = qi (lc_code (c));
		  q = p;
		  p = trie_r[q];
		  first_child = false;
		}
	  };
	  if (first_child) {
		trie_l[q] = 0;
	  } else {
		trie_r[q] = 0;
	  }
	  /* end expansion of Store all current |lc_code| values */
	};
	/* end expansion of Store hyphenation codes for current language */
  } else {
	print_err ("Too late for ");
	print_esc_string ("patterns");
	help1 ("All patterns must be given before typesetting begins.");
	error();
	link (garbage) = scan_toks (false, false);
	flush_list (def_ref);
  };
};

/* module 1799 */

/* When reading \.{\\patterns} while \.{\\savinghyphcodes} is positive
 * the current |lc_code| values are stored together with the hyphenation
 * patterns for the current language. They will later be used instead of
 * the |lc_code| values for hyphenation purposes.
 * 
 * The |lc_code| values are stored in the linked trie analogous to patterns
 * $p_1$ of length~1, with |hyph_root=trie_r[0]| replacing |trie_root| and
 * |lc_code(p_1)| replacing the |trie_op| code. This allows to compress
 * and pack them together with the patterns with minimal changes to the
 * existing code.
 */
#define hyph_root  trie_r [0 ]

/* module 1110 */

/* Finally we put everything together: Here is how the trie gets to its
 * final, efficient form.
 * The following packing routine is rigged so that the root of the linked
 * tree gets mapped into location 1 of |trie|, as required by the hyphenation
 * algorithm. This happens because the first call of |first_fit| will
 * ``take'' location~1.
 */

void
init_trie (void) {
  trie_pointer p; /* pointer for initialization */ 
  integer j, k, t; /* all-purpose registers for initialization */ 
  trie_pointer r, s; /* used to clean up the packed |trie| */
  /* begin expansion of Get ready to compress the trie */
  /* module 1096 */
  /* Here is how the trie-compression data structures are initialized.
   * If storage is tight, it would be possible to overlap |trie_op_hash|,
   * |trie_op_lang|, and |trie_op_val| with |trie|, |trie_hash|, and |trie_taken|,
   * because we finish with the former just before we need the latter.
   */
  /* begin expansion of Sort \(t)the hyphenation... */
  /* module 1089 */
  /* After |new_trie_op| has compressed the necessary opcode information,
   * plenty of information is available to unscramble the data into the
   * final form needed by our hyphenation algorithm.
   */
  op_start[0] = -min_trie_op;
  for (j = 1; j <= 255; j++)
	op_start[j] = op_start[j - 1] + qo (trie_used[j - 1]);
  for (j = 1; j <= trie_op_ptr; j++)
	trie_op_hash(j) = op_start[trie_op_lang[j]] + trie_op_val[j]; /* destination */ 
  for (j = 1; j <= trie_op_ptr; j++)
	while (trie_op_hash(j) > j) {
	  k = trie_op_hash(j);
	  t = hyf_distance[k];
	  hyf_distance[k] = hyf_distance[j];
	  hyf_distance[j] = t;
	  t = hyf_num[k];
	  hyf_num[k] = hyf_num[j];
	  hyf_num[j] = t;
	  t = hyf_next[k];
	  hyf_next[k] = hyf_next[j];
	  hyf_next[j] = t;
	  trie_op_hash(j) = trie_op_hash(k);
	  trie_op_hash(k) = k;
	};
  /* end expansion of Sort \(t)the hyphenation... */
  for (p = 0; p <= trie_size; p++)
	trie_hash[p] = 0;
  hyph_root = compress_trie (hyph_root);
  trie_root = compress_trie (trie_root); /* identify equivalent subtries */ 
  for (p = 0; p <= trie_ptr; p++)
	trie_ref[p] = 0;
  for (p = 0; p <= 255; p++)
	trie_min[p] = p + 1;
  trie_link (0) = 1;
  trie_max = 0;
  /* end expansion of Get ready to compress the trie */
  if (trie_root != 0) {
	first_fit (trie_root);
	trie_pack (trie_root);
  };
  if (hyph_root != 0) {
	/* begin expansion of Pack all stored |hyph_codes| */
	/* module 1802 */
	/* We must avoid to ``take'' location~1, in order to distinguish between
	 * |lc_code| values and patterns.
	 */
	if (trie_root == 0)
	  for (p = 0; p <= 255; p++)
		trie_min[p] = p + 2;
	first_fit (hyph_root);
	trie_pack (hyph_root);
	hyph_start = trie_ref[hyph_root];
  };
  /* end expansion of Pack all stored |hyph_codes| */
  /* begin expansion of Move the data into |trie| */
  /* module 1102 */
  /* When the whole trie has been allocated into the sequential table, we
   * must go through it once again so that |trie| contains the correct
   * information. Null pointers in the linked trie will be represented by the
   * value~0, which properly implements an ``empty'' family.
   */
  if (trie_max == 0) {	/* no patterns were given */
	for (r = 0; r <= 256; r++)
	  clear_trie;
	trie_max = 256;
  }  else  {
	if (hyph_root > 0)
	  trie_fix (hyph_root);
	if (trie_root > 0)
	  trie_fix (trie_root); /* this fixes the non-holes in |trie| */ 
	r = 0; 
	/* now we will zero out all the holes */
	do {
	  s = trie_link (r);
	  clear_trie;
	  r = s;
	} while (!(r > trie_max));
  };
  trie_char (0) = qi ('?'); /* make |trie_char(c)<>c| for all |c| */
  /* end expansion of Move the data into |trie| */
  trie_not_ready = false;
}

/* module 1090 */
/* Before we forget how to initialize the data structures that have been
 * mentioned so far, let's write down the code that gets them started.
 */
/* module 1095 */
/* Each time \.{\\patterns} appears, it contributes further patterns to
 * the future trie, which will be built only when hyphenation is attempted or
 * when a format file is dumped. The boolean variable |trie_not_ready|
 * will change to |false| when the trie is compressed; this will disable
 * further patterns.
 */
void
trie_initialize_init (void) {
  int k;
  /* module 1090 */
  for (k = -trie_op_size; k <= trie_op_size; k++)
	trie_op_hash(k) = 0;
  for (k = 0; k <= 255; k++)
	trie_used[k] = min_trie_op;
  max_op_used = min_trie_op;
  trie_op_ptr = 0;
  /* module 1095 */
  trie_not_ready = true;
}

void
trie_xmalloc (integer trie_size) {
  trie_trl = xmalloc_array (trie_pointer, trie_size);
  trie_tro = xmalloc_array (trie_pointer, trie_size);
  trie_trc = xmalloc_array (quarterword, trie_size);
  trie_c = xmalloc_array (packed_ASCII_code, trie_size);
  trie_o = xmalloc_array (trie_opcode, trie_size);
  trie_l = xmalloc_array (trie_pointer, trie_size);
  trie_r = xmalloc_array (trie_pointer, trie_size);
  trie_hash = xmalloc_array (trie_pointer, trie_size);
  trie_taken = xmalloc_array (boolean, trie_size);
  trie_root = 0;
  trie_c[0] = 0;
  trie_ptr = 0;
  hyph_root = 0;
  hyph_start = 0;
}

/* module 1072 */
void
trie_initialize (void) {
  hyph_pointer z;		/* runs through the exception dictionary */
  for (z = 0; z <= hyph_size; z++) {
	hyph_word[z] = 0;
	hyph_list[z] = null;
	hyph_link[z] = 0;
  };
  hyph_count = 0;
  hyph_next = hyph_prime + 1;
  if (hyph_next > hyph_size)
    hyph_next = hyph_prime;
}

/* module 1803 */

/* The global variable |hyph_index| will point to the hyphenation codes
 * for the current language.
 */
trie_pointer hyph_start; /* root of the packed trie for |hyph_codes| */
trie_pointer hyph_index; /* pointer to hyphenation codes for |cur_lang| */

/* module 1469 */
/* Dump the hyphenation tables */
void
dump_trie (void) {
  int k;
  dump_int (hyph_count);
  if (hyph_next <= hyph_prime)
	hyph_next = hyph_size;
  dump_int (hyph_next);  /* minumum value of |hyphen_size| needed */
  for (k = 0; k <= hyph_size; k++)
	if (hyph_word[k] != 0) {
	  dump_int (k + 65536 * hyph_link[k]); 
	  /* assumes number of hyphen exceptions does not exceed 65535 */
	  dump_int (hyph_word[k]);
	  dump_int (hyph_list[k]);
	};
  print_ln();
  print_int (hyph_count);
  if (hyph_count != 1) {
	zprint_string(" hyphenation exceptions");
  } else {
	zprint_string(" hyphenation exception");
  }
  if (trie_not_ready)
	init_trie();
  dump_int (trie_max);
  dump_int (hyph_start);
  dump_things (trie_trl[0], trie_max + 1);
  dump_things (trie_tro[0], trie_max + 1);
  dump_things (trie_trc[0], trie_max + 1);
  dump_int (trie_op_ptr);
  dump_things (hyf_distance[1], trie_op_ptr);
  dump_things (hyf_num[1], trie_op_ptr);
  dump_things (hyf_next[1], trie_op_ptr);
  print_nl_string("Hyphenation trie of length ");
  print_int (trie_max);
  zprint_string(" has ");
  print_int (trie_op_ptr);
  if (trie_op_ptr != 1) {
	zprint_string(" ops");
  } else {
	zprint_string(" op");
  }
  zprint_string(" out of ");
  print_int (trie_op_size);
  for (k = 255; k >= 0; k--)
	if (trie_used[k] > min_quarterword) {
	  print_nl_string("  ");
	  print_int (qo (trie_used[k]));
	  zprint_string(" for language ");
	  print_int (k);
	  dump_int (k);
	  dump_int (qo (trie_used[k]));
	};
}

/* module 1448 */
#define too_small( arg ) { wake_up_terminal ;\
           fprintf(term_out,"%s%s\n","---! Must increase the ",arg ); return false ;}

#define UNDUMP(var,varx,vary)  { undump_int(x); if ((x<var) ||\
                                  (x>varx)) { return false; } else  vary=x;}


#define UNDUMP_SIZE(var,varw,varx,vary)  {\
                                    undump_int(x); \
                                    if (x<var) return false; if (x>varw) {\
                                    too_small(varx) } else {\
                                    FORMAT_DEBUG (varx,x); vary = x; }}

/* module 1470 */
/* Only ``nonempty'' parts of |op_start| need to be restored. */
/* Undump the hyphenation tables */
boolean
undump_trie (void) {
  int j,k; /* indices */
  int x;   /* something undumped */
  UNDUMP_SIZE (0,hyph_size,"hyph_size",hyph_count);
  UNDUMP_SIZE (hyph_prime,hyph_size,"hyph_size",hyph_next);
  j = 0;
  for (k = 1; k <= hyph_count; k++)
    {
      undump_int (j);
      if (j < 0)
		return false;
      if (j > 65535)
	{
	  hyph_next = j / 65536;
	  j = j - hyph_next * 65536;
	}
      else
	hyph_next = 0;
      if ((j >= hyph_size) || (hyph_next > hyph_size)) {
		do_something;
		return false;
	  }
      hyph_link[j] = hyph_next;
      UNDUMP (0,(integer)str_ptr,hyph_word[j]);
      UNDUMP (min_halfword,max_halfword,hyph_list[j]);
    };
  /* |j| is now the largest occupied location in |hyph_word| */
  incr (j);
  if (j < hyph_prime)
    j = hyph_prime;
  hyph_next = j;
  if (hyph_next >= hyph_size) {
    hyph_next = hyph_prime;
  } else
    if (hyph_next >= hyph_prime)
      incr (hyph_next);
  UNDUMP_SIZE (0,trie_size,"trie size",j);
  trie_max = j;
  UNDUMP (0,j,hyph_start);
  /* These first three haven't been allocated yet unless we're \.{INITEX}; w
	 e do that precisely so we don't allocate more space than necessary. */
  if (!trie_trl)
    trie_trl = xmalloc_array (trie_pointer, j + 1);
  undump_things (trie_trl[0], j + 1);
  if (!trie_tro)
    trie_tro = xmalloc_array (trie_pointer, j + 1);
  undump_things (trie_tro[0], j + 1);
  if (!trie_trc)
    trie_trc = xmalloc_array (quarterword, j + 1);
  undump_things (trie_trc[0], j + 1);
  UNDUMP_SIZE (0,trie_op_size,"trie op size",j);
  trie_op_ptr = j;
  /* I'm not sure we have such a strict limitation (64) on these values, so let's leave them unchecked. */
  undump_things (hyf_distance[1], j);
  undump_things (hyf_num[1], j);
  undump_upper_check_things (max_trie_op, hyf_next[1], (unsigned)j);
  for (k = 0; k <= 255; k++)
    trie_used[k] = min_quarterword;
  k = 256;
  while (j > 0) {
	UNDUMP (0,k - 1,k);
	UNDUMP (1,j,x);
	trie_used[k] = qi (x);
	j = j - x;
	op_start[k] = qo (j);
  };
  trie_not_ready = false;
  return true;
}
