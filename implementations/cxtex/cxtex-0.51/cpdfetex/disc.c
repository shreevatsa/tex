
#include "types.h"
#include "c-compat.h"
#include "globals.h"

/* module 145 */

/* A |disc_node|, which occurs only in horizontal lists, specifies a
 * ``dis\-cretion\-ary'' line break. If such a break occurs at node |p|, the text
 * that starts at |pre_break(p)| will precede the break, the text that starts at
 * |post_break(p)| will follow the break, and text that appears in the next
 * |replace_count(p)| nodes will be ignored. For example, an ordinary
 * discretionary hyphen, indicated by `\.{\\-}', yields a |disc_node| with
 * |pre_break| pointing to a |char_node| containing a hyphen, |post_break=null|,
 * and |replace_count=0|. All three of the discretionary texts must be
 * lists that consist entirely of character, kern, box, rule, and ligature nodes.
 * 
 * If |pre_break(p)=null|, the |ex_hyphen_penalty| will be charged for this
 * break. Otherwise the |hyphen_penalty| will be charged. The texts will
 * actually be substituted into the list by the line-breaking algorithm if it
 * decides to make the break, and the discretionary node will disappear at
 * that time; thus, the output routine sees only discretionaries that were
 * not chosen.
 */
pointer
new_disc (void) { /* creates an empty |disc_node| */
  pointer p;			/* the new node */
  p = get_node (small_node_size);
  type (p) = disc_node;
  replace_count (p) = 0;
  pre_break (p) = null;
  post_break (p) = null;
  return p;
};



/* module 1262 */

/* The space factor does not change when we append a discretionary node,
 * but it starts out as 1000 in the subsidiary lists.
 */
void 
append_discretionary (void) {
  int c; /* hyphen character */ 
  tail_append (new_disc());
  if (cur_chr == 1) {
	c = hyphen_char[cur_font];
	if (c >= 0)
	  if (c < 256)
		pre_break (tail) = new_character (cur_font, c);
  } else {
	incr (save_ptr);
	saved (-1) = 0;
	new_save_level (disc_group);
	scan_left_brace();
	push_nest();
	mode = -hmode;
	space_factor = 1000;
  };
};

/* module 1264 */
void 
build_discretionary (void) {
  pointer p, q; /* for link manipulation */ 
  int n; /* length of discretionary list */ 
  unsave();
  /* begin expansion of Prune the current list, if necessary, until it contains only
	 |char_node|, |kern_node|, |hlist_node|, |vlist_node|, |rule_node|, and |ligature_node|
	 items; set |n| to the length of the list, and set |q| to the list's tail */
  /* module 1266 */
  /* During this loop, |p=link(q)| and there are |n| items preceding |p|. */
  q = head;
  p = link (q);
  n = 0;
  while (p != null) {
	if (!is_char_node (p))
	  if (type (p) > rule_node)
		if (type (p) != kern_node)
		  if (type (p) != ligature_node) {
			print_err ("Improper discretionary list");
			help1  ("Discretionary lists must contain only boxes and kerns.");
			error();
			begin_diagnostic();
			print_nl_string ("The following discretionary sublist has been deleted:");
			show_box (p);
			end_diagnostic (true);
			flush_node_list (p);
			link (q) = null;
			goto DONE;
		  };
	q = p;
	p = link (q);
	incr (n);
  };
 DONE:
  /* end expansion of Prune the current list, if necessary, until it contains ...*/
  p = link (head);
  pop_nest();
  switch (saved (-1)) {
  case 0:pre_break (tail) = p;
	break;
  case 1:post_break (tail) = p;
	break;
  case 2:
	/* begin expansion of Attach list |p| to the current list, and record its length; 
	   then finish up and |return| */
	/* module 1265 */
	if ((n > 0) && (abs (mode) == mmode)) {
	  print_err ("Illegal math ");
	  print_esc_string ("discretionary");
	  help2 ("Sorry: The third part of a discretionary break must be",
			 "empty, in math formulas. I had to delete your third part.");
	  flush_node_list (p);
	  n = 0;
	  error();
	} else {
	  link (tail) = p;
	}
	if (n <= max_quarterword) {
	  replace_count (tail) = n;
	} else {
	  print_err ("Discretionary list is too long");
	  help2 ("Wow---I never thought anybody would tweak me here.",
			 "You can't seriously need such a huge discretionary list?");
	  error();
	};
	if (n > 0)
	  tail = q;
	decr (save_ptr);
	return;
	/* end expansion of Attach list |p| to the current list, and..*/
  }; /* there are no other cases */ 
  incr (saved (-1));
  new_save_level (disc_group);
  scan_left_brace();
  push_nest();
  mode = -hmode;
  space_factor = 1000;
};

 
