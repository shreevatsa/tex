

#include "types.h"
#include "c-compat.h"


#include "globals.h"



/* module 1485 */

/* 
 * The program above includes a bunch of ``hooks'' that allow further
 * capabilities to be added without upsetting \TeX's basic structure.
 * Most of these hooks are concerned with ``whatsit'' nodes, which are
 * intended to be used for special purposes; whenever a new extension to
 * \TeX\ involves a new kind of whatsit node, a corresponding change needs
 * to be made to the routines below that deal with such nodes,
 * but it will usually be unnecessary to make many changes to the
 * other parts of this program.
 * 
 * In order to demonstrate how extensions can be made, we shall treat
 * `\.{\\write}', `\.{\\openout}', `\.{\\closeout}', `\.{\\immediate}',
 * `\.{\\special}', and `\.{\\setlanguage}' as if they were extensions.
 * These commands are actually primitives of \TeX, and they should
 * appear in all implementations of the system; but let's try to imagine
 * that they aren't. Then the program below illustrates how a person
 * could add them.
 * 
 * Sometimes, of course, an extension will require changes to \TeX\ itself;
 * no system of hooks could be complete enough for all conceivable extensions.
 * The features associated with `\.{\\write}' are almost all confined to the
 * following paragraphs, but there are small parts of the |print_ln| and
 * |print_char| procedures that were introduced specifically to \.{\\write}
 * characters. Furthermore one of the token lists recognized by the scanner
 * is a |write_text|; and there are a few other miscellaneous places where we
 * have already provided for some aspect of \.{\\write}. The goal of a \TeX\
 * extender should be to minimize alterations to the standard parts of the
 * program, and to avoid them completely if possible. He or she should also
 * be quite sure that there's no easy way to accomplish the desired goals
 * with the standard features that \TeX\ already has. ``Think thrice before
 * extending,'' because that may save a lot of work, and it will also keep
 * incompatible extensions of \TeX\ from proliferating.
 */


/* module 1487 */

/* The sixteen possible \.{\\write} streams are represented by the |write_file|
 * array. The |j|th file is open if and only if |write_open[j]=true|. The last
 * two streams are special; |write_open[16]| represents a stream number
 * greater than 15, while |write_open[17]| represents a negative stream number,
 * and both of these variables are always |false|.
 */
FILE * write_file[16];
boolean write_open[18];


/* module 1490 */

/* The variable |write_loc| just introduced is used to provide an
 * appropriate error message in case of ``runaway'' write texts.
 */
pointer write_loc; /* |eqtb| address of \.{\\write} */

/* module 1493 */
void 
do_extension (void) {
  int i, j, k; /* all-purpose integers */ 
  pointer p, q, r; /* all-purpose pointers */ 
  switch (cur_chr) {
  case open_node:
	/* begin expansion of Implement \.{\\openout} */
	/* module 1496 */
	new_write_whatsit (open_node_size);
	scan_optional_equals();
	scan_file_name();
	open_name (tail) = cur_name;
	open_area (tail) = cur_area;
	open_ext (tail) = cur_ext;
	/* end expansion of Implement \.{\\openout} */
	break;
  case write_node:
	/* begin expansion of Implement \.{\\write} */
	/* module 1497 */
	/* When `\.{\\write 12\{...\}}' appears, we scan the token list `\.{\{...\}}'
	 * without expanding its macros; the macros will be expanded later when this
	 * token list is rescanned.
	 */
	k = cur_cs;
	new_write_whatsit (write_node_size);
	cur_cs = k;
	p = scan_toks (false, false);
	write_tokens (tail) = def_ref;
	/* end expansion of Implement \.{\\write} */
	break;
  case close_node:
	/* begin expansion of Implement \.{\\closeout} */
	/* module 1498 */
	new_write_whatsit (write_node_size);
	write_tokens (tail) = null;
	/* end expansion of Implement \.{\\closeout} */
	break;
  case special_node:
	/* begin expansion of Implement \.{\\special} */
	/* module 1499 */
	/* When `\.{\\special\{...\}}' appears, we expand the macros in the token
	 * list as in \.{\\xdef} and \.{\\mark}.
	 */
	new_whatsit (special_node, write_node_size);
	write_stream (tail) = null;
	p = scan_toks (false, true);
	write_tokens (tail) = def_ref;
	/* end expansion of Implement \.{\\special} */
	break;
  case immediate_code:
	/* begin expansion of Implement \.{\\immediate} */
	/* module 1571 */
	/* The presence of `\.{\\immediate}' causes the |do_extension| procedure
	 * to descend to one level of recursion. Nothing happens unless \.{\\immediate}
	 * is followed by `\.{\\openout}', `\.{\\write}', or `\.{\\closeout}'.
	 */
	get_x_token();
	if (cur_cmd == extension) {
	  if (cur_chr <= close_node) {
		p = tail;
		do_extension(); /* append a whatsit node */
		out_what (tail);  /* do the action immediately */
		flush_node_list (tail);
		tail = p;
		link (p) = null;
	  } else {
		switch (cur_chr) {
		case pdf_obj_code:
		  do_extension();  /* scan object and set |pdf_last_obj| */
		  if (obj_data_ptr (pdf_last_obj) == 0) /* this object has not been initialized yet */
			pdf_error_string("ext1","`\\pdfobj reserveobjnum' cannot be used with \\immediate");
		  pdf_write_obj (pdf_last_obj);
		  break;
		case pdf_xform_code:
		  do_extension(); /* scan form and set |pdf_last_xform| */
		  pdf_cur_form = pdf_last_xform;
		  pdf_ship_out (obj_xform_box (pdf_last_xform), false);
		  break;
		case pdf_ximage_code:
		  do_extension(); /* scan image and set |pdf_last_ximage| */
		  pdf_write_image (pdf_last_ximage);
		};
	  };
	} else {
	  back_input();
	}
	/* end expansion of Implement \.{\\immediate} */
	break;
  case set_language_code:
	/* begin expansion of Implement \.{\\setlanguage} */
	/* module 1573 */
	if (abs (mode) != hmode) {
	  report_illegal_case();
	} else {
	  new_whatsit (language_node, small_node_size);
	  scan_int();
	  if (cur_val <= 0) {
		clang = 0;
	  } else if (cur_val > 255) {
		clang = 0;
	  } else {
		clang = cur_val;
	  }
	  what_lang (tail) = clang;
	  what_lhm (tail) = norm_min (left_hyphen_min);
	  what_rhm (tail) = norm_min (right_hyphen_min);
	};
	/* end expansion of Implement \.{\\setlanguage} */
	break;
  case pdf_literal_node:
	/* begin expansion of Implement \.{\\pdfliteral} */
	/* module 1502 */
	check_pdfoutput("\\pdfliteral");
	new_whatsit (pdf_literal_node, write_node_size);
	if (scan_keyword ("direct")) {
	  pdf_literal_direct (tail) = 1;
	} else {
	  pdf_literal_direct (tail) = 0;
	}
	scan_pdf_ext_toks;
	pdf_literal_data (tail) = def_ref;
	/* end expansion of Implement \.{\\pdfliteral} */
	break;
  case pdf_obj_code:
	/* begin expansion of Implement \.{\\pdfobj} */
	/* module 1504 */
	check_pdfoutput("\\pdfobj");
	if (scan_keyword ("reserveobjnum")) {
	  incr (pdf_obj_count);
	  pdf_create_obj (obj_type_obj, pdf_obj_count);
	  pdf_last_obj = obj_ptr;
	} else {
	  if (scan_keyword ("useobjnum")) {
		scan_int();
		k = cur_val;
		if ((k == 0) || (obj_data_ptr (k) != 0))
		  pdf_error_string("ext1","this object numer appears to have been used");
	  }  else {
		incr (pdf_obj_count);
		pdf_create_obj (obj_type_obj, pdf_obj_count);
		k = obj_ptr;
	  };
	  obj_data_ptr (k) = pdf_get_mem (pdfmem_obj_size);
	  if (scan_keyword ("stream")) {
		obj_obj_is_stream (k) = 1;
		if (scan_keyword ("attr")) {
		  scan_pdf_ext_toks;
		  obj_obj_stream_attr (k) = def_ref;
		} else {
		  obj_obj_stream_attr (k) = null;
		}
	  } else {
		obj_obj_is_stream (k) = 0;
	  }
	  if (scan_keyword ("file")) {
		obj_obj_is_file (k) = 1;
	  } else {
		obj_obj_is_file (k) = 0;
	  }
	  scan_pdf_ext_toks;
	  obj_obj_data (k) = def_ref;
	  pdf_last_obj = k;
	};
	/* end expansion of Implement \.{\\pdfobj} */
	break;
  case pdf_refobj_node:
	/* begin expansion of Implement \.{\\pdfrefobj} */
	/* module 1506 */
	check_pdfoutput("\\pdfrefobj");
	scan_int();
	pdf_check_obj (obj_type_obj, cur_val);
	new_whatsit (pdf_refobj_node, pdf_refobj_node_size);
	pdf_obj_objnum (tail) = cur_val;
	/* end expansion of Implement \.{\\pdfrefobj} */
	break;
  case pdf_xform_code:
	/* begin expansion of Implement \.{\\pdfxform} */
	/* module 1508 */
	check_pdfoutput("\\pdfxform");
	incr (pdf_xform_count);
	pdf_create_obj (obj_type_xform, pdf_xform_count);
	k = obj_ptr;
	obj_data_ptr (k) = pdf_get_mem (pdfmem_xform_size);
	if (scan_keyword ("attr")) {
	  scan_pdf_ext_toks;
	  obj_xform_attr (k) = def_ref;
	} else {
	  obj_xform_attr (k) = null;
	}
	if (scan_keyword ("resources")) {
	  scan_pdf_ext_toks;
	  obj_xform_resources (k) = def_ref;
	} else {
	  obj_xform_resources (k) = null;
	}
	scan_int();
	if (box (cur_val) == null)
	  pdf_error_string("ext1","\\pdfxform cannot be used with a void box");
	obj_xform_width (k) = width (box (cur_val));
	obj_xform_height (k) = height (box (cur_val));
	obj_xform_depth (k) = depth (box (cur_val));
	obj_xform_box (k) = box (cur_val); /* save pointer to the box */ 
	box (cur_val) = null;
	pdf_last_xform = k;
	/* end expansion of Implement \.{\\pdfxform} */
	break;
  case pdf_refxform_node:
	/* begin expansion of Implement \.{\\pdfrefxform} */
	/* module 1509 */
	check_pdfoutput("\\pdfrefxform");
	scan_int();
	pdf_check_obj (obj_type_xform, cur_val);
	new_whatsit (pdf_refxform_node,pdf_refxform_node_size);
	pdf_xform_objnum (tail) = cur_val;
	pdf_width (tail) = obj_xform_width (cur_val);
	pdf_height (tail) = obj_xform_height (cur_val);
	pdf_depth (tail) = obj_xform_depth (cur_val);
	/* end expansion of Implement \.{\\pdfrefxform} */
	break;
  case pdf_ximage_code:
	/* begin expansion of Implement \.{\\pdfximage} */
	/* module 1513 */
	check_pdfoutput("\\pdfximage");
	check_and_set_pdfoptionpdfminorversion();
	scan_image();
	/* end expansion of Implement \.{\\pdfximage} */
	break;
  case pdf_refximage_node:
	/* begin expansion of Implement \.{\\pdfrefximage} */
	/* module 1514 */
	check_pdfoutput("\\pdfrefximage");
	scan_int();
	pdf_check_obj (obj_type_ximage, cur_val);
	new_whatsit (pdf_refximage_node, pdf_refximage_node_size);
	pdf_ximage_objnum (tail) = cur_val;
	pdf_width (tail) = obj_ximage_width (cur_val);
	pdf_height (tail) = obj_ximage_height (cur_val);
	pdf_depth (tail) = obj_ximage_depth (cur_val);
	/* end expansion of Implement \.{\\pdfrefximage} */
	break;
  case pdf_annot_node:
	/* begin expansion of Implement \.{\\pdfannot} */
	/* module 1518 */
	check_pdfoutput("\\pdfannot");
	k = pdf_new_objnum();
	new_annot_whatsit (pdf_annot_node,pdf_annot_node_size);
	pdf_annot_objnum (tail) = k;
    scan_toks (false, true);
	pdf_annot_data (tail) = def_ref;
	pdf_last_annot = k;
	/* end expansion of Implement \.{\\pdfannot} */
	break;
  case pdf_start_link_node:
	/* begin expansion of Implement \.{\\pdfstartlink} */
	/* module 1519 */
	check_pdfoutput("\\pdfstartlink");
	if (abs (mode) == vmode)
	  pdf_error_string("ext1","\\pdfstartlink cannot be used in vertical mode");
	new_annot_whatsit (pdf_start_link_node,pdf_annot_node_size);
	pdf_link_action (tail) = scan_action();
	/* end expansion of Implement \.{\\pdfstartlink} */
	break;
  case pdf_end_link_node:
	/* begin expansion of Implement \.{\\pdfendlink} */
	/* module 1520 */
	check_pdfoutput("\\pdfendlink");
	if (abs (mode) == vmode)
	  pdf_error_string("ext1","\\pdfendlink cannot be used in vertical mode");
	new_whatsit (pdf_end_link_node, small_node_size);
	/* end expansion of Implement \.{\\pdfendlink} */
	break;
  case pdf_outline_code:
	/* begin expansion of Implement \.{\\pdfoutline} */
	/* module 1522 */
	check_pdfoutput("\\pdfoutline");
	if (scan_keyword ("attr")) {
	  scan_pdf_ext_toks;
	  r = def_ref;
	} else {
	  r = 0;
	}
	p = scan_action();
	if (scan_keyword ("count")) {
	  scan_int();
	  i = cur_val;
	}  else {
	  i = 0;
	}
	scan_toks (false, true);
	q = def_ref;
	pdf_new_obj (obj_type_others, 0);
	j = obj_ptr;
	write_action (p);
	pdf_end_obj();
	delete_action_ref (p);
	pdf_create_obj (obj_type_outline, 0);
	k = obj_ptr;
	obj_outline_ptr (k) = pdf_get_mem (pdfmem_outline_size);
	obj_outline_action_objnum (k) = j;
	obj_outline_count (k) = i;
	pdf_new_obj (obj_type_others, 0);
	pdf_print_str_ln (tokens_to_string (q));
	flush_str (last_tokens_string);
	delete_token_ref (q);
	pdf_end_obj();
	obj_outline_title (k) = obj_ptr;
	obj_outline_prev (k) = 0;
	obj_outline_next (k) = 0;
	obj_outline_first (k) = 0;
	obj_outline_last (k) = 0;
	obj_outline_parent (k) = pdf_parent_outline;
	obj_outline_attr (k) = r;
	if (pdf_first_outline == 0)
	  pdf_first_outline = k;
	if (pdf_last_outline == 0) {
	  if (pdf_parent_outline != 0)
		obj_outline_first (pdf_parent_outline) = k;
	} else {
	  obj_outline_next (pdf_last_outline) = k;
	  obj_outline_prev (k) = pdf_last_outline;
	};
	pdf_last_outline = k;
	if (obj_outline_count (k) != 0) {
	  pdf_parent_outline = k;
	  pdf_last_outline = 0;
	} else if ((pdf_parent_outline != 0)
			   && (outline_list_count (k) == abs (obj_outline_count(pdf_parent_outline)))) {
	  j = pdf_last_outline;
	  do {
		obj_outline_last (pdf_parent_outline) = j;
		j = pdf_parent_outline;
		pdf_parent_outline =  obj_outline_parent (pdf_parent_outline);
	  } while (!((pdf_parent_outline == 0) || (outline_list_count (j) < abs (obj_outline_count(pdf_parent_outline)))));
	  if (pdf_parent_outline == 0) {
		pdf_last_outline = pdf_first_outline;
	  } else {
		pdf_last_outline =obj_outline_first (pdf_parent_outline);
	  }
	  while (obj_outline_next (pdf_last_outline) != 0)
		pdf_last_outline = obj_outline_next (pdf_last_outline);
	};
	/* end expansion of Implement \.{\\pdfoutline} */
    do_something;
	break;
  case pdf_dest_node:
	/* begin expansion of Implement \.{\\pdfdest} */
	/* module 1524 */
	/* Notice that |scan_keyword| doesn't care if two words have same prefix; so
	 * we should be careful when scan keywords with same prefix. The main rule: if
	 * there are two or more keywords with the same prefix, then always test in
	 * order from the longest one to the shortest one.
	 */
	check_pdfoutput("\\pdfdest");
	q = tail;
	new_whatsit (pdf_dest_node, pdf_dest_node_size);
	if (scan_keyword ("num")) {
	  scan_int();
	  if (cur_val <= 0)
		pdf_error_string("ext1","num identifier must be positive");
	  if (cur_val > max_halfword)
		pdf_error_string("ext1","number too big");
	  pdf_dest_id (tail) = cur_val;
	  pdf_dest_named_id (tail) = 0;
	}  else if (scan_keyword ("name")) {
	  scan_toks (false, true);
	  pdf_dest_id (tail) = def_ref;
	  pdf_dest_named_id (tail) = 1;
	} else {
	  pdf_error_string("ext1","identifier type missing");
	}
	if (scan_keyword ("xyz")) {
	  pdf_dest_type (tail) = pdf_dest_xyz;
	  if (scan_keyword ("zoom")) {
		scan_int();
		if (cur_val > max_halfword)
		  pdf_error_string("ext1","number too big");
		pdf_dest_xyz_zoom (tail) = cur_val;
	  } else {
		pdf_dest_xyz_zoom (tail) = null;
	  }
	} else if (scan_keyword ("fitbh")) {
	  pdf_dest_type (tail) = pdf_dest_fitbh;
	} else if (scan_keyword ("fitbv")) {
	  pdf_dest_type (tail) = pdf_dest_fitbv;
	} else if (scan_keyword ("fitb")) {
	  pdf_dest_type (tail) = pdf_dest_fitb;
	} else if (scan_keyword ("fith")) {
	  pdf_dest_type (tail) = pdf_dest_fith;
	} else if (scan_keyword ("fitv")) {
	  pdf_dest_type (tail) = pdf_dest_fitv;
	} else if (scan_keyword ("fitr")) {
	  pdf_dest_type (tail) = pdf_dest_fitr;
	} else if (scan_keyword ("fit")) {
	  pdf_dest_type (tail) = pdf_dest_fit;
	} else {
	  pdf_error_string("ext1","destination type missing");
	}
	if (pdf_dest_type (tail) == pdf_dest_fitr) {
	  scan_alt_rule(); /* scans |<rule spec>| to |alt_rule| */
	  pdf_width (tail) = width (alt_rule);
	  pdf_height (tail) = height (alt_rule);
	  pdf_depth (tail) = depth (alt_rule);
	};
	if (pdf_dest_named_id (tail) != 0) {
	  i = tokens_to_string (pdf_dest_id (tail));
	  k = find_obj (obj_type_dest, i, true);
	  flush_str (i);
	} else {
	  k = find_obj (obj_type_dest, pdf_dest_id (tail),false);
	}
	if ((k != 0) && (obj_dest_ptr (k) != null)) {
	  warn_dest_dup (pdf_dest_id (tail), pdf_dest_named_id (tail),"ext4","has been already used, duplicate ignored");
	  flush_node_list (tail);
	  tail = q;
	  link (q) = null;
	};
	/* end expansion of Implement \.{\\pdfdest} */
	break;
  case pdf_thread_node:
	/* begin expansion of Implement \.{\\pdfthread} */
	/* module 1526 */
	check_pdfoutput("\\pdfthread");
	new_annot_whatsit (pdf_thread_node, pdf_thread_node_size);
	scan_thread_id();
	/* end expansion of Implement \.{\\pdfthread} */
	break;
  case pdf_start_thread_node:
	/* begin expansion of Implement \.{\\pdfstartthread} */
	/* module 1527 */
	check_pdfoutput("\\pdfstartthread");
	new_annot_whatsit (pdf_start_thread_node, pdf_thread_node_size);
	scan_thread_id();
	/* end expansion of Implement \.{\\pdfstartthread} */
	break;
  case pdf_end_thread_node:
	/* begin expansion of Implement \.{\\pdfendthread} */
	/* module 1528 */
	check_pdfoutput("\\pdfendthread");
	new_whatsit (pdf_end_thread_node, small_node_size);
	/* end expansion of Implement \.{\\pdfendthread} */
	break;
  case pdf_save_pos_node:
	/* begin expansion of Implement \.{\\pdfsavepos} */
	/* module 1531 */
	check_pdfoutput("\\pdfsavepos");
	new_whatsit (pdf_save_pos_node, small_node_size);
	/* end expansion of Implement \.{\\pdfsavepos} */
	break;
  case pdf_snap_ref_point_node:
	/* begin expansion of Implement \.{\\pdfsnaprefpoint} */
	/* module 1532 */
	check_pdfoutput("\\pdfsnaprefpoint");
	new_whatsit (pdf_snap_ref_point_node,small_node_size);
	/* end expansion of Implement \.{\\pdfsnaprefpoint} */
	break;
  case pdf_snap_x_node:
	/* begin expansion of Implement \.{\\pdfsnapx} */
	/* module 1535 */
	check_pdfoutput("\\pdfsnapx");
	tail_append (new_snap_node (pdf_snap_x_node));
	/* end expansion of Implement \.{\\pdfsnapx} */
	break;
  case pdf_snap_y_node:
	/* begin expansion of Implement \.{\\pdfsnapy} */
	/* module 1536 */
	check_pdfoutput("\\pdfsnapy");
	tail_append (new_snap_node (pdf_snap_y_node));
	/* end expansion of Implement \.{\\pdfsnapy} */
	break;
  case pdf_line_snap_x_code:
	/* begin expansion of Implement \.{\\pdflinesnapx} */
	/* module 1537 */
	check_pdfoutput("\\pdflinesnapx");
	if (pdf_line_snap_x != null)
	  flush_node_list (pdf_line_snap_x);
	pdf_line_snap_x = new_snap_node (pdf_snap_x_node);
	/* end expansion of Implement \.{\\pdflinesnapx} */
	break;
  case pdf_line_snap_y_code:
	/* begin expansion of Implement \.{\\pdflinesnapy} */
	/* module 1538 */
	check_pdfoutput("\\pdflinesnapy");
	if (pdf_line_snap_y != null)
	  flush_node_list (pdf_line_snap_y);
	pdf_line_snap_y = new_snap_node (pdf_snap_y_node);
	/* end expansion of Implement \.{\\pdflinesnapy} */
	break;
  case pdf_info_code:
	/* begin expansion of Implement \.{\\pdfinfo} */
	/* module 1540 */
	check_pdfoutput("\\pdfinfo");
	scan_toks (false, true);
	pdf_info_toks = concat_tokens (pdf_info_toks, def_ref);
	/* end expansion of Implement \.{\\pdfinfo} */
	break;
  case pdf_catalog_code:
	/* begin expansion of Implement \.{\\pdfcatalog} */
	/* module 1541 */
	check_pdfoutput("\\pdfcatalog");
	scan_toks (false, true);
	pdf_catalog_toks = concat_tokens (pdf_catalog_toks, def_ref);
	if (scan_keyword ("openaction")) {
	  if (pdf_catalog_openaction != 0) {
		pdf_error_string("ext1", "duplicate of openaction" );
	  } else {
		p = scan_action();
		pdf_new_obj (obj_type_others, 0);
		pdf_catalog_openaction = obj_ptr;
		write_action (p);
		pdf_end_obj();
		delete_action_ref (p);
	  };
	}
	/* end expansion of Implement \.{\\pdfcatalog} */
	break;
  case pdf_names_code:
	/* begin expansion of Implement \.{\\pdfnames} */
	/* module 1542 */
	check_pdfoutput("\\pdfnames");
	scan_toks (false, true);
	pdf_names_toks = concat_tokens (pdf_names_toks, def_ref);
	/* end expansion of Implement \.{\\pdfnames} */
	break;
  case pdf_include_chars_code:
	/* begin expansion of Implement \.{\\pdfincludechars} */
	/* module 1545 */
	check_pdfoutput("\\pdfincludechars");
	pdf_include_chars();
	/* end expansion of Implement \.{\\pdfincludechars} */
	break;
  case pdf_font_attr_code:
	/* begin expansion of Implement \.{\\pdffontattr} */
	/* module 1546 */
	check_pdfoutput("\\pdffontattr");
	scan_font_ident();
	k = cur_val;
	if (k == null_font)
	  pdf_error_string("font","invalid font identifier");
	scan_toks (false, true);
	pdf_font_attr[k] = tokens_to_string (def_ref);
	/* end expansion of Implement \.{\\pdffontattr} */
	break;
  case pdf_font_expand_code:
	/* begin expansion of Implement \.{\\pdffontexpand} */
	/* module 1547 */
	do_expand_font();
	/* end expansion of Implement \.{\\pdffontexpand} */
	break;
  case pdf_map_file_code:
	/* begin expansion of Implement \.{\\pdfmapfile} */
	/* module 1548 */
	check_pdfoutput("\\pdfmapfile");
	scan_toks (false, true);
	pdfmapfile (def_ref);
	delete_token_ref (def_ref);
	/* end expansion of Implement \.{\\pdfmapfile} */
	break;
  case pdf_trailer_code:
	/* begin expansion of Implement \.{\\pdftrailer} */
	/* module 1543 */
	check_pdfoutput("\\pdftrailer");
	scan_toks (false, true);
	pdf_trailer_toks = concat_tokens (pdf_trailer_toks, def_ref);
	/* end expansion of Implement \.{\\pdftrailer} */
  };
};

/* module 1494 */

/* Here is a subroutine that creates a whatsit node having a given |subtype|
 * and a given number of words. It initializes only the first word of the whatsit,
 * and appends it to the current list.
 */
void 
new_whatsit (small_number s, small_number w) {
  pointer p; /* the new node */ 
  p = get_node (w);
  type (p) = whatsit_node;
  subtype (p) = s;
  link (tail) = p;
  tail = p;
};



/* module 1495 */

/* The next subroutine uses |cur_chr| to decide what sort of whatsit is
 * involved, and also inserts a |write_stream| number.
 */
void 
new_write_whatsit (small_number w) {
  new_whatsit (cur_chr, w);
  if (w != write_node_size) {
	scan_four_bit_int();
  } else {
	scan_int();
	if (cur_val < 0) {
	  cur_val = 17;
	} else if ((cur_val > 15) && (cur_val != 18))
	  cur_val = 16;
  };
  write_stream (tail) = cur_val;
};

/* module 1551 */

/* Each new type of node that appears in our data structure must be capable
 * of being displayed, copied, destroyed, and so on. The routines that we
 * need for write-oriented whatsits are somewhat like those for mark nodes;
 * other extensions might, of course, involve more subtlety here.
 */

void
print_write_whatsit (char *s, pointer p) {
  print_esc_string ((s));
  if (write_stream (p) < 16) {
	print_int (write_stream (p));
  } else if (write_stream (p) == 16) {
	print_char ('*');
  } else {
	print_char ('-');
  };
};


/* module 1566 */
void
write_out (pointer p) {
  unsigned char old_setting; /* holds zprint |selector| */ 
  int old_mode; /* saved |mode| */ 
  small_number j; /* write stream number */ 
  pointer q, r; /* temporary variables for list manipulation */ 
  int d; /* number of characters in incomplete current string */ 
  boolean clobbered; /* system string is ok? */
  /* begin expansion of Expand macros in the token list and make |link(def_ref)| point to the result */
  /* module 1567 */
  /* The final line of this routine is slightly subtle; at least, the author
   * didn't think about it until getting burnt! There is a used-up token list
   * 
   * on the stack, namely the one that contained |end_write_token|. (We
   * insert this artificial `\.{\\endwrite}' to prevent runaways, as explained
   * above.) If it were not removed, and if there were numerous writes on a
   * single page, the stack would overflow.
   */
  q = get_avail();
  info (q) = right_brace_token + '}';
  r = get_avail();
  link (q) = r;
  info (r) = end_write_token;
  ins_list (q);
  begin_token_list (write_tokens (p), write_text);
  q = get_avail();
  info (q) = left_brace_token + '{';
  ins_list (q);
  /* now we're ready to scan `\.\{$\langle\,$token list$\,\rangle$\.{\} \\endwrite}' */
  old_mode = mode;
  mode = 0;
  /* disable \.{\\prevdepth}, \.{\\spacefactor}, \.{\\lastskip}, \.{\\prevgraf} */
  cur_cs = write_loc;
  q = scan_toks (false, true); /* expand macros, etc. */ 
  get_token();
  if (cur_tok != end_write_token) {
	/* begin expansion of Recover from an unbalanced write command */
	/* module 1568 */
      print_err ("Unbalanced write command");
      help2 ("On this page there's a \\write with fewer real {'s than }'s.",
			 "I can't handle that very well; good luck.");
      error();
      do {
		get_token();
	  } while (cur_tok != end_write_token);
  };
  /* end expansion of Recover from an unbalanced write command */
  mode = old_mode;
  end_token_list();		/* conserve stack space */
  /* end expansion of Expand macros in the token list and make |link(def_ref)| point to the result */
  old_setting = selector;
  j = write_stream (p);
  if (shell_enabled_p && (j == 18)) {
	selector = new_string;
  }else if (write_open[j]) {
    selector = j;
  } else {				/* write to the terminal if file isn't open */
	if ((j == 17) && (selector == term_and_log))
	  selector = log_only;
	print_nl_string("");
  };
  token_show (def_ref);
  print_ln();
  flush_list (def_ref);
  if (j == 18) {
	if ((tracing_online <= 0)) {
	  selector = log_only;	/* Show what we're doing in the log file. */
	} else {
	  selector = term_and_log; /* Show what we're doing. */ 
	}
	print_nl_string("system(");
	for (d = 0; d <= cur_length - 1; d++) {	 /* |zprint| gives up if passed |str_ptr|, so do it by hand. */
	  zprint (str_pool[str_start[str_ptr] + d]); /* N.B.: not |print_char| */ 
	};
	zprint_string(")...");
	if (shell_enabled_p) {
	  str_room (1);
	  append_char (0); /* Append a null byte to the expansion. */ 
	  clobbered = false;
	  for (d = 0; d <= cur_length - 1; d++) { /* Convert to external character set. */
		str_pool[str_start[str_ptr] + d] = Xchr(str_pool[str_start[str_ptr] + d]);
		if ((str_pool[str_start[str_ptr] + d] == null_code) && (d < cur_length - 1))
		  clobbered = true; 	/* minimal checking: NUL not allowed in argument string of |system|() */
	  };
	  if (clobbered) {
	    zprint_string("clobbered");
	  } else {
	    /* We have the string; run system(3). We don't have anything reasonable to do with the 
		   return status, unfortunately discard it. */
		system_exec (stringcast (address_of (str_pool[str_start[str_ptr]])));
		zprint_string("executed");
	  };
	  pool_ptr = str_start[str_ptr]; /* erase the string */ 
	} else {
	  zprint_string("disabled");
	};
	print_char ('.');
	print_nl_string("");
	print_ln();
  };
  selector = old_setting;
};



/* module 1569 */

/* The |out_what| procedure takes care of outputting whatsit nodes for
 * |vlist_out| and |hlist_out|\kern-.3pt.
 */

void
out_what (pointer p) {
  small_number j; /* write stream number */ 
  unsigned char old_setting;
  switch (subtype (p)) {
  case open_node:
  case write_node:
  case close_node:
	/* begin expansion of Do some work that has been queued up for \.{\\write} */
	/* module 1570 */
	/* We don't implement \.{\\write} inside of leaders. (The reason is that
	 * the number of times a leader box appears might be different in different
	 * implementations, due to machine-dependent rounding in the glue calculations.)
	 */
	if (!doing_leaders) {
	  j = write_stream (p);
	  if (subtype (p) == write_node) {
	    write_out (p);
	  } else {
		if (write_open[j])
		  close_out_what (write_file[j]);
		if (subtype (p) == close_node) {
		  write_open[j] = false;
		} else if (j < 16) {
		  open_out_what(p,j); /* NEW function */
		  write_open[j] = true;
		  /* If on first line of input, log file is not ready yet, so don't log. */
		  if (log_opened) {
			old_setting = selector;
			if ((tracing_online <= 0)) {
			  selector = log_only;	/* Show what we're doing in the log file. */
			} else {
			selector = term_and_log; /* Show what we're doing. */
			}
			print_nl_string("\\openout");
			print_int (j);
			zprint_string(" = `");
			print_file_name (cur_name, cur_area, cur_ext);
			zprint_string("'.");
			print_nl_string("");
			print_ln();
			selector = old_setting;
		  };
		};
	  };
	};
	/* end expansion of Do some work that has been queued up for \.{\\write} */
	break;
  case special_node:
	special_out (p);
	break;
  case language_node:
	do_something;
	break;
  default:
	confusion ("ext4");
  };
};



/* module 1572 */

/* The \.{\\language} extension is somewhat different.
 * We need a subroutine that comes into play when a character of
 * a non-|clang| language is being appended to the current paragraph.
 */

void fix_language (void) {
  ASCII_code l; /* the new current language */ 
  if (language <= 0) {
    l = 0;
  } else if (language > 255) {
    l = 0;
  } else
    l = language;
  if (l != clang) {
    new_whatsit (language_node, small_node_size);
    what_lang (tail) = l;
    clang = l;
    what_lhm (tail) = norm_min (left_hyphen_min);
    what_rhm (tail) = norm_min (right_hyphen_min);
  };
};

boolean shell_enabled_p;

/* module 1488 */
void
exten_initialize (void) {
  int k;
  for (k = 0; k <= 17; k++)
    write_open[k] = false;
}
