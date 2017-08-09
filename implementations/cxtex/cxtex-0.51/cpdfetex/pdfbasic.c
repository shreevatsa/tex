

#include "types.h"
#include "c-compat.h"

#include "globals.h"

/* module 645 */

/* Integer parameters are initialized immediately after the config file is
 * read, but dimension parameters are set when opening the PDF output file.
 */
#define do_pdf_int_pars(arg) \
     arg(cfg_output_code);\
     arg(cfg_adjust_spacing_code);\
     arg(cfg_compress_level_code);\
     arg(cfg_decimal_digits_code);\
     arg(cfg_move_chars_code);\
     arg(cfg_image_resolution_code);\
     arg(cfg_pk_resolution_code);\
     arg(cfg_unique_resname_code);\
     arg(cfg_protrude_chars_code);

#define do_pdf_dimen_pars(arg) \
     arg(cfg_h_origin_code);\
     arg(cfg_v_origin_code);\
     arg(cfg_page_height_code);\
     arg(cfg_page_width_code);\
     arg(cfg_link_margin_code);\
     arg(cfg_dest_margin_code);\
     arg(cfg_thread_margin_code)

#define get_cfg_int(arg)  int_par(arg) = cfg_par(arg)

#define get_cfg_dimen(arg) dimen_par(arg) = cfg_par(arg)

#define mag_cfg_dimen(arg)   if ((dimen_par(arg)==cfg_par(arg)) \
    &&  is_cfg_truedimen(arg)) {                                \
   dimen_par(arg)=round_xn_over_d (dimen_par(arg),1000,mag); };

#define get_cfg_int_pars  do_pdf_int_pars (get_cfg_int)

#define get_cfg_dimen_pars  do_pdf_dimen_pars (get_cfg_dimen)

#define mag_cfg_dimen_pars  do_pdf_dimen_pars (mag_cfg_dimen)

void
do_mag_cfg_dimen_pars (void) {
  mag_cfg_dimen_pars ;
}

/* module 646 */

/* Here we read the values from the config file and initialize any integer
 * parameters, e.g. |pdf_option_pdf_minor_version|.
 */

void
read_values_from_config_file (void) {
  read_config_file();
  get_cfg_int_pars;
  get_cfg_dimen_pars;
  if (pdf_pk_resolution == 0)
	pdf_pk_resolution = 600;
  pdf_option_pdf_minor_version = 4;
  if (cfg_par (cfg_pdf12_compliant_code) > 0)
	pdf_option_pdf_minor_version = 2;
  if (cfg_par (cfg_pdf13_compliant_code) > 0)
	pdf_option_pdf_minor_version = 3;
  if (cfg_par (cfg_pdf_minor_version_code) > -1)
	pdf_option_pdf_minor_version =
	  cfg_par (cfg_pdf_minor_version_code);
  pdf_option_always_use_pdfpagebox = cfg_par (cfg_always_use_pdf_pagebox_code);
  pdf_option_pdf_inclusion_errorlevel = cfg_par (cfg_pdf_option_pdf_inclusion_errorlevel_code);
}



pointer 
get_nullcs (void) {
  return null_cs;
};
	
pointer 
get_nullptr (void) {
  return null;
};




/* Here we have some utilities for tracing purpose. */
void 
short_display_n (int p, int m) { /* prints highlights of list |p| */
  int n; /* for replacement counts */ 
  int i;
  i = 0;
  font_in_short_display = null_font;
  if (p == null)
	return;
  while (p > mem_min) {
	if (is_char_node (p)) {
	  if (p <= mem_end) {
		if (font (p) != font_in_short_display) {
		  if ((font (p) < font_base) || (font (p) > font_max)) {
			print_char ('*');
		  } else {
			/* Print the font identifier for |font(p)| */
			print_font_ident(p);
		  }
		  print_char (' ');
		  font_in_short_display = font (p);
		};
		print_ASCII (qo (character (p)));
	  };
	} else {
	  if ((type (p) == glue_node) || (type (p) == disc_node)
		  || (type (p) == penalty_node)
		  || ((type (p) == kern_node) && (subtype (p) == explicit)))
		incr (i);
	  if (i >= m)
		return;
	  if ((type (p) == disc_node)) {
		zprint ('|');
		short_display (pre_break (p));
		zprint ('|');
		short_display (post_break (p));
		zprint ('|');
		n = replace_count (p);
		while (n > 0) {
		  if (link (p) != null)
			p = link (p);
		  decr (n);
		};
	  } else {
		/* Print a short indication of the contents of node |p| */
		p= print_short_node(p);
	  };
	};
	p = link (p);
	if (p == null)
	  return;
  };
  update_terminal;
}

#define print_delta_field(a,b)     \
  s = a;                           \
  if (s != 0)  {                   \
    if (not_printed_plus_yet)  {   \
     zprint_string (" plus ");     \
     not_printed_plus_yet = false; \
    };                             \
    print_glue ( s , b  - 2 ,"");  \
  }

#define print_delta_like_node( arg ) { \
  print_scaled ( arg (1));             \
  print_delta_field ( arg (2), 2 );    \
  print_delta_field ( arg (3), 3 );    \
  print_delta_field ( arg (4), 4 );    \
  print_delta_field ( arg (5), 5 );    \
  if (  arg (6 )  != 0  )  {           \
    zprint_string ( " minus " );       \
    print_glue ( arg (6 ),0 ,"");      \
  };                                   \
}

#define delta_field( arg )  mem [ p  +  arg ]. sc

integer 
show_line_breaking (void)	{
  boolean not_printed_plus_yet;
  pointer p; /* q not used */
  scaled s;
  not_printed_plus_yet=true; /*TH -Wall*/
  print_nl_string("cur_active_width: ");
  print_delta_like_node (cur_active_width_field);
  print_ln();
  not_printed_plus_yet = true;
  print_nl_string("active list: ");
  p = link (active);
  while (true) {
	if (type (p) == 2 ) { /* |delta_node| == 2 */
	  zprint_string("delta: ");
	  print_delta_like_node (delta_field);
	} else {
	  zprint_string("active: ");
	  if (break_node (p) != null)
		short_display_n (cur_break (break_node (p)), 5);
	};
	if (link (p) == last_active)
	  goto DONE;
	p = link (p);
	print_ln();
  };
 DONE:
  print_ln();
  print_nl_string("active_width: ");
  print_delta_like_node (active_width_field);
  print_nl_string("break_width: ");
  print_delta_like_node (break_width_field);
  print_nl_string("prev_active_width: ");
  print_delta_like_node (prev_active_width_field);
  print_nl_string("cur_p: ");
  short_display_n (cur_p, 5);
  if (prev_legal != null) {
	print_nl_string("prev_legal: ");
	short_display_n (prev_legal, 5);
  };
  if (rejected_cur_p != null) {
	print_nl_string("rejected_cur_p: ");
	short_display_n (rejected_cur_p, 5);
  };
  if (prev_p != null) {
	print_nl_string("prev_p: ");
	short_display_n (prev_p, 5);
  };
  if (prev_legal != null) {
	print_nl_string("prev_legal: ");
	short_display_n (prev_legal, 5);
  };
  print_ln();
  return 0;
}

pointer 
mi (pointer p) {
  return info (p);
}

pointer 
ml (pointer p) {
  return link (p);
}

integer pdf_mem_size;
integer *pdf_mem;
integer pdf_mem_ptr;

/* module 651 */
/* the first word is not used so we can use zero as a value for testing
   whether a pointer to |pdfmem| is valid */
void
pdfbasic_initialize (void) {
  pdf_mem_ptr = 1;
}

/* module 652 */

/* We use |pdf_get_mem| to allocate memory in |pdf_mem|
 */
integer 
pdf_get_mem (int s) { /* allocate |s| words in |pdf_mem| */
  integer ret;
  if (pdf_mem_ptr + s > pdf_mem_size)
	overflow ("PDF memory size", pdf_mem_size);
  ret = pdf_mem_ptr;
  pdf_mem_ptr = pdf_mem_ptr + s;
  return ret;
};

