
#include "types.h"
#include "c-compat.h"
#include "globals.h"

/* module 173 */

/* 
 * We can reinforce our knowledge of the data structures just introduced
 * by considering two procedures that display a list in symbolic form.
 * The first of these, called |short_display|, is used in ``overfull box''
 * messages to give the top-level description of a list. The other one,
 * called |show_node_list|, prints a detailed description of exactly what
 * is in the data structure.
 * 
 * The philosophy of |short_display| is to ignore the fine points about exactly
 * what is inside boxes, except that ligatures and discretionary breaks are
 * expanded. As a result, |short_display| is a recursive procedure, but the
 * recursion is never more than one level deep.
 * 
 * A global variable |font_in_short_display| keeps track of the font code that
 * is assumed to be present when |short_display| begins; deviations from this
 * font will be printed.
 */

integer font_in_short_display; /* an internal font number */

/* module 174 */

/* Boxes, rules, inserts, whatsits, marks, and things in general that are
 * sort of ``complicated'' are indicated only by printing `\.{[]}'.
 */
void 
short_display (pointer p) { /* prints highlights of list |p| */
  while (p > mem_min) {
    if (is_char_node (p)) {
      if (p <= mem_end) {
		if (font (p) != font_in_short_display) {
		  if ((font (p) > font_max)) {
			print_char ('*');
		  } else {
			/* Print the font identifier for |font(p)| */
			print_font_ident(p);
		  }
		  font_in_short_display = font (p);
		};
		print_ASCII (qo (character (p)));
      };
    } else {
      /* Print a short indication of the contents of node |p| */
      p = print_short_node(p);
    };
    p = link (p);
  };
};

/* module 175 */
/* Print a short indication of the contents of node |p| */
pointer
print_short_node (pointer p) {
  int n;			/* for replacement counts */
  switch (type (p)) {
  case hlist_node:
  case vlist_node:
  case ins_node:
  case whatsit_node:
  case mark_node:
  case adjust_node:
  case unset_node:
	zprint_string ("[]");
	break;
  case rule_node:
	print_char ('|');
	break;
  case glue_node:
	if (glue_ptr (p) != zero_glue)
	  print_char (' ');
	break;
  case math_node:
	if (subtype (p) >= L_code) {
	  zprint_string ("[]");
	} else {
	  print_char ('$');
	};
	break;
  case ligature_node:
	short_display (lig_ptr (p));
	break;
  case disc_node:
	short_display (pre_break (p));
	short_display (post_break (p));
	n = replace_count (p);
	while (n > 0) {
	  if (link (p) != null) {
		p = link (p);
	  }
	  decr (n);
	};
	do_something;
	break;
  default:
	do_nothing;
  };
  return p;
}

/* module 1550 */
/*  Display <rule spec> for whatsit node created by pdf\TeX */
#define display_rule_spec zprint ('(');         \
            print_rule_dimen (pdf_height (p));  \
		    print_char ('+');                   \
		    print_rule_dimen (pdf_depth (p));   \
		    zprint_string(")x");                \
		    print_rule_dimen (pdf_width (p))

/* module 182 */

/* Now we are ready for |show_node_list| itself. This procedure has been
 * written to be ``extra robust'' in the sense that it should not crash or get
 * into a loop even if the data structures have been messed up by bugs in
 * the rest of the program. You can safely call its parent routine
 * |show_box(p)| for arbitrary values of |p| when you are debugging \TeX.
 * However, in the presence of bad data, the procedure may
 * 
 * fetch a |memory_word| whose variant is different from the way it was stored;
 * for example, it might try to read |mem[p].hh| when |mem[p]|
 * contains a scaled integer, if |p| is a pointer that has been
 * clobbered or chosen at random.
 */
void
show_node_list (int p) { /* prints a node list symbolically */
  int n;			/* the number of items already printed at this level */
  real g;			/* a glue ratio, as a floating point number */
  if (cur_length > depth_threshold) {
	if (p > null)
	  zprint_string (" []"); /* indicate that there's been some truncation */ 
	return;
  };
  n = 0;
  while (p > mem_min) {
	print_ln();
	print_current_string();	/* display the nesting history */
	if (p > mem_end) {	/* pointer out of range */
	  zprint_string ("Bad link, display aborted.");
	  return;
	};
	incr (n);
	if (n > breadth_max) { /* time to stop */
	  zprint_string("etc.");
	  return;
	};
	/* begin expansion of Display node |p| */
	/* module 183 */
	if (is_char_node (p)) {
	  print_font_and_char (p);
	} else {
	  switch (type (p)) {
	  case hlist_node:
	  case vlist_node:
	  case unset_node:
		/* begin expansion of Display box |p| */
		/* module 184 */
		{
		  if (type (p) == hlist_node) {
		    print_esc_string (( "h" ));
		  } else if (type (p) == vlist_node) {
			print_esc_string (("v"));
		  } else {
			print_esc_string ("unset");
		  };
		  zprint_string("box(");
		  print_scaled (height (p));
		  print_char ('+');
		  print_scaled (depth (p));
		  zprint_string(")x");
		  print_scaled (width (p));
		  if (type (p) == unset_node) {
		    /* begin expansion of Display special fields of the unset node |p| */
		    /* module 185 */
		    if (span_count (p) != min_quarterword) {
			  zprint_string(" (");
			  print_int (qo (span_count (p)) + 1);
			  zprint_string(" columns)");
			};
		    if (glue_stretch (p) != 0) {
			  zprint_string(", stretch ");
			  print_glue (glue_stretch (p), glue_order (p), "");
			};
		    if (glue_shrink (p) != 0) {
			  zprint_string(", shrink ");
			  print_glue (glue_shrink (p), glue_sign (p), "");
			};
		    /* end expansion of Display special fields of the unset node |p| */
		  } else {
		    /* begin expansion of Display the value of |glue_set(p)| */
			/* module 186 */
			/* The code will have to change in this place if |glue_ratio| is
			 * a structured type instead of an ordinary |real|. Note that this routine
			 * should avoid arithmetic errors even if the |glue_set| field holds an
			 * arbitrary random value. The following code assumes that a properly
			 * formed nonzero |real| number has absolute value $2^{20}$ or more when
			 * it is regarded as an integer; this precaution was adequate to prevent
			 * floating point underflow on the author's computer.
			 */
		    g = zfloat (glue_set (p));
		    if ((g != 0.0) && (glue_sign (p) != normal)) {
			  zprint_string(", glue set ");
			  if (glue_sign (p) == shrinking)
				zprint_string("- ");
			  /*  The Unix |pc| folks removed this restriction with a remark 
			      that invalid bit patterns were vanishingly improbable, so we follow 
			      their example without really understanding it. 
			      |if abs(mem[p+glue_offset].cint)< oct(4000000) then zprint('?.?')| |else|  */
			  if (z_fabs (g) > 20000.0) {
			    if (g > 0.0) {
				  print_char ('>');
				} else {
				  zprint_string("< -");
				};
			    print_glue (20000 * unity, glue_order (p), "");
			  } else {
			    //print_glue (unity * g, glue_order (p), "");
			    print_glue (round(unity * g), glue_order (p), "");
			  }
			  /* end expansion of Display the value of |glue_set(p)| */
			};
		    if (shift_amount (p) != 0) {
			  zprint_string(", shifted ");
			  print_scaled (shift_amount (p));
			};
		    if (eTeX_ex) {
		      /* begin expansion of Display if this box is never to be reversed */
			  /* module 1650 */
			  /* An hbox with subtype dlist will never be reversed, even when embedded
			   * in right-to-left text.
			   */
		      if ((type (p) == hlist_node) && (subtype (p) == dlist))
				zprint_string(", display");
			  /* end expansion of Display if this box is never to be reversed */
			};
		  };
		  node_list_display (list_ptr (p));	/* recursive call */
		};
		/* end expansion of Display box |p| */
		break;
	  case rule_node:
		/* begin expansion of Display rule |p| */
		/* module 187 */
		print_esc_string ("rule(");
		print_rule_dimen (height (p));
		print_char ('+');
		print_rule_dimen (depth (p));
		zprint_string(")x");
		print_rule_dimen (width (p));
		/* end expansion of Display rule |p| */
		break;
	  case ins_node:
		/* begin expansion of Display insertion |p| */
		/* module 188 */
		print_esc_string ("insert");
		print_int (qo (subtype (p)));
		zprint_string(", natural size ");
		print_scaled (height (p));
		zprint_string("; split(");
		print_spec (split_top_ptr (p), "");
		print_char (',');
		print_scaled (depth (p));
		zprint_string("); float cost ");
		print_int (float_cost (p));
		node_list_display (ins_ptr (p));	/* recursive call */
		/* end expansion of Display insertion |p| */
		break;
	  case whatsit_node:
		/* begin expansion of Display the whatsit node |p| */
		/* module 1552 */
		switch (subtype (p)) {
		case open_node:
		  print_write_whatsit ("openout", p);
		  print_char ('=');
		  print_file_name (open_name (p), open_area (p), open_ext (p));
		  break;
		case write_node:
		  print_write_whatsit ("write", p);
		  print_mark (write_tokens (p));
		  break;
		case close_node:
		  print_write_whatsit ("closeout", p);
		  break;
		case special_node:
		  print_esc_string ("special");
		  print_mark (write_tokens (p));
		  break;
		case language_node:
		  print_esc_string ("setlanguage");
		  print_int (what_lang (p));
		  zprint_string(" (hyphenmin ");
		  print_int (what_lhm (p));
		  print_char (',');
		  print_int (what_rhm (p));
		  print_char (')');
		  break;
		case pdf_literal_node:
		  print_esc_string ("pdfliteral");
		  if (pdf_literal_direct (p) > 0)
			zprint_string(" direct");
		  print_mark (pdf_literal_data (p));
		  break;
		case pdf_refobj_node:
		  print_esc_string ("pdfrefobj");
		  if (obj_obj_is_stream (pdf_obj_objnum (p)) > 0) {
			if (obj_obj_stream_attr (pdf_obj_objnum (p)) != null) {
			  zprint_string(" attr");
			  print_mark (obj_obj_stream_attr(pdf_obj_objnum (p)));
			};
			zprint_string(" stream");
		  };
		  if (obj_obj_is_file (pdf_obj_objnum (p)) > 0)
			zprint_string(" file");
		  print_mark (obj_obj_data (pdf_obj_objnum (p)));
		  break;
		case pdf_refxform_node:
		  print_esc_string ("pdfrefxform");
		  zprint ('(');
		  print_scaled (obj_xform_height (pdf_xform_objnum (p)));
		  print_char ('+');
		  print_scaled (obj_xform_depth (pdf_xform_objnum (p)));
		  zprint_string(")x");
		  print_scaled (obj_xform_width (pdf_xform_objnum (p)));
		  break;
		case pdf_refximage_node:
		  print_esc_string ("pdfrefximage");
		  zprint ('(');
		  print_scaled (obj_ximage_height (pdf_ximage_objnum (p)));
		  print_char ('+');
		  print_scaled (obj_ximage_depth (pdf_ximage_objnum (p)));
		  zprint_string(")x");
		  print_scaled (obj_ximage_width (pdf_ximage_objnum (p)));
		  break;
		case pdf_annot_node:
		  print_esc_string ("pdfannot");
		  /* Display <rule spec> for whatsit node created by pdf\TeX */
		  display_rule_spec;
		  print_mark (pdf_annot_data (p));
		  break;
		case pdf_start_link_node:
		  print_esc_string ("pdflink");
		  /* Display <rule spec> for whatsit node created by pdf\TeX */
		  display_rule_spec;
		  if (pdf_link_attr (p) != null) {
			zprint_string(" attr");
			print_mark (pdf_link_attr (p));
		  };
		  zprint_string(" action");
		  if (pdf_action_type (pdf_link_action (p)) == pdf_action_user) {
			zprint_string(" user");
			print_mark (pdf_action_user_tokens (pdf_link_action (p)));
			return;
		  };
		  if (pdf_action_file (pdf_link_action (p)) != null) {
			zprint_string(" file");
			print_mark (pdf_action_file (pdf_link_action (p)));
		  };
		  switch (pdf_action_type (pdf_link_action (p))) {
		  case pdf_action_goto:
			if (pdf_action_named_id (pdf_link_action (p)) > 0) {
			  zprint_string(" goto name");
			  print_mark (pdf_action_id (pdf_link_action (p)));
			} else {
			  zprint_string(" goto num");
			  print_int (pdf_action_id (pdf_link_action (p)));
			};
			break;
		  case pdf_action_page:
			zprint_string(" page");
			print_int (pdf_action_id (pdf_link_action (p)));
			print_mark (pdf_action_page_tokens (pdf_link_action (p)));
			break;
		  case pdf_action_thread:
			if (pdf_action_named_id (pdf_link_action (p)) > 0) {
			  zprint_string(" thread name");
			  print_mark (pdf_action_id(pdf_link_action (p)));
			} else {
			  zprint_string(" thread num");
			  print_int (pdf_action_id (pdf_link_action (p)));
			};
			break;
		  default:
			pdf_error_string("displaying","unknown action type");
		  };
		  break;
		case pdf_end_link_node:
		  print_esc_string ("pdfendlink");
		  break;
		case pdf_dest_node:
		  print_esc_string ("pdfdest");
		  if (pdf_dest_named_id (p) > 0) {
			zprint_string(" name");
			print_mark (pdf_dest_id (p));
		  } else {
			zprint_string(" num");
			print_int (pdf_dest_id (p));
		  };
		  zprint (' ');
		  switch (pdf_dest_type (p)) {
		  case pdf_dest_xyz:
			zprint_string("xyz");
			if (pdf_dest_xyz_zoom (p) != null) {
			  zprint_string(" zoom");
			  print_int (pdf_dest_xyz_zoom (p));
			};
			break;
		  case pdf_dest_fitbh:
			zprint_string("fitbh");
			break;
		  case pdf_dest_fitbv:
			zprint_string("fitbv");
			break;
		  case pdf_dest_fitb:
			zprint_string("fitb");
			break;
		  case pdf_dest_fith:
			zprint_string("fith");
			break;
		  case pdf_dest_fitv:
			zprint_string("fitv");
			break;
		  case pdf_dest_fitr:
			zprint_string("fitr");
			/* Display <rule spec> for whatsit node created by pdf\TeX */
			display_rule_spec;
			break;
		  case pdf_dest_fit:
			zprint_string("fit");
			break;
		  default:
			zprint_string("unknown!");
		  };
		  break;
		case pdf_thread_node:
		case pdf_start_thread_node:
		  if (subtype (p) == pdf_thread_node) {
			print_esc_string ("pdfthread");
		  } else {
			print_esc_string ("pdfstartthread");
		  }
		  zprint ('(');
		  print_rule_dimen (pdf_height (p));
		  print_char ('+');
		  print_rule_dimen (pdf_depth (p));
		  zprint_string(")x");
		  print_rule_dimen (pdf_width (p));
		  if (pdf_thread_attr (p) != null) {
			zprint_string(" attr");
			print_mark (pdf_thread_attr (p));
		  };
		  if (pdf_thread_named_id (p) > 0) {
			zprint_string(" name");
			print_mark (pdf_thread_id (p));
		  } else {
			zprint_string(" num");
			print_int (pdf_thread_id (p));
		  };
		  break;
		case pdf_end_thread_node:
		  print_esc_string ("pdfendthread");
		  break;
		case pdf_save_pos_node:
		  print_esc_string ("pdfsavepos");
		  break;
		case pdf_snap_ref_point_node:
		  print_esc_string ("pdfsnaprefpoint");
		  break;
		case pdf_snap_x_node:
		  print_esc_string ("pdfsnapx");
		  print_char (' ');
		  print_spec (snap_glue_ptr (p), "");
		  break;
		case pdf_snap_y_node:
		  print_esc_string ("pdfsnapy");
		  print_char (' ');
		  print_spec (snap_glue_ptr (p), "");
		  break;
		default:
		  zprint_string("whatsit?");
		};
		/* end expansion of Display the whatsit node |p| */
		break;
	  case glue_node:
		/* begin expansion of Display glue |p| */
		/* module 189 */
		if (subtype (p) >= a_leaders) {
		  /* begin expansion of Display leaders |p| */
		  /* module 190 */
		  print_esc_string ("");
		  if (subtype (p) == c_leaders) {
			print_char ('c');
		  } else { if (subtype (p) == x_leaders)
			print_char ('x');
		  }
		  zprint_string("leaders ");
		  print_spec (glue_ptr (p), "");
		  node_list_display (leader_ptr (p));	/* recursive call */
		  /* end expansion of Display leaders |p| */
		} else {
		  print_esc_string ("glue");
		  if (subtype (p) != normal) {
			print_char ('(');
			if (subtype (p) < cond_math_glue) {
			  print_skip_param (subtype (p) - 1);
			} else {
			  if (subtype (p) == cond_math_glue) {
				print_esc_string ("nonscript");
			  } else {
				print_esc_string ("mskip");
			  }
			};
			print_char (')');
		  };
		  if (subtype (p) != cond_math_glue) {
			print_char (' ');
			if (subtype (p) < cond_math_glue) {
			  print_spec (glue_ptr (p), "");
			} else {
			  print_spec (glue_ptr (p),"mu");
			};
		  };
		};
		/* end expansion of Display glue |p| */
		break;
	  case margin_kern_node:
		print_esc_string ("kern");
		print_scaled (width (p));
		if (subtype (p) == left_side) {
		  zprint_string(" (left margin)");
		} else {
		  zprint_string(" (right margin)");
		};
		break;
	  case kern_node:
		/* begin expansion of Display kern |p| */
		/* module 191 */
		/* An ``explicit'' kern value is indicated implicitly by an explicit space. */
		if (subtype (p) != mu_glue) {
		  print_esc_string ("kern");
		  if (subtype (p) != normal)
		    print_char (' ');
		  print_scaled (width (p));
		  if (subtype (p) == acc_kern)
		    zprint_string(" (for accent)");
		} else {
		  print_esc_string ("mkern");
		  print_scaled (width (p));
		  zprint_string("mu");
		};
		/* end expansion of Display kern |p| */
		break;
	  case math_node:
		/* begin expansion of Display math node |p| */
		/* module 192 */
		if (subtype (p) > after) {
		  if (end_LR (p)) {
			print_esc_string ("end");
		  } else {
			print_esc_string ("begin");
		  };
		  if (subtype (p) > R_code) {
			print_char ('R');
		  } else if (subtype (p) > L_code)   {
			print_char ('L');
		  } else {
			print_char ('M');
		  };
		} else {
		  print_esc_string ("math");
		  if (subtype (p) == before) {
			zprint_string("on");
		  } else {
			zprint_string("off");
		  };
		  if (width (p) != 0) {
			zprint_string(", surrounded ");
			print_scaled (width (p));
		  };
		};
		/* end expansion of Display math node |p| */
		break;
	  case ligature_node:
		/* begin expansion of Display ligature |p| */
		/* module 193 */
		print_font_and_char (lig_char (p));
		zprint_string(" (ligature ");
		if (subtype (p) > 1)
		  print_char ('|');
		font_in_short_display = font (lig_char (p));
		short_display (lig_ptr (p));
		if (odd (subtype (p)))
		  print_char ('|');
		print_char (')');
		/* end expansion of Display ligature |p| */
		break;
	  case penalty_node:
		/* begin expansion of Display penalty |p| */
		/* module 194 */
		print_esc_string ("penalty ");
		print_int (penalty (p));
		/* end expansion of Display penalty |p| */
		break;
	  case disc_node:
		/* begin expansion of Display discretionary |p| */
		/* module 195 */
		/* The |post_break| list of a discretionary node is indicated by a prefixed
		 * `\.{\char'174}' instead of the `\..' before the |pre_break| list.
		 */
		print_esc_string ("discretionary");
		if (replace_count (p) > 0) {
		  zprint_string(" replacing ");
		  print_int (replace_count (p));
		};
		node_list_display (pre_break (p));	/* recursive call */
		append_char ('|');
		show_node_list (post_break (p));
		flush_char;	/* recursive call */
		/* end expansion of Display discretionary |p| */
		break;
	  case mark_node:
		/* begin expansion of Display mark |p| */
		/* module 196 */
		print_esc_string ("mark");
		if (mark_class (p) != 0) {
		  print_char ('s');
		  print_int (mark_class (p));
		};
		print_mark (mark_ptr (p));
		/* end expansion of Display mark |p| */
		break;
	  case adjust_node:
		/* begin expansion of Display adjustment |p| */
		/* module 197 */
		print_esc_string ("vadjust");
		if (adjust_pre (p) != 0)
		  zprint_string(" pre ");
		node_list_display (adjust_ptr (p));	/* recursive call */
		/* end expansion of Display adjustment |p| */
		break;
		/* begin expansion of Cases of |show_node_list| that arise in mlists only */
		/* module 834 */
		/* Let's consider now the previously unwritten part of |show_node_list|
		 * that displays the things that can only be present in mlists; this
		 * program illustrates how to access the data structures just defined.
		 * 
		 * In the context of the following program, |p| points to a node or noad that
		 * should be displayed, and the current string contains the ``recursion history''
		 * that leads to this point. The recursion history consists of a dot for each
		 * outer level in which |p| is subsidiary to some node, or in which |p| is
		 * subsidiary to the |nucleus| field of some noad; the dot is replaced by
		 * `\.\_' or `\.\^' or `\./' or `\.\\' if |p| is descended from the |subscr|
		 * or |supscr| or |denominator| or |numerator| fields of noads. For example,
		 * the current string would be `\.{.\^.\_/}' if |p| points to the |ord_noad| for
		 * |x| in the (ridiculous) formula
		 * `\.{\$\\sqrt\{a\^\{\\mathinner\{b\_\{c\\over x+y\}\}\}\}\$}'.
		 */
	  case style_node:
		print_style (subtype (p));
		break;
	  case choice_node:
		/* begin expansion of Display choice node |p| */
		/* module 839 */
		print_esc_string ("mathchoice");
		append_char ('D');
		show_node_list (display_mlist (p));
		flush_char;
		append_char ('T');
		show_node_list (text_mlist (p));
		flush_char;
		append_char ('S');
		show_node_list (script_mlist (p));
		flush_char;
		append_char ('s');
		show_node_list (script_script_mlist (p));
		flush_char;
		/* end expansion of Display choice node |p| */
		break;
	  case ord_noad:
	  case op_noad:
	  case bin_noad:
	  case rel_noad:
	  case open_noad:
	  case close_noad:
	  case punct_noad:
	  case inner_noad:
	  case radical_noad:
	  case over_noad:
	  case under_noad:
	  case vcenter_noad:
	  case accent_noad:
	  case left_noad:
	  case right_noad:
		/* begin expansion of Display normal noad |p| */
		/* module 840 */
		switch (type (p)) {
		  case ord_noad:
		    print_esc_string ("mathord");
		    break;
		  case op_noad:
		    print_esc_string ("mathop");
		    break;
		  case bin_noad:
		    print_esc_string ("mathbin");
		    break;
		  case rel_noad:
		    print_esc_string ("mathrel");
		    break;
		  case open_noad:
		    print_esc_string ("mathopen");
		    break;
		  case close_noad:
		    print_esc_string ("mathclose");
		    break;
		  case punct_noad:
		    print_esc_string ("mathpunct");
		    break;
		  case inner_noad:
		    print_esc_string ("mathinner");
		    break;
		  case over_noad:
		    print_esc_string ("overline");
		    break;
		  case under_noad:
		    print_esc_string ("underline");
		    break;
		  case vcenter_noad:
		    print_esc_string ("vcenter");
		    break;
		  case radical_noad:
			print_esc_string ("radical");
			print_delimiter (left_delimiter (p));
		    break;
		  case accent_noad:
			print_esc_string ("accent");
			print_fam_and_char (accent_chr (p));
		    break;
		  case left_noad:
			print_esc_string ("left");
			print_delimiter (nucleus (p));
		    break;
		  case right_noad:
			if (subtype (p) == normal) {
			  print_esc_string ("right");
			} else {
			  print_esc_string ("middle");
			};
			print_delimiter (nucleus (p));
		};
		if (type (p) < left_noad) {
		  if (subtype (p) != normal) {
			if (subtype (p) == limits) {
			  print_esc_string ("limits");
			} else {
			  print_esc_string ("nolimits");
			}
		  }
		  print_subsidiary_data (nucleus (p), '.');
		}
		print_subsidiary_data (supscr (p), '^');
		print_subsidiary_data (subscr (p), '_');
		/* end expansion of Display normal noad |p| */
		break;
	  case fraction_noad:
		/* begin expansion of Display fraction noad |p| */
		/* module 841 */
		print_esc_string ("fraction, thickness ");
		if (thickness (p) == default_code) {
		  zprint_string("= default");
		} else {
		  print_scaled (thickness (p));
		};
		if ((small_fam (left_delimiter (p)) != 0)
		    || (small_char (left_delimiter (p)) != min_quarterword)
		    || (large_fam (left_delimiter (p)) != 0)
		    || (large_char (left_delimiter (p)) != min_quarterword)) {
		  zprint_string(", left-delimiter ");
		  print_delimiter (left_delimiter (p));
		};
		if ((small_fam (right_delimiter (p)) != 0)
		    || (small_char (right_delimiter (p)) != min_quarterword)
		    || (large_fam (right_delimiter (p)) != 0)
		    || (large_char (right_delimiter (p)) != min_quarterword)) {
		  zprint_string(", right-delimiter ");
		  print_delimiter (right_delimiter (p));
		  };
		print_subsidiary_data (numerator (p), '\\');
		print_subsidiary_data (denominator (p), '/');
		/* end expansion of Display fraction noad |p| */
		break;
	  default:
		zprint_string("Unknown node type!");
	  };
	  /* end expansion of Display node |p| */
	};
	p = link (p);
  };
}

/* module 199 */

/* 
 * When we are done with a node list, we are obliged to return it to free
 * storage, including all of its sublists. The recursive procedure
 * |flush_node_list| does this for us.
 */



/* module 200 */

/* First, however, we shall consider two non-recursive procedures that do
 * simpler tasks. The first of these, |delete_token_ref|, is called when
 * a pointer to a token list's reference count is being removed. This means
 * that the token list should disappear if the reference count was |null|,
 * otherwise the count should be decreased by one.
 */
void
delete_token_ref (pointer p) {
  /* |p| points to the reference count of a token list that is losing one reference */
  if (token_ref_count (p) == null) {
	flush_list (p);
  } else {
	decr (token_ref_count (p));
  };
}

/* module 201 */

/* Similarly, |delete_glue_ref| is called when a pointer to a glue
 * specification is being withdrawn.
 */
void
delete_glue_ref (pointer p) { /* |p| points to a glue specification */
  fast_delete_glue_ref (p);
};
 
/* module 202 */

/* Now we are ready to delete any node list, recursively.
 * In practice, the nodes deleted are usually charnodes (about 2/3 of the time),
 * and they are glue nodes in about half of the remaining cases.
 */
void
flush_node_list (pointer p) {
  /* erase list of nodes starting at |p| */
  /* label done: go here when node |p| has been freed */
  pointer q;			/* successor to node |p| */
  while (p != null) {
	q = link (p);
	if (is_char_node (p)) {
	  free_avail (p);
	} else {
	  switch (type (p)) {
	  case hlist_node:
	  case vlist_node:
	  case unset_node:
		flush_node_list (list_ptr (p));
		free_node (p, box_node_size);
		goto DONE;
	  case rule_node:
		free_node (p, rule_node_size);
		goto DONE;
	  case ins_node:
		flush_node_list (ins_ptr (p));
		delete_glue_ref (split_top_ptr (p));
		free_node (p, ins_node_size);
		goto DONE;
	  case whatsit_node:
		/* begin expansion of Wipe out the whatsit node |p| and |goto done| */
		/* module 1554 */
		switch (subtype (p)) {
		case open_node:
		  free_node (p, open_node_size);
		  break;
		case write_node:
		case special_node:
		  delete_token_ref (write_tokens (p));
		  free_node (p, write_node_size);
		  break;
		case close_node:
		case language_node:
		  free_node (p, small_node_size);
		  break;
		case pdf_literal_node:
		  delete_token_ref (pdf_literal_data (p));
		  free_node (p, write_node_size);
		  break;
		case pdf_refobj_node:
		  free_node (p, pdf_refobj_node_size);
		  break;
		case pdf_refxform_node:
		  free_node (p, pdf_refxform_node_size);
		  break;
		case pdf_refximage_node:
		  free_node (p, pdf_refximage_node_size);
		  break;
		case pdf_annot_node:
		  delete_token_ref (pdf_annot_data (p));
		  free_node (p, pdf_annot_node_size);
		  break;
		case pdf_start_link_node:
		  if (pdf_link_attr (p) != null)
			delete_token_ref (pdf_link_attr (p));
		  delete_action_ref (pdf_link_action (p));
		  free_node (p, pdf_annot_node_size);
		  break;
		case pdf_end_link_node:
		  free_node (p, small_node_size);
		  break;
		case pdf_dest_node:
		  if (pdf_dest_named_id (p) > 0)
			delete_token_ref (pdf_dest_id (p));
		  free_node (p, pdf_dest_node_size);
		  break;
		case pdf_thread_node:
		case pdf_start_thread_node:
		  if (pdf_thread_named_id (p) > 0)
			delete_token_ref (pdf_thread_id (p));
		  if (pdf_thread_attr (p) != null)
			delete_token_ref (pdf_thread_attr (p));
		  free_node (p, pdf_thread_node_size);
		  break;
		case pdf_end_thread_node:
		  free_node (p, small_node_size);
		  break;
		case pdf_save_pos_node:
		case pdf_snap_ref_point_node:
		  free_node (p, small_node_size);
		  break;
		case pdf_snap_x_node:
		case pdf_snap_y_node:
		  delete_glue_ref (snap_glue_ptr (p));
		  free_node (p, small_node_size);
		  break;
		default:
		  confusion ("ext3");
		};
		goto DONE;
		/* end expansion of Wipe out the whatsit node |p| and |goto done| */
	  case glue_node:
		fast_delete_glue_ref (glue_ptr (p));
		if (leader_ptr (p) != null)
		  flush_node_list (leader_ptr (p));
		break;
	  case margin_kern_node:
		free_avail (margin_char (p));
		free_node (p, margin_kern_node_size);
		goto DONE;
	  case kern_node:
	  case math_node:
	  case penalty_node:
		do_something;
		break;
	  case ligature_node:
		flush_node_list (lig_ptr (p));
		break;
	  case mark_node:
		delete_token_ref (mark_ptr (p));
		break;
	  case disc_node:
		flush_node_list (pre_break (p));
		flush_node_list (post_break (p));
		break;
	  case adjust_node:
		flush_node_list (adjust_ptr (p));
		break;
		/* begin expansion of Cases of |flush_node_list| that arise in mlists only */
		/* module 842 */
		/* That which can be displayed can also be destroyed. */
	  case style_node:
		free_node (p, style_node_size);
		goto DONE;
	  case choice_node:
		flush_node_list (display_mlist (p));
		flush_node_list (text_mlist (p));
		flush_node_list (script_mlist (p));
		flush_node_list (script_script_mlist (p));
		free_node (p, style_node_size);
		goto DONE;
	  case ord_noad:
	  case op_noad:
	  case bin_noad:
	  case rel_noad:
	  case open_noad:
	  case close_noad:
	  case punct_noad:
	  case inner_noad:
	  case radical_noad:
	  case over_noad:
	  case under_noad:
	  case vcenter_noad:
	  case accent_noad:
		if (math_type (nucleus (p)) >= sub_box)
		  flush_node_list (info (nucleus (p)));
		if (math_type (supscr (p)) >= sub_box)
		  flush_node_list (info (supscr (p)));
		if (math_type (subscr (p)) >= sub_box)
		  flush_node_list (info (subscr (p)));
		if (type (p) == radical_noad) {
		  free_node (p, radical_noad_size);
		} else if (type (p) == accent_noad) {
		  free_node (p, accent_noad_size);
		} else {
		  free_node (p, noad_size);
		};
		goto DONE;
	  case left_noad:
	  case right_noad:
		free_node (p, noad_size);
		goto DONE;
	  case fraction_noad:
		flush_node_list (info (numerator (p)));
		flush_node_list (info (denominator (p)));
		free_node (p, fraction_noad_size);
		goto DONE;
	  default:
		confusion ("flushing");
	  };
	  free_node (p, small_node_size);
	};
    DONE:
	p = q;
  };
}

/* module 204 */

/* The copying procedure copies words en masse without bothering
 * to look at their individual fields. If the node format changes---for
 * example, if the size is altered, or if some link field is moved to another
 * relative position---then this code may need to be changed too.
 */

pointer
copy_node_list (pointer p) {
  /* makes a duplicate of the node list that starts at |p| and returns a pointer to the new list */
  pointer h;			/* temporary head of copied list */
  pointer q;			/* previous position in new list */
  pointer r;			/* current node being fabricated for new list */
  unsigned char words;		/* number of words remaining to be copied */
  r = 0; /*TH please -O2 -Wall*/
  h = get_avail ();
  q = h;
  while (p != null) {
	/* begin expansion of Make a copy of node |p| in node |r| */
	/* module 205 */
	words = 1; /* this setting occurs in more branches than any other */
	if (is_char_node (p)) {
	  r = get_avail ();
	} else {
	  /* begin expansion of Case statement to copy different types and 
	     set |words| to the number of initial words not yet copied */
	  /* module 206 */
	  switch (type (p)) {
	  case hlist_node:
	  case vlist_node:
	  case unset_node:
		r = get_node (box_node_size);
		mem[r + 6] = mem[p + 6];
		mem[r + 5] = mem[p + 5]; /* copy the last two words */
		list_ptr (r) = copy_node_list (list_ptr (p)); /* this affects |mem[r+5]| */
		words = 5;
		break;
	  case rule_node:
		r = get_node (rule_node_size);
		words = rule_node_size;
		break;
	  case ins_node:
		r = get_node (ins_node_size);
		mem[r + 4] = mem[p + 4];
		add_glue_ref (split_top_ptr (p));
		ins_ptr (r) = copy_node_list (ins_ptr (p)); /* this affects |mem[r+4]| */
		words = ins_node_size - 1;
		break;
	  case whatsit_node:
		/* begin expansion of Make a partial copy of the whatsit node |p| and make 
		   |r| point to it; set |words| to the number of initial words not yet copied */
		do_a_partial_whatsit_copy;
		break;
	  case glue_node:
		r = get_node (small_node_size);
		add_glue_ref (glue_ptr (p));
		glue_ptr (r) = glue_ptr (p);
		leader_ptr (r) = copy_node_list (leader_ptr (p));
		break;
	  case margin_kern_node:
		r = get_node (margin_kern_node_size);
		fast_get_avail (margin_char (r));
		font (margin_char (r)) = font (margin_char (p));
		character (margin_char (r)) = character (margin_char (p));
		words = small_node_size;
		break;
	  case kern_node:
	  case math_node:
		r = get_node (small_node_size);
		words = small_node_size;
		break;
	  case penalty_node:
		r = get_node (small_node_size);
		penalty (r) = penalty (p);
		if ((pdf_max_penalty != 0) && (penalty (r) > pdf_max_penalty))
		  penalty (r) = pdf_max_penalty;
		if ((pdf_min_penalty != 0) && (penalty (r) < pdf_min_penalty))
		  penalty (r) = pdf_min_penalty;
		break;
	  case ligature_node:
		r = get_node (small_node_size);
		mem[lig_char (r)] = mem[lig_char (p)]; 
		/* copy |font| and |character| */
		lig_ptr (r) = copy_node_list (lig_ptr (p));
		break;
	  case disc_node:
		r = get_node (small_node_size);
		pre_break (r) = copy_node_list (pre_break (p));
		post_break (r) = copy_node_list (post_break (p));
		break;
	  case mark_node:
		r = get_node (small_node_size);
		add_token_ref (mark_ptr (p));
		words = small_node_size;
		break;
	  case adjust_node:
		r = get_node (small_node_size);
		adjust_ptr (r) = copy_node_list (adjust_ptr (p));
		break;
	  default:
		confusion ("copying");
	  };
	  /* end expansion of Case statement to copy different types and set  ...*/
	};
	while (words > 0) {
	  decr (words);
	  mem[r + words] = mem[p + words];
	};
	/* end expansion of Make a copy of node |p| in node |r| */
	link (q) = r;
	q = r;
	p = link (p);
  };
  link (q) = null;
  q = link (h);
  free_avail (h);
  return q;
}


/* module 837 */
/* this module is not needed in this C version (show_info) */

/* module 1580 */
/* from pdftex */
integer 
node_type (pointer p)	{
  if (p == null)
    return 0;
  if (is_char_node (p)) {
    zprint_string ("char_node") ;
  } else {
    switch (type (p)) {
    case hlist_node:
      zprint_string("hlist_node");
      break;
    case vlist_node:
      zprint_string("vlist_node");
      break;
    case rule_node:
      zprint_string("rule_node");
      break;
    case ins_node:
      zprint_string("ins_node");
      break;
    case mark_node:
      zprint_string("mark_node");
      break;
    case adjust_node:
      zprint_string("adjust_node");
      break;
    case ligature_node:
      zprint_string("ligature_node");
      break;
    case disc_node:
      zprint_string("disc_node");
      break;
    case whatsit_node:
      zprint_string("whatsit_node");
      break;
    case math_node:
      zprint_string("math_node");
      break;
    case glue_node:
      zprint_string("glue_node");
      break;
    case kern_node:
      zprint_string("kern_node");
      break;
    case penalty_node:
      zprint_string("penalty_node");
      break;
    case unset_node:
      zprint_string("unset_node");
      break;
    case style_node:
      zprint_string("style_node");
      break;
    case choice_node:
      zprint_string("choice_node");
    };
  };
  print_ln();
  return 1;
};


