#include "types.h"
#include "c-compat.h"
#include "globals.h"

/* module 1598 */
unsigned char eTeX_mode; /* identifies compatibility and extended mode */

/* module 1602 */

/* The |eTeX_enabled| function simply returns its first argument as
 * result. This argument is |true| if an optional \eTeX\ feature is
 * currently enabled; otherwise, if the argument is |false|, the function
 * gives an error message.
 */
boolean 
eTeX_enabled (boolean b, quarterword j, halfword k) {
  if (!b) {
	print_err ("Improper ");
	print_cmd_chr (j, k);
	help1 ("Sorry, this optional e-TeX feature has been disabled.");
	error();
  };
  return b;
};

/* module  1606 */

/* In order to handle \.{\\everyeof} we need an array |eof_seen| of
 * boolean variables.
 */

boolean *eof_seen; /* has eof been seen? */


/* module 1608 */

/* The |group_trace| procedure is called when a new level of grouping
 * begins (|e=false|) or ends (|e=true|) with |saved(-1)| containing the
 * line number.
 */

void
group_trace (boolean e) {
  begin_diagnostic();
  print_char ('{');
  if (e) {
    zprint_string("leaving ");
  } else {
    zprint_string("entering ");
  };
  print_group (e);
  print_char ('}');
  end_diagnostic (false);
};



/* module  1695 */

/* The global variable |pseudo_files| is used to maintain a stack of
 * pseudo files. The |info| field of each pseudo file points to a linked
 * list of variable size nodes representing lines not yet processed: the
 * |info| field of the first word contains the size of this node, all the
 * following words contain ASCII codes.
 */
pointer pseudo_files; /* stack of pseudo files */

/* module 1696 */
void
etex_initialize (void) {
  pseudo_files = null;
}



/* module 1698 */
void
pseudo_start (void) {
  unsigned char old_setting; /* holds |selector| setting */ 
  str_number s; /* string to be converted into a pseudo file */ 
  pool_pointer l, m; /* indices into |str_pool| */ 
  pointer p, q, r; /* for list construction */ 
  four_quarters w; /* four ASCII codes */ 
  int nl, sz;
  scan_general_text();
  old_setting = selector;
  selector = new_string;
  token_show (temp_head);
  selector = old_setting;
  flush_list (link (temp_head));
  str_room (1);
  s = make_string();
  /* begin expansion of Convert string |s| into a new pseudo file */
  /* module 1699 */
  str_pool[pool_ptr] = ' ';
  l = str_start[s];
  nl = new_line_char;
  p = get_avail();
  q = p;
  while (l < pool_ptr) {
	m = l;
	while ((l < pool_ptr) && (str_pool[l] != nl))
	  incr (l);
	sz = (l - m + 7) / 4;
	if (sz == 1)
	  sz = 2;
	r = get_node (sz);
	link (q) = r;
	q = r;
	info (q) = hi (sz);
	while (sz > 2) {
	  decr (sz);
	  incr (r);
	  w.b0 = qi (str_pool[m]);
	  w.b1 = qi (str_pool[m + 1]);
	  w.b2 = qi (str_pool[m + 2]);
	  w.b3 = qi (str_pool[m + 3]);
	  mem[r].qqqq = w;
	  m = m + 4;
	};
	w.b0 = qi (' ');
	w.b1 = qi (' ');
	w.b2 = qi (' ');
	w.b3 = qi (' ');
	if (l > m) {
	  w.b0 = qi (str_pool[m]);
	  if (l > m + 1) {
		w.b1 = qi (str_pool[m + 1]);
		if (l > m + 2) {
		  w.b2 = qi (str_pool[m + 2]);
		  if (l > m + 3)
		    w.b3 = qi (str_pool[m + 3]);
		};
	  };
	};
	mem[r + 1].qqqq = w;
	if (str_pool[l] == nl)
	  incr (l);
  };
  info (p) = link (p);
  link (p) = pseudo_files;
  pseudo_files = p;
  /* end expansion of Convert string |s| into a new pseudo file */
  flush_string;
  /* begin expansion of Initiate input from new pseudo file */
  /* module 1700 */
  begin_file_reading();
  /* set up |cur_file| and new level of input */ line = 0;
  limit = start;
  loc = limit + 1; /* force line read */ 
  if (tracing_scan_tokens > 0) {
	if (term_offset > (unsigned)max_print_line - 3) {
	  print_ln();
	} else if ((term_offset > 0) || (file_offset > 0)) {
	  print_char (' ');
	}
	name = 19;
	zprint_string("( ");
	incr (open_parens);
	update_terminal;
  } else {
    name = 18;
  }
  /* end expansion of Initiate input from new pseudo file */
};


/* module 1701 */

/* Here we read a line from the current pseudo file into |buffer|.
 */
boolean
pseudo_input (void) { /* inputs the next line or returns |false| */
  pointer p;			/* current line from pseudo file */
  int sz;			/* size of node |p| */
  in_state_record save_cur_input;
  four_quarters w;		/* four ASCII codes */
  pointer r;			/* loop index */
  last = first;			/* cf.\ Matthew 19:30 */
  p = info (pseudo_files);
  if (p == null) {
	return false;
  } else {
	info (pseudo_files) = link (p);
	sz = ho (info (p));
	if (4 * sz - 3 >= (int)(buf_size - last)) {
	  /* begin expansion of Report overflow of the input buffer, and abort */
	  /* module 35 */
	  /* We need a special routine to read the first line of \TeX\ input from
	   * the user's terminal. This line is different because it is read before we
	   * have opened the transcript file; there is sort of a ``chicken and
	   * egg'' problem here. If the user types `\.{\\input paper}' on the first
	   * line, or if some macro invoked by that line does such an \.{\\input},
	   * the transcript file will be named `\.{paper.log}'; but if no \.{\\input}
	   * commands are performed during the first line of terminal input, the transcript
	   * file will acquire its default name `\.{texput.log}'. (The transcript file
	   * will not contain error messages generated by the first line before the
	   * first \.{\\input} command.)
	   * 
	   * The first line is even more special if we are lucky enough to have an operating
	   * system that treats \TeX\ differently from a run-of-the-mill \PASCAL\ object
	   * program. It's nice to let the user start running a \TeX\ job by typing
	   * a command line like `\.{tex paper}'; in such a case, \TeX\ will operate
	   * as if the first line of input were `\.{paper}', i.e., the first line will
	   * consist of the remainder of the command line, after the part that invoked
	   * \TeX.
	   * 
	   * The first line is special also because it may be read before \TeX\ has
	   * input a format file. In such cases, normal error messages cannot yet
	   * be given. The following code uses concepts that will be explained later.
	   * (If the \PASCAL\ compiler does not support non-local | goto|\unskip, the
	   * 
	   * statement `|goto final_end|' should be replaced by something that
	   * quietly terminates the program.)
	   * 
	   * Routine is implemented in C; part of module is, however, needed for e-TeX.
	   */
	  save_cur_input = cur_input;
	  cur_input.loc_field = first;
	  cur_input.limit_field = last - 1;
	  realloc_buffer();
	  cur_input = save_cur_input;
	};
	/* end expansion of Report overflow of the input buffer, and abort */
	last = first;
	for (r = p + 1; r <= p + sz - 1; r++) {
	  w = mem[r].qqqq;
	  buffer[last] = w.b0;
	  buffer[last + 1] = w.b1;
	  buffer[last + 2] = w.b2;
	  buffer[last + 3] = w.b3;
	  last = last + 4;
	};
	if (last >= (unsigned)max_buf_stack)
	  max_buf_stack = last + 1;
	while ((last > first) && (buffer[last - 1] == ' '))
	  decr (last);
	free_node (p, sz);
	return true;
  };
};

/* module 1702 */

/* When we are done with a pseudo file we `close' it.  */
void
pseudo_close (void) {/* close the top level pseudo file */
  pointer p, q;
  p = link (pseudo_files);
  q = info (pseudo_files);
  free_avail (pseudo_files);
  pseudo_files = p;
  while (q != null) {
	p = q;
	q = link (p);
	free_node (p, ho (info (p)));
  };
};



/* module 1718 */

/* A group entered (or a conditional started) in one file may end in a
 * different file. Such slight anomalies, although perfectly legitimate,
 * may cause errors that are difficult to locate. In order to be able to
 * give a warning message when such anomalies occur, \eTeX\ uses the
 * |grp_stack| and |if_stack| arrays to record the initial |cur_boundary|
 * and |cond_ptr| values for each input file.
 */

save_pointer *grp_stack; /* initial |cur_boundary| */
pointer *if_stack; /* initial |cond_ptr| */

/* module 1719 */

/* When a group ends that was apparently entered in a different input
 * file, the |group_warning| procedure is invoked in order to update the
 * |grp_stack|. If moreover \.{\\tracingnesting} is positive we want to
 * give a warning message. The situation is, however, somewhat complicated
 * by two facts: (1)~There may be |grp_stack| elements without a
 * corresponding \.{\\input} file or \.{\\scantokens} pseudo file (e.g.,
 * error insertions from the terminal); and (2)~the relevant information is
 * recorded in the |name_field| of the |input_stack| only loosely
 * synchronized with the |in_open| variable indexing |grp_stack|.
 */

void
group_warning (void) {
  unsigned int i; /* index into |grp_stack| */ 
  boolean w; /* do we need a warning? */ 
  base_ptr = input_ptr;   
  input_stack[base_ptr] = cur_input; /* store current state */ 
  i = in_open;
  w = false;
  while ((grp_stack[i] == cur_boundary) && (i > 0)) {
    /* begin expansion of Set variable |w| to indicate if this case should be reported */
    set_w_for_reporting;
    /* end expansion of Set variable |w| to indicate if this case should be reported */
    grp_stack[i] = save_index (save_ptr);
    decr (i);
  };
  if (w) {
    print_nl_string("Warning: end of ");
    print_group (true);
    zprint_string(" of a different file");
    print_ln();
    if (tracing_nesting > 1)
      show_context();
    if (history == spotless)
      history = warning_issued;
  };
};



/* module 1722 */

/* Conversely, the |file_warning| procedure is invoked when a file ends
 * and some groups entered or conditionals started while reading from that
 * file are still incomplete.
 */
void
file_warning (void) {
  pointer p;			/* saved value of |save_ptr| or |cond_ptr| */
  quarterword l;		/* saved value of |cur_level| or |if_limit| */
  quarterword c;		/* saved value of |cur_group| or |cur_if| */
  int i;			/* saved value of |if_line| */
  p = save_ptr;
  l = cur_level;
  c = cur_group;
  save_ptr = cur_boundary;
  while (grp_stack[in_open] != (unsigned)save_ptr) {
	decr (cur_level);
	print_nl_string("Warning: end of file when ");
	print_group (true);
	zprint_string(" is incomplete");
	cur_group = save_level (save_ptr);
	save_ptr = save_index (save_ptr);
  };
  save_ptr = p;
  cur_level = l;
  cur_group = c;
  /* restore old values */
  p = cond_ptr;
  l = if_limit;
  c = cur_if;
  i = if_line;
  while (if_stack[in_open] != cond_ptr) {
	print_nl_string("Warning: end of file when ");
	print_cmd_chr (if_test, cur_if);
	if (if_limit == fi_code)
	  print_esc_string ("else");
	print_if_line (if_line);
	zprint_string(" is incomplete");
	if_line = if_line_field (cond_ptr);
	cur_if = subtype (cond_ptr);
	if_limit = type (cond_ptr);
	cond_ptr = link (cond_ptr);
  };
  cond_ptr = p;
  if_limit = l;
  cur_if = c;
  if_line = i;
  /* restore old values */
  print_ln();
  if (tracing_nesting > 1)
    show_context();
  if (history == spotless)
    history = warning_issued;
};


/* module 1759 */
halfword max_reg_num; /* largest allowed register number */
char *max_reg_help_line; /* first line of help message */

/* module 1787 */

pointer last_line_fill; /* the |par_fill_skip| glue node of the new paragraph*/
boolean  do_last_line_fit; /* special algorithm for last line of paragraph? */
small_number  active_node_size; /* number of words in active nodes */
scaled fill_width[3]; /* infinite stretch components of |par_fill_skip| */
scaled  best_pl_short[tight_fit]; /* |shortfall| corresponding to 
									 |minimal_demerits| */
scaled best_pl_glue[tight_fit]; /* corresponding glue stretch or shrink */

/* module 1804 */

/* When |saving_vdiscards| is positive then the glue, kern, and penalty
 * nodes removed by the page builder or by \.{\\vsplit} from the top of a
 * vertical list are saved in special lists instead of being discarded.
 */

pointer disc_ptr[(vsplit_code+1)]; /* list pointers */


void
dump_etex_stuff (void) {
  int j;
  /* module 1600 */
  dump_int (eTeX_mode);
  for (j = 0; j <= eTeX_states - 1; j++)
	eTeX_state (j) = 0;  /* disable all enhancements */
  /* module 1703 */
  while (pseudo_files != null)
	pseudo_close();   /* flush pseudo files */
}


