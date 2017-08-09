
#include "types.h"
#include "c-compat.h"


#include "globals.h"

/* module 1761 */

/* The roots of the seven trees for the additional registers and mark
 * classes are kept in the |sa_root| array. The first six locations must
 * be dumped and undumped; the last one is also known as |sa_mark|.
 */
pointer sa_root[(mark_val+1)]; /* roots of sparse arrays */
pointer cur_ptr; /* value returned by |new_index| and |find_sa_element| */
memory_word sa_null; /* two |null| pointers */



/* module 1760 */

/* There are seven almost identical doubly linked trees, one for the
 * sparse array of the up to 32512 additional registers of each kind and
 * one for the sparse array of the up to 32767 additional mark classes.
 * The root of each such tree, if it exists, is an index node containing 16
 * pointers to subtrees for 4096 consecutive array elements. Similar index
 * nodes are the starting points for all nonempty subtrees for 4096, 256,
 * and 16 consecutive array elements. These four levels of index nodes are
 * followed by a fifth level with nodes for the individual array elements.
 * 
 * Each index node is nine words long. The pointers to the 16 possible
 * subtrees or are kept in the |info| and |link| fields of the last eight
 * words. (It would be both elegant and efficient to declare them as
 * array, unfortunately \PASCAL\ doesn't allow this.)
 * 
 * The fields in the first word of each index node and in the nodes for the
 * array elements are closely related. The |link| field points to the next
 * lower index node and the |sa_index| field contains four bits (one
 * hexadecimal digit) of the register number or mark class. For the lowest
 * index node the |link| field is |null| and the |sa_index| field indicates
 * the type of quantity (|int_avl|, |dimen_val|, |glue_val|, |mu_val|,
 * |box_val|, |tok_val|, or |mark_val|). The |sa_used| field in the index
 * nodes counts how many of the 16 pointers are non-null.
 * 
 * The |sa_index| field in the nodes for array elements contains the four
 * bits plus 16 times the type. Therefore such a node represents a count
 * or dimen register if and only if |sa_index<dimen_val_limit|; it
 * represents a skip or muskip register if and only if
 * |dimen_val_limit<=sa_index<mu_val_limit|; it represents a box register
 * if and only if |mu_val_limit<=sa_index<box_val_limit|; it represents a
 * token list register if and only if
 * |box_val_limit<=sa_index<tok_val_limit|; finally it represents a mark
 * class if and only if |tok_val_limit<=sa_index|.
 * 
 * The |new_index| procedure creates an index node (returned in |cur_ptr|)
 * having given contents of the |sa_index| and |link| fields.
 */

void 
new_index (quarterword i, pointer q) {
  small_number k; /* loop index */ 
  cur_ptr = get_node (index_node_size);
  sa_index (cur_ptr) = i;
  sa_used (cur_ptr) = 0;
  link (cur_ptr) = q;
  for (k = 1; k <= index_node_size - 1; k++) /* clear all 16 pointers */ 
	mem[cur_ptr + k] = sa_null;
};

/* module 1764 */

/* Given a type |t| and a sixteen-bit number |n|, the |find_sa_element|
 * procedure returns (in |cur_ptr|) a pointer to the node for the
 * corresponding array element, or |null| when no such element exists. The
 * third parameter |w| is set |true| if the element must exist, e.g.,
 * because it is about to be modified. The procedure has two main
 * branches: one follows the existing tree structure, the other (only used
 * when |w| is |true|) creates the missing nodes.
 * 
 * We use macros to extract the four-bit pieces from a sixteen-bit register
 * number or mark class and to fetch or store one of the 16 pointers from
 * an index node.
 */

#define if_cur_ptr_is_null_then_return_or_goto( arg ) { if (cur_ptr== null) { if(w) { goto  arg; } else { return ;}}}
#define hex_dig1( arg ) (arg / 4096)
#define hex_dig2( arg ) (( arg / 256 )  % 16)
#define hex_dig3( arg ) (( arg   / 16 )  % 16)
#define hex_dig4( arg ) ( arg   % 16)


void
find_sa_element (small_number t, halfword n, boolean w) {
  /* sets |cur_val| to sparse array element location or |null| */
  pointer q; /* for list manipulations */ 
  small_number i; /* a four bit index */ 
  cur_ptr = sa_root[t];
  if_cur_ptr_is_null_then_return_or_goto (NOT_FOUND);
  q = cur_ptr;
  i = hex_dig1 (n);
  get_sa_ptr;
  if_cur_ptr_is_null_then_return_or_goto (NOT_FOUND1);
  q = cur_ptr;
  i = hex_dig2 (n);
  get_sa_ptr;
  if_cur_ptr_is_null_then_return_or_goto (NOT_FOUND2);
  q = cur_ptr;
  i = hex_dig3 (n);
  get_sa_ptr;
  if_cur_ptr_is_null_then_return_or_goto (NOT_FOUND3);
  q = cur_ptr;
  i = hex_dig4 (n);
  get_sa_ptr;
  if ((cur_ptr == null) && w) {
    goto NOT_FOUND4;
  }
  do_something;
  return;
NOT_FOUND:
  new_index (t, null); /* create first level index node */ 
  sa_root[t] = cur_ptr;
  q = cur_ptr;
  i = hex_dig1 (n);
NOT_FOUND1:
  new_index (i, q); /* create second level index node */ 
  add_sa_ptr;
  q = cur_ptr;
  i = hex_dig2 (n);
NOT_FOUND2:
  new_index (i, q); /* create third level index node */ 
  add_sa_ptr;
  q = cur_ptr;
  i = hex_dig3 (n);
NOT_FOUND3:
  new_index (i, q); /* create fourth level index node */ 
  add_sa_ptr;
  q = cur_ptr;
  i = hex_dig4 (n);
NOT_FOUND4:
  /* begin expansion of Create a new array element of type |t| with index |i| */
  /* module 1765 */
  /* The array elements for registers are subject to grouping and have an
   * |sa_lev| field (quite analogous to |eq_level|) instead of |sa_used|.
   * Since saved values as well as shorthand definitions (created by e.g.,
   * \.{\\countdef}) refer to the location of the respective array element,
   * we need a reference count that is kept in the |sa_ref| field. An array
   * element can be deleted (together with all references to it) when its
   * |sa_ref| value is |null| and its value is the default value.
   * 
   * Skip, muskip, box, and token registers use two word nodes, their values
   * are stored in the |sa_ptr| field.
   * Count and dimen registers use three word nodes, their
   * values are stored in the |sa_int| resp.\ |sa_dim| field in the third
   * word; the |sa_ptr| field is used under the name |sa_num| to store
   * the register number. Mark classes use four word nodes. The last three
   * words contain the five types of current marks
   */
  if (t == mark_val) {		/* a mark class */
	cur_ptr = get_node (mark_class_node_size);
	mem[cur_ptr + 1] = sa_null;
	mem[cur_ptr + 2] = sa_null;
	mem[cur_ptr + 3] = sa_null;
  } else {
	if (t <= dimen_val)	{ /* a count or dimen register */
	  cur_ptr = get_node (word_node_size);
	  sa_int (cur_ptr) = 0;
	  sa_num (cur_ptr) = n;
	} else {
	  cur_ptr = get_node (pointer_node_size);
	  if (t <= mu_val)	{ /* a skip or muskip register */
		sa_ptr (cur_ptr) = zero_glue;
		add_glue_ref (zero_glue);
	  } else {
	    sa_ptr (cur_ptr) = null;  /* a box or token list register */ 
	  };
	};
	sa_ref (cur_ptr) = null; /* all registers have a reference count */ 
  };
  sa_index (cur_ptr) = 16 * t + i;
  sa_lev (cur_ptr) = level_one;
  /* end expansion of Create a new array element of type |t| with index |i| */
  link (cur_ptr) = q;
  add_sa_ptr;
};


/* module 1766 */

/* The |delete_sa_ref| procedure is called when a pointer to an array
 * element representing a register is being removed; this means that the
 * reference count should be decreased by one. If the reduced reference
 * count is |null| and the register has been (globally) assigned its
 * default value the array element should disappear, possibly together with
 * some index nodes. This procedure will never be used for mark class
 * nodes.
 */
void
delete_sa_ref (pointer q) {	/* reduce reference count */
  pointer p;			/* for list manipulations */
  small_number i;		/* a four bit index */
  small_number s;		/* size of a node */
  s = 0; /* please -Wall -O2 */
  decr (sa_ref (q));
  if (sa_ref (q) != null)
    return;
  if (sa_index (q) < dimen_val_limit) {
	if (sa_int (q) == 0) {
	  s = word_node_size;
	} else {
	  return;
	}
  } else {
	if (sa_index (q) < mu_val_limit) {
	  if (sa_ptr (q) == zero_glue) {
		delete_glue_ref (zero_glue);
	  } else {
		return;
	  };
	} else {
	  if (sa_ptr (q) != null)
	    return;
	  s = pointer_node_size;
	};
  };
  do {
	i = hex_dig4 (sa_index (q));
	p = q;
	q = link (p);
	free_node (p, s);
	if (q == null) { /* the whole tree has been freed */
	  sa_root[i] = null;
	  return;
	};
	delete_sa_ptr;
	s = index_node_size; /* node |q| is an index node */
  } while (!(sa_used (q) > 0));
};


/* module 1767 */

/* The |print_sa_num| procedure prints the register number corresponding
 * to an array element.
 */

void
print_sa_num (pointer q) { /* print register number */
  halfword n;			/* the register number */
  if (sa_index (q) < dimen_val_limit) {
	n = sa_num (q);	  /* the easy case */
  } else {
	n = hex_dig4 (sa_index (q));
	q = link (q);
	n = n + 16 * sa_index (q);
	q = link (q);
	n = n + 256 * (sa_index (q) + 16 * sa_index (link (q)));
  };
  print_int (n);
};

/* module 1768 */

/* Here is a procedure that displays the contents of an array element
 * symbolically. It is used under similar circumstances as is
 * |restore_trace| (together with |show_eqtb|) for the quantities kept in
 * the |eqtb| array.
 */

void
show_sa (pointer p, char *s) {
  small_number t;		/* the type of element */
  begin_diagnostic();
  print_char ('{');
  zprint_string (s);
  print_char (' ');
  if (p == null) {
	print_char ('?');		/* this can't happen */
  } else {
	t = sa_type (p);
	if (t < box_val) {
	  print_cmd_chr (register_cmd, p);
	} else if (t == box_val) {
	  print_esc_string ("box");
	  print_sa_num (p);
	} else if (t == tok_val) {
	  print_cmd_chr (toks_register, p);
	} else {
	  print_char ('?'); /* this can't happen either */
	}
	print_char ('=');
	if (t == int_val) {
	  print_int (sa_int (p));
	} else if (t == dimen_val) {
	  print_scaled (sa_dim (p));
	  zprint_string("pt");
	} else {
	  p = sa_ptr (p);
	  if (t == glue_val) {
		print_spec (p,"pt");
	  } else if (t == mu_val) {
		print_spec (p,"mu");
	  } else if (t == box_val) {
		if (p == null) {
		  zprint_string("void");
		} else {
		  depth_threshold = 0;
		  breadth_max = 1;
		  show_node_list (p);
		}
	  } else if (t == tok_val) {
		if (p != null)
		  show_token_list (link (p), null, 32);
	  } else  {
		print_char ('?'); /* this can't happen either */
	  }			
	};
  };
  print_char ('}');
  end_diagnostic (false);
};



/* module 1780 */

/* The task to maintain (change, save, and restore) register values is
 * essentially the same when the register is realized as sparse array
 * element or entry in |eqtb|. The global variable |sa_chain| is the head
 * of a linked list of entries saved at the topmost level |sa_level|; the
 * lists for lowel levels are kept in special save stack entries.
 */
pointer sa_chain; /* chain of saved sparse array entries */
quarterword sa_level; /* group level for |sa_chain| */


/* module 1782 */

/* The individual saved items are kept in pointer or word nodes similar
 * to those used for the array elements: a word node with value zero is,
 * however, saved as pointer node with the otherwise impossible |sa_index|
 * value |tok_val_limit|.
 */
void
sa_save (pointer p) { /* saves value of |p| */
  pointer q;			/* the new save node */
  quarterword i;		/* index field of node */
  if (cur_level != sa_level) {
	check_full_save_stack;
	save_type (save_ptr) = restore_sa;
	save_level (save_ptr) = sa_level;
	save_index (save_ptr) = sa_chain;
	incr (save_ptr);
	sa_chain = null;
	sa_level = cur_level;
  };
  i = sa_index (p);
  if (i < dimen_val_limit) {
	if (sa_int (p) == 0) {
	  q = get_node (pointer_node_size);
	  i = tok_val_limit;
	} else {
	  q = get_node (word_node_size);
	  sa_int (q) = sa_int (p);
	};
	sa_ptr (q) = null;
  } else {
	q = get_node (pointer_node_size);
	sa_ptr (q) = sa_ptr (p);
  };
  sa_loc (q) = p;
  sa_index (q) = i;
  sa_lev (q) = sa_lev (p);
  link (q) = sa_chain;
  sa_chain = q;
  add_sa_ref (p);
};


/* module 1783 */
void
sa_destroy (pointer p) { /* destroy value of |p| */
  if (sa_index (p) < mu_val_limit) {
	delete_glue_ref (sa_ptr (p));
  }  else if (sa_ptr (p) != null) {
	if (sa_index (p) < box_val_limit) {
	  flush_node_list (sa_ptr (p));
	} else {
	  delete_token_ref (sa_ptr (p));
	};
  };
};


/* module 1784 */

/* The procedure |sa_def| assigns a new value to sparse array elements,
 * and saves the former value if appropriate. This procedure is used only
 * for skip, muskip, box, and token list registers. The counterpart of
 * |sa_def| for count and dimen registers is called |sa_w_def|.
 */
void
sa_def (pointer p, halfword e) {  /* new data for sparse array elements */
  add_sa_ref (p);
  if (sa_ptr (p) == e) {
	if (tracing_assigns > 0)
	  show_sa (p, "reassigning");
	sa_destroy (p);
  } else {
	if (tracing_assigns > 0)
	  show_sa (p, "changing");
	if (sa_lev (p) == cur_level) {
	  sa_destroy (p);
	} else {
	  sa_save (p);
	}
	sa_lev (p) = cur_level;
	sa_ptr (p) = e;
	if (tracing_assigns > 0)
	  show_sa (p, "into");
  };
  delete_sa_ref (p);
};

void
sa_w_def (pointer p, int w) {
  add_sa_ref (p);
  if (sa_int (p) == w) {
	if (tracing_assigns > 0)
	  show_sa (p, "reassigning");
  } else {
	if (tracing_assigns > 0)
	  show_sa (p, "changing");
	if (sa_lev (p) != cur_level)
	  sa_save (p);
	sa_lev (p) = cur_level;
	sa_int (p) = w;
	if (tracing_assigns > 0)
	  show_sa (p, "into");
  };
  delete_sa_ref (p);
};


/* module 1785 */

/* The |sa_def| and |sa_w_def| routines take care of local definitions.
 * 
 * Global definitions are done in almost the same way, but there is no need
 * to save old values, and the new value is associated with |level_one|.
 */

void
gsa_def (pointer p, halfword e) {
  /* global |sa_def| */
  add_sa_ref (p);
  if (tracing_assigns > 0)
    show_sa (p, "globally changing");
  sa_destroy (p);
  sa_lev (p) = level_one;
  sa_ptr (p) = e;
  if (tracing_assigns > 0)
    show_sa (p, "into");
  delete_sa_ref (p);
};

void
gsa_w_def (pointer p, int w) {
  /* global |sa_w_def| */
  add_sa_ref (p);
  if (tracing_assigns > 0)
    show_sa (p, "globally changing");
  sa_lev (p) = level_one;
  sa_int (p) = w;
  if (tracing_assigns > 0)
    show_sa (p, "into");
  delete_sa_ref (p);
};


/* module 1786 */

/* The |sa_restore| procedure restores the sparse array entries pointed
 * at by |sa_chain|
 */

void
sa_restore (pointer my_p, pointer my_save_ptr) {
  pointer p;  /* sparse array element */
  do {
	p = sa_loc (sa_chain);
	if (sa_lev (p) == level_one) {
	  if (sa_index (p) >= dimen_val_limit) {
		sa_destroy (sa_chain);
	  };
	  if (tracing_restores > 0)
	    show_sa (p, "retaining");
	} else {
	  if (sa_index (p) < dimen_val_limit)  {
		if (sa_index (sa_chain) < dimen_val_limit) {
		  sa_int (p) = sa_int (sa_chain);
		} else {
		  sa_int (p) = 0;
		}
	  } else {
		sa_destroy (p);
		sa_ptr (p) = sa_ptr (sa_chain);
	  };
	  sa_lev (p) = sa_lev (sa_chain);
	  if (tracing_restores > 0)
	    show_sa (p, "restoring");
	};
	delete_sa_ref (p);
	p = sa_chain;
	sa_chain = link (p);
	if (sa_index (p) < dimen_val_limit) {
	  free_node (p, word_node_size);
	} else {
	  free_node (p, pointer_node_size);
	};
  } while (sa_chain != null);
  sa_chain = my_p;
  sa_level = save_level (my_save_ptr);

};

void
sa_initialize (void) {
  /* module 1762 */
  sa_mark = null;
  sa_null.hh.lhfield = null;
  sa_null.hh.rh = null;

  /* module 1781 */
  sa_chain = null;
  sa_level = level_zero;
}

/* module 1763 */
void
sa_initialize_init (void) {
  int i;
  for (i = int_val; i <= tok_val; i++)
	sa_root[i] = null;
}
