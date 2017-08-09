
#include "types.h"
#include "c-compat.h"
#include "globals.h"
#include "mainio.h"

/* module 675 */

/* 
 * As pdfTeX should also act as a back-end driver, it needs to support virtual
 * font too. Information about virtual font can be found in source of some
 * \.{DVI}-related programs.
 * 
 * Whenever we want to write out a character in a font to PDF output, we
 * should check whether the used font is new font (has not been used yet),
 * virtual font or real font. The array |pdf_font_type| holds flag of each used
 * font. After initialization flag of each font is set to |new_font_type|.
 * The first time when a character of a font is written out, pdfTeX looks for
 * the corresponding virtual font. If the corresponding virtual font exists, then
 * the font type is set to |virtual_font_type|; otherwise it will be set to
 * |real_font_type|. |subst_font_type| indicates fonts that have been substituted
 * during adjusting spacing. Such fonts are linked via the |pdf_font_link| array.
 */

/* module 698 */

/* 
 * To ship out a \TeX\ box to PDF page description we need to implement
 * |pdf_hlist_out|, |pdf_vlist_out| and |pdf_ship_out|, which are equivalent to
 * the \TeX' original |hlist_out|, |vlist_out| and |ship_out| resp. But first we
 * need to declare some procedures needed in |pdf_hlist_out| and |pdf_vlist_out|.
 */

void 
pdf_out_literal (pointer p) {
  unsigned char old_setting; /* holds print |selector| */ 
  str_number s;
  old_setting = selector;
  selector = new_string;
  show_token_list (link (pdf_literal_data (p)), null, pool_size - pool_ptr);
  selector = old_setting;
  s = make_string();
  if (pdf_literal_direct (p) == 1) {
	literal (s, false, false, false);
  } else {
	literal (s, true, false, false);
  }
  flush_str (s);
}

void 
pdf_special (pointer p) {
  unsigned char old_setting; /* holds print |selector| */ 
  str_number s;
  old_setting = selector;
  selector = new_string;
  show_token_list (link (write_tokens (p)), null,pool_size - pool_ptr);
  selector = old_setting;
  s = make_string();
  literal (s, true, true, true);
  flush_str (s);
}

str_number 
tokens_to_string (pointer p) { /* return a string from tokens list */
  old_setting = selector;
  selector = new_string;
  show_token_list (link (p), null, pool_size - pool_ptr);
  selector = old_setting;
  last_tokens_string = make_string();
  return last_tokens_string;
};

void 
pdf_print_toks (pointer p) { /* print tokens list |p| */
  str_number s;
  s = tokens_to_string (p);
  if (length (s) > 0)
	pdf_print (s);
  flush_str (s);
}

void 
pdf_print_toks_ln (pointer p) {	/* print tokens list |p| */
  str_number s;
  s = tokens_to_string (p);
  if (length (s) > 0) {
	pdf_print (s);
	pdf_print_nl;
  };
  flush_str (s);
}

/* module 701 */
/* Create link annottation for the current hbox if needed */
#define create_link_annot   if (running_link == null) {           \
	if (is_running (pdf_link_wd) && (pdf_link_level == cur_s)) {  \
	  append_link (this_box, left_edge, base_line);               \
	  running_link = last_link;                                   \
	};                                                            \
  } else {                                                        \
	last_link = running_link;                                     \
  }

/* module 1584 */
/* Save current position to |pdf_last_x_pos|, |pdf_last_y_pos| */
#define save_pos_to_last {                                         \
  pdf_last_x_pos = cur_h;                                          \
  if (is_shipping_page) {                                          \
	pdf_last_y_pos = cur_page_height - cur_v;                      \
  } else {                                                         \
	  pdf_last_y_pos = pdf_xform_height + pdf_xform_depth - cur_v; \
  }                                                                \
}
	
/* module 1585 */
/* Save current position to |pdf_snap_x_pos|, |pdf_snap_y_pos| */
#define save_pos_to_snap { \
  pdf_snap_x_pos = cur_h;   \
  pdf_snap_y_pos = cur_v;   \
}

/* module 700 */
/* The implementation of procedure |pdf_hlist_out| is similiar to |hlist_out| */
void 
pdf_hlist_out (void) { /* output an |hlist_node| box */
  scaled base_line; /* the baseline coordinate for this box */ 
  scaled left_edge; /* the left coordinate for this box */ 
  scaled save_h; /* what |cur_h| should pop to */ 
  pointer this_box; /* pointer to containing box */ 
  unsigned char g_sign; /* selects type of glue */ 
  pointer p; /* current position in the hlist */ 
  pointer leader_box; /* the leader box being replicated */ 
  scaled leader_wd; /* width of leader box being replicated */ 
  scaled lx; /* extra space between leader boxes */ 
  boolean outer_doing_leaders; /* were we doing leaders? */ 
  scaled edge; /* right edge of sub-box or leader space */ 
  pointer prev_p; /* one step behind |p| */ 
  pointer running_link; /* pointer to `running' link if exists */ 
  this_box = temp_ptr;
  g_sign = glue_sign (this_box);
  p = list_ptr (this_box);
  incr (cur_s);
  base_line = cur_v;
  prev_p = this_box + list_offset;
  if (eTeX_ex) {
	/* Initialize the LR stack */
	initialize_LR_stack;
	if (subtype (this_box) == dlist) {
	  if (cur_dir == right_to_left) {
		cur_dir = left_to_right;
		cur_h = cur_h - width (this_box);
	  } else {
		subtype (this_box) = min_quarterword; 
	  }
	}
	if ((cur_dir == right_to_left) && (subtype (this_box) != reversed))
	  /* Reverse the complete hlist and set the subtype to |reversed| */
	  reverse_the_hlist;
  };
  left_edge = cur_h;
  running_link = null;
  /* Create link annottation for the current hbox if needed */
  create_link_annot;
  while (p != null) {
	/* begin expansion of Output node |p| for |pdf_hlist_out| and move to the next node, 
	   maintaining the condition |cur_v=base_line| */
	/* module 702 */
  RESWITCH:
	if (is_char_node (p)) {
	  do {
		f = font (p);
		c = character (p);
		if (is_valid_char (c)) {
		  output_one_char (c);
		  cur_h = cur_h + char_width (f, char_info (f, c));
		  goto CONTINUE;
		};
		if (mltex_enabled_p) {
		  /* begin expansion of (\pdfTeX) Output a substitution, |goto continue| if not possible */
		  /* module 710 */
		  /* Get substitution information, check it, goto |found| if all is ok, otherwise goto |continue| */
		  get_substitution_info;
		FOUND:
		  /* Print character substition tracing log */
		  print_substitution_log;
		  /* begin expansion of (\pdfTeX) Rebuild character using substitution information */
		  /* module 711 */
		  base_x_height = x_height (f);
		  base_slant = slant (f) / FLOAT_CONSTANT (65536);
		  accent_slant = base_slant; /* slant of accent character font */ 
		  base_width = char_width (f, ib_c);
		  base_height = char_height (f,height_depth (ib_c));
		  accent_width = char_width (f, ia_c);
		  accent_height = char_height (f,height_depth (ia_c)); /* compute necessary horizontal shift (don't forget slant) */
		  delta =round ((base_width -accent_width) / FLOAT_CONSTANT (2) + base_height* base_slant - base_x_height * accent_slant);
		  /* 1. For centering/horizontal shifting insert a kern node. */
		  cur_h = cur_h + delta;
		  /* 2. Then insert the accent character possibly shifted up or down. */
		  if (((base_height !=base_x_height) && (accent_height > 0))) {	
			/* the accent must be shifted up or down */
			cur_v = base_line + (base_x_height - base_height);
			output_one_char (accent_c);
			cur_v = base_line;
		  } else {
			output_one_char (accent_c);
		  };
		  cur_h = cur_h + accent_width;
		  /* 3. For centering/horizontal shifting insert another kern node. */
		  cur_h = cur_h + (-accent_width - delta);
		  /* 4. Output the base character. */
		  output_one_char (base_c);
		  cur_h = cur_h + base_width;
		  /* end expansion of (\pdfTeX) Rebuild character using substitution information */
		};
		/* end expansion of (\pdfTeX) Output a substitution, |goto continue| if not possible */
	  CONTINUE:
		prev_p = link (prev_p); /* N.B.: not |prev_p:=p|, |p| might be |lig_trick| */
		p = link (p);
	  } while (is_char_node (p));
	} else {
	  /* begin expansion of Output the non-|char_node| |p| for |pdf_hlist_out| and move to the next node */
	  /* module 703 */
	  switch (type (p)) {
	  case hlist_node:
	  case vlist_node:
		/* begin expansion of (\pdfTeX) Output a box in an hlist */
		/* module 704 */
		if (list_ptr (p) == null) {
		  cur_h = cur_h + width (p);
		} else {
		  cur_v = base_line + shift_amount (p); /* shift the box down */ 
		  temp_ptr = p;
		  edge = cur_h + width (p);
		  if (cur_dir == right_to_left)
			cur_h = edge;
		  if (type (p) == vlist_node) {
			pdf_vlist_out();
		  } else {
			pdf_hlist_out();
		  }
		  cur_h = edge;
		  cur_v = base_line;
		};
		/* end expansion of (\pdfTeX) Output a box in an hlist */
		break;
	  case rule_node:
		rule_ht = height (p);
		rule_dp = depth (p);
		rule_wd = width (p);
		goto FIN_RULE;
	  case whatsit_node:
		/* begin expansion of Output the whatsit node |p| in |pdf_hlist_out| */
		/* module 1588 */
		switch (subtype (p)) {
		case pdf_literal_node:
		  pdf_out_literal (p);
		  break;
		case pdf_refobj_node:
		  pdf_append_list (pdf_obj_objnum (p), pdf_obj_list);
		  break;
		case pdf_refxform_node:
		  /* begin expansion of Output a Form node in a hlist */
		  /* module 1590 */
		  cur_v = base_line;
		  edge = cur_h;
		  out_form (p);
		  cur_h = edge + pdf_width (p);
		  cur_v = base_line;
		  /* end expansion of Output a Form node in a hlist */
		  break;
		case pdf_refximage_node:
		  /* begin expansion of Output a Image node in a hlist */
		  /* module 1589 */
		  cur_v = base_line + pdf_depth (p);
		  edge = cur_h;
		  out_image (p);
		  cur_h = edge + pdf_width (p);
		  cur_v = base_line;
		  /* end expansion of Output a Image node in a hlist */
		  break;
		case pdf_annot_node:
		  do_annot (p, this_box, left_edge, base_line);
		  break;
		case pdf_start_link_node:
		  do_link (p, this_box, left_edge, base_line);
		  break;
		case pdf_end_link_node:
		  end_link();
		  /* Create link annottation for the current hbox if needed */
		  create_link_annot;
		  break;
		case pdf_dest_node:
		  do_dest (p, this_box, left_edge, base_line);
		  break;
		case pdf_thread_node:
		  do_thread (p, this_box, left_edge, base_line);
		  break;
		case pdf_start_thread_node:
		  pdf_error_string("ext4","\\pdfstartthread ended up in hlist");
		  break;
		case pdf_end_thread_node:
		  pdf_error_string("ext4","\\pdfendthread ended up in hlist");
		  break;
		case pdf_save_pos_node:
		  /* Save current position to |pdf_last_x_pos|, |pdf_last_y_pos| */
		  save_pos_to_last;
		  break;
		case pdf_snap_ref_point_node:
		  /* Save current position to |pdf_snap_x_pos|, |pdf_snap_y_pos| */
		  save_pos_to_snap;
		  break;
		case pdf_snap_x_node:
		case pdf_snap_y_node:
		  {
			do_snap (p);
			if (list_ptr (this_box) == p) {
			  base_line = cur_v;
			  left_edge = cur_h;
			};
		  };
		  break;
		case special_node:
		  pdf_special (p);
		  break;
		default:
		  out_what (p);
		};
		/* end expansion of Output the whatsit node |p| in |pdf_hlist_out| */
		break;
	  case glue_node:
		/* begin expansion of (\pdfTeX) Move right or output leaders */
		/* module 706 */
		g = glue_ptr (p);
		rule_wd = width (g);
		if (g_sign != normal)
		  add_glue (rule_wd);
		if (subtype (p) >= a_leaders) {
		  /* begin expansion of (\pdfTeX) Output leaders in an hlist, 
			 |goto FIN_RULE| if a rule or to |NEXTP| if done */
		  /* module 707 */
		  leader_box = leader_ptr (p);
		  if (type (leader_box) == rule_node) {
			rule_ht = height (leader_box);
			rule_dp = depth (leader_box);
			goto FIN_RULE;
		  };
		  leader_wd = width (leader_box);
		  if ((leader_wd > 0) && (rule_wd > 0)) {
			rule_wd = rule_wd + 10; /* compensate for floating-point rounding */
			if (cur_dir == right_to_left)
			  cur_h = cur_h - 10;
			edge = cur_h + rule_wd;
			lx = 0;
			/* begin expansion of (\pdfTeX) Let |cur_h| be the position of the first box, 
			   and set |leader_wd+lx| to the spacing between corresponding parts of boxes */
			/* module 708 */
			if (subtype (p) == a_leaders) {
			  save_h = cur_h;
			  cur_h = left_edge + leader_wd * ((cur_h - left_edge) /leader_wd);
			  if (cur_h < save_h)
				cur_h = cur_h + leader_wd;
			} else {
			  lq = rule_wd / leader_wd; /* the number of box copies */ 
			  lr = rule_wd % leader_wd; /* the remaining space */
			  if (subtype (p) == c_leaders) {
				cur_h = cur_h + (lr / 2);
			  } else {
				lx = (2 * lr + lq + 1) / (2 * lq + 2); /* round|(lr/(lq+1))| */ 
				cur_h = cur_h + ((lr - (lq - 1) * lx) / 2);
			  };
			}
			/* end expansion of (\pdfTeX) Let |cur_h| be the position of the first box, ...*/
			while (cur_h + leader_wd <= edge) {
			  /* begin expansion of (\pdfTeX) Output a leader box at |cur_h|, then advance 
				 |cur_h| by |leader_wd+lx| */
			  /* module 709 */
			  cur_v = base_line + shift_amount (leader_box);
			  save_h = cur_h;
			  temp_ptr = leader_box;
			  if (cur_dir == right_to_left)
				cur_h = cur_h + leader_wd;
			  outer_doing_leaders = doing_leaders;
			  doing_leaders = true;
			  if (type (leader_box) == vlist_node) {
				pdf_vlist_out();
			  } else {
				pdf_hlist_out();
			  }
			  doing_leaders = outer_doing_leaders;
			  cur_v = base_line;
			  cur_h = save_h + leader_wd + lx;
			};
			/* end expansion of (\pdfTeX) Output a leader box at |cur_h|,  ..*/
			if (cur_dir == right_to_left) {
			  cur_h = edge;
			} else {
			  cur_h = edge - 10;
			}
			goto NEXTP;
		  };
		  /* end expansion of (\pdfTeX) Output leaders in an hlist, |goto FIN_RULE| ... */
		};
		do_something;
		goto MOVE_PAST;
		/* end expansion of (\pdfTeX) Move right or output leaders */
		break;
	  case margin_kern_node:
		cur_h = cur_h + width (p);
		break;
	  case kern_node:
		cur_h = cur_h + width (p);
		break;
	  case math_node:
		if (eTeX_ex) {
		  /* begin expansion of Adjust \(t)the LR stack for the |hlist_out| routine;
			 if necessary reverse an hlist segment and |goto reswitch| */
		  /* module 1662 */
		  /* Breaking a paragraph into lines while \TeXXeT\ is disabled may result
		   * in lines whith unpaired math nodes. Such hlists are silently accepted
		   * in the absence of text direction directives.
		   */
		  if (end_LR (p)) {
			if (info (LR_ptr) == end_LR_type (p)) {
			  pop_LR;
			} else {
			  if (subtype (p) > L_code)
				incr (LR_problems);
			};
		  } else {
			push_LR (p);
			if (LR_dir (p) != cur_dir) {
			  /* begin expansion of Reverse an hlist segment and |goto reswitch| */
			  /* module 1667 */
			  /* We detach the remainder of the hlist, replace the math node by
			   * an edge node, and append the reversed hlist segment to it; the tail of
			   * the reversed segment is another edge node and the remainder of the
			   * original list is attached to it.
			   */
			  save_h = cur_h;
			  temp_ptr = link (p);
			  rule_wd = width (p);
			  free_node (p, small_node_size);
			  cur_dir = reflected;
			  p = new_edge (cur_dir, rule_wd);
			  link (prev_p) = p;
			  cur_h = cur_h - left_edge + rule_wd;
			  link (p) = reverse (this_box, new_edge (reflected, 0));
			  edge_dist (p) = cur_h;
			  cur_dir = reflected;
			  cur_h = save_h;
			  goto RESWITCH;
			};
			/* end expansion of Reverse an hlist segment and |goto reswitch| */
		  };
		  type (p) = kern_node;
		}
		/* end expansion of Adjust \(t)the LR stack for the |hlist_out| routine;...*/
		cur_h = cur_h + width (p);
		break;
	  case ligature_node:
		/* Make node |p| look like a |char_node| and |goto reswitch| */
		p = mimic_char_node (p);
		goto RESWITCH;
	  case edge_node:
		/* Cases of |hlist_out| that arise in mixed direction text only */
		hlist_edge_node;
		break;
	  default:
		do_nothing;
	  };
	  do_something;
	  goto NEXTP;
	FIN_RULE:
	  /* begin expansion of (\pdfTeX) Output a rule in an hlist */
	  /* module 705 */
	  if (is_running (rule_ht))
		rule_ht = height (this_box);
	  if (is_running (rule_dp))
		rule_dp = depth (this_box);
	  rule_ht = rule_ht + rule_dp; /* this is the rule thickness */ 
	  if ((rule_ht > 0)  && (rule_wd > 0)) { /* we don't output empty rules */
		cur_v = base_line + rule_dp;
		pdf_set_rule (cur_h, cur_v, rule_wd, rule_ht);
		cur_v = base_line;
	  };
	  /* end expansion of (\pdfTeX) Output a rule in an hlist */
	MOVE_PAST:
	  cur_h = cur_h + rule_wd;
	NEXTP:
	  prev_p = p;
	  p = link (p);
	}
	/* end expansion of Output the non-|char_node| |p| for |pdf_hlist_out| and move to the next node */
	/* end expansion of Output node |p| for |pdf_hlist_out| and move to the next node, maintaining the condition |cur_v=base_line| */
  };
  if (eTeX_ex) {
	/* Check for LR anomalies at the end of |hlist_out| */
	check_LR_after_hlist_out;
	if (subtype (this_box) == dlist)
	  cur_dir = right_to_left;
  };
  decr (cur_s);
};


/* module 712 */

/* The |pdf_vlist_out| routine is similar to |pdf_hlist_out|, but a bit simpler.
 */
void 
pdf_vlist_out (void) { /* output a |pdf_vlist_node| box */
  scaled left_edge; /* the left coordinate for this box */ 
  scaled top_edge; /* the top coordinate for this box */ 
  scaled save_v; /* what |cur_v| should pop to */ 
  pointer this_box; /* pointer to containing box */ 
  unsigned char g_sign; /* selects type of glue */ 
  pointer p; /* current position in the vlist */ 
  pointer leader_box; /* the leader box being replicated */ 
  scaled leader_ht; /* height of leader box being replicated */ 
  scaled lx; /* extra space between leader boxes */ 
  boolean outer_doing_leaders;  /* were we doing leaders? */ 
  scaled edge; /* bottom boundary of leader space */ 
  this_box = temp_ptr;
  g_sign = glue_sign (this_box);
  p = list_ptr (this_box);
  incr (cur_s);
  left_edge = cur_h;
  cur_v = cur_v - height (this_box);
  top_edge = cur_v;
  /* begin expansion of Create thread for the current vbox if needed */
  /* module 713 */
  if ((last_thread != null) && is_running (pdf_thread_dp) && (pdf_thread_level == cur_s))
	append_thread (this_box, left_edge, top_edge + height (this_box));
  /* end expansion of Create thread for the current vbox if needed */
  while (p != null) {
	/* begin expansion of Output node |p| for |pdf_vlist_out| and move to the next node, 
	   maintaining the condition |cur_h=left_edge| */
	/* module 714 */
	if (is_char_node (p)) {
	  confusion ("pdfvlistout");
	} else {
	  /* begin expansion of Output the non-|char_node| |p| for |pdf_vlist_out| */
	  /* module 715 */
	  switch (type (p)) {
	  case hlist_node:
	  case vlist_node:
		/* begin expansion of (\pdfTeX) Output a box in a vlist */
		/* module 716 */
		if (list_ptr (p) == null) {
		  cur_v = cur_v + height (p) + depth (p);
		} else {
		  cur_v = cur_v + height (p);
		  save_v = cur_v;
		  if (cur_dir == right_to_left) {
			cur_h = left_edge - shift_amount (p);
		  } else {
			cur_h = left_edge + shift_amount (p); /* shift the box right */ 
		  }
		  temp_ptr = p;
		  if (type (p) == vlist_node) {
			pdf_vlist_out();
		  } else {
			pdf_hlist_out();
		  }
		  cur_v = save_v + depth (p);
		  cur_h = left_edge;
		};
		/* end expansion of (\pdfTeX) Output a box in a vlist */
		break;
	  case rule_node:
		rule_ht = height (p);
		rule_dp = depth (p);
		rule_wd = width (p);
		goto FIN_RULE;
	  case whatsit_node:
		/* begin expansion of Output the whatsit node |p| in |pdf_vlist_out| */
		/* module 1582 */
		switch (subtype (p)) {
		case pdf_literal_node:
		  pdf_out_literal (p);
		  break;
		case pdf_refobj_node:
		  pdf_append_list (pdf_obj_objnum (p), pdf_obj_list);
		  break;
		case pdf_refxform_node:
		  /* begin expansion of Output a Form node in a vlist */
		  /* module 1587 */
		  cur_v = cur_v + pdf_height (p);
		  save_v = cur_v;
		  cur_h = left_edge;
		  out_form (p);
		  cur_v = save_v + pdf_depth (p);
		  cur_h = left_edge;
		  /* end expansion of Output a Form node in a vlist */
		  break;
		case pdf_refximage_node:
		  /* begin expansion of Output a Image node in a vlist */
		  /* module 1586 */
		  cur_v = cur_v + pdf_height (p) + pdf_depth (p);
		  save_v = cur_v;
		  cur_h = left_edge;
		  out_image (p);
		  cur_v = save_v;
		  cur_h = left_edge;
		  /* end expansion of Output a Image node in a vlist */
		  break;
		case pdf_annot_node:
		  do_annot (p, this_box, left_edge, top_edge + height (this_box));
		  break;
		case pdf_start_link_node:
		  pdf_error_string("ext4","\\pdfstartlink ended up in vlist");
		  break;
		case pdf_end_link_node:
		  pdf_error_string("ext4","\\pdfendlink ended up in vlist");
		  break;
		case pdf_dest_node:
		  do_dest (p, this_box, left_edge, top_edge + height (this_box));
		  break;
		case pdf_thread_node:
		case pdf_start_thread_node:
		  do_thread (p, this_box, left_edge,top_edge + height (this_box));
		  break;
		case pdf_end_thread_node:
		  end_thread();
		  break;
		case pdf_save_pos_node:
		  /* Save current position to |pdf_last_x_pos|, |pdf_last_y_pos| */
          save_pos_to_last;
		  break;
		case pdf_snap_ref_point_node:
		  /* Save current position to |pdf_snap_x_pos|, |pdf_snap_y_pos| */
          save_pos_to_snap;
		  break;
		case pdf_snap_x_node:
		case pdf_snap_y_node:
		  do_snap (p);
		  if (list_ptr (this_box) == p) {
			top_edge = cur_v;
			left_edge = cur_h;
		  };
		  break;
		case special_node:
		  pdf_special (p);
		  break;
		default:
		  out_what (p);
		};
		/* end expansion of Output the whatsit node |p| in |pdf_vlist_out| */
		break;
	  case glue_node:
		/* begin expansion of (\pdfTeX) Move down or output leaders */
		/* module 718 */
		{
		  g = glue_ptr (p);
		  rule_ht = width (g);
		  if (g_sign != normal)
			add_glue (rule_ht);
		  if (subtype (p) >= a_leaders) {
			/* begin expansion of (\pdfTeX) Output leaders in a vlist, |goto FIN_RULE| if a rule or to |NEXTP| if done */
			/* module 719 */
			leader_box = leader_ptr (p);
			if (type (leader_box) == rule_node) {
			  rule_wd = width (leader_box);
			  rule_dp = 0;
			  goto FIN_RULE;
			};
			leader_ht = height (leader_box) + depth (leader_box);
			if ((leader_ht > 0) && (rule_ht > 0)) {
			  rule_ht = rule_ht + 10; /* compensate for floating-point rounding */
			  edge = cur_v + rule_ht;
			  lx = 0;
			  /* begin expansion of (\pdfTeX) Let |cur_v| be the position of the first box, and
				 set |leader_ht+lx| to the spacing between corresponding parts of boxes */
			  /* module 720 */
			  if (subtype (p) == a_leaders) {
				save_v = cur_v;
				cur_v = top_edge + leader_ht * ((cur_v - top_edge) / leader_ht);
				if (cur_v < save_v)
				  cur_v = cur_v + leader_ht;
			  } else  {
				lq = rule_ht / leader_ht; /* the number of box copies */ 
				lr = rule_ht % leader_ht; /* the remaining space */
				if (subtype (p) == c_leaders) {
				  cur_v = cur_v + (lr / 2);
				} else {
				  lx = (2 * lr + lq + 1) / (2 * lq + 2); /* round|(lr/(lq+1))| */ 
				  cur_v = cur_v + ((lr - (lq - 1) * lx) / 2);
				};
			  }
			  /* end expansion of (\pdfTeX) Let |cur_v| be the position of the first box ...*/
			  while (cur_v + leader_ht <= edge) {
				/* begin expansion of (\pdfTeX) Output a leader box at |cur_v|, then advance |cur_v| by |leader_ht+lx| */
				/* module 721 */
				if (cur_dir == right_to_left) {
				  cur_h = left_edge - shift_amount (leader_box);
				} else {
				  cur_h = left_edge + shift_amount (leader_box);
				}
				cur_v = cur_v + height (leader_box);
				save_v = cur_v;
				temp_ptr = leader_box;
				outer_doing_leaders = doing_leaders;
				doing_leaders = true;
				if (type (leader_box) == vlist_node) {
				  pdf_vlist_out();
				} else {
				  pdf_hlist_out();
				}
				doing_leaders = outer_doing_leaders;
				cur_h = left_edge;
				cur_v = save_v - height (leader_box) + leader_ht + lx;
			  };
			  /* end expansion of (\pdfTeX) Output a leader box at |cur_v|, then advance |cur_v| by |leader_ht+lx| */
			  cur_v = edge - 10;
			  goto NEXTP;
			};
		  };
		  /* end expansion of (\pdfTeX) Output leaders in a vlist, |goto FIN_RULE| if a rule or to |NEXTP| if done */
		  do_something;
		  goto MOVE_PAST;
		};
		/* end expansion of (\pdfTeX) Move down or output leaders */
		break;
	  case kern_node:
		cur_v = cur_v + width (p);
		break;
	  default:
		do_nothing;
	  };
	  do_something;
	  goto NEXTP;
	FIN_RULE:
	  /* begin expansion of (\pdfTeX) Output a rule in a vlist, |goto NEXTP| */
	  /* module 717 */
	  if (is_running (rule_wd))
		rule_wd = width (this_box);
	  rule_ht = rule_ht + rule_dp; /* this is the rule thickness */ 
	  cur_v = cur_v + rule_ht;
	  if ((rule_ht > 0) && (rule_wd > 0)) {	/* we don't output empty rules */
		if (cur_dir == right_to_left)
		  cur_h = cur_h - rule_wd;
		pdf_set_rule (cur_h, cur_v, rule_wd, rule_ht);
		cur_h = left_edge;
	  };
	  goto NEXTP;
	  /* end expansion of (\pdfTeX) Output a rule in a vlist, |goto NEXTP| */
	MOVE_PAST:
	  cur_v = cur_v + rule_ht;
	};
	/* end expansion of Output the non-|char_node| |p| for |pdf_vlist_out| */
  NEXTP:
	p = link (p);
  };
  /* end expansion of Output node |p| for |pdf_vlist_out| and move to the next node, maintaining the condition |cur_h=left_edge| */
  decr (cur_s);
};

/* module 726 */
/* Reset resources lists */
#define  reset_resource_lists pdf_font_list = null; \
  pdf_obj_list = null;\
  pdf_xform_list = null;\
  pdf_ximage_list = null;\
  pdf_text_procset = false;\
  pdf_image_procset = 0


/* module 722 */

/* |pdf_ship_out| is used instead of |ship_out| to shipout a box to PDF
 * output. If |shipping_page| is not set then the output will be a Form object,
 * otherwise it will be a Page object.
 */
void 
pdf_ship_out (pointer p, boolean shipping_page) {  /* output the box |p| */
  int i, j, k; /* general purpose accumulators */ 
  /* int r; *//* accumulator to copy node for pending link annotation */
  pointer save_font_list;  /* to save |pdf_font_list| during flushing pending forms */
  pointer save_obj_list; /* to save |pdf_obj_list| */ 
  pointer save_ximage_list; /* to save |pdf_ximage_list| */ 
  pointer save_xform_list; /* to save |pdf_xform_list| */ 
  int save_image_procset; /* to save |pdf_image_procset| */ 
  int save_text_procset; /* to save |pdf_text_procset| */ 
  int pdf_last_resources;  /* pointer to most recently generated Resources object */
  /* str_number s;*/
  /*unsigned char old_setting;*/ /* saved |selector| setting */ 
  if (tracing_output > 0) {
	print_nl_string("");
	print_ln();
	zprint_string("Completed box being shipped out");
  };
  if (!init_pdf_output) {
	/* begin expansion of Initialize variables for \.{PDF} output */
	/* module 765 */
	ensure_pdf_open();
	check_and_set_pdfoptionpdfminorversion();
	prepare_mag();
	if ((mag != 1000) && (mag != 0)) {
	  do_mag_cfg_dimen_pars();
	};
	fixed_decimal_digits = fix_int (pdf_decimal_digits, 0, 4);
	min_bp_val = divide_scaled (one_hundred_bp,ten_pow[fixed_decimal_digits + 2], 0);
	fixed_pk_resolution = fix_int (pdf_pk_resolution, 72, 2400);
	pk_scale_factor = divide_scaled (72, fixed_pk_resolution,5 + fixed_decimal_digits);
	set_job_id (year, month, day, tex_time, pdftex_version, pdftex_revision);
	if ((pdf_unique_resname > 0) && (pdf_resname_prefix == 0))
	  pdf_resname_prefix = get_resname_prefix();
	/* end expansion of Initialize variables for \.{PDF} output */
	init_pdf_output = true;
  };
  is_shipping_page = shipping_page;
  if (shipping_page) {
	if (term_offset > (unsigned)max_print_line - 9) {
	  print_ln();
	} else if ((term_offset > 0) || (file_offset > 0))
	  print_char (' ');
	print_char ('[');
	j = 9;
	while ((count (j) == 0) && (j > 0))
	  decr (j);
	for (k = 0; k <= j; k++) {
	  print_int (count (k));
	  if (k < j)
		print_char ('.');
	};
	update_terminal;
  };
  if (tracing_output > 0) {
	if (shipping_page)
	  print_char (']');
	begin_diagnostic();
	show_box (p);
	end_diagnostic (true);
  };
  /* begin expansion of (\pdfTeX) Ship box |p| out */
  /* module 724 */
  /* begin expansion of (\pdfTeX) Update the values of |max_h| and 
	 |max_v|; but if the page is too large, |goto done| */
  /* module 732 */
  if ((height (p) > max_dimen) || (depth (p) > max_dimen)
	  || (height (p) + depth (p) + v_offset > max_dimen)
	  || (width (p) + h_offset > max_dimen)) {
	  print_err ("Huge page cannot be shipped out");
	  help2 ("The page just created is more than 18 feet tall or",
		 "more than 18 feet wide, so I suspect something went wrong.");
	  error();
	  if (tracing_output <= 0) {
		begin_diagnostic();
		print_nl_string("The following box has been deleted:");
		show_box (p);
		end_diagnostic (true);
	  };
	  goto DONE;
	};
  if (height (p) + depth (p) + v_offset > max_v)
	max_v = height (p) + depth (p) + v_offset;
  if (width (p) + h_offset > max_h)
	max_h = width (p) + h_offset;
  /* end expansion of (\pdfTeX) Update the values of |max_h|
	 and |max_v|; but if the page is too large, |goto done| */
  /* begin expansion of Initialize variables as |pdf_ship_out| begins */
  /* module 725 */
  temp_ptr = p;
  prepare_mag();
  pdf_last_resources = pdf_new_objnum();
  /* Reset resources lists */
  reset_resource_lists;
  if (!shipping_page) {
	pdf_xform_width = width (p);
	pdf_xform_height = height (p);
	pdf_xform_depth = depth (p);
	pdf_begin_dict (pdf_cur_form);
	pdf_last_stream = pdf_cur_form;
	cur_v = height (p);
	cur_h = 0;
	pdf_origin_h = 0;
	pdf_origin_v = pdf_xform_height + pdf_xform_depth;
  } else {
	/* begin expansion of Calculate page dimensions and margins */
	/* module 728 */
	cur_h_offset = pdf_h_origin + h_offset;
	cur_v_offset = pdf_v_origin + v_offset;
	if (pdf_page_width != 0) {
	  cur_page_width = pdf_page_width;
	} else {
	  cur_page_width = width (p) + 2 * cur_h_offset;
	}
	if (pdf_page_height != 0) {
	  cur_page_height = pdf_page_height;
	} else {
	  cur_page_height = height (p) + depth (p) + 2 * cur_v_offset;
	}
	/* end expansion of Calculate page dimensions and margins */
	pdf_last_page = get_obj (obj_type_page, total_pages + 1, 0);
	obj_aux (pdf_last_page) = 1; /* mark that this page has beed created */
	pdf_new_dict (obj_type_others, 0);
	pdf_last_stream = obj_ptr;
	cur_h = cur_h_offset;
	cur_v = height (p) + cur_v_offset;
	pdf_origin_h = 0;
	pdf_origin_v = cur_page_height;
	/* begin expansion of Reset PDF mark lists */
	/* module 727 */
	pdf_annot_list = null;
	pdf_link_list = null;
	pdf_dest_list = null;
	pdf_bead_list = null;
	last_link = null;
	last_thread = null;
	/* end expansion of Reset PDF mark lists */
  };
  if (!shipping_page) {
	/* begin expansion of Write out Form stream header */
	/* module 729 */
	/* Here we write out the header for Form.
	 */
	pdf_print_ln_string("/Type /XObject");
	pdf_print_ln_string("/Subtype /Form");
	if (obj_xform_attr (pdf_cur_form) != null) {
	  pdf_print_toks_ln (obj_xform_attr (pdf_cur_form));
	  delete_toks (obj_xform_attr (pdf_cur_form));
	};
	pdf_print_string("/BBox [");
	pdf_print_string("0 0 ");
	pdf_print_bp (pdf_xform_width);
	pdf_out (' ');
	pdf_print_bp (pdf_xform_height + pdf_xform_depth);
	pdf_print_ln (']');
	pdf_print_ln_string("/FormType 1");
	pdf_print_ln_string("/Matrix [1 0 0 1 0 0]");
	pdf_indirect_string_ln ("Resources", pdf_last_resources);
	/* end expansion of Write out Form stream header */
  };

  /* begin expansion of Start stream of page/form contents */
  /* module 730 */
  pdf_begin_stream();
  if (shipping_page) {
	/* begin expansion of Adjust tranformation matrix for the
	   magnification ratio */
	/* module 731 */
	if ((mag != 1000) && (mag != 0)) {
	  pdf_print_real (mag, 3);
	  pdf_print_string(" 0 0 ");
	  pdf_print_real (mag, 3);
	  pdf_print_ln_string(" 0 0 cm");
	};
	/* end expansion of Adjust tranformation matrix for the 
	   magnification ratio */
  };
  /* end expansion of Start stream of page/form contents */
  /* end expansion of Initialize variables as |pdf_ship_out| begins */
  if (type (p) == vlist_node) {
	pdf_vlist_out();
  } else
	pdf_hlist_out();
  if (shipping_page)
	incr (total_pages);
  cur_s = -1;
  /* begin expansion of Finish shipping */
  /* module 733 */
  /* begin expansion of Finish stream of page/form contents */
  /* module 734 */
  pdf_end_text();
  pdf_end_stream();
  /* end expansion of Finish stream of page/form contents */
  if (shipping_page) {
	/* begin expansion of Write out page object */
	/* module 742 */
	pdf_begin_dict (pdf_last_page);
	pdf_print_ln_string("/Type /Page");
	pdf_indirect_string_ln ("Contents", pdf_last_stream);
	pdf_indirect_string_ln ("Resources", pdf_last_resources);
	pdf_print_string("/MediaBox [0 0 ");
	pdf_print_mag_bp (cur_page_width);
	pdf_out (' ');
	pdf_print_mag_bp (cur_page_height);
	pdf_print_ln (']');
	if (pdf_page_attr != null)
	  pdf_print_toks_ln (pdf_page_attr);
	/* begin expansion of Generate parent pages object */
	/* module 743 */
	if (total_pages % pages_tree_kids_max == 1) {
	  pdf_create_obj (obj_type_pages, pages_tree_kids_max);
	  pdf_last_pages = obj_ptr;
	};
	pdf_indirect_string_ln ("Parent", pdf_last_pages);
	/* end expansion of Generate parent pages object */
	/* begin expansion of Generate array of annotations or beads in page */
	/* module 744 */
	if ((pdf_annot_list != null) || (pdf_link_list != null)) {
	  pdf_print_string("/Annots [ ");
	  k = pdf_annot_list;
	  while (k != null) {
		pdf_print_int (info (k));
		pdf_print_string(" 0 R ");
		k = link (k);
	  };
	  k = pdf_link_list;
	  while (k != null) {
		pdf_print_int (info (k));
		pdf_print_string(" 0 R ");
		k = link (k);
	  };
	  pdf_print_ln (']');
	};
	if (pdf_bead_list != null) {
	  k = pdf_bead_list;
	  pdf_print_string("/B [ ");
	  while (k != null) {
		pdf_print_int (info (k));
		pdf_print_string(" 0 R ");
		k = link (k);
	  };
	  pdf_print_ln (']');
	};
	/* end expansion of Generate array of annotations or beads in page */
	pdf_end_dict();
	/* end expansion of Write out page object */
  };
  /* begin expansion of Flush out pending raw objects */
  /* module 746 */
  if (pdf_obj_list != null) {
	k = pdf_obj_list;
	while (k != null) {
	  if (!is_obj_written (info (k)))
		pdf_write_obj (info (k));
	  k = link (k);
	};
  };
  /* end expansion of Flush out pending raw objects */
  /* begin expansion of Flush out pending forms */
  /* module 747 */
  /* When flushing pending forms we need to save and restore resources lists
   * 
   * (|pdf_font_list|, |pdf_obj_list|, |pdf_xform_list| and |pdf_ximage_list|),
   * 
   * which are also used by page shipping.
   */
  if (pdf_xform_list != null) {
	k = pdf_xform_list;
	while (k != null) {
	  if (!is_obj_written (info (k))) {
		pdf_cur_form = info (k);
		/* begin expansion of Save resources lists */
		/* module 748 */
		save_font_list = pdf_font_list;
		save_obj_list = pdf_obj_list;
		save_xform_list = pdf_xform_list;
		save_ximage_list = pdf_ximage_list;
		save_text_procset = pdf_text_procset;
		save_image_procset = pdf_image_procset;
		/* end expansion of Save resources lists */
		/* Reset resources lists */
		reset_resource_lists;
		pdf_ship_out (obj_xform_box (pdf_cur_form), false);
		/* begin expansion of Restore resources lists */
		/* module 749 */
		pdf_font_list = save_font_list;
		pdf_obj_list = save_obj_list;
		pdf_xform_list = save_xform_list;
		pdf_ximage_list = save_ximage_list;
		pdf_text_procset = save_text_procset;
		pdf_image_procset = save_image_procset;
		/* end expansion of Restore resources lists */
	  };
	  k = link (k);
	};
  };
  /* end expansion of Flush out pending forms */
  /* begin expansion of Flush out pending images */
  /* module 751 */
  if (pdf_ximage_list != null) {
	k = pdf_ximage_list;
	while (k != null) {
	  if (!is_obj_written (info (k)))
		pdf_write_image (info (k));
	  k = link (k);
	};
  };
  /* end expansion of Flush out pending images */
  if (shipping_page) {
	/* begin expansion of Flush out pending PDF marks */
	/* module 752 */
	pdf_origin_h = 0;
	pdf_origin_v = cur_page_height;
	/* begin expansion of Flush out PDF annotations */
	/* module 753 */
	if (pdf_annot_list != null) {
	  k = pdf_annot_list;
	  while (k != null) {
		i = obj_annot_ptr (info (k)); /* |i| points to |pdf_annot_node| */
		pdf_begin_dict (info (k));
		pdf_print_ln_string("/Type /Annot");
		pdf_print_toks_ln (pdf_annot_data (i));
		pdf_rectangle (pdf_left (i), pdf_top (i),
					   pdf_right (i), pdf_bottom (i));
		pdf_end_dict();
		k = link (k);
	  };
	};
	/* end expansion of Flush out PDF annotations */
	/* begin expansion of Flush out PDF link annotations */
	/* module 754 */
	if (pdf_link_list != null) {
	  /* begin expansion of Write out PDF link annotations */
	  /* module 755 */
	  k = pdf_link_list;
	  while (k != null) {
		i = obj_annot_ptr (info (k));
		pdf_begin_dict (info (k));
		pdf_print_ln_string("/Type /Annot");
		if (pdf_link_attr (i) != null)
		  pdf_print_toks_ln (pdf_link_attr (i));
		pdf_rectangle (pdf_left (i), pdf_top (i),
					   pdf_right (i), pdf_bottom (i));
		if (pdf_action_type (pdf_link_action (i)) !=
			pdf_action_user) {
		  pdf_print_ln_string("/Subtype /Link");
		  pdf_print_string("/A ");
		};
		write_action (pdf_link_action (i));
		pdf_end_dict();
		k = link (k);
	  };
	  /* end expansion of Write out PDF link annotations */
	  /* begin expansion of Free PDF link annotations */
	  /* module 756 */
	  k = pdf_link_list;
	  while (k != null) {
		i = obj_annot_ptr (info (k));
		/* nodes with |info = null| were created by |append_link| and 
		   must be flushed here, as they are not linked in any list */
		if (info (i) == max_halfword) 
		  flush_whatsit_node (i, pdf_start_link_node);
		k = link (k);
	  };
	  /* end expansion of Free PDF link annotations */
	};
	/* end expansion of Flush out PDF link annotations */
	/* begin expansion of Flush out PDF mark destinations */
	/* module 757 */
	if (pdf_dest_list != null) {
	  k = pdf_dest_list;
	  while (k != null) {
		if (is_obj_written (info (k))) {
		 pdf_error_string("ext5","destination has been already written (this shouldn't happen)");
		} else {
		  i = obj_dest_ptr (info (k));
		  if (pdf_dest_named_id (i) > 0) {
			pdf_begin_dict (info (k));
			pdf_print_string("/D ");
		  }
		  else
			pdf_begin_obj (info (k));
		  pdf_out ('[');
		  pdf_print_int (pdf_last_page);
		  pdf_print_string(" 0 R ");
		  switch (pdf_dest_type (i)) {
		  case pdf_dest_xyz:
			{
			  pdf_print_string("/XYZ ");
			  pdf_print_mag_bp (pdf_x (pdf_left (i)));
			  pdf_out (' ');
			  pdf_print_mag_bp (pdf_y (pdf_top (i)));
			  pdf_out (' ');
			  if (pdf_dest_xyz_zoom (i) == null) {
				pdf_print_string("null");
			  } else {
				pdf_print_int (pdf_dest_xyz_zoom (i) / 1000);
				pdf_out ('.');
				pdf_print_int ((pdf_dest_xyz_zoom (i) % 1000));
			  };
			};
			break;
		  case pdf_dest_fit:
			pdf_print_string("/Fit");
			break;
		  case pdf_dest_fith:
			{
			  pdf_print_string("/FitH ");
			  pdf_print_mag_bp (pdf_y (pdf_top (i)));
			};
			break;
		  case pdf_dest_fitv:
			{
			  pdf_print_string("/FitV ");
			  pdf_print_mag_bp (pdf_x (pdf_left (i)));
			};
			break;
		  case pdf_dest_fitb:
			pdf_print_string("/FitB");
			break;
		  case pdf_dest_fitbh:
			{
			  pdf_print_string("/FitBH ");
			  pdf_print_mag_bp (pdf_y (pdf_top (i)));
			};
			break;
		  case pdf_dest_fitbv:
			{
			  pdf_print_string("/FitBV ");
			  pdf_print_mag_bp (pdf_x (pdf_left (i)));
			};
			break;
		  case pdf_dest_fitr:
			{
			  pdf_print_string("/FitR ");
			  pdf_print_rect_spec (i);
			};
			break;
		  default:
			pdf_error_string("ext5","unknown dest type");
			;
		  };
		  pdf_print_ln (']');
		  if (pdf_dest_named_id (i) > 0) {
			pdf_end_dict();
		  } else
			pdf_end_obj();
		};
		k = link (k);
	  };
	};
	/* end expansion of Flush out PDF mark destinations */
	/* begin expansion of Flush out PDF bead rectangle specifications */
	/* module 759 */
	if (pdf_bead_list != null) {
	  k = pdf_bead_list;
	  while (k != null) {
		pdf_new_obj (obj_type_others, 0);
		pdf_out ('[');
		i = obj_bead_data (info (k)); /* pointer to a whasit or whatsit-like node */
		pdf_print_rect_spec (i);
		if (info (i) == max_halfword)	/* not a whatsit node, so must be destroyed here */
		  flush_whatsit_node (i, pdf_start_thread_node);
		pdf_print_ln (']');
		obj_bead_rect (info (k)) = obj_ptr; /* rewrite |obj_bead_data| */ 
		pdf_end_obj();
		k = link (k);
	  };
	};
	/* end expansion of Flush out PDF bead rectangle specifications */
	/* end expansion of Flush out pending PDF marks */
  };
  /* begin expansion of Write out resources dictionary */
  /* module 735 */
  pdf_begin_dict (pdf_last_resources);
  /* begin expansion of Print additional resources */
  /* module 736 */
  if (shipping_page) {
	if (pdf_page_resources != null)
	  pdf_print_toks_ln (pdf_page_resources);
  } else {
	if (obj_xform_resources (pdf_cur_form) != null) {
	  pdf_print_toks_ln (obj_xform_resources (pdf_cur_form));
	  delete_toks (obj_xform_resources (pdf_cur_form));
	};
  };
  /* end expansion of Print additional resources */
  /* begin expansion of Generate font resources */
  /* module 739 */
  if (pdf_font_list != null) {
	pdf_print_string("/Font << ");
	k = pdf_font_list;
	while (k != null) {
	  pdf_print_string("/F");
	  set_ff (info (k));
	  pdf_print_int (ff);
	  pdf_print_font_tag (info (k));
	  pdf_print_resname_prefix;
	  pdf_out (' ');
	  pdf_print_int (pdf_font_num[ff]);
	  pdf_print_string(" 0 R ");
	  k = link (k);
	};
	pdf_print_ln_string(">>");
	pdf_text_procset = true;
  };
  /* end expansion of Generate font resources */
  /* begin expansion of Generate XObject resources */
  /* module 740 */
  if ((pdf_xform_list != null) || (pdf_ximage_list != null)) {
	pdf_print_string("/XObject << ");
	k = pdf_xform_list;
	while (k != null) {
	  pdf_print_string("/Fm");
	  pdf_print_int (obj_info (info (k)));
	  pdf_print_resname_prefix;
	  pdf_out (' ');
	  pdf_print_int (info (k));
	  pdf_print_string(" 0 R ");
	  k = link (k);
	};
	k = pdf_ximage_list;
	while (k != null) {
	  pdf_print_string("/Im");
	  pdf_print_int (obj_info (info (k)));
	  pdf_print_resname_prefix;
	  pdf_out (' ');
	  pdf_print_int (info (k));
	  pdf_print_string(" 0 R ");
	  update_image_procset (obj_ximage_data (info (k)));
	  k = link (k);
	};
	pdf_print_ln_string(">>");
  };
  /* end expansion of Generate XObject resources */
  /* begin expansion of Generate ProcSet */
  /* module 741 */
  pdf_print_string("/ProcSet [ /PDF");
  if (pdf_text_procset)
	pdf_print_string(" /Text");
  if (check_image_b (pdf_image_procset))
	pdf_print_string(" /ImageB");
  if (check_image_c (pdf_image_procset))
	pdf_print_string(" /ImageC");
  if (check_image_i (pdf_image_procset))
	pdf_print_string(" /ImageI");
  pdf_print_ln_string(" ]");
  /* end expansion of Generate ProcSet */
  pdf_end_dict();
  /* end expansion of Write out resources dictionary */
  /* begin expansion of Flush resources lists */
  /* module 737 */
  /* In the end of shipping out a page we reset all the lists holding objects
   * have been created during the page shipping.
   */
  flush_list (pdf_font_list);
  flush_list (pdf_obj_list);
  flush_list (pdf_xform_list);
  flush_list (pdf_ximage_list);
  /* end expansion of Flush resources lists */
  if (shipping_page) {
	/* begin expansion of Flush PDF mark lists */
	/* module 738 */
	flush_list (pdf_annot_list);
	flush_list (pdf_link_list);
	flush_list (pdf_dest_list);
	flush_list (pdf_bead_list);
	/* end expansion of Flush PDF mark lists */
  };
  /* end expansion of Finish shipping */
 DONE:
  /* end expansion of (\pdfTeX) Ship box |p| out */
  if (eTeX_ex)
    /* Check for LR anomalies at the end of |ship_out| */
    check_LR_after_ship_out;
  if ((tracing_output <= 0) && shipping_page)
	print_char (']');
  dead_cycles = 0;
  update_terminal; /* progress report */
  /* begin expansion of (\pdfTeX) Flush the box from memory, showing 
	 statistics if requested */
  /* module 723 */
  if (tracing_stats > 1) {
	print_nl_string("Memory usage before: ");
	print_int (var_used);
	print_char ('&');
	print_int (dyn_used);
	print_char (';');
  };
  flush_node_list (p);
  if (tracing_stats > 1) {
	zprint_string(" after: ");
	print_int (var_used);
	print_char ('&');
	print_int (dyn_used);
	zprint_string("; still untouched: ");
	print_int (hi_mem_min - lo_mem_max - 1);
	print_ln();
  };
  /* end expansion of (\pdfTeX) Flush the box from memory,... */
};


/* module 745 */
void 
pdf_write_obj (int n) {	/* write a raw PDF object */
  str_number s;
  FILE *f;
  s = tokens_to_string (obj_obj_data (n));
  delete_toks (obj_obj_data (n));
  if (obj_obj_is_stream (n) > 0) {
	pdf_begin_dict (n);
	if (obj_obj_stream_attr (n) != null) {
	  pdf_print_toks_ln (obj_obj_stream_attr (n));
	  delete_toks (obj_obj_stream_attr (n));
	};
	pdf_begin_stream();
  } else {
	pdf_begin_obj (n);
  }
  if (obj_obj_is_file (n) > 0) {
	cur_name = s;
	cur_area = null_string;
	cur_ext = null_string;
	pack_cur_name;
	if (!tex_b_openin (f))
	  pdf_error_string("ext5","cannot open file for embedding");
	zprint_string("<<");
	zprint (s);
	while (!feof (f))
	  pdf_out (getc (f));
	zprint_string(">>");
	b_close (f);
  } else if (obj_obj_is_stream (n) > 0) {
	pdf_print (s);
  } else {
	pdf_print_ln (s);
  }
  if (obj_obj_is_stream (n) > 0) {
	pdf_end_stream();
  } else {
	pdf_end_obj();
  }
  flush_str (s);
}


void 
flush_whatsit_node (pointer p, small_number s) {
  type (p) = whatsit_node;
  subtype (p) = s;
  if (link (p) != null)
	pdf_error_string("flush_whatsit_node","link(p) is not null");
  flush_node_list (p);
}

/* module 750 */
void 
pdf_write_image (int n) { /* write an image */
  pdf_begin_dict (n);
  if (obj_ximage_attr (n) != null) {
	pdf_print_toks_ln (obj_ximage_attr (n));
	delete_toks (obj_ximage_attr (n));
  };
  write_image (obj_ximage_data (n));
  delete_image (obj_ximage_data (n));
}

/* module 758 */
void 
pdf_print_rect_spec (pointer r) { /* prints a rect spec */
  pdf_print_mag_bp (pdf_x (pdf_left (r)));
  pdf_out (' ');
  pdf_print_mag_bp (pdf_y (pdf_bottom (r)));
  pdf_out (' ');
  pdf_print_mag_bp (pdf_x (pdf_right (r)));
  pdf_out (' ');
  pdf_print_mag_bp (pdf_y (pdf_top (r)));
}

/* module 764 */

/* Now we are ready to declare our new procedure |ship_out|. It will call
 * |pdf_ship_out| if integer parametr |pdf_output| is positive; otherwise it
 * will call |dvi_ship_out|, which is the \TeX\ original |ship_out|.
 */
void ship_out (pointer p) {	/* output the box |p| */
  if (total_pages == 0) {
	fixed_output = pdf_output;
  } else {
	if (fixed_output != pdf_output)
	  pdf_error_string("setup","\\pdfoutput cannot be changed after shipping out the first page");
  };
  if (pdf_output > 0) {
	pdf_ship_out (p, true);
  } else {
	dvi_ship_out (p);
  }
}

/* module 766 */

/* Finishing the PDF output file.
 * 
 * The following procedures sort the table of destination names
 */
void sort_dest_names (int l, int r) { /* sorts |dest_names| by names */
  int i, j;
  str_number s;
  /* int x, y; */
  dest_name_entry e;
  i = l;
  j = r;
  s = dest_names[(l + r) / 2].objname;
  do {
	while (str_less_str (dest_names[i].objname, s))
	  incr (i);
	while (str_less_str (s, dest_names[j].objname))
	  decr (j);
	if (i <= j) {
	  e = dest_names[i];
	  dest_names[i] = dest_names[j];
	  dest_names[j] = e;
	  incr (i);
	  decr (j);
	};
  } while (!(i > j));
  if (l < j)
	sort_dest_names (l, j);
  if (i < r)
	sort_dest_names (i, r);
}


/* module 768 */

/* Destinations that have been referenced but don't exists have
 * |obj_dest_ptr=null|. Leaving them undefined might cause troubles for
 * PDF browsers, so we need to fix them.
 */
void 
pdf_fix_dest (int k) {
  if (obj_dest_ptr (k) != null)
	return;
  pdf_warning_string ("dest","", false);
  if (obj_info (k) < 0) {
	zprint_string("name{");
	zprint (-obj_info (k));
	zprint ('}');
  } else {
	zprint_string("num");
	print_int (obj_info (k));
  };
  zprint_string (" has been referenced but does not exist, replaced by a fixed one");
  print_ln();
  print_ln();
  pdf_begin_obj (k);
  pdf_out ('[');
  pdf_print_int (head_tab[obj_type_page]);
  pdf_print_ln_string(" 0 R /Fit]");
  pdf_end_obj();
}



/* module 778 */

/* If the same keys in a dictionary are given several times,
 * then it is not defined which value is choosen by an application.
 * Therefore the keys |/Producer| and |/Creator| are only set,
 * if the token list |pdf_info_toks|, converted to a string does
 * not contain these key strings.
 */
void 
pdf_print_info (void) { /* print info object */
  char * s;
  str_number ss;
  boolean creator_given, producer_given, creationdate_given;
  s=""; ss=0; /*TH -Wall*/
  pdf_new_dict (obj_type_others, 0);
  creator_given = false;
  producer_given = false;
  creationdate_given = false;
  if (pdf_info_toks != null) {
	ss = tokens_to_string(pdf_info_toks); 
	s = makecstring(ss);
	creator_given = substr_of_str ("/Creator", s);
	producer_given = substr_of_str ("/Producer", s);
	creationdate_given = substr_of_str ("/CreationDate", s);
  };
  if (!producer_given) {
	/* begin expansion of Print the Producer key */
	/* module 779 */
	pdf_print_string("/Producer (pdfTeX-");
	pdf_print_int (pdftex_version / 100);
	pdf_out ('.');
	pdf_print_int (pdftex_version % 100);
	pdf_print (pdftex_revision);
	pdf_print_ln (')');
	/* end expansion of Print the Producer key */
  };
  if (pdf_info_toks != null) {
	if (strlen (s) > 0) {
	  pdf_print_string (s);
	  pdf_print_nl;
	};
	flush_str (ss);
	free(s);
	delete_toks (pdf_info_toks);
  };
  if (!creator_given)
	pdf_str_entry_ln ("Creator","TeX");
  if (!creationdate_given) {
	/* begin expansion of Print the CreationDate key */
	/* module 780 */
	print_creation_date();
	/* end expansion of Print the CreationDate key */
  };
  pdf_end_dict();
}

/* module 781 */
str_number pdftex_banner; /* the complete banner */

/* test whether a char in font is marked */
#define pdf_char_marked(f, c) (pdf_char_used[f][c/8] & (1<<(c%8)))

/* module 767 */
/* Now the finish of PDF output file. At this moment all Page object
 * are already written completly to PDF output file.
 */
void
finish_pdf_file (void) {
  int a, b, c, i, j, l; /* all-purpose index */ 
  int k;
  boolean is_root; /* |pdf_last_pages| is root of Pages tree? */ 
  int	root, outlines, threads, names_tree, dests; /* , fixed_dest*/
  if (total_pages == 0) {
	print_nl_string("No pages of output.");
  } else {
	pdf_flush(); /* to make sure that the output file name has been already created */
	if (total_pages % pages_tree_kids_max != 0)
	  obj_info (pdf_last_pages) = total_pages % pages_tree_kids_max;
	/* last pages object may have less than |pages_tree_kids_max| chilrend */
	/* begin expansion of Check for non-existing pages */
	/* module 770 */
	k = head_tab[obj_type_page];
	while (obj_aux (k) == 0) {
	  pdf_warning_string ("dest","Page ", false);
	  print_int (obj_info (k));
	  zprint_string (" has been referenced but does not exist!");
	  print_ln();
	  print_ln();
	  k = obj_link (k);
	};
	head_tab[obj_type_page] = k;
	/* end expansion of Check for non-existing pages */
	/* begin expansion of Reverse the linked list of Page and Pages objects */
	/* module 771 */
	k = head_tab[obj_type_page];
	l = 0;
	do {
	  i = obj_link (k);
	  obj_link (k) = l;
	  l = k;
	  k = i;
	} while (k != 0);
	head_tab[obj_type_page] = l;
	k = head_tab[obj_type_pages];
	l = 0;
	do {
	  i = obj_link (k);
	  obj_link (k) = l;
	  l = k;
	  k = i;
	} while (k != 0);
	head_tab[obj_type_pages] = l;
	/* end expansion of Reverse the linked list of Page and Pages objects */
	/* begin expansion of Check for non-existing destinations */
	/* module 769 */
	k = head_tab[obj_type_dest];
	while (k != 0) {
	  pdf_fix_dest (k);
	  k = obj_link (k);
	};
	/* end expansion of Check for non-existing destinations */
	/* begin expansion of Output fonts definition */
	/* module 772 */
	for (k = font_base + 1; k <= font_ptr; k++)
	  if ((font_used[k]) && (pdf_font_map[k] >= 0)) {
		if ((pdf_font_num[k] < 0)) {
		  i = -pdf_font_num[k];
		} else {
		  i = tfm_of_fm (pdf_font_map[k]);
		}
		if (i != k)
		  for (j = 0; j <= 255; j++)
			if ((boolean)pdf_char_marked (k, j))
			  pdf_mark_char (i, j);
	  };
	k = head_tab[obj_type_font];
	while (k != 0) {
	  f = obj_info (k);
	  do_pdf_font (k, f);
	  k = obj_link (k);
	};
	/* end expansion of Output fonts definition */
	/* begin expansion of Output pages tree */
	/* module 773 */
	/* We will generate in each single step the parents of all Pages/Page objects in
	 * the previous level. These new generated Pages object will create a new level of
	 * Pages tree. We will repeat this until search only one Pages object. This one
	 * will be the Root object.
	 */
	a = obj_ptr + 1;		/* all Pages object whose childrend are not Page objects
							   should have index greater than |a| */ 
	l = head_tab[obj_type_pages]; /* |l| is the index of current Pages object which is being output */ 
	k = head_tab[obj_type_page]; /* |k| is the index of current child of |l| */
	if (obj_link (l) == 0) {
	  is_root = true;	/* only Pages object; total pages is not greater than |pages_tree_kids_max| */
	} else {
	  is_root = false;
	}
	b = obj_ptr + 1; /* to init |c| in next step */
	do {
	  i = 0; /* counter of Pages object in current level */
	  c = b; /* first Pages object in previous level */
	  b = obj_ptr + 1; /* succcesor of last created object */
	  do {
		if (!is_root) {
		  if (i % pages_tree_kids_max == 0)  {	/* create a new Pages object for next level */
			pdf_last_pages = pdf_new_objnum();
			obj_info (pdf_last_pages) = obj_info (l);
		  } else {
			obj_info (pdf_last_pages) =obj_info (pdf_last_pages)+ obj_info (l);
		  };
		};
		/* begin expansion of Output the current Pages object in this level */
		/* module 774 */
		pdf_begin_dict (l);
		pdf_print_ln_string("/Type /Pages");
		pdf_int_entry_ln ("Count", obj_info (l));
		if (!is_root)
		  pdf_indirect_string_ln ("Parent", pdf_last_pages);
		pdf_print_string("/Kids [");
		j = 0;
		do {
		  pdf_print_int (k);
		  pdf_print_string(" 0 R ");
		  if (k < a)	{/* the next Pages/Page object must be |obj_link(k)| */
			k = obj_link (k);
		  } else {	/* |k >= a|, the next Pages object is |k + 1| */
			incr (k);
		  }
		  incr (j);
		} while (!(((l < a) && (j == obj_info (l))) 
				   || (k == 0) || (k == c) || (j == pages_tree_kids_max)));
		remove_last_space();
		pdf_print_ln (']');
		if (k == 0) {
		  if (head_tab[obj_type_pages] != 0) {	/* we are in Page objects list */
			k = head_tab[obj_type_pages];
			head_tab[obj_type_pages] = 0;
		  } else { /* we are in Pages objects list */
			k = a;
		  }
		};
		if (is_root && (pdf_pages_attr != null))
		  pdf_print_toks_ln (pdf_pages_attr);
		pdf_end_dict();
		if (is_root)
		  goto DONE;
		/* end expansion of Output the current Pages object in this level */
		incr (i);
		if (l < a) {
		  l = obj_link (l);
		} else {
		  incr (l);
		}
	  } while (!((l == 0) || (l == b)));
	  if (l == 0)
		l = a;
	  if (b == obj_ptr)
		is_root = true;
	} while (!(false));
  DONE:
	/* end expansion of Output pages tree */
	/* begin expansion of Output outlines */
	/* module 761 */
	/* In the end we must flush PDF objects that cannot be written out
	 * immediately after shipping out pages.
	 */
	if (pdf_first_outline != 0) {
	  pdf_new_dict (obj_type_others, 0);
	  outlines = obj_ptr;
	  l = pdf_first_outline;
	  k = 0;
	  do {
		incr (k);
		a = open_subentries (l);
		if (obj_outline_count (l) > 0)
		  k = k + a;
		obj_outline_parent (l) = obj_ptr;
		l = obj_outline_next (l);
	  } while (l != 0);
	  pdf_print_ln_string("/Type /Outlines");
	  pdf_indirect_string_ln ("First", pdf_first_outline);
	  pdf_indirect_string_ln ("Last", pdf_last_outline);
	  pdf_int_entry_ln ("Count", k);
	  pdf_end_dict();
	  /* begin expansion of Output PDF outline entries */
	  /* module 762 */
	  k = head_tab[obj_type_outline];
	  while (k != 0) {
		if (obj_outline_parent (k) == pdf_parent_outline) {
		  if (obj_outline_prev (k) == 0)
			pdf_first_outline = k;
		  if (obj_outline_next (k) == 0)
			pdf_last_outline = k;
		};
		pdf_begin_dict (k);
		pdf_indirect_string_ln ("Title",obj_outline_title (k));
		pdf_indirect_string_ln ("A",obj_outline_action_objnum(k));
		if (obj_outline_parent (k) != 0)
		  pdf_indirect_string_ln ("Parent", obj_outline_parent(k));
		if (obj_outline_prev (k) != 0)
		  pdf_indirect_string_ln ("Prev", obj_outline_prev(k));
		if (obj_outline_next (k) != 0)
		  pdf_indirect_string_ln ("Next",obj_outline_next(k));
		if (obj_outline_first (k) != 0)
		  pdf_indirect_string_ln ("First",obj_outline_first(k));
		if (obj_outline_last (k) != 0)
		  pdf_indirect_string_ln ("Last",obj_outline_last (k));
		if (obj_outline_count (k) != 0)
		  pdf_int_entry_ln ("Count",obj_outline_count(k));
		if (obj_outline_attr (k) != 0) {
		  pdf_print_toks_ln (obj_outline_attr (k));
		  delete_toks (obj_outline_attr (k));
		};
		pdf_end_dict();
		k = obj_link (k);
	  };
	  /* end expansion of Output PDF outline entries */
	} else {
	  outlines = 0;
	}
	/* end expansion of Output outlines */
	/* begin expansion of Output name tree */
	/* module 775 */
	/* The name tree is very similiar to Pages tree so its construction should be
	 * certain from Pages tree construction. For intermediate node |obj_info| will be
	 * the first name and |obj_link| will be the last name in \.{\\Limits} array.
	 * Note that |pdf_dest_names_ptr| will be less than |obj_ptr|, so we test if
	 * |k < pdf_dest_names_ptr| then |k| is index of leaf in |dest_names|; else
	 * |k| will be index in |obj_tab| of some intermediate node.
	 */		
	if (pdf_dest_names_ptr == 0) {
	  dests = 0;
	  goto DONE1;
	};
	sort_dest_names (0, pdf_dest_names_ptr - 1);
	a = obj_ptr + 1; /* first intermediate node of name tree */
	l = a; /* index of node being output */ 
	k = 0; /* index of current child of |l|; if |k < pdf_dest_names_ptr| then this is
			  pointer to |dest_names| array; otherwise it is the pointer to |obj_tab|
			  (object number)  */
	do {
	  c = obj_ptr + 1; /* first node in current level */
	  do {
		pdf_create_obj (obj_type_others, 0); /* create a new node for next level */
		/* begin expansion of Output the current node in this level */
		/* module 776 */
		pdf_begin_dict (l);
		j = 0;
		if (k < pdf_dest_names_ptr) {
		  obj_info (l) = dest_names[k].objname;
		  pdf_print_string("/Names [");
		  do {
			pdf_print_str (dest_names [k].objname);
			pdf_out (' ');
			pdf_print_int (dest_names [k].objnum);
			pdf_print_string(" 0 R ");
			incr (j);
			incr (k);
		  } while (! ((j == name_tree_kids_max) || (k == pdf_dest_names_ptr)));
		  remove_last_space();
		  pdf_print_ln (']');
		  obj_link (l) =dest_names[k - 1].objname;
		  if (k == pdf_dest_names_ptr)
			k = a;
		} else {
		  obj_info (l) = obj_info (k);
		  pdf_print_string("/Kids [");
		  do {
			pdf_print_int (k);
			pdf_print_string(" 0 R ");
			incr (j);
			incr (k);
		  } while (!((j = name_tree_kids_max) || (k == c)));
		  remove_last_space();
		  pdf_print_ln (']');
		  obj_link (l) = obj_link (k - 1);
		};
		if ((l > k) || (l == a)) {
		  pdf_print_string("/Limits [");
		  pdf_print_str (obj_info (l));
		  pdf_out (' ');
		  pdf_print_str (obj_link (l));
		  pdf_print_ln (']');
		  pdf_end_dict();
		} else {
		  pdf_end_dict();
		  dests = l;
		  goto DONE1;
		};
		/* end expansion of Output the current node in this level */
		incr (l);
		incr (i);
	  } while (k != c);
	} while (!(false));
  DONE1:
	if ((dests != 0) || (pdf_names_toks !=null)){
	  pdf_new_dict (obj_type_others, 0);
	  if ((dests != 0))
		pdf_indirect_string_ln ("Dests", dests);
	  if (pdf_names_toks != null){
		pdf_print_toks_ln (pdf_names_toks);
		delete_toks (pdf_names_toks);
	  };
	  pdf_end_dict();
	  names_tree = obj_ptr;
	} else {
	  names_tree = 0;
	}
	/* end expansion of Output name tree */
	/* begin expansion of Output article threads */
	/* module 763 */
	if (head_tab[obj_type_thread] != 0) {
	  pdf_new_obj (obj_type_others, 0);
	  threads = obj_ptr;
	  pdf_out ('[');
	  k = head_tab[obj_type_thread];
	  while (k != 0) {
		pdf_print_int (k);
		pdf_print_string(" 0 R ");
		k = obj_link (k);
	  };
	  remove_last_space();
	  pdf_print_ln (']');
	  pdf_end_obj();
	  k = head_tab[obj_type_thread];
	  while (k != 0) {
		out_thread (k);
		k = obj_link (k);
	  };
	} else {
	  threads = 0;
	}
	/* end expansion of Output article threads */
	/* begin expansion of Output the catalog object */
	/* module 777 */
	pdf_new_dict (obj_type_others, 0);
	root = obj_ptr;
	pdf_print_ln_string("/Type /Catalog");
	pdf_indirect_string_ln ("Pages", pdf_last_pages);
	if (threads != 0)
	  pdf_indirect_string_ln ("Threads", threads);
	if (outlines != 0)
	  pdf_indirect_string_ln ("Outlines", outlines);
	if (names_tree != 0)
	  pdf_indirect_string_ln ("Names", names_tree);
	if (pdf_catalog_toks != null) {
	  pdf_print_toks_ln (pdf_catalog_toks);
	  delete_toks (pdf_catalog_toks);
	};
	if (pdf_catalog_openaction != 0)
	  pdf_indirect_string_ln ("OpenAction",pdf_catalog_openaction);
	pdf_str_entry ("PTEX.Fullbanner",pdftex_banner);
	if (pdftex_banner!=0)
	  pdf_print_nl;
	pdf_end_dict();
	/* end expansion of Output the catalog object */
	pdf_print_info();
	/* begin expansion of Output the |obj_tab| */
	/* module 782 */
	l = 0;
	for (k = 1; k <= obj_ptr; k++)
	  if (obj_offset (k) == 0) {
		obj_link (l) = k;
		l = k;
	  };
	obj_link (l) = 0;
	pdf_save_offset = pdf_offset;
	pdf_print_ln_string("xref");
	pdf_print_string("0 ");
	pdf_print_int_ln (obj_ptr + 1);
	pdf_print_fw_int (obj_link (0), 10);
	pdf_print_ln_string(" 65535 f ");
	for (k = 1; k <= obj_ptr; k++) {
	  if (obj_offset (k) == 0) {
		pdf_print_fw_int (obj_link (k), 10);
		pdf_print_ln_string(" 00000 f ");
	  } else {
		pdf_print_fw_int (obj_offset (k), 10);
		pdf_print_ln_string(" 00000 n ");
	  };
	};
	/* end expansion of Output the |obj_tab| */
	/* begin expansion of Output the trailer */
	/* module 783 */
	pdf_print_ln_string("trailer");
	pdf_print_ln_string("<<");
	pdf_int_entry_ln ("Size", obj_ptr + 1);
	pdf_indirect_string_ln ("Root", root);
	pdf_indirect_string_ln ("Info", obj_ptr);
	if (pdf_trailer_toks != null) {
	  pdf_print_toks_ln (pdf_trailer_toks);
	  delete_toks (pdf_trailer_toks);
	};
	printID (output_file_name);
	pdf_print_ln_string(">>");
	pdf_print_ln_string("startxref");
	pdf_print_int_ln (pdf_save_offset);
	pdf_print_ln_string ("%%EOF");
	/* end expansion of Output the trailer */		  
	pdf_flush();
	print_nl_string("Output written on ");
	slow_print (output_file_name);
	zprint_string(" (");
	print_int (total_pages);
	zprint_string(" page");
	if (total_pages != 1)
	  print_char ('s');
	zprint_string(", ");
	print_int (pdf_offset);
	zprint_string(" bytes).");
	libpdffinish();
	b_close (pdf_file);
  };
}
