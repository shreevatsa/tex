
#include "types.h"
#include "c-compat.h"
#include "globals.h"

/* module 665 */
internal_font_number pdf_f; /* the current font in PDF output page */
scaled pdf_h; /* current horizontal coordinate in PDF output page */
scaled pdf_v; /* current vertical coordinate in PDF output page */
scaled pdf_last_h; /* last horizontal coordinate in PDF output page */
scaled pdf_last_v; /* last vertical coordinate in PDF output page */
scaled pdf_origin_h; /* current horizontal origin in PDF output page */
scaled pdf_origin_v; /* current vertical origin in PDF output page */
int pdf_first_space_corr; /* amount of first word spacing while drawing a 
							 string; for some reason it is not taken into 
							 account of the length of the string, so we
							 have to save it in order to adjust spacing 
							 when string drawing is finished */
boolean pdf_doing_string;/* we are writing string to PDF file? */
boolean pdf_doing_text;/* we are writing text section to PDF file? */
boolean pdf_font_changed;/* current font has been changed? */
scaled min_bp_val;
integer fixed_pk_resolution;
integer fixed_decimal_digits;
integer pk_scale_factor;

/* module 666 */

/* Following procedures implement low-level subroutines to convert \TeX{}
 * internal structures to PDF page description.
 */
void 
pdf_set_origin (void) {			/* set the origin to |cur_h|, |cur_v| */
  if ((abs (cur_h - pdf_origin_h) >= min_bp_val) || (abs (cur_v - pdf_origin_v) >= min_bp_val)) {
	pdf_print_string("1 0 0 1 ");
	pdf_print_bp (cur_h - pdf_origin_h);
	pdf_origin_h = pdf_origin_h + scaled_out;
	pdf_out (' ');
	pdf_print_bp (pdf_origin_v - cur_v);
	pdf_origin_v = pdf_origin_v - scaled_out;
	pdf_print_ln_string(" cm");
  };
  pdf_h = pdf_origin_h;
  pdf_last_h = pdf_origin_h;
  pdf_v = pdf_origin_v;
  pdf_last_v = pdf_origin_v;
};

void 
pdf_end_string (void) {			/* end the current string */
  if (pdf_doing_string) {
	pdf_print_string(")]TJ");
	if (pdf_first_space_corr != 0) {
	  pdf_h = pdf_h - pdf_first_space_corr;
	  pdf_first_space_corr = 0;
	};
	pdf_doing_string = false;
  };
};

static void 
pdf_moveto (scaled v, scaled v_out) { 	/* set the next starting point to |cur_h|, |cur_v| */
  pdf_out (' ');
  pdf_print_bp (cur_h - pdf_last_h);
  pdf_h = pdf_last_h + scaled_out;
  pdf_out (' ');
  pdf_print_real (v, fixed_decimal_digits);
  pdf_print_string(" Td");
  pdf_v = pdf_last_v - v_out;
  pdf_last_h = pdf_h;
  pdf_last_v = pdf_v;
};

void 
pdf_print_font_tag (internal_font_number f) {
  if (pdf_font_expand_ratio[f] == 0)
	return;
  if (pdf_font_expand_ratio[f] > 0)
	pdf_print ('+'); /* minus sign will be printed by |pdf_print_int| */
  pdf_print_int (pdf_font_expand_ratio[f]);
};

static void 
pdf_set_font (internal_font_number f) {
  pointer p;
  internal_font_number k;
  if (!font_used[f])
	pdf_init_font (f);
  set_ff (f);
  k = ff;
  p = pdf_font_list;
  while (p != null) {
	set_ff (info (p));
	if (ff == k)
	  goto FOUND;
	p = link (p);
  };
  pdf_append_list (f, pdf_font_list); /* |f| not found in |pdf_font_list| */ 
 FOUND:
  pdf_begin_text();
  pdf_print_string("/F");
  pdf_print_int (k);
  pdf_print_resname_prefix;
  pdf_print_font_tag (f);
  pdf_out (' ');
  pdf_print_bp (font_size[f]);
  pdf_print_string(" Tf");
  pdf_f = f;
  pdf_font_changed = true;
};

void 
pdf_begin_text (void) {			/* begin a text section */
  scaled temp_cur_h, temp_cur_v;
  if (!pdf_doing_text) {
	temp_cur_h = cur_h;
	temp_cur_v = cur_v;
	cur_h = 0;
	cur_v = cur_page_height;
	pdf_set_origin();
	cur_h = temp_cur_h;
	cur_v = temp_cur_v;
	pdf_print_ln_string("BT");
	pdf_doing_text = true;
	pdf_f = null_font;
	pdf_first_space_corr = 0;
	pdf_doing_string = false;
  };
};
	
void 
pdf_begin_string (internal_font_number f) {			/* begin to draw a string */
  boolean b; /* |b| is true only when we must adjust word spacing at the beginning of string */ 
  scaled s, s_out, v, v_out;
  pdf_begin_text();
  if (f != pdf_f) {
	pdf_end_string();
	pdf_set_font (f);
  };
  b = false;
  s = divide_scaled (cur_h - pdf_h, pdf_font_size[f], 3);
  s_out = scaled_out;
  if (abs (cur_v - pdf_v) >= min_bp_val) {
	v = divide_scaled (pdf_last_v - cur_v, one_hundred_bp,fixed_decimal_digits + 2);
	v_out = scaled_out;
  } else {
	v = 0;
	v_out = 0;
  };
  if (pdf_font_changed || (v != 0) || (abs (s) >= 32768)) {
	pdf_end_string();
	pdf_moveto (v, v_out);
	pdf_font_changed = false;
	s = 0;
	s_out = 0;
  };
  if (pdf_doing_string) {
	if (s != 0)
	  pdf_out (')');
  } else {
	pdf_out ('[');
	if (s != 0) {
	  b = true;
	} else {
	  pdf_out ('(');
	}
	pdf_doing_string = true;
  };
  if (s != 0) {
	pdf_print_int (-s);
	if (b)
	  pdf_first_space_corr = s_out;
	pdf_out ('(');
	pdf_h = pdf_h + s_out;
  };
};
 
void 
pdf_end_text (void) {			/* end a text section */
  if (pdf_doing_text) {
	pdf_end_string();
	pdf_print_nl;
	pdf_print_ln_string("ET");
	pdf_doing_text = false;
  };
};

void 
pdf_set_rule (scaled x, scaled y, scaled w, scaled h) {	/* draw a rule */
  pdf_end_text();
  pdf_set_origin();
  if (h <= one_bp) {
	pdf_print_ln ('q');
	pdf_print_ln_string("[]0 d");
	pdf_print_ln_string("0 J");
	pdf_print_bp (h);
	pdf_print_ln_string(" w");
	pdf_print_string("0 ");
	pdf_print_bp ((h + 1) / 2);
	pdf_print_ln_string(" m");
	pdf_print_bp (w);
	pdf_print (' ');
	pdf_print_bp ((h + 1) / 2);
	pdf_print_ln_string(" l");
	pdf_print_ln ('S');
	pdf_print_ln ('Q');
  } else if (w <= one_bp) {
	pdf_print_ln ('q');
	pdf_print_ln_string("[]0 d");
	pdf_print_ln_string("0 J");
	pdf_print_bp (w);
	pdf_print_ln_string(" w");
	pdf_print_bp ((w + 1) / 2);
	pdf_print_ln_string(" 0 m");
	pdf_print_bp ((w + 1) / 2);
	pdf_print (' ');
	pdf_print_bp (h);
	pdf_print_ln_string(" l");
	pdf_print_ln ('S');
	pdf_print_ln ('Q');
  } else {
	pdf_print_bp (pdf_x (x));
	pdf_out (' ');
	pdf_print_bp (pdf_y (y));
	pdf_out (' ');
	pdf_print_bp (w);
	pdf_out (' ');
	pdf_print_bp (h);
	pdf_print_ln_string(" re f");
  };
};

void 
pdf_rectangle (scaled left, scaled top, scaled right, scaled bottom) {			
  /* output a rectangle specification to PDF file */
  prepare_mag();
  pdf_print_string("/Rect [");
  pdf_print_mag_bp (pdf_x (left));
  pdf_out (' ');
  pdf_print_mag_bp (pdf_y (bottom));
  pdf_out (' ');
  pdf_print_mag_bp (pdf_x (right));
  pdf_out (' ');
  pdf_print_mag_bp (pdf_y (top));
  pdf_print_ln (']');
};

