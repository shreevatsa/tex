

#include "types.h"
#include "c-compat.h"
#include "globals.h"

/* forward: */
void just_copy (pointer, pointer, pointer);

/* module 1297 */
/* 
 * An active character that is an |outer_call| is allowed here.
 */
#define treat_as_active	{         \
  cur_cs = cur_chr + active_base; \
  cur_cmd = eq_type (cur_cs);     \
  cur_chr = equiv (cur_cs);       \
  x_token();                      \
  back_input(); }


/* module 1676 */

/* Some special actions are required for displayed equation in paragraphs
 * with mixed direction texts. First of all we have to set the text
 * direction preceding the display.
 */
/* Set the value of |x|... */
#define set_the_value_of_x  if (LR_save == null) {\
			    x = 0;\
			    } else\
			      if (info (LR_save) >= R_code) {\
				x = -1;\
			    } else\
			    x = 1

/* module 1687 */
/* Retrieve the prototype box */
#define get_prototype_box \
  if (mode == mmode)      \
	j = LR_box


/* module 1192 */
void 
insert_dollar_sign (void) {
  back_input();
  cur_tok = math_shift_token + '$';
  print_err ("Missing $ inserted");
  help2 ("I've inserted a begin-math/end-math symbol since I think",
		 "you left one out. Proceed, with fingers crossed.");
  ins_error();
};


/* module  1281 */

/* 
 * The routines that \TeX\ uses to create mlists are similar to those we have
 * just seen for the generation of hlists and vlists. But it is necessary to
 * make ``noads'' as well as nodes, so the reader should review the
 * discussion of math mode data structures before trying to make sense out of
 * the following program.
 * 
 * Here is a little routine that needs to be done whenever a subformula
 * is about to be processed. The parameter is a code like |math_group|.
 */
void 
push_math (group_code c) {
  push_nest();
  mode = -mmode;
  incompleat_noad = null;
  new_save_level (c);
};

/* module 1284 */
/* Go into ordinary math mode */
#define start_normal_math {                          \
  push_math (math_shift_group);                      \
  eq_word_define (int_base + cur_fam_code, -1);      \
  if (every_math != null)                            \
	begin_token_list (every_math, every_math_text);  \
  }


/* module 1283 */

void 
init_math (void) {
  scaled w; /* new or partial |pre_display_size| */ 
  pointer j; /* prototype box for display */ 
  int x; /* new |pre_display_direction| */ 
  scaled l; /* new |display_width| */ 
  scaled s; /* new |display_indent| */ 
  pointer p; /* current node when calculating |pre_display_size| */
  pointer q; /* glue specification when calculating |pre_display_size| */
  internal_font_number f; /* font in current |char_node| */ 
  int n; /* scope of paragraph shape specification */ 
  scaled v; /* |w| plus possible glue amount */ 
  scaled d; /* increment to |v| */ 
  get_token(); /* |get_x_token| would fail on \.{\\ifmmode}\thinspace! */
  if ((cur_cmd == math_shift) && (mode > 0)) {
	/* begin expansion of Go into display math mode */
	/* module 1290 */
	/* When we enter display math mode, we need to call |line_break| to
	 * process the partial paragraph that has just been interrupted by the
	 * display. Then we can set the proper values of |display_width| and
	 * |display_indent| and |pre_display_size|.
	 */
	j = null;
	w = -max_dimen;
	if (head == tail) {	/* `\.{\\noindent\$\$}' or `\.{\$\${ }\$\$}' */
	  /* begin expansion of Prepare for display after an empty paragraph */
	  /* module 1677 */
	  pop_nest();
	  /* Set the value of |x|... */
	  set_the_value_of_x;
	  /* end expansion of Prepare for display after an empty paragraph */
	} else {
	  line_break (true);
	  /* begin expansion of Calculate the natural width, |w|, by which the characters 
		 of the final line extend to the right of the reference point, plus two ems; or 
		 set |w:=max_dimen| if the non-blank information on that line is affected
		 by stretching or shrinking */
	  /* module 1291 */
	  /* begin expansion of Prepare for display after a non-empty paragraph */
	  /* module 1679 */
	  /* When the final line ends with R-text, the value |w| refers to the line
	   * reflected with respect to the left edge of the enclosing vertical list.
	   */
	  if (eTeX_ex) {
		/* begin expansion of Let |j| be the prototype box for the display */
		/* module 1685 */
		/* The prototype box is an hlist node with the width, glue set, and shift
		 * amount of |just_box|, i.e., the last line preceding the display. Its
		 * hlist reflects the current \.{\\leftskip} and \.{\\rightskip}.
		 */
		if (right_skip == zero_glue) {
		  j = new_kern (0);
		} else {
		  j = new_param_glue (right_skip_code);
		}
		if (left_skip == zero_glue) {
		  p = new_kern (0);
		} else {
		  p = new_param_glue (left_skip_code);
		}
		link (p) = j;
		j = new_null_box();
		width (j) = width (just_box);
		shift_amount (j) = shift_amount (just_box);
		list_ptr (j) = p;
		glue_order (j) = glue_order (just_box);
		glue_sign (j) = glue_sign (just_box);
		glue_set (j) = glue_set (just_box);
	  };
	  /* end expansion of Let |j| be the prototype box for the display */
	  v = shift_amount (just_box);
	  /* Set the value of |x|... */
	  set_the_value_of_x;
	  if (x >= 0) {
		p = list_ptr (just_box);
		link (temp_head) = null;
	  } else {
		v = -v - width (just_box);
		p = new_math (0, begin_L_code);
		link (temp_head) = p;
		just_copy (list_ptr (just_box), p, new_math (0, end_L_code));
		cur_dir = right_to_left;
	  };
	  v = v + 2 * quad (cur_font);
	  if (TeXXeT_en)
		/* Initialize the LR stack */
		initialize_LR_stack;
	  /* end expansion of Prepare for display after a non-empty paragraph */
	  while (p != null) {
		/* begin expansion of Let |d| be the natural width of node |p|; if
		   the node is ``visible,'' |goto found|; if the node is glue that stretches 
		   or shrinks, set |v:=max_dimen| */
		/* module 1292 */
	  RESWITCH:
		if (is_char_node (p)) {
		  f = font (p);
		  d = char_width (f, char_info (f, character (p)));
		  goto FOUND;
		};
		switch (type (p)) {
		case hlist_node:
		case vlist_node:
		case rule_node:
		  {
			d = width (p);
			goto FOUND;
		  };
		  break;
		case ligature_node:
		  /* Make node |p| look like a |char_node| and |goto reswitch| */
		  p = mimic_char_node (p);
		  goto RESWITCH;
		case kern_node:
		  d = width (p);
		  break;
		  /* begin expansion of Cases of `Let |d| be the natural width' 
			 that need special treatment */
		  /* module 1681 */
		  /* In the presence of text direction directives we assume that any LR
		   * problems have been fixed by the |hpack| routine. If the final line
		   * contains, however, text direction directives while \TeXXeT\ is disabled,
		   * then we set |w:=max_dimen|.
		   */
		  case math_node:
			{
			  d = width (p);
			  if (TeXXeT_en) {
				/* begin expansion of Adjust \(t)the LR stack for the |init_math| routine */
				/* module 1682 */
				if (end_LR (p)) {
				  if (info (LR_ptr) == end_LR_type (p)) {
					pop_LR;
				  } else if (subtype (p) > L_code) {
					w = max_dimen;
					goto DONE;
				  }
				} else {
				  push_LR (p);
				  if (LR_dir (p) != cur_dir) {
					just_reverse (p);
					p = temp_head;
				  };
				}
			  /* end expansion of Adjust \(t)the LR stack for the |init_math| routine */
			  } else if (subtype (p) >= L_code) {
				w = max_dimen;
				goto DONE;
			  };
			};
			do_something;
			break;
		case edge_node:
		  {
			d = width (p);
			cur_dir = subtype (p);
		  };
		  break;
		  /* end expansion of Cases of `Let |d| be the natural width' that need special treatment */
		case margin_kern_node:
		  d = width (p);
		  break;
		case glue_node:
		  /* begin expansion of Let |d| be the natural width of this glue; if stretching or
			 shrinking, set |v:=max_dimen|; |goto found| in the case of leaders */
		  /* module 1293 */
		  /* We need to be careful that |w|, |v|, and |d| do not depend on any |glue_set|
		   * values, since such values are subject to system-dependent rounding.
		   * System-dependent numbers are not allowed to infiltrate parameters like
		   * |pre_display_size|, since \TeX82 is supposed to make the same decisions on all
		   * machines.
		   */
		  {
			q = glue_ptr (p);
			d = width (q);
			if (glue_sign (just_box) == stretching) {
			  if ((glue_order (just_box) == stretch_order (q))  && (stretch (q) != 0))
				v = max_dimen;
			} else if (glue_sign (just_box) == shrinking) {
			  if ((glue_order (just_box) == shrink_order (q))  && (shrink (q) != 0))
				v = max_dimen;
			};
			if (subtype (p) >= a_leaders)
			  goto FOUND;
		  }
		  /* end expansion of Let |d| be the natural width of this glue;  ...*/
		  do_something;
		  break;
		case whatsit_node:
		  /* begin expansion of Let |d| be the width of the whatsit |p| */
		  /* module 1557 */
		  if ((subtype (p) == pdf_refxform_node) || (subtype (p) == pdf_refximage_node)) {
			d = pdf_width (p);
		  } else {
			d = 0;
		  }
		  /* end expansion of Let |d| be the width of the whatsit |p| */
		  break;
		default:
		  d = 0;
		}
		/* end expansion of Let |d| be the natural width of node |p|;... */
		if (v < max_dimen)
		  v = v + d;
		goto NOT_FOUND;
	  FOUND:
		if (v < max_dimen) {
		  v = v + d;
		  w = v;
		}  else {
		  w = max_dimen;
		  goto DONE;
		};
	  NOT_FOUND:
		p = link (p);
	  };
	DONE:
	  /* begin expansion of Finish the natural width computation */
	  /* module 1680 */
	  if (TeXXeT_en) {
		while (LR_ptr != null)
		  pop_LR;
		if (LR_problems != 0) {
		  w = max_dimen;
		  LR_problems = 0;
		};
	  };
	  cur_dir = left_to_right;
	  flush_node_list (link (temp_head));
	  /* end expansion of Finish the natural width computation */
	  /* end expansion of Calculate the natural width, |w|, by which th ...*/
	};
	/* now we are in vertical mode, working on the list that will contain the display */
	/* begin expansion of Calculate the length, |l|, and the shift amount, |s|, of the display lines */
	/* module 1294 */
	/* A displayed equation is considered to be three lines long, so we
	 * calculate the length and offset of line number |prev_graf+2|.
	 */
	if (par_shape_ptr == null) {
	  if ((hang_indent != 0)&&(((hang_after >= 0) && (prev_graf + 2 > hang_after))
							   || (prev_graf + 1 < -hang_after))) {
		l = hsize - abs (hang_indent);
		if (hang_indent > 0) {
		  s = hang_indent;
		} else {
		  s = 0;
		}
	  } else {
		l = hsize;
		s = 0;
	  }
	} else {
	  n = info (par_shape_ptr);
	  if (prev_graf + 2 >= n) {
		p = par_shape_ptr + 2 * n;
	  } else {
		p = par_shape_ptr + 2 * (prev_graf + 2);
	  };
	  s = mem[p - 1].sc;
	  l = mem[p].sc;
	  /* end expansion of Calculate the length, |l|, and the shift amount, |s|, ...*/
	  push_math (math_shift_group);
	  mode = mmode;
	  eq_word_define (int_base + cur_fam_code, -1);
	  eq_word_define (dimen_base + pre_display_size_code, w);
	  LR_box = j;
	  if (eTeX_ex)
		eq_word_define (int_base + pre_display_direction_code, x);
	  eq_word_define (dimen_base + display_width_code, l);
	  eq_word_define (dimen_base + display_indent_code, s);
	  if (every_display != null)
		begin_token_list (every_display, every_display_text);
	  if (nest_ptr == 1)
		build_page();
	  /* end expansion of Go into display math mode */
	}
  } else  {
	back_input();
	/* Go into ordinary math mode */
    start_normal_math;
  };
};


/* module 1287 */
/* When \TeX\ is in display math mode, |cur_group=math_shift_group|,
 * so it is not necessary for the |start_eq_no| procedure to test for
 * this condition.
 */

void 
start_eq_no (void) {
  saved (0) = cur_chr;
  incr (save_ptr);
  /* Go into ordinary math mode */
  start_normal_math;
}


/* module 1296 */

/* Recall that the |nucleus|, |subscr|, and |supscr| fields in a noad are
 * broken down into subfields called |math_type| and either |info| or
 * |(fam,character)|. The job of |scan_math| is to figure out what to place
 * in one of these principal fields; it looks at the subformula that
 * comes next in the input, and places an encoding of that subformula
 * into a given word of |mem|.
 */

void 
scan_math (pointer p) {
  int c; /* math character code */ 
 RESTART:
  /* Get the next non-blank non-relax non-call token */
  get_nblank_nrelax_ncall;
 RESWITCH:
  switch (cur_cmd) {
  case letter:
  case other_char:
  case char_given:
	c = ho (math_code (cur_chr));
	if (c == 32768) {
	  /* Treat |cur_chr| as an active character */
	  treat_as_active;
	  goto RESTART;
	};
	do_something;
	break;
  case char_num:
	scan_char_num();
	cur_chr = cur_val;
	cur_cmd = char_given;
	goto RESWITCH;
  case math_char_num:
	scan_fifteen_bit_int();
	c = cur_val;
	break;
  case math_given:
	c = cur_chr;
	break;
  case delim_num:
	scan_twenty_seven_bit_int();
	c = cur_val / 4096;
	break;
  default:
	/* begin expansion of Scan a subformula enclosed in braces and |return| */
	/* module 1298 */
	/* The pointer |p| is placed on |save_stack| while a complex subformula
	 * is being scanned.
	 */
	back_input();
	scan_left_brace();
	saved (0) = p;
	incr (save_ptr);
	push_math (math_group);
	return;
	/* end expansion of Scan a subformula enclosed in braces and |return| */
  };
  math_type (p) = math_char;
  character (p) = qi (c % 256);
  if ((c >= var_code) && fam_in_range) {
	fam (p) = cur_fam;
  } else {
	fam (p) = (c / 256) % 16;
  }
}

/* module  1300 */

/* The |set_math_char| procedure creates a new noad appropriate to a given
 * math code, and appends it to the current mlist. However, if the math code
 * is sufficiently large, the |cur_chr| is treated as an active character and
 * nothing is appended.
 */
void 
set_math_char (int c) {
  pointer p; /* the new noad */ 
  if (c >= 32768) {
	/* Treat |cur_chr|... */
	treat_as_active; 
  } else {
	p = new_noad();
	math_type (nucleus (p)) = math_char;
	character (nucleus (p)) = qi (c % 256);
	fam (nucleus (p)) = (c / 256) % 16;
	if (c >= var_code) {
	  if (fam_in_range)
		fam (nucleus (p)) = cur_fam;
	  type (p) = ord_noad;
	} else {
	  type (p) = ord_noad + (c / 4096);
	}
	link (tail) = p;
	tail = p;
  };
};

/* module 1304 */
void 
math_limit_switch (void) {
  if (head != tail)
	if (type (tail) == op_noad) {
	  subtype (tail) = cur_chr;
	  return;
	};
  print_err  ("Limit controls must follow a math operator");
  help1("I'm ignoring this misplaced \\limits or \\nolimits command.");
  error();
};


/* module 1305 */

/* Delimiter fields of noads are filled in by the |scan_delimiter| routine.
 * The first parameter of this procedure is the |mem| address where the
 * delimiter is to be placed; the second tells if this delimiter follows
 * \.{\\radical} or not.
 */
void 
scan_delimiter (pointer p, boolean r) {
  if (r) {
	scan_twenty_seven_bit_int();
  } else {
	/* Get the next non-blank non-relax non-call token */
	get_nblank_nrelax_ncall;
	switch (cur_cmd) {
	case letter:
	case other_char:
	  cur_val = del_code (cur_chr);
	  break;
	case delim_num:
	  scan_twenty_seven_bit_int();
	  break;
	default:
	  cur_val = -1;
	};
  };
  if (cur_val < 0) {
	/* begin expansion of Report that an invalid delimiter code 
	   is being changed to null; set~|cur_val:=0| */
	/* module 1306 */
	print_err ("Missing delimiter (. inserted)");
	help6 ("I was expecting to see something like `(' or `\\{' or",
		   "`\\}' here. If you typed, e.g., `{' instead of `\\{', you",
		   "should probably delete the `{' by typing `1' now, so that",
		   "braces don't get unbalanced. Otherwise just proceed.",
		   "Acceptable delimiters are characters whose \\delcode is",
		   "nonnegative, or you can use `\\delimiter <delimiter code>'.");
	back_error();
	cur_val = 0;
  }
  /* end expansion of Report that an invalid delimiter code is being ... */
  small_fam (p) = (cur_val / 1048576) % 16;
  small_char (p) = qi ((cur_val / 4096) % 256);
  large_fam (p) = (cur_val / 256) % 16;
  large_char (p) = qi (cur_val % 256);
}


/* module 1308 */
void 
math_radical (void) {
  tail_append (get_node (radical_noad_size));
  type (tail) = radical_noad;
  subtype (tail) = normal;
  mem[nucleus (tail)].hh = empty_field;
  mem[subscr (tail)].hh = empty_field;
  mem[supscr (tail)].hh = empty_field;
  scan_delimiter (left_delimiter (tail), true);
  scan_math (nucleus (tail));
}


/* module */
void 
math_ac (void) {
  if (cur_cmd == accent) {
	/* begin expansion of Complain that the user should have said \.{\\mathaccent} */
	/* module 1311 */
	print_err ("Please use ");
	print_esc_string ("mathaccent");
	zprint_string(" for accents in math mode");
	help2 ("I'm changing \\accent to \\mathaccent here; wish me luck.",
		   "(Accents are not the same in formulas as they are in text.)");
	error();
	/* end expansion of Complain that the user should have said \.{\\mathaccent} */
  };
  tail_append (get_node (accent_noad_size));
  type (tail) = accent_noad;
  subtype (tail) = normal;
  mem[nucleus (tail)].hh = empty_field;
  mem[subscr (tail)].hh = empty_field;
  mem[supscr (tail)].hh = empty_field;
  math_type (accent_chr (tail)) = math_char;
  scan_fifteen_bit_int();
  character (accent_chr (tail)) = qi (cur_val % 256);
  if ((cur_val >= var_code) && fam_in_range) {
	fam (accent_chr (tail)) = cur_fam;
  } else {
	fam (accent_chr (tail)) = (cur_val / 256) % 16;
  }
  scan_math (nucleus (tail));
};


/* module 1317 */

/* The routine that scans the four mlists of a \.{\\mathchoice} is very
 * much like the routine that builds discretionary nodes.
 */

void 
append_choices (void) {
  tail_append (new_choice());
  incr (save_ptr);
  saved (-1) = 0;
  push_math (math_choice_group);
  scan_left_brace();
};



/* module 1329 */

/* At the end of a math formula or subformula, the |fin_mlist| routine is
 * called upon to return a pointer to the newly completed mlist, and to
 * pop the nest back to the enclosing semantic level. The parameter to
 * |fin_mlist|, if not null, points to a |right_noad| that ends the
 * current mlist; this |right_noad| has not yet been appended.
 */

pointer 
fin_mlist (pointer p) {
  pointer q; /* the mlist to return */ 
  if (incompleat_noad != null) {
	/* begin expansion of Compleat the incompleat noad */
	/* module 1330 */
	math_type (denominator (incompleat_noad)) = sub_mlist;
	info (denominator (incompleat_noad)) = link (head);
	if (p == null) {
	  q = incompleat_noad;
	} else {
	  q = info (numerator (incompleat_noad));
	  if ((type (q) != left_noad) || (delim_ptr == null))
		confusion ("right");
	  info (numerator (incompleat_noad)) = link (delim_ptr);
	  link (delim_ptr) = incompleat_noad;
	  link (incompleat_noad) = p;
	};
	/* end expansion of Compleat the incompleat noad */
  } else {
	link (tail) = p;
	q = link (head);
  };
  pop_nest();
  return q;
}


/* module 1319 */

void 
build_choices (void) {
  pointer p; /* the current mlist */ 
  unsave();
  p = fin_mlist (null);
  switch (saved (-1)) {
  case 0: display_mlist (tail) = p;
	break;
  case 1: text_mlist (tail) = p;
	break;
  case 2: script_mlist (tail) = p;
	break;
  case 3:
	script_script_mlist (tail) = p;
	decr (save_ptr);
	return;
  }; /* there are no other cases */ 
  incr (saved (-1));
  push_math (math_choice_group);
  scan_left_brace();
};

/* module 1321 */
void 
sub_sup (void) {
  small_number t; /* type of previous sub/superscript */ 
  pointer p; /* field to be filled by |scan_math| */ 
  t = empty;
  p = null;
  if (tail != head)
	if (scripts_allowed (tail)) {
	  p = supscr (tail) + cur_cmd - sup_mark; /* |supscr| or |subscr| */ 
	  t = math_type (p);
	};
  if ((p == null) || (t != empty)) {
	/* begin expansion of Insert a dummy noad to be sub/superscripted */
	/* module 1322 */
	tail_append (new_noad());
	p = supscr (tail) + cur_cmd - sup_mark; /* |supscr| or |subscr| */ 
	if (t != empty) {
	  if (cur_cmd == sup_mark) {
		print_err ("Double superscript");
		help1  ("I treat `x^1^2' essentially like `x^1{}^2'.");
	  } else {
		print_err ("Double subscript");
		help1  ("I treat `x_1_2' essentially like `x_1{}_2'.");
	  };
	  error();
	};
  };
  /* end expansion of Insert a dummy noad to be sub/superscripted */
  scan_math (p);
};


/* module 1326 */
void 
math_fraction (void) {
  small_number c; /* the type of generalized fraction we are scanning */ 
  c = cur_chr;
  if (incompleat_noad != null) {
	/* begin expansion of Ignore the fraction operation and complain about this ambiguous case */
	/* module 1328 */
	if (c >= delimited_code) {
	  scan_delimiter (garbage, false);
	  scan_delimiter (garbage, false);
	};
	if (c % delimited_code == above_code)
	  scan_normal_dimen;
	print_err ("Ambiguous; you need another { and }");
	help3 ("I'm ignoring this fraction specification, since I don't",
		   "know whether a construction like `x \\over y \\over z'",
		   "means `{x \\over y} \\over z' or `x \\over {y \\over z}'.");
	error();
	/* end expansion of Ignore the fraction operation and complain about this ambiguous case */
  }  else {
	incompleat_noad = get_node (fraction_noad_size);
	type (incompleat_noad) = fraction_noad;
	subtype (incompleat_noad) = normal;
	math_type (numerator (incompleat_noad)) = sub_mlist;
	info (numerator (incompleat_noad)) = link (head);
	mem[denominator (incompleat_noad)].hh = empty_field;
	mem[left_delimiter (incompleat_noad)].qqqq = null_delimiter;
	mem[right_delimiter (incompleat_noad)].qqqq =null_delimiter;
	link (head) = null;
	tail = head;
	/* begin expansion of Use code |c| to distinguish between generalized fractions */
	/* module 1327 */
	if (c >= delimited_code) {
	  scan_delimiter (left_delimiter (incompleat_noad), false);
	  scan_delimiter (right_delimiter (incompleat_noad), false);
	};
	switch (c % delimited_code) {
	case above_code: 
	  scan_normal_dimen;
	  thickness (incompleat_noad) = cur_val;
	  break;
	case over_code:
	  thickness (incompleat_noad) = default_code;
	  break;
	case atop_code:
	  thickness (incompleat_noad) = 0;
	}	/* there are no other cases */
	/* end expansion of Use code |c| to distinguish between generalized fractions */
  };
};


/* module 1336 */
void 
math_left_right (void) {
  small_number t; /* |left_noad| or |right_noad| */ 
  pointer p; /* new noad */ 
  pointer q; /* resulting mlist */ 
  t = cur_chr;
  if ((t != left_noad) && (cur_group != math_left_group)) {
	/* begin expansion of Try to recover from mismatched \.{\\right} */
	/* module 1337 */
	if (cur_group == math_shift_group) {
	  scan_delimiter (garbage, false);
	  print_err ("Extra ");
	  if (t == middle_noad) {
		print_esc_string ("middle");
		help1  ("I'm ignoring a \\middle that had no matching \\left.");
	  } else {
		print_esc_string ("right");
		help1  ("I'm ignoring a \\right that had no matching \\left.");
	  };
	  error();
	} else {
	  off_save();
	}
	/* end expansion of Try to recover from mismatched \.{\\right} */
  } else {
	p = new_noad();
	type (p) = t;
	scan_delimiter (delimiter (p), false);
	if (t == middle_noad) {
	  type (p) = right_noad;
	  subtype (p) = middle_noad;
	};
	if (t == left_noad) {
	  q = p;
	} else {
	  q = fin_mlist (p);
	  unsave(); /* end of |math_left_group| */ 
	};
	if (t != right_noad) {
	  push_math (math_left_group);
	  link (head) = q;
	  tail = p;
	  delim_ptr = p;
	}  else  {
	  tail_append (new_noad());
	  type (tail) = inner_noad;
	  math_type (nucleus (tail)) = sub_mlist;
	  info (nucleus (tail)) = q;
	};
  };
};


/* module 1340 */
#define check_math_fonts                                                                \
  if ((font_params[fam_fnt (2 + text_size)] < total_mathsy_params)                      \
	  || (font_params[fam_fnt (2 + script_size)] < total_mathsy_params)                 \
	  || (font_params[fam_fnt (2 + script_script_size)] < total_mathsy_params)) {       \
	print_err ("Math formula deleted: Insufficient symbol fonts");                      \
	help3 ("Sorry, but I can't typeset math unless \\textfont 2",                       \
		   "and \\scriptfont 2 and \\scriptscriptfont 2 have all",                      \
		   "the \\fontdimen values needed in math symbol fonts.");                      \
	error();                                                                            \
	flush_math();                                                                       \
	danger = true;                                                                      \
  } else if ((font_params[fam_fnt (3 + text_size)] < total_mathex_params)               \
			 || (font_params[fam_fnt (3 + script_size)] <  total_mathex_params)         \
			 || (font_params[fam_fnt (3 + script_script_size)] < total_mathex_params)){ \
	print_err ("Math formula deleted: Insufficient extension fonts");                   \
	help3 ("Sorry, but I can't typeset math unless \\textfont 3",                       \
		   "and \\scriptfont 3 and \\scriptscriptfont 3 have all",                      \
		   "the \\fontdimen values needed in math extension fonts.");                   \
	error();                                                                            \
	flush_math();                                                                       \
	danger = true;                                                                      \
  }


/* module 1339 */

void 
after_math (void) {
  boolean l; /* `\.{\\leqno}' instead of `\.{\\eqno}' */ 
  boolean danger; /* not enough symbol fonts are present */ 
  int m; /* |mmode| or |-mmode| */ 
  pointer p; /* the formula */ 
  pointer a; /* box containing equation number */
  /* begin expansion of Local variables for finishing a displayed formula */
  /* module 1343 */
  /* We have saved the worst for last: The fussiest part of math mode processing
   * occurs when a displayed formula is being centered and placed with an optional
   * equation number.
   */
  pointer b; /* box containing the equation */ 
  scaled w; /* width of the equation */ 
  scaled z; /* width of the line */ 
  scaled e; /* width of equation number */ 
  scaled q; /* width of equation number plus space to separate from equation */
  scaled d; /* displacement of equation in the line */ 
  scaled s; /* move the line right this much */ 
  small_number g1, g2; /* glue parameter codes for before and after */ 
  pointer r; /* kern node used to position the display */ 
  pointer t; /* tail of adjustment list */ 
  pointer pre_t; /* tail of pre-adjustment list */
  /* module 1686 */
  /* At the end of a displayed equation we retrieve the prototype box. */
  pointer j; /* prototype box */
  j =null; /*TH -Wall*/
  /* end expansion of Local variables for finishing a displayed formula */
  danger = false;
  /* Retrieve the prototype box */
  get_prototype_box;
  /* Check that the necessary fonts for math symbols are present; 
	 if not, flush the current math lists and set |danger:=true| */
  check_math_fonts;
  m = mode;
  l = false;
  p = fin_mlist (null); /* this pops the nest */ 
  if (mode == -m) { /* end of equation number */
	/* Check that another \.\$ follows */
    check_for_dollar;
	cur_mlist = p;
	cur_style = text_style;
	mlist_penalties = false;
	mlist_to_hlist();
	a = hpack (link (temp_head), 0, additional);
	subtype (a) = dlist;
	unsave();
	decr (save_ptr); /* now |cur_group=math_shift_group| */ 
	if (saved (0) == 1)
	  l = true;
	danger = false;
	/* Retrieve the prototype box */
	get_prototype_box;
	/* Check that the necessary fonts for math symbols are present; 
	   if not, flush the current math lists and set |danger:=true| */
	check_math_fonts;
	m = mode;
	p = fin_mlist (null);
  } else {
	a = null;
  }
  if (m < 0) {
	/* begin expansion of Finish math in text */
	/* module 1341 */
	/* The |unsave| is done after everything else here; hence an appearance of
	 * `\.{\\mathsurround}' inside of `\.{\$...\$}' affects the spacing at these
	 * particular \.\$'s. This is consistent with the conventions of
	 * `\.{\$\$...\$\$}', since `\.{\\abovedisplayskip}' inside a display affects the
	 * space above that display.
	 */
	tail_append (new_math (math_surround, before));
	cur_mlist = p;
	cur_style = text_style;
	mlist_penalties = (mode > 0);
	mlist_to_hlist();
	link (tail) = link (temp_head);
	while (link (tail) != null)
	  tail = link (tail);
	tail_append (new_math (math_surround, after));
	space_factor = 1000;
	unsave();
	/* end expansion of Finish math in text */
	} else {
	  if (a == null) {
		/* Check that another \.\$ follows */
		check_for_dollar;
	  }
	  /* begin expansion of Finish displayed math */
	  /* module 1344 */
	  /* At this time |p| points to the mlist for the formula; |a| is either
	   * |null| or it points to a box containing the equation number; and we are in
	   * vertical mode (or internal vertical mode).
	   */
	  cur_mlist = p;
	  cur_style = display_style;
	  mlist_penalties = false;
	  mlist_to_hlist();
	  p = link (temp_head);
	  adjust_tail = adjust_head;
	  pre_adjust_tail = pre_adjust_head;
	  b = hpack (p, 0, additional);
	  p = list_ptr (b);
	  t = adjust_tail;
	  adjust_tail = null;
	  pre_t = pre_adjust_tail;
	  pre_adjust_tail = null;
	  w = width (b);
	  z = display_width;
	  s = display_indent;
	  if (pre_display_direction < 0)
		s = -s - z;
	  if ((a == null) || danger) {
		e = 0;
		q = 0;
	  } else {
		e = width (a);
		q = e + math_quad (text_size);
	  };
	  if (w + q > z) {
		/* begin expansion of Squeeze the equation as much as possible; if 
		   there is an equation number that should go on a separate line by itself, set~|e:=0| */
		/* module 1346 */
		/* The user can force the equation number to go on a separate line
		 * by causing its width to be zero.
		 */
		if ((e != 0) && ((w - total_shrink[normal] + q <= z)
						 || (total_shrink[fil] != 0)
						 || (total_shrink[fill] != 0)
						 || (total_shrink[filll] != 0))) {
		  free_node (b, box_node_size);
		  b = hpack (p, z - q, exactly);
		} else {
		  e = 0;
		  if (w > z) {
			free_node (b, box_node_size);
			b = hpack (p, z, exactly);
		  };
		};
		w = width (b);
	  };
	  /* end expansion of Squeeze the equation as much as possible; ... */
	  /* begin expansion of Determine the displacement, |d|, of the left edge of the equation, 
		 with respect to the line size |z|, assuming that |l=false| */
	  /* module 1347 */
	  /* We try first to center the display without regard to the existence of
	   * the equation number. If that would make it too close (where ``too close''
	   * means that the space between display and equation number is less than the
	   * width of the equation number), we either center it in the remaining space
	   * or move it as far from the equation number as possible. The latter alternative
	   * is taken only if the display begins with glue, since we assume that the
	   * user put glue there to control the spacing precisely.
	   */
	  subtype (b) = dlist;
	  d = half (z - w);
	  if ((e > 0) && (d < 2 * e)) {	/* too close */
		d = half (z - w - e);
		if (p != null)
		  if (!is_char_node (p))
			if (type (p) == glue_node)
			  d = 0;
	  };
	  /* end expansion of Determine the displacement, |d|, of the left edge of ...*/
	  /* begin expansion of Append the glue or equation number preceding the display */
	  /* module 1348 */
	  /* If the equation number is set on a line by itself, either before or
	   * after the formula, we append an infinite penalty so that no page break will
	   * separate the display from its number; and we use the same size and
	   * displacement for all three potential lines of the display, even though
	   * `\.{\\parshape}' may specify them differently.
	   */
	  tail_append (new_penalty (pre_display_penalty));
	  if ((d + s <= pre_display_size) || l)	{ /* not enough clearance */
		g1 = above_display_skip_code;
		g2 = below_display_skip_code;
	  }  else {
		g1 = above_display_short_skip_code;
		g2 = below_display_short_skip_code;
	  };
	  if (l && (e == 0)) {	/* it follows that |type(a)=hlist_node| */
		app_display (j, a, 0);
		tail_append (new_penalty (inf_penalty));
	  }  else {
		tail_append (new_param_glue (g1));
	  }
	  /* end expansion of Append the glue or equation number preceding the display */
	  /* begin expansion of Append the display and perhaps also the equation number */
	  /* module 1349 */
	  if (e != 0) {
		r = new_kern (z - w - e - d);
		if (l) {
		  link (a) = r;
		  link (r) = b;
		  b = a;
		  d = 0;
		} else {
		  link (b) = r;
		  link (r) = a;
		};
		b = hpack (b, 0, additional);
	  };
	  app_display (j, b, d);
	  /* end expansion of Append the display and perhaps also the equation number */
	  /* begin expansion of Append the glue or equation number following the display */
	  /* module 1350 */
	  if ((a != null) && (e == 0) && !l) {
		tail_append (new_penalty (inf_penalty));
		app_display (j, a, z - width (a));
		g2 = 0;
	  };
	  if (t != adjust_head)	{ /* migrating material comes after equation number */
		link (tail) = link (adjust_head);
		tail = t;
	  };
	  if (pre_t != pre_adjust_head) {
		link (tail) = link (pre_adjust_head);
		tail = pre_t;
	  };
	  tail_append (new_penalty (post_display_penalty));
	  if (g2 > 0)
		tail_append (new_param_glue (g2));
	  /* end expansion of Append the glue or equation number following the display */
	  /* begin expansion of Flush the prototype box */
	  /* module 1688 */
	  flush_node_list (j);
	  /* end expansion of Flush the prototype box */
	  resume_after_display();
	  /* end expansion of Finish displayed math */
	};
}



/* module 1678 */

/* When calculating the natural width, |w|, of the final line preceding
 * the display, we may have to copy all or part of its hlist. We copy,
 * however, only those parts of the original list that are relevant for the
 * computation of |pre_display_size|.
 */
void 
just_copy (pointer p, pointer h, pointer t) {
  pointer r; /* current node being fabricated for new list */ 
  unsigned char words; /* number of words remaining to be copied */
  r=null; /*TH -Wall*/
  while (p != null) {
	words = 1; /* this setting occurs in more branches than any other */
	if (is_char_node (p)) {
	  r = get_avail();
	} else {
	  switch (type (p)) {
	  case hlist_node:
	  case vlist_node:
		r = get_node (box_node_size);
		mem[r + 6] = mem[p + 6];
		mem[r + 5] = mem[p + 5]; /* copy the last two words */ 
		words = 5;
		list_ptr (r) = null; /* this affects |mem[r+5]| */ 
		break;
	  case rule_node:
		r = get_node (rule_node_size);
		words = rule_node_size;
		break;
	  case ligature_node:
		r = get_avail(); /* only |font| and |character| are needed */
		mem[r] = mem[lig_char (p)];
		goto FOUND;
	  case kern_node:
	  case math_node:
		r = get_node (small_node_size);
		words = small_node_size;
		break;
	  case glue_node:
		r = get_node (small_node_size);
		add_glue_ref (glue_ptr (p));
		glue_ptr (r) = glue_ptr (p);
		leader_ptr (r) = null;
		break;
	  case whatsit_node:
		/* begin expansion of Make a partial copy of the whatsit node |p| and make 
		   |r| point to it; set |words| to the number of initial words not yet copied */
		do_a_partial_whatsit_copy;
		break;
	  default:
		goto NOT_FOUND;
	  };
	}
	while (words > 0) {
	  decr (words);
	  mem[r + words] = mem[p + words];
	};
  FOUND:
	link (h) = r;
	h = r;
  NOT_FOUND:
	p = link (p);
  };
  link (h) = t;
}


/* module 1683 */
void 
just_reverse (pointer p) {
  pointer l; /* the new list */ 
  pointer t; /* tail of reversed segment */ 
  pointer q; /* the next node */ 
  halfword m, n; /* count of unmatched math nodes */ 
  m = min_halfword;
  n = min_halfword;
  if (link (temp_head) == null) {
    just_copy (link (p), temp_head, null);
    q = link (temp_head);
  } else {
    q = link (p);
    link (p) = null;
    flush_node_list (link (temp_head));
  };
  t = new_edge (cur_dir, 0);
  l = t;
  cur_dir = reflected;
  while (q != null) {
    if (is_char_node (q)) {
      do {
		p = q;
		q = link (p);
		link (p) = l;
		l = p;
      } while (is_char_node (q));
    } else {
      p = q;
    };
    q = link (p);
    if (type (p) == math_node) {
	  /* begin expansion of Adjust \(t)the LR stack for the |just_reverse| routine */
	  /* module 1684 */
	  if (end_LR (p))
		if (info (LR_ptr) != end_LR_type (p)) {
		  type (p) = kern_node;
		  incr (LR_problems);
		} else {
		  pop_LR;
		  if (n > min_halfword) {
			decr (n);
			decr (subtype (p)); /* change |after| into |before| */ 
		  } else {
			if (m > min_halfword) {
			  decr (m);
			} else {
			  do_something;
			  goto FOUND;
			}
			type (p) = kern_node;
		  };
		}  else {
		  push_LR (p);
		  if ((n > min_halfword) || (LR_dir (p) != cur_dir)) {
			incr (n);
			incr (subtype (p)); /* change |before| into |after| */ 
		  }else {
			type (p) = kern_node;
			incr (m);
		  };
		}
	  /* end expansion of Adjust \(t)the LR stack for the |just_reverse| routine */
	};
	link (p) = l;
	l = p;
  };
  do_something;
  goto DONE;
 FOUND:
  width (t) = width (p);
  link (t) = q;
  free_node (p, small_node_size);
 DONE:
  link (temp_head) = l;
};

/* module 1691 */
#define cancel_glue(aa,bb,cc,dd,ee)  j   =  new_skip_param (aa); link ( bb )  =  j ;\
                                link ( j )  =  cc ; j   =  glue_ptr ( dd );\
                                stretch_order ( temp_ptr )  =  stretch_order ( j );\
                                shrink_order ( temp_ptr )  =  shrink_order ( j );\
                                width ( temp_ptr )  =  ee  -  width ( j );\
                                stretch ( temp_ptr )  =  -  stretch ( j );\
                                shrink ( temp_ptr )  =  -  shrink ( j )


/* module 1689 */

/* The |app_display| procedure used to append the displayed equation
 * and\slash or equation number to the current vertical list has three
 * parameters: the prototype box, the hbox to be appended, and the
 * displacement of the hbox in the display line.
 */

void 
app_display (pointer j, pointer b, scaled d) {
  scaled z; /* width of the line */ 
  scaled s; /* move the line right this much */ 
  scaled e; /* distance from right edge of box to end of line */ 
  int x; /* |pre_display_direction| */ 
  pointer p, q, r, t, u; /* for list manipulation */ 
  s = display_indent;
  x = pre_display_direction;
  if (x == 0) {
	shift_amount (b) = s + d;
  } else {
	z = display_width;
	p = b;
	/* begin expansion of Set up the hlist for the display line */
	/* module 1690 */
	/* Here we construct the hlist for the display, starting with node |p|
	 * and ending with node |q|. We also set |d| and |e| to the amount of
	 * kerning to be added before and after the hlist (adjusted for the
	 * prototype box).
	 */
	if (x > 0) {
	  e = z - d - width (p);
	} else {
	  e = d;
	  d = z - e - width (p);
	};
	if (j != null) {
	  b = copy_node_list (j);
	  height (b) = height (p);
	  depth (b) = depth (p);
	  s = s - shift_amount (b);
	  d = d + s;
	  e = e + width (b) - z - s;
	};
	if (subtype (p) == dlist) {
	  q = p;	/* display or equation number */
	} else {	/* display and equation number */
	  r = list_ptr (p);
	  free_node (p, box_node_size);
	  if (r == null)
		confusion ("LR4");
	  if (x > 0) {
		p = r;
		do {
		  q = r;
		  r = link (r);  /* find tail of list */ 
		} while (r != null);
	  } else {
		p = null;
		q = r;
		do {
		  t = link (r);
		  link (r) = p;
		  p = r;
		  r = t; /* reverse list */ 
		} while (r != null);
	  };
	};
	/* end expansion of Set up the hlist for the display line */
	/* begin expansion of Package the display line */
	/* module 1691 */
	/* In the presence of a prototype box we use its shift amount and width
	 * to adjust the values of kerning and add these values to the glue nodes
	 * inserted to cancel the \.{\\leftskip} and \.{\\rightskip}. If there is
	 * no prototype box (because the display is preceded by an empty
	 * paragraph), or if the skip parameters are zero, we just add kerns.
	 * 
	 * The |cancel_glue| macro creates and links a glue node that is, together
	 * with another glue node, equivalent to a given amount of kerning. We can
	 * use |j| as temporary pointer, since all we need is |j<>null|.
	 */
	if (j == null) {
	  r = new_kern (0);
	  t = new_kern (0); /* the widths will be set later */ 
	}  else {
	  r = list_ptr (b);
	  t = link (r);
	};
	u = new_math (0, end_M_code);
	if (type (t) == glue_node) {	/* |t| is \.{\\rightskip} glue */
	  cancel_glue (right_skip_code,q,u,t,e);
	  link (u) = t;
	}  else {
	  width (t) = e;
	  link (t) = u;
	  link (q) = t;
	};
	u = new_math (0, begin_M_code);
	if (type (r) == glue_node)	{ /* |r| is \.{\\leftskip} glue */
	  cancel_glue (left_skip_code,u,p,r,d);
	  link (r) = u;
	} else {
	  width (r) = d;
	  link (r) = p;
	  link (u) = r;
	  if (j == null) {
		b = hpack (u, 0, additional);
		shift_amount (b) = s;
	  }  else {
		list_ptr (b) = u;
	  }
	};
	/* end expansion of Package the display line */
  };
  append_to_vlist (b);
}
