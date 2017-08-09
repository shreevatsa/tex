
#include "types.h"
#include "c-compat.h"


#include "globals.h"


#define add_delims_to( arg )    \
          arg  +  math_shift:   \
     case arg  +  tab_mark:     \
     case arg  +  mac_param:    \
     case arg  +  sub_mark:     \
     case arg  +  letter:       \
     case arg  +  other_char

#define any_state_plus(arg)    \
            mid_line  +  arg:  \
    case skip_blanks  +  arg:  \
       case new_line  +  arg


/* module 352 */
#define is_hex(arg) (((arg>='0') && (arg<='9')) || ((arg>='a') && (arg<='f')))

#define hex_to_cur_chr                                    \
  if (c <= '9')   { cur_chr = c - '0';}                   \
  else { cur_chr = c - 'a' + 10; };                       \
  if (cc <= '9')  { cur_chr = 16 * cur_chr + cc - '0'; }  \
  else { cur_chr = 16 * cur_chr + cc - 'a' + 10; }

/* module 355 */

/* Whenever we reach the following piece of code, we will have
 * |cur_chr=buffer[k-1]| and |k<=limit+1| and |cat=cat_code(cur_chr)|. If an
 * expanded code like \.{\^\^A} or \.{\^\^df} appears in |buffer[(k-1)..(k+1)]|
 * or |buffer[(k-1)..(k+2)]|, we
 * will store the corresponding code in |buffer[k-1]| and shift the rest of
 * the buffer left two or three places.
 */
/* If an expanded code is present, reduce it and |goto start_cs| */
#define if_an_expanded {                             \
  if (buffer[k] == cur_chr)                          \
    if (cat == sup_mark) if (k < limit) {            \
 	 c = buffer[k + 1];                              \
	 if (c < 128) { /* yes, one is indeed present */ \
	   d = 2;                                        \
	   if (is_hex (c))                               \
	     if (k + 2 <= limit) {                       \
	       cc = buffer[k + 2];                       \
	       if (is_hex (cc))                          \
	         incr (d);                               \
	   };                                            \
	   if (d > 2) {                                  \
	     hex_to_cur_chr;                             \
	     buffer[k - 1] = cur_chr;                    \
	   } else if (c < 64){                           \
         buffer[k - 1] = c + 64;                     \
	   } else {                                      \
	     buffer[k - 1] = c - 64;                     \
	   }                                             \
	   limit = limit - d;                            \
	   first = first - d;                            \
	   while (k <= limit) {                          \
	     buffer[k] = buffer[k + d];                  \
	     incr (k);                                   \
	   };                                            \
	   goto START_CS;                                \
    };                                               \
  };                                                 \
 }

/* module 333 */

/* The value of |par_loc| is the |eqtb| address of `\.{\\par}'. This quantity
 * is needed because a blank line of input is supposed to be exactly equivalent
 * to the appearance of \.{\\par}; we must set |cur_cs:=par_loc|
 * when detecting a blank line.
 */

pointer par_loc; /* location of `\.{\\par}' in |eqtb| */
halfword par_token; /* token representing `\.{\\par}' */

/* module 361 */

/* The global variable |force_eof| is normally |false|; it is set |true|
 * by an \.{\\endinput} command.
 */
boolean force_eof; /* should the next \.{\\input} be aborted early? */

/* module 336 */

/* Before getting into |get_next|, let's consider the subroutine that
 * is called when an `\.{\\outer}' control sequence has been scanned or
 * when the end of a file has been reached. These two cases are distinguished
 * by |cur_cs|, which is zero at the end of a file.
 */
void
check_outer_validity (void) {
  pointer p;			/* points to inserted token list */
  pointer q;			/* auxiliary pointer */
  if (scanner_status != normal) {
	deletions_allowed = false;
	/* begin expansion of Back up an outer control sequence so that it can be reread */
	/* module 337 */
	/* An outer control sequence that occurs in a \.{\\read} will not be reread,
	 * since the error recovery for \.{\\read} is not very powerful.
	 */
	if (cur_cs != 0) {
	  if ((state == token_list) || (name < 1) || (name > 17)) {
		p = get_avail ();
		info (p) = cs_token_flag + cur_cs;
		back_list (p);
		/* prepare to read the control sequence again */ 
	  };
	  cur_cmd = spacer;
	  cur_chr = ' '; /* replace it by a space */
	};
	/* end expansion of Back up an outer control sequence so that it can be reread */
	if (scanner_status > skipping) {
	  /* begin expansion of Tell the user what has run away and try to recover */
	  /* module 338 */
	  runaway();
	  /* print a definition, argument, or preamble */
	  if (cur_cs == 0) {
		print_err ("File ended");
	  } else {
		cur_cs = 0;
		print_err ("Forbidden control sequence found");
	  };
	  /* begin expansion of Print either `\.{definition}' or `\.{use}' or `\.{preamble}' 
	     or `\.{text}', and insert tokens that should lead to recovery */
	  /* module 339 */
	  /* The recovery procedure can't be fully understood without knowing more
	   * about the \TeX\ routines that should be aborted, but we can sketch the
	   * ideas here: For a runaway definition we will insert a right brace; for a
	   * runaway preamble, we will insert a special \.{\\cr} token and a right
	   * brace; and for a runaway argument, we will set |long_state| to
	   * |outer_call| and insert \.{\\par}.
	   */
	  p = get_avail ();
	  switch (scanner_status) {
	  case defining:
		zprint_string(" while scanning definition");
		info (p) = right_brace_token + '}';
		break;
	  case matching:
		zprint_string(" while scanning use");
		info (p) = par_token;
		set_long_state(outer_call);
		break;
	  case aligning:
		zprint_string(" while scanning preamble");
		info (p) = right_brace_token + '}';
		q = p;
		p = get_avail ();
		link (p) = q;
		info (p) = cs_token_flag + frozen_cr;
		align_state = -1000000;
		break;
	  case absorbing:
		zprint_string(" while scanning text");
		info (p) = right_brace_token + '}';
	  }; /* there are no other cases */
	  ins_list (p);
	  /* end expansion of Print either `\.{definition}' or `\.{use}' or `\.{preamble}' ...*/
	  zprint_string(" of ");
	  sprint_cs (warning_index);
	  help4 ("I suspect you have forgotten a `}', causing me",
	         "to read past where you wanted me to stop.",
             "I'll try to recover; but if the error is serious,",
             "you'd better type `E' or `X' now and fix your file.");
	  error();
	  /* end expansion of Tell the user what has run away and try to recover */
	} else  {
	  print_err ("Incomplete ");
	  print_cmd_chr (if_test, cur_if);
	  zprint_string("; all text was ignored after line ");
	  print_int (skip_line);
	  help3  ("A forbidden control sequence occurred in skipped text.",
			  "This kind of error happens when you say `\\if...' and forget",
			  "the matching `\\fi'. I've inserted a `\\fi'; this might work.");
	  if (cur_cs != 0) {
		cur_cs = 0;
	  } else {
		help_line[2] = "The file ended while I was skipping conditional text.";
		cur_tok = cs_token_flag + frozen_fi;
		ins_error();
	  };
	  deletions_allowed = true;
	};
  };
}



/* module 331 */

/* To get \TeX's whole input mechanism going, we perform the following actions.
 */

/* module 332 */

/* 
 * The heart of \TeX's input mechanism is the |get_next| procedure, which
 * we shall develop in the next few sections of the program. Perhaps we
 * shouldn't actually call it the ``heart,'' however, because it really acts
 * as \TeX's eyes and mouth, reading the source files and gobbling them up.
 * And it also helps \TeX\ to regurgitate stored token lists that are to be
 * processed again.
 * 
 * The main duty of |get_next| is to input one token and to set |cur_cmd|
 * and |cur_chr| to that token's command code and modifier. Furthermore, if
 * the input token is a control sequence, the |eqtb| location of that control
 * sequence is stored in |cur_cs|; otherwise |cur_cs| is set to zero.
 * 
 * Underlying this simple description is a certain amount of complexity
 * because of all the cases that need to be handled.
 * However, the inner loop of |get_next| is reasonably short and fast.
 * 
 * When |get_next| is asked to get the next token of a \.{\\read} line,
 * it sets |cur_cmd=cur_chr=cur_cs=0| in the case that no more tokens
 * appear on that line. (There might not be any tokens at all, if the
 * |end_line_char| has |ignore| as its catcode.)
 */


/* module 363 */

/* If the user has set the |pausing| parameter to some positive value,
 * and if nonstop mode has not been selected, each line of input is displayed
 * on the terminal and the transcript file, followed by `\.{=>}'.
 * \TeX\ waits for a response. If the response is simply |carriage_return|, the
 * line is accepted as it stands, otherwise the line typed is
 * used instead of the line in the file.
 */
void
firm_up_the_line (void) {
  integer k;		/* an index into |buffer| */
  limit = last;
  if (pausing > 0) {
    if (interaction > nonstop_mode) {
      wake_up_terminal;
      print_ln();
      if (start < limit)
		for (k = start; k <= limit - 1; k++)
		  zprint (buffer[k]);
      first = limit;
      prompt_input ("=>"); /* wait for user response */ 
      if (last > first) {
		for (k = first; k <= (integer)last - 1; k++)
		  /* move line down in buffer */
		  buffer[k + start - first] = buffer[k];
		limit = start + last - first;
      };
    };
  };
};


signed char
input_from_external_file (void) {
  unsigned char Restart;
  integer k;		/* an index into |buffer| */
  unsigned cat;		/* |cat_code(cur_chr)|, usually */
  ASCII_code c, cc;		/* constituents of a possible expanded code */
  signed char d;		/* number of excess characters in an expanded code */
  cc = 0; /*TH -Wall -O2 */
  Restart = 0;
  /* module 343 */
 SWITCH:
  if (loc <= limit) { /* current line not yet finished */
    cur_chr = buffer[loc];
    incr (loc);
  RESWITCH:
    cur_cmd = cat_code (cur_chr);
    /* begin expansion of Change state if necessary, and |goto switch| if the current 
       character should be ignored, or |goto reswitch| if the current character 
       changes to another */
    /* module 344 */
    /* The following 48-way switch accomplishes the scanning quickly, assuming
     * that a decent \PASCAL\ compiler has translated the code. Note that the numeric
     * values for |mid_line|, |skip_blanks|, and |new_line| are spaced
     * apart from each other by |max_char_code+1|, so we can add a character's
     * command code to the state to get a single number that characterizes both.
     */
    switch (state + cur_cmd) {
    case any_state_plus (ignore):
    case skip_blanks + spacer:
    case new_line + spacer:
      do_something;
      goto SWITCH;
    case any_state_plus (escape):
      /* begin expansion of Scan a control sequence and 
	 set |state:=skip_blanks| or |mid_line| */
      /* module 354 */
      /* Control sequence names are scanned only when they appear in some line of
       * a file; once they have been scanned the first time, their |eqtb| location
       * serves as a unique identification, so \TeX\ doesn't need to refer to the
       * original name any more except when it prints the equivalent in symbolic form.
       * 
       * The program that scans a control sequence has been written carefully
       * in order to avoid the blowups that might otherwise occur if a malicious
       * user tried something like `\.{\\catcode\'15=0}'. The algorithm might
       * look at |buffer[limit+1]|, but it never looks at |buffer[limit+2]|.
       * 
       * If expanded characters like `\.{\^\^A}' or `\.{\^\^df}'
       * appear in or just following
       * a control sequence name, they are converted to single characters in the
       * buffer and the process is repeated, slowly but surely.
       */
      if (loc > limit) {
	cur_cs = null_cs;	/* |state| is irrelevant in this case */
      } else {
      START_CS:
	k = loc;
	cur_chr = buffer[k];
	cat = cat_code (cur_chr);
	incr (k);
	if (cat == letter) {
	  state = skip_blanks;
	} else if (cat == spacer) {
	  state = skip_blanks;
	} else {
	  state = mid_line;
	};
	if ((cat == letter) && (k <= limit)) {
	  /* begin expansion of Scan ahead in the buffer until finding a nonletter; 
	     if an expanded code is encountered, reduce it and |goto start_cs|; 
	     otherwise if a multiletter control
	     sequence is found, adjust |cur_cs| and |loc|, and |goto found| */
	  /* module 356 */
	  do {
	    cur_chr = buffer[k];
	    cat = cat_code (cur_chr);
	    incr (k);
	  } while (!((cat != letter) || (k > limit)));
	  /* If an expanded... */
	  if_an_expanded;
	  if (cat != letter)
	    decr (k);	/* now |k| points to first nonletter */
	  if (k > loc + 1) {
	    /* multiletter control sequence has been scanned */
	    cur_cs = id_lookup (loc, k - loc);
	    loc = k;
	    goto FOUND;
	  };
	  /* end expansion of Scan ahead in the buffer until finding  ... */
	} else {
	  /* If an expanded... */
	  if_an_expanded;
	}
	cur_cs = single_base + buffer[loc];
	incr (loc);
      };
    FOUND:
      cur_cmd = eq_type (cur_cs);
      cur_chr = equiv (cur_cs);
      if (cur_cmd >= outer_call)
	check_outer_validity();
      /* end expansion of Scan a control sequence and set |state:=skip_blanks| or |mid_line| */
      break;
    case any_state_plus (active_char):
      /* begin expansion of Process an active-character control sequence 
	 and set |state:=mid_line| */
      /* module 353 */
      cur_cs = cur_chr + active_base;
      cur_cmd = eq_type (cur_cs);
      cur_chr = equiv (cur_cs);
      state = mid_line;
      if (cur_cmd >= outer_call)
	check_outer_validity();
      /* end expansion of Process an active-character control sequence ... */
      break;
    case any_state_plus (sup_mark):
      /* begin expansion of If this |sup_mark| starts an expanded character like~\.{\^\^A}
	 or~\.{\^\^df}, then |goto reswitch|, otherwise set |state:=mid_line| */
      /* module 352 */
      /* Notice that a code like \.{\^\^8} becomes \.x if not followed by a hex digit. */
      if (cur_chr == buffer[loc])
	if (loc < limit) {
	  c = buffer[loc + 1];
	  if (c < 128) {	/* yes we have an expanded char */
	    loc = loc + 2;
	    if (is_hex (c))
	      if (loc <= limit) {
		cc = buffer[loc];
		if (is_hex (cc)) {
		  incr (loc);
		  hex_to_cur_chr;
		  goto RESWITCH;
		};
	      };
	    if (c < 64) {
	      cur_chr = c + 64;
	    } else {
	      cur_chr = c - 64;
	    };
	    goto RESWITCH;
	  };
	};
      state = mid_line;
      /* end expansion of If this |sup_mark| starts an expanded character like~\.{\^\^A} ...*/ 
      break;
    case any_state_plus (invalid_char):
      /* begin expansion of Decry the invalid character and |goto restart| */
      /* module 346 */
      /* We go to |restart| instead of to |switch|, because |state| might equal
       * |token_list| after the error has been dealt with
       * (cf.\ |clear_for_error_prompt|). 
       */
      print_err ("Text line contains an invalid character");
      help2 ("A funny symbol that I can't read has just been input.",
	     "Continue, and I'll forget that it ever happened.");
      deletions_allowed = false;
      error();
      deletions_allowed = true;
      Restart = 1;
      return Restart;
      /* end expansion of Decry the invalid character and |goto restart| */
      break;
      /* begin expansion of Handle situations involving spaces, braces, changes of state */
      /* module 347 */
    case mid_line + spacer:
      /* begin expansion of Enter |skip_blanks| state, emit a space */
      /* module 349 */
      /* The following code is performed only when |cur_cmd=spacer|. */
      state = skip_blanks;
      cur_chr = ' ';
      /* end expansion of Enter |skip_blanks| state, emit a space */
      break;
    case mid_line + car_ret:
      /* begin expansion of Finish line, emit a space */
      /* module 348 */
      /* When a character of type |spacer| gets through, its character code is
       * changed to $\.{"\ "}= oct(40)$. This means that the ASCII codes for tab and space,
       * and for the space inserted at the end of a line, will
       * be treated alike when macro parameters are being matched. We do this
       * since such characters are indistinguishable on most computer terminal displays.
       */
      loc = limit + 1;
      cur_cmd = spacer;
      cur_chr = ' ';
      /* end expansion of Finish line, emit a space */
      break;
    case skip_blanks + car_ret:
    case any_state_plus (comment):
      /* begin expansion of Finish line, |goto switch| */
      /* module 350 */
      loc = limit + 1;
      goto SWITCH;
      /* end expansion of Finish line, |goto switch| */
    case new_line + car_ret:
      /* begin expansion of Finish line, emit a \.{\\par} */
      /* module 351 */
      loc = limit + 1;
      cur_cs = par_loc;
      cur_cmd = eq_type (cur_cs);
      cur_chr = equiv (cur_cs);
      if (cur_cmd >= outer_call)
	check_outer_validity();
      /* end expansion of Finish line, emit a \.{\\par} */
      break;
    case mid_line + left_brace:
      incr (align_state);
      break;
    case skip_blanks + left_brace:
    case new_line + left_brace:
      state = mid_line;
      incr (align_state);
      break;
    case mid_line + right_brace:
      decr (align_state);
      break;
    case skip_blanks + right_brace:
    case new_line + right_brace:
      state = mid_line;
      decr (align_state);
      break;
    case add_delims_to (skip_blanks):
    case add_delims_to (new_line):
      state = mid_line;
      break;
    default:
      do_nothing;
    };
    /* end expansion of Change state if necessary, and |goto switch| if the current ... */
  } else {
    state = new_line;
    /* begin expansion of Move to next line of file, or |goto restart| if there is no next line,
     * or |return| if a \.{\\read} line has finished */
    /* module 360 */
    /* All of the easy branches of |get_next| have now been taken care of.
     * There is one more branch.
     */
    if (name > 17) {
      /* begin expansion of Read next line of file into |buffer|, or 
	 |goto restart| if the file has ended */
      /* module 362 */
      incr (line);
      first = start;
      if (!force_eof) {
	if (name <= 19) {
	  if (pseudo_input()) {	/* not end of file */
	    firm_up_the_line();	/* this sets |limit| */
	  } else if ((every_eof != null) && !eof_seen[index]) {
	    limit = first - 1;
	    eof_seen[index] = true;	/* fake one empty line */
	    begin_token_list (every_eof, every_eof_text);
	    Restart = 1;
	    return Restart;
	  } else {
	    force_eof = true;
	  };
	} else {
	  if (input_next_line()) {		/* not end of file */
	    firm_up_the_line();	/* this sets |limit| */
	  } else if ((every_eof != null) && !eof_seen[index]) {
	    limit = first - 1;
	    eof_seen[index] = true; /* fake one empty line */
	    begin_token_list (every_eof, every_eof_text);
	    Restart = 1;
	    return Restart;
	  } else {
	    force_eof = true;
	  };
	};
      };
      if (force_eof) {
	if (tracing_nesting > 0)
	  if ((grp_stack[in_open] != cur_boundary) || (if_stack[in_open] != cond_ptr))
	    file_warning();
	/* give warning for some unfinished groups and/or conditionals */
	if (name >= 19) {
	  print_char (')');
	  decr (open_parens);
	  update_terminal; /* show user that file has been read */
	};
	force_eof = false;
	end_file_reading();
	/* resume previous level */
	check_outer_validity();
	Restart = 1;
	return Restart;
      };
      if (end_line_char_inactive) {
	decr (limit);
      } else {
	buffer[limit] = end_line_char;
      };
      first = limit + 1;
      loc = start; /* ready to read */
      /* end expansion of Read next line of file into |buffer|, or ...*/
    } else {
      if (!terminal_input) {	/* \.{\\read} line has ended */
	cur_cmd = 0;
	cur_chr = 0;
	Restart = -1;
	return Restart;
      };
      if (input_ptr > 0) { /* text was inserted during error recovery */
	end_file_reading();
	Restart = 1;
	return Restart; /* resume previous level */
      };
      if (selector < log_only)
	open_log_file();
      if (interaction > nonstop_mode) {
	if (end_line_char_inactive)
	  incr (limit);
	if (limit == start)	/* previous line was empty */
	  print_nl_string("(Please type a command or say `\\end')");
	print_ln();
	first = start;
	prompt_input ("*"); /* input on-line into |buffer| */
	limit = last;
	if (end_line_char_inactive) {
	  decr (limit);
	} else {
	  buffer[limit] = end_line_char;
	};
	first = limit + 1;
	loc = start;
      } else {
	fatal_error ("*** (job aborted, no legal \\end found)");
	/* nonstop mode, which is intended for overnight batch processing, 
	   never waits for on-line input */
      };
    };
    /* end expansion of Move to next line of file, or |goto restart| if there is no next  .. */
    check_interrupt;
    goto SWITCH;
  };
  return Restart;
}


integer state_extfile = 0;
integer state_toklist_exhaust = 0;
integer state_toklist_call = 0;
integer state_toklist_csname = 0;
integer state_toklist_normal = 0;


/* module 341 */

/* Now we're ready to take the plunge into |get_next| itself. Parts of
 * this routine are executed more often than any other instructions of \TeX.
 */
void
get_next (void) {
  /* sets |cur_cmd|, |cur_chr|, |cur_cs| to next token */
   halfword t;			/* a token */
  signed char Restart;
 RESTART:
  cur_cs = 0;
  if (state != token_list) {
    /* Input from external file, |goto restart| if no input found */
	  //	  state_extfile++;
	Restart = input_from_external_file();
    if(Restart == 1) {
      goto RESTART;
    } else if (Restart <0) {
      return ;
    } 
  } else {
    /* begin expansion of Input from token list, |goto restart| if end of list or
       if a parameter needs to be expanded */
    /* module 357 */
    /* Let's consider now what happens when |get_next| is looking at a token list. */
    if (loc != null) { /* list not exhausted */
      t = info (loc);
      loc = link (loc); /* move to next */
      if (t >= cs_token_flag) { /* a control sequence token */
		cur_cs = t - cs_token_flag;
		cur_cmd = eq_type (cur_cs);
		cur_chr = equiv (cur_cs);
		if (cur_cmd >= outer_call) {
		  //  state_toklist_call++;
		  if (cur_cmd == dont_expand) {
			/* begin expansion of Get the next token, suppressing expansion */
			/* module 358 */
			/* The present point in the program is reached only when the |expand|
			 * routine has inserted a special marker into the input. In this special
			 * case, |info(loc)| is known to be a control sequence token, and |link(loc)=null|.
			 */
			cur_cs = info (loc) - cs_token_flag;
			loc = null;
			cur_cmd = eq_type (cur_cs);
			cur_chr = equiv (cur_cs);
			if (cur_cmd > max_command) {
			  cur_cmd = relax;
			  cur_chr = no_expand_flag;
			};
			/* end expansion of Get the next token, suppressing expansion */
		  } else {
			check_outer_validity();
		  };
		} else {
		  state_toklist_csname++;
		}
      } else {
		state_toklist_normal++;
		cur_cmd = t / 256;
		cur_chr = t % 256;
		/*TH if() statements are somewhat faster than switch() */
		if (cur_cmd==left_brace) {
		  align_state++;
		} else if (cur_cmd==right_brace) {
		  align_state--;
		} else if (cur_cmd==out_param) {
		  /* begin expansion of Insert macro parameter and |goto restart| */
		  /* module 359 */
		  begin_token_list (param_stack[param_start + cur_chr - 1],parameter);
		  goto RESTART;
	  /* end expansion of Insert macro parameter and |goto restart| */
		};
      };
    } else { /* we are done with this token list */
	  //state_toklist_exhaust++;
      end_token_list();
      goto RESTART; /* resume previous level */
    };
    /* end expansion of Input from token list, |goto restart| if end .. */
  };
  /* begin expansion of If an alignment entry has just ended, take appropriate action */
  /* module 342 */
  /* An alignment entry ends when a tab or \.{\\cr} occurs, provided that the
   * current level of braces is the same as the level that was present at the
   * beginning of that alignment entry; i.e., provided that |align_state| has
   * returned to the value it had after the \<u_j> template for that entry.
   */
  if (align_state == 0 && cur_cmd <= car_ret && cur_cmd >= tab_mark) {
    /* Insert the \(v)\<v_j> template and |goto restart| */
    insert_v_part();
    goto RESTART;
  };
  /* end expansion of If an alignment entry has just ended, take appropriate action */
};

/* module 364 */

/* Since |get_next| is used so frequently in \TeX, it is convenient
 * to define three related procedures that do a little more:
 * 
 * \yskip\hang|get_token| not only sets |cur_cmd| and |cur_chr|, it
 * also sets |cur_tok|, a packed halfword version of the current token.
 * 
 * \yskip\hang|get_x_token|, meaning ``get an expanded token,'' is like
 * |get_token|, but if the current token turns out to be a user-defined
 * control sequence (i.e., a macro call), or a conditional,
 * or something like \.{\\topmark} or \.{\\expandafter} or \.{\\csname},
 * it is eliminated from the input by beginning the expansion of the macro
 * or the evaluation of the conditional.
 * 
 * \yskip\hang|x_token| is like |get_x_token| except that it assumes that
 * |get_next| has already been called.
 * 
 * \yskip\noindent
 * In fact, these three procedures account for {\sl all\/} uses of |get_next|,
 * except for two places in the ``inner loop'' when |cur_tok| need not be set,
 * and except when the arguments to \.{\\ifx} are being scanned.
 */


/* module 365 */

/* No new control sequences will be defined except during a call of
 * |get_token|, or when \.{\\csname} compresses a token list, because
 * |no_new_control_sequence| is always |true| at other times.
 */
void
get_token (void) {	/* sets |cur_cmd|, |cur_chr|, |cur_tok| */
  set_no_new_control_sequence(false);
  get_next();
  set_no_new_control_sequence(true);
  if (cur_cs == 0) {
	cur_tok = (cur_cmd * 256) + cur_chr;
  } else {
	cur_tok = cs_token_flag + cur_cs;
  };
};

/* module 380 */

/* Here is a recursive procedure that is \TeX's usual way to get the
 * next token of input. It has been slightly optimized to take account of
 * common cases.
 */
void
get_x_token (void) { /* sets |cur_cmd|, |cur_chr|, |cur_tok|, and expands macros */
 RESTART:
  get_next();
  if (cur_cmd <= max_command)
    goto DONE;
  if (cur_cmd >= call) {
    if (cur_cmd < end_template) {
	  macro_call();
	} else {
	  cur_cs = frozen_endv;
	  cur_cmd = endv;
	  goto DONE;  /* |cur_chr=null_list| */ 
	}  
  } else {
    expand();
  }
  goto RESTART;
 DONE:
  if (cur_cs == 0) {
	cur_tok = (cur_cmd * 256) + cur_chr;
  } else {
    cur_tok = cs_token_flag + cur_cs;
  }
};


/* module 381 */

/* The |get_x_token| procedure is equivalent to two consecutive
 * procedure calls: |get_next(); x_token|.
 */
void
x_token (void) { /* |get_x_token| without the initial |get_next| */
  while (cur_cmd > max_command) {
	expand();
	get_next();
  };
  if (cur_cs == 0) {
	cur_tok = (cur_cmd * 256) + cur_chr;
  } else {
    cur_tok = cs_token_flag + cur_cs;
  }
};

/* module 1717 */

/* The |get_x_or_protected| procedure is like |get_x_token| except that
 * protected macros are not expanded.
 */

void
get_x_or_protected (void) {	
  /* sets |cur_cmd|, |cur_chr|, |cur_tok|, and expands non-protected macros */
 loop {
    get_token();
    if (cur_cmd <= max_command)
      return;
    if ((cur_cmd >= call) && (cur_cmd < end_template))
      if (info (link (cur_chr)) == protected_token)
		return;
    expand();
  };
};


