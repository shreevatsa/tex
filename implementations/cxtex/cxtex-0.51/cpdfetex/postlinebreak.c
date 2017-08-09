#include "types.h"
#include "c-compat.h"


#include "globals.h"



/* module 1654 */
#define adjust_LR_stack  if (end_LR (q)) { if (LR_ptr != null) \
        if (info (LR_ptr) == end_LR_type (q))\
         pop_LR;\
        }  else {\
         push_LR (q);\
        }

/* module 1021 */

/* The total number of lines that will be set by |post_line_break|
 * is |best_line-prev_graf-1|. The last breakpoint is specified by
 * |break_node(best_bet)|, and this passive node points to the other breakpoints
 * via the |prev_break| links. The finishing-up phase starts by linking the
 * relevant passive nodes in forward order, changing |prev_break| to
 * |next_break|. (The |next_break| fields actually reside in the same memory
 * space as the |prev_break| fields did, but we give them a new name because
 * of their new significance.) Then the lines are justified, one by one.
 */

void 
post_line_break (boolean d) {
  pointer q, r, s; /* temporary registers for list manipulation */ 
  pointer p, k;
  scaled w;
  boolean disc_break; /* was the current break at a discretionary node? */ 
  boolean post_disc_break; /* and did it have a nonempty post-break part? */ 
  scaled cur_width; /* width of line number |cur_line| */ 
  scaled cur_indent; /* left margin of line number |cur_line| */ 
  quarterword t; /* used for replacement counts in discretionary nodes */ 
  int pen; /* use when calculating penalties between lines */ 
  halfword cur_line; /* the current line number being justified */ 
  pointer LR_ptr; /* stack of LR codes */ 
  LR_ptr = LR_save;
  /* begin expansion of Reverse the links of the relevant passive nodes, setting |cur_p| to the first breakpoint */
  /* module 1022 */
  /* The job of reversing links in a list is conveniently regarded as the job
   * of taking items off one stack and putting them on another. In this case we
   * take them off a stack pointed to by |q| and having |prev_break| fields;
   * we put them on a stack pointed to by |cur_p| and having |next_break| fields.
   * Node |r| is the passive node being moved from stack to stack.
   */
  q = break_node (best_bet);
  cur_p = null;
  do {
	r = q;
	q = prev_break (q);
	next_break (r) = cur_p;
	cur_p = r;
  } while (q != null);
  /* end expansion of Reverse the links of the relevant passive nodes, setting |cur_p| to the first breakpoint */
  cur_line = prev_graf + 1;
  do {
	/* begin expansion of Justify the line ending at breakpoint |cur_p|, 
	   and append it to the current vertical list, together with associated penalties and other insertions */
	/* module 1024 */
	/* The current line to be justified appears in a horizontal list starting
	 * at |link(temp_head)| and ending at |cur_break(cur_p)|. If |cur_break(cur_p)| is
	 * a glue node, we reset the glue to equal the |right_skip| glue; otherwise
	 * we append the |right_skip| glue at the right. If |cur_break(cur_p)| is a
	 * discretionary node, we modify the list so that the discretionary break
	 * is compulsory, and we set |disc_break| to |true|. We also append
	 * the |left_skip| glue at the left of the line, unless it is zero.
	 */
	if (TeXXeT_en) {
	  /* begin expansion of Insert LR nodes at the beginning of the current line and adjust the 
		 LR stack based on LR nodes in this line */
	  /* module 1653 */
	  q = link (temp_head);
	  if (LR_ptr != null) {
		temp_ptr = LR_ptr;
		r = q;
		do {
		  s = new_math (0, begin_LR_type (info (temp_ptr)));
		  link (s) = r;
		  r = s;
		  temp_ptr = link (temp_ptr);
		} while (temp_ptr != null);
		link (temp_head) = r;
	  };
	  while (q != cur_break (cur_p)) {
		if (!is_char_node (q)) {
		  if (type (q) == math_node) {
			/* Adjust \(t)the LR stack for the |p... */
            adjust_LR_stack;
		  }
		}
		q = link (q);
	  };
	}
	/* end expansion of Insert LR nodes at the beginning of the current line and ...*/
	
	/* begin expansion of Modify the end of the line to reflect the nature of the break and
	   to include \.{\\rightskip}; also set the proper value of |disc_break| */

	/* module 1025 */
	/* At the end of the following code, |q| will point to the final node on the
	 * list about to be justified.
	 */
	q = cur_break (cur_p);
	disc_break = false;
	post_disc_break = false;
	if (q != null)	{ /* |q| cannot be a |char_node| */
	  if (type (q) == glue_node) {
		delete_glue_ref (glue_ptr (q));
		glue_ptr (q) = right_skip;
		subtype (q) = right_skip_code + 1;
		add_glue_ref (right_skip);
		goto DONE;
	  } else if (type (q) == disc_node) {
		/* begin expansion of Change discretionary to compulsory and set |disc_break:=true| */
		/* module 1026 */
		t = replace_count (q);
		/* begin expansion of Destroy the |t| nodes following |q|, and make |r| point to the following node */
		/* module 1027 */
		if (t == 0) {
		  r = link (q);
		} else {
		  r = q;
		  while (t > 1) {
			r = link (r);
			decr (t);
		  };
		  s = link (r);
		  r = link (s);
		  link (s) = null;
		  flush_node_list (link (q));
		  replace_count (q) = 0;
		};
		/* end expansion of Destroy the |t| nodes following |q|, and make |r| point to the following node */
		if (post_break (q) != null) {
		  /* begin expansion of Transplant the post-break list */
		  /* module 1028 */
		  /* We move the post-break list from inside node |q| to the main list by
		   * re\-attaching it just before the present node |r|, then resetting |r|.
		   */
		  s = post_break (q);
		  while (link (s) != null)
			s = link (s);
		  link (s) = r;
		  r = post_break (q);
		  post_break (q) = null;
		  post_disc_break = true;
		};
		/* end expansion of Transplant the post-break list */
		if (pre_break (q) != null) {
		  /* begin expansion of Transplant the pre-break list */
		  /* module 1029 */
		  /* We move the pre-break list from inside node |q| to the main list by
		   * re\-attaching it just after the present node |q|, then resetting |q|.
		   */
		  s = pre_break (q);
		  link (q) = s;
		  while (link (s) != null)
			s = link (s);
		  pre_break (q) = null;
		  q = s;
		};
		/* end expansion of Transplant the pre-break list */
		link (q) = r;
		disc_break = true;
		/* end expansion of Change discretionary to compulsory and set |disc_break:=true| */
	  } else if (type (q) == kern_node) {
		width (q) = 0;
	  } else if (type (q) == math_node) {
		width (q) = 0;
		if (TeXXeT_en) {
		  /* Adjust \(t)the LR stack for the |p... */
		  adjust_LR_stack;
		}
	  }
	} else {
	  q = temp_head;
	  while (link (q) != null)
		q = link (q);
	};
	/* begin expansion of Put the \(r)\.{\\rightskip} glue after node |q| */
	/* module 1030 */
	r = new_param_glue (right_skip_code);
	link (r) = link (q);
	link (q) = r;
	q = r;
	/* end expansion of Put the \(r)\.{\\rightskip} glue after node |q| */
  DONE:
	if (pdf_protrude_chars > 0) {
	  p = prev_rightmost (temp_head, q);
	  if ((p != null)&& ((type (p) == disc_node)|| (type (p) == penalty_node)))
		p = prev_rightmost (temp_head, p);
	  w = right_pw (p);
	  if (p != null) {
		while (link (p) != q)
		  p = link (p);
		if (w != 0) {
		  k = new_margin_kern (-w, last_rightmost_char, right_side);
		  link (p) = k;
		  link (k) = q;
		};
	  };
	};
	/* end expansion of Modify the end of the line to reflect the nature of t...*/
	if (TeXXeT_en)
	  /* begin expansion of Insert LR nodes at the end of the current line */
	  /* module 1655 */
	  /* We use the fact that |q| now points to the node with \.{\\rightskip} glue.
	   */
	  if (LR_ptr != null) {
		s = temp_head;
		r = link (s);
		while (r != q) {
		  s = r;
		  r = link (s);
		};
		r = LR_ptr;
		while (r != null) {
		  temp_ptr = new_math (0, info (r));
		  link (s) = temp_ptr;
		  s = temp_ptr;
		  r = link (r);
		};
		link (s) = q;
	  };
	/* end expansion of Insert LR nodes at the end of the current line */
	/* begin expansion of Put the \(l)\.{\\leftskip} glue at the left and detach this line */
	/* module 1031 */
	/* The following code begins with |q| at the end of the list to be
	 * justified. It ends with |q| at the beginning of that list, and with
	 * |link(temp_head)| pointing to the remainder of the paragraph, if any.
	 */
	r = link (q);
	link (q) = null;
	q = link (temp_head);
	link (temp_head) = r;
	if (pdf_protrude_chars > 0) {
	  p = q;
	  while ((p != null) && discardable (p))
		p = link (p);
	  w = left_pw (p);
	  if (w != 0) {
		k = new_margin_kern (-w, last_leftmost_char, left_side);
		link (k) = q;
		q = k;
	  };
	};
	if (left_skip != zero_glue) {
	  r = new_param_glue (left_skip_code);
	  link (r) = q;
	  q = r;
	};
	/* end expansion of Put the \(l)\.{\\leftskip} glue at the left and detach this line */

	/* begin expansion of Call the packaging subroutine, setting |just_box| to the justified box */
	/* module 1033 */
	/* Now |q| points to the hlist that represents the current line of the
	 * paragraph. We need to compute the appropriate line width, pack the
	 * line into a box of this size, and shift the box by the appropriate
	 * amount of indentation.
	 */
	if (cur_line > last_special_line) {
	  cur_width = second_width;
	  cur_indent = second_indent;
	} else if (par_shape_ptr == null) {
	  cur_width = first_width;
	  cur_indent = first_indent;
	} else {
	  cur_width = mem[par_shape_ptr + 2 * cur_line].sc;
	  cur_indent = mem[par_shape_ptr + 2 * cur_line - 1].sc;
	};
	adjust_tail = adjust_head;
	pre_adjust_tail = pre_adjust_head;
	if (pdf_adjust_spacing > 0) {
	  just_box = hpack (q, cur_width, cal_expand_ratio);
	} else {
	  just_box = hpack (q, cur_width, exactly);
	}
	shift_amount (just_box) = cur_indent;
	/* end expansion of Call the packaging subroutine, setting |just_box| to the justified box */

	/* begin expansion of Append the new box to the current vertical list, followed 
	   by the list of special nodes taken out of the box by the packager */
	/* module 1032 */
	/* |append_list| is used to append a list to |tail|.
	 */
	if (pre_adjust_head != pre_adjust_tail)
	  append_list (pre_adjust_head,pre_adjust_tail);
	pre_adjust_tail = null;
	prepend_line_snap_nodes();
	append_to_vlist (just_box);
	if (adjust_head != adjust_tail)
	  append_list (adjust_head,adjust_tail);
	adjust_tail = null;
	/* end expansion of Append the new box to the current vertical list..*/
	
	/* begin expansion of Append a penalty node, if a nonzero penalty is appropriate */
	/* module 1034 */
	/* Penalties between the lines of a paragraph come from club and widow lines,
	 * from the |inter_line_penalty| parameter, and from lines that end at
	 * discretionary breaks. Breaking between lines of a two-line paragraph gets
	 * both club-line and widow-line penalties. The local variable |pen| will
	 * be set to the sum of all relevant penalties for the current line, except
	 * that the final line is never penalized.
	 */
	if (cur_line + 1 != best_line) {
	  q = inter_line_penalties_ptr;
	  if (q != null) {
		r = cur_line;
		if (r > penalty (q))
		  r = penalty (q);
		pen = penalty (q + r);
	  } else {
		pen = inter_line_penalty;
	  }
	  q = club_penalties_ptr;
	  if (q != null) {
		r = cur_line - prev_graf;
		if (r > penalty (q))
		  r = penalty (q);
		pen = pen + penalty (q + r);
	  } else if (cur_line == prev_graf + 1) {
		pen = pen + club_penalty;
	  }
	  if (d) {
		q = display_widow_penalties_ptr;
	  } else {
		q = widow_penalties_ptr;
	  }
	  if (q != null) {
		r = best_line - cur_line - 1;
		if (r > penalty (q))
		  r = penalty (q);
		pen = pen + penalty (q + r);
	  } else if (cur_line + 2 == best_line) {
		if (d) {
		  pen = pen + display_widow_penalty;
		} else {
		  pen = pen + widow_penalty;
		}
	  }
	  if (disc_break)
		pen = pen + broken_penalty;
	  if (pen != 0) {
		r = new_penalty (pen);
		link (tail) = r;
		tail = r;
	  };
	}
	/* end expansion of Append a penalty node, if a nonzero penalty is appropriate */
	/* end expansion of Justify the line ending at breakpoint |cur_p|, and append it to ... */
	incr (cur_line);
	cur_p = next_break (cur_p);
	if (cur_p != null)
	  if (!post_disc_break) {
		/* begin expansion of Prune unwanted nodes at the beginning of the next line */
		/* module 1023 */
		/* Glue and penalty and kern and math nodes are deleted at the beginning of
		 * a line, except in the anomalous case that the node to be deleted is actually
		 * one of the chosen breakpoints. Otherwise
		 * the pruning done here is designed to match
		 * the lookahead computation in |try_break|, where the |break_width| values
		 * are computed for non-discretionary breakpoints.
		 */
		r = temp_head;
		loop {
		  q = link (r);
		  if (q == cur_break (cur_p)) {
			goto DONE1; /* |cur_break(cur_p)| is the next breakpoint */
		  }
		  /* now |q| cannot be |null| */
		  if (is_char_node (q)) {
			goto DONE1;
		  }
		  if (non_discardable (q)) {
			goto DONE1;
		  }
		  if (type (q) == kern_node) {
			if (subtype (q) != explicit) {
			  goto DONE1;
			}
		  }
		  r = q; /* now |type(q)=glue_node|, |kern_node|, |math_node| or |penalty_node| */
		  if (type (q) == math_node) {
			if (TeXXeT_en) {
			  /* begin expansion of Adjust \(t)the LR stack for the |post_line_break| routine */
			  adjust_LR_stack;
			};
		  }
		};
	  DONE1:
		if (r != temp_head) {
		  link (r) = null;
		  flush_node_list (link (temp_head));
		  link (temp_head) = q;
		};
	  };
	/* end expansion of Prune unwanted nodes at the beginning of the next line */
  } while (cur_p != null);
  if ((cur_line != best_line) || (link (temp_head) != null))
	confusion ("line breaking");
  prev_graf = best_line - 1;
  LR_save = LR_ptr;
};

