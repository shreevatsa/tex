
#include "types.h"
#include "c-compat.h"
#include "globals.h"


/* module 402 */

/* 
 * Let's turn now to some procedures that \TeX\ calls upon frequently to digest
 * certain kinds of patterns in the input. Most of these are quite simple;
 * some are quite elaborate. Almost all of the routines call |get_x_token|,
 * which can cause them to be invoked recursively.
 */


/* module 403 */

/* The |scan_left_brace| routine is called when a left brace is supposed to be
 * the next non-blank token. (The term ``left brace'' means, more precisely,
 * a character whose catcode is |left_brace|.) \TeX\ allows \.{\\relax} to
 * appear before the |left_brace|.
 */
void
scan_left_brace (void) { /* reads a mandatory |left_brace| */
  /* Get the next non-blank non-relax non-call token */
  get_nblank_nrelax_ncall;
  if (cur_cmd != left_brace) {
	print_err ("Missing { inserted");
	help4 ("A left brace was mandatory here, so I've put one in.",
		   "You might want to delete and/or insert some corrections",
		   "so that I will find a matching right brace soon.",
		   "(If you're confused by all this, try typing `I}' now.)");
	back_error();
	cur_tok = left_brace_token + '{';
	cur_cmd = left_brace;
	cur_chr = '{';
	incr (align_state);
  };
};

/* module 405 */

/* The |scan_optional_equals| routine looks for an optional `\.=' sign preceded
 * by optional spaces; `\.{\\relax}' is not ignored here.
 */
void
scan_optional_equals (void) {
  /* Get the next non-blank non-call token */
  get_nblank_ncall;
  if (cur_tok != other_token + '=')
    back_input();
};


/* module 407 */

/* In case you are getting bored, here is a slightly less trivial routine:
 * Given a string of lowercase letters, like `\.{pt}' or `\.{plus}' or
 * `\.{width}', the |scan_keyword| routine checks to see whether the next
 * tokens of input match this string. The match must be exact, except that
 * uppercase letters will match their lowercase counterparts; uppercase
 * equivalents are determined by subtracting |"a"-"A"|, rather than using the
 * |uc_code| table, since \TeX\ uses this routine only for its own limited
 * set of keywords.
 * 
 * If a match is found, the characters are effectively removed from the input
 * and |true| is returned. Otherwise |false| is returned, and the input
 * is left essentially unchanged (except for the fact that some macros
 * may have been expanded, etc.).
 */

boolean
scan_keyword (char * str) {	/* look for a given string */
  pointer p; /* tail of the backup list */ 
  pointer q; /* new node being added to the token list via |store_new_token| */
  unsigned k; /* index into the keyword string */ 
  p = backup_head;
  link (p) = null;
  k = 0;
  while (k < strlen(str)) {
	get_x_token(); /* recursion is possible here */
	if ((cur_cs == 0) && ((cur_chr == str[k]) || (cur_chr == str[k] - 'a' + 'A'))) {
	  store_new_token (cur_tok);
	  incr (k);
	} else if ((cur_cmd != spacer) || (p != backup_head)) {
	  back_input();
	  if (p != backup_head)
		back_list (link (backup_head));
	  return false;		
	};
  };
  flush_list (link (backup_head));
  return true;
};

/* module 409 */

/* The next routine `|scan_something_internal|' is used to fetch internal
 * numeric quantities like `\.{\\hsize}', and also to handle the `\.{\\the}'
 * when expanding constructions like `\.{\\the\\toks0}' and
 * `\.{\\the\\baselineskip}'. Soon we will be considering the |scan_int|
 * procedure, which calls |scan_something_internal|; on the other hand,
 * |scan_something_internal| also calls |scan_int|, for constructions like
 * `\.{\\catcode\`\\\$}' or `\.{\\fontdimen} \.3 \.{\\ff}'. So we
 * have to declare |scan_int| as a |forward| procedure. A few other
 * procedures are also declared at this point.
 */

/* module 410 */

/* \TeX\ doesn't know exactly what to expect when |scan_something_internal|
 * begins. For example, an integer or dimension or glue value could occur
 * immediately after `\.{\\hskip}'; and one can even say \.{\\the} with
 * respect to token lists in constructions like
 * `\.{\\xdef\\o\{\\the\\output\}}'. On the other hand, only integers are
 * allowed after a construction like `\.{\\count}'. To handle the various
 * possibilities, |scan_something_internal| has a |level| parameter, which
 * tells the ``highest'' kind of quantity that |scan_something_internal| is
 * allowed to produce. Six levels are distinguished, namely |int_val|,
 * |dimen_val|, |glue_val|, |mu_val|, |ident_val|, and |tok_val|.
 * 
 * The output of |scan_something_internal| (and of the other routines
 * |scan_int|, |scan_dimen|, and |scan_glue| below) is put into the global
 * variable |cur_val|, and its level is put into |cur_val_level|. The highest
 * values of |cur_val_level| are special: |mu_val| is used only when
 * |cur_val| points to something in a ``muskip'' register, or to one of the
 * three parameters \.{\\thinmuskip}, \.{\\medmuskip}, \.{\\thickmuskip};
 * |ident_val| is used only when |cur_val| points to a font identifier;
 * |tok_val| is used only when |cur_val| points to |null| or to the reference
 * count of a token list. The last two cases are allowed only when
 * |scan_something_internal| is called with |level=tok_val|.
 * 
 * If the output is glue, |cur_val| will point to a glue specification, and
 * the reference count of that glue will have been updated to reflect this
 * reference; if the output is a nonempty token list, |cur_val| will point to
 * its reference count, but in this case the count will not have been updated.
 * Otherwise |cur_val| will contain the integer or scaled value in question.
 */
integer cur_val; /* value returned by numeric scanners */ 
unsigned char cur_val_level; /* the ``level'' of this value */

/* module 431 */
#define negate_curval {             \
        negate (width (cur_val));   \
        negate (stretch (cur_val)); \
	    negate (shrink (cur_val)); }


/* module 413 */

/* OK, we're ready for |scan_something_internal| itself. A second parameter,
 * |negative|, is set |true| if the value that is found should be negated.
 * It is assumed that |cur_cmd| and |cur_chr| represent the first token of
 * the internal quantity to be scanned; an error will be signalled if
 * |cur_cmd<min_internal| or |cur_cmd>max_internal|.
 */
#define scanned_result(a,b) { cur_val   =  a ; cur_val_level   =  b ; }

void
scan_something_internal (small_number level, boolean negative) {	
  /* fetch an internal parameter */
  halfword m; /* |chr_code| part of the operand token */ 
  int n, k; /* accumulators */ 
  halfword q; /* general purpose index */ 
  four_quarters i; /* character info */ 
  unsigned int p; /* index into |nest| */ 
  m = cur_chr; 
  switch (cur_cmd)
    {
    case def_code:
	  /* begin expansion of Fetch a character code from some table */
	  /* module 414 */
	  scan_char_num();
	  if (m == math_code_base) {
		scanned_result (ho (math_code (cur_val)), int_val);
	  } else if (m < math_code_base) {
		scanned_result (equiv (m + cur_val), int_val);
	  } else {
		scanned_result (eqtb[m + cur_val].cint, int_val);
	  }
	  /* end expansion of Fetch a character code from some table */
      break;
    case toks_register:
    case assign_toks:
    case def_family:
    case set_font:
    case def_font:
	  /* begin expansion of Fetch a token list or font identifier, provided that |level=tok_val| */
	  /* module 415 */
      if (level != tok_val) {
		print_err ("Missing number, treated as zero");
		help3 ("A number should have been here; I inserted `0'.",
			   "(If you can't figure out why I needed to see a number,",
			   "look up `weird error' in the index to The TeXbook.)");
		back_error();
		scanned_result (0, dimen_val);
	  } else if (cur_cmd <= assign_toks) {
		if (cur_cmd < assign_toks) {	/* |cur_cmd=toks_register| */
	      if (m == mem_bot) {
			scan_register_num();
			if (cur_val < 256) {
		      cur_val = equiv (toks_base + cur_val);
		    } else {
		      find_sa_element (tok_val, cur_val, false);
		      if (cur_ptr == null) {
				cur_val = null;
			  } else {
				cur_val = sa_ptr (cur_ptr);
			  }
		    };
		  } else {
			cur_val = sa_ptr (m);
		  }
	    } else {
		  cur_val = equiv (m);
		}
		cur_val_level = tok_val;
	  } else {
		back_input();
		scan_font_ident();
		scanned_result (font_id_base + cur_val, ident_val);
	  }
	  /* end expansion of Fetch a token list or font identifier, provided that |level=tok_val| */
      break;
    case assign_int:
      scanned_result (eqtb[m].cint, int_val);
      break;
    case assign_dimen:
      scanned_result (eqtb[m].sc, dimen_val);
      break;
    case assign_glue:
      scanned_result (equiv (m), glue_val);
      break;
    case assign_mu_glue:
      scanned_result (equiv (m), mu_val);
      break;
    case set_aux:
	  /* begin expansion of Fetch the |space_factor| or the |prev_depth| */
	  /* module 418 */
      if (abs (mode) != m) {
		print_err ("Improper ");
		print_cmd_chr (set_aux, m);
		help4("You can refer to \\spacefactor only in horizontal mode;",
			  "you can refer to \\prevdepth only in vertical mode; and",
			  "neither of these is meaningful inside \\write. So",
			  "I'm forgetting what you said and using zero instead.");
		error();
		if (level != tok_val) {
		  scanned_result (0, dimen_val);
		} else {
		  scanned_result (0, int_val);
		}
	  } else if (m == vmode) {
		cur_val = prev_depth;
		cur_val_level = dimen_val;
	  } else {
		cur_val = space_factor;
		cur_val_level = int_val;
	  }
	  /* end expansion of Fetch the |space_factor| or the |prev_depth| */
      break;
    case set_prev_graf:
	  /* begin expansion of Fetch the |prev_graf| */
	  /* module 422 */
      if (mode == 0) {
		scanned_result (0, int_val);	/* |prev_graf=0| within \.{\\write} */
	  } else {
		nest[nest_ptr] = cur_list;
		p = nest_ptr;
		while (abs (nest[p].mode_field) != vmode)
		  decr (p);
		scanned_result (nest[p].pg_field, int_val);
	  }
	  /* end expansion of Fetch the |prev_graf| */
      break;
    case set_page_int:
	  /* begin expansion of Fetch the |dead_cycles| or the |insert_penalties| */
	  /* module 419 */
	  if (m == 0) {
		cur_val = dead_cycles;
		/* begin expansion of Cases for `Fetch the |dead_cycles| or the |insert_penalties|' */
		/* module 1640 */
	  } else if (m == 2) {
		cur_val = interaction;
		/* end expansion of Cases for `Fetch the |dead_cycles| or the |insert_penalties|' */
	  } else {
		cur_val = insert_penalties;
	  }
	  cur_val_level = int_val;
	  /* end expansion of Fetch the |dead_cycles| or the |insert_penalties| */
      break;
    case set_page_dimen:
	  /* begin expansion of Fetch something on the |page_so_far| */
	  /* module 421 */
	  /* Inside an \.{\\output} routine, a user may wish to look at the page totals
	   * that were present at the moment when output was triggered.
	   */
	  if ((page_contents == empty) && (!output_active)) {
		if (m == 0) {
		  cur_val = max_dimen;
		} else {
		  cur_val = 0;
		}
	  } else {
		cur_val = page_so_far[m];
	  }
	  cur_val_level = dimen_val;
	  /* end expansion of Fetch something on the |page_so_far| */
      break;
    case set_shape:
	  /* begin expansion of Fetch the |par_shape| size */
	  /* module 423 */
	  if (m > par_shape_loc) {
		/* begin expansion of Fetch a penalties array element */
		/* module 1811 */
		scan_int();
		if ((equiv (m) == null) || (cur_val < 0)) {
		  cur_val = 0;
		} else {
		  if (cur_val > penalty (equiv (m)))
			cur_val = penalty (equiv (m));
		  cur_val = penalty (equiv (m) + cur_val);
		};
		/* end expansion of Fetch a penalties array element */
	  } else if (par_shape_ptr == null) {
		cur_val = 0;
	  } else {
		cur_val = info (par_shape_ptr);
	  }
	  cur_val_level = int_val;
	  /* end expansion of Fetch the |par_shape| size */
      break;
    case set_box_dimen:
	  /* begin expansion of Fetch a box dimension */
	  /* module 420 */
	  scan_register_num();
	  fetch_box (q);
	  if (q == null) {
		cur_val = 0;
	  } else {
		cur_val = mem[q + m].sc;
	  }
	  cur_val_level = dimen_val;
	  /* end expansion of Fetch a box dimension */
      break;
    case char_given:
    case math_given:
      scanned_result (cur_chr, int_val);
      break;
    case assign_font_dimen:
	  /* begin expansion of Fetch a font dimension */
	  /* module 425 */
	  find_font_dimen (false);
	  font_info[fmem_ptr].sc = 0;
	  scanned_result (font_info[cur_val].sc, dimen_val);
	  /* end expansion of Fetch a font dimension */
      break;
    case assign_font_int:
	  /* begin expansion of Fetch a font integer */
	  /* module 426 */
	  scan_font_ident();
	  if (m == 0) {
		scanned_result (hyphen_char[cur_val], int_val);
	  } else if (m == 1) {
		scanned_result (skew_char[cur_val], int_val);
	  } else {
		n = cur_val;
		scan_char_num();
		k = cur_val;
		switch (m) {
		case lp_code_base:
		  scanned_result (get_lp_code (n, k), int_val);
		  break;
		case rp_code_base:
		  scanned_result (get_rp_code (n, k), int_val);
		  break;
		case ef_code_base:
		  scanned_result (get_ef_code (n, k), int_val);
		};
	  };
	  /* end expansion of Fetch a font integer */
      break;
	case register_cmd:
	  /* begin expansion of Fetch a register */
	  /* module 427 */
	  if ((m < mem_bot) || (m > lo_mem_stat_max)) {
		cur_val_level = sa_type (m);
		if (cur_val_level < glue_val) {
		  cur_val = sa_int (m);
		} else {
		  cur_val = sa_ptr (m);
		}
	  } else {
		scan_register_num();
		cur_val_level = m - mem_bot;
		if (cur_val > 255) {
		  find_sa_element (cur_val_level, cur_val, false);
		  if (cur_ptr == null) {
			if (cur_val_level < glue_val) {
			  cur_val = 0;
			} else {
			  cur_val = zero_glue;
			}
		  } else if (cur_val_level < glue_val) {
			cur_val = sa_int (cur_ptr);
		  } else {
			cur_val = sa_ptr (cur_ptr);
		  };
		} else {
		  switch (cur_val_level) {
		  case int_val:
			cur_val = count (cur_val);
			break;
		  case dimen_val:
			cur_val = dimen (cur_val);
			break;
		  case glue_val:
			cur_val = skip (cur_val);
			break;
		  case mu_val:
			cur_val = mu_skip (cur_val);
		  }; /* there are no other cases */ 
		};
	  };
	  /* end expansion of Fetch a register */
	  break;
    case last_item:
	  /* begin expansion of Fetch an item in the current node, if appropriate */
	  /* module 424 */
	  /* Here is where \.{\\lastpenalty}, \.{\\lastkern}, and \.{\\lastskip} are
	   * implemented. The reference count for \.{\\lastskip} will be updated later.
	   * A final \.{\\endM} node is temporarily removed.
	   * 
	   * We also handle \.{\\inputlineno} and \.{\\badness} here, because they are
	   * legal in similar contexts.
	   */
      if (m > last_node_type_code)
		if (m >= eTeX_glue) {
		  /* begin expansion of Process an expression and |return| */
		  /* module 1725 */
		  /* This code for reducing |cur_val_level| and\slash or negating the
		   * result is similar to the one for all the other cases of
		   * |scan_something_internal|, with the difference that |scan_expr| has
		   * already increased the reference count of a glue specification.
		   */
		  if (m < eTeX_mu) {
			switch (m) {
			  /* begin expansion of Cases for fetching a glue value */
			  /* module 1752 */
			case mu_to_glue_code:
			  scan_mu_glue();
			  /* end expansion of Cases for fetching a glue value */
			}; /* there are no other cases */ 
			cur_val_level = glue_val;
	      } else if (m < eTeX_expr) {
			switch (m) {
			  /* begin expansion of Cases for fetching a mu value */
			  /* module 1753 */
			case glue_to_mu_code:
			  scan_normal_glue();
			  /* end expansion of Cases for fetching a mu value */
			}; /* there are no other cases */ 
			cur_val_level = mu_val;
	      } else {
			cur_val_level = m - eTeX_expr + int_val;
			scan_expr();
	      };
		  while (cur_val_level > level) {
			if (cur_val_level == glue_val) {
			  m = cur_val;
			  cur_val = width (m);
			  delete_glue_ref (m);
			} else if (cur_val_level == mu_val) {
			  mu_error();
			}
			decr (cur_val_level);
	      };
		  if (negative) {
			if (cur_val_level >= glue_val) {
			  m = cur_val;
			  cur_val = new_spec (m);
			  delete_glue_ref (m);
			  /* begin expansion of Negate all three glue components of |cur_val| */
			  negate_curval;
			  /* end expansion of Negate all three glue components of |cur_val| */		  
			}  else {
			  negate (cur_val); 
			}
		  }
		  return;
		  /* end expansion of Process an expression and |return| */
		} else if (m >= eTeX_dim) {
		  switch (m) {
	      case font_char_wd_code:
	      case font_char_ht_code:
	      case font_char_dp_code:
	      case font_char_ic_code:
			scan_font_ident();
			q = cur_val;
			scan_char_num();
			if ((font_bc[q] <= cur_val) && (font_ec[q] >= cur_val)) {
			  i = char_info (q, qi (cur_val));
			  switch (m) {
			  case font_char_wd_code:
				cur_val = char_width (q,i);
				break;
			  case font_char_ht_code:
				cur_val = char_height (q,height_depth (i));
				break;
			  case font_char_dp_code:
				cur_val = char_depth (q,height_depth (i));
				break;
			  case font_char_ic_code:
				cur_val = char_italic (q,i);
			  }; /* there are no other cases */ 
			} else {
			  cur_val = 0;
			};
			break;
			/* module 1620 */
	      case par_shape_length_code:
	      case par_shape_indent_code:
	      case par_shape_dimen_code:
			q = cur_chr - par_shape_length_code;
			scan_int();
			if ((par_shape_ptr == null) || (cur_val <= 0)) {
			  cur_val = 0;
			} else {
			  if (q == 2) {
				q = cur_val % 2;
				cur_val = (cur_val + q) / 2;
			  };
			  if (cur_val > info (par_shape_ptr))
				cur_val = info (par_shape_ptr);
			  cur_val = mem[par_shape_ptr + 2 * cur_val - q].sc;
			};
			cur_val_level = dimen_val;
			break;
			/* module 1749 */
	      case glue_stretch_code:
	      case glue_shrink_code:
			scan_normal_glue();
			q = cur_val;
			if (m == glue_stretch_code) {
			  cur_val = stretch (q);
			} else {
			  cur_val = shrink (q);
			}
			delete_glue_ref (q);
			/* end expansion of Cases for fetching a dimension value */
	      }; /* there are no other cases */ 
		  cur_val_level = dimen_val;
		} else {
	    switch (m) 
		  {
	      case input_line_no_code:
			cur_val = line;
			break;
	      case badness_code:
			cur_val = last_badness;
			break;
	      case pdftex_version_code:
			cur_val = pdftex_version;
			break;
	      case pdf_last_obj_code:
			cur_val = pdf_last_obj;
			break;
	      case pdf_last_xform_code:
			cur_val = pdf_last_xform;
			break;
	      case pdf_last_ximage_code:
			cur_val = pdf_last_ximage;
			break;
	      case pdf_last_ximage_pages_code:
			cur_val = pdf_last_ximage_pages;
			break;
	      case pdf_last_annot_code:
			cur_val = pdf_last_annot;
			break;
	      case pdf_last_x_pos_code:
			cur_val = pdf_last_x_pos;
			break;
	      case pdf_last_y_pos_code:
			cur_val = pdf_last_y_pos;
			break;
	      case pdf_last_demerits_code:
			cur_val = fewest_demerits;
			break;
	      case pdf_last_vbreak_penalty_code:
			cur_val = last_vbreak_penalty;
			break;
			/* begin expansion of Cases for fetching an integer value */
			/* module 1594 */
	      case eTeX_version_code:
			cur_val = eTeX_version;
			break;
			/* module 1611 */
	      case current_group_level_code:
			cur_val = cur_level - level_one;
			break;
	      case current_group_type_code:
			cur_val = cur_group;
			break;
			/* module 1614 */
	      case current_if_level_code:
			q = cond_ptr;
			cur_val = 0;
			while (q != null) {
			  incr (cur_val);
			  q = link (q);
			};
			do_something;
			break;
	      case current_if_type_code:
			if (cond_ptr == null) {
			  cur_val = 0;
			} else if (cur_if < unless_code) {
			  cur_val = cur_if + 1;
			} else {
			  cur_val = -(cur_if - unless_code + 1);
			}
			break;
	      case current_if_branch_code:
			if ((if_limit == or_code) || (if_limit == else_code)) {
			  cur_val = 1;
			} else if (if_limit == fi_code) {
			  cur_val = -1;
			} else {
			  cur_val = 0;
			}
			break;
			/* module 1748 */
	      case glue_stretch_order_code:
	      case glue_shrink_order_code:
			scan_normal_glue();
			q = cur_val;
			if (m == glue_stretch_order_code) {
			  cur_val = stretch_order (q);
			} else {
			  cur_val = shrink_order (q);
			}
			delete_glue_ref (q);
			/* end expansion of Cases for fetching an integer value */
	      }; /* there are no other cases */ 
		cur_val_level = int_val;
		} else {
		  if (cur_chr == glue_val) {
			cur_val = zero_glue;
		  } else {
			cur_val = 0;
		  }
		  if (cur_chr == last_node_type_code) {
			cur_val = int_val;
			if ((tail == head) || (mode == 0))
			  cur_val = -1;
		  } else {
			cur_val_level = cur_chr;
		  }
		  if (!is_char_node (tail) && (mode != 0)) {
			if ((type (tail) == math_node)&&(subtype (tail) == end_M_code))
			  remove_end_M();
			switch (cur_chr) {
			case int_val:
			  if (type (tail) == penalty_node)
				cur_val = penalty (tail);
			  break;
			case dimen_val:
			  if (type (tail) == kern_node)
				cur_val = width (tail);
			  break;
			case glue_val:
			  if (type (tail) == glue_node) {
				cur_val = glue_ptr (tail);
				if (subtype (tail) == mu_glue)
				  cur_val_level = mu_val;
			  };
			  break;
			case last_node_type_code:
			  if ((type (tail) != math_node) || (subtype (tail) != end_M_code)) {
				if (type (tail) <= unset_node) {
				  cur_val = type (tail) + 1;
				} else {
				  cur_val = unset_node + 2;
				}
			  };
			};			/* there are no other cases */ 
			if (LR_temp != null)
			  insert_end_M();
		  } else if ((mode == vmode) && (tail == head)) {
			switch (cur_chr) {
			case int_val:
			  cur_val = last_penalty;
			  break;
			case dimen_val:
			  cur_val = last_kern;
			  break;
			case glue_val:
			  if (last_glue != max_halfword)
				cur_val = last_glue;
			  break;
			case last_node_type_code:
			  cur_val = last_node_type;
			};
	      };		  /* there are no other cases */ 
		};
	  /* end expansion of Fetch an item in the current node, if appropriate */
      break;
    default:
	  /* begin expansion of Complain that \.{\\the} can't do this; give zero result */
	  /* module 428 */
	  print_err ("You can't use `");
	  print_cmd_chr (cur_cmd, cur_chr);
	  zprint_string("' after ");
	  print_esc_string ("the");
	  help1 ("I'm forgetting what you said and using zero instead.");
	  error();
	  if (level != tok_val) {
		scanned_result (0, dimen_val);
	  } else {
		scanned_result (0, int_val);
	  }
	  /* end expansion of Complain that \.{\\the} can't do this; give zero result */
    };
  while (cur_val_level > level) {
	/* begin expansion of Convert \(c)|cur_val| to a lower level */
	/* module 429 */
	/* When a |glue_val| changes to a |dimen_val|, we use the width component
	 * of the glue; there is no need to decrease the reference count, since it
	 * has not yet been increased. When a |dimen_val| changes to an |int_val|,
	 * we use scaled points so that the value doesn't actually change. And when a
	 * |mu_val| changes to a |glue_val|, the value doesn't change either.
	 */
	if (cur_val_level == glue_val) {
	  cur_val = width (cur_val);
	} else if (cur_val_level == mu_val) {
	  mu_error();
	}
	decr (cur_val_level);
	/* end expansion of Convert \(c)|cur_val| to a lower level */
  };
  /* begin expansion of Fix the reference count, if any, and negate |cur_val| if |negative| */
  /* module 430 */
  /* If |cur_val| points to a glue specification at this point, the reference
   * count for the glue does not yet include the reference by |cur_val|.
   * If |negative| is |true|, |cur_val_level| is known to be |<=mu_val|.
   */
  if (negative) {
	if (cur_val_level >= glue_val) {
	  cur_val = new_spec (cur_val);
	  /* Negate all three glue components of |cur_val| */
      negate_curval;
	} else {
	  negate (cur_val);
	}
  } else if ((cur_val_level >= glue_val) && (cur_val_level <= mu_val)) {
    add_glue_ref (cur_val);
  }
  /* end expansion of Fix the reference count, if any, and negate |cur_val| if |negative| */
}

/* module 432 */

/* Our next goal is to write the |scan_int| procedure, which scans anything that
 * \TeX\ treats as an integer. But first we might as well look at some simple
 * applications of |scan_int| that have already been made inside of
 * |scan_something_internal|.
 */

/* module 433 */
void
scan_eight_bit_int (void) {
  scan_int();
  if ((cur_val < 0) || (cur_val > 255)) {
	print_err ("Bad register code");
	help2 ("A register number must be between 0 and 255.","I changed this one to zero.");
	int_error (cur_val);
	cur_val = 0;
  };
};

#ifdef CDEBUG
int callno = 0;
#endif

/* module 434 */
void
scan_char_num (void) {
#ifdef CDEBUG
  fprintf (stdout,"Call no %d\n", ++callno);
  print_input_stack();
#endif
  scan_int();
  if ((cur_val < 0) || (cur_val > 255)) {
	print_err ("Bad character code");
	help2 ("A character number must be between 0 and 255.","I changed this one to zero.");
	int_error (cur_val);
	cur_val = 0;
  };
};

/* module module 435 */

/* While we're at it, we might as well deal with similar routines that
 * will be needed later.
 */
void
scan_four_bit_int (void) {
  scan_int();
  if ((cur_val < 0) || (cur_val > 15)) {
	print_err ("Bad number");
	help2 ("Since I expected to read a number between 0 and 15,",
		   "I changed this one to zero.");
	int_error (cur_val);
	cur_val = 0;
  };
};

/* module 436 */
void
scan_fifteen_bit_int (void) {
  scan_int();
  if ((cur_val < 0) || (cur_val > 32767)) {
	print_err ("Bad mathchar");
	help2 ("A mathchar number must be between 0 and 32767.",
		   "I changed this one to zero.");
	int_error (cur_val);
	cur_val = 0;
  };
};

/* module 437 */
void
scan_twenty_seven_bit_int (void) {
  scan_int();
  if ((cur_val < 0) || (cur_val > 134217727)) {
	print_err ("Bad delimiter code");
	help2 ("A numeric delimiter code must be between 0 and 2^{27}-1.",
		   "I changed this one to zero.");
	int_error (cur_val);
	cur_val = 0;
  };
};


/* module 438 */

/* An integer number can be preceded by any number of spaces and `\.+' or
 * `\.-' signs. Then comes either a decimal constant (i.e., radix 10), an
 * octal constant (i.e., radix 8, preceded by~\.\'), a hexadecimal constant
 * (radix 16, preceded by~\."), an alphabetic constant (preceded by~\.\`), or
 * an internal variable. After scanning is complete,
 * |cur_val| will contain the answer, which must be at most
 * $2^{31}-1=2147483647$ in absolute value. The value of |radix| is set to
 * 10, 8, or 16 in the cases of decimal, octal, or hexadecimal constants,
 * otherwise |radix| is set to zero. An optional space follows a constant.
 */
small_number radix; /* |scan_int| sets this to 8, 10, 16, or zero */


/* module 439 */

/* We initialize the following global variables just in case |expand|
 * comes into action before any of the basic scanning routines has assigned
 * them a value.
 */
void
scan_initialize (void) {
  cur_val = 0;
  cur_val_level = int_val;
  radix = 0;
  cur_order = 0;
}

/* module 441 */
#define get_next_nblank_nsign negative = false; \
  do { get_nblank_ncall;                        \
    if (cur_tok == other_token + '-') {         \
	  negative = !negative;                     \
	  cur_tok = other_token + '+';              \
	};                                          \
  } while (cur_tok == other_token + '+')

/* module 440 */

/* The |scan_int| routine is used also to scan the integer part of a
 * fraction; for example, the `\.3' in `\.{3.14159}' will be found by
 * |scan_int|. The |scan_dimen| routine assumes that |cur_tok=point_token|
 * after the integer part of such a fraction has been scanned by |scan_int|,
 * and that the decimal point has been backed up to be scanned again.
 */
void
scan_int (void) { /* sets |cur_val| to an integer */
  boolean negative; /* should the answer be negated? */ 
  int m; /* | div radix|, the threshold of danger */ 
  small_number d; /* the digit just scanned */ 
  boolean vacuous; /* have no digits appeared? */ 
  boolean OK_so_far; /* has an error message been issued? */ 
  radix = 0;
  OK_so_far = true;
  /* Get the next non-blank non-sign token; set |negative| appropriately */
  get_next_nblank_nsign;
  if (cur_tok == alpha_token) {
	/* begin expansion of Scan an alphabetic character code into |cur_val| */
	/* module 442 */
	/* A space is ignored after an alphabetic character constant, so that
	 * such constants behave like numeric ones.
	 */
	get_token(); /* suppress macro expansion */ 
	if (cur_tok < cs_token_flag) {
	  cur_val = cur_chr;
	  if (cur_cmd <= right_brace) {
	    if (cur_cmd == right_brace) {
		  incr (align_state);
		} else {
	      decr (align_state); 
		}
	  }
	} else if (cur_tok < cs_token_flag + single_base) {
	  cur_val = cur_tok - cs_token_flag - active_base;
	} else {
	  cur_val = cur_tok - cs_token_flag - single_base;
	}
	if (cur_val > 255) {
	  print_err ("Improper alphabetic constant");
	  help2 ("A one-character control sequence belongs after a ` mark.",
		 "So I'm essentially inserting \\0 here.");
	  cur_val = '0';
	  back_error();
	} else {
	  /* Scan an optional space */
	  scan_optional_space;
	}
	/* end expansion of Scan an alphabetic character code into |cur_val| */
  } else if ((cur_cmd >= min_internal) && (cur_cmd <= max_internal)) {
	scan_something_internal (int_val, false);
  } else {
	/* begin expansion of Scan a numeric constant */
	/* module 444 */
	radix = 10;
	m = 214748364;
	if (cur_tok == octal_token) {
	  radix = 8;
	  m = 268435456;
	  get_x_token();
	} else if (cur_tok == hex_token) {
	  radix = 16;
	  m = 134217728;
	  get_x_token();
	};
	vacuous = true;
	cur_val = 0;
	/* begin expansion of Accumulate the constant until |cur_tok| is not a suitable digit */
	/* module 445 */
	loop {
	  if ((cur_tok < zero_token + radix) && (cur_tok >= zero_token)
		  && (cur_tok <= zero_token + 9)) {
	    d = cur_tok - zero_token;
	  } else if (radix == 16) {
	    if ((cur_tok <= A_token + 5) && (cur_tok >= A_token)) {
		  d = cur_tok - A_token + 10;
		} else if ((cur_tok <= other_A_token + 5) && (cur_tok >= other_A_token)) {
		  d = cur_tok - other_A_token + 10;
		} else {
		  goto DONE;
		}
	  } else {
		do_something;
		goto DONE;
	  }
	  vacuous = false;
	  if ((cur_val >= m) && ((cur_val > m) || (d > 7) || (radix != 10))) {
	    if (OK_so_far) {
		  print_err ("Number too big");
		  help2 ("I can only go up to 2147483647='17777777777=\"7FFFFFFF,",
				 "so I'm using that number instead of yours.");
		  error();
		  cur_val = infinity;
		  OK_so_far = false;
		};
	  } else {
		cur_val = cur_val * radix + d;
	  }
	  get_x_token();
	};
  DONE:
	/* end expansion of Accumulate the constant until |cur_tok| is not a suitable digit */
	if (vacuous) {
	  /* begin expansion of Express astonishment that no number was here */
	  /* module 446 */
	  print_err ("Missing number, treated as zero");
	  help3 ("A number should have been here; I inserted `0'.",
	     "(If you can't figure out why I needed to see a number,",
		 "look up `weird error' in the index to The TeXbook.)");
	  back_error();
	  /* end expansion of Express astonishment that no number was here */
	} else if (cur_cmd != spacer) {
	  back_input();
	}
  };
  /* end expansion of Scan a numeric constant */
  if (negative)
    negate (cur_val);
}

/* module 447 */

/* The |scan_dimen| routine is similar to |scan_int|, but it sets |cur_val| to
 * a |scaled| value, i.e., an integral number of sp. One of its main tasks
 * is therefore to interpret the abbreviations for various kinds of units and
 * to convert measurements to scaled points.
 * 
 * There are three parameters: |mu| is |true| if the finite units must be
 * `\.{mu}', while |mu| is |false| if `\.{mu}' units are disallowed;
 * |inf| is |true| if the infinite units `\.{fil}', `\.{fill}', `\.{filll}'
 * are permitted; and |shortcut| is |true| if |cur_val| already contains
 * an integer and only the units need to be considered.
 * 
 * The order of infinity that was found in the case of infinite glue is returned
 * in the global variable |cur_order|.
 */
glue_ord cur_order; /* order of infinity found by |scan_dimen| */

/* module 451 */

/* The following code is executed when |scan_something_internal| was
 * called asking for |mu_val|, when we really wanted a ``mudimen'' instead
 * of ``muglue.''
 */
#define coerce_glue	  if (cur_val_level >= glue_val) { \
	      v = width (cur_val);                         \
	      delete_glue_ref (cur_val);                   \
	      cur_val = v; }


/* module 448 */

/* Constructions like `\.{-\'77 pt}' are legal dimensions, so |scan_dimen|
 * may begin with |scan_int|. This explains why it is convenient to use
 * |scan_int| also for the integer part of a decimal fraction.
 * 
 * Several branches of |scan_dimen| work with |cur_val| as an integer and
 * with an auxiliary fraction |f|, so that the actual quantity of interest is
 * $|cur_val|+|f|/2^{16}$. At the end of the routine, this ``unpacked''
 * representation is put into the single word |cur_val|, which suddenly
 * switches significance from |integer| to |scaled|.
 */
#define set_conversion(a,b) { num   =  a ;   denom   =  b ;}

void
scan_dimen (boolean mu, boolean inf, boolean shortcut) { /* sets |cur_val| to a dimension */
  boolean negative; /* should the answer be negated? */
  int f; /* numerator of a fraction whose denominator is $2^{16}$ */
  /* begin expansion of Local variables for dimension calculations */
  /* module 450 */
  unsigned int num, denom; /* conversion ratio for the scanned units */ 
  small_number k, kk; /* number of digits in a decimal fraction */ 
  pointer p, q; /* top of decimal digit stack */ 
  scaled v;   /* an internal dimension */ 
  int save_cur_val; /* temporary storage of |cur_val| */
  /* end expansion of Local variables for dimension calculations */
  f = 0;
  arith_error = false;
  cur_order = normal;
  negative = false;
  if (!shortcut) {
	/* Get the next non-blank non-sign... */
	get_next_nblank_nsign;
	if ((cur_cmd >= min_internal) && (cur_cmd <= max_internal)) {
	  /* begin expansion of Fetch an internal dimension and |goto attach_sign|, or fetch an internal integer */
	  /* module 449 */
	  if (mu) {
	    scan_something_internal (mu_val, false);
		/* Coerce glue to a dimension */
		coerce_glue;
	    if (cur_val_level == mu_val)
	      goto ATTACH_SIGN;
	    if (cur_val_level != int_val)
	      mu_error();
	  } else {
	    scan_something_internal (dimen_val, false);
	    if (cur_val_level == dimen_val)
	      goto ATTACH_SIGN;
	  }
	  /* end expansion of Fetch an internal dimension and |goto attach_sign|, or fetch an internal integer */
	} else {
	  back_input();
	  if (cur_tok == continental_point_token)
		cur_tok = point_token;
	  if (cur_tok != point_token) {
		scan_int();
	  } else {
		radix = 10;
		cur_val = 0;
	  };
	  if (cur_tok == continental_point_token)
		cur_tok = point_token;
	  if ((radix == 10) && (cur_tok == point_token)) {
		/* begin expansion of Scan decimal fraction */
		/* module 452 */
		/* When the following code is executed, we have |cur_tok=point_token|, but this
		 * token has been backed up using |back_input|; we must first discard it.
		 * 
		 * It turns out that a decimal point all by itself is equivalent to `\.{0.0}'.
		 * Let's hope people don't use that fact.
		 */
		k = 0;
		p = null;
		get_token(); /* |point_token| is being re-scanned */ 
		loop {
		  get_x_token();
		  if ((cur_tok > zero_token + 9) || (cur_tok < zero_token)) {
			do_something;
			goto DONE1;
		  }
		  if (k < 17)	{ /* digits for |k>=17| cannot affect the result */
			q = get_avail ();
			link (q) = p;
			info (q) = cur_tok - zero_token;
			p = q;
			incr (k);
		  };
		};
	  DONE1:
		for (kk = k; kk >= 1; kk--) {
		  dig[kk - 1] = info (p);
		  q = p;
		  p = link (p);
		  free_avail (q);
		};
		f = round_decimals (k);
		if (cur_cmd != spacer)
		  back_input();
	  };
	  /* end expansion of Scan decimal fraction */
	};
  };
  if (cur_val < 0)	{ /* in this case |f=0| */
	negative = !negative;
	negate (cur_val);
  };
  /* begin expansion of Scan units and set |cur_val| to $x\cdot(|cur_val|+f/2^{16})$, 
	 where there are |x| sp per unit; |goto attach_sign| if the units are internal */
  /* module 453 */
  /* Now comes the harder part: At this point in the program, |cur_val| is a
   * nonnegative integer and $f/2^{16}$ is a nonnegative fraction less than 1;
   * we want to multiply the sum of these two quantities by the appropriate
   * factor, based on the specified units, in order to produce a |scaled|
   * result, and we want to do the calculation with fixed point arithmetic that
   * does not overflow.
   */
  if (inf) {
	/* begin expansion of Scan for \(f)\.{fil} units; |goto attach_fraction| if found */
	/* module 454 */
	/* A specification like `\.{filllll}' or `\.{fill L L L}' will lead to two
	 * error messages (one for each additional keyword \.{"l"}).
	 */
    if (scan_keyword ("fil")) {
	  cur_order = fil;
	  while (scan_keyword ("l")) {
		if (cur_order == filll) {
		  print_err ("Illegal unit of measure (");
		  zprint_string("replaced by filll)");
		  help1 ("I dddon't go any higher than filll.");
		  error();
		} else {
	      incr (cur_order);
		}
	  };
	  do_something;
	  goto ATTACH_FRACTION;
	};
	/* end expansion of Scan for \(f)\.{fil} units; |goto attach_fraction| if found */
  }
  /* begin expansion of Scan for \(u)units that are internal dimensions; |goto attach_sign|
	 with |cur_val| set if found */
  /* module 455 */
  save_cur_val = cur_val;
  /* Get the next non-blank non-call... */
  get_nblank_ncall;
  if ((cur_cmd < min_internal) || (cur_cmd > max_internal)) {
	back_input();
  } else {
	if (mu) {
	  scan_something_internal (mu_val, false);
	  /* Coerce glue... */
      coerce_glue;
	  if (cur_val_level != mu_val)
	    mu_error();
	} else {
	  scan_something_internal (dimen_val, false);
	}
	v = cur_val;
	goto FOUND;
  };
  if (mu)
    goto NOT_FOUND;
  if (scan_keyword ("em")) {
	/* begin expansion of The em width for |cur_font| */
	/* module 558 */
	/* Font parameters are referred to as |slant(f)|, |space(f)|, etc. */
	v = (quad (cur_font));
	/* end expansion of The em width for |cur_font| */
  } else if (scan_keyword ("ex"))  {
	/* begin expansion of The x-height for |cur_font| */
	/* module 559 */
	v = (x_height (cur_font));
	/* end expansion of The x-height for |cur_font| */
  } else {
	do_something;
    goto NOT_FOUND;
  }
  /* Scan an optional space */
  scan_optional_space;
 FOUND:
  cur_val = NX_PLUS_Y (save_cur_val, v, xn_over_d (v, f, 65536));
  goto ATTACH_SIGN;
  /* end expansion of Scan for \(u)units that are internal dimensions;... */
 NOT_FOUND:
  if (mu) {
	/* begin expansion of Scan for \(m)\.{mu} units and |goto attach_fraction| */
	/* module 456 */
    if (scan_keyword ("mu")) {
	  goto ATTACH_FRACTION;
	} else {
	  print_err ("Illegal unit of measure (");
	  zprint_string("mu inserted)");
	  help4( "The unit of measurement in math glue must be mu.",
			 "To recover gracefully from this error, it's best to",
			 "delete the erroneous units; e.g., type `2' to delete",
			 "two letters. (See Chapter 27 of The TeXbook.)");
	  error();
	  goto ATTACH_FRACTION;
	}
	/* end expansion of Scan for \(m)\.{mu} units and |goto attach_fraction| */
  };
  if (scan_keyword ("true")) {
	/* begin expansion of Adjust \(f)for the magnification ratio */
	/* module 457 */
	prepare_mag();
	if (mag != 1000) {
	  cur_val = xn_over_d (cur_val, 1000, mag);
	  f = (1000 * f + 65536 * tex_remainder) / mag;
	  cur_val = cur_val + (f / 65536);
	  f = f % 65536;
	};
	/* end expansion of Adjust \(f)for the magnification ratio */
  };
  if (scan_keyword ("pt"))
    goto ATTACH_FRACTION; /* the easy case */
  /* begin expansion of Scan for \(a)all other units and adjust |cur_val| and
   |f| accordingly; |goto done| in the case of scaled points */
  /* module 458 */
  /* The necessary conversion factors can all be specified exactly as
   * fractions whose numerator and denominator are 32768 or less.
   * According to the definitions here, $\rm2660\,dd\approx1000.33297\,mm$;
   * this agrees well with the value $\rm1000.333\,mm$ cited by Bosshard
   * 
   * in {\sl Technische Grundlagen zur Satzherstellung\/} (Bern, 1980).
   */
  if (scan_keyword ("in")) {
    set_conversion (7227,100);
  } else if (scan_keyword ("pc")) {
    set_conversion (12,1);
  } else if (scan_keyword ("cm")) {
    set_conversion (7227,254);
  } else if (scan_keyword ("mm")) {
    set_conversion (7227,2540);
  } else if (scan_keyword ("bp")) {
    set_conversion (7227,7200);
  } else if (scan_keyword ("dd")) {
    set_conversion (1238,1157);
  } else if (scan_keyword ("cc")) {
    set_conversion (14856,1157);
  } else if (scan_keyword ("sp")) {
	goto DONE; 
  } else {
	/* begin expansion of Complain about unknown unit and |goto done2| */
	/* module 459 */
	print_err ("Illegal unit of measure (");
	zprint_string("pt inserted)");
	help6 ("Dimensions can be in units of em, ex, in, pt, pc,",
		   "cm, mm, dd, cc, bp, or sp; but yours is a new one!",
		   "I'll assume that you meant to say pt, for printer's points.",
		   "To recover gracefully from this error, it's best to",
		   "delete the erroneous units; e.g., type `2' to delete",
		   "two letters. (See Chapter 27 of The TeXbook.)");
	error();
	goto DONE2;
	/* end expansion of Complain about unknown unit and |goto done2| */
  };
  cur_val = xn_over_d (cur_val, num, denom);
  f = (num * f + 65536 * tex_remainder) / denom;
  cur_val = cur_val + (f / 65536);
  f = f % 65536;
 DONE2:
  /* end expansion of Scan for \(a)all other units and adjust |cur_val| and |f| accordingly; ...*/
 ATTACH_FRACTION:
  if (cur_val >= 16384) {
	arith_error = true;
  } else {
    cur_val = cur_val * unity + f;
  }
 DONE:
  /* end expansion of Scan units and set |cur_val| to $x\cdot(|cur_val|+f/2^{16})$, where..*/
  /* Scan an optional space */
  scan_optional_space;
 ATTACH_SIGN:
  if (arith_error || (abs (cur_val) >= 1073741824)) {
	/* begin expansion of Report that this dimension is out of range */
	/* module 460 */
	print_err ("Dimension too large");
	help2 ("I can't work with sizes bigger than about 19 feet.",
		   "Continue and I'll use the largest value I can.");
	error();
	cur_val = max_dimen;
	arith_error = false;
  };
  /* end expansion of Report that this dimension is out of range */
  if (negative)
    negate (cur_val);
};


/* module 461 */

/* The final member of \TeX's value-scanning trio is |scan_glue|, which
 * makes |cur_val| point to a glue specification. The reference count of that
 * glue spec will take account of the fact that |cur_val| is pointing to~it.
 * 
 * The |level| parameter should be either |glue_val| or |mu_val|.
 * 
 * Since |scan_dimen| was so much more complex than |scan_int|, we might expect
 * |scan_glue| to be even worse. But fortunately, it is very simple, since
 * most of the work has already been done.
 */

void
scan_glue (small_number level) {/* sets |cur_val| to a glue spec pointer */
  boolean negative; /* should the answer be negated? */ 
  pointer q; /* new glue specification */ 
  boolean mu; /* does |level=mu_val|? */ 
  mu = (level == mu_val);
  /* Get the next non-blank non-sign... */
  get_next_nblank_nsign;
  if ((cur_cmd >= min_internal) && (cur_cmd <= max_internal)) {
	scan_something_internal (level, negative);
	if (cur_val_level >= glue_val) {
	  if (cur_val_level != level)
	    mu_error();
	  return;
	};
	if (cur_val_level == int_val) {
	  scan_dimen (mu, false, true);
	} else if (level == mu_val) {
	  mu_error();
	}
  } else {
	back_input();
	scan_dimen (mu, false, false);
	if (negative)
	  negate (cur_val);
  };
  /* begin expansion of Create a new glue specification whose width is |cur_val|; 
	 scan for its stretch and shrink components */
  /* module 462 */
  q = new_spec (zero_glue);
  width (q) = cur_val;
  if (scan_keyword ("plus")) {
	scan_dimen (mu, true, false);
	stretch (q) = cur_val;
	stretch_order (q) = cur_order;
  };
  if (scan_keyword ("minus")) {
	scan_dimen (mu, true, false);
	shrink (q) = cur_val;
	shrink_order (q) = cur_order;
  };
  cur_val = q;
  /* end expansion of Create a new glue specification whose width is |cur_val|; ...*/
}

/* module 463 */

/* Here's a similar procedure that returns a pointer to a rule node. This
 * routine is called just after \TeX\ has seen \.{\\hrule} or \.{\\vrule};
 * therefore |cur_cmd| will be either |hrule| or |vrule|. The idea is to store
 * the default rule dimensions in the node, then to override them if
 * `\.{height}' or `\.{width}' or `\.{depth}' specifications are
 * found (in any order).
 */
#define default_rule 26214


pointer
scan_rule_spec (void) {
  pointer q; /* the rule node being created */ 
  q = new_rule(); /* |width|, |depth|, and |height| all equal |null_flag| now */
  if (cur_cmd == vrule) {
    width (q) = default_rule;
  } else {
	height (q) = default_rule;
	depth (q) = 0;
  };
 RESWITCH:
  if (scan_keyword ("width")) {
	scan_normal_dimen;
	width (q) = cur_val;
	goto RESWITCH;
  };
  if (scan_keyword ("height")) {
	scan_normal_dimen;
	height (q) = cur_val;
	goto RESWITCH;
  };
  if (scan_keyword ("depth")) {
	scan_normal_dimen;
	depth (q) = cur_val;
	goto RESWITCH;
  };
  return q;
};


/* module 785 */

/* The parameters to |hpack| and |vpack| correspond to \TeX's primitives
 * like `\.{\\hbox} \.{to} \.{300pt}', `\.{\\hbox} \.{spread} \.{10pt}'; note
 * that `\.{\\hbox}' with no dimension following it is equivalent to
 * `\.{\\hbox} \.{spread} \.{0pt}'. The |scan_spec| subroutine scans such
 * constructions in the user's input, including the mandatory left brace that
 * follows them, and it puts the specification onto |save_stack| so that the
 * desired box can later be obtained by executing the following code:
 * $$\vbox{\halign{#\hfil\cr
 * |save_ptr:=save_ptr-2;|\cr
 * |hpack(p,saved(1),saved(0)).|\cr}}$$
 * Special care is necessary to ensure that the special |save_stack| codes
 * are placed just below the new group code, because scanning can change
 * |save_stack| when \.{\\csname} appears.
 */
void scan_spec (group_code c, boolean three_codes) { /* scans a box specification and left brace */
  int s; /* temporarily saved value */ 
  unsigned char spec_code;
  s = 0; /*TH -Wall*/
  if (three_codes)
	s = saved (0);
  if (scan_keyword ("to")) {
	spec_code = exactly;
  } else  if (scan_keyword ("spread")) {
	spec_code = additional;
  } else {
	spec_code = additional;
	cur_val = 0;
	goto FOUND;
  };
  scan_normal_dimen;
 FOUND:
  if (three_codes) {
	saved (0) = s;
	incr (save_ptr);
  };
  saved (0) = spec_code;
  saved (1) = cur_val;
  save_ptr = save_ptr + 2;
  new_save_level (c);
  scan_left_brace();
};


/* module 1628 */

/* The |scan_general_text| procedure is much like |scan_toks(false,false)|,
 * but will be invoked via |expand|, i.e., recursively.
 */

/* module 1629 */

/* The token list (balanced text) created by |scan_general_text| begins
 * at |link(temp_head)| and ends at |cur_val|. (If |cur_val=temp_head|,
 * the list is empty.)
 */

void
scan_general_text (void) {
  unsigned char s; /* to save |scanner_status| */ 
  pointer w; /* to save |warning_index| */ 
  pointer d; /* to save |def_ref| */ 
  pointer p; /* tail of the token list being built */ 
  pointer q; /* new node being added to the token list via |store_new_token| */
  halfword unbalance; /* number of unmatched left braces */ 
  s = scanner_status;
  w = warning_index;
  d = def_ref;
  scanner_status = absorbing;
  warning_index = cur_cs;
  def_ref = get_avail();
  token_ref_count (def_ref) = null;
  p = def_ref;
  scan_left_brace(); /* remove the compulsory left brace */ 
  unbalance = 1;
  loop {
    get_token();
    if (cur_tok < right_brace_limit) {
      if (cur_cmd < right_brace) {
		incr (unbalance);
	  } else {
		decr (unbalance);
		if (unbalance == 0)
		  goto FOUND;
	  }; 
	};
    store_new_token (cur_tok);
  };
FOUND:
  q = link (def_ref);
  free_avail (def_ref); /* discard reference count */ 
  if (q == null) {
    cur_val = temp_head;
  } else {
    cur_val = p;
  }
  link (temp_head) = q;
  scanner_status = s;
  warning_index = w;
  def_ref = d;
};

/* module 1728 */
#define expr_none 0
#define expr_add 1
#define expr_sub 2
#define expr_mult 3
#define expr_div 4
#define expr_scale 5

/* module 1733 */
#define expr_node_size 4
#define expr_e_field( arg )  mem [ arg  + 1 ]. cint
#define expr_t_field( arg )  mem [ arg  + 2 ]. cint
#define expr_n_field( arg )  mem [ arg  + 3 ]. cint

/* module 1735 */
#define glue_error( arg ) { arith_error   =  true ; delete_glue_ref ( arg ); arg   =  new_spec ( zero_glue );}

/* module 1736 */
#define normalize_glue(arg) if (stretch (arg) == 0) stretch_order (arg) = normal;\
                            if (shrink (arg) == 0) shrink_order (arg) = normal

/* module 1740 */
#define expr_m( arg )  arg   =  NX_PLUS_Y ( arg , f ,0 )
/* module 1741 */
#define expr_d( arg )  arg   =  quotient ( arg , f )
/* module 1743 */
#define expr_s( arg )  arg   =  fract ( arg , n , f , max_dimen )
/* module 1744 */


/* module 1727 */

/* The |scan_expr| procedure scans and evaluates an expression.
 */
#define expr_add_sub(a,b,c)        add_or_sub (a,b,c,r == expr_sub ) 
#define expr_a(a,b)                expr_add_sub (a,b,max_dimen)

void
scan_expr (void) {/* scans and evaluates an expression */
  boolean a, b; /* saved values of |arith_error| */ 
  small_number l; /* type of expression */ 
  small_number r; /* state of expression so far */ 
  small_number s; /* state of term so far */ 
  small_number o; /* next operation or type of next factor */ 
  int e; /* expression so far */ 
  int t; /* term so far */ 
  int f; /* current factor */ 
  int n; /* numerator of combined multiplication and division */ 
  pointer p; /* top of expression stack */ 
  pointer q; /* for stack manipulations */ 
  l = cur_val_level;
  a = arith_error;
  b = false;
  p = null;
  /* begin expansion of Scan and evaluate an expression |e| of type |l| */
  /* module 1728 */
  /* Evaluating an expression is a recursive process: When the left
   * parenthesis of a subexpression is scanned we descend to the next level
   * of recursion; the previous level is resumed with the matching right
   * parenthesis.
   */
 RESTART:
  r = expr_none;
  e = 0;
  s = expr_none;
  t = 0;
  n = 0;
 CONTINUE:
  if (s == expr_none) {
	o = l;
  } else {
    o = int_val;
  }
  /* begin expansion of Scan a factor |f| of type |o| or start a subexpression */
  /* module 1730 */
  /* Get the next non-blank non-call... */
  get_nblank_ncall;
  if (cur_tok == other_token + '(') {
	/* begin expansion of Push the expression stack and |goto restart| */
	/* module 1733 */
	/* Parenthesized subexpressions can be inside expressions, and this
	 * nesting has a stack. Seven local variables represent the top of the
	 * expression stack: |p| points to pushed-down entries, if any; |l|
	 * specifies the type of expression currently beeing evaluated; |e| is the
	 * expression so far and |r| is the state of its evaluation; |t| is the
	 * term so far and |s| is the state of its evaluation; finally |n| is the
	 * numerator for a combined multiplication and division, if any.
	 */
	q = get_node (expr_node_size);
	link (q) = p;
	type (q) = l;
	subtype (q) = 4 * s + r;
	expr_e_field (q) = e;
	expr_t_field (q) = t;
	expr_n_field (q) = n;
	p = q;
	l = o;
	goto RESTART;
	/* end expansion of Push the expression stack and |goto restart| */
  }
  back_input();
  if (o == int_val) {
    scan_int();
  } else  if (o == dimen_val) {
    scan_normal_dimen;
  } else  if (o == glue_val) {
    scan_normal_glue();
  } else {
    scan_mu_glue();
  }
  f = cur_val;
  /* end expansion of Scan a factor |f| of type |o| or start a subexpression */
 FOUND:
  /* begin expansion of Scan the next operator and set |o| */
  /* module 1729 */
  /* Get the next non-blank non-call... */
  get_nblank_ncall;
  if (cur_tok == other_token + '+') {
    o = expr_add;
  } else	if (cur_tok == other_token + '-') {
	o = expr_sub;
  } else  if (cur_tok == other_token + '*') {
	o = expr_mult;
  } else  if (cur_tok == other_token + '/') {
	o = expr_div;
  } else {
	o = expr_none;
	if (p == null) {
	  if (cur_cmd != relax)
	    back_input();
	} else if (cur_tok != other_token + ')') {
	  print_err ("Missing ) inserted for expression");
	  help1 ("I was expecting to see `+', `-', `*', `/', or `)'. Didn't.");
	  back_error();
	};
  };
  /* end expansion of Scan the next operator and set |o| */
  arith_error = b;
  /* begin expansion of Make sure that |f| is in the proper range */
  /* module 1735 */
  /* We want to make sure that each term and (intermediate) result is in
   * the proper range. Integer values must not exceed |infinity|
   * ($2^{31}-1$) in absolute value, dimensions must not exceed |max_dimen|
   * ($2^{30}-1$). We avoid the absolute value of an integer, because this
   * might fail for the value $-2^{31}$ using 32-bit arithmetic.
   */
  /* module 1736 */
  /* Applying the factor |f| to the partial term |t| (with the operator
   * |s|) is delayed until the next operator |o| has been scanned. Here we
   * handle the first factor of a partial term. A glue spec has to be copied
   * unless the next operator is a right parenthesis; this allows us later on
   * to simply modify the glue components.
   */
  if ((l == int_val) || (s > expr_sub)) {
	if ((f > infinity) || (f < -infinity))
	  num_error (f);
  } else if (l == dimen_val) {
	if (abs (f) > max_dimen)
	  num_error (f);
  } else {
	if ((abs (width (f)) > max_dimen) || (abs (stretch (f)) > max_dimen) || (abs (shrink (f)) > max_dimen))
	  glue_error (f);
  };
  /* end expansion of Make sure that |f| is in the proper range */
  switch (s) {
  case expr_none:
	if ((l >= glue_val) && (o != expr_none)) {
	  t = new_spec (f);
	  delete_glue_ref (f);
	  normalize_glue (t);
	} else {
	  t = f;
	}
	break;
	/* module 1740 */
	/* If a multiplication is followed by a division, the two operations are
	 * combined into a `scaling' operation. Otherwise the term |t| is
	 * multiplied by the factor |f|.
	 */
   case expr_mult:
	 if (o == expr_div) {
	   n = f;
	   o = expr_scale;
	 } else if (l == int_val) {
	   t = MULT_INTEGERS (t, f);
	 } else if (l == dimen_val) {
	   expr_m (t);
	 } else {
	   expr_m (width (t));
	   expr_m (stretch (t));
	   expr_m (shrink (t));
	 };
	 break;
	 /* module 1741 */
	 /* Here we divide the term |t| by the factor |f|.
	  */
  case expr_div:
	if (l < glue_val) {
	  expr_d (t);
	} else {
	  expr_d (width (t));
	  expr_d (stretch (t));
	  expr_d (shrink (t));
	};
	break;
	/* module 1743 */
	/* Here the term |t| is multiplied by the quotient $n/f$.
	 */
  case expr_scale:
	if (l == int_val) {
	  t = fract (t, n, f, infinity);
	} else if (l == dimen_val) {
	  expr_s (t);
	} else {
	  expr_s (width (t));
	  expr_s (stretch (t));
	  expr_s (shrink (t));
	};
	/* end expansion of Cases for evaluation of the current term */
  }; /* there are no other cases */ 
  if (o > expr_sub) {
    s = o;
  } else {
	/* begin expansion of Evaluate the current expression */
	/* module 1737 */
	/* When a term |t| has been completed it is copied to, added to, or
	 * subtracted from the expression |e|.
	 */
	s = expr_none;
	if (r == expr_none) {
	  e = t;
	} else if (l == int_val) {
	  e = expr_add_sub (e, t, infinity);
	} else  if (l == dimen_val) {
	  e = expr_a (e, t);
	} else {
	  /* begin expansion of Compute the sum or difference of two glue specs */
	  /* module 1739 */
	  /* We know that |stretch_order(e)>normal| implies |stretch(e)<>0| and
	   * |shrink_order(e)>normal| implies |shrink(e)<>0|.
	   */
	  width (e) = expr_a (width (e), width (t));
	  if (stretch_order (e) == stretch_order (t)) {
	    stretch (e) = expr_a (stretch (e), stretch (t));
	  } else if ((stretch_order (e) < stretch_order (t)) && (stretch (t) != 0)) {
		stretch (e) = stretch (t);
		stretch_order (e) = stretch_order (t);
	  };
	  if (shrink_order (e) == shrink_order (t)) {
	    shrink (e) = expr_a (shrink (e), shrink (t));
	  } else if ((shrink_order (e) < shrink_order (t)) && (shrink (t) != 0)) {
		shrink (e) = shrink (t);
		shrink_order (e) = shrink_order (t);
	  };
	  delete_glue_ref (t);
	  normalize_glue (e);
	  /* end expansion of Compute the sum or difference of two glue specs */
	};
	r = o;
	/* end expansion of Evaluate the current expression */
  }
  b = arith_error;
  if (o != expr_none)
    goto CONTINUE;
  if (p != null) {
	/* begin expansion of Pop the expression stack and |goto found| */
	/* module 1734 */
	f = e;
	q = p;
	e = expr_e_field (q);
	t = expr_t_field (q);
	n = expr_n_field (q);
	s = subtype (q) / 4;
	r = subtype (q) % 4;
	l = type (q);
	p = link (q);
	free_node (q, expr_node_size);
	goto FOUND;
	/* end expansion of Pop the expression stack and |goto found| */
  }
  /* end expansion of Scan and evaluate an expression |e| of type |l| */
  if (b) {
	print_err ("Arithmetic overflow");
	help2 ("I can't evaluate this expression,","since the result is out of range.");
	error();
	if (l >= glue_val) {
	  delete_glue_ref (e);
	  e = zero_glue;
	  add_glue_ref (e);
	} else {
	  e = 0;
	}
  };
  arith_error = a;
  cur_val = e;
  cur_val_level = l;
}


/* module 1732 */

/* Here we declare to trivial procedures in order to avoid mutually
 * recursive procedures with parameters.
 */
void
scan_normal_glue (void) {
  scan_glue (glue_val);
};

void
scan_mu_glue (void) {
  scan_glue (mu_val);
};


/* module 1756 */
void
scan_register_num (void) {
  scan_int();
  if ((cur_val < 0) || (cur_val > max_reg_num)) {
	print_err ("Bad register code");
	help2 (max_reg_help_line,"I changed this one to zero.");
	int_error (cur_val);
	cur_val = 0;
  };
};
