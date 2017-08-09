

#include "types.h"
#include "c-compat.h"
#include "globals.h"


/* module 1360 */

/* When a control sequence is to be defined, by \.{\\def} or \.{\\let} or
 * something similar, the |get_r_token| routine will substitute a special
 * control sequence for a token that is not redefinable.
 */
void 
get_r_token (void) {
 RESTART:
  do {
	get_token();
  } while (cur_tok == space_token);
  if ((cur_cs == 0) ||
	  ((cur_cs > frozen_control_sequence) && (cur_cs <= eqtb_size))) {
	print_err ("Missing control sequence inserted");
	help5 ("Please don't say `\\def cs{...}', say `\\def\\cs{...}'.",
		   "I've inserted an inaccessible control sequence so that your",
		   "definition will be completed without mixing me up too badly.",
		   "You can recover graciously from this error, if you're",
		   "careful; see exercise 27.2 in The TeXbook.");
	if (cur_cs == 0)
	  back_input();
	cur_tok = cs_token_flag + frozen_protection;
	ins_error();
	goto RESTART;
  };
};

/* module 1374 */

/* When a glue register or parameter becomes zero, it will always point to
 * |zero_glue| because of the following procedure. (Exception: The tabskip
 * glue isn't trapped while preambles are being scanned.)
 */
void 
trap_zero_glue (void) {
  if ((width (cur_val) == 0) && (stretch (cur_val) == 0) && (shrink (cur_val) == 0)) {
	add_glue_ref (zero_glue);
	delete_glue_ref (cur_val);
	cur_val = zero_glue;
  };
};

/* module 1381 */

/* We use the fact that |register<advance<multiply<divide|.  */
void 
do_register_command (small_number a) {
  pointer l, q, r, s; /* for list manipulation */ 
  unsigned char p; /* type of register involved */ 
  boolean e; /* does |l| refer to a sparse array element? */ 
  int w; /* integer or dimen value of |l| */ 
  q = cur_cmd;
  e = false; /* just in case, will be set |true| for sparse array elements */
  /* begin expansion of Compute the register location |l| and its type |p|; but |return| if invalid */
  l = null; s = null; w=0; /*TH -Wall*/
  /* module 1382 */
  /* Here we use the fact that the consecutive codes |int_val...mu_val| and
   * |assign_int..assign_mu_glue| correspond to each other nicely.
   */
  if (q != register_cmd) {
	get_x_token();
	if ((cur_cmd >= assign_int) && (cur_cmd <= assign_mu_glue)) {
	  l = cur_chr;
	  p = cur_cmd - assign_int;
	  goto FOUND;
	};
	if (cur_cmd != register_cmd) {
	  print_err ("You can't use `");
	  print_cmd_chr (cur_cmd, cur_chr);
	  zprint_string("' after ");
	  print_cmd_chr (q, 0);
	  help1  ("I'm forgetting what you said and not changing anything.");
	  error();
	  return;
	};
  };
  if ((cur_chr < mem_bot) || (cur_chr > lo_mem_stat_max)) {
	l = cur_chr;
	p = sa_type (l);
	e = true;
  } else {
	p = cur_chr - mem_bot;
	scan_register_num();
	if (cur_val > 255) {
	  find_sa_element (p, cur_val, true);
	  l = cur_ptr;
	  e = true;
	} else {
	  switch (p) {
	  case int_val:
		l = cur_val + count_base;
		break;
	  case dimen_val:
		l = cur_val + scaled_base;
		break;
	  case glue_val:
		l = cur_val + skip_base;
		break;
	  case mu_val:
		l = cur_val + mu_skip_base;
	  }; /* there are no other cases */ 
	};
  };
 FOUND:
  if (p < glue_val) {
	if (e) {
	  w = sa_int (l);
	} else {
	  w = eqtb[l].cint;
	} 
  } else if (e) {
	s = sa_ptr (l);
  } else {
	s = equiv (l);
  }
  /* end expansion of Compute the register location |l| and its type |p|; but |return| if invalid */
  if (q == register_cmd) {
	scan_optional_equals();
  } else if (scan_keyword ("by")) {
	do_nothing; /* optional `\.{by}' */ 
  };
  arith_error = false;
  if (q < multiply) {
	/* begin expansion of Compute result of |register| or |advance|, put it in |cur_val| */
	/* module 1383 */
	if (p < glue_val) {
	  if (p == int_val) {
		scan_int();
	  } else {
		scan_normal_dimen;
	  }
	  if (q == advance)
		cur_val = cur_val + w;
	} else {
	  scan_glue (p);
	  if (q == advance) {
		/* begin expansion of Compute the sum of two glue specs */
		/* module 1384 */
		q = new_spec (cur_val);
		r = s;
		delete_glue_ref (cur_val);
		width (q) = width (q) + width (r);
		if (stretch (q) == 0)
		  stretch_order (q) = normal;
		if (stretch_order (q) == stretch_order (r)) {
		  stretch (q) = stretch (q) + stretch (r);
		} else if ((stretch_order (q) < stretch_order (r)) && (stretch (r) != 0)) {
		  stretch (q) = stretch (r);
		  stretch_order (q) = stretch_order (r);
		};
		if (shrink (q) == 0)
		  shrink_order (q) = normal;
		if (shrink_order (q) == shrink_order (r)) {
		  shrink (q) = shrink (q) + shrink (r);
		} else if ((shrink_order (q) < shrink_order (r)) && (shrink (r) != 0)) {
		  shrink (q) = shrink (r);
		  shrink_order (q) = shrink_order (r);
		};
		cur_val = q;
		/* end expansion of Compute the sum of two glue specs */
	  }
	  /* end expansion of Compute result of |register| or |advance|, put it in |cur_val| */
	}
  } else {
	/* begin expansion of Compute result of |multiply| or |divide|, put it in |cur_val| */
	/* module 1385 */
	scan_int();
	if (p < glue_val) {
	  if (q == multiply) {
		if (p == int_val) {
		  cur_val = MULT_INTEGERS (w, cur_val);
		} else {
		  cur_val = NX_PLUS_Y (w, cur_val, 0);
		}
	  } else {
		cur_val = x_over_n (w, cur_val);
	  }
	} else {
	  r = new_spec (s);
	  if (q == multiply) {
		width (r) = NX_PLUS_Y (width (s), cur_val, 0);
		stretch (r) = NX_PLUS_Y (stretch (s), cur_val, 0);
		shrink (r) = NX_PLUS_Y (shrink (s), cur_val, 0);
	  } else {
		width (r) = x_over_n (width (s), cur_val);
		stretch (r) =  x_over_n (stretch (s), cur_val);
		shrink (r) = x_over_n (shrink (s), cur_val);
	  };
	  cur_val = r;
	};
  };
  /* end expansion of Compute result of |multiply| or |divide|, put it in |cur_val| */
  if (arith_error){
	print_err ("Arithmetic overflow");
	help2 ("I can't carry out that multiplication or division,",
		   "since the result is out of range.");
	error();
	return;
  };
  if (p < glue_val) {
	sa_word_define (l, cur_val);
  } else {
	trap_zero_glue();
	sa_define (l, cur_val) (l, glue_ref, cur_val);
  };
};

/* module 1388 */
void 
alter_aux (void) {
  halfword c; /* |hmode| or |vmode| */ 
  if (cur_chr != abs (mode)) {
	report_illegal_case();
  } else {
	c = cur_chr;
	scan_optional_equals();
	if (c == vmode) {
	  scan_normal_dimen;
	  prev_depth = cur_val;
	} else {
	  scan_int();
	  if ((cur_val <= 0) || (cur_val > 32767)) {
		print_err ("Bad space factor");
		help1  ("I allow only values in the range 1..32767 here.");
		int_error (cur_val);
	  } else {
		space_factor = cur_val;
	  }
	};
  };
};

/* module 1390 */
void 
alter_page_so_far (void) {
  unsigned char c; /* index into |page_so_far| */ 
  c = cur_chr;
  scan_optional_equals();
  scan_normal_dimen;
  page_so_far[c] = cur_val;
};


/* module 1391 */
void 
alter_integer (void) {
  small_number c; /* 0 for \.{\\deadcycles}, 1 for \.{\\insertpenalties}, etc. */
  c = cur_chr;
  scan_optional_equals();
  scan_int();
  if (c == 0) {
	dead_cycles = cur_val;
	/* begin expansion of Cases for |alter_integer| */
	/* module 1642 */
  } else if (c == 2) {
	if ((cur_val < batch_mode) || (cur_val > error_stop_mode)) {
	  print_err ("Bad interaction mode");
	  help2  ("Modes are 0=batch, 1=nonstop, 2=scroll, and",
			  "3=errorstop. Proceed, and I'll ignore this case.");
	  int_error (cur_val);
	} else {
	  cur_chr = cur_val;
	  new_interaction();
	};
  /* end expansion of Cases for |alter_integer| */
  } else {
	insert_penalties = cur_val;
  }
};


/* module 1392 */
void 
alter_box_dimen (void) {
  small_number c; /* |width_offset| or |height_offset| or |depth_offset| */
  pointer b; /* box register */ 
  c = cur_chr;
  scan_register_num();
  fetch_box (b);
  scan_optional_equals();
  scan_normal_dimen;
  if (b != null)
	mem[b + c].sc = cur_val;
};


/* module 1402 */
void 
new_font (small_number a) {
  pointer u; /* user's font identifier */ 
  scaled s; /* stated ``at'' size, or negative of scaled magnification */
  internal_font_number f; /* runs through existing fonts */ 
  str_number t; /* name for the frozen font identifier */ 
  unsigned char old_setting; /* holds |selector| setting */
  /* str_number  flushable_string;*/ /* string not yet referenced */ 
  if (jobname == 0)
	open_log_file();  /* avoid confusing \.{texput} with the font name */
  get_r_token();
  u = cur_cs;
  if (u >= hash_base) {
	t = text (u);
  } else if (u >= single_base) {
	if (u == null_cs) {
	  t = slow_make_tex_string("FONT");
	} else {
	  t = u - single_base;
	} 
  } else {
	old_setting = selector;
	selector = new_string;
	zprint_string("FONT");
	zprint (u - active_base);
	selector = old_setting;
	str_room (1);
	t = make_string();
  };
  DEFINE (u, set_font, null_font);
  scan_optional_equals();
  scan_file_name();
  /* begin expansion of Scan the font size specification */
  /* module 1403 */
  name_in_progress = true; /* this keeps |cur_name| from being changed */
  if (scan_keyword ("at")) {
	/* begin expansion of Put the positive `at' size into |s| */
	/* module 1404 */
	scan_normal_dimen;
	s = cur_val;
	if ((s <= 0) || (s >= 134217728)) {
	  print_err ("Improper `at' size (");
	  print_scaled (s);
	  zprint_string("pt), replaced by 10pt");
	  help2 ("I can only handle fonts at positive sizes that are",
			 "less than 2048pt, so I've changed what you said to 10pt.");
	  error();
	  s = 10 * unity;
	};
	/* end expansion of Put the positive `at' size into |s| */
  } else if (scan_keyword ("scaled")) {
	scan_int();
	s = -cur_val;
	if ((cur_val <= 0) || (cur_val > 32768)) {
	  print_err ("Illegal magnification has been changed to 1000");
	  help1  ("The magnification ratio must be between 1 and 32768.");
	  int_error (cur_val);
	  s = -1000;
	};
  } else {
	s = -1000;
  }
  name_in_progress = false;
  /* end expansion of Scan the font size specification */
  /* begin expansion of If this font has already been loaded, 
	 set |f| to the internal font number and |goto common_ending| */
  /* module 1405 */
  /* When the user gives a new identifier to a font that was previously loaded,
   * the new name becomes the font identifier of record. Font names `\.{xyz}' and
   * `\.{XYZ}' are considered to be different.
   */
  for (f = font_base + 1; f <= font_ptr; f++)
	if (str_eq_str (font_name[f], cur_name) && str_eq_str (font_area[f], cur_area)) {
	  if (pdf_font_step[f] == 0) {
		if (s > 0) {
		  if (s == font_size[f])
			goto COMMON_ENDING;
		} else if (font_size[f] == xn_over_d (font_dsize[f], -s, 1000)) {
		  goto COMMON_ENDING;
		}
	  }
	}
  /* end expansion of If this font has already been loaded, set |f| to the internal ... */
  f = read_font_info (u, cur_name, cur_area, s);
 COMMON_ENDING:
  equiv (u) = f;
  eqtb[font_id_base + f] = eqtb[u];
  font_id_text (f) = t;
};

/* module 1410 */
void 
new_interaction (void) {
  print_ln();
  interaction = cur_chr;
  if (interaction == batch_mode) {
	kpse_make_tex_discard_errors = 1;
  } else {
	kpse_make_tex_discard_errors = 0;
  }
  /* Initialize the print |selector| based on |interaction| */
  initialize_selector;
  if (log_opened)
	selector = selector + 2;
};



/* module 1362 */

/* Here's an example of the way many of the following routines operate.
 * (Unfortunately, they aren't all as simple as this.)
 */
void 
prefixed_command (void) {
  small_number a; /* accumulated prefix codes so far */
  internal_font_number f; /* identifies a font */ 
  halfword j; /* index into a \.{\\parshape} specification */
  font_index k;  /* index into |font_info| */ 
  pointer p, q; /* for temporary short-term use */ 
  int n; /* ditto */ 
  boolean e; /* should a definition be expanded? or was \.{\\let} not done? */
  a = 0;
  while (cur_cmd == prefix) {
	if (!odd (a / cur_chr))
	  a = a + cur_chr;
	/* Get the next non-blank non-relax non-call token */
	get_nblank_nrelax_ncall;
	if (cur_cmd <= max_non_prefixed_command) {
	  /* begin expansion of Discard erroneous prefixes and |return| */
	  /* module 1357 */
	  print_err ("You can't use a prefix with `");
	  print_cmd_chr (cur_cmd, cur_chr);
	  print_char ('\'');
	  help1  ("I'll pretend you didn't say \\long or \\outer or \\global.");
	  back_error();
	  return;
	};
	/* end expansion of Discard erroneous prefixes and |return| */
	if (tracing_commands > 2)
	  if (eTeX_ex)
		show_cur_cmd_chr();
  };
  /* begin expansion of Discard the prefixes \.{\\long} and \.{\\outer} if they are irrelevant */
  /* module 1358 */
  if (a >= 8) {
	j = protected_token;
	a = a - 8;
  } else {
	j = 0;
  }
  if ((cur_cmd != def) && ((a % 4 != 0) || (j != 0))) {
	print_err ("You can't use `");
	print_esc_string ("long");
	zprint_string("' or `");
	print_esc_string ("outer");
	zprint_string("' with `");
	print_cmd_chr (cur_cmd, cur_chr);
	print_char ('\'');
	help1  ("I'll pretend you didn't say \\long or \\outer here.");
	error();
  };
  /* end expansion of Discard the prefixes \.{\\long} and \.{\\outer} if they are irrelevant */
  /* begin expansion of Adjust \(f)for the setting of \.{\\globaldefs} */
  /* module 1359 */
  /* The previous routine does not have to adjust |a| so that |a mod 4=0|,
   * since the following routines test for the \.{\\global} prefix as follows.
   */
  if (global_defs != 0) {
	if (global_defs < 0) {
	  if (global)
		a = a - 4;
	} else {
	  if (!global)
		a = a + 4;
	}
  };
  /* end expansion of Adjust \(f)for the setting of \.{\\globaldefs} */
  switch (cur_cmd) {
  case set_font:
	DEFINE (cur_font_loc, data, cur_chr);
	break;
	/* module 1363 */
	/* When a |def| command has been scanned,
	 * |cur_chr| is odd if the definition is supposed to be global, and
	 * |cur_chr>=2| if the definition is supposed to be expanded.
	 */
  case def:
	{
	  if (odd (cur_chr) && !global &&(global_defs >= 0))
		a = a + 4;
	  e = (cur_chr >= 2);
	  get_r_token();
	  p = cur_cs;
	  q = scan_toks (true, e);
	  if (j != 0) {
		q = get_avail();
		info (q) = j;
		link (q) = link (def_ref);
		link (def_ref) = q;
	  };
	  DEFINE (p, call + (a % 4), def_ref);
	};
	break;
	/* module 1366 */
  case let:
	{
	  n = cur_chr;
	  get_r_token();
	  p = cur_cs;
	  if (n == normal) {
		do {
		  get_token();
		}  while (cur_cmd == spacer);
		if (cur_tok == other_token + '=') {
		  get_token();
		  if (cur_cmd == spacer)
			get_token();
		};
	  } else {
		get_token();
		q = cur_tok;
		get_token();
		back_input();
		cur_tok = q;
		back_input(); /* look ahead, then back up */ 
	  }; /* note that |back_input| doesn't affect |cur_cmd|, |cur_chr| */
	  if (cur_cmd >= call)
		add_token_ref (cur_chr);
	  DEFINE (p, cur_cmd, cur_chr);
	};
	break;
	/* module 1369 */
	/* We temporarily define |p| to be |relax|, so that an occurrence of |p|
	 * while scanning the definition will simply stop the scanning instead of
	 * producing an ``undefined control sequence'' error or expanding the
	 * previous meaning. This allows, for instance, `\.{\\chardef\\foo=123\\foo}'.
	 */
	case shorthand_def:
	  if (cur_chr == char_sub_def_code) {
		scan_char_num();
		p = char_sub_code_base + cur_val;
		scan_optional_equals();
		scan_char_num();
		n = cur_val; /* accent character in substitution */
		scan_char_num();
		if ((tracing_char_sub_def > 0)) {
		  begin_diagnostic();
		  print_nl_string("New character substitution: ");
		  print_ASCII (p - char_sub_code_base);
		  zprint_string(" = ");
		  print_ASCII (n);
		  print_char (' ');
		  print_ASCII (cur_val);
		  end_diagnostic (false);
		};
		n = n * 256 + cur_val;
		DEFINE (p, data, hi (n));
		if ((p - char_sub_code_base) < char_sub_def_min)
		  WORD_DEFINE (int_base + char_sub_def_min_code, p - char_sub_code_base);
		if ((p - char_sub_code_base) > char_sub_def_max)
		  WORD_DEFINE (int_base + char_sub_def_max_code,p - char_sub_code_base);
	  } else {
		n = cur_chr;
		get_r_token();
		p = cur_cs;
		DEFINE (p, relax, 256);
		scan_optional_equals();
		switch (n) {
		case char_def_code:
		  {
			scan_char_num();
			DEFINE (p, char_given, cur_val);
		  };
		  break;
		case math_char_def_code:
		  {
			scan_fifteen_bit_int();
			DEFINE (p, math_given, cur_val);
		  };
		  break;
		default:
		  {
			scan_register_num();
			if (cur_val > 255)
			  {
				j = n - count_def_code; /* |int_val..box_val| */ 
				if (j > mu_val)
				  j = tok_val; /* |int_val..mu_val| or |tok_val| */
				find_sa_element (j, cur_val, true);
				add_sa_ref (cur_ptr);
				if (j == tok_val) {
				  j = toks_register;
				} else {
				  j = register_cmd;
				}
				DEFINE (p, j, cur_ptr);
			  } else {
				switch (n) {
				case count_def_code:
				  DEFINE (p, assign_int, count_base + cur_val);
				  break;
				case dimen_def_code:
				  DEFINE (p, assign_dimen, scaled_base + cur_val);
				  break;
				case skip_def_code:
				  DEFINE (p, assign_glue,skip_base + cur_val);
				  break;
				case mu_skip_def_code:
				  DEFINE (p, assign_mu_glue, mu_skip_base + cur_val);
				  break;
				case toks_def_code:
				  DEFINE (p, assign_toks,toks_base + cur_val);
				};
			  };
			/* there are no other cases */ 
		  };
		};
	  };
	  break;
	  /* module 1370 */
  case read_to_cs:
	{
	  j = cur_chr;
	  scan_int();
	  n = cur_val;
	  if (!scan_keyword ("to")) {
		print_err ("Missing `to' inserted");
		help2 ("You should have said `\\read<number> to \\cs'.",
			   "I'm going to look for the \\cs now.");
		error();
	  };
	  get_r_token();
	  p = cur_cs;
	  read_toks (n, p, j);
	  DEFINE (p, call, cur_val);
	};
	break;
	/* module 1371 */
	/* The token-list parameters, \.{\\output} and \.{\\everypar}, etc., receive
	 * their values in the following way. (For safety's sake, we place an
	 * enclosing pair of braces around an \.{\\output} list.)
	 */
  case toks_register:
  case assign_toks:
	{
	  q = cur_cs;
	  e = false;  /* just in case, will be set |true| for sparse array elements */
	  if (cur_cmd == toks_register) {
		if (cur_chr == mem_bot) {
		  scan_register_num();
		  if (cur_val > 255) {
			find_sa_element (tok_val, cur_val, true);
			cur_chr = cur_ptr;
			e = true;
		  } else {
			cur_chr = toks_base + cur_val;
		  }
		} else {
		  e = true; 
		}
	  }
	  p = cur_chr; /* |p=every_par_loc| or |output_routine_loc| or \dots */
	  scan_optional_equals();
	  /* begin expansion of Get the next non-blank non-relax non-call token */
	  /* Get the next non-blank non-relax non-call token */
	  get_nblank_nrelax_ncall;
	  if (cur_cmd != left_brace)
		/* begin expansion of If the right-hand side is a token parameter or 
		   token register, finish the assignment and |goto done| */
		/* module 1372 */
		if ((cur_cmd == toks_register)|| (cur_cmd == assign_toks)){
		  if (cur_cmd == toks_register) {
			if (cur_chr == mem_bot){
			  scan_register_num();
			  if (cur_val < 256) {
				q = equiv (toks_base + cur_val);
			  } else {
				find_sa_element (tok_val, cur_val,false);
				if (cur_ptr == null) {
				  q = null;
				} else {
				  q = sa_ptr (cur_ptr);
				}
			  };
			} else {
			  q = sa_ptr (cur_ptr); }
		  }  else {
			q = equiv (cur_chr);
		  }
		  if (q == null) {
			sa_define (p, null) (p, undefined_cs, null);
		  } else {
			add_token_ref (q);
			sa_define (p, q) (p, call, q);
		  };
		  goto DONE;
		}
	  /* end expansion of If the right-hand side is a token parameter or ...*/
	  back_input();
	  cur_cs = q;
	  q = scan_toks (false, false);
	  if (link (def_ref) == null) {	/* empty list: revert to the default */
		sa_define (p, null) (p, undefined_cs, null);
		free_avail (def_ref);
	  } else {
		if ((p == output_routine_loc) && !e) {	/* enclose in curlies */
		  link (q) = get_avail();
		  q = link (q);
		  info (q) = right_brace_token + '}';
		  q = get_avail();
		  info (q) = left_brace_token + '{';
		  link (q) = link (def_ref);
		  link (def_ref) = q;
		};
		sa_define (p, def_ref) (p, call, def_ref);
	  };
	};
	break;
	/* module 1373 */
	/* Similar routines are used to assign values to the numeric parameters.
	 */
  case assign_int:
	{
	  p = cur_chr;
	  scan_optional_equals();
	  scan_int();
	  WORD_DEFINE (p, cur_val);
	};
	break;
  case assign_dimen:
	{
	  p = cur_chr;
	  scan_optional_equals();
	  scan_normal_dimen;
	  WORD_DEFINE (p, cur_val);
	};
	break;
  case assign_glue:
  case assign_mu_glue:
	{
	  p = cur_chr;
	  n = cur_cmd;
	  scan_optional_equals();
	  if (n == assign_mu_glue) {
		scan_glue (mu_val);
	  } else {
		scan_glue (glue_val);
	  }
	  trap_zero_glue();
	  DEFINE (p, glue_ref, cur_val);
	};
	break;
	/* module 1377 */
	/* The different types of code values have different legal ranges; the
	 * following program is careful to check each case properly.
	 */
  case def_code:
	  /* begin expansion of Let |n| be the largest legal code value, based on |cur_chr| */
	  /* module 1378 */
	  if (cur_chr == cat_code_base) {
		n = max_char_code;
	  } else if (cur_chr == math_code_base) {
		n = 32768;
	  } else if (cur_chr == sf_code_base) {
		n = 32767;
	  } else if (cur_chr == del_code_base) {
		n = 16777215;
	  } else {
		n = 255;
	  };
	  /* end expansion of Let |n| be the largest legal code value, based on |cur_chr| */
	  p = cur_chr;
	  scan_char_num();
	  p = p + cur_val;
	  scan_optional_equals();
	  scan_int();
	  if (((cur_val < 0) && (p < del_code_base)) || (cur_val > n)) {
		print_err ("Invalid code (");
		print_int (cur_val);
		if (p < del_code_base) {
		  zprint_string("), should be in the range 0..");
		} else {
		  zprint_string("), should be at most ");
		}
		print_int (n);
		help1  ("I'm going to use 0 instead of that illegal code value.");
		error();
		cur_val = 0;
	  };
	  if (p < math_code_base) {
		DEFINE (p, data, cur_val);
	  } else if (p < del_code_base) {
		DEFINE (p, data, hi (cur_val));
	  } else {
		WORD_DEFINE (p, cur_val);
	  }
	  break;
	  /* module 1379 */
  case def_family:
	p = cur_chr;
	scan_four_bit_int();
	p = p + cur_val;
	scan_optional_equals();
	scan_font_ident();
	DEFINE (p, data, cur_val);
	break;
	/* module 1380 */
	/* Next we consider changes to \TeX's numeric registers. */
  case register_cmd:
  case advance:
  case multiply:
  case divide:
	do_register_command (a);
	break;
	/* module 1386 */
	/* The processing of boxes is somewhat different, because we may need
	 * to scan and create an entire box before we actually change the value of the old
	 * one.
	 */
  case set_box:
	scan_register_num();
	if (global) {
	  n = global_box_flag + cur_val;
	} else {
	  n = box_flag + cur_val;
	}
	scan_optional_equals();
	if (set_box_allowed) {
	  scan_box (n);
	} else {
	  print_err ("Improper ");
	  print_esc_string ("setbox");
	  help2  ("Sorry, \\setbox is not allowed after \\halign in a display,",
			  "or between \\accent and an accented character.");
	  error();
	};
	break;
	/* module 1387 */
	/* The |space_factor| or |prev_depth| settings are changed when a |set_aux|
	 * command is sensed. Similarly, |prev_graf| is changed in the presence of
	 * |set_prev_graf|, and |dead_cycles| or |insert_penalties| in the presence of
	 * |set_page_int|. These definitions are always global.
	 * 
	 * When some dimension of a box register is changed, the change isn't exactly
	 * global; but \TeX\ does not look at the \.{\\global} switch.
	 */
  case set_aux:
	alter_aux();
	break;
  case set_prev_graf:
	alter_prev_graf();
	break;
  case set_page_dimen:
	alter_page_so_far();
	break;
  case set_page_int:
	alter_integer();
	break;
  case set_box_dimen:
	alter_box_dimen();
	break;
	/* module 1393 */
	/* Paragraph shapes are set up in the obvious way. */
  case set_shape:
	{
	  q = cur_chr;
	  scan_optional_equals();
	  scan_int();
	  n = cur_val;
	  if (n <= 0) {
		p = null;
	  } else if (q > par_shape_loc) {
		n = (cur_val / 2) + 1;
		p = get_node (2 * n + 1);
		info (p) = n;
		n = cur_val;
		mem[p + 1].cint = n;
		/* number of penalties */ 
		for (j = p + 2; j <= p + n + 1; j++) {
		  scan_int();
		  mem[j].cint = cur_val; /* penalty values */ 
		};
		if (!odd (n))
		  mem[p + n + 2].cint = 0; /* unused */ 
	  } else {
		p = get_node (2 * n + 1);
		info (p) = n;
		for (j = 1; j <= n; j++) {
		  scan_normal_dimen;
		  mem[p + 2 * j - 1].sc = cur_val; /* indentation */ 
		  scan_normal_dimen;
		  mem[p + 2 * j].sc = cur_val; /* width */ 
		};
	  };
	  DEFINE (q, shape_ref, p);
	};
	break;
	/* module 1397 */
	case hyph_data:
	  if (cur_chr == 1) {
		if (ini_version) {
		  new_patterns();
		  goto DONE;
		}
		print_err ("Patterns can be loaded only by INITEX");
		help0;
		error();
		do {
		  get_token();
		} while (cur_cmd != right_brace); /* flush the patterns */ 
		do_something;
		return;
	  } else {
		new_hyph_exceptions();
		goto DONE;
	  };
	  /* module 1398 */
	  /* All of \TeX's parameters are kept in |eqtb| except the font information,
	   * the interaction mode, and the hyphenation tables; these are strictly global.
	   */
  case assign_font_dimen:
	{
	  find_font_dimen (true);
	  k = cur_val;
	  scan_optional_equals();
	  scan_normal_dimen;
	  font_info[k].sc = cur_val;
	};
	break;
  case assign_font_int:
	{
	  n = cur_chr;
	  scan_font_ident();
	  f = cur_val;
	  if (n < lp_code_base) {
		scan_optional_equals();
		scan_int();
		if (n == 0) {
		  hyphen_char[f] = cur_val;
		} else {
		  skew_char[f] = cur_val;
		}
	  }	else {
		scan_char_num();
		p = cur_val;
		scan_optional_equals();
		scan_int();
		switch (n) {
		case lp_code_base:
		  set_lp_code (f, p, cur_val);
		  break;
		case rp_code_base:
		  set_rp_code (f, p, cur_val);
		  break;
		case ef_code_base:
		  set_ef_code (f, p, cur_val);
		  ;
		};
	  };
	};
	break;
	/* module 1401 */
	/* Here is where the information for a new font gets loaded. */
  case def_font:
	new_font (a);
	break;
	/* module 1409 */
  case set_interaction:
	new_interaction();
	break;
  default:
	confusion ("prefix");
  };
 DONE:
  /* begin expansion of Insert a token saved by \.{\\afterassignment}, if any */
  /* module 1414 */
  if (after_token != 0) {
	cur_tok = after_token;
	back_input();
	after_token = 0;
  };
  /* end expansion of Insert a token saved by \.{\\afterassignment}, if any */
}

/* module 1411 */

/* The \.{\\afterassignment} command puts a token into the global
 * variable |after_token|. This global variable is examined just after
 * every assignment has been performed.
 */
halfword after_token; /* zero, or a saved token */

/* module 1412 */

void
prefix_initialize (void) {
  after_token = 0;
}
