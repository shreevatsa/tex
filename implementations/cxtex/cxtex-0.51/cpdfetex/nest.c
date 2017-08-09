
#include "types.h"
#include "c-compat.h"


#include "globals.h"



/* module 213 */
list_state_record *nest;
unsigned int nest_ptr; /* first unused location of |nest| */ 
unsigned int max_nest_stack; /* maximum of |nest_ptr| when pushing */ 
list_state_record cur_list; /* the ``top'' semantic state */ 

/* module 211 */
/* 
 * \TeX\ is typically in the midst of building many lists at once. For example,
 * when a math formula is being processed, \TeX\ is in math mode and
 * working on an mlist; this formula has temporarily interrupted \TeX\ from
 * being in horizontal mode and building the hlist of a paragraph; and this
 * paragraph has temporarily interrupted \TeX\ from being in vertical mode
 * and building the vlist for the next page of a document. Similarly, when a
 * \.{\\vbox} occurs inside of an \.{\\hbox}, \TeX\ is temporarily
 * interrupted from working in restricted horizontal mode, and it enters
 * internal vertical mode. The ``semantic nest'' is a stack that
 * keeps track of what lists and modes are currently suspended.
 * 
 * At each level of processing we are in one of six modes:
 * 
 * \yskip
 * \hang|vmode| stands for vertical mode (the page builder);
 * \hang|hmode| stands for horizontal mode (the paragraph builder);
 * \hang|mmode| stands for displayed formula mode;
 * \hang|-vmode| stands for internal vertical mode (e.g., in a \.{\\vbox});
 * \hang|-hmode| stands for restricted horizontal mode (e.g., in an \.{\\hbox});
 * \hang|-mmode| stands for math formula mode (not displayed).
 * 
 * \yskip\noindent The mode is temporarily set to zero while processing \.{\\write}
 * texts in the |ship_out| routine.
 * 
 * Numeric values are assigned to |vmode|, |hmode|, and |mmode| so that
 * \TeX's ``big semantic switch'' can select the appropriate thing to
 * do by computing the value |abs(mode)+cur_cmd|, where |mode| is the current
 * mode and |cur_cmd| is the current command code.
 */

void
print_mode (int m) { /* prints the mode represented by |m| */
  if (m > 0) {
    switch (m / (max_command + 1)) {
    case 0:
      zprint_string("vertical");
      break;
    case 1:
      zprint_string("horizontal");
      break;
    case 2:
      zprint_string("display math");;
    };
  } else {
    if (m == 0) {
      zprint_string("no");
    } else {
      switch ((-m) / (max_command + 1)) {
      case 0:
	zprint_string("internal vertical");
	break;
      case 1:
	zprint_string("restricted horizontal");
	break;
      case 2:
	zprint_string("math");;
      };
    };
  };
  zprint_string(" mode");
};


/* module 216 */

/* When \TeX's work on one level is interrupted, the state is saved by
 * calling |push_nest|. This routine changes |head| and |tail| so that
 * a new (empty) list is begun; it does not change |mode| or |aux|.
 */

void
push_nest (void) { /* enter a new semantic level, save the old */
  if (nest_ptr > max_nest_stack) {
    max_nest_stack = nest_ptr;
    if (nest_ptr == (unsigned)nest_size) {
	  nest = xrealloc_array(nest,sizeof(list_state_record)*nest_size*2); \
	  if (nest == NULL) {
		overflow ("semantic nest size", nest_size);
	  } else {
		nest_size = nest_size*2;
	  }
	}
  };
  nest[nest_ptr] = cur_list; /* stack the record */
  incr (nest_ptr);
  head = get_avail ();
  tail = head;
  prev_graf = 0;
  mode_line = line;
  eTeX_aux = null;
};


/* module 217 */

/* Conversely, when \TeX\ is finished on the current level, the former
 * state is restored by calling |pop_nest|. This routine will never be
 * called at the lowest semantic level, nor will it be called unless |head|
 * is a node that should be returned to free memory.
 */

void
pop_nest (void) { /* leave a semantic level, re-enter the old */
  free_avail (head);
  decr (nest_ptr);
  cur_list = nest[nest_ptr];
};

/* module 218 */

/* Here is a procedure that displays what \TeX\ is working on, at all levels.
 */

void 
show_activities (void) {
  int p;		/* index into |nest| */
  int m;			/* mode */
  memory_word a;		/* auxiliary */
  pointer q, r;			/* for showing the current page */
  integer t;			/* ditto */
  nest[nest_ptr] = cur_list;	/* put the top level into the array */
  print_nl_string("");
  print_ln();
  for (p = nest_ptr; p >= 0 ; p--) {
	m = nest[p].mode_field;
	a = nest[p].aux_field;
	print_nl_string("### ");
	print_mode (m);
	zprint_string(" entered at line ");
	print_int (abs (nest[p].ml_field));
	if (m == hmode) {
	  if (nest[p].pg_field != 8585216) {
		zprint_string(" (language");
		print_int (nest[p].pg_field % 65536);
		zprint_string(":hyphenmin");
		print_int (nest[p].pg_field / 4194304);
		print_char (',');
		print_int ((nest[p].pg_field / 65536) % 64);
		print_char (')');
	  };
	};
	if (nest[p].ml_field < 0)
	  zprint_string(" (\\output routine)");
	if (p == 0) {
	  /* begin expansion of Show the status of the current page */
	  /* module 1131 */
	  if (page_head != page_tail) {
		print_nl_string("### current page:");
		if (output_active)
		  zprint_string(" (held over for next output)");
		show_box (link (page_head));
		if (page_contents > empty) {
		  print_nl_string("total height ");
		  print_totals();
		  print_nl_string(" goal height ");
		  print_scaled (page_goal);
		  r = link (page_ins_head);
		  while (r != page_ins_head) {
			print_ln();
			print_esc_string ("insert");
			t = qo (subtype (r));
			print_int (t);
			zprint_string(" adds ");
			t = x_over_n (height (r), 1000) * count (t);
			print_scaled (t);
			if (type (r) == split_up) {
			  q = page_head;
			  t = 0;
			  do {
				q = link (q);
				if ((type (q) == ins_node) && (subtype (q) = subtype (r)))
				  incr (t);
			  } while (q != broken_ins (r));
			  zprint_string(", #");
			  print_int (t);
			  zprint_string(" might split");
			};
			r = link (r);
		  };
		};
	  };
	  /* end expansion of Show the status of the current page */
	  if (link (contrib_head) != null)
		print_nl_string("### recent contributions:");
	};
	show_box (link (nest[p].head_field));
	/* begin expansion of Show the auxiliary field, |a| */
	/* module 219 */
	switch (abs (m) / (max_command + 1)) {
	case 0:
	  print_nl_string("prevdepth ");
	  if (a.sc <= ignore_depth) {
		zprint_string("ignored");
	  } else {
		print_scaled (a.sc);
	  }
	  if (nest[p].pg_field != 0) {
	    zprint_string(", prevgraf ");
	    print_int (nest[p].pg_field);
	    if (nest[p].pg_field != 1) {
	      zprint_string(" lines");
	    } else {
	      zprint_string(" line");
	    };
	  };
	  break;
	case 1:
	  print_nl_string("spacefactor ");
	  print_int (a.hh.lhfield);
	  if (m > 0)
	    if (a.hh.rh > 0) {
	      zprint_string(", current language ");
	      print_int (a.hh.rh);
	    };
	  break;
	case 2:
	  if (a.cint != null) {
		zprint_string("this will be denominator of:");
		show_box (a.cint);
	  }; /* there are no other cases */
	  /* end expansion of Show the auxiliary field, |a| */
    }
  }
}


/* module 215 */

/* We will see later that the vertical list at the bottom semantic level is split
 * into two parts; the ``current page'' runs from |page_head| to |page_tail|,
 * and the ``contribution list'' runs from |contrib_head| to |tail| of
 * semantic level zero. The idea is that contributions are first formed in
 * vertical mode, then ``contributed'' to the current page (during which time
 * the page-breaking decisions are made). For now, we don't need to know
 * any more details about the page-building process.
 */

void
nest_initialize (void) {
  nest_ptr = 0;
  max_nest_stack = 0;
  mode = vmode;
  head = contrib_head;
  tail = contrib_head;
  eTeX_aux = null;
  prev_depth = ignore_depth;
  mode_line = 0;
  prev_graf = 0;
}


/* module 1389 */
void 
alter_prev_graf (void) {
  unsigned int p; /* index into |nest| */ 
  nest[nest_ptr] = cur_list;
  p = nest_ptr;
  while (abs (nest[p].mode_field) != vmode)
	decr (p);
  scan_optional_equals();
  scan_int();
  if (cur_val < 0) {
	print_err ("Bad ");
	print_esc_string ("prevgraf");
	help1 ("I allow only nonnegative values here.");
	int_error (cur_val);
  } else {
	nest[p].pg_field = cur_val;
	cur_list = nest[nest_ptr];
  };
};


/* module 1625 */

/* The modifications of \TeX\ required for the display produced by the
 * |show_save_groups| procedure were first discussed by Donald~E. Knuth in
 * {\sl TUGboat\/} {\bf 11}, 165--170 and 499--511, 1990.
 * 
 * In order to understand a group type we also have to know its mode.
 * Since unrestricted horizontal modes are not associated with grouping,
 * they are skipped when traversing the semantic nest.
 */
void 
show_save_groups (void)  {
  unsigned int p; /* index into |nest| */ 
  int m; /* mode */ 
  save_pointer v; /* saved value of |save_ptr| */ 
  quarterword l; /* saved value of |cur_level| */ 
  group_code c; /* saved value of |cur_group| */ 
  int a; /* to keep track of alignments */ 
  int i;
  quarterword j;
  char *sa;
  sa = ""; /*TH -Wall*/
  p = nest_ptr;
  nest[p] = cur_list; /* put the top level into the array */ 
  v = save_ptr;
  l = cur_level;
  c = cur_group;
  save_ptr = cur_boundary;
  decr (cur_level);
  a = 1;
  print_nl_string("");
  print_ln();
  loop {
	print_nl_string("### ");
	print_group (true);
	if (cur_group == bottom_level)
	  goto DONE;
	do {
	  m = nest[p].mode_field;
	  if (p > 0) {
		decr (p);
	  } else
		m = vmode;
	} while (m == hmode);
	zprint_string(" (");
	switch (cur_group) {
	case simple_group:
	  incr (p);
	  goto FOUND2;
	case hbox_group:
	case adjusted_hbox_group:
	  sa = "hbox";
	  break;
	case vbox_group:
	  sa = "vbox";
	  break;
	case vtop_group:
	  sa = "vtop";
	  break;
	case align_group:
	  if (a == 0) {
		if (m == -vmode) {
		  sa = "halign";
		} else {
		  sa = "valign";
		}
		a = 1;
		goto FOUND1;
	  } else {
		if (a == 1) {
		  zprint_string("align entry");
		} else {
		  print_esc_string ("cr");
		}
		if (p >= (unsigned)a)
		  p = p - a;
		a = 0;
		goto FOUND;
	  };
	case no_align_group:
	  incr (p);
	  a = -1;
	  print_esc_string ("noalign");
	  goto FOUND2;
	case output_group:
	  print_esc_string ("output");
	  goto FOUND;
	case math_group:
	  do_something;
	  goto FOUND2;
	case disc_group:
	case math_choice_group:
	  if (cur_group == disc_group) {
		print_esc_string ("discretionary");
	  } else {
		print_esc_string ("mathchoice");
	  }
	  for (i = 1; i <= 3; i++) {
		if (i <= saved (-2)) {
		  zprint_string("{}");
		}
	  }
	  do_something;
	  goto FOUND2;
	case insert_group:
	  if (saved (-2) == 255) {
		print_esc_string ("vadjust");
	  } else {
		print_esc_string ("insert");
		print_int (saved (-2));
	  };
	  goto FOUND2;
	case vcenter_group:
	  sa = "vcenter";
	  goto FOUND1;
	case semi_simple_group:
	  incr (p);
	  print_esc_string ("begingroup");
	  goto FOUND;
	case math_shift_group:
	  if (m == mmode) {
		print_char ('$');
	  } else if (nest[p].mode_field == mmode) {
		print_cmd_chr (eq_no, saved (-2));
		goto FOUND;
	  };
	  print_char ('$');
	  goto FOUND;
	case math_left_group:
	  if (type (nest[p + 1].eTeX_aux_field) == left_noad) {
		print_esc_string ("left");
	  } else {
		print_esc_string ("middle");
	  }
	  goto FOUND;
	}; /* there are no other cases */
	/* begin expansion of Show the box context */
	/* module 1627 */
	i = saved (-4);
	if (i != 0) {
	  if (i < box_flag) {
		if (abs (nest[p].mode_field) == vmode) {
		  j = hmove;
		} else {
		  j = vmove;
		}
		if (i > 0) {
		  print_cmd_chr (j, 0);
		} else {
		  print_cmd_chr (j, 1);
		}
		print_scaled (abs (i));
		zprint_string("pt");
	  } else if (i < ship_out_flag) {
		if (i >= global_box_flag) {
		  print_esc_string ("global");
		  i = i - (global_box_flag - box_flag);
		};
		print_esc_string ("setbox");
		print_int (i - box_flag);
		print_char ('=');
	  } else {
		print_cmd_chr (leader_ship, i - (leader_flag - a_leaders));
	  }
	  /* end expansion of Show the box context */
	}
  FOUND1:
	print_esc_string ((sa));
	/* begin expansion of Show the box packaging info */
	/* module 1626 */
	if (saved (-2) != 0) {
	  print_char (' ');
	  if (saved (-3) == exactly) {
		zprint_string("to");
	  } else {
		zprint_string("spread");
	  }
	  print_scaled (saved (-2));
	  zprint_string("pt");
	};
	/* end expansion of Show the box packaging info */
  FOUND2:
	print_char ('{');
  FOUND:
	print_char (')');
	decr (cur_level);
	cur_group = save_level (save_ptr);
	save_ptr = save_index (save_ptr); 
  };
 DONE:
  save_ptr = v;
  cur_level = l;
  cur_group = c;
}


