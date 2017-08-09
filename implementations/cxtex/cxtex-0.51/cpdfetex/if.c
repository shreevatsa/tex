
#include "types.h"
#include "c-compat.h"
#include "globals.h"

static void if_warning (void);

/* module 489 */

/* Conditions can be inside conditions, and this nesting has a stack
 * that is independent of the |save_stack|.
 * 
 * Four global variables represent the top of the condition stack:
 * |cond_ptr| points to pushed-down entries, if any; |if_limit| specifies
 * the largest code of a |fi_or_else| command that is syntactically legal;
 * |cur_if| is the name of the current type of conditional; and |if_line|
 * is the line number at which it began.
 * 
 * If no conditions are currently in progress, the condition stack has the
 * special state |cond_ptr=null|, |if_limit=normal|, |cur_if=0|, |if_line=0|.
 * Otherwise |cond_ptr| points to a two-word node; the |type|, |subtype|, and
 * |link| fields of the first word contain |if_limit|, |cur_if|, and
 * |cond_ptr| at the next level, and the second word contains the
 * corresponding |if_line|.
 */

pointer cond_ptr; /* top of the condition stack */ 
unsigned char if_limit; /* upper bound on |fi_or_else| codes */ 
small_number cur_if; /* type of conditional being worked on */ 
int if_line; /* line where that conditional began */

/* module 493 */

/* When we skip conditional text, we keep track of the line number
 * where skipping began, for use in error messages.
 */

integer skip_line; /* skipping began here */

/* module 494 */

/* Here is a procedure that ignores text until coming to an \.{\\or},
 * \.{\\else}, or \.{\\fi} at level zero of $\.{\\if}\ldots\.{\\fi}$
 * nesting. After it has acted, |cur_chr| will indicate the token that
 * was found, but |cur_tok| will not be set (because this makes the
 * procedure run faster).
 */

static void
pass_text (void) {
  int l; /* level of $\.{\\if}\ldots\.{\\fi}$ nesting */ 
  small_number save_scanner_status; /* |scanner_status| upon entry */ 
  save_scanner_status = scanner_status;
  scanner_status = skipping;
  l = 0;
  skip_line = line;
  loop {
    get_next();
    if (cur_cmd == fi_or_else) {
      if (l == 0)
	goto DONE;
      if (cur_chr == fi_code)
        l--;
    } else if (cur_cmd == if_test) {
      l++;
    }
  };
 DONE:
  scanner_status = save_scanner_status;
  if (tracing_ifs > 0)
    show_cur_cmd_chr();
};

/* module 496 */
#define pop_the_condition_stack  {\
		  if (if_stack[in_open] == cond_ptr)\
		    if_warning(); /* conditionals possibly not properly nested with files */\
		  p = cond_ptr;\
		  if_line = if_line_field (p);\
		  cur_if = subtype (p);\
		  if_limit = type (p);\
		  cond_ptr = link (p);\
		  free_node (p, if_node_size); }


/* module 497 */

/* Here's a procedure that changes the |if_limit| code corresponding to
 * a given value of |cond_ptr|.
 */
static void
change_if_limit (small_number l, pointer p) {
  pointer q;
  if (p == cond_ptr) {
    if_limit = l;		/* that's the easy case */
  } else {
	q = cond_ptr;
	loop {
	  if (q == null)
		confusion ("if");
	  if (link (q) == p) {
	    type (q) = l;
	    return;
	  };
	  q = link (q);
	};
  };
};


/* module 506 */
#define get_x_token_or_active_char { get_x_token() ;  if (  cur_cmd  ==  relax  )\
           if (  cur_chr  ==  no_expand_flag  )  { cur_cmd   =  active_char ;\
                 cur_chr   =  cur_tok  -  cs_token_flag  -  active_base ;};}


/* module 498 */

/* A condition is started when the |expand| procedure encounters
 * an |if_test| command; in that case |expand| reduces to |conditional|,
 * which is a recursive procedure.
 */
void
conditional (void) {
  boolean b; /* is the condition true? */ 
  unsigned int r; /* relation to be evaluated */ 
  int m, n; /* to be tested against the second operand */ 
  pointer p, q; /* for traversing token lists in \.{\\ifx} tests */ 
  small_number save_scanner_status; /* |scanner_status| upon entry */ 
  pointer save_cond_ptr; /* |cond_ptr| corresponding to this conditional */ 
  small_number this_if; /* type of this conditional */ 
  boolean is_unless; /* was this if preceded by `\.{\\unless}' ? */
  b=false; /*TH -Wall*/
  if (tracing_ifs > 0)
    if (tracing_commands <= 1)
      show_cur_cmd_chr();
  /* begin expansion of Push the condition stack */
  /* module 495 */
  /* When we begin to process a new \.{\\if}, we set |if_limit:=if_code|; then
   * if\/ \.{\\or} or \.{\\else} or \.{\\fi} occurs before the current \.{\\if}
   * condition has been evaluated, \.{\\relax} will be inserted.
   * For example, a sequence of commands like `\.{\\ifvoid1\\else...\\fi}'
   * would otherwise require something after the `\.1'.
   */
  p = get_node (if_node_size);
  link (p) = cond_ptr;
  type (p) = if_limit;
  subtype (p) = cur_if;
  if_line_field (p) = if_line;
  cond_ptr = p;
  cur_if = cur_chr;
  if_limit = if_code;
  if_line = line;
  /* end expansion of Push the condition stack */
  save_cond_ptr = cond_ptr;
  is_unless = (cur_chr >= unless_code);
  this_if = cur_chr % unless_code;
  /* begin expansion of Either process \.{\\ifcase} or set |b| to the value of a boolean condition */
  /* module 501 */
  switch (this_if) {
  case if_char_code:
  case if_cat_code:
	/* begin expansion of Test if two characters match */
	/* module 506 */
	/* An active character will be treated as category 13 following
	 * \.{\\if\\noexpand} or following \.{\\ifcat\\noexpand}. We use the fact that
	 * active characters have the smallest tokens, among all control sequences.
	 */
	get_x_token_or_active_char;
	if ((cur_cmd > active_char) || (cur_chr > 255))	{ /* not a character */
	  m = relax;
	  n = 256;
	} else {
	  m = cur_cmd;
	  n = cur_chr;
	};
	get_x_token_or_active_char;
	if ((cur_cmd > active_char) || (cur_chr > 255)) {
	  cur_cmd = relax;
	  cur_chr = 256;
	};
	if (this_if == if_char_code) {
	  b = (n == cur_chr);
	} else {
	  b = (m == cur_cmd);
	}
	/* end expansion of Test if two characters match */
	break;
  case if_int_code:
  case if_dim_code:
	/* begin expansion of Test relation between integers or dimensions */
	/* module 503 */
	/* Here we use the fact that |"<"|, |"="|, and |">"| are consecutive ASCII
	 * codes.
	 */
	if (this_if == if_int_code) {
	  scan_int();
	} else {
	  scan_normal_dimen;
	}
	n = cur_val;
	/* Get the next non-blank non-call... */
	get_nblank_ncall;
	if ((cur_tok >= other_token + '<') && (cur_tok <= other_token + '>')) {
	  r = cur_tok - other_token;
	} else {
	  print_err ("Missing = inserted for ");
	  print_cmd_chr (if_test, this_if);
	  help1 ("I was expecting to see `<', `=', or `>'. Didn't.");
	  back_error();
	  r = '=';
	};
	if (this_if == if_int_code) {
	  scan_int();
	} else {
	  scan_normal_dimen;
	}
	switch (r) {
	case '<':
	  b = (n < cur_val);
	  break;
	case '=':
	  b = (n == cur_val);
	  break;
	case '>':
	  b = (n > cur_val);
	};
	/* end expansion of Test relation between integers or dimensions */
	break;
  case if_odd_code:
	/* begin expansion of Test if an integer is odd */
	/* module 504 */
	scan_int();
	b = odd (cur_val);
	/* end expansion of Test if an integer is odd */
	break;
  case if_vmode_code:
	b = (abs (mode) == vmode);
	break;
  case if_hmode_code:
	b = (abs (mode) == hmode);
	break;
  case if_mmode_code:
	b = (abs (mode) == mmode);
	break;
  case if_inner_code:
	b = (mode < 0);
	break;
  case if_void_code:
  case if_hbox_code:
  case if_vbox_code:
	/* begin expansion of Test box register status */
	/* module 505 */
	scan_register_num();
	fetch_box (p);
	if (this_if == if_void_code) {
	  b = (p == null);
	} else if (p == null) {
	  b = false;
	} else if (this_if == if_hbox_code) {
	  b = (type (p) == hlist_node);
	} else {
	  b = (type (p) == vlist_node);
	}
	/* end expansion of Test box register status */
	break;
  case ifx_code:
	/* begin expansion of Test if two tokens match */
	/* module 507 */
	/* Note that `\.{\\ifx}' will declare two macros different if one is \\{long}
	 * or \\{outer} and the other isn't, even though the texts of the macros are
	 * the same.
	 * 
	 * We need to reset |scanner_status|, since \.{\\outer} control sequences
	 * are allowed, but we might be scanning a macro definition or preamble.
	 */
	save_scanner_status = scanner_status;
	scanner_status = normal;
	get_next();
	n = cur_cs;
	p = cur_cmd;
	q = cur_chr;
	get_next();
	if (cur_cmd != p) {
	  b = false;
	} else if (cur_cmd < call) {
	  b = (cur_chr == q);
	} else {
	  /* begin expansion of Test if two macro texts match */
	  /* module 508 */
	  /* Note also that `\.{\\ifx}' decides that macros \.{\\a} and \.{\\b} are
	   * different in examples like this:
	   * $$\vbox{\halign{\.{#}\hfil&\qquad\.{#}\hfil\cr
	   * 
	   * {}\\def\\a\{\\c\}&
	   * {}\\def\\c\{\}\cr
	   * {}\\def\\b\{\\d\}&
	   * {}\\def\\d\{\}\cr}}$$
	   */
	  p = link (cur_chr);
	  q = link (equiv (n)); /* omit reference counts */ 
	  if (p == q) {
		b = true;
	  } else {
		while ((p != null) && (q != null)) {
		  if (info (p) != info (q)) {
		    p = null;
		  } else {
			p = link (p);
			q = link (q);
		  };
		};
		b = ((p == null) && (q == null));
	  };
	  /* end expansion of Test if two macro texts match */
	}
	scanner_status = save_scanner_status;
	/* end expansion of Test if two tokens match */
	break;
  case if_eof_code:
	scan_four_bit_int();
	b = (read_open[cur_val] == closed);
	break;
  case if_true_code:
	b = true;
	break;
  case if_false_code:
	b = false;
	break;
	/* begin expansion of Cases for |conditional| */
  case if_def_code:
	/* module 1711 */
	/* The conditional \.{\\ifdefined} tests if a control sequence is
	 * defined.
	 * 
	 * We need to reset |scanner_status|, since \.{\\outer} control sequences
	 * are allowed, but we might be scanning a macro definition or preamble.
	 */
	save_scanner_status = scanner_status;
	scanner_status = normal;
	get_next();
	b = (cur_cmd != undefined_cs);
	scanner_status = save_scanner_status;
	break;
  case if_cs_code:
	/* module 1712 */
	/* The conditional \.{\\ifcsname} is equivalent to \.{\{\\expandafter}
	 * \.{\}\\expandafter} \.{\\ifdefined} \.{\\csname}, except that no new
	 * control sequence will be entered into the hash table (once all tokens
	 * preceding the mandatory \.{\\endcsname} have been expanded).
	 */
	n = get_avail();
	p = n; /* head of the list of characters */
	do {
	  get_x_token();
	  if (cur_cs == 0)
		store_new_token (cur_tok);
	} while (cur_cs == 0);
	if (cur_cmd != end_cs_name)
	  /* Complain about missing \.{\\endcsname} */
	  complain_missing_csname;
	/* begin expansion of Look up the characters of list |n| in the hash table, and set |cur_cs| */
	/* module 1713 */
	m = first;
	p = link (n);
	while (p != null) {
	  if (m >= max_buf_stack) {
		max_buf_stack = m + 1;
		if (max_buf_stack == buf_size)
		  realloc_buffer();
	  };
	  buffer[m] = info (p) % 256;
	  incr (m);
	  p = link (p);
	};
	if (m > (int)first + 1) {
	  cur_cs = id_lookup (first, m - first);	/* |no_new_control_sequence| is |true| */
	} else if (m == (int)first) {
	  cur_cs = null_cs;	/* the list is empty */
	} else {
	  cur_cs = single_base + buffer[first];	/* the list has length one */
	}
	/* end expansion of Look up the characters of list |n| in the hash table, and set |cur_cs| */
	flush_list (n);
	b = (eq_type (cur_cs) != undefined_cs);
	break;
  case if_font_char_code:
	/* module 1714 */
	/* The conditional \.{\\iffontchar} tests the existence of a character in a font. */
	scan_font_ident();
	n = cur_val;
	scan_char_num();
	if ((font_bc[n] <= cur_val) && (font_ec[n] >= cur_val)) {
	  b = char_exists (char_info (n,qi (cur_val)));
	} else {
	  b = false;
	};
	break;
	/* end expansion of Cases for |conditional| */
  case if_case_code:
	/* begin expansion of Select the appropriate case and |return| or |goto common_ending| */
	/* module 509 */
	scan_int();
	n = cur_val; /* |n| is the number of cases to pass */ 
	if (tracing_commands > 1) {
	  begin_diagnostic();
	  zprint_string("{case ");
	  print_int (n);
	  print_char ('}');
	  end_diagnostic (false);
	};
	while (n != 0) {
	  pass_text();
	  if (cond_ptr == save_cond_ptr) {
		if (cur_chr == or_code) {
		  decr (n);
		} else {
		  goto COMMON_ENDING;
		}
	  } else if (cur_chr == fi_code) {
		/* Pop the condition stack */
		pop_the_condition_stack;
	  }
	};
	change_if_limit (or_code, save_cond_ptr);
	return;
	/* wait for \.{\\or}, \.{\\else}, or \.{\\fi} */ 
	/* end expansion of Select the appropriate case and |return| or |goto common_ending| */
	/* there are no other cases */
  };
  /* end expansion of Either process \.{\\ifcase} or set |b| to the value of a boolean condition */
  if (is_unless)
    b = !b;
  if (tracing_commands > 1) {
	/* begin expansion of Display the value of |b| */
	/* module 502 */
	begin_diagnostic();
	if (b) {
	  zprint_string("{true}");
	} else {
	  zprint_string("{false}");
	}
	end_diagnostic (false);
	/* end expansion of Display the value of |b| */
  };
  if (b) {
	change_if_limit (else_code, save_cond_ptr);
	return; /* wait for \.{\\else} or \.{\\fi} */ 
  };
  /* begin expansion of Skip to \.{\\else} or \.{\\fi}, then |goto common_ending| */
  /* module 500 */
  /* In a construction like `\.{\\if\\iftrue abc\\else d\\fi}', the first
   * \.{\\else} that we come to after learning that the \.{\\if} is false is
   * not the \.{\\else} we're looking for. Hence the following curious
   * logic is needed.
   */
  loop {
    pass_text();
    if (cond_ptr == save_cond_ptr) {
	  if (cur_chr != or_code)
		goto COMMON_ENDING;
	  print_err ("Extra ");
	  print_esc_string ("or");
	  help1 ("I'm ignoring this; it doesn't match any \\if.");
	  error();
	} else if (cur_chr == fi_code) {
	  /* Pop the condition stack */
	  pop_the_condition_stack;
	}
  }
  /* end expansion of Skip to \.{\\else} or \.{\\fi}, then |goto common_ending| */
 COMMON_ENDING:
  if (cur_chr == fi_code) {
	/* Pop the condition stack */
	pop_the_condition_stack;
  } else {
    if_limit = fi_code;
  } /* wait for \.{\\fi} */ 
};

/* module 510 */
/* The processing of conditionals is complete except for the following
 * code, which is actually part of |expand|. It comes into play when
 * \.{\\or}, \.{\\else}, or \.{\\fi} is scanned.
 */
/* Terminate the current conditional and skip to \.{\\fi} */
void
finish_conditional(void) {
  pointer p;
  if (tracing_ifs > 0)
	if (tracing_commands <= 1)
	  show_cur_cmd_chr();
  if (cur_chr > if_limit) {
	if (if_limit == if_code) {
	  insert_relax();	/* condition not yet evaluated */
	} else {
	  print_err ("Extra ");
	  print_cmd_chr (fi_or_else, cur_chr);
	  help1 ("I'm ignoring this; it doesn't match any \\if.");
	  error();
	};
  } else {
	while (cur_chr != fi_code) 
	  pass_text(); /* skip to \.{\\fi} */
	/* Pop the condition stack */
	pop_the_condition_stack;
  };
}

/* module 1721 */

/* When a conditional ends that was apparently started in a different
 * input file, the |if_warning| procedure is invoked in order to update the
 * |if_stack|. If moreover \.{\\tracingnesting} is positive we want to
 * give a warning message (with the same complications as above).
 */
static void
if_warning (void) {
  unsigned int i; /* index into |if_stack| */ 
  boolean w; /* do we need a warning? */ 
  base_ptr = input_ptr; 
  input_stack[base_ptr] = cur_input; /* store current state */ 
  i = in_open;
  w = false;
  while (if_stack[i] == cond_ptr) {
	/* Set variable |w| to... */
	set_w_for_reporting;
	if_stack[i] = link (cond_ptr);
	decr (i);
  };
  if (w) {
	print_nl_string("Warning: end of ");
	print_cmd_chr (if_test, cur_if);
	print_if_line (if_line);
	zprint_string(" of a different file");
	print_ln();
	if (tracing_nesting > 1)
	  show_context();
	if (history == spotless)
	  history = warning_issued;
  };
};

/* module 490 */

void
if_initialize (void) {
  cond_ptr = null;
  if_limit = normal;
  cur_if = 0;
  if_line = 0;
}
