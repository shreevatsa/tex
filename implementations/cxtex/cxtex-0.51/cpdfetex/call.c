
#include "types.h"
#include "c-compat.h"


#include "globals.h"


/* module 366 */

/* 
 * Only a dozen or so command codes |>max_command| can possibly be returned by
 * |get_next|; in increasing order, they are |undefined_cs|, |expand_after|,
 * |no_expand|, |input|, |if_test|, |fi_or_else|, |cs_name|, |convert|, |the|,
 * |top_bot_mark|, |call|, |long_call|, |outer_call|, |long_outer_call|, and
 * |end_template|.{\emergencystretch=40pt\par}
 * 
 * The |expand| subroutine is used when |cur_cmd>max_command|. It removes a
 * ``call'' or a conditional or one of the other special operations just
 * listed. It follows that |expand| might invoke itself recursively. In all
 * cases, |expand| destroys the current token, but it sets things up so that
 * the next |get_next| will deliver the appropriate next token. The value of
 * |cur_tok| need not be known when |expand| is called.
 * 
 * Since several of the basic scanning routines communicate via global variables,
 * their values are saved as local variables of |expand| so that
 * recursive calls don't invalidate them.
 */

/* module 387 */

/* A control sequence that has been \.{\\def}'ed by the user is expanded by
 * \TeX's |macro_call| procedure.
 *
 * Now let's consider |macro_call| itself, which is invoked when \TeX\ is
 * scanning a control sequence whose |cur_cmd| is either |call|, |long_call|,
 * |outer_call|, or |long_outer_call|. The control sequence definition
 * appears in the token list whose reference count is in location |cur_chr|
 * of |mem|.
 * 
 * The global variable |long_state| will be set to |call| or to |long_call|,
 * depending on whether or not the control sequence disallows \.{\\par}
 * in its parameters. The |get_next| routine will set |long_state| to
 * |outer_call| and emit \.{\\par}, if a file ends or if an \.{\\outer}
 * control sequence occurs in the midst of an argument.
 */

unsigned int global_long_state; /* governs the acceptance of \.{\\par} */

/* module 388 */

/* The parameters, if any, must be scanned before the macro is expanded.
 * Parameters are token lists without reference counts. They are placed on
 * an auxiliary stack called |pstack| while they are being scanned, since
 * the |param_stack| may be losing entries during the matching process.
 * (Note that |param_stack| can't be gaining entries, since |macro_call| is
 * the only routine that puts anything onto |param_stack|, and it
 * is not recursive.)
 */

pointer pstack[9]; /* arguments supplied to a macro */


/* module 396 */
/* 
 * If |long_state=outer_call|, a runaway argument has already been reported.
 */

#define report_a_runaway  { if (get_long_state() == call) {	\
		    runaway();\
		    print_err  ("Paragraph ended before ");\
		    sprint_cs (warning_index);\
		    zprint_string(" was complete");\
		    help3 ("I suspect you've forgotten a `}', causing me to apply this",\
		       "control sequence to too much text. How can we recover?",\
		       "My plan is to forget the whole thing and hope for the best.");\
		    back_error();\
		  };\
		pstack[n] = link (temp_head);\
		align_state = align_state - unbalance;\
		for (m = 0; m <= n; m++)\
		  flush_list (pstack[m]);\
		return; }




/* module 389 */

/* After parameter scanning is complete, the parameters are moved to the
 * |param_stack|. Then the macro body is fed to the scanner; in other words,
 * |macro_call| places the defined text of the control sequence at the
 * top of\/ \TeX's input stack, so that |get_next| will proceed to read it
 * next.
 * 
 * The global variable |cur_cs| contains the |eqtb| address of the control sequence
 * being expanded, when |macro_call| begins. If this control sequence has not been
 * declared \.{\\long}, i.e., if its command code in the |eq_type| field is
 * not |long_call| or |long_outer_call|, its parameters are not allowed to contain
 * the control sequence \.{\\par}. If an illegal \.{\\par} appears, the macro
 * call is aborted, and the \.{\\par} will be rescanned.
 */
void
macro_call (void) {				/* invokes a user-defined control sequence */
  pointer r; /* current node in the macro's token list */ 
  pointer p; /* current node in parameter token list being built */ 
  pointer q; /* new node being put into the token list */ 
  pointer s; /* backup pointer for parameter matching */ 
  pointer t; /* cycle pointer for backup recovery */ 
  pointer u, v; /* auxiliary pointers for backup recovery */ 
  pointer rbrace_ptr; /* one step before the last |right_brace| token */ 
  small_number n; /* the number of parameters scanned */ 
  halfword unbalance; /* unmatched left braces in current parameter */ 
  halfword m; /* the number of tokens or groups (usually) */ 
  pointer ref_count; /* start of the token list */ 
  small_number save_scanner_status; /* |scanner_status| upon entry */ 
  pointer save_warning_index; /* |warning_index| upon entry */ 
  ASCII_code match_chr; /* character used in parameter */
  save_scanner_status = scanner_status;
  save_warning_index = warning_index;
  warning_index = cur_cs;
  ref_count = cur_chr;
  r = link (ref_count);
  n = 0;
  p = null; /*TH -Wall -O2 */
  rbrace_ptr = null; /*TH -Wall -O2 */
  m = 0; /*TH -Wall -O2 */
  match_chr =0; /*TH -Wall -O2 */
  if (tracing_macros > 0) {
	/* begin expansion of Show the text of the macro being expanded */
	/* module 401 */
	begin_diagnostic();
	print_ln();
	print_cs (warning_index);
	token_show (ref_count);
	end_diagnostic (false);
	/* end expansion of Show the text of the macro being expanded */
  };
  if (info (r) == protected_token)
    r = link (r);
  if (info (r) != end_match_token) {
	/* begin expansion of Scan the parameters and make |link(r)| point to the
	   macro body; but |return| if an illegal \.{\\par} is detected */
	/* module 391 */
	/* At this point, the reader will find it advisable to review the explanation
	 * of token list format that was presented earlier, since many aspects of that
	 * format are of importance chiefly in the |macro_call| routine.
	 * 
	 * The token list might begin with a string of compulsory tokens before the
	 * first |match| or |end_match|. In that case the macro name is supposed to be
	 * followed by those tokens; the following program will set |s=null| to
	 * represent this restriction. Otherwise |s| will be set to the first token of
	 * a string that will delimit the next parameter.
	 */
	scanner_status = matching;
	unbalance = 0;
	set_long_state(eq_type (cur_cs));
	if (get_long_state() >= outer_call)
	  set_long_state(get_long_state() - 2);
	do {
	  link (temp_head) = null;
	  if ((info (r) > match_token + 255) || (info (r) < match_token)) {
		s = null;
	  } else {
		match_chr = info (r) - match_token;
		s = link (r);
		r = s;
		p = temp_head;
		m = 0;
	  };
	  /* begin expansion of Scan a parameter until its delimiter string has been found; 
		 or, if |s=null|, simply scan the delimiter string */
	  /* module 392 */
	  /* If |info(r)| is a |match| or |end_match| command, it cannot be equal to
	   * any token found by |get_token|. Therefore an undelimited parameter---i.e.,
	   * a |match| that is immediately followed by |match| or |end_match|---will
	   * always fail the test `|cur_tok=info(r)|' in the following algorithm.
	   */
	CONTINUE:
	  get_token(); /* set |cur_tok| to the next token of input */
	  if (cur_tok == info (r)) {
		/* begin expansion of Advance \(r)|r|; |goto found| if the parameter delimiter
		   has been fully matched, otherwise |goto continue| */
		/* module 394 */
		/* A slightly subtle point arises here: When the parameter delimiter ends
		 * with `\.{\#\{}', the token list will have a left brace both before and
		 * after the |end_match|\kern-.4pt. Only one of these should affect the
		 * |align_state|, but both will be scanned, so we must make a correction.
		 */
		r = link (r);
		if ((info (r) >= match_token) && (info (r) <= end_match_token)) {
		  if (cur_tok < left_brace_limit)
		    decr (align_state);
		  goto FOUND;
		} else { 
		  do_something;
		  goto CONTINUE; 
		}
		/* end expansion of Advance \(r)|r|; |goto found| if the parameter delimiter has been ... */
	  };

	  /* begin expansion of Contribute the recently matched tokens to the current parameter, and 
		 |goto continue| if a partial match is still in effect; but abort if |s=null| */
	  /* module 397 */
	  /* When the following code becomes active, we have matched tokens from |s| to
	   * the predecessor of |r|, and we have found that |cur_tok<>info(r)|. An
	   * interesting situation now presents itself: If the parameter is to be
	   * delimited by a string such as `\.{ab}', and if we have scanned `\.{aa}',
	   * we want to contribute one `\.a' to the current parameter and resume
	   * looking for a `\.b'. The program must account for such partial matches and
	   * for others that can be quite complex. But most of the time we have |s=r|
	   * and nothing needs to be done.
	   * 
	   * Incidentally, it is possible for \.{\\par} tokens to sneak in to certain
	   * parameters of non-\.{\\long} macros. For example, consider a case like
	   * `\.{\\def\\a\#1\\par!\{...\}}' where the first \.{\\par} is not followed
	   * by an exclamation point. In such situations it does not seem appropriate
	   * to prohibit the \.{\\par}, so \TeX\ keeps quiet about this bending of
	   * the rules.
	   */
	  if (s != r) {
	    if (s == null) {
		  /* begin expansion of Report an improper use of the macro and abort */
		  /* module 398 */
		  print_err ("Use of ");
		  sprint_cs (warning_index);
		  zprint_string(" doesn't match its definition");
		  help4  ("If you say, e.g., `\\def\\a1{...}', then you must always",
				  "put `1' after `\\a', since control sequence names are",
				  "made up of letters only. The macro here has not been",
				  "followed by the required stuff, so I'm ignoring it.");
		  error();
		  return;
		  /* end expansion of Report an improper use of the macro and abort */
	    } else {
		  t = s;
		  do {
		    store_new_token (info (t));
		    incr (m);
		    u = link (t);
		    v = s;
		    loop {
		      if (u == r) {
				if (cur_tok != info (v)) {
				  goto DONE;
				} else {
				  r = link (v);
				  goto CONTINUE;
				}; 
			  }
		      if (info (u) != info (v))
				goto DONE;
		      u = link (u);
		      v = link (v);
		    };
		  DONE:
			t = link (t);
		  } while (t != r);
		  r = s; /* at this point, no tokens are recently matched */ 
		}
		/* end expansion of Contribute the recently matched tokens to the current.. */
	  };
	  if (cur_tok == par_token)
	    if (get_long_state() != long_call)
		  /* Report a runaway argument and abort */
		  report_a_runaway;
	  if (cur_tok < right_brace_limit) {
	    if (cur_tok < left_brace_limit) {
		  /* begin expansion of Contribute an entire group to the current parameter */
		  /* module 399 */
		  unbalance = 1;
		  loop {
			fast_store_new_token (cur_tok);
			get_token();
			if (cur_tok == par_token)
			  if (get_long_state() != long_call)
				/* Report a runaway argument and abort */
				report_a_runaway;
			if (cur_tok < right_brace_limit) {
			  if (cur_tok < left_brace_limit) {
				incr (unbalance);
		      } else {
				decr (unbalance);
				if (unbalance == 0)
				  goto DONE1;
		      };
			}
		  };
		DONE1:
		  rbrace_ptr = p;
		  store_new_token (cur_tok);
		  /* end expansion of Contribute an entire group to the current parameter */
		} else {
		  /* begin expansion of Report an extra right brace and |goto continue| */
		  /* module 395 */
		  back_input();
		  print_err ("Argument of ");
		  sprint_cs (warning_index);
		  zprint_string(" has an extra }");
		  help6 ("I've run across a `}' that doesn't seem to match anything.",
				 "For example, `\\def\\a#1{...}' and `\\a}' would produce",
				 "this error. If you simply proceed now, the `\\par' that",
				 "I've just inserted will cause me to report a runaway",
				 "argument that might be the root of the problem. But if",
				 "your `}' was spurious, just type `2' and it will go away.");
		  incr (align_state);
		  set_long_state(call);
		  cur_tok = par_token;
		  ins_error();
		  /* a white lie; the \.{\\par} won't always trigger a runaway */
		  /* end expansion of Report an extra right brace and |goto continue| */
		}
	  } else {
		/* begin expansion of Store the current token, but |goto continue| if it is a
		   blank space that would become an undelimited parameter */
		/* module 393 */
		if (cur_tok == space_token)
		  if (info (r) <= end_match_token)
			if (info (r) >= match_token)
			  goto CONTINUE;
		store_new_token (cur_tok);
		/* end expansion of Store the current token, but |goto continue| if it is a blank ..*/
	  };
	  incr (m);
	  if (info (r) > end_match_token)
	    goto CONTINUE;
	  if (info (r) < match_token)
	    goto CONTINUE;
	FOUND:
	  if (s != null) {
		/* begin expansion of Tidy up the parameter just scanned, and tuck it away */
		/* module 400 */
		/* If the parameter consists of a single group enclosed in braces, we must
		 * strip off the enclosing braces. That's why |rbrace_ptr| was introduced.
		 */
		if ((m == 1) && (info (p) < right_brace_limit) && (p != temp_head)) {
		  link (rbrace_ptr) = null;
		  free_avail (p);
		  p = link (temp_head);
		  pstack[n] = link (p);
		  free_avail (p);
		} else {
		  pstack[n] = link (temp_head);
		}
		incr (n);
		if (tracing_macros > 0) {
		  begin_diagnostic();
		  print_nl (match_chr);
		  print_int (n);
		  zprint_string("<-");
		  show_token_list (pstack[n - 1], null, 1000);
		  end_diagnostic (false);
		};
		/* end expansion of Tidy up the parameter just scanned, and tuck it away */
	  };
	  /* end expansion of Scan a parameter until its delimiter string has been found; ..*/
	  /* now |info(r)| is a token whose command code is either |match| or |end_match| */ 
	} while (info (r) != end_match_token);
	/* end expansion of Scan the parameters and make |link(r)| point to the macro body;... */
  };
  /* begin expansion of Feed the macro body and its parameters to the scanner */
  /* module 390 */
  /* Before we put a new token list on the input stack, it is wise to clean off
   * all token lists that have recently been depleted. Then a user macro that ends
   * with a call to itself will not require unbounded stack space.
   */
  while ((state == token_list) && (loc == null) && (token_type != v_template))
    end_token_list(); /* conserve stack space */ 
  begin_token_list (ref_count, macro);
  name = warning_index;
  loc = link (r);
  if (n > 0) {
	if (param_ptr + n > (unsigned)max_param_stack) {
	  max_param_stack = param_ptr + n;
	  if (max_param_stack > param_size)
	    overflow ("parameter stack size", param_size);
	};
	for (m = 0; m <= n - 1; m++)
	  param_stack[param_ptr + m] = pstack[m];
	param_ptr = param_ptr + n;
  };
  /* end expansion of Feed the macro body and its parameters to the scanner */
  scanner_status = save_scanner_status;
  warning_index = save_warning_index;
};

/* module 379 */

/* Sometimes the expansion looks too far ahead, so we want to insert
 * a harmless \.{\\relax} into the user's input.
 */
void
insert_relax (void) {
  cur_tok = cs_token_flag + cur_cs;
  back_input();
  cur_tok = cs_token_flag + frozen_relax;
  back_input();
  token_type = inserted;
};


void
expand (void) {
  halfword t; /* token that is being ``expanded after'' */ 
  pointer p, q, r; /* for list manipulation */ 
  unsigned int j; /* index into |buffer| */ 
  int cv_backup; /* to save the global quantity |cur_val| */ 
  small_number cvl_backup, radix_backup, co_backup; /* to save |cur_val_level|, etc. */ 
  pointer backup_backup; /* to save |link(backup_head)| */ 
  small_number save_scanner_status; /* temporary storage of |scanner_status| */ 
  cv_backup = cur_val;
  cvl_backup = cur_val_level;
  radix_backup = radix;
  co_backup = cur_order;
  backup_backup = link (backup_head);
 RESWITCH:
  if (cur_cmd < call) {
	/* begin expansion of Expand a nonmacro */
	/* module 367 */
	if (tracing_commands > 1)
	  show_cur_cmd_chr();
	switch (cur_cmd) {
	case top_bot_mark:
	  /* begin expansion of Insert the \(a)appropriate mark text into the scanner */
	  /* module 386 */
	  /* The following code is activated when |cur_cmd=top_bot_mark| and
	   * when |cur_chr| is a code like |top_mark_code|.
	   */
	  t = cur_chr % marks_code;
	  if (cur_chr >= marks_code) {
		scan_register_num();
	  } else {
		cur_val = 0;
	  }
	  if (cur_val == 0) {
		cur_ptr = cur_mark[t];
	  } else {
		/* begin expansion of Compute the mark pointer for mark type |t| and class |cur_val| */
		/* module 1769 */
		/* Here we compute the pointer to the current mark of type |t| and mark
		 * class |cur_val|.
		 */
		find_sa_element (mark_val, cur_val, false);
		if (cur_ptr != null) {
		  if (odd (t)) {
			cur_ptr = link (cur_ptr + (t / 2) + 1);
		  } else {
			cur_ptr = info (cur_ptr + (t / 2) + 1); 
		  }
		}
		/* end expansion of Compute the mark pointer for mark type |t| and class |cur_val| */
	  };
	  if (cur_ptr != null)
		begin_token_list (cur_ptr, mark_text);
	  /* end expansion of Insert the \(a)appropriate mark text into the scanner */
	  break;
	case expand_after:
	  if (cur_chr == 0) {
		/* begin expansion of Expand the token after the next token */
		/* module 368 */
		/* It takes only a little shuffling to do what \TeX\ calls \.{\\expandafter}. */
		get_token();
		t = cur_tok;
		get_token();
		if (cur_cmd > max_command) {
		  expand();
		} else {
		  back_input();
		}
		cur_tok = t;
		back_input();
		/* end expansion of Expand the token after the next token */
	  } else {
		/* begin expansion of Negate a boolean conditional and |goto reswitch| */
		/* module 1710 */
		/* The result of a boolean condition is reversed when the conditional is
		 * preceded by \.{\\unless}.
		 */
		get_token();
		if ((cur_cmd == if_test) && (cur_chr != if_case_code)) {
		  cur_chr = cur_chr + unless_code;
		  goto RESWITCH;
		};
		print_err ("You can't use `");
		print_esc_string (("unless"));
		zprint_string ("' before `");
		print_cmd_chr (cur_cmd, cur_chr);
		print_char ('\'');
		help1 ("Continue, and I'll forget that it ever happened.");
		back_error();
		/* end expansion of Negate a boolean conditional and |goto reswitch| */
	  };
	  break;
	case no_expand:
	  /* begin expansion of Suppress expansion of the next token */
	  /* module 369 */
	  /* The implementation of \.{\\noexpand} is a bit trickier, because it is
	   * necessary to insert a special `|dont_expand|' marker into \TeX's reading
	   * mechanism. This special marker is processed by |get_next|, but it does
	   * not slow down the inner loop.
	   * 
	   * Since \.{\\outer} macros might arise here, we must also
	   * clear the |scanner_status| temporarily.
	   */
	  save_scanner_status = scanner_status;
	  scanner_status = normal;
	  get_token();
	  scanner_status = save_scanner_status;
	  t = cur_tok;
	  back_input(); /* now |start| and |loc| point to the backed-up token |t| */
	  if (t >= cs_token_flag) {
		p = get_avail ();
		info (p) = cs_token_flag + frozen_dont_expand;
		link (p) = loc;
		start = p;
		loc = p;
	  };
	  /* end expansion of Suppress expansion of the next token */
	  break;
	case cs_name:
	  /* begin expansion of Manufacture a control sequence name */
	  /* module 372 */
	  r = get_avail ();
	  p = r; /* head of the list of characters */
	  do {
		get_x_token();
		if (cur_cs == 0)
		  store_new_token (cur_tok);
	  } while (cur_cs == 0);
	  if (cur_cmd != end_cs_name)
		/* Complain about missing \.{\\endcsname} */
        complain_missing_csname;
	  /* begin expansion of Look up the characters of list |r| in the hash table, and set |cur_cs| */
	  /* module 374 */
	  j = first;
	  p = link (r);
	  while (p != null) {
		if (j >= (unsigned)max_buf_stack) {
		  max_buf_stack = j + 1;
		  if (max_buf_stack == buf_size) 
			realloc_buffer();
		};
		buffer[j] = info (p) % 256;
		incr (j);
		p = link (p);
	  };
	  if (j > first + 1) {
		set_no_new_control_sequence(false);
		cur_cs = id_lookup (first, j - first);
		set_no_new_control_sequence(true);
	  } else if (j == first) {
		cur_cs = null_cs;	/* the list is empty */
	  } else {
		cur_cs = single_base + buffer[first];	/* the list has length one */
	  }
	  /* end expansion of Look up the characters of list |r| in the hash table, and set |cur_cs| */
	  flush_list (r);
	  if (eq_type (cur_cs) == undefined_cs) {
		eq_define (cur_cs, relax, 256); /* N.B.: The |save_stack| might change */ 
	  }; /* the control sequence will now match `\.{\\relax}' */
	  cur_tok = cur_cs + cs_token_flag;
	  back_input();
	  /* end expansion of Manufacture a control sequence name */
	  break;
	case convert:
	  conv_toks();
	  break;
	case the: /* this procedure is discussed in Part 27 below */ 
	  ins_the_toks();
	  break;
	case if_test: /* this procedure is discussed in Part 27 below */ 
	  conditional();
	  break;
	case fi_or_else: /* this procedure is discussed in Part 28 below */ 
	  /* Terminate the current conditional and skip to \.{\\fi} */
	  finish_conditional();
	  break;
	case input:
	  /* begin expansion of Initiate or terminate input from a file */
	  /* module 378 */
	  if (cur_chr == 1) {
		force_eof = true;
		/* begin expansion of Cases for |input| */
		/* module 1694 */
	  } else if (cur_chr == 2) {
		pseudo_start();
		/* end expansion of Cases for |input| */
	  } else if (name_in_progress) {
		insert_relax();
	  } else {
	    start_input();
	  };
	  /* end expansion of Initiate or terminate input from a file */
	  break;
	default:
	  /* begin expansion of Complain about an undefined macro */
	  /* module 370 */
	  print_err ("Undefined control sequence");
	  help5 ("The control sequence at the end of the top line",
			 "of your error message was never \\def'ed. If you have",
			 "misspelled it (e.g., `\\hobx'), type `I' and the correct",
			 "spelling (e.g., `I\\hbox'). Otherwise just continue,",
			 "and I'll forget about whatever was undefined.");
	  error();
	  /* end expansion of Complain about an undefined macro */
    };
	/* end expansion of Expand a nonmacro */
  } else if (cur_cmd < end_template) {
	macro_call();
  } else {
	/* begin expansion of Insert a token containing |frozen_endv| */
	/* module 375 */
	/* An |end_template| command is effectively changed to an |endv| command
	 * by the following code. (The reason for this is discussed below; the
	 * |frozen_end_template| at the end of the template has passed the
	 * |check_outer_validity| test, so its mission of error detection has been
	 * accomplished.)
	 */
	cur_tok = cs_token_flag + frozen_endv;
	back_input();
	/* end expansion of Insert a token containing |frozen_endv| */
  };
  cur_val = cv_backup;
  cur_val_level = cvl_backup;
  radix = radix_backup;
  cur_order = co_backup;
  link (backup_head) = backup_backup;
};


