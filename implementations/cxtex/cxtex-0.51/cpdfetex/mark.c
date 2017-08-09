
#include "types.h"
#include "c-compat.h"
#include "globals.h"

/* module 382 */

/* 
 * Before we get into the details of |macro_call|, however, let's consider the
 * treatment of primitives like \.{\\topmark}, since they are essentially
 * macros without parameters. The token lists for such marks are kept in a
 * global array of five pointers; we refer to the individual entries of this
 * array by symbolic names |top_mark|, etc. The value of |top_mark| is either
 * |null| or a pointer to the reference count of a token list.
 */

pointer cur_mark[split_bot_mark_code]; /* token lists for marks */

/* module 383 */
void
mark_initialize (void) {
  top_mark = null;
  first_mark = null;
  bot_mark = null;
  split_first_mark = null;
  split_bot_mark = null;
}


/* module 1246 */
void 
make_mark (void) {
  pointer p; /* new node */ 
  halfword c;  /* the mark class */ 
  if (cur_chr == 0) {
	c = 0;
  } else {
	scan_register_num();
	c = cur_val;
  };
  p = scan_toks (false, true);
  p = get_node (small_node_size);
  mark_class (p) = c;
  type (p) = mark_node;
  subtype (p) = 0; /* the |subtype| is not used */ 
  mark_ptr (p) = def_ref;
  link (tail) = p;
  tail = p;
};

/* module 1770 */

/* The current marks for all mark classes are maintained by the |vsplit|
 * and |fire_up| routines and are finally destroyed (for \.{INITEX} only)
 * by the |final_cleanup| routine. Apart from updating the current marks
 * when mark nodes are encountered, these routines perform certain actions
 * on all existing mark classes. The recursive |do_marks| procedure walks
 * through the whole tree or a subtree of existing mark class nodes and
 * preforms certain actions indicted by its first parameter |a|, the action
 * code. The second parameter |l| indicates the level of recursion (at
 * most four); the third parameter points to a nonempty tree or subtree.
 * The result is |true| if the complete tree or subtree has been deleted.
 */

/* module 1771 */

/* At the start of the |vsplit| routine the existing |split_first_mark|
 * and |split_bot_mark| are discarded.
 */
boolean 
do_marks (small_number a, small_number l, pointer q) {
  small_number i; /* a four bit index */ 
  if (l < 4) { /* |q| is an index node */
	for (i = 0; i <= 15; i++) {
	  get_sa_ptr;
	  if (cur_ptr != null)
		if (do_marks (a, l + 1, cur_ptr))
		  delete_sa_ptr;
	};
	if (sa_used (q) == 0) {
	  free_node (q, index_node_size);
	  q = null;
	};
  } else {	/* |q| is the node for a mark class */
	switch (a) {
	case vsplit_init:
	  if (sa_split_first_mark (q) != null) {
		delete_token_ref (sa_split_first_mark (q));
		sa_split_first_mark (q) = null;
		delete_token_ref (sa_split_bot_mark (q));
		sa_split_bot_mark (q) = null;
	  };
	  break;
	  /* module 1773 */
	  /* At the start of the |fire_up| routine the old |top_mark| and
	   * |first_mark| are discarded, whereas the old |bot_mark| becomes the new
	   * |top_mark|. An empty new |top_mark| token list is, however, discarded
	   * as well in order that mark class nodes can eventually be released. We
	   * use again the fact that |bot_mark<>null| implies |first_mark<>null|; it
	   * also knows that |bot_mark=null| implies |top_mark=first_mark=null|.
	   */
	case fire_up_init:
	  if (sa_bot_mark (q) != null) {
		if (sa_top_mark (q) != null)
		  delete_token_ref (sa_top_mark (q));
		delete_token_ref (sa_first_mark (q));
		sa_first_mark (q) = null;
		if (link (sa_bot_mark (q)) == null)	{ /* an empty token list */
		  delete_token_ref (sa_bot_mark (q));
		  sa_bot_mark (q) = null;
		} else {
		  add_token_ref (sa_bot_mark (q));
		}
		sa_top_mark (q) = sa_bot_mark (q);
	  };
	  break;
	  /* module 1774 */
	case fire_up_done:
	  if ((sa_top_mark (q) != null) && (sa_first_mark (q) == null)) {
		sa_first_mark (q) = sa_top_mark (q);
		add_token_ref (sa_top_mark (q));
	  };
	  break;
	  /* module 1776 */
	  /* Here we use the fact that the five current mark pointers in a mark
	   * class node occupy the same locations as the the first five pointers of
	   * an index node. For systems using a run-time switch to distinguish
	   * between \.{VIRTEX} and \.{INITEX}, the codewords `$|init|\ldots|tini|$'
	   * surrounding the following piece of code should be removed.
	   */
	case destroy_marks:
	  for (i = top_mark_code; i <= split_bot_mark_code; i++) {
		get_sa_ptr;
		if (cur_ptr != null)
		  {
			delete_token_ref (cur_ptr);
			put_sa_ptr (null);
		  };
	  };
	  /* end expansion of Cases for |do_marks| */
	}; /* there are no other cases */ 
	if (sa_bot_mark (q) == null)
	  if (sa_split_bot_mark (q) == null) {
		free_node (q, mark_class_node_size);
		q = null;
	  };
  };
  return (q == null);
}

void
print_mark (int p) { /* prints token list data in braces */
  print_char ('{');
  if ((p < hi_mem_min) || (p > mem_end)) {
	print_esc_string ("CLOBBERED.");
  } else {
	show_token_list (link (p), null, max_print_line - 10);
  };
  print_char ('}');
}


