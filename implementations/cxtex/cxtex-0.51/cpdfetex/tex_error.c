
#include "types.h"
#include "c-compat.h"
#include "globals.h"

integer error_line; /* width of context lines on terminal error messages */ 
integer half_error_line; /* width of first lines of contexts in terminal error
						 messages; should be between 30 and |error_line-15| */

extern integer ready_already; /* a sacrifice of purity for economy */

/* module 13 */

/* In case somebody has inadvertently made bad settings of the ``constants,''
 * \TeX\ checks them using a global variable called |bad|.
 * 
 * This is the first of many sections of \TeX\ where global variables are
 * defined.
 */

int bad; /* is some ``constant'' wrong? */

/*
 * The global variable |history| records the worst level of error that
 * has been detected. It has four possible values: |spotless|, |warning_issued|,
 * |error_message_issued|, and |fatal_error_stop|.
 * 
 * Another global variable, |error_count|, is increased by one when an
 * |error| occurs without an interactive dialog, and it is reset to zero at
 * the end of every paragraph. If |error_count| reaches 100, \TeX\ decides
 * that there is no point in continuing further.
 */

unsigned char history; /* has the source input been clean so far? */ 
char error_count; /* the number of scrolled errors since the last paragraph ended */

/* module 73 */

/* The global variable |interaction| has four settings, representing increasing
 * amounts of user interaction:
 */

unsigned char interaction; /* current level of interaction */ 
unsigned char interaction_option; /* set from command line */

/* module 76 */

/* A global variable |deletions_allowed| is set |false| if the |get_next|
 * routine is active when |error| is called; this ensures that |get_next|
 * and related routines like |get_token| will never be called recursively.
 * A similar interlock is provided by |set_box_allowed|.
 */
boolean deletions_allowed; /* is it safe for |error| to call |get_token|? */
boolean set_box_allowed; /* is it safe to do a \.{\\setbox} assignment? */

/* module 96 */

/* Users occasionally want to interrupt \TeX\ while it's running.
 * If the \PASCAL\ runtime system allows this, one can implement
 * a routine that sets the global variable |interrupt| to some nonzero value
 * when such an interrupt is signalled. Otherwise there is probably at least
 * a way to make |interrupt| nonzero using the \PASCAL\ debugger.
 */
integer interrupt; /* should \TeX\ pause for instructions? */ 
boolean OK_to_interrupt; /* should interrupts be observed? */



/* module 84 */
#define edit_file  input_stack [ base_ptr ]

/* module 78 */

/* Since errors can be detected almost anywhere in \TeX, we want to declare the
 * error procedures near the beginning of the program. But the error procedures
 * in turn use some other procedures, which need to be declared |forward|
 * before we get to |error| itself.
 * 
 * It is possible for |error| to be called recursively if some error arises
 * when |get_token| is being used to delete a token, and/or if some fatal error
 * occurs while \TeX\ is trying to fix a non-fatal one. But such recursion
 * 
 * is never more than two levels deep.
 */

/* module 81 */

/* The |jump_out| procedure just cuts across all active procedure levels and
 * goes to |end_of_TEX|. This is the only nontrivial | goto| statement in the
 * whole program. It is used when there is no recovery from a particular error.
 * 
 * Some \PASCAL\ compilers do not implement non-local |goto| statements.
 * 
 * In such cases the body of |jump_out| should simply be
 * `|close_files_and_terminate|;\thinspace' followed by a call on some system
 * procedure that quietly terminates the program.
 */

void 
jump_out (void) {
  close_files_and_terminate();
  do_final_end;
};




/* module 82 */
/* Here now is the general |error| routine.  */
void
error (void) { /* completes the job of error reporting */
  ASCII_code c;			/* what the user types */
  int s1, s2, s3, s4;		/* used to save global variables when deleting tokens */
  if (history < error_message_issued)
    history = error_message_issued;
  print_char ('.');
  show_context();
  if (interaction == error_stop_mode) {
      /* begin expansion of Get user's advice and |return| */
      /* module 83 */
      loop {
      CONTINUE:
		clear_for_error_prompt();
		prompt_input ("? ");
		if (last == first)
		  return;
		c = buffer[first];
		if (c >= 'a')
		  c = c + 'A' - 'a'; /* convert to uppercase */
		/* begin expansion of Interpret code |c| and |return| if done */
		/* module 84 */
		switch (c) {
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
		  if (deletions_allowed) {
			/* begin expansion of Delete \(c)|c-"0"| tokens and |goto continue| */
			/* module 88 */
			/* We allow deletion of up to 99 tokens at a time. */
			s1 = cur_tok;
			s2 = cur_cmd;
			s3 = cur_chr;
			s4 = align_state;
			align_state = 1000000;
			OK_to_interrupt = false;
			if ((last > first + 1) && (buffer[first + 1] >= '0')
				&& (buffer[first + 1] <= '9')) {
			  c = c * 10 + buffer[first + 1] - '0' * 11;
			} else {
			  c = c - '0';
			};
			while (c > 0) {
			  get_token(); /* one-level recursive call of |error| is possible */
			  decr (c);
			};
			cur_tok = s1;
			cur_cmd = s2;
			cur_chr = s3;
			align_state = s4;
			OK_to_interrupt = true;
			help2 ("I have just deleted some text, as you asked.",
				   "You can now delete more, or insert, or whatever.");
			show_context();
			goto CONTINUE;
		  };
		  /* end expansion of Delete \(c)|c-"0"| tokens and |goto continue| */
		  do_something;
		  break;
#ifdef TEXMF_DEBUG
		  /* There is a secret `\.D' option available when the debugging routines haven't
		   * been commented~out.
		   */
		case 'D':
		  debug_help();
		  goto CONTINUE;
#endif /* TEXMF_DEBUG */
		  /* It is desirable to provide an `\.E' option here that gives the user
		   * an easy way to return from \TeX\ to the system editor, with the offending
		   * line ready to be edited.
		   * We do this by calling the external procedure |call_edit| with a pointer to
		   * the filename, its length, and the line number.
		   * However, here we just set up the variables that will be used as arguments,
		   * since we don't want to do the switch-to-editor until after TeX has closed
		   * its files.
		   */
		case 'E':
		  if (base_ptr > 0) {
			edit_name_start = str_start[edit_file.name_field];
			edit_name_length = str_start[edit_file.name_field + 1] - str_start[edit_file.name_field];
			edit_line = line;
			jump_out();
		  };
		  break;
		  /* begin expansion of Print the help information and |goto continue| */
		  /* module 89 */
		case 'H':
		  if (use_err_help) {
			give_err_help();
			use_err_help = false;
		  } else {
			if (help_ptr == 0)
			  help2 ("Sorry, I don't know how to help in this situation.",
					 "Maybe you should try asking a human?");
			do {
			  decr (help_ptr);
			  zprint_string (help_line[help_ptr]);
			  print_ln();
			} while (help_ptr != 0);
		  };
		  help4 ("Sorry, I already gave what help I could...",
				 "Maybe you should try asking a human?",
				 "An error might have occurred before I noticed any problems.",
		     "``If all else fails, read the instructions.''");
		  goto CONTINUE;
		  /* end expansion of Print the help information and |goto continue| */
		case 'I':
		  /* begin expansion of Introduce new material from the terminal and |return| */
		  /* module 87 */
		  /* When the following code is executed, |buffer[(first+1)..(last-1)]| may
		   * contain the material inserted by the user; otherwise another prompt will
		   * be given. In order to understand this part of the program fully, you need
		   * to be familiar with \TeX's input stacks.
		   */
		  begin_file_reading();
		  /* enter a new syntactic level for terminal input */
		  /* now |state=mid_line|, so an initial blank space will count as a blank */
		  if (last > first + 1) {
			loc = first + 1;
			buffer[first] = ' ';
		  } else {
			prompt_input ("insert>");
			loc = first;
		  };
		  first = last;
		  cur_input.limit_field = last - 1; /* no |end_line_char| ends this line */
		  return;
		  /* end expansion of Introduce new material from the terminal and |return| */
		case 'Q':
		case 'R':
		case 'S':
		  /* begin expansion of Change the interaction level and |return| */
		  /* module 86 */
		  /* Here the author of \TeX\ apologizes for making use of the numerical
		   * relation between |"Q"|, |"R"|, |"S"|, and the desired interaction settings
		   * |batch_mode|, |nonstop_mode|, |scroll_mode|.
		   */
		  error_count = 0;
		  interaction = batch_mode + c - 'Q';
		  zprint_string ("OK, entering ");
		  switch (c) {
		  case 'Q':
			print_esc_string ("batchmode");
			decr (selector);
			break;
		  case 'R':
			print_esc_string ("nonstopmode");
			break;
		  case 'S':
			print_esc_string ("scrollmode");
		  };
		  /* there are no other cases */
		  zprint_string ("...");
		  print_ln();
		  update_terminal;
		  return;
		  /* end expansion of Change the interaction level and |return| */
		case 'X':
		  interaction = scroll_mode;
		  jump_out();
		  break;
		};
		/* begin expansion of Print the menu of available options */
		/* module 85 */
		zprint_string ("Type <return> to proceed, S to scroll future error messages,");
		print_nl_string("R to run without stopping, Q to run quietly,");
		print_nl_string("I to insert something, ");
		if (base_ptr > 0)
		  zprint_string ("E to edit your file,");
		if (deletions_allowed)
		  print_nl_string ("1 or ... or 9 to ignore the next 1 to 9 tokens of input,");
		print_nl_string("H for help, X to quit.");
		/* end expansion of Print the menu of available options */
		/* end expansion of Interpret code |c| and |return| if done */
      };
  };
  /* end expansion of Get user's advice and |return| */
  incr (error_count);
  if (error_count == 100) {
    print_nl_string("(That makes 100 errors; please try again.)");
    history = fatal_error_stop;
    jump_out();
  };
  /* begin expansion of Put help message on the transcript file */
  /* module 90 */
  if (interaction > batch_mode)
    decr (selector);
  /* avoid terminal output */
  if (use_err_help) {
    print_ln();
    give_err_help();
  } else {
    while (help_ptr > 0) {
      decr (help_ptr);
      print_ln();
      zprint_string (help_line[help_ptr]);
    };
  }
  print_ln();
  if (interaction > batch_mode)
    incr (selector); /* re-enable terminal output */ 
  print_ln();
  /* end expansion of Put help message on the transcript file */
};


/* module 93 */
/* The following procedure prints \TeX's last words before dying.
 */
void
fatal_error (char *s) { /* prints |s|, and that's it */
  normalize_selector();
  print_err ("Emergency stop");
  help1 (s);
  succumb;
};

/* module 94 */
/* Here is the most dreaded error message.  */
void
overflow (char *s, int n) { /* stop due to finiteness */
  normalize_selector();
  print_err ("TeX capacity exceeded, sorry [");
  zprint_string (s);
  print_char ('=');
  print_int (n);
  print_char (']');
  help2 ("If you really absolutely need more capacity,",
	 "you can ask a wizard to enlarge me.");
  succumb;
};

/* module 95 */

/* The program might sometime run completely amok, at which point there is
 * no choice but to stop. If no previous error has been detected, that's bad
 * news; a message is printed that is really intended for the \TeX\
 * maintenance person instead of the user (unless the user has been
 * particularly diabolical). The index entries for `this can't happen' may
 * help to pinpoint the problem.
 */

void
confusion (char *s) { /* consistency check violated; |s| tells where */
  normalize_selector();
  if (history < error_message_issued) {
    print_err ("This can't happen (");
    zprint_string (s);
    print_char (')');
    help1 ("I'm broken. Please show this to someone who can fix can fix");
  } else {
    print_err ("I can't go on meeting you like this");
    help2 ("One of your faux pas seems to have wounded me deeply...",
	   "in fact, I'm barely conscious. Please fix it and try again.");
  };
  succumb;
};

/* module 92 */

/* In anomalous cases, the zprint selector might be in an unknown state;
 * the following subroutine is called to fix things just enough to keep
 * running a bit longer.
 */
void
normalize_selector (void) {
  if (log_opened) {
    selector = term_and_log;
  } else {
    selector = term_only;
  }
  if (jobname == 0)
    open_log_file();
  if (interaction == batch_mode)
    decr (selector);
};


/* module 98 */

/* When an interrupt has been detected, the program goes into its
 * highest interaction level and lets the user have nearly the full flexibility of
 * the |error| routine. \TeX\ checks for interrupts only at times when it is
 * safe to do this.
 */
void
pause_for_instructions (void) {
  if (OK_to_interrupt) {
    interaction = error_stop_mode;
    if ((selector == log_only) || (selector == no_print))
      incr (selector);
    print_err ("Interruption");
    help3 ("You rang?",
	   "Try to insert some instructions for me (e.g.,`I\\showlists'),",
	   "unless you just want to quit by typing `X'.");
    deletions_allowed = false;
    error();
    deletions_allowed = true;
    interrupt = 0;
  };
};


/* module 91 */

/* A dozen or so error messages end with a parenthesized integer, so we
 * save a teeny bit of program space by declaring the following procedure:
 */
void
int_error (int n) {
  zprint_string (" (");
  print_int (n);
  print_char (')');
  error();
};

/* module 306 */

/* Here is a procedure that uses |scanner_status| to zprint a warning message
 * when a subfile has ended, and at certain other crucial times:
 */

void
runaway (void) {
  pointer p;			/* head of runaway list */
  if (scanner_status > skipping) { 
    p = def_ref;
    switch (scanner_status)
      {
      case defining:
	{
	  print_nl_string("Runaway definition"); /*p = def_ref;*/
	};
	break;
      case matching:
	{
	  print_nl_string("Runaway argument");
	  p = temp_head;
	};
	break;
      case aligning:
	{
	  print_nl_string("Runaway preamble");
	  p = hold_head;
	};
	break;
      case absorbing:
	{
	  print_nl_string("Runaway text"); /*p = def_ref;*/
	};
      };    /* there are no other cases */
    print_char ('?');
    print_ln();
    show_token_list (link (p), null, error_line - 10);
  };
};

/* module 330 */

/* In order to keep the stack from overflowing during a long sequence of
 * inserted `\.{\\show}' commands, the following routine removes completed
 * error-inserted lines from memory.
 */
void
clear_for_error_prompt (void) {
  while ((state != token_list) && terminal_input && (input_ptr > 0) && (loc > limit))
    end_file_reading();
  print_ln();
  clear_terminal;
};


/* module 660 */

/* Basic printing procedures for PDF output are very similiar to \TeX\ basic
 * printing ones but the output is going to PDF buffer. Subroutines with
 * suffix |_ln| append a new-line character to the PDF output.
 */
void
pdf_error (str_number t, str_number p) {
  normalize_selector();
  print_err ("pdfTeX error");
  if (t != 0) {
    zprint_string (" (");
    zprint (t);
    zprint (')');
  };
  zprint_string (": ");
  zprint (p);
  succumb;
};

void
pdf_error_string(char *t, char *p) {
  normalize_selector();
  print_err ("pdfTeX error");
  if (t != 0) {
    zprint_string (" (");
    zprint_string (t);
    zprint (')');
  };
  zprint_string (": ");
  zprint_string (p);
  succumb;
};

void
pdf_warning (str_number t, str_number p, boolean append_nl) {
  print_err ("pdfTeX warning");
  if (t != 0) {
    zprint_string (" (");
    zprint (t);
    zprint (')');
  };
  zprint_string(": ");
  zprint (p);
  if (append_nl)
    print_ln();
};

void
pdf_warning_string (char *t, char *p, boolean append_nl) {
  print_err ("pdfTeX warning");
  if (t != 0) {
    zprint_string (" (");
    zprint_string (t);
    zprint (')');
  };
  zprint_string(": ");
  zprint_string (p);
  if (append_nl)
    print_ln();
};

/* module 408 */

/* Here is a procedure that sounds an alarm when mu and non-mu units
 * are being switched.
 */
void
mu_error (void) {
  print_err ("Incompatible glue units");
  help1 ("I'm going to assume that 1mu=1pt when they're mixed.");
  error();
};



/* module 327 */

/* The |back_error| routine is used when we want to replace an offending token
 * just before issuing an error message. This routine, like |back_input|,
 * requires that |cur_tok| has been set. We disable interrupts during the
 * call of |back_input| so that the help message won't be lost.
 */
void
back_error (void) { /* back up one token and call |error| */
  OK_to_interrupt = false;
  back_input();
  OK_to_interrupt = true;
  error();
};

void
ins_error (void) { /* back up one inserted token and call |error| */
  OK_to_interrupt = false;
  back_input();
  token_type = inserted;
  OK_to_interrupt = true;
  error();
};


/* module 1194 */

/* The `|you_cant|' procedure prints a line saying that the current command
 * is illegal in the current mode; it identifies these things symbolically.
 */
void 
you_cant (void) {
  print_err ("You can't use `");
  print_cmd_chr (cur_cmd, cur_chr);
  zprint_string("' in ");
  print_mode (mode);
};

/* module 1195 */
void 
report_illegal_case (void) {
  you_cant();
  help4 ("Sorry, but I'm not programmed to handle this case;",
	 "I'll just pretend that you didn't ask for it.",
	 "If you're in the wrong mode, you might be able to",
	 "return to the right one by typing `I}' or `I$' or `I\\par'.");
  error();
};

/* module 1214 */
void 
extra_right_brace (void) {
  print_err ("Extra }, or forgotten ");
  switch (cur_group) {
  case semi_simple_group:
    print_esc_string ("endgroup");
    break;
  case math_shift_group:
    print_char ('$');
    break;
  case math_left_group:
    print_esc_string ("right");
    ;
  };
  help5 ("I've deleted a group-closing symbol because it seems to be",
	 "spurious, as in `$x}$'. But perhaps the } is legitimate and",
	 "you forgot something else, as in `\\hbox{$x}'. In such cases",
	 "the way to recover is to insert both the forgotten and the",
	 "deleted material, e.g., by typing `I$}'.");
  error();
  incr (align_state);
};


/* module 1209 */

/* We have to deal with errors in which braces and such things are not
 * properly nested. Sometimes the user makes an error of commission by
 * inserting an extra symbol, but sometimes the user makes an error of omission.
 * \TeX\ can't always tell one from the other, so it makes a guess and tries
 * to avoid getting into a loop.
 * 
 * The |off_save| routine is called when the current group code is wrong. It tries
 * to insert something into the user's input that will help clean off
 * the top level.
 */

void 
off_save (void) {
  pointer p;  /* inserted token */ 
  if (cur_group == bottom_level) {
    /* begin expansion of Drop current token and complain that it was unmatched */
    /* module 1211 */
    print_err ("Extra ");
    print_cmd_chr (cur_cmd, cur_chr);
    help1 ("Things are pretty mixed up, but I think the worst is over.");
    error();
    /* end expansion of Drop current token and complain that it was unmatched */
  } else {
    back_input();
    p = get_avail();
    link (temp_head) = p;
    print_err ("Missing ");
    /* begin expansion of Prepare to insert a token that matches |cur_group|, and zprint what it is */
    /* module 1210 */
    /* At this point, |link(temp_head)=p|, a pointer to an empty one-word node. */
    switch (cur_group) {
    case semi_simple_group:
      {
	info (p) = cs_token_flag + frozen_end_group;
	print_esc_string ("endgroup");
      };
      break;
    case math_shift_group:
      {
	info (p) = math_shift_token + '$';
	print_char ('$');
      };
      break;
    case math_left_group:
      {
	info (p) = cs_token_flag + frozen_right;
	link (p) = get_avail();
	p = link (p);
	info (p) = other_token + '.';
	print_esc_string ("right.");
      };
      break;
    default:
      {
	info (p) = right_brace_token + '}';
	print_char ('}');
      };
    };
    /* end expansion of Prepare to insert a token that matches |cur_group|, and zprint what it is */
    zprint_string(" inserted");
    ins_list (link (temp_head));
    help5 ("I've inserted something that you may have forgotten.",
	   "(See the <inserted text> above.)",
	   "With luck, this will get me unwedged. But if you",
	   "really didn't forget anything, try typing `2' now; then",
	   "my insertion and my current dilemma will both disappear.");
    error();
  };
};

/* module 1280 */
void 
cs_error (void) {
  print_err ("Extra ");
  print_esc_string ("endcsname");
  help1  ("I'm ignoring this, since I wasn't doing a \\csname.");
  error();
};

/* module 1812 */

/* 
 * Here are extra variables for Web2c. (This numbering of the
 * system-dependent section allows easy integration of Web2c and e-\TeX, etc.)
 */

pool_pointer edit_name_start; /* where the filename to switch to starts */
int edit_name_length, edit_line; /* what line to start editing at */


void
error_initialize (void) {
  /* module 74 */
  if (interaction_option == unspecified_mode) {
    interaction = error_stop_mode;
  } else {
    interaction = interaction_option;
  }
  /* module 77 */
  /* The value of |history| is initially |fatal_error_stop|, but it will
   * be changed to |spotless| if \TeX\ survives the initialization process.
   */
  deletions_allowed = true;
  set_box_allowed = true;
  /* |history| is initialized elsewhere */
  error_count = 0;
  /* module 97 */
  interrupt = 0;
  OK_to_interrupt = true;
  /* module 1813 */
  /* The |edit_name_start| will be set to point into |str_pool| somewhere after
   * its beginning if \TeX\ is supposed to switch to an editor on exit.
   */
  edit_name_start = 0;
  stop_at_space = true;
}

