 

#include "types.h"
#include "c-compat.h"


#include "globals.h"


/* module 1525 */
void 
scan_thread_id (void) {
  if (scan_keyword ("num")) {
	scan_int();
	if (cur_val <= 0)
	  pdf_error_string("ext1","num identifier must be positive");
	if (cur_val > max_halfword)
	  pdf_error_string("ext1","number too big");
	pdf_thread_id (tail) = cur_val;
	pdf_thread_named_id (tail) = 0;
  } else if (scan_keyword ("name"))  {
	scan_toks (false, true);
	pdf_thread_id (tail) = def_ref;
	pdf_thread_named_id (tail) = 1;
  }  else {
	pdf_error_string("ext1","identifier type missing");
  }
};


/* module 1549 */
/* The following function are needed for outputing the article thread. */
void 
thread_title (int thread) {
  pdf_print_string("/Title (");
  if (obj_info (thread) < 0) {
	pdf_print (-obj_info (thread));
  } else {
	pdf_print_int (obj_info (thread));
  }
  pdf_print_ln (')');
};

void 
pdf_fix_thread (int thread) {
  pointer a;
  pdf_warning_string ("thread","destination ", false);
  if (obj_info (thread) < 0) {
	zprint_string("name{");
	zprint (-obj_info (thread));
	zprint ('}');
  }  else {
	zprint_string("num");
	print_int (obj_info (thread));
  };
  zprint_string (" has been referenced but does not exist, replaced by a fixed one");
  print_ln();
  print_ln();
  pdf_new_dict (obj_type_others, 0);
  a = obj_ptr;
  pdf_indirect_string_ln ("T", thread);
  pdf_indirect_string_ln ("V", a);
  pdf_indirect_string_ln ("N", a);
  pdf_indirect_string_ln ("P", head_tab[obj_type_page]);
  pdf_print_string("/R [0 0 ");
  pdf_print_mag_bp (pdf_page_width);
  pdf_out (' ');
  pdf_print_mag_bp (pdf_page_height);
  pdf_print_ln (']');
  pdf_end_dict();
  pdf_begin_dict (thread);
  pdf_print_ln_string("/I << ");
  thread_title (thread);
  pdf_print_ln_string(">>");
  pdf_indirect_string_ln ("F", a);
  pdf_end_dict();
};


void 
out_thread (int thread) {
  pointer a, b; /* c*/
  int last_attr;
  if (obj_thread_first (thread) == 0) {
	pdf_fix_thread (thread);
	return;
  };
  pdf_begin_dict (thread);
  a = obj_thread_first (thread);
  b = a;
  last_attr = 0;
  do {
	if (obj_bead_attr (a) != 0)
	  last_attr = obj_bead_attr (a);
	a = obj_bead_next (a);
  } while (a != b);
  if (last_attr != 0) {
	pdf_print_ln (last_attr);
  } else {
	pdf_print_ln_string("/I << ");
	thread_title (thread);
	pdf_print_ln_string(">>");
  };
  pdf_indirect_string_ln ("F", a);
  pdf_end_dict();
  do {
	pdf_begin_dict (a);
	if (a == b)
	  pdf_indirect_string_ln ("T", thread);
	pdf_indirect_string_ln ("V", obj_bead_prev (a));
	pdf_indirect_string_ln ("N", obj_bead_next (a));
	pdf_indirect_string_ln ("P", obj_bead_page (a));
	pdf_indirect_string_ln ("R", obj_bead_rect (a));
	pdf_end_dict();
	a = obj_bead_next (a);
  } while (a != b);
};



/* module 1581 */

/* Threads are handled in similar way as link annotations.  */
void 
append_bead (pointer p) {
  int a, b, c, t;
  t = get_obj (obj_type_thread, pdf_thread_id (p), pdf_thread_named_id (p));
  b = pdf_new_objnum();
  obj_bead_ptr (b) = pdf_get_mem (pdfmem_bead_size);
  obj_bead_page (b) = pdf_last_page;
  obj_bead_data (b) = p;
  if (pdf_thread_attr (p) != null) {
	obj_bead_attr (b) = tokens_to_string (pdf_thread_attr (p));
  } else {
	obj_bead_attr (b) = 0;
  }
  if (obj_thread_first (t) == 0) {
	obj_thread_first (t) = b;
	obj_bead_next (b) = b;
	obj_bead_prev (b) = b;
  } else {
	a = obj_thread_first (t);
	c = obj_bead_prev (a);
	obj_bead_prev (b) = c;
	obj_bead_next (b) = a;
	obj_bead_prev (a) = b;
	obj_bead_next (c) = b;
  };
  pdf_append_list (b, pdf_bead_list);
};


void 
do_thread (pointer p, pointer parent_box, scaled x, scaled y) {
  if (doing_leaders)
	return;
  if (subtype (p) == pdf_start_thread_node) {
	pdf_thread_wd = pdf_width (p);
	pdf_thread_ht = pdf_height (p);
	pdf_thread_dp = pdf_depth (p);
	pdf_last_thread_id = pdf_thread_id (p);
	pdf_last_thread_named_id = (pdf_thread_named_id (p) > 0);
	if (pdf_last_thread_named_id)
	  add_token_ref (pdf_thread_id (p));
	pdf_thread_level = cur_s;
  };
  set_rect_dimens (p, parent_box, x, y, pdf_width (p),
				   pdf_height (p), pdf_depth (p), pdf_thread_margin);
  append_bead (p);
  last_thread = p;
};
	
void 
append_thread (pointer parent_box, scaled x, scaled y) {
  pointer p;
  p = get_node (pdf_thread_node_size);
  info (p) = max_halfword; /* this is not a whatsit node */ 
  link (p) = null; /* this node will be destroyed separately */ 
  pdf_width (p) = pdf_thread_wd;
  pdf_height (p) = pdf_thread_ht;
  pdf_depth (p) = pdf_thread_dp;
  pdf_thread_attr (p) = null;
  pdf_thread_id (p) = pdf_last_thread_id;
  if (pdf_last_thread_named_id) {
	add_token_ref (pdf_thread_id (p));
	pdf_thread_named_id (p) = 1;
  } else {
	pdf_thread_named_id (p) = 0;
  }
  set_rect_dimens (p, parent_box, x, y, pdf_width (p),
				   pdf_height (p), pdf_depth (p), pdf_thread_margin);
  append_bead (p);
  last_thread = p;
};

void 
end_thread (void) {
  if (pdf_thread_level != cur_s)
	pdf_error_string("ext4","\\pdfendthread ended up in different nesting level than \\pdfstartthread");
  if (is_running (pdf_thread_dp) && (last_thread != null))
	pdf_bottom (last_thread) = cur_v + pdf_thread_margin;
  if (pdf_last_thread_named_id)
	delete_token_ref (pdf_last_thread_id);
  last_thread = null;
};
