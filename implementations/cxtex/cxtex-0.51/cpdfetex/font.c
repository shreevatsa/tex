
#include "types.h"
#include "c-compat.h"
#include "globals.h"

/* module 176 */

/* The |show_node_list| routine requires some auxiliary subroutines: one to
 * zprint a font-and-character combination, one to zprint a token list without
 * its reference count, and one to zprint a rule dimension.
 */
void
print_font_and_char (int p) { /* prints |char_node| data */
  if (p > mem_end) {
	print_esc_string ("CLOBBERED.");
  } else {
	if ((font (p) > font_max)) {
	  print_char ('*');
	} else
	  /* Print the font identifier for |font(p)| */
	  print_font_ident(p);
	print_ASCII (qo (character (p)));
  };
};


/* module 549 */

/* Here now is the (rather formidable) array of font arrays. */
fmemory_word *font_info; /* the big collection of font data */ 
font_index fmem_ptr; /* first unused word of |font_info| */
internal_font_number font_ptr; /* largest internal font number in use */
four_quarters *font_check; /* check sum */
scaled *font_size; /* ``at'' size */
scaled *font_dsize; /* ``design'' size */
font_index *font_params; /* how many font parameters are present */
str_number *font_name; /* name of the font */
str_number *font_area; /* area of the font */
eight_bits *font_bc; /* beginning (smallest) character code */
eight_bits *font_ec; /* ending (largest) character code */
pointer *font_glue; /* glue specification for interword space,  |null| if not allocated */
boolean  *font_used; /* has a character from this font actually  appeared in the output? */
integer *hyphen_char; /* current \.{\\hyphenchar} values */
integer *skew_char; /* current \.{\\skewchar} values */
font_index *bchar_label; /* start of |lig_kern| program for left boundary 
							character, |non_address| if there is none */
nine_bits *font_bchar; /* right boundary character, |non_char| if there  is none */
nine_bits  *font_false_bchar; /* |font_bchar| if it doesn't exist in the 
								 font, otherwise |non_char| */

/* module 550 */

/* Besides the arrays just enumerated, we have directory arrays that make it
 * easy to get at the individual entries in |font_info|. For example, the
 * |char_info| data for character |c| in font |f| will be in
 * |font_info[char_base[f]+c].qqqq|; and if |w| is the |width_index|
 * part of this word (the |b0| field), the width of the character is
 * |font_info[width_base[f]+w].sc|. (These formulas assume that
 * |min_quarterword| has already been added to |c| and to |w|, since \TeX\
 * stores its quarterwords that way.)
 */

integer *char_base; /* base addresses for |char_info| */
integer *width_base; /* base addresses for widths */
integer *height_base; /* base addresses for heights */
integer *depth_base; /* base addresses for depths */
integer *italic_base; /* base addresses for italic corrections */
integer *lig_kern_base; /* base addresses for ligature/kerning programs */
integer *kern_base; /* base addresses for kerns */
integer *exten_base; /* base addresses for extensible recipes */
integer *param_base; /* base addresses for font parameters */

/* module 555 */

/* The global variable |null_character| is set up to be a word of
 * |char_info| for a character that doesn't exist. Such a word provides a
 * convenient way to deal with erroneous situations.
 */
four_quarters null_character; /* nonexistent character information */


/* module 577 */

/* Before we forget about the format of these tables, let's deal with two
 * of \TeX's basic scanning routines related to font information.
 */
void
scan_font_ident (void) {
  internal_font_number f;
  halfword m;
  /* Get the next non-blank non-call... */
  get_nblank_ncall;
  if (cur_cmd == def_font) {
	f = cur_font;
  } else if (cur_cmd == set_font) {
	f = cur_chr;
  } else if (cur_cmd == def_family) {
	m = cur_chr;
	scan_four_bit_int();
	f = equiv (m + cur_val);
  } else {
	print_err ("Missing font identifier");
	help2 ("I was looking for a control sequence whose",
		   "current meaning has been defined by \\font.");
	back_error();
	f = null_font;
  };
  cur_val = f;
}

/* module 578 */

/* The following routine is used to implement `\.{\\fontdimen} |n| |f|'.
 * The boolean parameter |writing| is set |true| if the calling program
 * intends to change the parameter value.
 */

void
find_font_dimen (boolean writing) {	/* sets |cur_val| to |font_info| location */
  internal_font_number f;
  int n; /* the parameter number */ 
  scan_int();
  n = cur_val;
  scan_font_ident();
  f = cur_val;
  if (n <= 0) {
	cur_val = fmem_ptr;
  } else {
	if (writing && (n <= space_shrink_code) && (n >= space_code) && (font_glue[f] != null)) {
	  delete_glue_ref (font_glue[f]);
	  font_glue[f] = null;
	};
	if (n > font_params[f]) {
	  if (f < font_ptr) {
	    cur_val = fmem_ptr;
	  } else {
		/* begin expansion of Increase the number of parameters in the last font */
		/* module 580 */
	    do {
		  if (fmem_ptr == font_mem_size)
			overflow ("font memory", font_mem_size);
		  font_info[fmem_ptr].sc = 0;
		  incr (fmem_ptr);
		  incr (font_params[f]);
		} while (n != font_params[f]);
	    cur_val = fmem_ptr - 1; /* this equals |param_base[f]+font_params[f]| */ 
	  }
	  /* end expansion of Increase the number of parameters in the last font */
	} else {
	  cur_val = n + param_base[f];
	}
  };
  /* begin expansion of Issue an error message if |cur_val=fmem_ptr| */
  /* module 579 */
  if (cur_val == fmem_ptr) {
	print_err ("Font ");
	print_esc (font_id_text (f));
	zprint_string (" has only ");
	print_int (font_params[f]);
	zprint_string (" fontdimen parameters");
	help2 ("To increase the number of font parameters, you must",
		   "use \\fontdimen immediately after the \\font is loaded.");
	error();
  }
  /* end expansion of Issue an error message if |cur_val=fmem_ptr| */
}

/* module 581 */

/* When \TeX\ wants to typeset a character that doesn't exist, the
 * character node is not created; thus the output routine can assume
 * that characters exist when it sees them. The following procedure
 * prints a warning message unless the user has suppressed it.
 */
void
char_warning (internal_font_number f, eight_bits c) {
  int old_setting; /* saved value of |tracing_online| */ 
  if (tracing_lost_chars > 0) {
	old_setting = tracing_online;
	if (eTeX_ex && (tracing_lost_chars > 1))
	  tracing_online = 1;
	begin_diagnostic();
	print_nl_string("Missing character: There is no ");
	print_ASCII (c);
	zprint_string(" in font ");
	slow_print (font_name[f]);
	print_char ('!');
	end_diagnostic (false);
	tracing_online = old_setting;
  };
}


/* module 582 */

/* Here is a function that returns a pointer to a character node for a
 * given character in a given font. If that character doesn't exist,
 * |null| is returned instead.
 * 
 * This allows a character node to be used if there is an equivalent
 * in the |char_sub_code| list.
 */
pointer
new_character (internal_font_number f, eight_bits c) {
  pointer p; /* newly allocated node */ 
  quarterword ec; /* effective character of |c| */ 
  ec = effective_char (false, f, qi (c));
  if (font_bc[f] <= qo (ec))
    if (font_ec[f] >= qo (ec))
      if (char_exists (orig_char_info (f, ec))) { /* N.B.: not |char_info| */
		p = get_avail();
		font (p) = f;
		character (p) = qi (c);
		return p;
	  };
  char_warning (f, c);
  return null;
}

/* module 647 */

/* The subroutines define the corresponding macros so we can use them
 * in C.
 */
	
internal_font_number 
get_null_font (void) {
  return null_font;
};

scaled 
get_x_height (internal_font_number f) {
  return x_height (f);
};

scaled 
get_char_width (internal_font_number f, eight_bits c) {
  if (is_valid_char (c)) {
    return char_width (f, orig_char_info (f,c));
  } else {
    return 0;
  }
};

scaled 
get_char_height (internal_font_number f, eight_bits c) {
  if (is_valid_char (c)) {
    return char_height (f,height_depth (orig_char_info (f,c)));
  } else {
    return 0;
  }
};

scaled 
get_char_depth (internal_font_number f, eight_bits c) {
  if (is_valid_char (c)) {
    return char_depth (f,height_depth (orig_char_info (f,c)));
  } else {
    return 0;
  }
};

scaled 
get_quad (internal_font_number f)   {
  return quad (f);
};

scaled 
get_slant (internal_font_number f) {
  return slant (f);
};

internal_font_number 
new_null_font (void) {
  return  read_font_info (null_cs,slow_make_tex_string("dummy"),null_string, -1000);
};

internal_font_number 
get_tfm_num (str_number s) {
  internal_font_number k;
  for (k = font_base + 1; k <= font_ptr; k++)
    if (str_eq_str (font_name[k], s)) {
	  flush_fontname_k (s);
	  if (pdf_font_expand_ratio[k] == 0)
		return k;
	};
  k = read_font_info (null_cs, s,null_string, -1000);
  return k;
};


/* module 664 */

/* Next subroutines are needed for controling spacing in PDF page description.
 * The procedure |adv_char_width| advances |pdf_h| by the amount |w|, which is
 * the character width. We cannot simply add |w| to |pdf_h|, but must
 * calculate the amount corresponding to |w| in the PDF output. For PK fonts
 * things are more complicated, as we have to deal with scaling bitmaps as well.
 */
void 
adv_char_width (internal_font_number f, scaled w) {
  /* update |pdf_h| by character width |w| from font |f| */
  if (pdf_font_map[f] >= 0) {
	divide_scaled (w, pdf_font_size[f], 3);
	pdf_h = pdf_h + scaled_out;
  } else {
	pdf_h = pdf_h + get_pk_char_width (f, w);
  };
}


/* module 1258 */
void 
append_italic_correction (void) {
  pointer p; /* |char_node| at the tail of the current list */
  internal_font_number f; /* the font in the |char_node| */ 
  if (tail != head) {
	if (is_char_node (tail)) {
	  p = tail;
	} else if (type (tail) == ligature_node) {
	  p = lig_char (tail);
	} else {
	  do_something;
	  return;
	}
	f = font (p);
	tail_append (new_kern (char_italic (f, char_info (f, character (p)))));
	subtype (tail) = explicit;
  };
}

/* module 1268 */

/* The positioning of accents is straightforward but tedious. Given an accent
 * of width |a|, designed for characters of height |x| and slant |s|;
 * and given a character of width |w|, height |h|, and slant |t|: We will shift
 * the accent down by |x-h|, and we will insert kern nodes that have the effect of
 * centering the accent over the character and shifting the accent to the
 * right by $\delta={1\over2}(w-a)+h\cdot t-x\cdot s$. If either character is
 * absent from the font, we will simply use the other, without shifting.
 */
void 
make_accent (void) {
  real s, t; /* amount of slant */ 
  pointer p, q, r; /* character, box, and kern nodes */ 
  internal_font_number f; /* relevant font */ 
  scaled a, h, x, w, delta; /* heights and widths, as explained above */ 
  four_quarters i; /* character information */ 
  scan_char_num();
  f = cur_font;
  p = new_character (f, cur_val);
  if (p != null) {
	x = x_height (f);
	s = slant (f) / FLOAT_CONSTANT (65536);
	a = char_width (f, char_info (f, character (p)));
	do_assignments();
	/* begin expansion of Create a character node |q| for the next character, but set
	   |q:=null| if problems arise */
	/* module 1269 */
	q = null;
	f = cur_font;
	if ((cur_cmd == letter) || (cur_cmd == other_char) || (cur_cmd == char_given)) {
	  q = new_character (f, cur_chr);
	} else if (cur_cmd == char_num) {
	  scan_char_num();
	  q = new_character (f, cur_val);
	} else {
	  back_input();
	};
	if (q != null) {
	  /* begin expansion of Append the accent with appropriate kerns, then set |p:=q| */
	  /* module 1270 */
	  /* The kern nodes appended here must be distinguished from other kerns, lest
	   * they be wiped away by the hyphenation algorithm or by a previous line break.
	   * 
	   * The two kerns are computed with (machine-dependent) |real| arithmetic, but
	   * their sum is machine-independent; the net effect is machine-independent,
	   * because the user cannot remove these nodes nor access them via \.{\\lastkern}.
	   */
	  t = slant (f) / FLOAT_CONSTANT (65536);
	  i = char_info (f, character (q));
	  w = char_width (f, i);
	  h = char_height (f, height_depth (i));
	  if (h != x) {/* the accent must be shifted up or down */
		p = hpack (p, 0, additional);
		shift_amount (p) = x - h;
	  };
	  delta = round ((w - a) / FLOAT_CONSTANT (2) + h * t - x * s);
	  r = new_kern (delta);
	  subtype (r) = acc_kern;
	  link (tail) = r;
	  link (r) = p;
	  tail = new_kern (-a - delta);
	  subtype (tail) = acc_kern;
	  link (p) = tail;
	  p = q;
	};
	/* end expansion of Append the accent with appropriate kerns, then set |p:=q| */
	link (tail) = p;
	tail = p;
	space_factor = 1000;
  };
}


/* module 556 */

void
font_initialize (void) {
  null_character.b0 = min_quarterword;
  null_character.b1 = min_quarterword;
  null_character.b2 = min_quarterword;
  null_character.b3 = min_quarterword;
}

void
font_xmalloc (integer font_max) {
  font_check = xmalloc_array (four_quarters, font_max);         
  font_size = xmalloc_array (scaled, font_max);
  font_dsize = xmalloc_array (scaled, font_max);
  font_params = xmalloc_array (font_index, font_max);
  font_name = xmalloc_array (str_number, font_max);
  font_area = xmalloc_array (str_number, font_max);
  font_bc = xmalloc_array (eight_bits, font_max);
  font_ec = xmalloc_array (eight_bits, font_max);
  font_glue = xmalloc_array (halfword, font_max);
  hyphen_char = xmalloc_array (integer, font_max);
  skew_char = xmalloc_array (integer, font_max);
  bchar_label = xmalloc_array (font_index, font_max);
  font_bchar = xmalloc_array (nine_bits, font_max);
  font_false_bchar = xmalloc_array (nine_bits, font_max);
  char_base = xmalloc_array (integer, font_max);
  width_base = xmalloc_array (integer, font_max);
  height_base = xmalloc_array (integer, font_max);
  depth_base = xmalloc_array (integer, font_max);
  italic_base = xmalloc_array (integer, font_max);
  lig_kern_base = xmalloc_array (integer, font_max);
  kern_base = xmalloc_array (integer, font_max);
  exten_base = xmalloc_array (integer, font_max);
  param_base = xmalloc_array (integer, font_max);
}

void
font_initialize_init (void) {
  integer font_k;
  font_ptr = null_font;
  fmem_ptr = 7;
  font_name[null_font] = slow_make_tex_string("nullfont");
  font_area[null_font] = null_string;
  hyphen_char[null_font] = '-';
  skew_char[null_font] = -1;
  bchar_label[null_font] = non_address;
  font_bchar[null_font] = non_char;
  font_false_bchar[null_font] = non_char;
  font_bc[null_font] = 1;
  font_ec[null_font] = 0;
  font_size[null_font] = 0;
  font_dsize[null_font] = 0;
  char_base[null_font] = 0;
  width_base[null_font] = 0;
  height_base[null_font] = 0;
  depth_base[null_font] = 0;
  italic_base[null_font] = 0;
  lig_kern_base[null_font] = 0;
  kern_base[null_font] = 0;
  exten_base[null_font] = 0;
  font_glue[null_font] = null;
  font_params[null_font] = 7;
  param_base[null_font] = -1;
  for (font_k = 0; font_k <= 6; font_k++)
	font_info[font_k].sc = 0;
}

/* module 794 */
pointer 
mimic_char_node (pointer pp) {
  mem[lig_trick] = mem[lig_char (pp)];
  link (lig_trick) = link (pp);
  pp = lig_trick;
  return pp;
}
