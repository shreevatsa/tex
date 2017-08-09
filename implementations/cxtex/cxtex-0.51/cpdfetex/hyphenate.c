#include "types.h"
#include "c-compat.h"


#include "globals.h"

/* module 1052 */
#define append_charnode_to_t(arg) { \
    link (t) = get_avail();         \
    t = link (t);                   \
    font (t)  =  hf;                \
    character (t)  =  arg;          \
  }


/* module 1058 */
#define advance_major_tail { major_tail = link (major_tail); incr (r_count);}


/* module 1044 */

/* 
 * If a hyphen may be inserted between |hc[j]| and |hc[j+1]|, the hyphenation
 * procedure will set |hyf[j]| to some small odd number. But before we look
 * at \TeX's hyphenation procedure, which is independent of the rest of the
 * line-breaking algorithm, let us consider what we will do with the hyphens
 * it finds, since it is better to work on this part of the program before
 * forgetting what |ha| and |hb|, etc., are all about.
 */

unsigned char hyf[65]; /* odd values indicate discretionary hyphens */
pointer init_list; /* list of punctuation characters preceding the word */
boolean init_lig; /* does |init_list| represent a ligature? */
boolean init_lft; /* if so, did the ligature involve a left boundary? */

/* module 1049 */

/* The processing is facilitated by a subroutine called |reconstitute|. Given
 * a string of characters $x_j\ldots x_n$, there is a smallest index $m\ge j$
 * such that the ``translation'' of $x_j\ldots x_n$ by ligatures and kerning
 * has the form $y_1\ldots y_t$ followed by the translation of $x_{m+1}\ldots x_n$,
 * where $y_1\ldots y_t$ is some nonempty sequence of character, ligature, and
 * kern nodes. We call $x_j\ldots x_m$ a ``cut prefix'' of $x_j\ldots x_n$.
 * For example, if $x_1x_2x_3=\.{fly}$, and if the font contains `fl' as a
 * ligature and a kern between `fl' and `y', then $m=2$, $t=2$, and $y_1$ will
 * be a ligature node for `fl' followed by an appropriate kern node~$y_2$.
 * In the most common case, $x_j$~forms no ligature with $x_{j+1}$ and we
 * simply have $m=j$, $y_1=x_j$. If $m<n$ we can repeat the procedure on
 * $x_{m+1}\ldots x_n$ until the entire translation has been found.
 * 
 * The |reconstitute| function returns the integer $m$ and puts the nodes
 * $y_1\ldots y_t$ into a linked list starting at |link(hold_head)|,
 * getting the input $x_j\ldots x_n$ from the |hu| array. If $x_j=256$,
 * we consider $x_j$ to be an implicit left boundary character; in this
 * case |j| must be strictly less than~|n|. There is a
 * parameter |bchar|, which is either 256 or an implicit right boundary character
 * assumed to be present just following~$x_n$. (The value |hu[n+1]| is never
 * explicitly examined, but the algorithm imagines that |bchar| is there.)
 * 
 * If there exists an index |k| in the range $j\le k\le m$ such that |hyf[k]|
 * is odd and such that the result of |reconstitute| would have been different
 * if $x_{k+1}$ had been |hchar|, then |reconstitute| sets |hyphen_passed|
 * to the smallest such~|k|. Otherwise it sets |hyphen_passed| to zero.
 * 
 * A special convention is used in the case |j=0|: Then we assume that the
 * translation of |hu[0]| appears in a special list of charnodes starting at
 * |init_list|; moreover, if |init_lig| is |true|, then |hu[0]| will be
 * a ligature character, involving a left boundary if |init_lft| is |true|.
 * This facility is provided for cases when a hyphenated
 * word is preceded by punctuation (like single or double quotes) that might
 * affect the translation of the beginning of the word.
 */

small_number hyphen_passed; /* first hyphen in a ligature, if any */

/* module 1051 */

/* The reconstitution procedure shares many of the global data structures
 * by which \TeX\ has processed the words before they were hyphenated.
 * There is an implied ``cursor'' between characters |cur_l| and |cur_r|;
 * these characters will be tested for possible ligature activity. If
 * |ligature_present| then |cur_l| is a ligature character formed from the
 * original characters following |cur_q| in the current translation list.
 * There is a ``ligature stack'' between the cursor and character |j+1|,
 * consisting of pseudo-ligature nodes linked together by their |link| fields.
 * This stack is normally empty unless a ligature command has created a new
 * character that will need to be processed later. A pseudo-ligature is
 * a special node having a |character| field that represents a potential
 * ligature and a |lig_ptr| field that points to a |char_node| or is |null|.
 * We have
 * $$|cur_r|=\cases{|character(lig_stack)|,&if |lig_stack>null|;\cr
 * 
 * |qi(hu[j+1])|,&if |lig_stack=null| and |j<n|;\cr
 * 
 * bchar,&if |lig_stack=null| and |j=n|.\cr}$$
 */

halfword cur_l, cur_r; /* characters before and after the cursor */
pointer cur_q; /* where a ligature should be detached */
pointer lig_stack; /* unfinished business to the right of the cursor */
boolean ligature_present; /* should a ligature node be made for |cur_l|? */
boolean lft_hit, rt_hit; /* did we hit a ligature with a boundary character? */



/* module 1048 */

/* We must now face the fact that the battle is not over, even though the
 * {\def\!{\kern-1pt}%
 * hyphens have been found: The process of reconstituting a word can be nontrivial
 * because ligatures might change when a hyphen is present. {\sl The \TeX book\/}
 * discusses the difficulties of the word ``difficult'', and
 * the discretionary material surrounding a
 * hyphen can be considerably more complex than that. Suppose
 * \.{abcdef} is a word in a font for which the only ligatures are \.{b\!c},
 * \.{c\!d}, \.{d\!e}, and \.{e\!f}. If this word permits hyphenation
 * between \.b and \.c, the two patterns with and without hyphenation are
 * $\.a\,\.b\,\.-\,\.{c\!d}\,\.{e\!f}$ and $\.a\,\.{b\!c}\,\.{d\!e}\,\.f$.
 * Thus the insertion of a hyphen might cause effects to ripple arbitrarily
 * far into the rest of the word. A further complication arises if additional
 * hyphens appear together with such rippling, e.g., if the word in the
 * example just given could also be hyphenated between \.c and \.d; \TeX\
 * avoids this by simply ignoring the additional hyphens in such weird cases.}
 * 
 * Still further complications arise in the presence of ligatures that do not
 * delete the original characters. When punctuation precedes the word being
 * hyphenated, \TeX's method is not perfect under all possible scenarios,
 * because punctuation marks and letters can propagate information back and forth.
 * For example, suppose the original pre-hyphenation pair
 * \.{*a} changes to \.{*y} via a \.{\?=:} ligature, which changes to \.{xy}
 * via a \.{=:\?} ligature; if $p_{a-1}=\.x$ and $p_a=\.y$, the reconstitution
 * procedure isn't smart enough to obtain \.{xy} again. In such cases the
 * font designer should include a ligature that goes from \.{xa} to \.{xy}.
 */

/* module 1050 */
small_number 
reconstitute (small_number j, small_number n, halfword bchar, halfword hchar) {
  pointer p; /* temporary register for list manipulation */ 
  pointer t; /* a node being appended to */ 
  four_quarters q; /* character information or a lig/kern instruction */ 
  halfword cur_rh; /* hyphen character for ligature testing */ 
  halfword test_char; /* hyphen or other character for ligature testing */ 
  scaled w; /* amount of kerning */ 
  font_index k; /* position of current lig/kern instruction */ 
  hyphen_passed = 0;
  t = hold_head;
  w = 0;
  link (hold_head) = null; /* at this point |ligature_present=lft_hit=rt_hit=false| */
  /* begin expansion of Set up data structures with the cursor following position |j| */
  /* module 1052 */
  cur_l = qi (hu[j]);
  cur_q = t;
  if (j == 0) {
	ligature_present = init_lig;
	p = init_list;
	if (ligature_present)
	  lft_hit = init_lft;
	while (p > null) {
	  append_charnode_to_t (character (p));
	  p = link (p);
	};
  } else if (cur_l < non_char) {
	append_charnode_to_t (cur_l);
  }
  lig_stack = null;
  set_cur_r;
  /* end expansion of Set up data structures with the cursor following position |j| */
 CONTINUE:
  /* begin expansion of If there's a ligature or kern at the cursor position, 
	 update the data structures, possibly advancing~|j|; continue until the cursor moves */
  /* module 1053 */
  /* We may want to look at the lig/kern program twice, once for a hyphen
   * and once for a normal letter. (The hyphen might appear after the letter
   * in the program, so we'd better not try to look for both at once.)
   */
  if (cur_l == non_char) {
	k = bchar_label[hf];
	if (k == non_address) {
	  goto DONE;
	} else {
	  q = font_info[k].qqqq;
	}
  } else {
	q = char_info (hf, cur_l);
	if (char_tag (q) != lig_tag)
	  goto DONE;
	k = lig_kern_start (hf,q);
	q = font_info[k].qqqq;
	if (skip_byte (q) > stop_flag) {
	  k = lig_kern_restart (hf,q);
	  q = font_info[k].qqqq;
	};
  }; /* now |k| is the starting address of the lig/kern program */
  if (cur_rh < non_char) {
	test_char = cur_rh;
  } else {
	test_char = cur_r;
  }
  loop {
	if (next_char (q) == test_char) {
	  if (skip_byte (q) <= stop_flag) {
		if (cur_rh < non_char) {
		  hyphen_passed = j;
		  hchar = non_char;
		  cur_rh = non_char;
		  goto CONTINUE;
		}  else { //else
		  if (hchar < non_char)
			if (odd (hyf[j])) {
			  hyphen_passed = j;
			  hchar = non_char;
			};
		  if (op_byte (q) < kern_flag) {
			/* begin expansion of Carry out a ligature replacement, updating the cursor 
			   structure and possibly advancing~|j|; |goto continue| if the cursor doesn't 
			   advance, otherwise |goto done| */
			/* module 1055 */
			if (cur_l == non_char)
			  lft_hit = true;
			if (j == n)
			  if (lig_stack == null)
				rt_hit = true;
			check_interrupt; /* allow a way out in case there's an infinite ligature loop */
			switch (op_byte (q)) {
			case qi (1):
			case qi (5):
			  cur_l = rem_byte (q); /* \.{=:\?}, \.{=:\?>} */ 
			  ligature_present = true;
			  break;
			case qi (2):
			case qi (6):
			  cur_r = rem_byte (q); /* \.{\?=:}, \.{\?=:>} */ 
			  if (lig_stack > null) {
				character (lig_stack) = cur_r;
			  } else {
				lig_stack = new_lig_item (cur_r);
				if (j == n) {
				  bchar = non_char;
				} else {
				  p = get_avail();
				  lig_ptr (lig_stack) = p;
				  character (p) = qi (hu[j + 1]);
				  font (p) = hf;
				};
			  };
			  break;
			case qi (3):
			  cur_r = rem_byte (q); /* \.{\?=:\?} */ 
			  p = lig_stack;
			  lig_stack = new_lig_item (cur_r);
			  link (lig_stack) = p;
			  break;
			case qi (7):
			case qi (11):
			  wrap_lig (false); /* \.{\?=:\?>}, \.{\?=:\?>>} */ 
			  cur_q = t;
			  cur_l = rem_byte (q);
			  ligature_present = true;
			  break;
			default:
			  cur_l = rem_byte (q);
			  ligature_present = true; /* \.{=:} */ 
			  if (lig_stack > null) {
				pop_lig_stack;
			  } else if (j == n) {
				goto DONE;
			  } else {
				append_charnode_to_t (cur_r);
				incr (j);
				set_cur_r;
			  };
			};
			if (op_byte (q) > qi (4))
			  if (op_byte (q) != qi (7))
				goto DONE;
			do_something; 
			goto CONTINUE;
		  };
		  /* end expansion of Carry out a ligature replacement ...*/
		  w = char_kern (hf,q);
		  goto DONE; /* this kern will be inserted below */ 
		};
	  }
	}
	if (skip_byte (q) >= stop_flag) {
	  if (cur_rh == non_char) {
		goto DONE;
	  } else {
		cur_rh = non_char;
		goto CONTINUE;
	  };
	};
	k = k + qo (skip_byte (q)) + 1;
	q = font_info[k].qqqq;
  };
 DONE:
  /* end expansion of If there's a ligature or kern at the cursor ... */

  /* begin expansion of Append a ligature and/or kern to the translation; |goto continue| 
	 if the stack of inserted ligatures is nonempty */
  wrap_lig (rt_hit);
  if (w != 0) {
	link (t) = new_kern (w);
	t = link (t);
	w = 0;
  };
  if (lig_stack > null) {
	cur_q = t;
	cur_l = character (lig_stack);
	ligature_present = true;
	pop_lig_stack;
	goto CONTINUE;
  }
  /* end expansion of Append a ligature and/or kern to the translation; |goto con... */
  return j;
};


/* module 1063 */

/* 
 * When a word |hc[1..hn]| has been set up to contain a candidate for hyphenation,
 * \TeX\ first looks to see if it is in the user's exception dictionary. If not,
 * hyphens are inserted based on patterns that appear within the given word,
 * using an algorithm due to Frank~M. Liang.
 * 
 * Let's consider Liang's method first, since it is much more interesting than the
 * exception-lookup routine. The algorithm begins by setting |hyf[j]| to zero
 * for all |j|, and invalid characters are inserted into |hc[0]|
 * and |hc[hn+1]| to serve as delimiters. Then a reasonably fast method is
 * used to see which of a given set of patterns occurs in the word
 * |hc[0..(hn+1)]|. Each pattern $p_1\ldots p_k$ of length |k| has an associated
 * sequence of |k+1| numbers $n_0\ldots n_k$; and if the pattern occurs in
 * |hc[(j+1)..(j+k)]|, \TeX\ will set |hyf[j+i]:=@tmax@>(hyf[j+i],@t$n_i$@>)| for
 * |0<=i<=k|. After this has been done for each pattern that occurs, a
 * discretionary hyphen will be inserted between |hc[j]| and |hc[j+1]| when
 * |hyf[j]| is odd, as we have already seen.
 * 
 * The set of patterns $p_1\ldots p_k$ and associated numbers $n_0\ldots n_k$
 * depends, of course, on the language whose words are being hyphenated, and
 * on the degree of hyphenation that is desired. A method for finding
 * appropriate |p|'s and |n|'s, from a given dictionary of words and acceptable
 * hyphenations, is discussed in Liang's Ph.D. thesis (Stanford University,
 * 1983); \TeX\ simply starts with the patterns and works from there.
 */

void 
hyphenate (void) {
  /* module 1045 */
  unsigned char i, j, l; /* indices into |hc| or |hu| */ 
  pointer q, r, s; /* temporary registers for list manipulation */ 
  halfword bchar; /* right boundary character of hyphenated word, or |non_char| */
  /* module 1056 */
  /* Okay, we're ready to insert the potential hyphenations that were found.
   * When the following program is executed, we want to append the word
   * |hu[1..hn]| after node |ha|, and node |q| should be appended to the result.
   * During this process, the variable |i| will be a temporary
   * index into |hu|; the variable |j| will be an index to our current position
   * in |hu|; the variable |l| will be the counterpart of |j|, in a discretionary
   * branch; the variable |r| will point to new nodes being created; and
   * we need a few new local variables:
   */
  pointer major_tail, minor_tail;  /* the end of lists in the main and discretionary branches being reconstructed */
  ASCII_code c; /* character temporarily replaced by a hyphen */ 
  unsigned char c_loc; /* where that character came from */ 
  int r_count; /* replacement count for discretionary */ 
  pointer hyf_node; /* the hyphen, if it exists */
  /* module 1066 */
  trie_pointer z; /* an index into |trie| */ 
  int v; /* an index into |hyf_distance|, etc. */
  /* module 1073 */
  /* The algorithm for exception lookup is quite simple, as soon as we have
   * a few more local variables to work with.
   */
  hyph_pointer h; /* an index into |hyph_word| and |hyph_list| */ 
  str_number k; /* an index into |str_start| */ 
  pool_pointer u; /* an index into |str_pool| */
  c = 0; /*TH -Wall*/
  /* begin expansion of Find hyphen locations for the word in |hc|, or |return| */
  /* module 1067 */
  /* Assuming that these auxiliary tables have been set up properly, the
   * hyphenation algorithm is quite short. In the following code we set |hc[hn+2]|
   * to the impossible value 256, in order to guarantee that |hc[hn+3]| will
   * never be fetched.
   */
  for (j = 0; j <= hn; j++)
	hyf[j] = 0;
  /* begin expansion of Look for the word |hc[1..hn]| in the exception table,
	 and |goto found| (with |hyf| containing the hyphens) if an entry is found */
  /* module 1074 */
  /* First we compute the hash code |h|, then we search until we either
   * find the word or we don't. Words from different languages are kept
   * separate by appending the language code to the string.
   */
   h = hc[1];
   incr (hn);
   hc[hn] = cur_lang;
   for (j = 2; j <= hn; j++)
	 h = (h + h + hc[j]) % hyph_prime;
   loop {
	 /* begin expansion of If the string |hyph_word[h]| is less than \(hc)|hc[1..hn]|, 
		|goto not_found|; but if the two strings are equal, set |hyf| to the hyphen positions and |goto found| */
	 /* module 1075 */
	 /* This is now a simple hash list, not an ordered one, so
		the module title is no longer descriptive. */ 
	 k = hyph_word[h];
	 if (k == 0)
	   goto NOT_FOUND;
	 if (length (k) == hn) {
	   j = 1;
	   u = str_start[k];
	   do {
		 if (str_pool[u] != hc[j])
		   goto DONE;
		 incr (j);
		 incr (u);
	   } while (!(j > hn));
	   /* begin expansion of Insert hyphens as specified in |hyph_list[h]| */
	   /* module 1076 */
	   s = hyph_list[h];
	   while (s != null) {
		 hyf[info (s)] = 1;
		 s = link (s);
	   }
	   /* end expansion of Insert hyphens as specified in |hyph_list[h]| */
	   decr (hn);
	   goto FOUND;
	 };
   DONE:
	 /* end expansion of If the string |hyph_word[h]| is less than ...*/
	 h = hyph_link[h];
	 if (h == 0)
	   goto NOT_FOUND;
	 decr (h);
   };
 NOT_FOUND:
   decr (hn);
   /* end expansion of Look for the word |hc[1..hn]| in the exception table,...*/
   if (trie_char (cur_lang + 1) != qi (cur_lang))
	 return;   /* no patterns for |cur_lang| */ 
   hc[0] = 0;
   hc[hn + 1] = 0;
   hc[hn + 2] = 256; /* insert delimiters */ 
   for (j = 0; j <= hn - r_hyf + 1; j++) {
	 z = trie_link (cur_lang + 1) + hc[j];
	 l = j;
	 while (hc[l] == qo (trie_char (z))) {
	   if (trie_op (z) != min_trie_op) {
		 /* begin expansion of Store \(m)maximum values in the |hyf| table */
		 /* module 1068 */
		 v = trie_op (z);
		 do {
		   v = v + op_start[cur_lang];
		   i = l - hyf_distance[v];
		   if (hyf_num[v] > hyf[i])
			 hyf[i] = hyf_num[v];
		   v = hyf_next[v];
		 } while (v != min_trie_op);
	   };
	   /* end expansion of Store \(m)maximum values in the |hyf| table */
	   incr (l);
	   z = trie_link (z) + hc[l];
	 };
   };
 FOUND:
   for (j = 0; j <= l_hyf - 1; j++)
	 hyf[j] = 0;
   for (j = 0; j <= r_hyf - 1; j++)
	 hyf[hn - j] = 0;
   /* end expansion of Find hyphen locations for the word in |hc|, or |return| */
   /* begin expansion of If no hyphens were found, |return| */
   /* module 1046 */
   /* \TeX\ will never insert a hyphen that has fewer than
	* \.{\\lefthyphenmin} letters before it or fewer than
	* \.{\\righthyphenmin} after it; hence, a short word has
	* comparatively little chance of being hyphenated. If no hyphens have
	* been found, we can save time by not having to make any changes to the
	* paragraph.
	*/
   for (j = l_hyf; j <= hn - r_hyf; j++) {
	 if (odd (hyf[j])) {
	   goto FOUND1;
	 }
   }
   do_something; 
   return;
   /* end expansion of If no hyphens were found, |return| */
 FOUND1:
   /* begin expansion of Replace nodes |ha..hb| by a sequence of nodes that includes the discretionary hyphens */
   /* module 1047 */
   /* If hyphens are in fact going to be inserted, \TeX\ first deletes the
	* subsequence of nodes between |ha| and~|hb|. An attempt is made to
	* preserve the effect that implicit boundary characters and punctuation marks
	* had on ligatures inside the hyphenated word, by storing a left boundary or
	* preceding character in |hu[0]| and by storing a possible right boundary
	* in |bchar|. We set |j:=0| if |hu[0]| is to be part of the reconstruction;
	* otherwise |j:=1|.
	* The variable |s| will point to the tail of the current hlist, and
	* |q| will point to the node following |hb|, so that
	* things can be hooked up after we reconstitute the hyphenated word.
	*/
   q = link (hb);
   link (hb) = null;
   r = link (ha);
   link (ha) = null;
   bchar = hyf_bchar;
   if (is_char_node (ha))
	 if (font (ha) != hf) {
	   goto FOUND2;
	 } else {
	   init_list = ha;
	   init_lig = false;
	   hu[0] = qo (character (ha));
	 } else if (type (ha) == ligature_node)
	   if (font (lig_char (ha)) != hf) {
		 goto FOUND2;
	   } else {
		 init_list = lig_ptr (ha);
		 init_lig = true;
		 init_lft = (subtype (ha) > 1);
		 hu[0] = qo (character (lig_char (ha)));
		 if (init_list == null)
		   if (init_lft) {
			 hu[0] = 256;
			 init_lig = false;
		   }; /* in this case a ligature will be reconstructed from scratch */
		 free_node (ha, small_node_size);
	   } else { /* no punctuation found; look for left boundary */
		 if (!is_char_node (r))
		   if (type (r) == ligature_node)
			 if (subtype (r) > 1)
			   goto FOUND2;
		 j = 1;
		 s = ha;
		 init_list = null;
		 goto COMMON_ENDING;
	   };
   s = cur_p; /* we have |cur_p<>ha| because |type(cur_p)=glue_node| */
   while (link (s) != ha)
	 s = link (s);
   j = 0;
   goto COMMON_ENDING;
 FOUND2:
   s = ha;
   j = 0;
   hu[0] = 256;
   init_lig = false;
   init_list = null;
 COMMON_ENDING:
   flush_node_list (r);
   /* begin expansion of Reconstitute nodes for the hyphenated word, inserting discretionary hyphens */
   /* module 1057 */
   /* When the following code is performed, |hyf[0]| and |hyf[hn]| will be zero.
	*/
   do {
	 l = j;
	 j = reconstitute (j, hn, bchar, qi (hyf_char)) + 1;
	 if (hyphen_passed == 0) {
	   link (s) = link (hold_head);
	   while (link (s) > null)
		 s = link (s);
	   if (odd (hyf[j - 1])) {
		 l = j;
		 hyphen_passed = j - 1;
		 link (hold_head) = null;
	   };
	 };
	 if (hyphen_passed > 0)
	   /* begin expansion of Create and append a discretionary node as an alternative to 
		  the unhyphenated word, and continue to develop both branches until they become equivalent */
	   /* module 1058 */
	   /* In this repeat loop we will insert another discretionary if |hyf[j-1]| is
		* odd, when both branches of the previous discretionary end at position |j-1|.
		* Strictly speaking, we aren't justified in doing this, because we don't know
		* that a hyphen after |j-1| is truly independent of those branches. But in almost
		* all applications we would rather not lose a potentially valuable hyphenation
		* point. (Consider the word `difficult', where the letter `c' is in position |j|.)
		*/
	   do {
		 r = get_node (small_node_size);
		 link (r) = link (hold_head);
		 type (r) = disc_node;
		 major_tail = r;
		 r_count = 0;
		 while (link (major_tail) > null)
		   advance_major_tail;
		 i = hyphen_passed;
		 hyf[i] = 0;
		 /* begin expansion of Put the \(c)characters |hu[l..i]| and a hyphen into |pre_break(r)| */
		 /* module 1059 */
		 /* The new hyphen might combine with the previous character via ligature
		  * or kern. At this point we have |l-1<=i<j| and |i<hn|.
		  */
		 minor_tail = null;
		 pre_break (r) = null;
		 hyf_node = new_character (hf, hyf_char);
		 if (hyf_node != null) {
		   incr (i);
		   c = hu[i];
		   hu[i] = hyf_char;
		   free_avail (hyf_node);
		 };
		 while (l <= i) {
		   l = reconstitute (l, i, font_bchar[hf], non_char) + 1;
		   if (link (hold_head) > null) {
			 if (minor_tail == null) {
			   pre_break (r) = link (hold_head);
			 } else {
			   link (minor_tail) = link (hold_head);
			 }
			 minor_tail = link (hold_head);
			 while (link (minor_tail) > null)
			   minor_tail = link (minor_tail);
		   };
		 };
		 if (hyf_node != null) {
		   hu[i] = c; /* restore the character in the hyphen position */
		   l = i;
		   decr (i);
		 }
		 /* end expansion of Put the \(c)characters |hu[l..i]| and a hyphen into |pre_break(r)| */
		 /* begin expansion of Put the \(c)characters |hu[i+1..]| into |post_break(r)|, appending to 
			this list and to |major_tail| until synchronization has been achieved */
		 /* module 1060 */
		 /* The synchronization algorithm begins with |l=i+1<=j|.  */
		 minor_tail = null;
		 post_break (r) = null;
		 c_loc = 0;
		 if (bchar_label[hf] != non_address) {	/* put left boundary at beginning of new line */
		   decr (l);
		   c = hu[l];
		   c_loc = l;
		   hu[l] = 256;
		 };
		 while (l < j) {
		   do {
			 l = reconstitute (l, hn, bchar, non_char) + 1;
			 if (c_loc > 0) {
			   hu[c_loc] = c;
			   c_loc = 0;
			 };
			 if (link (hold_head) > null) {
			   if (minor_tail == null) {
				 post_break (r) = link (hold_head);
			   } else {
				 link (minor_tail) = link (hold_head);
			   }
			   minor_tail = link (hold_head);
			   while (link (minor_tail) > null)
				 minor_tail = link (minor_tail);
			 };
		   } while (!(l >= j));
		   while (l > j) {
			 /* begin expansion of Append characters of |hu[j..]| to |major_tail|, advancing~|j| */
			 /* module 1061 */
			 j = reconstitute (j, hn, bchar, non_char) + 1;
			 link (major_tail) = link (hold_head);
			 while (link (major_tail) > null)
			   advance_major_tail;
		   }
		   /* end expansion of Append characters of |hu[j..]| to |major_tail|, advancing~|j| */
		 }
		 /* end expansion of Put the \(c)characters |hu[i+1..]| into |post_break(r)| ...*/
		 
		 /* begin expansion of Move pointer |s| to the end of the current list, and set |replace_count(r)| appropriately */
		 /* module 1062 */
		 /* Ligature insertion can cause a word to grow exponentially in size. Therefore
		  * we must test the size of |r_count| here, even though the hyphenated text
		  * was at most 63 characters long.
		  */
		 if (r_count > 127)	{ /* we have to forget the discretionary hyphen */
		   link (s) = link (r);
		   link (r) = null;
		   flush_node_list (r);
		 } else {
		   link (s) = r;
		   replace_count (r) = r_count;
		 };
		 s = major_tail;
		 /* end expansion of Move pointer |s| to the end of the current list, and set |replace_count(r)| appropriately */
		 hyphen_passed = j - 1;
		 link (hold_head) = null;
	   } while (odd (hyf[j - 1]));
	 /* end expansion of Create and append a discretionary node as an alternative to the ...*/
   } while (!(j > hn));
   link (s) = q;
   /* end expansion of Reconstitute nodes for the hyphenated word, inserting discretionary hyphens */
   flush_list (init_list);
   /* end expansion of Replace nodes |ha..hb| by a sequence of nodes that includes the discretionary hyphens */
};
