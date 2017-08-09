
#include "types.h"
#include "c-compat.h"
#include "globals.h"

/* module 291 */

/* A token list is a singly linked list of one-word nodes in |mem|, where
 * each word contains a token and a link. Macro definitions, output-routine
 * definitions, marks, \.{\\write} texts, and a few other things
 * are remembered by \TeX\ in the form
 * of token lists, usually preceded by a node with a reference count in its
 * |token_ref_count| field. The token stored in location |p| is called
 * |info(p)|.
 * 
 * Three special commands appear in the token lists of macro definitions.
 * When |m=match|, it means that \TeX\ should scan a parameter
 * for the current macro; when |m=end_match|, it means that parameter
 * matching should end and \TeX\ should start reading the macro text; and
 * when |m=out_param|, it means that \TeX\ should insert parameter
 * number |c| into the text at this point.
 * 
 * The enclosing \.{\char'173} and \.{\char'175} characters of a macro
 * definition are omitted, but the final right brace of an output routine
 * is included at the end of its token list.
 * 
 * Here is an example macro definition that illustrates these conventions.
 * After \TeX\ processes the text
 * $$\.{\\def\\mac a\#1\#2 \\b \{\#1\\-a \#\#1\#2 \#2\}}$$
 * the definition of \.{\\mac} is represented as a token list containing
 * $$\def\,{\hskip2pt}
 * \vbox{\halign{\hfil#\hfil\cr
 * (reference count), |letter|\,\.a, |match|\,\#, |match|\,\#, |spacer|\,\.\ ,
 * \.{\\b}, |end_match|,\cr
 * |out_param|\,1, \.{\\-}, |letter|\,\.a, |spacer|\,\.\ , |mac_param|\,\#,
 * |other_char|\,\.1,\cr
 * |out_param|\,2, |spacer|\,\.\ , |out_param|\,2.\cr}}$$
 * The procedure |scan_toks| builds such token lists, and |macro_call|
 * does the parameter matching.
 * 
 * Examples such as
 * $$\.{\\def\\m\{\\def\\m\{a\}\ b\}}$$
 * explain why reference counts would be needed even if \TeX\ had no \.{\\let}
 * operation: When the token list for \.{\\m} is being read, the redefinition of
 * \.{\\m} changes the |eqtb| entry before the token list has been fully
 * consumed, so we dare not simply destroy a token list when its
 * control sequence is being redefined.
 * 
 * If the parameter-matching part of a definition ends with `\.{\#\{}',
 * the corresponding token list will have `\.\{' just before the `|end_match|'
 * and also at the very end. The first `\.\{' is used to delimit the parameter; the
 * second one keeps the first from disappearing.
 */

/* module 292 */

/* The procedure |show_token_list|, which prints a symbolic form of
 * the token list that starts at a given node |p|, illustrates these
 * conventions. The token list being displayed should not begin with a reference
 * count. However, the procedure is intended to be robust, so that if the
 * memory links are awry or if |p| is not really a pointer to a token list,
 * nothing catastrophic will happen.
 * 
 * An additional parameter |q| is also given; this parameter is either null
 * or it points to a node in the token list where a certain magic computation
 * takes place that will be explained later. (Basically, |q| is non-null when
 * we are printing the two-line context information at the time of an error
 * message; |q| marks the place corresponding to where the second line
 * should begin.)
 * 
 * For example, if |p| points to the node containing the first \.a in the
 * token list above, then |show_token_list| will zprint the string
 * $$\hbox{`\.{a\#1\#2\ \\b\ ->\#1\\-a\ \#\#1\#2\ \#2}';}$$
 * and if |q| points to the node containing the second \.a,
 * the magic computation will be performed just before the second \.a is printed.
 * 
 * The generation will stop, and `\.{\\ETC.}' will be printed, if the length
 * of printing exceeds a given limit~|l|. Anomalous entries are printed in the
 * form of control sequences that are not followed by a blank space, e.g.,
 * `\.{\\BAD.}'; this cannot be confused with actual control sequences because
 * a real control sequence named \.{BAD} would come out `\.{\\BAD\ }'.
 */

void
show_token_list (int p, int q, int l) {
  int m, c;			/* pieces of a token */
  ASCII_code match_chr;		/* character used in a `|match|' */
  ASCII_code n;			/* the highest parameter number, as an ASCII digit */
  match_chr = '#';
  n = '0';
  tally = 0;
  while ((p != null) && (tally < l)) {
      if (p == q)
		/* begin expansion of Do magic computation */
		/* module 320 */
		/* Here is the missing piece of |show_token_list| that is activated when the
		 * token beginning line~2 is about to be shown:
		 */
		set_trick_count;
      /* end expansion of Do magic computation */
      /* begin expansion of Display token |p|, and |return| if there are problems */
      /* module 293 */
      if ((p < hi_mem_min) || (p > mem_end)) {
		print_esc_string ("CLOBBERED.");
		return;
	  };
      if (info (p) >= cs_token_flag) {
		print_cs (info (p) - cs_token_flag);
	  } else {
		m = info (p) / 256;
		c = info (p) % 256;
		if (info (p) < 0) {
	      print_esc_string ("BAD.");
	    } else {
	      /* begin expansion of Display the token $(|m|,|c|)$ */
		  /* module 294 */
		  /* The procedure usually ``learns'' the character code used for macro
		   * parameters by seeing one in a |match| command before it runs into any
		   * |out_param| commands.
		   */
	      switch (m) {
		  case left_brace:
		  case right_brace:
		  case math_shift:
		  case tab_mark:
		  case sup_mark:
		  case sub_mark:
		  case spacer:
		  case letter:
		  case other_char:
			zprint (c);
			break;
		  case mac_param:
		    zprint (c);
		    zprint (c);
			break;
		  case out_param:
			zprint (match_chr);
			if (c <= 9) {
			print_char (c + '0');
			} else {
			  print_char ('!');
			  return;
			};
			do_something;
			break;
		  case match:
			match_chr = c;
			zprint (c);
			incr (n);
			print_char (n);
			if (n > '9') {
			  return;
			}
			do_something;
			break;
		  case end_match:
			if (c == 0)
			  zprint_string ("->");
			break;
		  default:
			print_esc_string ("BAD.");
		  }
	      /* end expansion of Display the token $(|m|,|c|)$ */
	    };
	  };
      /* end expansion of Display token |p|, and |return| if there are problems */
      p = link (p);
  };
  if (p != null)
    print_esc_string ("ETC.");
};


/* module 295 */

/* Here's the way we sometimes want to display a token list, given a pointer
 * to its reference count; the pointer may be null.
 */

void
token_show (pointer p) {
  if (p != null)
    show_token_list (link (p), null, 10000000);
};


/* module 296 */

/* The |print_meaning| subroutine displays |cur_cmd| and |cur_chr| in
 * symbolic form, including the expansion of a macro or mark.
 */
void
print_meaning (void) {
  print_cmd_chr (cur_cmd, cur_chr);
  if (cur_cmd >= call) {
	print_char (':');
	print_ln();
	token_show (cur_chr);
  } else if ((cur_cmd == top_bot_mark) && (cur_chr < marks_code)) {
	print_char (':');
	print_ln();
	token_show (cur_mark[cur_chr]);
  };
}

/* module 464 */

/* 
 * The token lists for macros and for other things like \.{\\mark} and \.{\\output}
 * and \.{\\write} are produced by a procedure called |scan_toks|.
 * 
 * Before we get into the details of |scan_toks|, let's consider a much
 * simpler task, that of converting the current string into a token list.
 * The |str_toks| function does this; it classifies spaces as type |spacer|
 * and everything else as type |other_char|.
 * 
 * The token list created by |str_toks| begins at |link(temp_head)| and ends
 * at the value |p| that is returned. (If |p=temp_head|, the list is empty.)
 */

pointer
str_toks (pool_pointer b) {
  /* changes the string |str_pool[b..pool_ptr]| to a token list */
  pointer p; /* tail of the token list */ 
  pointer q; /* new node being added to the token list via |store_new_token| */
  halfword t; /* token being appended */ 
  pool_pointer k; /* index into |str_pool| */ 
  str_room (1);
  p = temp_head;
  link (p) = null;
  k = b;
  while (k < pool_ptr) {
	t = str_pool[k];
	if (t == ' ') {
	  t = space_token;
	} else {
	  t = other_token + t;
	}
	fast_store_new_token (t);
	incr (k);
  };
  pool_ptr = b;
  return p;
}


/* module 465 */

/* The main reason for wanting |str_toks| is the next function,
 * |the_toks|, which has similar input/output characteristics.
 * 
 * This procedure is supposed to scan something like `\.{\\skip\\count12}',
 * i.e., whatever can follow `\.{\\the}', and it constructs a token list
 * containing something like `\.{-3.0pt minus 0.5fill}'.
 */
pointer
the_toks (void) {
  unsigned char old_setting; /* holds |selector| setting */ 
  pointer p, q, r; /* used for copying a token list */ 
  pool_pointer b; /* base of temporary string */ 
  small_number c; /* value of |cur_chr| */
  /* begin expansion of Handle \.{\\unexpanded} or \.{\\detokenize} and |return| */
  /* module 1634 */
  if (odd (cur_chr)) {
	c = cur_chr;
	scan_general_text();
	if (c == 1) {
	  return cur_val;
	} else {
	  old_setting = selector;
	  selector = new_string;
	  b = pool_ptr;
	  p = get_avail();
	  link (p) = link (temp_head);
	  token_show (p);
	  flush_list (p);
	  selector = old_setting;
	  return str_toks (b);
	};
  }
  /* end expansion of Handle \.{\\unexpanded} or \.{\\detokenize} and |return| */
  get_x_token();
  scan_something_internal (tok_val, false);
  if (cur_val_level >= ident_val) {
	/* begin expansion of Copy the token list */
	/* module 466 */
	p = temp_head;
	link (p) = null;
	if (cur_val_level == ident_val) {
	  store_new_token (cs_token_flag + cur_val) ;
	} else if (cur_val != null) {
	  r = link (cur_val); /* do not copy the reference count */ 
	  while (r != null) {
		fast_store_new_token (info (r));
		r = link (r);
	  };
	};
	return p;
	/* end expansion of Copy the token list */
  } else {
	old_setting = selector;
	selector = new_string;
	b = pool_ptr;
	switch (cur_val_level) {
	case int_val:
	  print_int (cur_val);
	  break;
	case dimen_val:
	  print_scaled (cur_val);
	  zprint_string("pt");
	  break;
	case glue_val:
	  print_spec (cur_val,"pt");
	  delete_glue_ref (cur_val);
	  break;
	case mu_val:
	  print_spec (cur_val,"mu");
	  delete_glue_ref (cur_val);
	}; /* there are no other cases */ 
	selector = old_setting;
	return str_toks (b);
  };
}


/* module 467 */

/* Here's part of the |expand| subroutine that we are now ready to complete:
 */
void
ins_the_toks (void) {
  link (garbage) = the_toks();
  ins_list (link (temp_head));
};


/* module 470 */

/* The procedure |conv_toks| uses |str_toks| to insert the token list
 * for |convert| functions into the scanner; `\.{\\outer}' control sequences
 * are allowed to follow `\.{\\string}' and `\.{\\meaning}'.
 */
void
conv_toks (void) {
  unsigned char old_setting; /* holds |selector| setting */ 
  small_number c; /* desired type of conversion */ 
  small_number save_scanner_status; /* |scanner_status| upon entry */ 
  pool_pointer b; /* base of temporary string */ 
  c = cur_chr;
  /* begin expansion of Scan the argument for command |c| */
  /* module 471 */
  switch (c) {
  case number_code:
  case roman_numeral_code:
	scan_int();
	break;
  case string_code:
  case meaning_code:
	save_scanner_status = scanner_status;
	scanner_status = normal;
	get_token();
	scanner_status = save_scanner_status;
	break;
  case font_name_code:
	scan_font_ident();
	break;
  case job_name_code:
	if (jobname == 0)
	  open_log_file();
	break;
	/* begin expansion of Cases of `Scan the argument for command |c|' */
	/* module 1596 */
  case eTeX_revision_code:
	do_something;
	break;
	/* end expansion of Cases of `Scan the argument for command |c|' */
  case pdftex_revision_code:
	do_something;
	break;
  case pdf_font_name_code:
  case pdf_font_objnum_code:
  case pdf_font_size_code:
	scan_font_ident();
	if (cur_val == null_font)
	  pdf_error_string("font","invalid font identifier");
	if (c != pdf_font_size_code) {
	  pdf_check_vf (cur_val);
	  if (!font_used[cur_val])
		pdf_init_font (cur_val);
	};
  }				/* there are no other cases */
  /* end expansion of Scan the argument for command |c| */
  old_setting = selector;
  selector = new_string;
  b = pool_ptr;
  /* begin expansion of Print the result of command |c| */
  /* module 472 */
  switch (c) {
  case number_code:
	print_int (cur_val);
	break;
  case roman_numeral_code:
	print_roman_int (cur_val);
	break;
  case string_code:
	if (cur_cs != 0) {
	  sprint_cs (cur_cs);
	} else {
	  print_char (cur_chr);
	}
	break;
  case meaning_code:
	print_meaning();
	break;
  case font_name_code:
	zprint (font_name[cur_val]);
	if (font_size[cur_val] != font_dsize[cur_val]) {
	  zprint_string(" at ");
	  print_scaled (font_size[cur_val]);
	  zprint_string("pt");
	};
	break;
  case job_name_code:
	zprint (jobname);
	break;
	/* begin expansion of Cases of `Print the result of command |c|' */
	/* module 1597 */
  case eTeX_revision_code:
	zprint_string (eTeX_revision);
	break;
	/* end expansion of Cases of `Print the result of command |c|' */
  case pdftex_revision_code:
	zprint (pdftex_revision);
	break;
  case pdf_font_name_code:
  case pdf_font_objnum_code:
	set_ff (cur_val);
	if (c == pdf_font_name_code) {
	  print_int (obj_info (pdf_font_num[ff]));
	} else {
	  print_int (pdf_font_num[ff]);
	};
	break;
  case pdf_font_size_code:
	print_scaled (font_size[cur_val]);
  };				/* there are no other cases */
  /* end expansion of Print the result of command |c| */
  selector = old_setting;
  link (garbage) = str_toks (b);
  ins_list (link (temp_head));
}


/* module 473 */

/* Now we can't postpone the difficulties any longer; we must bravely tackle
 * |scan_toks|. This function returns a pointer to the tail of a new token
 * list, and it also makes |def_ref| point to the reference count at the
 * head of that list.
 * 
 * There are two boolean parameters, |macro_def| and |xpand|. If |macro_def|
 * is true, the goal is to create the token list for a macro definition;
 * otherwise the goal is to create the token list for some other \TeX\
 * primitive: \.{\\mark}, \.{\\output}, \.{\\everypar}, \.{\\lowercase},
 * \.{\\uppercase}, \.{\\message}, \.{\\errmessage}, \.{\\write}, or
 * \.{\\special}. In the latter cases a left brace must be scanned next; this
 * left brace will not be part of the token list, nor will the matching right
 * brace that comes at the end. If |xpand| is false, the token list will
 * simply be copied from the input using |get_token|. Otherwise all expandable
 * tokens will be expanded until unexpandable tokens are left, except that
 * the results of expanding `\.{\\the}' are not expanded further.
 * If both |macro_def| and |xpand| are true, the expansion applies
 * only to the macro body (i.e., to the material following the first
 * |left_brace| character).
 * 
 * The value of |cur_cs| when |scan_toks| begins should be the |eqtb|
 * address of the control sequence to display in ``runaway'' error
 * messages.
 */
pointer
scan_toks (boolean macro_def, boolean xpand) {
  halfword t; /* token representing the highest parameter number */ 
  halfword s; /* saved token */ 
  pointer p; /* tail of the token list being built */ 
  pointer q; /* new node being added to the token list via |store_new_token| */
  halfword unbalance; /* number of unmatched left braces */ 
  halfword hash_brace; /* possible `\.{\#\{}' token */ 
  if (macro_def) {
    scanner_status = defining;
  } else {
    scanner_status = absorbing;
  }
  warning_index = cur_cs;
  def_ref = get_avail();
  token_ref_count (def_ref) = null;
  p = def_ref;
  hash_brace = 0;
  t = zero_token;
  if (macro_def) {
	/* begin expansion of Scan and build the parameter part of the macro definition */
	/* module 474 */
	loop {
	  get_token(); /* set |cur_cmd|, |cur_chr|, |cur_tok| */ 
	  if (cur_tok < right_brace_limit)
		goto DONE1;
	  if (cur_cmd == mac_param) {
		/* begin expansion of If the next character is a parameter number, make |cur_tok| 
		   a |match| token; but if it is a left brace, store `|left_brace|, |end_match|', 
		   set |hash_brace|, and |goto done| */
		/* module 476 */
	    s = match_token + cur_chr;
	    get_token();
	    if (cur_cmd == left_brace) {
		  hash_brace = cur_tok;
		  store_new_token (cur_tok);
		  store_new_token (end_match_token);
		  goto DONE;
		};
	    if (t == zero_token + 9) {
		  print_err ("You already have nine parameters");
		  help1 ("I'm going to ignore the # sign you just used.");
		  error();
		} else {
		  incr (t);
		  if (cur_tok != t) {
		    print_err ("Parameters must be numbered consecutively");
		    help2 ("I've inserted the digit you should have used after the #.",
			   "Type `1' to delete what you did use.");
		    back_error();
		  };
		  cur_tok = s;
		};
	  }
	  /* end expansion of If the next character is a parameter number, ... */
	  store_new_token (cur_tok);
	};
    DONE1:
	store_new_token (end_match_token);
	if (cur_cmd == right_brace) {
	  /* begin expansion of Express shock at the missing left brace; |goto found| */
	  /* module 475 */
	  print_err ("Missing { inserted");
	  incr (align_state);
	  help2 ("Where was the left brace? You said something like `\\def\\a}',",
		 "which I'm going to interpret as `\\def\\a{}'.");
	  error();
	  goto FOUND;
	};
	/* end expansion of Express shock at the missing left brace; |goto found| */
  DONE:
	do_nothing;
	/* end expansion of Scan and build the parameter part of the macro definition */
  } else {
    scan_left_brace(); /* remove the compulsory left brace */
  } 
  /* begin expansion of Scan and build the body of the token list; |goto found| when finished */
  /* module 477 */
  unbalance = 1;
  loop {
    if (xpand) {
	  /* begin expansion of Expand the next part of the input */
	  /* module 478 */
	  /* Here we insert an entire token list created by |the_toks| without
	   * expanding it further.
	   */
	  loop {
		get_next();
		if (cur_cmd >= call)
		  if (info (link (cur_chr)) == protected_token) {
			cur_cmd = relax;
			cur_chr = no_expand_flag;
	      };
		if (cur_cmd <= max_command)
		  goto DONE2;
		if (cur_cmd != the) {
		  expand();
	    } else {
	      q = the_toks();
	      if (link (temp_head) != null) {
			link (p) = link (temp_head);
			p = q;
		  };
	    };
	  };
    DONE2:
	  x_token();
	  /* end expansion of Expand the next part of the input */
	} else {
      get_token();
	}
    if (cur_tok < right_brace_limit) {
      if (cur_cmd < right_brace) {
		incr (unbalance);
	  } else {
		decr (unbalance);
		if (unbalance == 0)
		  goto FOUND;
	  }
	} else if (cur_cmd == mac_param)
	  if (macro_def) {
		/* begin expansion of Look for parameter number or \.{\#\#} */
		/* module 479 */
	    s = cur_tok;
	    if (xpand) {
	      get_x_token();
		} else {
	      get_token();
		}
	    if (cur_cmd != mac_param) {
	      if ((cur_tok <= zero_token) || (cur_tok > t)) {
			print_err ("Illegal parameter number in definition of ");
			sprint_cs (warning_index);
			help3  ("You meant to type ## instead of #, right?",
					"Or maybe a } was forgotten somewhere earlier, and things",
					"are all screwed up? I'm going to assume that you meant ##.");
			back_error();
			cur_tok = s;
		  } else {
			cur_tok = out_param_token - '0' + cur_chr; 
		  }
		};
	  }
	/* end expansion of Look for parameter number or \.{\#\#} */
    store_new_token (cur_tok);
  }
  /* end expansion of Scan and build the body of the token list; |goto found| when finished */
 FOUND:
  scanner_status = normal;
  if (hash_brace != 0)
    store_new_token (hash_brace);
  return p;
}

/* module 480 */

/* Another way to create a token list is via the \.{\\read} command. The
 * sixteen files potentially usable for reading appear in the following
 * global variables. The value of |read_open[n]| will be |closed| if
 * stream number |n| has not been opened or if it has been fully read;
 * |just_open| if an \.{\\openin} but not a \.{\\read} has been done;
 * and |normal| if it is open and ready to read the next line.
 */

FILE *read_file[16]; /* used for \.{\\read} */ 
unsigned char read_open[17]; /* state of |read_file[n]| */

/* module 482 */

/* The |read_toks| procedure constructs a token list like that for any
 * macro definition, and makes |cur_val| point to it. Parameter |r| points
 * to the control sequence that will receive this token list.
 */
void
read_toks (int n, pointer r, halfword j) {
  pointer p; /* tail of the token list */ 
  pointer q; /* new node being added to the token list via |store_new_token| */ 
  int s; /* saved value of |align_state| */ 
  small_number m; /* stream number */ 
  scanner_status = defining;
  warning_index = r;
  def_ref = get_avail();
  token_ref_count (def_ref) = null;
  p = def_ref; /* the reference count */ 
  store_new_token (end_match_token);
  if ((n < 0) || (n > 15)) {
    m = 16;
  } else {
    m = n;
  }
  s = align_state;
  align_state = 1000000; /* disable tab marks, etc. */
  do {
	/* begin expansion of Input and store tokens from the next line of the file */
	/* module 483 */
	begin_file_reading();
	name = m + 1;
	if (read_open[m] == closed) {
	  /* begin expansion of Input for \.{\\read} from the terminal */
	  /* module 484 */
	  /* Here we input on-line into the |buffer| array, prompting the user explicitly
	   * if |n>=0|. The value of |n| is set negative so that additional prompts
	   * will not be given in the case of multi-line input.
	   */
	  if (interaction > nonstop_mode) {
		if (n < 0) {
		  prompt_input ("");
	    } else {
	      wake_up_terminal;
	      print_ln();
	      sprint_cs (r);
	      prompt_input ("=");
	      n = -1;
	    }
	  } else {
	    fatal_error ("*** (cannot \\read from terminal in nonstop modes)");
	  }
	  /* end expansion of Input for \.{\\read} from the terminal */
	} else if (read_open[m] == just_open) {
	  /* Input the first line of |read_file[m]| */
	  read_first_line(m);
	} else {
	  /* Input the next line of |read_file[m]| */
	  read_next_line(m);
	};
	limit = last;
	if (end_line_char_inactive) {
	  decr (limit);
	} else {
	  buffer[limit] = end_line_char;
	}
	first = limit + 1;
	loc = start;
	state = new_line;
	/* begin expansion of Handle \.{\\readline} and |goto done| */
	/* module 1706 */
	if (j == 1) {
	  while (loc <= limit)	{ /* current line not yet finished */
		cur_chr = buffer[loc];
		incr (loc);
		if (cur_chr == ' ') {
		  cur_tok = space_token;
		} else {
		  cur_tok = cur_chr + other_token;
		}
		store_new_token (cur_tok);
	  };
	  goto DONE;
	};
	/* end expansion of Handle \.{\\readline} and |goto done| */
	loop {
	  get_token();
	  if (cur_tok == 0)
		goto DONE;
	  /* |cur_cmd=cur_chr=0| will occur at the end of the line */
	  if (align_state < 1000000) { /* unmatched `\.\}' aborts the line */
	    do {
		  get_token();
		} while (!(cur_tok == 0));
	    align_state = 1000000;
	    goto DONE;
	  };
	  store_new_token (cur_tok);
	};
  DONE: 
	end_file_reading();
	/* end expansion of Input and store tokens from the next line of the file */
  } while (!(align_state == 1000000));
  cur_val = def_ref;
  scanner_status = normal;
  align_state = s;
};

/* module 1539 */

/* To implement primitives as \.{\\pdfinfo}, \.{\\pdfcatalog} or
 * \.{\\pdfnames} we need to concatenate tokens lists.
 */
pointer 
concat_tokens (pointer q, pointer r) {
  /* concat |q| and |r| and returns the result tokens list */
  pointer p;
  if (q == null)
	return r;
  p = q;
  while (link (p) != null)
	p = link (p);
  link (p) = link (r);
  free_avail (r);
  return q;
};

/* module 481 */

void 
tokenlist_initialize (void) {
  int k;
  for (k = 0; k <= 16; k++)
    read_open[k] = closed;
}
