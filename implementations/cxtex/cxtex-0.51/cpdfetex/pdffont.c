
#include "types.h"
#include "c-compat.h"
#include "globals.h"

/* module 676 */
eight_bits *pdf_font_type;/* the type of font */
str_number *pdf_font_attr;/* pointer to additional attributes */
internal_font_number *pdf_font_link;/* link to expanded fonts */
integer *pdf_font_stretch;/* limit of stretching */
integer *pdf_font_shrink;/* limit of shrinking */
integer *pdf_font_step;/* amount of one step */
integer *pdf_font_expand_factor;
integer *pdf_font_expand_ratio;
integer *pdf_font_lp_base;
integer *pdf_font_rp_base;
integer *pdf_font_ef_base;
internal_font_number tmp_f; /* for use with |pdf_init_font| */

/* module 677 */

/* Here come some subroutines to deal with expanded fonts for HZ-algorithm.
 */
integer 
init_font_base (void) {
  int i, j;
  i = pdf_get_mem (256);
  for (j = 0; j <= 255; j++)
	pdf_mem[i + j] = 0;
  return i;
};

void 
set_lp_code (internal_font_number f, eight_bits c, int i) {
  if (pdf_font_lp_base[f] < 0)
	pdf_font_lp_base[f] = init_font_base();
  pdf_mem[pdf_font_lp_base[f] + c] = fix_int (i, -1000, 1000);
};

void 
set_rp_code (internal_font_number f, eight_bits c, int i) {
  if (pdf_font_rp_base[f] < 0)
	pdf_font_rp_base[f] = init_font_base();
  pdf_mem[pdf_font_rp_base[f] + c] = fix_int (i, -1000, 1000);
};

void 
set_ef_code (internal_font_number f, eight_bits c, int i) {
  if (pdf_font_ef_base[f] < 0)
	pdf_font_ef_base[f] = init_font_base();
  pdf_mem[pdf_font_ef_base[f] + c] = fix_int (i, 0, 1000);
};

internal_font_number 
read_expand_font (internal_font_number f, scaled e) {
  /* read font |f| expanded by |e| thousandths into font memory */
  unsigned char old_setting; /* holds |selector| setting */ 
  str_number s; /* font name */ internal_font_number k;
  old_setting = selector;
  selector = new_string;
  zprint (font_name[f]);
  if (e > 0)
	zprint ('+'); /* minus sign will be printed by |print_int| */ 
  print_int (e);
  selector = old_setting;
  s = make_string();
  for (k = font_base + 1; k <= font_ptr; k++)
	if (str_eq_str (font_name[k], s)) {
	  flush_fontname_k (s);
	  if ((font_dsize[k] == font_dsize[f]) && (font_size[k] == font_size[f]) &&
		    ((pdf_font_expand_ratio[k] == e) && (pdf_font_expand_factor[k] =pdf_font_expand_factor[f])))
		goto FOUND;
	};
  k = read_font_info (null_cs, s,null_string, font_size[f]);
 FOUND:
  return k;
};


void 
set_expand_param (internal_font_number k,internal_font_number f, int e) {
  int i, j;
  pdf_font_expand_ratio[k] = e;
  pdf_font_step[k] = pdf_font_step[f];
  pdf_font_expand_factor[k] = pdf_font_expand_factor[f];
  copy_char_settings (pdf_font_lp_base);
  copy_char_settings (pdf_font_rp_base);
  copy_char_settings (pdf_font_ef_base);
};

integer 
get_expand_value (internal_font_number f, int e, int max_expand) {
  int step;
  boolean neg;
  if (e == 0) {
	return e;
  };
  if (e < 0) {
	e = -e;
	max_expand = -max_expand;
	neg = true;
  } else {
	neg = false;
  };
  step = pdf_font_step[f];
  if (e % step > 0)
	e = step * round_xn_over_d (e, 1, step);
  if (e > max_expand)
	e = max_expand - max_expand % step;
  if (neg)
	e = -e;
  return e;
};

integer 
get_expand_factor (internal_font_number f) {
  if (pdf_font_expand_ratio[f] == 0) {
	return 0;
  } else {
	return get_expand_value (f,
							 round_xn_over_d (pdf_font_expand_ratio[f],
											  pdf_font_expand_factor[f], 1000),
							 pdf_font_expand_ratio[f]);
  };
};

internal_font_number 
new_ex_font (internal_font_number f, scaled e) {
  internal_font_number k;
  k = pdf_font_link[f];
  while (k != null_font) {
	if (pdf_font_expand_ratio[k] == e) {
	  return k;
	};
	k = pdf_font_link[k];
  };
  k = read_expand_font (f, e);
  set_expand_param (k, f, e);
  check_ex_tfm (font_name[f], get_expand_factor (k));
  pdf_font_link[k] = pdf_font_link[f];
  pdf_font_link[f] = k;
  return k;
};

internal_font_number 
expand_font (internal_font_number f, int e) {
  /* look up for font |f| expanded by |e| thousandths */
  int max_expand;
  internal_font_number expand_font_r;
  expand_font_r = f;
  if (pdf_font_link[f] == null_font)
	pdf_error_string("expand","uninitialized pdf_font_link");
  if (e == 0)
	return expand_font_r;
  if (e < 0) {
	max_expand = pdf_font_expand_ratio[pdf_font_shrink[f]];
  } else {
	max_expand = pdf_font_expand_ratio[pdf_font_stretch[f]];
  }
  e = get_expand_value (f, e, max_expand);
  if (e != 0)
	expand_font_r = new_ex_font (f, e);
  return expand_font_r;
};

void 
do_expand_font (void) {
  int font_shrink, font_stretch, font_step, expand_factor;
  internal_font_number f;
  scan_font_ident();
  f = cur_val;
  if (f == null_font)
	pdf_error_string("font","invalid font identifier");
  scan_optional_equals();
  scan_int();
  font_stretch = fix_int (cur_val, 0, 1000);
  scan_int();
  font_shrink = fix_int (cur_val, 0, 1000);
  scan_int();
  font_step = fix_int (cur_val, 0, 1000);
  scan_int();
  expand_factor = fix_int (cur_val, 0, 1000);
  if ((expand_factor != 1000) && (pdf_output <= 0))
	pdf_error_string("expand","only font expansion factor 1000 can used in DVI mode");
  if (font_step == 0)
	pdf_error_string("expand","invalid step of font expansion");
  font_stretch = font_stretch - font_stretch % font_step;
  font_shrink = font_shrink - font_shrink % font_step;
  pdf_font_step[f] = font_step;
  pdf_font_expand_factor[f] = expand_factor;
  if (font_stretch > 0)
	pdf_font_stretch[f] = new_ex_font (f, font_stretch);
  if (font_shrink > 0)
	pdf_font_shrink[f] = new_ex_font (f, -font_shrink);
};

/* module 678 */

/* To set PDF font we need to find out fonts with the same name, because \TeX\
 * can load the same font several times for various sizes. For such fonts we
 * define only one font resources. The array |pdf_font_num| holds the object
 * number of font resource. A negative value of an entry of |pdf_font_num|
 * indicates that the corresponding font shares the font resource with the font
 * which internal number is the absolute value of the entry. For partial
 * downloading we also need to hold flags indicating which charaters in particular
 * font are used in array |pdf_char_used|.
 */
void 
pdf_use_font (internal_font_number f, int fontnum) {
  divide_scaled(font_size[f], one_hundred_bp,fixed_decimal_digits + 2);
  pdf_font_size[f] = scaled_out;
  font_used[f] = true;
  pdf_font_num[f] = fontnum;
  set_char_map (f);
};

void 
pdf_create_font_obj (void) {
  internal_font_number f, k;
  f = tmp_f;
  if (!font_used[f]) {
	if (pdf_font_map[f] == -1)
	  pdf_font_map[f] = fmlookup (f);
	if ((pdf_font_map[f] >= 0)) {
	  k = tfm_of_fm (pdf_font_map[f]);
	  if ((k != f) && str_eq_str (font_name[k], font_name[f])
		  && (font_dsize[k] == font_dsize[f])
		  && (get_expand_factor (k) == get_expand_factor (f))) {
		pdf_use_font (f, -k);
		return;
	  };
	};
	pdf_create_obj (obj_type_font, f);
	pdf_use_font (f, obj_ptr);
  };
};


/* module 680 */
char_used_array *pdf_char_used;	/* to mark used chars
								   TH pdf_char_used[][] ? */
char_map_array *pdf_char_map;	/* where to map chars 0..32 
								   TH pdf_char_map[][] ? */
scaled *pdf_font_size;/* used size of font in PDF file */
integer *pdf_font_num;/* mapping between internal font number in \TeX\ 
						 and font name defined in resources in PDF file */
integer *pdf_font_map;/* index in table of font mappings */
pointer pdf_font_list;/* list of used fonts in current page */
str_number pdf_resname_prefix;/* global prefix of resources name */
str_number last_tokens_string; /* the number of the most recently string
								  created by |tokens_to_string| */

/* module 681 */
void
pdffont_initialize (void) {
  pdf_resname_prefix = 0;
  last_tokens_string = 0;
}

void
pdffont_xmalloc (integer font_max) {
  pdf_char_used = xmalloc_array (char_used_array, font_max);
  pdf_char_map =  xmalloc_array (char_map_array, font_max);
  pdf_font_size = xmalloc_array (scaled, font_max);
  pdf_font_num = xmalloc_array (integer, font_max);
  pdf_font_map = xmalloc_array (integer, font_max);
  pdf_font_type =  xmalloc_array (eight_bits, font_max);
  pdf_font_attr =  xmalloc_array (str_number, font_max);
  pdf_font_link =  xmalloc_array (internal_font_number, font_max);
  pdf_font_stretch = xmalloc_array (integer, font_max);
  pdf_font_shrink = xmalloc_array (integer, font_max);
  pdf_font_step = xmalloc_array (integer, font_max);
  pdf_font_expand_factor =   xmalloc_array (integer, font_max);
  pdf_font_expand_ratio =   xmalloc_array (integer, font_max);
  pdf_font_lp_base =    xmalloc_array (integer, font_max);
  pdf_font_rp_base =   xmalloc_array (integer, font_max);
  pdf_font_ef_base =   xmalloc_array (integer, font_max);
}

void
pdffont_initialize_init (integer font_max) {
  integer k, font_k;
  for (font_k = font_base; font_k <= font_max;font_k++) {
	for (k = 0; k <= 31; k++) {
	  pdf_char_used[font_k][k] = 0;
	  pdf_char_map[font_k][k] = k;
	};
	pdf_char_map[font_k][32] = 32;
	pdf_font_size[font_k] = 0;
	pdf_font_num[font_k] = 0;
	pdf_font_map[font_k] = -1;
	pdf_font_type[font_k] = new_font_type;
	pdf_font_attr[font_k] = null_string;
	pdf_font_link[font_k] = null_font;
	pdf_font_stretch[font_k] = null_font;
	pdf_font_shrink[font_k] = null_font;
	pdf_font_step[font_k] = 0;
	pdf_font_expand_factor[font_k] = 0;
	pdf_font_expand_ratio[font_k] = 0;
	pdf_font_lp_base[font_k] = -1;
	pdf_font_rp_base[font_k] = -1;
	pdf_font_ef_base[font_k] = -1;
  };
}
