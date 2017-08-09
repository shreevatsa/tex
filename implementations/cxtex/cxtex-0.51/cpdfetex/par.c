
#include "types.h"
#include "c-compat.h"


#include "globals.h"


/* module 1215 */

/* Here is where we clear the parameters that are supposed to revert to their
 * default values after every paragraph and when internal vertical mode is entered.
 */
void 
normal_paragraph (void) {
  if (looseness != 0)
	eq_word_define (int_base + looseness_code, 0);
  if (hang_indent != 0)
	eq_word_define (dimen_base + hang_indent_code, 0);
  if (hang_after != 1)
	eq_word_define (int_base + hang_after_code, 1);
  if (par_shape_ptr != null)
	eq_define (par_shape_loc, shape_ref, null);
  if (inter_line_penalties_ptr != null)
	eq_define (inter_line_penalties_loc, shape_ref, null);
};


/* module 1236 */
small_number 
norm_min (int h) {
  if (h <= 0) {
	return 1;
  } else if (h >= 63) {
	return 63;
  } else
	return h;
};

void 
new_graf (boolean indented) {
  prev_graf = 0;
  if ((mode == vmode) || (head != tail))
	tail_append (new_param_glue (par_skip_code));
  push_nest();
  mode = hmode;
  space_factor = 1000;
  set_cur_lang;
  clang = cur_lang;
  prev_graf = (norm_min (left_hyphen_min) * 64 +
			   norm_min (right_hyphen_min)) * 65536 + cur_lang;
  if (indented) {
	tail = new_null_box();
	link (head) = tail;
	width (tail) = par_indent;
  };
  if (every_par != null)
	begin_token_list (every_par, every_par_text);
  if (nest_ptr == 1)
	build_page(); /* put |par_skip| glue on current page */ 
};

/* module 1238 */
void 
indent_in_hmode (void) {
  pointer p, q;
  if (cur_chr > 0)	{ /* \.{\\indent} */
	p = new_null_box();
	width (p) = par_indent;
	if (abs (mode) == hmode) {
	  space_factor = 1000;
	} else {
	  q = new_noad();
	  math_type (nucleus (q)) = sub_box;
	  info (nucleus (q)) = p;
	  p = q;
	};
	tail_append (p);
  };
};

/* module 1240 */
void 
head_for_vmode (void) {
  if (mode < 0)
	if (cur_cmd != hrule) {
	  off_save();
	} else {
	  print_err ("You can't use `");
	  print_esc_string ("hrule");
	  zprint_string("' here except with leaders");
	  help2 ("To put a horizontal rule in an hbox or an alignment,",
		     "you should use \\leaders or \\hrulefill (see The TeXbook).");
	  error();
	} else {
	  back_input();
	  cur_tok = par_token;
	  back_input();
	  token_type = inserted;
	};
};


/* module 1241 */
void 
end_graf (void) {
  if (mode == hmode) {
	if (head == tail) {
	  pop_nest();	/* null paragraphs are ignored */
	} else {
	  line_break (false);
	}
	if (LR_save != null) {
	  flush_list (LR_save);
	  LR_save = null;
	};
	normal_paragraph();
	error_count = 0;
  };
};

/* module 1345 */
void 
resume_after_display (void) {
  if (cur_group != math_shift_group)
	confusion ("display");
  unsave();
  prev_graf = prev_graf + 3;
  push_nest();
  mode = hmode;
  space_factor = 1000;
  set_cur_lang;
  clang = cur_lang;
  prev_graf = (norm_min (left_hyphen_min) * 64 +
			   norm_min (right_hyphen_min)) * 65536 + cur_lang;
  /* Scan an optional space */
  scan_optional_space;
  if (nest_ptr == 1)
	build_page();
};


