#include "types.h"
#include "c-compat.h"


#include "globals.h"




/* module 1125 */

/* 
 * When \TeX\ appends new material to its main vlist in vertical mode, it uses
 * a method something like |vsplit| to decide where a page ends, except that
 * the calculations are done ``on line'' as new items come in.
 * The main complication in this process is that insertions must be put
 * into their boxes and removed from the vlist, in a more-or-less optimum manner.
 * 
 * We shall use the term ``current page'' for that part of the main vlist that
 * is being considered as a candidate for being broken off and sent to the
 * user's output routine. The current page starts at |link(page_head)|, and
 * it ends at |page_tail|. We have |page_head=page_tail| if this list is empty.
 * 
 * Utter chaos would reign if the user kept changing page specifications
 * while a page is being constructed, so the page builder keeps the pertinent
 * specifications frozen as soon as the page receives its first box or
 * insertion. The global variable |page_contents| is |empty| when the
 * current page contains only mark nodes and content-less whatsit nodes; it
 * is |inserts_only| if the page contains only insertion nodes in addition to
 * marks and whatsits. Glue nodes, kern nodes, and penalty nodes are
 * discarded until a box or rule node appears, at which time |page_contents|
 * changes to |box_there|. As soon as |page_contents| becomes non-|empty|,
 * the current |vsize| and |max_depth| are squirreled away into |page_goal|
 * and |page_max_depth|; the latter values will be used until the page has
 * been forwarded to the user's output routine. The \.{\\topskip} adjustment
 * is made when |page_contents| changes to |box_there|.
 * 
 * Although |page_goal| starts out equal to |vsize|, it is decreased by the
 * scaled natural height-plus-depth of the insertions considered so far, and by
 * the \.{\\skip} corrections for those insertions. Therefore it represents
 * the size into which the non-inserted material should fit, assuming that
 * all insertions in the current page have been made.
 * 
 * The global variables |best_page_break| and |least_page_cost| correspond
 * respectively to the local variables |best_place| and |least_cost| in the
 * |vert_break| routine that we have already studied; i.e., they record the
 * location and value of the best place currently known for breaking the
 * current page. The value of |page_goal| at the time of the best break is
 * stored in |best_size|.
 */

pointer page_tail; /* the final node on the current page */
unsigned char page_contents; /* what is on the current page so far? */
scaled page_max_depth; /* maximum box depth on page being built */
pointer best_page_break; /* break here to get the best page known so far */
int least_page_cost; /* the score for this currently best page */
scaled best_size; /* its |page_goal| */


/* module 1126 */
/* The page builder has another data structure to keep track of insertions.
 * This is a list of four-word nodes, starting and ending at |page_ins_head|.
 * That is, the first element of the list is node |r@t$_1$@>=link(page_ins_head)|;
 * node $r_j$ is followed by |r@t$_{j+1}$@>=link(r@t$_j$@>)|; and if there are
 * |n| items we have |r@t$_{n+1}$@>=page_ins_head|. The |subtype| field of
 * each node in this list refers to an insertion number; for example, `\.{\\insert
 * 250}' would correspond to a node whose |subtype| is |qi(250)|
 * (the same as the |subtype| field of the relevant |ins_node|). These |subtype|
 * fields are in increasing order, and |subtype(page_ins_head)=
 * qi(255)|, so |page_ins_head| serves as a convenient sentinel
 * at the end of the list. A record is present for each insertion number that
 * appears in the current page.
 * 
 * The |type| field in these nodes distinguishes two possibilities that
 * might occur as we look ahead before deciding on the optimum page break.
 * If |type(r)=inserting|, then |height(r)| contains the total of the
 * height-plus-depth dimensions of the box and all its inserts seen so far.
 * If |type(r)=split_up|, then no more insertions will be made into this box,
 * because at least one previous insertion was too big to fit on the current
 * page; |broken_ptr(r)| points to the node where that insertion will be
 * split, if \TeX\ decides to split it, |broken_ins(r)| points to the
 * insertion node that was tentatively split, and |height(r)| includes also the
 * natural height plus depth of the part that would be split off.
 * 
 * In both cases, |last_ins_ptr(r)| points to the last |ins_node|
 * encountered for box |qo(subtype(r))| that would be at least partially
 * inserted on the next page; and |best_ins_ptr(r)| points to the last
 * such |ins_node| that should actually be inserted, to get the page with
 * minimum badness among all page breaks considered so far. We have
 * |best_ins_ptr(r)=null| if and only if no insertion for this box should
 * be made to produce this optimum page.
 * 
 * The data structure definitions here use the fact that the | height| field
 * appears in the fourth word of a box node.
 */


/* module 1127 */

/* An array |page_so_far| records the heights and depths of everything
 * on the current page. This array contains six |scaled| numbers, like the
 * similar arrays already considered in |line_break| and |vert_break|; and it
 * also contains |page_goal| and |page_depth|, since these values are
 * all accessible to the user via |set_page_dimen| commands. The
 * value of |page_so_far[1]| is also called |page_total|. The stretch
 * and shrink components of the \.{\\skip} corrections for each insertion are
 * included in |page_so_far|, but the natural space components of these
 * corrections are not, since they have been subtracted from |page_goal|.
 * 
 * The variable |page_depth| records the depth of the current page; it has been
 * adjusted so that it is at most |page_max_depth|. The variable
 * |last_glue| points to the glue specification of the most recent node
 * contributed from the contribution list, if this was a glue node; otherwise
 * |last_glue=max_halfword|. (If the contribution list is nonempty,
 * however, the value of |last_glue| is not necessarily accurate.)
 * The variables |last_penalty|, |last_kern|, and |last_node_type|
 * are similar. And
 * finally, |insert_penalties| holds the sum of the penalties associated with
 * all split and floating insertions.
 */

scaled page_so_far[8]; /* height and glue of the current page */
pointer last_glue; /* used to implement \.{\\lastskip} */
int last_penalty; /* used to implement \.{\\lastpenalty} */
scaled last_kern; /* used to implement \.{\\lastkern} */
int last_node_type; /* used to implement \.{\\lastnodetype} */
int insert_penalties; /* sum of the penalties for held-over insertions */

/* module 1130 */
#define print_plus( arg,b )            \
  if (page_so_far [arg] != 0) {        \
    zprint_string (" plus ");          \
    print_scaled ( page_so_far [arg]); \
    zprint_string ( b );               \
  }

void print_totals (void) {
  print_scaled (page_total);
  print_plus (2, "");
  print_plus (3, "fil");
  print_plus (4, "fill");
  print_plus (5, "filll");
  if (page_shrink != 0) {
	zprint_string(" minus ");
	print_scaled (page_shrink);
  };
};



/* module 1132 */

/* Here is a procedure that is called when the |page_contents| is changing
 * from |empty| to |inserts_only| or |box_there|.
 */
#define set_page_so_far_zero( arg )  page_so_far [ arg ]  = 0

void 
freeze_page_specs (small_number s) {
  page_contents = s;
  page_goal = vsize;
  page_max_depth = max_depth;
  page_depth = 0;
  do_all_six (set_page_so_far_zero);
  least_page_cost = awful_bad;
  if (tracing_pages > 0) {
	begin_diagnostic();
	print_nl_string("%% goal height=");
	print_scaled (page_goal);
	zprint_string(", max depth=");
	print_scaled (page_max_depth);
	end_diagnostic (false);
  };
};



/* module 1133 */

/* Pages are built by appending nodes to the current list in \TeX's
 * vertical mode, which is at the outermost level of the semantic nest. This
 * vlist is split into two parts; the ``current page'' that we have been
 * talking so much about already, and the ``contribution list'' that receives
 * new nodes as they are created. The current page contains everything that
 * the page builder has accounted for in its data structures, as described
 * above, while the contribution list contains other things that have been
 * generated by other parts of \TeX\ but have not yet been
 * seen by the page builder.
 * The contribution list starts at |link(contrib_head)|, and it ends at the
 * current node in \TeX's vertical mode.
 * 
 * When \TeX\ has appended new material in vertical mode, it calls the procedure
 * |build_page|, which tries to catch up by moving nodes from the contribution
 * list to the current page. This procedure will succeed in its goal of
 * emptying the contribution list, unless a page break is discovered, i.e.,
 * unless the current page has grown to the point where the optimum next
 * page break has been determined. In the latter case, the nodes after the
 * optimum break will go back onto the contribution list, and control will
 * effectively pass to the user's output routine.
 * 
 * We make |type(page_head)=glue_node|, so that an initial glue node on
 * the current page will not be considered a valid breakpoint.
 */

void
buildpage_initialize_init (void) {
      subtype (page_ins_head) = qi (255);
      type (page_ins_head) = split_up;
      link (page_ins_head) = page_ins_head;
      type (page_head) = glue_node;
      subtype (page_head) = normal;
}

/* module 1134 */

/* The global variable |output_active| is true during the time the
 * user's output routine is driving \TeX.
 */

boolean output_active; /* are we in the midst of an output routine? */

/* module 1135 */

void
buildpage_initialize (void) {
  output_active = false;
  insert_penalties = 0;
}

/* module 1137 */

/* At certain times box 255 is supposed to be void (i.e., |null|),
 * or an insertion box is supposed to be ready to accept a vertical list.
 * If not, an error message is printed, and the following subroutine
 * flushes the unwanted contents, reporting them to the user.
 */
void
box_error (eight_bits n) {
  error();
  begin_diagnostic();
  print_nl_string("The following box has been deleted:");
  show_box (box (n));
  end_diagnostic (true);
  flush_node_list (box (n));
  box (n) = null;
};


/* module 1138 */

/* The following procedure guarantees that a given box register
 * does not contain an \.{\\hbox}.
 */
void
ensure_vbox (eight_bits n) {
  pointer p; /* the box register contents */ 
  p = box (n);
  if (p != null)
	if (type (p) == hlist_node) {
	  print_err ("Insertions can only be added to a vbox");
	  help3  ("Tut tut: You're trying to \\insert into a",
			  "\\box register that now contains an \\hbox.",
			  "Proceed, and I'll discard its present contents.");
	  box_error (n);
	};
};

/* module 1136 */
/* The page builder is ready to start a fresh page if we initialize
 * the following state variables. (However, the page insertion list is initialized
 * elsewhere.)
 */
void
start_new_page  (void) {
  page_contents = empty;
  page_tail = page_head;
  last_glue = max_halfword;
  last_penalty = 0;
  last_kern = 0;
  page_depth = 0;
  page_max_depth = 0;
}

/* module 1157 */

/* When the page builder has looked at as much material as could appear before
 * the next page break, it makes its decision. The break that gave minimum
 * badness will be used to put a completed ``page'' into box 255, with insertions
 * appended to their other boxes.
 * 
 * We also set the values of |top_mark|, |first_mark|, and |bot_mark|. The
 * program uses the fact that |bot_mark<>null| implies |first_mark<>null|;
 * it also knows that |bot_mark=null| implies |top_mark=first_mark=null|.
 * 
 * The |fire_up| subroutine prepares to output the current page at the best
 * place; then it fires up the user's output routine, if there is one,
 * or it simply ships out the page. There is one parameter, |c|, which represents
 * the node that was being contributed to the page when the decision to
 * force an output was made.
 */
void
fire_up (pointer c) {
  pointer p, q, r, s; /* nodes being examined and/or changed */ 
  pointer prev_p; /* predecessor of |p| */ 
  unsigned char n; /* insertion box number */ 
  boolean wait; /* should the present insertion be held over? */ 
  int save_vbadness; /* saved value of |vbadness| */ 
  scaled save_vfuzz; /* saved value of |vfuzz| */ 
  pointer save_split_top_skip; /* saved value of |split_top_skip| */
  /* begin expansion of Set the value of |output_penalty| */
  /* module 1158 */
  if (type (best_page_break) == penalty_node) {
	geq_word_define (int_base + output_penalty_code, penalty (best_page_break));
	penalty (best_page_break) = inf_penalty;
  } else {
	geq_word_define (int_base + output_penalty_code, inf_penalty);
  }
  /* end expansion of Set the value of |output_penalty| */
  if (sa_mark != null)
	if (do_marks (fire_up_init, 0, sa_mark))
	  sa_mark = null;
  if (bot_mark != null) {
	if (top_mark != null)
	  delete_token_ref (top_mark);
	top_mark = bot_mark;
	add_token_ref (top_mark);
	delete_token_ref (first_mark);
	first_mark = null;
  };
  /* begin expansion of Put the \(o)optimal current page into box 255, update |first_mark|  and |bot_mark|, 
	 append insertions to their boxes, and put the remaining nodes back on the contribution list */
  /* module 1159 */
  /* As the page is finally being prepared for output,
   * pointer |p| runs through the vlist, with |prev_p| trailing behind;
   * pointer |q| is the tail of a list of insertions that
   * are being held over for a subsequent page.
   */
  if (c == best_page_break)
	      best_page_break = null; /* |c| not yet linked in */
  /* begin expansion of Ensure that box 255 is empty before output */
  /* module 1160 */
  if (box (255) != null) {
	print_err ("");
	print_esc_string ("box");
	zprint_string("255 is not void");
	help2 ("You shouldn't use \\box255 except in \\output routines.",
		   "Proceed, and I'll discard its present contents.");
	box_error (255);
  };
  /* end expansion of Ensure that box 255 is empty before output */
  insert_penalties = 0;  /* this will count the number of insertions held over */
  save_split_top_skip = split_top_skip;
  if (holding_inserts <= 0) {
	/* begin expansion of Prepare all the boxes involved in insertions to act as queues */
	/* module 1163 */
	/* If many insertions are supposed to go into the same box, we want to know
	 * the position of the last node in that box, so that we don't need to waste time
	 * when linking further information into it. The |last_ins_ptr| fields of the
	 * page insertion nodes are therefore used for this purpose during the
	 * packaging phase.
	 */
	r = link (page_ins_head);
	while (r != page_ins_head) {
	  if (best_ins_ptr (r) != null) {
		n = qo (subtype (r));
		ensure_vbox (n);
		if (box (n) == null)
		  box (n) = new_null_box();
		p = box (n) + list_offset;
		while (link (p) != null)
		  p = link (p);
		last_ins_ptr (r) = p;
	  };
	  r = link (r);
	};
  };
  /* end expansion of Prepare all the boxes involved in insertions to act as queues */
  q = hold_head;
  link (q) = null;
  prev_p = page_head;
  p = link (prev_p);
  while (p != best_page_break) {
	if (type (p) == ins_node) {
	  if (holding_inserts <= 0) {
		/* begin expansion of Either insert the material specified by node |p| into the appropriate box, 
		   or hold it for the next page; also delete node |p| from the current page */
		/* module 1165 */
		/* We will set |best_ins_ptr:=null| and package the box corresponding to
		 * insertion node~|r|, just after making the final insertion into that box.
		 * If this final insertion is `|split_up|', the remainder after splitting
		 * and pruning (if any) will be carried over to the next page.
		 */
		r = link (page_ins_head);
		while (subtype (r) != subtype (p))
		  r = link (r);
		if (best_ins_ptr (r) == null) {
		  wait = true;
		} else {
		  wait = false;
		  s = last_ins_ptr (r);
		  link (s) = ins_ptr (p);
		  if (best_ins_ptr (r) == p) {
			/* begin expansion of Wrap up the box specified by node |r|, splitting node |p| if called for; 
			   set |wait:=true| if node |p| holds a remainder after splitting */
			/* module 1166 */
			if (type (r) == split_up)
			  if ((broken_ins (r) == p) && (broken_ptr (r) != null)) {
				while (link (s) != broken_ptr (r))
				  s = link (s);
				link (s) = null;
				split_top_skip = split_top_ptr (p);
				ins_ptr (p) = prune_page_top (broken_ptr (r),false);
				if (ins_ptr (p) != null) {
				  temp_ptr = VPACK (ins_ptr (p), 0, additional);
				  height (p) =  height (temp_ptr) + depth (temp_ptr);
				  free_node (temp_ptr, box_node_size);
				  wait = true;
				};
			  };
			best_ins_ptr (r) = null;
			n = qo (subtype (r));
			temp_ptr = list_ptr (box (n));
			free_node (box (n), box_node_size);
			box (n) = VPACK (temp_ptr, 0, additional);
			/* end expansion of Wrap up the box specified by node |r|, splitting node |p| if... */
		  } else {
			while (link (s) != null)
			  s = link (s);
			last_ins_ptr (r) = s;
		  };
		};
		/* begin expansion of Either append the insertion node |p| after node |q|, and remove it 
		   from the current page, or delete |node(p)| */
		/* module 1167 */
		link (prev_p) = link (p);
		link (p) = null;
		if (wait) {
		  link (q) = p;
		  q = p;
		  incr (insert_penalties);
		} else {
		  delete_glue_ref (split_top_ptr (p));
		  free_node (p, ins_node_size);
		};
		p = prev_p;
		/* end expansion of Either append the insertion node |p| after node |q|, and remo... */
	  };
	  /* end expansion of Either insert the material specified by node |p| into the appropriate ..*/
	} else if (type (p) == mark_node) {
	  if (mark_class (p) != 0) {
		/* begin expansion of Update the current marks for |fire_up| */
		/* module 1775 */
		find_sa_element (mark_val, mark_class (p), true);
		if (sa_first_mark (cur_ptr) == null) {
		  sa_first_mark (cur_ptr) = mark_ptr (p);
		  add_token_ref (mark_ptr (p));
		};
		if (sa_bot_mark (cur_ptr) != null)
		  delete_token_ref (sa_bot_mark (cur_ptr));
		sa_bot_mark (cur_ptr) = mark_ptr (p);
		add_token_ref (mark_ptr (p));
		/* end expansion of Update the current marks for |fire_up| */
	  } else {
		/* begin expansion of Update the values of |first_mark| and |bot_mark| */
		/* module 1161 */
		if (first_mark == null){
		  first_mark = mark_ptr (p);
		  add_token_ref (first_mark);
		};
		if (bot_mark != null)
		  delete_token_ref (bot_mark);
		bot_mark = mark_ptr (p);
		add_token_ref (bot_mark);
		/* end expansion of Update the values of |first_mark| and |bot_mark| */
	  }
	};
	prev_p = p;
	p = link (prev_p);
  };
  split_top_skip = save_split_top_skip;
  /* begin expansion of Break the current page at node |p|, put it in box~255, and put 
	 the remaining nodes on the contribution list */
  /* module 1162 */
  /* When the following code is executed, the current page runs from node
   * |link(page_head)| to node |prev_p|, and the nodes from |p| to |page_tail|
   * are to be placed back at the front of the contribution list. Furthermore
   * the heldover insertions appear in a list from |link(hold_head)| to |q|; we
   * will put them into the current page list for safekeeping while the user's
   * output routine is active. We might have |q=hold_head|; and |p=null| if
   * and only if |prev_p=page_tail|. Error messages are suppressed within
   * |vpackage|, since the box might appear to be overfull or underfull simply
   * because the stretch and shrink from the \.{\\skip} registers for inserts
   * are not actually present in the box.
   */
  if (p != null) {
	if (link (contrib_head) == null) {
	  if (nest_ptr == 0) {
		tail = page_tail;
	  } else {
		contrib_tail = page_tail; 
	  }
	}
	link (page_tail) = link (contrib_head);
	link (contrib_head) = p;
	link (prev_p) = null;
  };
  save_vbadness = vbadness;
  vbadness = inf_bad;
  save_vfuzz = vfuzz;
  vfuzz = max_dimen; /* inhibit error messages */ 
  box (255) = vpackage (link (page_head), best_size, exactly, page_max_depth);
  vbadness = save_vbadness;
  vfuzz = save_vfuzz;
  if (last_glue != max_halfword)
	delete_glue_ref (last_glue);
  /* begin expansion of Start a new current page */
  start_new_page();
  link (page_head) = null;
  last_node_type = -1;
  /* end expansion of Start a new current page */
  /* this sets |last_glue:=max_halfword| */ 
  if (q != hold_head) {
	link (page_head) = link (hold_head);
	page_tail = q;
  };
  /* end expansion of Break the current page at node |p|, put it in box~255, and put...*/
  /* begin expansion of Delete \(t)the page-insertion nodes */
  /* module 1164 */
  r = link (page_ins_head);
  while (r != page_ins_head) {
	q = link (r);
	free_node (r, page_ins_node_size);
	r = q;
  };
  link (page_ins_head) = page_ins_head;
  /* end expansion of Delete \(t)the page-insertion nodes */
  /* end expansion of Put the \(o)optimal current page into box 255, .... */
  if (sa_mark != null)
	if (do_marks (fire_up_done, 0, sa_mark))
	  sa_mark = null;
  if ((top_mark != null) && (first_mark == null)) {
	first_mark = top_mark;
	add_token_ref (top_mark);
  };
  if (output_routine != null) {
	if (dead_cycles >= max_dead_cycles) {
	  /* begin expansion of Explain that too many dead cycles have occurred in a row */
	  /* module 1169 */
	  print_err ("Output loop---");
	  print_int (dead_cycles);
	  zprint_string(" consecutive dead cycles");
	  help3 ("I've concluded that your \\output is awry; it never does a",
			 "\\shipout, so I'm shipping \\box255 out myself. Next time",
			 "increase \\maxdeadcycles if you want me to be more patient!");
	  error();
	  /* end expansion of Explain that too many dead cycles have occurred in a row */
	} else {
	  /* begin expansion of Fire up the user's output routine and |return| */
	  /* module 1170 */
	  output_active = true;
	  incr (dead_cycles);
	  push_nest();
	  mode = -vmode;
	  prev_depth = ignore_depth;
	  mode_line = -line;
	  begin_token_list (output_routine, output_text);
	  new_save_level (output_group);
	  normal_paragraph();
	  scan_left_brace();
	  return;
	/* end expansion of Fire up the user's output routine and |return| */
	}
  };
  /* begin expansion of Perform the default output routine */
  /* module 1168 */
  /* The list of heldover insertions, running from |link(page_head)| to
   * |page_tail|, must be moved to the contribution list when the user has
   * specified no output routine.
   */
  if (link (page_head) != null) {
	if (link (contrib_head) == null) {
	  if (nest_ptr == 0) {
		tail = page_tail;
	  } else {
		contrib_tail = page_tail;
	  } 
	} else {
	  link (page_tail) = link (contrib_head);
	}
	link (contrib_head) = link (page_head);
	link (page_head) = null;
	page_tail = page_head;
  };
  flush_node_list (page_disc);
  page_disc = null;
  ship_out (box (255));
  box (255) = null;
  /* end expansion of Perform the default output routine */
};

/* module 1139 */

/* \TeX\ is not always in vertical mode at the time |build_page|
 * is called; the current mode reflects what \TeX\ should return to, after
 * the contribution list has been emptied. A call on |build_page| should
 * be immediately followed by `|goto BIG_SWITCH|', which is \TeX's central
 * control point.
 */

void build_page (void) { /* append contributions to the current page */
  pointer p; /* the node being appended */ 
  pointer q, r; /* nodes being examined */ 
  int b, c; /* badness and cost of current page */ 
  int pi; /* penalty to be added to the badness */ 
  unsigned char n; /* insertion box number */ 
  scaled delta, h, w; /* sizes used for insertion calculations */
  pi = 0; /*TH -Wall*/    
  if ((link (contrib_head) == null) || output_active) {
	return;
  }
  do {
  CONTINUE:
	p = link (contrib_head);
	/* begin expansion of Update the values of |last_glue|, |last_penalty|, and |last_kern| */
	/* module 1141 */
	if (last_glue != max_halfword)
	  delete_glue_ref (last_glue);
	last_penalty = 0;
	last_kern = 0;
	last_node_type = type (p) + 1;
	if (type (p) == glue_node) {
	  last_glue = glue_ptr (p);
	  add_glue_ref (last_glue);
	} else {
	  last_glue = max_halfword;
	  if (type (p) == penalty_node) {
		last_penalty = penalty (p);
	  } else if (type (p) == kern_node) {
		last_kern = width (p);
	  }
	};
	/* end expansion of Update the values of |last_glue|, |last_penalty|, and |last_kern| */
	/* begin expansion of Move node |p| to the current page; if it is time for a page break, 
	   put the nodes following the break back onto the contribution list, and |return| to 
	   the user's output routine if there is one */
	/* module 1142 */
	/* The code here is an example of a many-way switch into routines that
	 * merge together in different places. Some people call this unstructured
	 * programming, but the author doesn't see much wrong with it, as long as
	 * 
	 * the various labels have a well-understood meaning.
	 */
	/* begin expansion of If the current page is empty and node |p| is to be deleted, |goto done1|;
	   otherwise use node |p| to update the state of the current page; if this node is an insertion,
	   |goto contribute|; otherwise if this node is not a legal breakpoint, |goto contribute| or 
	   |update_heights|; otherwise set |pi| to the penalty associated with this breakpoint */
	/* module 1145 */
	/* The title of this section is already so long, it seems best to avoid
	 * making it more accurate but still longer, by mentioning the fact that a
	 * kern node at the end of the contribution list will not be contributed until
	 * we know its successor.
	 */
	switch (type (p)) {
	case hlist_node:
	case vlist_node:
	case rule_node:
	  if (page_contents < box_there) {
		/* begin expansion of Initialize the current page, insert the \.{\\topskip} glue ahead of 
		   |p|, and |goto continue| */
		/* module 1146 */
		if (page_contents == empty) {
		  freeze_page_specs (box_there);
		} else {
		  page_contents = box_there;
		}
		q = new_skip_param (top_skip_code); /* now |temp_ptr=glue_ptr(q)| */
		if (width (temp_ptr) > height (p)) {
		  width (temp_ptr) = width (temp_ptr) - height (p);
		} else {
		  width (temp_ptr) = 0;
		}
		link (q) = p;
		link (contrib_head) = q;
		goto CONTINUE;
		/* end expansion of Initialize the current page, insert the \.{\\topskip} g ...*/
	  } else {
		/* begin expansion of Prepare to move a box or rule node to the current page, then |goto contribute| */
		/* module 1147 */
		page_total = page_total + page_depth + height (p);
		page_depth = depth (p);
		goto CONTRIBUTE;
		/* end expansion of Prepare to move a box or rule node to the current page, then |goto contribute| */
	  };
	case whatsit_node:
	  /* begin expansion of Prepare to move whatsit |p| to the current page, then |goto contribute| */
	  /* module 1560 */
	  if ((subtype (p) == pdf_refxform_node) || (subtype (p) == pdf_refximage_node)) {
		page_total = page_total + page_depth + pdf_height (p);
		page_depth = pdf_depth (p);
	  };
	  goto CONTRIBUTE;
	  /* end expansion of Prepare to move whatsit |p| to the current page, then |goto contribute| */
	case glue_node:
	  if (page_contents < box_there) {
		goto DONE1;
	  } else if (precedes_break (page_tail)) {
		pi = 0;
	  } else {
		do_something; 
		goto UPDATE_HEIGHTS;
	  } 
	  do_something; 
	  break;
	case kern_node:
	  if (page_contents < box_there) {
		goto DONE1;
	  } else if (link (p) == null) {
		return;
	  } else  if (type (link (p)) == glue_node) {
		pi = 0;
	  } else {
		do_something; 
		goto UPDATE_HEIGHTS;
	  }
	  do_something; 
	  break;
	case penalty_node:
	  if (page_contents < box_there) {
		goto DONE1;
	  } else {
		pi = penalty (p);
	  }
	  break;
	case mark_node:
	  do_something; 
	  goto CONTRIBUTE;
	case ins_node:
	  /* begin expansion of Append an insertion to the current page and |goto contribute| */
	  /* module 1153 */
	  {
		if (page_contents == empty)
		  freeze_page_specs (inserts_only);
		n = subtype (p);
		r = page_ins_head;
		while (n >= subtype (link (r)))
		  r = link (r);
		n = qo (n);
		if (subtype (r) != qi (n)) {
		  /* begin expansion of Create a page insertion node with |subtype(r)=qi(n)|, 
			 and include the glue correction for box |n| in the current page state */
		  /* module 1154 */
		  /* We take note of the value of \.{\\skip} |n| and the height plus depth
		   * of \.{\\box}~|n| only when the first \.{\\insert}~|n| node is
		   * encountered for a new page. A user who changes the contents of \.{\\box}~|n|
		   * after that first \.{\\insert}~|n| had better be either extremely careful
		   * or extremely lucky, or both.
		   */
		  q = get_node (page_ins_node_size);
		  link (q) = link (r);
		  link (r) = q;
		  r = q;
		  subtype (r) = qi (n);
		  type (r) = inserting;
		  ensure_vbox (n);
		  if (box (n) == null) {
			height (r) = 0;
		  } else {
			height (r) = height (box (n)) + depth (box (n));
		  }
		  best_ins_ptr (r) = null;
		  q = skip (n);
		  if (count (n) == 1000) {
			h = height (r);
		  } else {
			h = x_over_n (height (r), 1000) * count (n);
		  }
		  page_goal = page_goal - h - width (q);
		  page_so_far[2 + stretch_order (q)] = page_so_far[2 + stretch_order (q)] + stretch (q);
		  page_shrink = page_shrink + shrink (q);
		  if ((shrink_order (q) != normal) && (shrink (q) != 0)) {
			print_err ("Infinite glue shrinkage inserted from ");
			print_esc_string ("skip");
			print_int (n);
			help3 ("The correction glue for page breaking with insertions",
				   "must have finite shrinkability. But you may proceed,",
				   "since the offensive shrinkability has been made finite.");
			error();
		  };
		};
		/* end expansion of Create a page insertion node with |subtype(r)=qi(n)|, a...*/
		if (type (r) == split_up) {
		  insert_penalties = insert_penalties + float_cost (p);
		} else {
		  last_ins_ptr (r) = p;
		  delta = page_goal - page_total - page_depth + page_shrink;
		  /* this much room is left if we shrink the maximum */
		  if (count (n) == 1000) {
			h = height (p);
		  } else {
			h = x_over_n (height (p), 1000) * count (n);
		  }
		  /* this much room is needed */
		  if (((h <= 0) || (h<= delta))&& (height (p) + height (r) <= dimen (n))) {
			page_goal = page_goal - h;
			height (r) = height (r) + height (p);
		  } else {
			/* begin expansion of Find the best way to split the insertion, and change |type(r)| to |split_up| */
			/* module 1155 */
			/* Here is the code that will split a long footnote between pages, in an
			 * emergency. The current situation deserves to be recapitulated: Node |p|
			 * is an insertion into box |n|; the insertion will not fit, in its entirety,
			 * either because it would make the total contents of box |n| greater than
			 * \.{\\dimen} |n|, or because it would make the incremental amount of growth
			 * |h| greater than the available space |delta|, or both. (This amount |h| has
			 * been weighted by the insertion scaling factor, i.e., by \.{\\count} |n|
			 * over 1000.) Now we will choose the best way to break the vlist of the
			 * insertion, using the same criteria as in the \.{\\vsplit} operation.
			 */
			if (count (n) <= 0) {
			  w = max_dimen;
			} else {
			  w = page_goal - page_total - page_depth;
			  if (count (n) != 1000)
				w = x_over_n (w, count (n)) * 1000;
			};
			if (w > dimen (n) - height (r))
			  w = dimen (n) - height (r);
			q = vert_break (ins_ptr (p), w, depth (p));
			height (r) = height (r) + best_height_plus_depth;
			if (tracing_pages > 0) {
			  /* begin expansion of Display the insertion split cost */
			  /* module 1156 */
			  begin_diagnostic();
			  print_nl_string("% split");
			  print_int (n);
			  zprint_string(" to ");
			  print_scaled (w);
			  print_char (',');
			  print_scaled (best_height_plus_depth);
			  zprint_string(" p=");
			  if (q == null) {
				print_int (eject_penalty);
			  } else if (type (q) == penalty_node) {
				print_int (penalty (q));
			  } else {
				print_char ('0');
			  }
			  end_diagnostic (false);
			};
			/* end expansion of Display the insertion split cost */
			if (count (n) != 1000)
			  best_height_plus_depth = x_over_n (best_height_plus_depth, 1000) * count (n);
			page_goal = page_goal - best_height_plus_depth;
			type (r) = split_up;
			broken_ptr (r) = q;
			broken_ins (r) = p;
			if (q == null) {
			  insert_penalties = insert_penalties + eject_penalty;
			} else if (type (q) == penalty_node)
			  insert_penalties = insert_penalties + penalty (q);
		  };
		  /* end expansion of Find the best way to split the insertion, and change |type(r)| to |split_up| */
		};
		goto CONTRIBUTE;
	  };
	  /* end expansion of Append an insertion to the current page and |goto contribute| */
	  break;
	default:
	  confusion ("page");
	}
	/* end expansion of If the current page is empty and node |p| is to be deleted,  ...*/
	/* begin expansion of Check if node |p| is a new champion breakpoint; then \(if)if it is
	   time for a page break, prepare for output, and either fire up the user's output routine 
	   and |return| or ship out the page and |goto done| */
	/* module 1150 */
	if (pi < inf_penalty) {
	  /* begin expansion of Compute the badness, |b|, of the current page, 
		 using |awful_bad| if the box is too full */
	  /* module 1152 */
	  if (page_total < page_goal) {
		if ((page_so_far[3] != 0) || (page_so_far[4] != 0) || (page_so_far[5] != 0)) {
		  b = 0;
		} else {
		  b = badness (page_goal - page_total, page_so_far[2]);
		} 
	  } else if (page_total - page_goal > page_shrink) {
		b = awful_bad;
	  } else {
		b = badness (page_total - page_goal, page_shrink);
	  }
	  /* end expansion of Compute the badness, |b|, of the current page,..*/
	  if (b < awful_bad) {
		if (pi <= eject_penalty) {
		  c = pi;
		} else if (b < inf_bad) {
		  c = b + pi + insert_penalties;
		} else {
		  c = deplorable;
		} 
	  } else {
		c = b;
	  }
	  if (insert_penalties >= 10000)
		c = awful_bad;
	  if (tracing_pages > 0) {
		/* begin expansion of Display the page break cost */
		/* module 1151 */
		begin_diagnostic();
		print_nl_string ("%");
		zprint_string(" t=");
		print_totals();
		zprint_string(" g=");
		print_scaled (page_goal);
		zprint_string(" b=");
		if (b == awful_bad) {
		  print_char ('*');
		} else {
		  print_int (b);
		}
		zprint_string(" p=");
		print_int (pi);
		zprint_string(" c=");
		if (c == awful_bad) {
		  print_char ('*');
		} else {
		  print_int (c);
		}
		if (c <= least_page_cost)
		  print_char ('#');
		end_diagnostic (false);
	  };
	  /* end expansion of Display the page break cost */
	  if (c <= least_page_cost) {
		best_page_break = p;
		best_size = page_goal;
		least_page_cost = c;
		r = link (page_ins_head);
		while (r != page_ins_head) {
		  best_ins_ptr (r) = last_ins_ptr (r);
		  r = link (r);
		};
	  };
	  if ((c == awful_bad) || (pi <= eject_penalty)) {
		fire_up (p); /* output the current page at the best place */
		if (output_active) {
		  return; /* user's output routine will act */ 
        } else { do_something; }
		goto DONE; /* the page has been shipped out by default output routine */
	  };
	};
	/* end expansion of Check if node |p| is a new champion breakpoint; ..*/
	if ((type (p) < glue_node) || (type (p) > kern_node))
	  { do_something; goto CONTRIBUTE; }
  UPDATE_HEIGHTS:
	/* begin expansion of Update the current page measurements with respect to 
	   the glue or kern specified by node~|p| */
	/* module 1149 */
	if (type (p) == kern_node) {
	  q = p;
	} else {
	  q = glue_ptr (p);
	  page_so_far[2 + stretch_order (q)] = page_so_far[2 + stretch_order (q)] + stretch (q);
	  page_shrink = page_shrink + shrink (q);
	  if ((shrink_order (q) != normal) && (shrink (q) != 0)) {
		print_err   ("Infinite glue shrinkage found on current page");
		help4 ("The page about to be output contains some infinitely",
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
	page_total = page_total + page_depth + width (q);
	page_depth = 0;
	/* end expansion of Update the current page measurements with respect to  ..*/
  CONTRIBUTE:
	/* begin expansion of Make sure that |page_max_depth| is not exceeded */
	/* module 1148 */
	if (page_depth > page_max_depth) {
	  page_total = page_total + page_depth - page_max_depth;
	  page_depth = page_max_depth;
	};
	/* end expansion of Make sure that |page_max_depth| is not exceeded */
	/* begin expansion of Link node |p| into the current page and |goto done| */
	/* module 1143 */
	link (page_tail) = p;
	page_tail = p;
	link (contrib_head) = link (p);
	link (p) = null;
	goto DONE;
	/* end expansion of Link node |p| into the current page and |goto done| */
  DONE1:
	/* begin expansion of Recycle node |p| */
	/* module 1144 */
	link (contrib_head) = link (p);
	link (p) = null;
	if (saving_vdiscards > 0) {
	  if (page_disc == null) {
		page_disc = p;
	  } else {
		link (tail_page_disc) = p;
	  }
	  tail_page_disc = p;
	} else {
	  flush_node_list (p);
	}
	/* end expansion of Recycle node |p| */
  DONE:
	/* end expansion of Move node |p| to the current page; if it is time for a page break..*/
	do_nothing;
  } while (link (contrib_head) != null);
  /* begin expansion of Make the contribution list empty by setting its tail to |contrib_head| */
  /* module 1140 */
  if (nest_ptr == 0) {
	tail = contrib_head;	/* vertical mode */
  } else {
	contrib_tail = contrib_head;	/* other modes */
  };
  /* end expansion of Make the contribution list empty by setting its tail to |contrib_head| */
};



