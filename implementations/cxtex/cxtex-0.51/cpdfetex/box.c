
#include "types.h"
#include "c-compat.h"
#include "globals.h"


/* module 136 */

/* The |new_null_box| function returns a pointer to an |hlist_node| in
 * which all subfields have the values corresponding to `\.{\\hbox\{\}}'.
 * The |subtype| field is set to |min_quarterword|, since that's the desired
 * |span_count| value if this |hlist_node| is changed to an |unset_node|.
 */
pointer
new_null_box (void) { /* creates a new box node */
  pointer p;			/* the new node */
  p = get_node (box_node_size);
  type (p) = hlist_node;
  subtype (p) = min_quarterword;
  width (p) = 0;
  depth (p) = 0;
  height (p) = 0;
  shift_amount (p) = 0;
  list_ptr (p) = null;
  glue_sign (p) = normal;
  glue_order (p) = normal;
  set_glue_ratio_zero (glue_set (p));
  return p;
};


/* module 181 */

/* A global variable called |depth_threshold| is used to record the maximum
 * depth of nesting for which |show_node_list| will show information. If we
 * have |depth_threshold=0|, for example, only the top level information will
 * be given and no sublists will be traversed. Another global variable, called
 * |breadth_max|, tells the maximum number of items to show at each level;
 * |breadth_max| had better be positive, or you won't see anything.
 */

integer depth_threshold; /* maximum nesting depth in box displays */ 
int breadth_max; /* maximum number of items shown at the same list level */

/* module 198 */

/* The recursive machinery is started by calling |show_box|.
 */
void
show_box (pointer p) {
  /* begin expansion of Assign the values |depth_threshold:=show_box_depth| 
	 and |breadth_max:=show_box_breadth| */
  /* module 236 */
  depth_threshold = show_box_depth;
  breadth_max = show_box_breadth;
  /* end expansion of Assign the values |depth_threshold:=show_box_depth| .... */
  if (breadth_max <= 0)
    breadth_max = 5;
  if (pool_ptr + depth_threshold >= pool_size)
    depth_threshold = pool_size - pool_ptr - 1;	/* now there's enough room for prefix string */
  show_node_list (p);		/* the show starts at |p| */
  print_ln();
}

/* module 1219 */

/* The global variable |cur_box| will point to a newly-made box. If the box
 * is void, we will have |cur_box=null|. Otherwise we will have
 * |type(cur_box)=hlist_node| or |vlist_node| or |rule_node|; the |rule_node|
 * case can occur only with leaders.
 */

pointer cur_box; /* box to be placed into its context */

/* module 1784 */
#define sa_def_box { find_sa_element ( box_val , cur_val , true );\
                    if (global) { gsa_def ( cur_ptr , cur_box ) ; } else { sa_def ( cur_ptr , cur_box );} }


/* module 1220 */

/* The |box_end| procedure does the right thing with |cur_box|, if
 * |box_context| represents the context as explained above.
 */
void box_end (int my_box_context) {
  pointer p; /* |ord_noad| for new box in math mode */ 
  small_number a; /* global prefix */ 
  if (my_box_context < box_flag) {
	/* begin expansion of Append box |cur_box| to the current list, shifted by |box_context| */
	/* module 1221 */
	/* The global variable |adjust_tail| will be non-null if and only if the
	 * current box might include adjustments that should be appended to the
	 * current vertical list.
	 */
	if (cur_box != null) {
	  shift_amount (cur_box) = my_box_context;
	  if (abs (mode) == vmode) {
		if (pre_adjust_tail != null) {
		  if (pre_adjust_head != pre_adjust_tail)
			append_list (pre_adjust_head,pre_adjust_tail);
		  pre_adjust_tail = null;
		};
		append_to_vlist (cur_box);
		if (adjust_tail != null) {
		  if (adjust_head != adjust_tail)
			append_list (adjust_head,adjust_tail);
		  adjust_tail = null;
		};
		if (mode > 0)
		  build_page();
	  } else {
		if (abs (mode) == hmode) {
		  space_factor = 1000;
		} else {
		  p = new_noad();
		  math_type (nucleus (p)) = sub_box;
		  info (nucleus (p)) = cur_box;
		  cur_box = p;
		};
		link (tail) = cur_box;
		tail = cur_box;
	  };
	};
  /* end expansion of Append box |cur_box| to the current list, shifted by |box_context| */
  } else if (my_box_context < ship_out_flag) {
	/* begin expansion of Store \(c)|cur_box| in a box register */
	/* module 1222 */
	if (my_box_context < global_box_flag) {
	  cur_val = my_box_context - box_flag;
	  a = 0;
	} else {
	  cur_val = my_box_context - global_box_flag;
	  a = 4;
	};
	if (cur_val < 256) {
	  DEFINE (box_base + cur_val, box_ref, cur_box);
	} else
	  sa_def_box;
	/* end expansion of Store \(c)|cur_box| in a box register */
  } else if (cur_box != null) {
	if (my_box_context > ship_out_flag) {
	  /* begin expansion of Append a new leader node that uses |cur_box| */
	  /* module 1223 */
	  /* Get the next non-blank non-relax non-call token */
	  get_nblank_nrelax_ncall;
	  if (((cur_cmd == hskip) && (abs (mode) != vmode))
		  || ((cur_cmd == vskip) && (abs (mode) == vmode))
		  || ((cur_cmd == mskip) && (abs (mode) == mmode))) {
		append_glue();
		subtype (tail) = my_box_context - (leader_flag - a_leaders);
		leader_ptr (tail) = cur_box;
	  } else {
		print_err ("Leaders not followed by proper glue");
		help3 ("You should say `\\leaders <box or rule><hskip or vskip>'.",
			   "I found the <box or rule>, but there's no suitable",
			   "<hskip or vskip>, so I'm ignoring these leaders.");
		back_error();
		flush_node_list (cur_box);
	  };
	/* end expansion of Append a new leader node that uses |cur_box| */
	} else {
	  ship_out (cur_box);
	}
  }
}


/* module  1224 */

/* Now that we can see what eventually happens to boxes, we can consider
 * the first steps in their creation. The |begin_box| routine is called when
 * |box_context| is a context specification, |cur_chr| specifies the type of
 * box desired, and |cur_cmd=make_box|.
 */
void 
begin_box (int box_context) {
  pointer p, q; /* run through the current list */ 
  quarterword m; /* the length of a replacement list */ 
  halfword k; /* 0 or |vmode| or |hmode| */ 
  halfword n; /* a box number */ 
  switch (cur_chr) {
  case box_code:
	scan_register_num();
	fetch_box (cur_box);
	change_box (null); 	  /* the box becomes void, at the same level */ 
	break;
  case copy_code:
	scan_register_num();
	fetch_box (q);
	cur_box = copy_node_list (q);
	break;
  case last_box_code:
	/* begin expansion of If the current list ends with a box node, delete it from 
	   the list and make |cur_box| point to it; otherwise set |cur_box:=null| */
	/* module 1225 */
	/* Note that the condition |not is_char_node(tail)| implies that |head<>tail|,
	 * since |head| is a one-word node.
	 * A final \.{\\endM} node is temporarily removed.
	 */
	cur_box = null;
	if (abs (mode) == mmode) {
	  you_cant();
	  help1 ("Sorry; this \\lastbox will be void.");
	  error();
	} else if ((mode == vmode) && (head == tail)) {
	  you_cant();
	  help2 ("Sorry...I usually can't take things from the current page.",
			 "This \\lastbox will therefore be void.");
	  error();
	} else {
	  if (!is_char_node (tail)) {
		if ((type (tail) == math_node) && (subtype (tail) == end_M_code))
		  remove_end_M();
		if ((type (tail) == hlist_node) || (type (tail) == vlist_node)) {
		  /* begin expansion of Remove the last box, unless it's part of a discretionary */
		  /* module 1226 */
		  q = head;
		  do {
			p = q;
			if (!is_char_node (q))
			  if (type (q) == disc_node) {
				for (m = 1; m <= replace_count (q); m++)
				  p = link (p);
				if (p == tail)
				  goto DONE;
			  };
			q = link (p);
		  } while (q != tail);
		  cur_box = tail;
		  shift_amount (cur_box) = 0;
		  tail = p;
		  link (p) = null;
		DONE: 
		  do_nothing; 
		}
		/* end expansion of Remove the last box, unless it's part of a discretionary */
		if (LR_temp != null)
		  insert_end_M();
	  };
	};
	/* end expansion of If the current list ends with a box node, delete ....*/
	break;
  case vsplit_code:
	/* begin expansion of Split off part of a vertical box, make |cur_box| point to it */
	/* module 1227 */
	/* Here we deal with things like `\.{\\vsplit 13 to 100pt}'.
	 */
	scan_register_num();
	n = cur_val;
	if (!scan_keyword ("to")) {
	  print_err ("Missing `to' inserted");
	  help2 ("I'm working on `\\vsplit<box number> to <dimen>';",
			 "will look for the <dimen> next.");
	  error();
	};
	scan_normal_dimen;
	cur_box = vsplit (n, cur_val);
	/* end expansion of Split off part of a vertical box, make |cur_box| point to it */
	break;
  default:
	/* begin expansion of Initiate the construction of an hbox or vbox, then |return| */
	/* module 1228 */
	/* Here is where we enter restricted horizontal mode or internal vertical
	 * mode, in order to make a box.
	 */
	k = cur_chr - vtop_code;
	saved (0) = box_context;
	if (k == hmode) {
	  if ((box_context < box_flag) && (abs (mode) == vmode)) {
		scan_spec (adjusted_hbox_group, true);
	  } else {
		scan_spec (hbox_group, true);
	  } 
	} else {
	  if (k == vmode) {
		scan_spec (vbox_group, true);
	  } else {
		scan_spec (vtop_group, true);
		k = vmode;
	  };
	  normal_paragraph();
	};
	push_nest();
	mode = -k;
	if (k == vmode) {
	  prev_depth = ignore_depth;
	  if (every_vbox != null)
		begin_token_list (every_vbox, every_vbox_text);
	} else {
	  space_factor = 1000;
	  if (every_hbox != null)
		begin_token_list (every_hbox, every_hbox_text);
	};
	return;
	/* end expansion of Initiate the construction of an hbox or vbox, then |return| */
  };
  box_end (box_context); /* in simple cases, we use the box immediately */ 
}

/* module 1229 */
void 
scan_box (int box_context) {
  /* the next input should specify a box or perhaps a rule */
  /* Get the next non-blank non-relax non-call token */
  get_nblank_nrelax_ncall;
  if (cur_cmd == make_box) {
	begin_box (box_context);
  } else if ((box_context >= leader_flag) && ((cur_cmd == hrule) || (cur_cmd == vrule))) {
	cur_box = scan_rule_spec();
	box_end (box_context);
  } else {
	print_err ("A <box> was supposed to be here");
	help3  ("I was expecting to see \\hbox or \\vbox or \\copy or \\box or",
			"something like that. So you might find something missing in",
			"your output. But keep trying; you can fix this later.");
	back_error();
  };
}

/* module 1231 */
void 
package (small_number c) {
  scaled h; /* height of box */ 
  pointer p; /* first node in a box */ 
  scaled d; /* max depth */ 
  d = box_max_depth;
  unsave();
  save_ptr = save_ptr - 3;
  if (mode == -hmode) {
	cur_box = hpack (link (head), saved (2), saved (1));
  } else {
	cur_box = vpackage (link (head), saved (2), saved (1), d);
	if (c == vtop_code) {
	  /* begin expansion of Readjust the height and depth of |cur_box|, for \.{\\vtop} */
	  /* module 1232 */
	  /* The height of a `\.{\\vtop}' box is inherited from the first item on its list,
	   * if that item is an |hlist_node|, |vlist_node|, or |rule_node|; otherwise
	   * the \.{\\vtop} height is zero.
	   */
	  h = 0;
	  p = list_ptr (cur_box);
	  if (p != null)
		if (type (p) <= rule_node)
		  h = height (p);
	  depth (cur_box) = depth (cur_box) - h + height (cur_box);
	  height (cur_box) = h;
	};
	/* end expansion of Readjust the height and depth of |cur_box|, for \.{\\vtop} */
  };
  pop_nest();
  box_end (saved (0));
};

/* module 1255 */
void 
unpackage (void) {
  pointer p; /* the box */ 
  pointer r; /* to remove marging kern nodes */ 
  unsigned char c; /* should we copy? */ 
  c =0; /*TH -Wall */
  if (cur_chr > copy_code) { 
	/* begin expansion of Handle saved items and |goto done| */
	/* module 1808 */
	link (tail) = disc_ptr[cur_chr];
	disc_ptr[cur_chr] = null;
	goto DONE;
	/* end expansion of Handle saved items and |goto done| */
  };
  c = cur_chr;
  scan_register_num();
  fetch_box (p);
  if (p == null)
	return;
  if ((abs (mode) == mmode)
	  || ((abs (mode) == vmode) && (type (p) != vlist_node))
	  || ((abs (mode) == hmode) && (type (p) != hlist_node))) {
	print_err ("Incompatible list can't be unboxed");
	help3 ("Sorry, Pandora. (You sneaky devil.)",
		   "I refuse to unbox an \\hbox in vertical mode or vice versa.",
		   "And I can't open any boxes in math mode.");
	error();
	return;
  };
  if (c == copy_code) {
	link (tail) = copy_node_list (list_ptr (p));
  } else {
	link (tail) = list_ptr (p);
	change_box (null);
	free_node (p, box_node_size);
  };
 DONE:
  if (c == copy_code) {
	while (link (tail) != null)
	  tail = link (tail);
  } else {
	while (link (tail) != null) {
	  r = link (tail);
	  if (!is_char_node (r) && (type (r) == margin_kern_node)) {
		link (tail) = link (r);
		free_avail (margin_char (r));
		free_node (r, margin_kern_node_size);
	  };
	  tail = link (tail);
	};
  };
}

