#include "types.h"
#include "c-compat.h"
#include "globals.h"


/* module 1438 */
void 
show_whatever (void) {
  pointer p; /* tail of a token list to show */ 
  small_number t; /* type of conditional being shown */ 
  unsigned char m; /* upper bound on |fi_or_else| codes */ 
  int l; /* line where that conditional began */ 
  int n; /* level of \.{\\if...\\fi} nesting */ 
  switch (cur_chr) {
  case show_lists:
	begin_diagnostic();
	show_activities();
	break;
  case show_box_code:
	/* begin expansion of Show the current contents of a box */
	/* module 1441 */
	scan_register_num();
	fetch_box (p);
	begin_diagnostic();
	print_nl_string("> \\box");
	print_int (cur_val);
	print_char ('=');
	if (p == null) {
	  zprint_string("void");
	} else {
	  show_box (p);
	}
	/* end expansion of Show the current contents of a box */
	break;
  case show_code:
	/* begin expansion of Show the current meaning of a token, then |goto common_ending| */
	/* module 1439 */
	get_token();
	if (interaction == error_stop_mode)
	  wake_up_terminal;
	print_nl_string("> ");
	if (cur_cs != 0) {
	  sprint_cs (cur_cs);
	  print_char ('=');
	};
	print_meaning();
	goto COMMON_ENDING;
	/* end expansion of Show the current meaning of a token, then |goto common_ending| */
	/* begin expansion of Cases for |show_whatever| */
	/* module 1623 */
  case show_groups:
	begin_diagnostic();
	show_save_groups();
	break;
	/* module 1637 */
  case show_ifs:
	begin_diagnostic();
	print_nl_string("");
	print_ln();
	if (cond_ptr == null) {
	  print_nl_string("### ");
	  zprint_string("no active conditionals");
	} else {
	  p = cond_ptr;
	  n = 0;
	  do {
		incr (n);
		p = link (p);
	  } while (p != null);
	  p = cond_ptr;
	  t = cur_if;
	  l = if_line;
	  m = if_limit;
	  do {
		print_nl_string("### level ");
		print_int (n);
		zprint_string(": ");
		print_cmd_chr (if_test, t);
		if (m == fi_code)
		  print_esc_string ("else");
		print_if_line (l);
		decr (n);
		t = subtype (p);
		l = if_line_field (p);
		m = type (p);
		p = link (p);
	  } while (p != null);
	};
	do_something;
	break;
  default:
	/* begin expansion of Show the current value of some parameter or register, then |goto common_ending| */
	/* module 1442 */
	p = the_toks();
	if (interaction == error_stop_mode)
	  wake_up_terminal;
	print_nl_string("> ");
	token_show (temp_head);
	flush_list (link (temp_head));
	goto COMMON_ENDING;
	/* end expansion of Show the current value of some parameter or register, then |goto common_ending| */
  };
  /* begin expansion of Complete a potentially long \.{\\show} command */
  /* module 1443 */
  end_diagnostic (true);
  print_err ("OK");
  if (selector == term_and_log)
	if (tracing_online <= 0) {
	  selector = term_only;
	  zprint_string(" (see the transcript file)");
	  selector = term_and_log;
	};
  /* end expansion of Complete a potentially long \.{\\show} command */
 COMMON_ENDING:
  if (interaction < error_stop_mode) {
	help0;
	decr (error_count);
  } else if (tracing_online > 0) {
	help3 ("This isn't an error message; I'm just \\showing something.",
		   "Type `I\\show...' to show more (e.g., \\show\\cs,",
		   "\\showthe\\count10, \\showbox255, \\showlists).");
  } else {
	help5 ("This isn't an error message; I'm just \\showing something.",
		   "Type `I\\show...' to show more (e.g., \\show\\cs,",
		   "\\showthe\\count10, \\showbox255, \\showlists).",
		   "And type `I\\tracingonline=1\\show...' to show boxes and",
		   "lists on your terminal as well as in the transcript file.");
  };
  error();
}

/* module 1607 */

/* The |print_group| procedure prints the current level of grouping and
 * the name corresponding to |cur_group|.
 */
void
print_group (boolean e) {
  switch (cur_group) {
  case bottom_level:
	zprint_string("bottom level");
	return;
	break;
  case simple_group:
  case semi_simple_group:
	if (cur_group == semi_simple_group)
	  zprint_string("semi ");
	zprint_string("simple");
	break;
  case hbox_group:
  case adjusted_hbox_group:
	if (cur_group == adjusted_hbox_group)
	  zprint_string("adjusted ");
	zprint_string("hbox");
	break;
  case vbox_group:
	zprint_string("vbox");
	break;
  case vtop_group:
	zprint_string("vtop");
	break;
  case align_group:
  case no_align_group:
	if (cur_group == no_align_group)
	  zprint_string("no ");
	zprint_string("align");
	break;
  case output_group:
	zprint_string("output");
	break;
  case disc_group:
	zprint_string("disc");
	break;
  case insert_group:
	zprint_string("insert");
	break;
  case vcenter_group:
	zprint_string("vcenter");
	break;
  case math_group:
  case math_choice_group:
  case math_shift_group:
  case math_left_group:
	zprint_string("math");
	if (cur_group == math_choice_group) {
	  zprint_string(" choice");
	} else if (cur_group == math_shift_group) {
	  zprint_string(" shift");
	} else if (cur_group == math_left_group) {
	  zprint_string(" left");
	}
  };  /* there are no other cases */
  zprint_string(" group (level ");
  print_int (qo (cur_level));
  print_char (')');
  if (saved (-1) != 0) {
	if (e) {
	  zprint_string(" entered at line ");
	} else {
	  zprint_string(" at line ");
	}
	print_int (saved (-1));
  };
}
