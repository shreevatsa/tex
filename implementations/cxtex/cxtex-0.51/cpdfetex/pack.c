
#include "types.h"
#include "c-compat.h"


#include "globals.h"

/* module 786 */

/* To figure out the glue setting, |hpack| and |vpack| determine how much
 * stretchability and shrinkability are present, considering all four orders
 * of infinity. The highest order of infinity that has a nonzero coefficient
 * is then used as if no other orders were present.
 * 
 * For example, suppose that the given list contains six glue nodes with
 * the respective stretchabilities 3pt, 8fill, 5fil, 6pt, $-3$fil, $-8$fill.
 * Then the total is essentially 2fil; and if a total additional space of 6pt
 * is to be achieved by stretching, the actual amounts of stretch will be
 * 0pt, 0pt, 15pt, 0pt, $-9$pt, and 0pt, since only `fil' glue will be
 * considered. (The `fill' glue is therefore not really stretching infinitely
 * with respect to `fil'; nobody would actually want that to happen.)
 * 
 * The arrays |total_stretch| and |total_shrink| are used to determine how much
 * glue of each kind is present. A global variable |last_badness| is used
 * to implement \.{\\badness}.
 */

scaled total_stretch[5], total_shrink[5]; /* glue found by |hpack| or  |vpack| */
int last_badness; /* badness of the most recently packaged box */

/* module 789 */
integer font_expand_ratio;
pointer last_leftmost_char;
pointer last_rightmost_char;

/* module 805 */

/* In order to provide a decent indication of where an overfull or underfull
 * box originated, we use a global variable |pack_begin_line| that is
 * set nonzero only when |hpack| is being called by the paragraph builder
 * or the alignment finishing routine.
 */

integer pack_begin_line; /* source file line where the current paragraph or 
							alignment began; a negative value denotes 
							alignment */


boolean check_expand_pars (internal_font_number f) {
  internal_font_number k;
  boolean check_expand_pars_r;
  check_expand_pars_r = false;
  if ((pdf_font_step[f] == 0) || ((pdf_font_stretch[f] == null_font)
								  && (pdf_font_shrink[f] == null_font)))
	return check_expand_pars_r;
  if (cur_font_step < 0) {
	cur_font_step = pdf_font_step[f];
  } else if (cur_font_step != pdf_font_step[f]) {
	pdf_error_string("HZ","using fonts with different step of expansion in one paragraph is not allowed");
  }
  k = pdf_font_stretch[f];
  if (k != null_font) {
	if (max_stretch_ratio < 0) {
	  max_stretch_ratio = pdf_font_expand_ratio[k];
	} else if (max_stretch_ratio != pdf_font_expand_ratio[k]) {
	  pdf_error_string("HZ","using fonts with different limit of expansion in one paragraph is not allowed");
	}
  };
  k = pdf_font_shrink[f];
  if (k != null_font) {
	if (max_shrink_ratio < 0) {
	  max_shrink_ratio = pdf_font_expand_ratio[k];
	} else if (max_shrink_ratio != pdf_font_expand_ratio[k]) {
	  pdf_error_string("HZ","using fonts with different limit of expansion in one paragraph is not allowed");
	}
  };
  check_expand_pars_r = true;
  return check_expand_pars_r;
};


scaled char_stretch (internal_font_number f, eight_bits c) {
  internal_font_number k;
  scaled dw; 
  int ef;
  scaled char_stretch_r ;
  char_stretch_r = 0;
  k = pdf_font_stretch[f];
  ef = get_ef_code (f, c);
  if ((k != null_font) && (ef > 0)) {
	dw = char_width (k, char_info (k, c)) -	char_width (f, char_info (f, c));
	if (dw > 0)
	  char_stretch_r = round_xn_over_d (dw, ef, 1000);
  };
  return char_stretch_r;
};

scaled char_shrink (internal_font_number f, eight_bits c) {
  internal_font_number k;
  scaled dw;
  int ef;
  scaled char_shrink_r;
  char_shrink_r = 0;
  k = pdf_font_shrink[f];
  ef = get_ef_code (f, c);
  if ((k != null_font) && (ef > 0)) {
	dw = char_width (f, char_info (f, c)) - char_width (k, char_info (k, c));
	if (dw > 0)
	  char_shrink_r = round_xn_over_d (dw, ef, 1000);
  };
  return char_shrink_r;
};

scaled get_kern (internal_font_number f, eight_bits lc, eight_bits rc) {
  scaled get_kern_r;
  four_quarters i;
  four_quarters j;
  font_index k;
  get_kern_r = 0;
  i = char_info (f, lc);
  if (char_tag (i) != lig_tag)
	return get_kern_r;
  k = lig_kern_start (f,i);
  j = font_info[k].qqqq;
  if (skip_byte (j) <= stop_flag)
	goto CONTINUEX;
  k = lig_kern_restart (f,j);
 CONTINUE:
  j = font_info[k].qqqq;
 CONTINUEX:
  if ((next_char (j) == rc) && (skip_byte (j) <= stop_flag) && (op_byte (j) >= kern_flag)) {
	get_kern_r = char_kern (f,j);
	return get_kern_r;
  };
  if (skip_byte (j) == qi (0)) {
	incr (k);
  } else {
	if (skip_byte (j) >= stop_flag)
	  return get_kern_r;
	k = k + qo (skip_byte (j)) + 1;
  };
  goto CONTINUE;
};

scaled kern_stretch (pointer p) { 
  scaled kern_stretch_r;
  pointer l, r;
  scaled d;
  kern_stretch_r = 0;
  if ((prev_char_p == null) || (link (prev_char_p) != p) || (link (p) == null))
	return kern_stretch_r;
  l = prev_char_p;
  r = link (p);
  if (type (l) == ligature_node)
	l = lig_char (l);
  if (type (r) == ligature_node)
	r = lig_char (r);
  if (!(is_char_node (l) && is_char_node (r)
		&& (font (l) = font (r))
		&& (pdf_font_stretch[font (l)] != null_font)))
	    return kern_stretch_r;
  d = get_kern (pdf_font_stretch[font (l)], character (l), character (r));
  kern_stretch_r = round_xn_over_d (d - width (p),get_ef_code (font (l), character (l)), 1000);
  return kern_stretch_r;
};

scaled kern_shrink (pointer p) { 
  scaled kern_shrink_r;
  pointer l, r;
  scaled d;
  kern_shrink_r = 0;
  if ((prev_char_p == null) || (link (prev_char_p) != p) || (link (p) == null))
	return kern_shrink_r;
  l = prev_char_p;
  r = link (p);
  if (type (l) == ligature_node)
	l = lig_char (l);
  if (type (r) == ligature_node)
	r = lig_char (r);
  if (!(is_char_node (l) && is_char_node (r)
		&& (font (l) = font (r))
		&& (pdf_font_shrink[font (l)] != null_font)))
	return kern_shrink_r;
  d = get_kern (pdf_font_shrink[font (l)], character (l), character (r));
  kern_shrink_r = round_xn_over_d (width (p) - d, get_ef_code (font (l), character (l)), 1000);
  return kern_shrink_r;
};

void do_subst_font (pointer p, int ex_ratio) {
  internal_font_number f, k;
  pointer r;
  int ef;
  r=null ; /*TH -Wall*/
  if (!is_char_node (p) && (type (p) == disc_node)) {
	r = pre_break (p);
	while (r != null) {
	  if (is_char_node (r) || (type (r) == ligature_node))
		do_subst_font (r, ex_ratio);
	  r = link (r);
	};
	r = post_break (p);
	while (r != null) {
	  if (is_char_node (r) || (type (r) == ligature_node))
		do_subst_font (r, ex_ratio);
	  r = link (r);
	};
	do_something;
	return;
  };
  if (is_char_node (p)) {
	r = p;
  } else if (type (p) == ligature_node) {
	r = lig_char (p);
  } else {
	short_display_n (p, 5);
	pdf_error_string("HZ","invalid node type");
  };
  f = font (r);
  ef = get_ef_code (f, character (r));
  if (ef == 0)
	return;
  if ((pdf_font_stretch[f] != null_font) && (ex_ratio > 0)) {
	k = expand_font (f, divide_scaled (ex_ratio * pdf_font_expand_ratio
									   [pdf_font_stretch[f]] * ef, 1000000, 0)) ;
  } else if ((pdf_font_shrink[f] != null_font) && (ex_ratio < 0)) {
	k = expand_font (f, -divide_scaled (ex_ratio *pdf_font_expand_ratio
					   [pdf_font_shrink[f]] * ef, 1000000, 0));
  } else {
	k = f;
  }
  if (k != f) {
	font (r) = k;
	if (!is_char_node (p)) {
	  r = lig_ptr (p);
	  while (r != null) {
		font (r) = k;
		r = link (r);
	  };
	};
  };
};

scaled char_pw (pointer p, small_number side) { 
  scaled char_pw_r;
  internal_font_number f;
  int c;
  char_pw_r = 0;
  if (side == left_side) {
	last_leftmost_char = null;
  } else {
	last_rightmost_char = null;
  }
  if (p == null)
	return char_pw_r;
  if (type (p) == ligature_node) {
	p = lig_char (p);
  } else if (!is_char_node (p)) {
	return char_pw_r;
  }
  f = font (p);
  if (side == left_side) {
	c = get_lp_code (f, character (p));
	last_leftmost_char = p;
  } else {
	c = get_rp_code (f, character (p));
	last_rightmost_char = p;
  };
  if (c == 0)
	return char_pw_r;
  char_pw_r = round_xn_over_d (quad (f), c, 1000);
  return char_pw_r;	
};

pointer new_margin_kern (scaled w, pointer p, small_number side) { 
  pointer k;
  k = get_node (margin_kern_node_size);
  type (k) = margin_kern_node;
  subtype (k) = side;
  width (k) = w;
  if (p == null)
	pdf_error_string("protruding","invalid pointer to marginal char node");
  fast_get_avail (margin_char (k));
  character (margin_char (k)) = character (p);
  font (margin_char (k)) = font (p);
  return k;
};


/* module 792 */
/* Clear dimensions to zero */
#define clear_dimensions     \
  d = 0;                     \
  x = 0;                     \
  total_stretch[normal] = 0; \
  total_shrink[normal] = 0;  \
  total_stretch[fil] = 0;    \
  total_shrink[fil] = 0;     \
  total_stretch[fill] = 0;   \
  total_shrink[fill] = 0;    \
  total_stretch[filll] = 0;  \
  total_shrink[filll] = 0

/* module 791 */

/* Here now is |hpack|, which is place where we do font substituting when
 * font expansion is being used.
 */
pointer 
hpack (pointer p, scaled w, small_number m) {
  pointer r; /* the box node that will be returned */ 
  pointer q; /* trails behind |p| */ 
  scaled h, d, x; /* height, depth, and natural width */ 
  scaled s; /* shift amount */ 
  pointer g; /* points to a glue specification */ 
  glue_ord o; /* order of infinity */ 
  internal_font_number f; /* the font in a |char_node| */ 
  four_quarters i; /* font information about a |char_node| */ 
  eight_bits hd; /* height and depth indices for a character */ 
  scaled font_stretch;
  scaled font_shrink;
  scaled k;
  font_stretch=0; font_shrink=0; /*TH -Wall*/
  last_badness = 0;
  r = get_node (box_node_size);
  type (r) = hlist_node;
  subtype (r) = min_quarterword;
  shift_amount (r) = 0;
  q = r + list_offset;
  link (q) = p;
  if (m == cal_expand_ratio) {
	prev_char_p = null;
	font_stretch = 0;
	font_shrink = 0;
	font_expand_ratio = 0;
  };
  h = 0;
  /* Clear dimensions to zero */
  clear_dimensions;
  if (TeXXeT_en) {
	/* Initialize the LR stack */
	initialize_LR_stack;
  }
  while (p != null) {
	/* begin expansion of Examine node |p| in the hlist, taking account of 
	   its effect on the dimensions of the new box, or moving it to the adjustment
	   list; then advance |p| to the next node */
	/* module 793 */
  RESWITCH:
	while (is_char_node (p)) {
	  /* begin expansion of Incorporate character dimensions into the dimensions
		 of the hbox that will contain~it, then move to the next node */
	  /* module 796 */
	  /* The following code is part of \TeX's inner loop; i.e., adding another
	   * character of text to the user's input will cause each of these instructions
	   * to be exercised one more time.
	   */
	  if (m >= cal_expand_ratio) {
		prev_char_p = p;
		switch (m) {
		case cal_expand_ratio:
		  f = font (p);
		  add_char_stretch (font_stretch, character (p));
		  add_char_shrink (font_shrink, character (p));
		  break;
		case subst_ex_font:
		  do_subst_font (p, font_expand_ratio);
		};
	  };
	  f = font (p);
	  i = char_info (f, character (p));
	  hd = height_depth (i);
	  x = x + char_width (f, i);
	  s = char_height (f,hd);
	  if (s > h)
		h = s;
	  s = char_depth (f,hd);
	  if (s > d)
		d = s;
	  p = link (p);
	};
	/* end expansion of Incorporate character dimensions into th.. */
	if (p != null) {
	  switch (type (p)) {
	  case hlist_node:
	  case vlist_node:
	  case rule_node:
	  case unset_node:
		/* begin expansion of Incorporate box dimensions into the dimensions of the hbox that will contain~it */
		/* module 795 */			
		/* The code here implicitly uses the fact that running dimensions are
		 * indicated by |null_flag|, which will be ignored in the calculations
		 * because it is a highly negative number.
		 */
		x = x + width (p);
		if (type (p) >= rule_node) {
		  s = 0;
		} else {
		  s = shift_amount (p);
		}
		if (height (p) - s > h)
		  h = height (p) - s;
		if (depth (p) + s > d)
		  d = depth (p) + s;
		/* end expansion of Incorporate box dimensions into the dimensions of the hbox that will contain~it */
		break;
	  case ins_node:
	  case mark_node:
	  case adjust_node:
		if (adjust_tail != null) {
		  /* begin expansion of Transfer node |p| to the adjustment list */
		  /* module 799 */
		  /* Materials in \.{\\vadjust} used with \.{pre} keyword will be appended to
		   * |pre_adjust_tail| instead of |adjust_tail|.
		   */
		  while (link (q) != p)
			q = link (q);
		  if (type (p) == adjust_node) {
			p = hpack_adjust_node(p);
			free_node (link (q), small_node_size);
		  } else {
			link (adjust_tail) = p;
			adjust_tail = p;
			p = link (p);
		  };
		  link (q) = p;
		  p = q;
		};
		/* end expansion of Transfer node |p| to the adjustment list */
		break;
	  case whatsit_node:
		/* begin expansion of Incorporate a whatsit node into an hbox */
		/* module 1556 */
		if ((subtype (p) == pdf_refxform_node) || (subtype (p) == pdf_refximage_node)) {
		  x = x + pdf_width (p);
		  s = 0;
		  if (pdf_height (p) - s > h)
			h = pdf_height (p) - s;
		  if (pdf_depth (p) + s > d)
			d = pdf_depth (p) + s;
		};
		/* end expansion of Incorporate a whatsit node into an hbox */
		break;
	  case glue_node:
		/* begin expansion of Incorporate glue into the horizontal totals */
		/* module 800 */
		g = glue_ptr (p);
		x = x + width (g);
		o = stretch_order (g);
		total_stretch[o] = total_stretch[o] + stretch (g);
		o = shrink_order (g);
		total_shrink[o] = total_shrink[o] + shrink (g);
		if (subtype (p) >= a_leaders) {
		  g = leader_ptr (p);
		  if (height (g) > h)
			h = height (g);
		  if (depth (g) > d)
			d = depth (g);
		};
		/* end expansion of Incorporate glue into the horizontal totals */
		break;
	  case math_node:
		x = x + width (p);
		if (TeXXeT_en) {
		  /* begin expansion of Adjust \(t)the LR stack for the |hpack| routine */
		  /* module 1659 */
		  if (end_LR (p)) {
			if (info (LR_ptr) == end_LR_type (p)) {
			  pop_LR;
			} else  {
			  incr (LR_problems);
			  type (p) = kern_node;
			  subtype (p) = explicit;
			}
		  } else {
			push_LR (p);
		  }
		  /* end expansion of Adjust \(t)the LR stack for the |hpack| routine */
		};
		break;
	  case margin_kern_node:
		if (m == cal_expand_ratio) {
		  f = font (margin_char (p));
		  do_subst_font (margin_char (p), 1000);
		  if (f != font (margin_char (p)))
			font_stretch = font_stretch - width (p) - char_pw (margin_char (p), subtype (p));
		  font (margin_char (p)) = f;
		  do_subst_font (margin_char (p), -1000);
		  if (f != font (margin_char (p)))
			font_shrink =	font_shrink - width (p) -char_pw (margin_char (p), subtype (p));
		  font (margin_char (p)) = f;
		} else if (m == subst_ex_font) {
		  do_subst_font (margin_char (p),font_expand_ratio);
		  width (p) = -char_pw (margin_char (p), subtype (p));
		};
		x = x + width (p);
		break;
	  case kern_node:
		if ((m == cal_expand_ratio) && (subtype (p) == normal)) {
		  k = kern_stretch (p);
		  if (k != 0) {
			subtype (p) = substituted;
			font_stretch = font_stretch + k;
		  };
		  k = kern_shrink (p);
		  if (k != 0) {
			subtype (p) = substituted;
			font_shrink = font_shrink + k;
		  };
		} else if ((m == subst_ex_font) && (subtype (p) == substituted)) {
		  if (type (link (p)) == ligature_node) {
			width (p) = get_kern (font (prev_char_p), character (prev_char_p), character (lig_char (link (p))));
		  } else {
			width (p) =get_kern (font (prev_char_p), character (prev_char_p),  character (link (p))); 
		  };
		}
		x = x + width (p);
		break;
	  case ligature_node:
		if (m == subst_ex_font)
		  do_subst_font (p, font_expand_ratio);
		/* begin expansion of Make node |p| look like a |char_node| and |goto reswitch| */
		p = mimic_char_node (p);
		goto RESWITCH;
	  case disc_node:
		if (m == subst_ex_font)
		  do_subst_font (p, font_expand_ratio);
		break;
	  default:
		do_nothing;
	  };
	  p = link (p);
	};
  }
  /* end expansion of Examine node |p| in the hlist, taking account o.. */
  if (adjust_tail != null)
	link (adjust_tail) = null;
  if (pre_adjust_tail != null)
	link (pre_adjust_tail) = null;
  height (r) = h;
  depth (r) = d;
  /* begin expansion of Determine the value of |width(r)| and the appropriate 
	 glue setting; then |return| or |goto common_ending| */
  /* module 801 */
  /* When we get to the present part of the program, |x| is the natural width
   * of the box being packaged.
   */
  if (m == additional)
	w = x + w;
  width (r) = w;
  x = w - x; /* now |x| is the excess to be made up */ 
  if (x == 0) {
	glue_sign (r) = normal;
	glue_order (r) = normal;
	set_glue_ratio_zero (glue_set (r));
	goto EXIT;
  } else if (x > 0) {
	/* begin expansion of Determine horizontal glue stretch setting, 
	   then |return| or \hbox{|goto common_ending|} */
	/* module 802 */
	/* If |hpack| is called with |m=cal_expand_ratio| we calculate
	 * |font_expand_ratio| and return without checking for overfull or underfull box.
	 */
	/* Determine the stretch order */
	determine_stretch_order;
	if ((m == cal_expand_ratio) && (o == normal) && (font_stretch > 0)) {
	  font_expand_ratio = divide_scaled (x, font_stretch, 3);
	  goto EXIT;
	};
	glue_order (r) = o;
	glue_sign (r) = stretching;
	if (total_stretch[o] != 0) {
	  glue_set (r) = unfloat (x /(double) total_stretch[o]) ;
	} else {
	  glue_sign (r) = normal;
	  set_glue_ratio_zero (glue_set (r)); /* there's nothing to stretch */ 
	};
	if (o == normal) {
	  if (list_ptr (r) != null) {
		/* begin expansion of Report an underfull hbox and |goto common_ending|, 
		   if this box is sufficiently bad */
		/* module 804 */
		last_badness = badness (x, total_stretch[normal]);
		if (last_badness > hbadness) {
		  print_ln();
		  if (last_badness > 100) {
			print_nl_string("Underfull");
		  } else {
			print_nl_string("Loose");
		  }
		  zprint_string(" \\hbox (badness ");
		  print_int (last_badness);
		  goto COMMON_ENDING;
		};
	  };
	}
	/* end expansion of Report an underfull hbox and |goto common_ending|, if this box is sufficiently bad */
	do_something;
	goto EXIT;
	/* end expansion of Determine horizontal glue stretch setting, then |return| or ... */
  } else {
	/* begin expansion of Determine horizontal glue shrink setting, 
	   then |return| or \hbox{|goto common_ending|} */
	/* module 808 */
	/* Determine the shrink order */
	determine_shrink_order;
	if ((m == cal_expand_ratio) && (o == normal) && (font_shrink > 0)) {
	  font_expand_ratio = divide_scaled (x, font_shrink, 3);
	  goto EXIT;
	};
	glue_order (r) = o;
	glue_sign (r) = shrinking;
	if (total_shrink[o] != 0) {
	  glue_set (r) = unfloat ((-x) / (double) total_shrink[o]);
	} else {
	  glue_sign (r) = normal;
	  set_glue_ratio_zero (glue_set (r));	 /* there's nothing to shrink */ 
	};
	if ((total_shrink[o] < -x) && (o == normal) && (list_ptr (r) != null)) {
	  last_badness = 1000000;
	  set_glue_ratio_one (glue_set (r)); /* use the maximum shrinkage */
	  /* begin expansion of Report an overfull hbox and |goto common_ending|, if this box is sufficiently bad */
	  /* module 810 */
	  if ((-x - total_shrink[normal] > hfuzz) || (hbadness < 100)) {
		if ((overfull_rule > 0) && (-x - total_shrink[normal] > hfuzz)) {
		  while (link (q) != null)
			q = link (q);
		  link (q) = new_rule();
		  width (link (q)) = overfull_rule;
		};
		print_ln();
		print_nl_string("Overfull \\hbox (");
		print_scaled (-x - total_shrink[normal]);
		zprint_string("pt too wide");
		goto COMMON_ENDING;
	  };
	  /* end expansion of Report an overfull hbox and |goto common_ending|, if this box is sufficiently bad */
	} else if (o == normal) {
	  if (list_ptr (r) != null) {
		/* begin expansion of Report a tight hbox and |goto common_ending|, if this box is sufficiently bad */
		/* module 811 */
		last_badness = badness (-x, total_shrink[normal]);
		if (last_badness > hbadness) {
		  print_ln();
		  print_nl_string("Tight \\hbox (badness ");
		  print_int (last_badness);
		  goto COMMON_ENDING;
		};
	  };
	  /* end expansion of Report a tight hbox and |goto common_ending|, if this box is sufficiently bad */
	}
	do_something;
	goto EXIT;
  };
  /* end expansion of Determine horizontal glue shrink setting, then |return| or \hbox{|goto common_ending|} */
  /* end expansion of Determine the value of |width(r)| and the appropriate ....*/
 COMMON_ENDING:
  /* begin expansion of Finish issuing a diagnostic message for an overfull or underfull hbox */
  /* module 807 */
  if (output_active) {
	zprint_string(") has occurred while \\output is active");
  } else {
	if (pack_begin_line != 0) {
	  if (pack_begin_line > 0) {
		zprint_string(") in paragraph at lines ");
	  } else {
		zprint_string(") in alignment at lines ");
	  }
	  print_int (abs (pack_begin_line));
	  zprint_string("--");
	} else {
	  zprint_string(") detected at line ");
	}
	print_int (line);
  };
  print_ln();
  font_in_short_display = null_font;
  short_display (list_ptr (r));
  print_ln();
  begin_diagnostic();
  show_box (r);
  end_diagnostic (true);
  /* end expansion of Finish issuing a diagnostic message for an overfull or underfull hbox */
 EXIT:
  if (TeXXeT_en) {
	/* begin expansion of Check for LR anomalies at the end of |hpack| */
	/* module 1660 */
	if (info (LR_ptr) != before) {
	  while (link (q) != null)
		q = link (q);
	  do {
		temp_ptr = q;
		q = new_math (0, info (LR_ptr));
		link (temp_ptr) = q;
		LR_problems = LR_problems + 10000;
		pop_LR;
	  } while (info (LR_ptr) != before);
	};
	if (LR_problems > 0) {
	  /* Report LR problems */
	  report_LR_problems;
	  goto COMMON_ENDING;
	};
	pop_LR;
	if (LR_ptr != null)
	  confusion ("LR1");
  };
  /* end expansion of Check for LR anomalies at the end of |hpack| */
  if ((m == cal_expand_ratio) && (font_expand_ratio != 0)) {
	font_expand_ratio = fix_int (font_expand_ratio, -1000, 1000);
	q = list_ptr (r);
	free_node (r, box_node_size);
	r = hpack (q, w, subst_ex_font);
  };
  return r;
}


/* module 812 */

/* The |vpack| subroutine is actually a special case of a slightly more
 * general routine called |vpackage|, which has four parameters. The fourth
 * parameter, which is |max_dimen| in the case of |vpack|, specifies the
 * maximum depth of the page box that is constructed. The depth is first
 * computed by the normal rules; if it exceeds this limit, the reference
 * point is simply moved down until the limiting depth is attained.
 */
pointer 
vpackage (pointer p, scaled h, small_number m, scaled l) {
  pointer r; /* the box node that will be returned */ 
  scaled w, d, x; /* width, depth, and natural height */ 
  scaled s; /* shift amount */ 
  pointer g; /* points to a glue specification */ 
  glue_ord o; /* order of infinity */ 
  last_badness = 0;
  r = get_node (box_node_size);
  type (r) = vlist_node;
  subtype (r) = min_quarterword;
  shift_amount (r) = 0;
  list_ptr (r) = p;
  w = 0;
  /* Clear dimensions to zero */
  clear_dimensions;
  while (p != null) {
	/* begin expansion of Examine node |p| in the vlist, taking account of its effect on the 
	   dimensions of the new box; then advance |p| to the next node */
	/* module 813 */
	if (is_char_node (p)) {
	  confusion ("vpack");
	} else
	  switch (type (p)) {
	  case hlist_node:
	  case vlist_node:
	  case rule_node:
	  case unset_node:
		/* begin expansion of Incorporate box dimensions into the dimensions of the vbox that will contain~it */
		/* module 814 */
		x = x + d + height (p);
		d = depth (p);
		if (type (p) >= rule_node) {
		  s = 0;
		} else {
		  s = shift_amount (p);
		}
		if (width (p) + s > w)
		  w = width (p) + s;
		/* end expansion of Incorporate box dimensions into the dimensions of the vbox that will contain~it */
		break;
	  case whatsit_node:
		/* begin expansion of Incorporate a whatsit node into a vbox */
		/* module 1555 */
		if ((subtype (p) == pdf_refxform_node) || (subtype (p) == pdf_refximage_node)) {
		  x = x + d + pdf_height (p);
		  d = pdf_depth (p);
		  s = 0;
		  if (pdf_width (p) + s > w)
			w = pdf_width (p) + s;
		}
		/* end expansion of Incorporate a whatsit node into a vbox */
		break;
	  case glue_node:
		/* begin expansion of Incorporate glue into the vertical totals */
		/* module 815 */
		x = x + d;
		d = 0;
		g = glue_ptr (p);
		x = x + width (g);
		o = stretch_order (g);
		total_stretch[o] = total_stretch[o] + stretch (g);
		o = shrink_order (g);
		total_shrink[o] = total_shrink[o] + shrink (g);
		if (subtype (p) >= a_leaders) {
		  g = leader_ptr (p);
		  if (width (g) > w)
			w = width (g);
		};
		/* end expansion of Incorporate glue into the vertical totals */
		break;
	  case kern_node:
		x = x + d + width (p);
		d = 0;
		break;
	  default:
		do_nothing;
	  };
	p = link (p);
  }
  /* end expansion of Examine node |p| in the vlist, taking account of its ...*/
  width (r) = w;
  if (d > l) {
	x = x + d - l;
	depth (r) = l;
  } else {
	depth (r) = d;
  }
  /* begin expansion of Determine the value of |height(r)| and the appropriate 
	 glue setting; then |return| or |goto common_ending| */
  /* module 816 */
  /* When we get to the present part of the program, |x| is the natural height
   * of the box being packaged.
   */
  if (m == additional)
	h = x + h;
  height (r) = h;
  x = h - x; /* now |x| is the excess to be made up */ 
  if (x == 0) {
	glue_sign (r) = normal;
	glue_order (r) = normal;
	set_glue_ratio_zero (glue_set (r));
	return r;
  } else if (x > 0) {
	/* begin expansion of Determine vertical glue stretch setting, 
	   then |return| or \hbox{|goto common_ending|} */
	/* module 817 */
	/* Determine the stretch order */
	determine_stretch_order;
	glue_order (r) = o;
	glue_sign (r) = stretching;
	if (total_stretch[o] != 0) {
	  glue_set (r) = unfloat (x / (double) total_stretch[o]);
	} else {
	  glue_sign (r) = normal;
	  set_glue_ratio_zero (glue_set (r)); /* there's nothing to stretch */ 
	};
	if (o == normal)
	  if (list_ptr (r) != null) {
		/* begin expansion of Report an underfull vbox and |goto common_ending|,
		   if this box is sufficiently bad */
		/* module 818 */
		last_badness = badness (x, total_stretch[normal]);
		if (last_badness > vbadness) {
		  print_ln();
		  if (last_badness > 100) {
			print_nl_string("Underfull");
		  } else {
			print_nl_string("Loose");
		  }
		  zprint_string(" \\vbox (badness ");
		  print_int (last_badness);
		  goto COMMON_ENDING;
		};
	  };
	/* end expansion of Report an underfull vbox and |goto common_ending|, if this box is sufficiently bad */
	return r;
	/* end expansion of Determine vertical glue stretch setting, then |return| or \hbox{|goto common_ending|} */
  } else {
	/* begin expansion of Determine vertical glue shrink setting, then |return| 
	   or \hbox{|goto common_ending|} */
	/* module 820 */
	/* Determine the shrink order */
	determine_shrink_order;
	glue_order (r) = o;
	glue_sign (r) = shrinking;
	if (total_shrink[o] != 0) {
	  glue_set (r) = unfloat ((-x) / (double) total_shrink[o]);
	} else {
	  glue_sign (r) = normal;
	  set_glue_ratio_zero (glue_set (r)); /* there's nothing to shrink */ 
	};
	if ((total_shrink[o] < -x) && (o == normal) && (list_ptr (r) != null)) {
	  last_badness = 1000000;
	  set_glue_ratio_one (glue_set (r)); /* use the maximum shrinkage */
	  /* begin expansion of Report an overfull vbox and |goto common_ending|, if this box is sufficiently bad */
	  /* module 821 */
	  if ((-x - total_shrink[normal] > vfuzz) || (vbadness < 100)) {
		print_ln();
		print_nl_string("Overfull \\vbox (");
		print_scaled (-x - total_shrink[normal]);
		zprint_string("pt too high");
		goto COMMON_ENDING;
	  }
	  /* end expansion of Report an overfull vbox and |goto common_ending|, if this box is sufficiently bad */
	} else if (o == normal)
	  if (list_ptr (r) != null) {
		/* begin expansion of Report a tight vbox and |goto common_ending|, if this box is sufficiently bad */
		/* module 822 */
		last_badness = badness (-x, total_shrink[normal]);
		if (last_badness > vbadness) {
		  print_ln();
		  print_nl_string("Tight \\vbox (badness ");
		  print_int (last_badness);
		  goto COMMON_ENDING;
		};
	  }
	/* end expansion of Report a tight vbox and |goto common_ending|, if this box is sufficiently bad */
	return r;
  }
  /* end expansion of Determine vertical glue shrink setting, then |return| or \hbox{|goto common_ending|} */
  /* end expansion of Determine the value of |height(r)| and the appropriate glue setting; ... */
 COMMON_ENDING:
  /* begin expansion of Finish issuing a diagnostic message for an overfull or underfull vbox */
  /* module 819 */
  if (output_active) {
	zprint_string(") has occurred while \\output is active");
  } else {
	if (pack_begin_line != 0) { /* it's actually negative */
	  zprint_string(") in alignment at lines ");
	  print_int (abs (pack_begin_line));
	  zprint_string("--");
	} else {
	  zprint_string(") detected at line ");
	}
	print_int (line);
	print_ln();
  };
  begin_diagnostic();
  show_box (r);
  end_diagnostic (true);
  /* end expansion of Finish issuing a diagnostic message for an overfull or underfull vbox */
  return r;
}


/* module 823 */

/* When a box is being appended to the current vertical list, the
 * baselineskip calculation is handled by the |append_to_vlist| routine.
 */
void 
append_to_vlist (pointer b) {
  scaled d; /* deficiency of space between baselines */ 
  pointer p; /* a new glue specification */ 
  if (prev_depth > ignore_depth) {
	d = width (baseline_skip) - prev_depth - height (b);
	if (d < line_skip_limit) {
	  p = new_param_glue (line_skip_code);
	} else {
	  p = new_skip_param (baseline_skip_code);
	  width (temp_ptr) = d; /* |temp_ptr=glue_ptr(p)| */ 
	};
	link (tail) = p;
	tail = p;
  };
  link (tail) = b;
  tail = b;
  prev_depth = depth (b);
}


void 
pack_initialize (void) {
  /* module 788 */
  last_badness = 0;
  /* module 806 */
  pack_begin_line = 0;
}
