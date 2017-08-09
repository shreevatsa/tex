
#include "types.h"
#include "c-compat.h"


#include "globals.h"


/* module 115 */

/* 
 * The \TeX\ system does nearly all of its own memory allocation, so that it
 * can readily be transported into environments that do not have automatic
 * facilities for strings, garbage collection, etc., and so that it can be in
 * control of what error messages the user receives. The dynamic storage
 * requirements of \TeX\ are handled by providing a large array |mem| in
 * which consecutive blocks of words are used as nodes by the \TeX\ routines.
 * 
 * Pointer variables are indices into this array, or into another array
 * called |eqtb| that will be explained later. A pointer variable might
 * also be a special flag that lies outside the bounds of |mem|, so we
 * allow pointers to assume any |halfword| value. The minimum halfword
 * value represents a null pointer. \TeX\ does not assume that |mem[null]| exists.
 */

pointer temp_ptr; /* a pointer variable for occasional emergency use */

/* module 116 */

/* The |mem| array is divided into two regions that are allocated separately,
 * but the dividing line between these two regions is not fixed; they grow
 * together until finding their ``natural'' size in a particular job.
 * Locations less than or equal to |lo_mem_max| are used for storing
 * variable-length records consisting of two or more words each. This region
 * is maintained using an algorithm similar to the one described in exercise
 * 2.5--19 of {\sl The Art of Computer Programming}. However, no size field
 * appears in the allocated nodes; the program is responsible for knowing the
 * relevant size when a node is freed. Locations greater than or equal to
 * |hi_mem_min| are used for storing one-word records; a conventional
 * \.{AVAIL} stack is used for allocation in this region.
 * 
 * Locations of |mem| between |mem_bot| and |mem_top| may be dumped as part
 * of preloaded format files, by the \.{INITEX} preprocessor.
 * 
 * Production versions of \TeX\ may extend the memory at both ends in order to
 * provide more space; locations between |mem_min| and |mem_bot| are always
 * used for variable-size nodes, and locations between |mem_top| and |mem_max|
 * are always used for single-word nodes.
 * 
 * The key pointers that govern |mem| allocation have a prescribed order:
 * $$\advance\thickmuskip-2mu
 * \hbox{|null<=mem_min<=mem_bot<lo_mem_max<
 * 
 * hi_mem_min<mem_top<=mem_end<=mem_max|.}$$
 * 
 * Empirical tests show that the present implementation of \TeX\ tends to
 * spend about 9\pct! of its running time allocating nodes, and about 6\pct!
 * deallocating them after their use.
 */

memory_word *yzmem; /* the big dynamic storage area */ 
memory_word *mem; /* the big dynamic storage area */ 
pointer lo_mem_max; /* the largest location of variable-size memory in use */ 
pointer hi_mem_min; /* the smallest location of one-word memory in use */


/* module 117 */

/* In order to study the memory requirements of particular applications, it
 * is possible to prepare a version of \TeX\ that keeps track of current and
 * maximum memory usage. When code between the delimiters | stat| $\ldots$
 * |tats| is not ``commented out,'' \TeX\ will run a bit slower but it will
 * report these statistics when |tracing_stats| is sufficiently large.
 */
integer var_used, dyn_used; /* how much memory is in use */

/* module 118 */

/* Let's consider the one-word memory region first, since it's the
 * simplest. The pointer variable |mem_end| holds the highest-numbered location
 * of |mem| that has ever been used. The free locations of |mem| that
 * occur between |hi_mem_min| and |mem_end|, inclusive, are of type
 * |two_halves|, and we write |info(p)| and |link(p)| for the |lh|
 * and |rh| fields of |mem[p]| when it is of this type. The single-word
 * free locations form a linked list
 * $$|avail|,\;\hbox{|link(avail)|},\;\hbox{|link(link(avail))|},\;\ldots$$
 * terminated by |null|.
 */

pointer avail; /* head of the list of available one-word nodes */ 
pointer mem_end; /* the last one-word node used in |mem| */

/* module 120 */

/* The function |get_avail| returns a pointer to a new one-word node whose
 * |link| field is null. However, \TeX\ will halt if there is no more room left.
 * 
 * If the available-space list is empty, i.e., if |avail=null|,
 * we try first to increase |mem_end|. If that cannot be done, i.e., if
 * |mem_end=mem_max|, we try to decrease |hi_mem_min|. If that cannot be
 * done, i.e., if |hi_mem_min=lo_mem_max+1|, we have to quit.
 */
pointer
get_avail (void) {	/* single-word node allocation */
  pointer p;			/* the new node being got */
  p = avail;			/* get top location in the |avail| stack */
  if (p != null) {
	avail = link (avail);	/* and pop it off */
  } else {
	if (mem_end < mem_max) {  /* or go into virgin territory */
	  incr (mem_end);
	  p = mem_end;
	} else {
	  decr (hi_mem_min);
	  p = hi_mem_min;
	  if (hi_mem_min <= lo_mem_max) {
		runaway();
		/* if memory is exhausted, display possible runaway text */
		overflow ("main memory size", mem_max + 1 - mem_min);
		/* quit; all one-word nodes are busy */
	  };
	};
  };
  link (p) = null; /* provide an oft-desired initialization of the new node */
  incr (dyn_used);  /* maintain statistics */
  return p;
}


/* module 123 */

/* The procedure |flush_list(p)| frees an entire linked list of
 * one-word nodes that starts at position |p|.
 */
void
flush_list (pointer p) { /* makes list of single-word nodes available */
  pointer q, r;			/* list traversers */
  if (p != null) {
	r = p;
	do {
	  q = r;
	  r = link (r);
	  decr (dyn_used);
	} while (r != null);
	/* now |q| is the last node on the list */
	link (q) = avail;
	avail = p;
  };
}


/* module 124 */

/* The available-space list that keeps track of the variable-size portion
 * of |mem| is a nonempty, doubly-linked circular list of empty nodes,
 * pointed to by the roving pointer |rover|.
 * 
 * Each empty node has size 2 or more; the first word contains the special
 * value |max_halfword| in its |link| field and the size in its |info| field;
 * the second word contains the two pointers for double linking.
 * 
 * Each nonempty node also has size 2 or more. Its first word is of type
 * |two_halves|\kern-1pt, and its |link| field is never equal to |max_halfword|.
 * Otherwise there is complete flexibility with respect to the contents
 * of its other fields and its other words.
 * 
 * (We require |mem_max<max_halfword| because terrible things can happen
 * when |max_halfword| appears in the |link| field of a nonempty node.)
 */
pointer rover; /* points to some node in the list of empties */



/* module 125 */

/* A call to |get_node| with argument |s| returns a pointer to a new node
 * of size~|s|, which must be 2~or more. The |link| field of the first word
 * of this new node is set to null. An overflow stop occurs if no suitable
 * space exists.
 * 
 * If |get_node| is called with $s=2^{30}$, it simply merges adjacent free
 * areas and returns the value |max_halfword|.
 */
pointer
get_node (int s) {	/* variable-size node allocation */
  pointer p;			/* the node currently under inspection */
  pointer q;			/* the node physically after node |p| */
  int r;			/* the newly allocated node, or a candidate for this honor */
  int t;			/* temporary register */
 RESTART:
  p = rover;
  /* start at some free node in the ring */
  do {
	/* begin expansion of Try to allocate within node |p| and its physical 
	   successors, and |goto found| if allocation was possible */
	/* module 127 */
	/* Empirical tests show that the routine in this section performs a
	 * node-merging operation about 0.75 times per allocation, on the average,
	 * after which it finds that |r>p+1| about 95\pct! of the time.
	 */
	q = p + node_size (p);
	/* find the physical successor */
	while (is_empty (q)) { /* merge node |p| with node |q| */
	  t = rlink (q);
	  if (q == rover)
	    rover = t;
	  llink (t) = llink (q);
	  rlink (llink (q)) = t;
	  q = q + node_size (q);
	};
	r = q - s;
	if (r > intcast (p + 1)) {
	  /* begin expansion of Allocate from the top of node |p| and |goto found| */
	  /* module 128 */
	  node_size (p) = r - p;	/* store the remaining size */
	  rover = p;		/* start searching here next time */
	  goto FOUND;
	};
	/* end expansion of Allocate from the top of node |p| and |goto found| */
	if (r == p) {
	  if (rlink (p) != p) {
		/* begin expansion of Allocate entire node |p| and |goto found| */
		/* module 129 */
		/* Here we delete node |p| from the ring, and let |rover| rove around.
		 */
		rover = rlink (p);
		t = llink (p);
		llink (rover) = t;
		rlink (t) = rover;
		goto FOUND;
	  }
	  /* end expansion of Allocate entire node |p| and |goto found| */
	};
	node_size (p) = q - p;	/* reset the size in case it grew */
	/* end expansion of Try to allocate within node |p| and its physical successors, .. */
	p = rlink (p);
	/* move to the next node in the ring */
  } while (p != rover);
  /* repeat until the whole list has been traversed */
  if (s == 1073741824) {
	return max_halfword;
  };
  if (lo_mem_max + 2 < hi_mem_min) {
	if (lo_mem_max + 2 <= mem_bot + max_halfword) {
	  /* begin expansion of Grow more variable-size memory and |goto restart| */
	  /* module 126 */
	  /* The lower part of |mem| grows by 1000 words at a time, unless
	   * we are very close to going under. When it grows, we simply link
	   * a new node into the available-space list. This method of controlled
	   * growth helps to keep the |mem| usage consecutive when \TeX\ is
	   * implemented on ``virtual memory'' systems.
	   */
	  if (hi_mem_min - lo_mem_max >= 1998) {
		t = lo_mem_max + 1000;
	  } else {
		t = lo_mem_max + 1 + (hi_mem_min - lo_mem_max) / 2;
	  };			/* |lo_mem_max+2<=t<hi_mem_min| */
	  p = llink (rover);
	  q = lo_mem_max;
	  rlink (p) = q;
	  llink (rover) = q;
	  if (t > mem_bot + max_halfword)
	    t = mem_bot + max_halfword;
	  rlink (q) = rover;
	  llink (q) = p;
	  link (q) = empty_flag;
	  node_size (q) = t - lo_mem_max;
	  lo_mem_max = t;
	  link (lo_mem_max) = null;
	  info (lo_mem_max) = null;
	  rover = q;
	  goto RESTART;
	  /* end expansion of Grow more variable-size memory and |goto restart| */
	};
  };
  overflow ("main memory size", mem_max + 1 - mem_min);
  /* sorry, nothing satisfactory is left */
FOUND:
  link (r) = null;		/* this node is now nonempty */
  var_used = var_used + s;	/* maintain usage statistics */
  return r;
}


/* module 130 */

/* Conversely, when some variable-size node |p| of size |s| is no longer needed,
 * the operation |free_node(p,s)| will make its words available, by inserting
 * |p| as a new empty node just before where |rover| now points.
 */
void
free_node (pointer p, halfword s) {	/* variable-size node liberation */
  pointer q;			/* |llink(rover)| */
  node_size (p) = s;
  link (p) = empty_flag;
  q = llink (rover);
  llink (p) = q;
  rlink (p) = rover;		/* set both links */
  llink (rover) = p;
  rlink (q) = p;		/* insert |p| into the ring */
  var_used = var_used - s; /* maintain statistics */
};


/* module 131 */

/* Just before \.{INITEX} writes out the memory, it sorts the doubly linked
 * available space list. The list is probably very short at such times, so a
 * simple insertion sort is used. The smallest available location will be
 * pointed to by |rover|, the next-smallest by |rlink(rover)|, etc.
 */

void
sort_avail (void) {	/* sorts the available variable-size nodes by location */
  pointer p, q, r;		/* indices into |mem| */
  pointer old_rover;		/* initial |rover| setting */
  p = get_node (1073741824);	/* merge adjacent free areas */
  p = rlink (rover);
  rlink (rover) = max_halfword;
  old_rover = rover;
  while (p != old_rover) {
	/* begin expansion of Sort \(p)|p| into the list starting at |rover| and advance |p| to |rlink(p)| */
	/* module 132 */
	/* The following |while| loop is guaranteed to
	 * terminate, since the list that starts at
	 * |rover| ends with |max_halfword| during the sorting procedure.
	 */
	if (p < rover) {
	  q = p;
	  p = rlink (q);
	  rlink (q) = rover;
	  rover = q;
	} else {
	  q = rover;
	  while (rlink (q) < p)
	    q = rlink (q);
	  r = rlink (p);
	  rlink (p) = rlink (q);
	  rlink (q) = p;
	  p = r;
	}
	/* end expansion of Sort \(p)|p| into the list starting at |rover| and advance |p| to |rlink(p)| */
  };
  p = rover;
  while (rlink (p) != max_halfword) {
	llink (rlink (p)) = p;
	p = rlink (p);
  };
  rlink (p) = rover;
  llink (rover) = p;
}

/* module 165 */

/* If \TeX\ is extended improperly, the |mem| array might get screwed up.
 * For example, some pointers might be wrong, or some ``dead'' nodes might not
 * have been freed when the last reference to them disappeared. Procedures
 * |check_mem| and |search_mem| are available to help diagnose such
 * problems. These procedures make use of two arrays called |free| and
 * |was_free| that are present only if \TeX's debugging routines have
 * been included. (You may want to decrease the size of |mem| while you
 * 
 * are debugging.)
 */

/* The debug memory arrays have not been mallocated yet. */ 
boolean free_arr[10]; /* free cells */ 
boolean was_free_arr[10]; /* previously free cells */ 
pointer was_mem_end, was_lo_max, was_hi_min; /* previous |mem_end|,|lo_mem_max |, and |hi_mem_min| */ 
boolean panicking; /* do we want to check memory constantly? */

/* module 166 */
void mem_initialize (void) {
#ifdef TEXMF_DEBUG
  was_mem_end = mem_min;	/* indicate that everything was previously free */
  was_lo_max = mem_min;
  was_hi_min = mem_max;
  panicking = false;
#endif /* TEXMF_DEBUG */
}

/* module 167 */

/* Procedure |check_mem| makes sure that the available space lists of
 * |mem| are well formed, and it optionally prints out all locations
 * that are reserved now but were free the last time this procedure was called.
 */

#ifdef TEXMF_DEBUG
void
check_mem (boolean print_locs) {
  pointer p, q;			/* current locations of interest in |mem| */
  boolean clobbered;		/* is something amiss? */
  for (p = mem_min; p <= lo_mem_max; p++)
    free_arr[p] = false;  /* you can probably do this faster */
  for (p = hi_mem_min; p <= mem_end; p++)
    free_arr[p] = false; /* ditto */
  /* begin expansion of Check single-word |avail| list */
  /* module 168 */
  p = avail;
  q = null;
  clobbered = false;
  while (p != null) {
	if ((p > mem_end) || (p < hi_mem_min)) {
	  clobbered = true;
	} else {
	  if (free_arr[p])
	    clobbered = true;
	}
	if (clobbered) {
	  print_nl_string("AVAIL list clobbered at ");
	  print_int (q);
	  goto DONE1;
	};
	free_arr[p] = true;
	q = p;
	p = link (q);
  };
 DONE1:
  /* end expansion of Check single-word |avail| list */
  /* begin expansion of Check variable-size |avail| list */
  /* module 169 */
  p = rover;
  q = null;
  clobbered = false;
  do {
	if ((p >= lo_mem_max) || (p < mem_min)) {
	  clobbered = true;
	} else {
	  if ((rlink (p) >= lo_mem_max) || (rlink (p) < mem_min)) {
		clobbered = true;
	  } else {
		if (!(is_empty (p)) || (node_size (p) < 2)
			|| (p + node_size (p) > lo_mem_max)
			|| (llink (rlink (p)) != p))
		  clobbered = true;
	  }
	};
	if (clobbered) {
	  print_nl_string("Double-AVAIL list clobbered at ");
	  print_int (q);
	  goto DONE2;
	};
	for (q = p; q <= p + node_size (p) - 1; q++) { /* mark all locations free */
	  if (free_arr[q]) {
		print_nl_string("Doubly free location at ");
		print_int (q);
		goto DONE2;
	  };
	  free_arr[q] = true;
	};
	q = p;
	p = rlink (p);
  } while (p != rover);
  /* end expansion of Check variable-size |avail| list */
 DONE2:
  /* begin expansion of Check flags of unavailable nodes */
  /* module 170 */
  p = mem_min;
  while (p <= lo_mem_max) {	/* node |p| should not be empty */
	if (is_empty (p)) {
	  print_nl_string("Bad flag at ");
	  print_int (p);
	};
	while ((p <= lo_mem_max) && !free_arr[p])
	  incr (p);
	while ((p <= lo_mem_max) && free_arr[p])
	  incr (p);
  };
  /* end expansion of Check flags of unavailable nodes */
  if (print_locs) {
	/* begin expansion of Print newly busy locations */
	/* module 171 */
	print_nl_string("New busy locs:");
	for (p = mem_min; p <= lo_mem_max; p++) {
	  if (!free_arr[p] && ((p > was_lo_max) || was_free_arr[p])) {
		print_char (' ');
		print_int (p);
	  };
	};
	for (p = hi_mem_min; p <= mem_end; p++) {
	  if (!free_arr[p] && ((p < was_hi_min) || (p > was_mem_end) || was_free_arr[p])) {
		print_char (' ');
		print_int (p);
	  };
	};
	/* end expansion of Print newly busy locations */
  };
  for (p = mem_min; p <= lo_mem_max; p++)
    was_free_arr[p] = free_arr[p];
  for (p = hi_mem_min; p <= mem_end; p++)
    was_free_arr[p] = free_arr[p];
  /* |was_free:=free| might be faster */
  was_mem_end = mem_end;
  was_lo_max = lo_mem_max;
  was_hi_min = hi_mem_min;
}
#endif /* TEXMF_DEBUG */


/* module 172 */

/* The |search_mem| procedure attempts to answer the question ``Who points
 * to node~|p|?'' In doing so, it fetches |link| and |info| fields of |mem|
 * that might not be of type |two_halves|. Strictly speaking, this is
 * 
 * undefined in \PASCAL, and it can lead to ``false drops'' (words that seem to
 * point to |p| purely by coincidence). But for debugging purposes, we want
 * to rule out the places that do {\sl not\/} point to |p|, so a few false
 * drops are tolerable.
 */

#ifdef TEXMF_DEBUG
void
search_mem (pointer p) { /* look for pointers to |p| */
  int q;			/* current position being searched */
  for (q = mem_min; q <= lo_mem_max; q++) {
	if (link (q) == p) {
	  print_nl_string("LINK(");
	  print_int (q);
	  print_char (')');
	};
	if (info (q) == p) {
	  print_nl_string("INFO(");
	  print_int (q);
	  print_char (')');
	};
  };
  for (q = hi_mem_min; q <= mem_end; q++) {
	if (link (q) == p) {
	  print_nl_string("LINK(");
	  print_int (q);
	  print_char (')');
	};
	if (info (q) == p) {
	  print_nl_string("INFO(");
	  print_int (q);
	  print_char (')');
	};
  };
  /* begin expansion of Search |eqtb| for equivalents equal to |p| */
  /* module 255 */
  /* When the debugging routine |search_mem| is looking for pointers having a
   * given value, it is interested only in regions 1 to~3 of~|eqtb|, and in the
   * first part of region~4.
   */
  for (q = active_base; q <= box_base + 255; q++) {
	if (equiv (q) == p) {
	  print_nl_string("EQUIV(");
	  print_int (q);
	  print_char (')');
	};
  };
  /* end expansion of Search |eqtb| for equivalents equal to |p| */
  /* begin expansion of Search |save_stack| for equivalents that point to |p| */
  /* module 285 */
  /* When looking for possible pointers to a memory location, it is helpful
   * to look for references from |eqtb| that might be waiting on the
   * save stack. Of course, we might find spurious pointers too; but this
   * routine is merely an aid when debugging, and at such times we are
   * grateful for any scraps of information, even if they prove to be irrelevant.
   */
  if (save_ptr > 0) {
	for (q = 0; q <= save_ptr - 1; q++) {
	  if (equiv_field (save_stack[q]) == p) {
		print_nl_string("SAVE(");
		print_int (q);
		print_char (')');
	  };
	};
	/* end expansion of Search |save_stack| for equivalents that point to |p| */
  };
  /* begin expansion of Search |hyph_list| for pointers to |p| */
  /* module 1077 */
  for (q = 0; q <= hyph_size; q++) {
	if (hyph_list[q] == p) {
	  print_nl_string("HYPH(");
	  print_int (q);
	  print_char (')');
	};
	/* end expansion of Search |hyph_list| for pointers to |p| */
  };
}
#endif /* TEXMF_DEBUG */

/* module 164 */
void
mem_initialize_init (void) {
  integer k;
  for (k = mem_bot + 1; k <= lo_mem_stat_max; k++)
	mem[k].sc = 0; /* all glue dimensions are zeroed */
  k = mem_bot;
  while (k <= lo_mem_stat_max) { /* set first words of glue specifications */
	glue_ref_count (k) = null + 1;
	stretch_order (k) = normal;
	shrink_order (k) = normal;
	k = k + glue_spec_size;
  };
  stretch (fil_glue) = unity;
  stretch_order (fil_glue) = fil;
  stretch (fill_glue) = unity;
  stretch_order (fill_glue) = fill;
  stretch (ss_glue) = unity;
  stretch_order (ss_glue) = fil;
  shrink (ss_glue) = unity;
  shrink_order (ss_glue) = fil;
  stretch (fil_neg_glue) = -unity;
  stretch_order (fil_neg_glue) = fil;
  rover = lo_mem_stat_max + 1;
  link (rover) = empty_flag;	/* now initialize the dynamic memory */
  node_size (rover) = 1000;	/* which is a 1000-word available node */
  llink (rover) = rover;
  rlink (rover) = rover;
  lo_mem_max = rover + 1000;
  link (lo_mem_max) = null;
  info (lo_mem_max) = null;
  for (k = hi_mem_stat_min; k <= mem_top; k++)
	mem[k] = mem[lo_mem_max];	/* clear list heads */
}
