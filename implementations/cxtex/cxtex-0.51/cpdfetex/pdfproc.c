
#include "types.h"
#include "c-compat.h"


#include "globals.h"



/* module 1501 */
/* We have to check whether \.{\\pdfoutput} is set for using \pdfTeX{}
 * extensions.
 */
void 
check_pdfoutput (char *s) {
  if (pdf_output <= 0) {
	print_nl_string("pdfTeX error (ext1): ");
	zprint_string (s);
	zprint_string(" used while \\pdfoutput is not set");
	succumb;
  };
};


/* module 1503 */

/* The \.{\\pdfobj} primitive is to create a ``raw'' object in PDF
 * output file. The object contents will be hold in memory and will be written
 * out only when the object je referenced by \.{\\pdfrefobj}. When \.{\\pdfobj}
 * is used with \.{\\immediate}, the object contents will be written out
 * immediately. Object referenced in current page are appended into
 * |pdf_obj_list|.
 */

integer pdf_last_obj;

/* module 1505 */
/* We need to check whether the referenced object exists.  */

void 
pdf_check_obj (int t, int n) {
  int k;
  k = head_tab[t];
  while ((k != 0) && (k != n))
    k = obj_link (k);
  if (k == 0)
    pdf_error_string("ext1", "cannot find referenced object");
};

/* module 1507 */

/* \.{\\pdfxform} and \.{\\pdfrefxform} are similiar to \.{\\pdfobj} and \.{\\pdfrefobj} */

integer pdf_last_xform;


/* module 1510 */

/* \.{\\pdfximage} and \.{\\pdfrefximage} are similiar to \.{\\pdfxform} and
 * \.{\\pdfrefxform}. As we have to scan |<rule spec>| quite often, it is better
 * have a |rule_node| that holds the most recently scanned |<rule spec>|.
 */

integer pdf_last_ximage;
integer pdf_last_ximage_pages;
pointer alt_rule;
integer pdf_last_pdf_box_spec;


/* module 1512 */
void 
scale_image (int n) {
  int x, y, xr, yr; /* size and resolution of image */ 
  scaled w, h; /* indeed size corresponds to image resolution */ 
  int default_res;
  w= 0; h = 0; /*TH -wall*/
  x = image_width (obj_ximage_data (n));
  y = image_height (obj_ximage_data (n));
  xr = image_x_res (obj_ximage_data (n));
  yr = image_y_res (obj_ximage_data (n));
  if ((xr > 65536) || (yr > 65536)) {
	xr = 0;
	yr = 0;
	pdf_warning_string ("ext1","too large image resolution ignored",true);
  };
  if ((x <= 0) || (y <= 0) || (xr < 0) || (yr < 0))
	pdf_error_string("ext1","invalid image dimensions");
  if (is_pdf_image (obj_ximage_data (n))) {
	w = x;
	h = y;
  } else {
	default_res = fix_int (pdf_image_resolution, 0, 2400);
	if ((default_res > 0) && ((xr == 0) || (yr == 0))) {
	  xr = default_res;
	  yr = default_res;
	};
	if (is_running (obj_ximage_width (n)) && is_running (obj_ximage_height (n))) {
	  if ((xr > 0) && (yr > 0)) {
		w = ext_xn_over_d (one_hundred_inch, x, 100 * xr);
		h = ext_xn_over_d (one_hundred_inch, y, 100 * yr);
	  } else {
		w = ext_xn_over_d (one_hundred_inch, x, 7200);
		h = ext_xn_over_d (one_hundred_inch, y, 7200);
	  };
	};
  };
  if (is_running (obj_ximage_width (n))
	  && is_running (obj_ximage_height (n))
	  && is_running (obj_ximage_depth (n))) {
	obj_ximage_width (n) = w;
	obj_ximage_height (n) = h;
	obj_ximage_depth (n) = 0;
  } else if (is_running (obj_ximage_width (n))) { /* image depth or height is explicitly specified */
	if (is_running (obj_ximage_height (n))) { /* image depth is explicitly specified */
	  obj_ximage_width (n) = ext_xn_over_d (h, x, y);
	  obj_ximage_height (n) = h - obj_ximage_depth (n);
	} else if (is_running (obj_ximage_depth (n))) {	/* image height is explicitly specified */
	  obj_ximage_width (n) = ext_xn_over_d (obj_ximage_height (n), x, y);
	  obj_ximage_depth (n) = 0;
	} else {	/* both image depth and height are explicitly specified */
	  obj_ximage_width (n) = ext_xn_over_d (obj_ximage_height (n) +  obj_ximage_depth (n), x, y);
	};
  } else  {		/* image width is explicitly specified */
	if (is_running (obj_ximage_height (n)) && is_running (obj_ximage_depth (n))) {
	  /* both image depth and height are not specified */
	  obj_ximage_height (n) = ext_xn_over_d (obj_ximage_width (n), y, x);
	  obj_ximage_depth (n) = 0;
	} else if (is_running (obj_ximage_height (n))) {	/* image depth is explicitly specified */
	  obj_ximage_height (n) = ext_xn_over_d (obj_ximage_width (n), y, x) - obj_ximage_depth (n);
	} else if (is_running (obj_ximage_depth (n))) {	/* image height is explicitly specified */
	  obj_ximage_depth (n) = 0;
	} else {	/* both image depth and height are explicitly specified */
	  do_nothing;
	}
  };
};


void 
scan_pdf_box_spec (void) { /* scans pdf-box-spec to |pdf_last_pdf_box_spec| */
  pdf_last_pdf_box_spec = pdf_pdf_box_spec_crop;
  if (scan_keyword ("mediabox")) {
	pdf_last_pdf_box_spec = pdf_pdf_box_spec_media;
  } else if (scan_keyword ("cropbox")) {
	pdf_last_pdf_box_spec = pdf_pdf_box_spec_crop;
  } else if (scan_keyword ("bleedbox")) {
	pdf_last_pdf_box_spec = pdf_pdf_box_spec_bleed;
  } else if (scan_keyword ("trimbox")) {
	pdf_last_pdf_box_spec = pdf_pdf_box_spec_trim;
  } else if (scan_keyword ("artbox"))
	pdf_last_pdf_box_spec = pdf_pdf_box_spec_art; 
};

void 
scan_alt_rule (void) {	/* scans rule spec to |alt_rule| */
  if (alt_rule == null) 
	alt_rule = new_rule();
  width (alt_rule) = null_flag;
  height (alt_rule) = null_flag;
  depth (alt_rule) = null_flag;
 RESWITCH:
  if (scan_keyword("width")) {
	scan_normal_dimen;
	width (alt_rule) = cur_val;
	goto RESWITCH;
  };
  if (scan_keyword ("height")) {
	scan_normal_dimen;
	height (alt_rule) = cur_val;
	goto RESWITCH;
  };
  if (scan_keyword ("depth")) {
	scan_normal_dimen;
	depth (alt_rule) = cur_val;
	goto RESWITCH;
  };
};

void 
scan_image (void) {
  int k;
  str_number named;
  str_number s;
  int page;
  page=0; /*TH -Wall*/
  incr (pdf_ximage_count);
  pdf_create_obj (obj_type_ximage, pdf_ximage_count);
  k = obj_ptr;
  obj_data_ptr (k) = pdf_get_mem (pdfmem_ximage_size);
  scan_alt_rule();  /* scans |<rule spec>| to |alt_rule| */
  obj_ximage_width (k) = width (alt_rule);
  obj_ximage_height (k) = height (alt_rule);
  obj_ximage_depth (k) = depth (alt_rule);
  if (scan_keyword ("attr")) {
	scan_pdf_ext_toks;
	obj_ximage_attr (k) = def_ref;
  } else {
	obj_ximage_attr (k) = null;
  }
  named = 0;
  if (scan_keyword ("named")) {
	scan_pdf_ext_toks;
	named = tokens_to_string (def_ref);
	delete_token_ref (def_ref);
  } else if (scan_keyword ("page")) {
	scan_int();
	page = cur_val;
  }  else {
	page = 1;
  }
  scan_pdf_box_spec(); /* scans pdf-box-spec to |pdf_last_pdf_box_spec| */
  scan_pdf_ext_toks;
  s = tokens_to_string (def_ref);
  delete_token_ref (def_ref);
  if (pdf_option_always_use_pdfpagebox > 0) {
	print_err ("pdfTeX warning (image inclusion): ");
	zprint_string ("\\pdfoptionalwaysusepdfpagebox is in use (");
	print_int (pdf_option_always_use_pdfpagebox);
	zprint (')');
	print_ln();
  };
  obj_ximage_data (k) = read_image (s, page, named,pdf_option_pdf_minor_version,
									pdf_option_always_use_pdfpagebox, 
									pdf_option_pdf_inclusion_errorlevel);
  if (named != 0)
	flush_str (named);
  flush_str (s);
  scale_image (k);
  pdf_last_ximage = k;
  pdf_last_ximage_pages = image_pages (obj_ximage_data (k));
};


/* module 1515 */

/* The following function finds object with identifier |i| and type |t|.
 * |i < 0| indicates that |-i| should be treated as a string number. If no
 * such object exists then it will be created. This function is used mainly to
 * find destination for link annotations and outlines; however it is also used
 * in |pdf_ship_out| (to check whether a Page object already exists) so we need
 * to declare it together with subroutines needed in |pdf_hlist_out| and
 * |pdf_vlist_out|.
 */

integer 
find_obj (int t, int i, small_number byname) {
  int p, r;
  p = head_tab[t];
  r = 0;
  if (byname > 0) {
	while (p != 0) {
	  if ((obj_info (p) < 0) && (str_eq_str (-obj_info (p), i))) {
		r = p;
		goto DONE;
	  };
	  p = obj_link (p);
	};
  } else {
	while (p != 0) {
	  if (obj_info (p) == i) {
		r = p;
		goto DONE;
	  };
	  p = obj_link (p);
	}; 
  }
 DONE: 
  return r;
};

integer 
get_obj (int t, int i, small_number byname) {
  int r;
  str_number s;
  if (byname > 0) {
	s = tokens_to_string (i);
	r = find_obj (t, s, true);
  } else {
	s = 0;
	r = find_obj (t, i, false);
  };
  if (r == 0) {
	if (byname > 0) {
	  pdf_create_obj (t, -s);
	  s = 0;
	} else {
	  pdf_create_obj (t, i);
	}
	r = obj_ptr;
	if (t == obj_type_dest)
	  obj_dest_ptr (r) = null;
  };
  if (s != 0)
	flush_str (s);
  return r;
};



/* module 1516 */
pointer scan_action (void) { 
  pointer scan_action_r;	 	/* read an action specification */
  int p;
  p = get_node (pdf_action_size);
  scan_action_r = p;
  pdf_action_file (p) = null;
  pdf_action_refcount (p) = null;
  if (scan_keyword ("user")) {
	pdf_action_type (p) = pdf_action_user;
  } else if (scan_keyword ("goto")) {
	pdf_action_type (p) = pdf_action_goto;
  } else if (scan_keyword ("thread")) {
	pdf_action_type (p) = pdf_action_thread;
  } else {
	pdf_error_string("ext1","action type missing");
  }
  if (pdf_action_type (p) == pdf_action_user) {
	scan_toks (false, true);
	pdf_action_user_tokens (p) = def_ref;
	return scan_action_r;
  };
  if (scan_keyword ("file")) {
	scan_toks (false, true);
	pdf_action_file (p) = def_ref;
  };
  if (scan_keyword ("page")) {
	if (pdf_action_type (p) != pdf_action_goto)
	  pdf_error_string("ext1","only GoTo action can be used with `page'");
	pdf_action_type (p) = pdf_action_page;
	scan_int();
	if (cur_val <= 0)
	  pdf_error_string("ext1","page number must be positive");
	pdf_action_id (p) = cur_val;
	pdf_action_named_id (p) = 0;
	scan_toks (false, true);
	pdf_action_page_tokens (p) = def_ref;
  } else if (scan_keyword ("name")) {
	scan_toks (false, true);
	pdf_action_named_id (p) = 1;
	pdf_action_id (p) = def_ref;
  } else if (scan_keyword ("num")) {
	if ((pdf_action_type (p) == pdf_action_goto) && (pdf_action_file (p) != null))
	  pdf_error_string("ext1","`goto' option cannot be used with both `file' and `num'");
	scan_int();
	if (cur_val <= 0)
	  pdf_error_string("ext1","num identifier must be positive");
	pdf_action_named_id (p) = 0;
	pdf_action_id (p) = cur_val;
  } else {
	pdf_error_string("ext1","identifier type missing");
  }
  if (scan_keyword ("newwindow")) {
	pdf_action_new_window (p) = 1;
  } else if (scan_keyword ("nonewwindow")) {
	pdf_action_new_window (p) = 2;
  } else{
    pdf_action_new_window (p) = 0;
  }
  if ((pdf_action_new_window (p) > 0) 
	  && 
	  (((pdf_action_type (p) != pdf_action_goto) && (pdf_action_type (p) != pdf_action_page))
	   || 
	   (pdf_action_file (p) == null)))
	pdf_error_string("ext1","`newwindow'/`nonewwindow' must be used with `goto' and `file' option");
  return scan_action_r;
};

void 
new_annot_whatsit (small_number w, small_number s) {
  /* create a new whatsit node for annotation */
  new_whatsit (w, s);
  scan_alt_rule(); /* scans |<rule spec>| to |alt_rule| */
  pdf_width (tail) = width (alt_rule);
  pdf_height (tail) = height (alt_rule);
  pdf_depth (tail) = depth (alt_rule);
  if ((w == pdf_start_link_node)) {
	if (scan_keyword ("attr")) {
	  scan_toks (false, true);
	  pdf_link_attr (tail) = def_ref;
	} else {
	  pdf_link_attr (tail) = null;
	}
  };
  if ((w == pdf_thread_node)|| (w == pdf_start_thread_node))  {
	if (scan_keyword ("attr")) {
	  scan_toks (false, true);
	  pdf_thread_attr (tail) = def_ref;
	} else {
	  pdf_thread_attr (tail) = null;
	}
  };
};



/* module 1517 */
integer pdf_last_annot;

/* module 1521 */
integer 
outline_list_count (pointer p) {
  /* return number of outline entries in the same level with |p| */
  int k;
  k = 1;
  while (obj_outline_prev (p) != 0) {
	incr (k);
	p = obj_outline_prev (p);
  };
  return k;
}

/* module 1523 */

/* When a destination is created we need to check whether another destination
 * with the same identifier already exists and give a warning if needed.
 */
void 
warn_dest_dup (int id, small_number byname, char *s1,char *s2) {
  pdf_warning_string (s1,"destination with the same identifier (",false);
  if (byname > 0) {
	zprint_string("name");
	print_mark (id);
  } else  {
	zprint_string("num");
	print_int (id);
  };
  zprint_string(") ");
  zprint_string (s2);
  print_ln();
  show_context();
};


/* module 1544 */

/* The following subroutines are about PDF-specific font issues.  */
void 
pdf_include_chars (void) {
  str_number s;
  pool_pointer k; /* running indices */ 
  internal_font_number f;
  scan_font_ident();
  f = cur_val;
  if (f == null_font)
	pdf_error_string("font","invalid font identifier");
  pdf_check_vf (f);
  if (!font_used[f])
	pdf_init_font (f);
  scan_toks (false, true);
  s = tokens_to_string (def_ref);
  delete_token_ref (def_ref);
  k = str_start[s];
  while (k < str_start[s + 1]) {
	pdf_mark_char (f, str_pool[k]);
	incr (k);
  };
  flush_str (s);
};


/* module 1576 */
scaled cur_page_width; /* width of page being shipped */
scaled cur_page_height; /* height of page being shipped */
scaled cur_h_offset; /* horizontal offset of page being shipped */
scaled cur_v_offset; /* vertical offset of page being shipped */
pointer pdf_obj_list; /* list of objects in the current page */
pointer pdf_xform_list; /* list of forms in the current page */
pointer pdf_ximage_list; /* list of images in the current page */
int pdf_link_level; /* depth of nesting of box containing link annotation */
pointer last_link; /* pointer to the last link annotation */
scaled pdf_link_ht, pdf_link_dp,  pdf_link_wd; /* dimensions of the last 
												  link annotation */
pointer last_thread; /* pointer to the last thread */
scaled pdf_thread_ht, pdf_thread_dp, pdf_thread_wd; /* dimensions of the 
													   last thread */
halfword pdf_last_thread_id;/* identifier of the last thread */
boolean pdf_last_thread_named_id;/* is identifier of the last thread named */
int pdf_thread_level; /* depth of nesting of box containing the last thread */
pointer pdf_annot_list; /* list of annotations in the current page */
pointer pdf_link_list; /* list of link annotations in the current page */
pointer pdf_dest_list; /* list of destinations in the current page */
pointer pdf_bead_list; /* list of thread beads in the current page */
int pdf_obj_count; /* counter of objects */
int pdf_xform_count; /* counter of forms */
int pdf_ximage_count; /* counter of images */
int pdf_cur_form; /* the form being output */
int pdf_first_outline, pdf_last_outline,   pdf_parent_outline;
scaled pdf_xform_width, pdf_xform_height, pdf_xform_depth; /* dimension of the
															  current form */
pointer pdf_info_toks; /* additional keys of Info dictionary */
pointer pdf_catalog_toks; /* additional keys of Catalog dictionary */
int pdf_catalog_openaction;
pointer pdf_names_toks; /* additional keys of Names dictionary */
int pdf_dest_names_ptr; /* first unused position in |dest_names| */
integer dest_names_size; /* maximum number of names in name tree of PDF 
							output file */
dest_name_entry *dest_names;
integer image_orig_x, image_orig_y; /* origin of cropped pdf images */
pointer link_level_stack; /* stack to save |pdf_link_level| */
pointer pdf_trailer_toks; /* additional keys of Trailer dictionary */


void
pdfproc_initialize (void) {
  /* module 1511 */
  alt_rule = null;
  /* module 1577 */
  pdf_link_level = -1;
  link_level_stack = null;
  pdf_first_outline = 0;
  pdf_last_outline = 0;
  pdf_parent_outline = 0;
  pdf_obj_count = 0;
  pdf_xform_count = 0;
  pdf_ximage_count = 0;
  pdf_dest_names_ptr = 0;
  pdf_info_toks = null;
  pdf_catalog_toks = null;
  pdf_names_toks = null;
  pdf_catalog_openaction = 0;
  pdf_trailer_toks = null;
}

/* module 1578 */

/* The following procedures are needed for outputing whatsit nodes for
 * pdfTeX.
 */
void 
write_action (pointer p) {	/* write an action specification */
  str_number s;
  int d;
  d=0; /*TH -Wall*/
  if (pdf_action_type (p) == pdf_action_user) {
	pdf_print_toks_ln (pdf_action_user_tokens (p));
	return;
  };
  pdf_print_string("<< ");
  if (pdf_action_file (p) != null) {
	pdf_print_string("/F ");
	s = tokens_to_string (pdf_action_file (p));
	if ((str_pool[str_start[s]] == 40) && (str_pool[str_start[s] + length (s) - 1] == 41)) {
	  pdf_print (s);
	} else {
	  pdf_print_str (s);
	};
	flush_str (s);
	pdf_print (' ');
	if (pdf_action_new_window (p) > 0) {
	  pdf_print_string("/NewWindow ");
	  if (pdf_action_new_window (p) == 1) {
		pdf_print_string("true ");
	  } else {
		pdf_print_string("false ");
	  }
	};
  };
  switch (pdf_action_type (p)) {
  case pdf_action_page:
	{
	  if (pdf_action_file (p) == null) {
		pdf_print_string("/S /GoTo /D [");
		pdf_print_int (get_obj (obj_type_page, pdf_action_id (p), false));
		pdf_print_string(" 0 R");
	  }else {
		pdf_print_string("/S /GoToR /D [");
		pdf_print_int (pdf_action_id (p) - 1);
	  };
	  pdf_out (' ');
	  pdf_print (tokens_to_string (pdf_action_page_tokens (p)));
	  flush_str (last_tokens_string);
	  pdf_out (']');
	};
	break;
  case pdf_action_goto:
	{
	  if (pdf_action_file (p) == null) {
		pdf_print_string("/S /GoTo ");
		d = get_obj (obj_type_dest, pdf_action_id (p), pdf_action_named_id (p));
	  }else {
		pdf_print_string("/S /GoToR ");
	  }
	  if (pdf_action_named_id (p) > 0) {
		pdf_str_entry ("D", tokens_to_string (pdf_action_id (p)));
		flush_str (last_tokens_string);
	  } else if (pdf_action_file (p) == null) {
		pdf_indirect_string ("D", d);
	  } else {
		pdf_error_string("ext4","`goto' option cannot be used with both `file' and `num'");
	  }
	};
	break;
  case pdf_action_thread:
	{
	  pdf_print_string("/S /Thread ");
	  if (pdf_action_file (p) == null)
		d = get_obj (obj_type_thread, pdf_action_id (p), pdf_action_named_id (p));
	  if (pdf_action_named_id (p) > 0) {
		pdf_str_entry ("D", tokens_to_string (pdf_action_id (p)));
		flush_str (last_tokens_string);
	  } else if (pdf_action_file (p) == null) {
		pdf_indirect_string ("D", d);
	  } else {
		pdf_int_entry ("D", pdf_action_id (p));
	  }
	};
	;
  };
  pdf_print_ln_string(" >>");
};

void 
set_rect_dimens (pointer p, pointer parent_box, scaled x, scaled y, scaled w, scaled h, scaled d, scaled margin) {
  pdf_left (p) = cur_h;
  if (is_running (w)) {
	pdf_right (p) = x + width (parent_box);
  } else {
	pdf_right (p) = cur_h + w;
  }
  if (is_running (h)) {
	pdf_top (p) = y - height (parent_box);
  } else {
	pdf_top (p) = cur_v - h;
  }
  if (is_running (d)) {
	pdf_bottom (p) = y + depth (parent_box);
  } else {
	pdf_bottom (p) = cur_v + d;
  }
  pdf_left (p) = pdf_left (p) - margin;
  pdf_top (p) = pdf_top (p) - margin;
  pdf_right (p) = pdf_right (p) + margin;
  pdf_bottom (p) = pdf_bottom (p) + margin;
};

void 
do_annot (pointer p, pointer parent_box, scaled x, scaled y) {
  if (doing_leaders)
	return;
  set_rect_dimens (p, parent_box, x, y, pdf_width (p), pdf_height (p), pdf_depth (p), 0);
  obj_annot_ptr (pdf_annot_objnum (p)) = p;
  pdf_append_list (pdf_annot_objnum (p), pdf_annot_list);
};





/* module 1579 */

/* To implement nesting link annotations, we need a stack to save box testing
 * level of each link that has been broken. Each stack entry holds the
 * box nesting level and pointer the whatsit node created for
 * corresponding \.{\\pdfstartlink}.
 */

void 
save_link_level (pointer l) {
  pointer p, r;
  pdf_link_level = cur_s;
  r = copy_node_list (l);
  pdf_link_wd = pdf_width (r);
  pdf_link_ht = pdf_height (r);
  pdf_link_dp = pdf_depth (r);
  p = get_node (small_node_size);
  link_level (p) = pdf_link_level;
  link_ptr (p) = r;
  link (p) = link_level_stack;
  link_level_stack = p;
};

void 
do_link (pointer p, pointer parent_box, scaled x, scaled y) {
  if (type (parent_box) != hlist_node)
	pdf_error_string("ext4","link annotations can be inside hbox only");
  save_link_level (p);
  set_rect_dimens (p, parent_box, x, y, pdf_link_wd, pdf_link_ht, pdf_link_dp, pdf_link_margin);
  last_link = p;
  pdf_create_obj (obj_type_others, 0);
  obj_annot_ptr (obj_ptr) = p;
  pdf_append_list (obj_ptr, pdf_link_list);
};

void 
restore_link_level (void) {
  pointer p, r;
  if (link_level_stack == null)
	pdf_error_string("ext4","invalid stack of link nesting level");
  p = link_level_stack;
  link_level_stack = link (p);
  r = link_ptr (p);
  flush_node_list (r);
  free_node (p, small_node_size);
  p = link_level_stack;
  if (p == null) {
	pdf_link_level = -1;
  } else {
	pdf_link_level = link_level (p);
	r = link_ptr (p);
	pdf_link_wd = pdf_width (r);
	pdf_link_ht = pdf_height (r);
	pdf_link_dp = pdf_depth (r);
  };
};

void 
end_link (void) {
  if (pdf_link_level != cur_s)
	pdf_error_string("ext4","\\pdfendlink ended up in different nesting level than \\pdfstartlink");
  if (is_running (pdf_link_wd) && (last_link != null))
	pdf_right (last_link) = cur_h + pdf_link_margin;
  restore_link_level();
  last_link = null;
};


/* module 1580 */

/* For ``running'' annotations we must append a new node when the end of
 * annotation is in other box than its start. The new created node is identical to
 * corresponding whatsit node representing the start of annotation, but its
 * |info| field is |null|. We set |info| field just before destroying the node, in
 * order to use |flush_node_list| to do the job.
 */
void 
append_link (pointer parent_box, scaled x, scaled y) {
  /* append a new |pdf_start_link_node| to |pdf_link_list| and update |last_link| */
  int r; /* p*/
  if (type (parent_box) != hlist_node)
	pdf_error_string("ext4","link annotations can be inside hbox only");
  r = copy_node_list (link_ptr (link_level_stack)); /* copy link node to |r| */ 
  info (r) = max_halfword; /* mark that this node is not a whatsit node */ 
  link (r) = null; /* this node will be destroyed separately */ 
  set_rect_dimens (r, parent_box, x, y, pdf_link_wd, pdf_link_ht, pdf_link_dp, pdf_link_margin);
  pdf_create_obj (obj_type_others, 0);
  obj_annot_ptr (obj_ptr) = r;
  last_link = r;
  pdf_append_list (obj_ptr, pdf_link_list);
};



integer 
open_subentries (pointer p) {
  int k, c;
  int l, r;
  k = 0;
  if (obj_outline_first (p) != 0) {
	l = obj_outline_first (p);
	do {
	  incr (k);
	  c = open_subentries (l);
	  if (obj_outline_count (l) > 0)
		k = k + c;
	  obj_outline_parent (l) = p;
	  r = obj_outline_next (l);
	  if (r == 0)
		obj_outline_last (p) = l;
	  l = r;
	} while (!(l == 0));
  };
  if (obj_outline_count (p) > 0) {
	obj_outline_count (p) = k;
  } else {
	obj_outline_count (p) = -k;
  }
  return k;
};

void 
do_dest (pointer p, pointer parent_box, scaled x, scaled y) {
  int k;
  if (doing_leaders)
	return;
  k = get_obj (obj_type_dest, pdf_dest_id (p), pdf_dest_named_id (p));
  if (obj_dest_ptr (k) != null) {
	warn_dest_dup (pdf_dest_id (p), pdf_dest_named_id (p), "ext4",
				   "has been already used, duplicate ignored");
	return;
  };
  obj_dest_ptr (k) = p;
  pdf_append_list (k, pdf_dest_list);
  switch (pdf_dest_type (p)) {
  case pdf_dest_xyz:
	{
	  pdf_left (p) = cur_h;
	  pdf_top (p) = cur_v;
	};
	break;
  case pdf_dest_fith:
  case pdf_dest_fitbh:
	pdf_top (p) = cur_v;
	break;
  case pdf_dest_fitv:
  case pdf_dest_fitbv:
	pdf_left (p) = cur_h;
	break;
  case pdf_dest_fit:
  case pdf_dest_fitb:
	do_something;
	break;
  case pdf_dest_fitr:
	set_rect_dimens (p, parent_box, x, y, pdf_width (p),
			       pdf_height (p), pdf_depth (p), pdf_dest_margin);
  };
};

void
out_form (pointer p) {
  pdf_end_text();
  pdf_print_ln ('q');
  if (pdf_lookup_list (pdf_xform_list, pdf_xform_objnum (p)) == null)
	pdf_append_list (pdf_xform_objnum (p), pdf_xform_list);
  cur_v = cur_v + obj_xform_depth (pdf_xform_objnum (p));
  pdf_print_string("1 0 0 1 ");
  pdf_print_bp (pdf_x (cur_h));
  pdf_out (' ');
  pdf_print_bp (pdf_y (cur_v));
  pdf_print_ln_string(" cm");
  pdf_print_string("/Fm");
  pdf_print_int (obj_info (pdf_xform_objnum (p)));
  pdf_print_resname_prefix;
  pdf_print_ln_string(" Do");
  pdf_print_ln ('Q');
}
		
void 
out_image (pointer p) {
  int image;
  image = obj_ximage_data (pdf_ximage_objnum (p));
  pdf_end_text();
  pdf_print_ln ('q');
  if (pdf_lookup_list (pdf_ximage_list, pdf_ximage_objnum (p)) == null)
	pdf_append_list (pdf_ximage_objnum (p), pdf_ximage_list);
  if (!is_pdf_image (image)) {
	pdf_print_real (ext_xn_over_d (pdf_width (p), 10000, one_bp), 4);
	/* 1000000,6 leads to overflows with large images */
	pdf_print_string(" 0 0 ");
	pdf_print_real (ext_xn_over_d (pdf_height (p) + pdf_depth (p), 10000, one_bp), 4);
	/* 1000000,6 leads to overflows with large images */
	pdf_out (' ');
	pdf_print_bp (pdf_x (cur_h));
	pdf_out (' ');
	pdf_print_bp (pdf_y (cur_v));
  } else {
	pdf_print_real (ext_xn_over_d (pdf_width (p), 1000000,image_width (image)), 6);
	pdf_print_string(" 0 0 ");
	pdf_print_real (ext_xn_over_d(pdf_height (p) + pdf_depth (p), 1000000,image_height (image)), 6);
	pdf_out (' ');
	pdf_print_bp (pdf_x (cur_h) -ext_xn_over_d (pdf_width (p),epdf_orig_x (image),image_width (image)));
	pdf_out (' ');
	pdf_print_bp (pdf_y (cur_v) -ext_xn_over_d (pdf_height (p),epdf_orig_y (image),image_height (image)));
  };
  pdf_print_ln_string(" cm");
  pdf_print_string("/Im");
  pdf_print_int (obj_info (pdf_ximage_objnum (p)));
  pdf_print_resname_prefix;
  pdf_print_ln_string(" Do");
  pdf_print_ln ('Q');
}

/* module 1583 */
boolean is_shipping_page; /* set to |shipping_page| when |pdf_ship_out|  starts */

