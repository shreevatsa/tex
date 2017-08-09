
#include "types.h"
#include "c-compat.h"


#include "globals.h"

	  /* module 1646 */
	  /* The optional |TeXXeT| feature of \eTeX\ contains the code for mixed
	   * left-to-right and right-to-left typesetting. This code is inspired by
	   * but different from \TeXeT\ as presented by Donald~E. Knuth and Pierre
	   * MacKay in {\sl TUGboat\/} {\bf 8}, 14--25, 1987.
	   * 
	   * In order to avoid confusion with \TeXeT\ the present implementation of
	   * mixed direction typesetting is called \TeXXeT. It differs from \TeXeT\
	   * in several important aspects: (1)~Right-to-left text is reversed
	   * explicitely by the |ship_out| routine and is written to a normal \.{DVI}
	   * file without any |begin_reflect| or |end_reflect| commands; (2)~a
	   * |math_node| is (ab)used instead of a |whatsit_node| to record the
	   * \.{\\beginL}, \.{\\endL}, \.{\\beginR}, and \.{\\endR} text direction
	   * primitives in order to keep the influence on the line breaking algorithm
	   * for pure left-to-right text as small as possible; (3)~right-to-left text
	   * interrupted by a displayed equation is automatically resumed after that
	   * equation; and (4)~the |valign| command code with a non-zero command
	   * modifier is (ab)used for the text direction primitives.
	   * 
	   * Nevertheless there is a subtle difference between \TeX\ and \TeXXeT\
	   * that may influence the line breaking algorithm for pure left-to-right
	   * text. When a paragraph containing math mode material is broken into
	   * lines \TeX\ may generate lines where math mode material is not enclosed
	   * by properly nested \.{\\mathon} and \.{\\mathoff} nodes. Unboxing such
	   * lines as part of a new paragraph may have the effect that hyphenation is
	   * attempted for `words' originating from math mode or that hyphenation is
	   * inhibited for words originating from horizontal mode.
	   * 
	   * In \TeXXeT\ additional \.{\\beginM}, resp.\ \.{\\endM} math nodes are
	   * supplied at the start, resp.\ end of lines such that math mode material
	   * inside a horizontal list always starts with either \.{\\mathon} or
	   * \.{\\beginM} and ends with \.{\\mathoff} or \.{\\endM}. These
	   * additional nodes are transparent to operations such as \.{\\unskip},
	   * \.{\\lastpenalty}, or \.{\\lastbox} but they do have the effect that
	   * hyphenation is never attempted for `words' originating from math mode
	   * and is never inhibited for words originating from horizontal mode.
	   */

/* module 1651 */

/* A number of routines are based on a stack of one-word nodes whose
 * |info| fields contain |end_M_code|, |end_L_code|, or |end_R_code|. The
 * top of the stack is pointed to by |LR_ptr|.
 * 
 * When the stack manipulation macros of this section are used below,
 * variable |LR_ptr| might be the global variable declared here for |hpack|
 * and |ship_out|, or might be local to |post_line_break|.
 */

pointer LR_temp; /* holds a temporarily removed \.{\\endM} node */
pointer LR_ptr; /* stack of LR codes for |hpack|, |ship_out|, and |init_math| */
int LR_problems; /* counts missing begins and ends */
small_number cur_dir; /* current text direction */


/* module 1652 */
void
xet_initialize (void) {
  LR_temp = null;
  LR_ptr = null;
  LR_problems = 0;
  cur_dir = left_to_right;
}



/* module 1656 */

/* Special \.{\\beginM} and \.{\\endM} nodes are inserted in cases where
 * math nodes are discarded during line breaking or end up in different
 * lines. When the current lists ends with an \.{\\endM} node that node is
 * temporarily removed and later reinserted when the last node is to be
 * inspected or removed. A final \.{\\endM} preceded by a |char_node| will
 * not be removed.
 */

void remove_end_M (void) {
  pointer p; /* runs through the current list */ 
  p = head;
  while (link (p) != tail)
    p = link (p);
  if (!is_char_node (p)) {
	LR_temp = tail;
	link (p) = null;
	tail = p;
  };
};

/* module 1657 */
void
insert_end_M (void) {
  pointer p; /* runs through the current list */ 
  if (!is_char_node (tail))
    if ((type (tail) == math_node) && (subtype (tail) == begin_M_code)) {
	  free_node (LR_temp, small_node_size);
	  p = head;
	  while (link (p) != tail)
		p = link (p);
	  free_node (tail, small_node_size);
	  link (p) = null;
	  tail = p;
	  goto DONE;
	};
  link (tail) = LR_temp;
  tail = LR_temp;
 DONE:
  LR_temp = null;
};


/* module 1664 */
pointer
new_edge (small_number s, scaled w) {	  /* create an edge node */
  pointer p; /* the new node */ 
  p = get_node (edge_node_size);
  type (p) = edge_node;
  subtype (p) = s;
  width (p) = w;
  edge_dist (p) = 0; /* the |edge_dist| field will be set later */ 
  return p;
};

/* module 1668 */

/* The |reverse| function defined here is responsible to reverse the
 * nodes of an hlist (segment). The first parameter |this_box| is the
 * enclosing hlist node, the second parameter |t| is to become the tail of
 * the reversed list, and the global variable |temp_ptr| is the head of the
 * list to be reversed. We remove nodes from the original list and add them
 * to the head of the new one.
 */

pointer
reverse (pointer this_box, pointer t) {
  pointer l; /* the new list */ 
  pointer p; /* the current node */ 
  pointer q; /* the next node */ 
  unsigned char g_sign; /* selects type of glue */ 
  halfword m, n; /* count of unmatched math nodes */ 
  g_sign = glue_sign (this_box);
  l = t;
  p = temp_ptr;
  m = min_halfword;
  n = min_halfword;
  loop {
    while (p != null)
	  /* begin expansion of Move node |p| to the new list and go to the next node; or 
		 |goto done| if the end of the reflected segment has been reached */
	  /* module 1669 */
	  RESWITCH:
	if (is_char_node (p)) {
	  do {
	    f = font (p);
	    c = character (p);
	    cur_h = cur_h + char_width (f, char_info (f, c));
	    q = link (p);
	    link (p) = l;
	    l = p;
	    p = q;
	  } while (is_char_node (p)); 
	} else {
	  /* begin expansion of Move the non-|char_node| |p| to the new list */
	  /* module 1670 */
	  q = link (p);
	  switch (type (p)) {
	  case hlist_node:
	  case vlist_node:
	  case rule_node:
	  case kern_node:
		rule_wd = width (p);
		break;
		/* begin expansion of Cases of |reverse| that need special treatment */
		/* module 1671 */
		/* Here we have to remember that |add_glue| may have converted the glue
		 * node into a kern node. If this is not the case we try to covert the
		 * glue node into a rule node.
		 */
	  case glue_node:
		g = glue_ptr (p);
		rule_wd = width (g);
		if (g_sign != normal)
		  add_glue (rule_wd);
		if (subtype (p) >= a_leaders) {
		  temp_ptr = leader_ptr (p);
		  if (type (temp_ptr) == rule_node) {
			delete_glue_ref (g);
			free_node (p, small_node_size);
			p = temp_ptr;
			width (p) = rule_wd;
		  };
		};
		break;
		/* module 1672 */
		/* A ligature node is replaced by a char node. */
	  case ligature_node:
		flush_node_list (lig_ptr (p));
		temp_ptr = p;
		p = get_avail();
		mem[p] = mem[lig_char (temp_ptr)];
		link (p) = q;
		free_node (temp_ptr, small_node_size);
		goto RESWITCH;
		break;
		/* module 1673 */
		/* Math nodes in an inner reflected segment are modified, those at the
		 * outer level are changed into kern nodes.
		 */
	  case math_node:
		rule_wd = width (p);
		if (end_LR (p))
		  if (info (LR_ptr) != end_LR_type (p)){
			type (p) = kern_node;
			incr (LR_problems);
		  } else {
			pop_LR;
			if (n > min_halfword) {
			  decr (n);
			  decr (subtype (p));				/* change |after| into |before| */ 
			} else {
			  type (p) = kern_node;
			  if (m > min_halfword) {
				decr (m);
			  } else {
				/* begin expansion of Finish the reversed hlist segment and |goto done| */
				/* module 1674 */
				/* Finally we have found the end of the hlist segment to be reversed; the
				 * final math node is released and the remaining list attached to the
				 * edge node terminating the reversed segment.
				 */
				free_node (p, small_node_size);
				link (t) = q;
				width (t) = rule_wd;
				edge_dist (t) = -cur_h - rule_wd;
				goto DONE;
			  };
			  /* end expansion of Finish the reversed hlist segment and |goto done| */
			};
		  } else {
			push_LR (p);
			if ((n > min_halfword) || (LR_dir (p) != cur_dir)) {
			  incr (n);
			  incr (subtype (p)); /* change |before| into |after| */ 
			} else {
			  type (p) = kern_node;
			  incr (m);
			};
		  };
		break;
		/* end expansion of Cases of |reverse| that need special treatment */
	  case edge_node:
		confusion ("LR2");
		break;
	  default:
		do_something;
		goto NEXTP;
	  };
	  cur_h = cur_h + rule_wd;
	NEXTP:
	  link (p) = l;
	  if (type (p) == kern_node)
	    if ((rule_wd == 0) || (l == null)) {
		  free_node (p, small_node_size);
		  p = l;
		};
	  l = p;
	  p = q;
	};
	/* end expansion of Move the non-|char_node| |p| to the new list */
	/* end expansion of Move node |p| to the new list and go to the... */
	if ((t == null) && (m == min_halfword) && (n == min_halfword))
	  goto DONE;
	p = new_math (0, info (LR_ptr));
	LR_problems = LR_problems + 10000; /* manufacture one missing math node */ 
  };
 DONE:
  return l;
};
