#include "types.h"
#include "c-compat.h"
#include "globals.h"

/* module  1111 */

/* 
 * The |vsplit| procedure, which implements \TeX's \.{\\vsplit} operation,
 * is considerably simpler than |line_break| because it doesn't have to
 * worry about hyphenation, and because its mission is to discover a single
 * break instead of an optimum sequence of breakpoints. But before we get
 * into the details of |vsplit|, we need to consider a few more basic things.
 */


/* module 1112 */

/* A subroutine called |prune_page_top| takes a pointer to a vlist and
 * returns a pointer to a modified vlist in which all glue, kern, and penalty nodes
 * have been deleted before the first box or rule node. However, the first
 * box or rule is actually preceded by a newly created glue node designed so that
 * the topmost baseline will be at distance |split_top_skip| from the top,
 * whenever this is possible without backspacing.
 * 
 * When the second argument |s| is |false| the deleted nodes are destroyed,
 * otherwise they are collected in a list starting at |split_disc|.
 * 
 * In this routine and those that follow, we make use of the fact that a
 * vertical list contains no character nodes, hence the |type| field exists
 * for each node in the list.
 */
pointer 
prune_page_top (pointer p, boolean s) { 	/* adjust top after page break */
  pointer prev_p; /* lags one step behind |p| */ 
  pointer q, r; /* temporary variables for list manipulation */
  r=null; /*TH -Wall */
  prev_p = temp_head;
  link (temp_head) = p;
  while (p != null)
	switch (type (p)) {
	case hlist_node:
	case vlist_node:
	case rule_node:
	  /* begin expansion of Insert glue for |split_top_skip| and set~|p:=null| */
	  /* module 1113 */
	  q = new_skip_param (split_top_skip_code);
	  link (prev_p) = q;
	  link (q) = p; /* now |temp_ptr=glue_ptr(q)| */ 
	  if (width (temp_ptr) > height (p)) {
		width (temp_ptr) = width (temp_ptr) - height (p);
	  } else {
		width (temp_ptr) = 0;
	  }
	  p = null;
	  /* end expansion of Insert glue for |split_top_skip| and set~|p:=null| */
	  break;
	case whatsit_node:
	case mark_node:
	case ins_node:
	  prev_p = p;
	  p = link (prev_p);
	  break;
	case glue_node:
	case kern_node:
	case penalty_node:
	  q = p;
	  p = link (q);
	  link (q) = null;
	  link (prev_p) = p;
	  if (s) {
		if (split_disc == null) {
		  split_disc = q;
		} else {
		  link (r) = q;
		}
		r = q;
	  } else {
		flush_node_list (q);
	  }
	  break;
	default:
	  confusion ("pruning");
	};
  return link (temp_head);
};


/* module 1115 */
integer last_vbreak_penalty;

/* module 1116 */

/* A global variable |best_height_plus_depth| will be set to the natural size
 * of the box that corresponds to the optimum breakpoint found by |vert_break|.
 * (This value is used by the insertion-splitting algorithm of the page builder.)
 */

scaled best_height_plus_depth; /* height of the best box, without stretching or shrinking */


/* module 1114 */

/* The next subroutine finds the best place to break a given vertical list
 * so as to obtain a box of height~|h|, with maximum depth~|d|.
 * A pointer to the beginning of the vertical list is given,
 * and a pointer to the optimum breakpoint is returned. The list is effectively
 * followed by a forced break, i.e., a penalty node with the |eject_penalty|;
 * if the best break occurs at this artificial node, the value |null| is returned.
 * 
 * An array of six |scaled| distances is used to keep track of the height
 * from the beginning of the list to the current place, just as in |line_break|.
 * In fact, we use one of the same arrays, only changing its name to reflect
 * its new significance.
 */
pointer 
vert_break (pointer p, scaled h, scaled d) {  /* finds optimum page break */
  pointer prev_p; /* if |p| is a glue node, |type(prev_p)| determines whether |p| is a legal breakpoint */
  pointer q, r; /* glue specifications */ 
  int pi; /* penalty value */ 
  int b; /* badness at a trial breakpoint */ 
  int least_cost; /* the smallest badness plus penalties found so far */ 
  pointer best_place; /* the most recent break that leads to |least_cost| */ 
  scaled prev_dp; /* depth of previous box in the list */ 
  small_number t; /* |type| of the node following a kern */ 
  pointer retval; /* our return value */
  pi = 0; best_place=null; /*TH -Wall*/
  prev_p = p; /* an initial glue node is not a legal breakpoint */
  least_cost = awful_bad;
  do_all_six (set_height_zero);
  prev_dp = 0;
  loop {
	/* begin expansion of If node |p| is a legal breakpoint, check if this break
	   is the best known, and |goto done| if |p| is null or if the page-so-far
	   is already too full to accept more stuff */
	/* module 1117 */
	/* A subtle point to be noted here is that the maximum depth~|d| might be
	 * negative, so |cur_height| and |prev_dp| might need to be corrected even
	 * after a glue or kern node.
	 */
	if (p == null) {
	  pi = eject_penalty;
	} else {
	  /* begin expansion of Use node |p| to update the current height and 
		 depth measurements; if this node is not a legal breakpoint, |goto not_found|
		 or |update_heights|, otherwise set |pi| to the associated penalty at the break */
	  /* module 1118 */
	  switch (type (p)) {
	  case hlist_node:
	  case vlist_node:
	  case rule_node:
		cur_height = cur_height + prev_dp + height (p);
		prev_dp = depth (p);
		goto NOT_FOUND;
	  case whatsit_node:
		/* begin expansion of Process whatsit |p| in |vert_break| loop, |goto not_found| */
		/* module 1561 */
		if ((subtype (p) == pdf_refxform_node) || (subtype (p) == pdf_refximage_node)) {
		  cur_height = cur_height + prev_dp + pdf_height (p);
		  prev_dp = pdf_depth (p);
		};
		goto NOT_FOUND;
		/* end expansion of Process whatsit |p| in |vert_break| loop, |goto not_found| */
	  case glue_node:
		if (precedes_break (prev_p)) {
		  pi = 0;
		} else {
		  do_something;
		  goto UPDATE_HEIGHTS;
		}
		do_something;
		break;
	  case kern_node:
		if (link (p) == null) {
		  t = penalty_node;
		} else {
		  t = type (link (p));
		}
		if (t == glue_node) {
		  pi = 0;
		} else {
		  do_something;
		  goto UPDATE_HEIGHTS;
		}
		do_something;
		break;
	  case penalty_node:
		pi = penalty (p);
		break;
	  case mark_node:
	  case ins_node:
		do_something;
		goto NOT_FOUND;
	  default:
		confusion ("vertbreak");
	  }
	  /* end expansion of Use node |p| to update the current height and depth  ...*/
	};
	/* begin expansion of Check if node |p| is a new champion breakpoint; then
	   \(go)|goto done| if |p| is a forced break or if the page-so-far is already too full */
	/* module 1119 */
	if (pi < inf_penalty) {
	  /* begin expansion of Compute the badness, |b|, using |awful_bad| if the box is too full */
	  /* module 1120 */
	  if (cur_height < h) {
		if ((active_height[3] != 0) || (active_height[4] != 0) || (active_height[5] != 0)) {
		  b = 0;
		} else {
		  b = badness (h - cur_height, active_height[2]);
		} 
	  }  else  if (cur_height - h > active_height[6]) {
		b = awful_bad;
	  } else {
		b = badness (cur_height - h, active_height[6]);
	  };
	  /* end expansion of Compute the badness, |b|, using |awful_bad| if the box is too full */
	  if (b < awful_bad) {
		if (pi <= eject_penalty) {
		  b = pi;
		} else if (b < inf_bad) {
		  b = b + pi;
		} 
	  } else {
		b = deplorable;
	  }
	  if (b <= least_cost) {
		best_place = p;
		least_cost = b;
		best_height_plus_depth = cur_height + prev_dp;
	  };
	  if ((b == awful_bad) || (pi <= eject_penalty)) {
		do_something;
		goto DONE;
	  }
	};
	/* end expansion of Check if node |p| is a new champion breakpoint;.. */
	if ((type (p) < glue_node) || (type (p) > kern_node)) {
	  do_something;
	  goto NOT_FOUND;
	}
  UPDATE_HEIGHTS:
	/* begin expansion of Update the current height and depth measurements with 
	   respect to a glue or kern node~|p| */
	/* module 1121 */
	/* Vertical lists that are subject to the |vert_break| procedure should not
	 * contain infinite shrinkability, since that would permit any amount of
	 * information to ``fit'' on one page.
	 */
	if (type (p) == kern_node) {
	  q = p;
	} else {
	  q = glue_ptr (p);
	  active_height[2 + stretch_order (q)] = active_height[2 + stretch_order (q)] + stretch (q);
	  active_height[6] = active_height[6] + shrink (q);
	  if ((shrink_order (q) != normal) && (shrink (q) != 0)) {
		print_err ("Infinite glue shrinkage found in box being split");
		help4 ("The box you are \\vsplitting contains some infinitely",
			   "shrinkable glue, e.g., `\\vss' or `\\vskip 0pt minus 1fil'.",
			   "Such glue doesn't belong there; but you can safely proceed,",
			   "since the offensive shrinkability has been made finite.");
		error();
		r = new_spec (q);
		shrink_order (r) = normal;
		delete_glue_ref (q);
		glue_ptr (p) = r;
		q = r;
	  };
	};
	cur_height = cur_height + prev_dp + width (q);
	prev_dp = 0;
	/* end expansion of Update the current height and depth measurements...*/
  NOT_FOUND:
	if (prev_dp > d) {
	  cur_height = cur_height + prev_dp - d;
	  prev_dp = d;
	};
	/* end expansion of If node |p| is a legal breakpoint, check if this break is.. */
	prev_p = p;
	p = link (prev_p);
  };
 DONE:
  retval = best_place;
  if (best_place == null) {
	last_vbreak_penalty = eject_penalty;
  } else if (type (best_place) == penalty_node) {
	last_vbreak_penalty = penalty (best_place);
  } else {
	last_vbreak_penalty = 0;
  }
  return retval;
};


/* module 1122 */

/* Now we are ready to consider |vsplit| itself. Most of
 * its work is accomplished by the two subroutines that we have just considered.
 * 
 * Given the number of a vlist box |n|, and given a desired page height |h|,
 * the |vsplit| function finds the best initial segment of the vlist and
 * returns a box for a page of height~|h|. The remainder of the vlist, if
 * any, replaces the original box, after removing glue and penalties and
 * adjusting for |split_top_skip|. Mark nodes in the split-off box are used to
 * set the values of |split_first_mark| and |split_bot_mark|; we use the
 * fact that |split_first_mark=null| if and only if |split_bot_mark=null|.
 * 
 * The original box becomes ``void'' if and only if it has been entirely
 * extracted. The extracted box is ``void'' if and only if the original
 * box was void (or if it was, erroneously, an hlist box).
 */

pointer 
vsplit (halfword n, scaled h) { /* extracts a page of height |h| from box |n| */
  pointer v; /* the box to be split */ 
  pointer p; /* runs through the vlist */ 
  pointer q; /* points to where the break occurs */ 
  cur_val = n;
  fetch_box (v);
  flush_node_list (split_disc);
  split_disc = null;
  if (sa_mark != null)
	if (do_marks (vsplit_init, 0, sa_mark))
	  sa_mark = null;
  if (split_first_mark != null) {
	delete_token_ref (split_first_mark);
	split_first_mark = null;
	delete_token_ref (split_bot_mark);
	split_bot_mark = null;
  };
  /* begin expansion of Dispense with trivial cases of void or bad boxes */
  /* module 1123 */
  if (v == null) {
	return null;
  };
  if (type (v) != vlist_node) {
	print_err  ("");
	print_esc_string ("vsplit");
	zprint_string(" needs a ");
	print_esc_string ("vbox");
	help2 ("The box you are trying to split is an \\hbox.",
		   "I can't split such a box, so I'll leave it alone.");
	error();
	return null;
  };
  /* end expansion of Dispense with trivial cases of void or bad boxes */
  q = vert_break (list_ptr (v), h, split_max_depth);
  /* begin expansion of Look at all the marks in nodes before the break, and set 
	 the final link to |null| at the break */
  /* module 1124 */
  /* It's possible that the box begins with a penalty node that is the
   * ``best'' break, so we must be careful to handle this special case correctly.
   */
  p = list_ptr (v);
  if (p == q) {
	list_ptr (v) = null;
  } else {
	loop {
	  if (type (p) == mark_node) {
		if (mark_class (p) != 0) {
		  /* begin expansion of Update the current marks for |vsplit| */
		  /* module 1772 */
		  /* We use again the fact that |split_first_mark=null| if and only if
		   * |split_bot_mark=null|.
		   */
		  find_sa_element (mark_val, mark_class (p), true);
		  if (sa_split_first_mark (cur_ptr) == null) {
			sa_split_first_mark (cur_ptr) = mark_ptr (p);
			add_token_ref (mark_ptr (p));
		  } else {
			delete_token_ref (sa_split_bot_mark (cur_ptr));
		  }
		  sa_split_bot_mark (cur_ptr) = mark_ptr (p);
		  add_token_ref (mark_ptr (p));
		  /* end expansion of Update the current marks for |vsplit| */
		} else if (split_first_mark == null) {
		  split_first_mark = mark_ptr (p);
		  split_bot_mark = split_first_mark;
		  token_ref_count (split_first_mark) = token_ref_count (split_first_mark) + 2;
		} else {
		  delete_token_ref (split_bot_mark);
		  split_bot_mark = mark_ptr (p);
		  add_token_ref (split_bot_mark);
		}; 
	  }
	  if (link (p) == q) {
		link (p) = null;
		goto DONE;
	  };
	  p = link (p);
	}
  };
 DONE:
  /* end expansion of Look at all the marks in nodes before the break, and set.. */
  q = prune_page_top (q, saving_vdiscards > 0);
  p = list_ptr (v);
  free_node (v, box_node_size);
  if (q != null)
	q = VPACK (q, 0, additional);
  change_box (q); /* the |eq_level| of the box stays the same */ 
  return vpackage (p, h, exactly, split_max_depth);
};

/* module 1805 */
void
vsplit_initialize (void) {
  page_disc = null;
  split_disc = null;
}
