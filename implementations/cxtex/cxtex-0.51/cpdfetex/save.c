#include "types.h"
#include "c-compat.h"
#include "globals.h"

/* forward */
static void restore_trace (pointer p, char *s);


/* module 268 */

/* 
 * The nested structure provided by `$\.{\char'173}\ldots\.{\char'175}$' groups
 * in \TeX\ means that |eqtb| entries valid in outer groups should be saved
 * and restored later if they are overridden inside the braces. When a new |eqtb|
 * value is being assigned, the program therefore checks to see if the previous
 * entry belongs to an outer level. In such a case, the old value is placed
 * on the |save_stack| just before the new value enters |eqtb|. At the
 * end of a grouping level, i.e., when the right brace is sensed, the
 * |save_stack| is used to restore the outer values, and the inner ones are
 * destroyed.
 * 
 * Entries on the |save_stack| are of type |memory_word|. The top item on
 * this stack is |save_stack[p]|, where |p=save_ptr-1|; it contains three
 * fields called |save_type|, |save_level|, and |save_index|, and it is
 * interpreted in one of five ways:
 * 
 * \yskip\hangg 1) If |save_type(p)=restore_old_value|, then
 * |save_index(p)| is a location in |eqtb| whose current value should
 * be destroyed at the end of the current group and replaced by |save_stack[p-1]|.
 * Furthermore if |save_index(p)>=int_base|, then |save_level(p)|
 * should replace the corresponding entry in |xeq_level|.
 * 
 * \yskip\hangg 2) If |save_type(p)=restore_zero|, then |save_index(p)|
 * is a location in |eqtb| whose current value should be destroyed at the end
 * of the current group, when it should be
 * replaced by the current value of |eqtb[undefined_control_sequence]|.
 * 
 * \yskip\hangg 3) If |save_type(p)=insert_token|, then |save_index(p)|
 * is a token that should be inserted into \TeX's input when the current
 * group ends.
 * 
 * \yskip\hangg 4) If |save_type(p)=level_boundary|, then |save_level(p)|
 * is a code explaining what kind of group we were previously in, and
 * |save_index(p)| points to the level boundary word at the bottom of
 * the entries for that group.
 * Furthermore, in extended \eTeX\ mode, |save_stack[p-1]| contains the
 * source line number at which the current level of grouping was entered.
 * 
 * \yskip\hang 5) If |save_type(p)=restore_sa|, then |sa_chain| points to a
 * chain of sparse array entries to be restored at the end of the current
 * group. Furthermore |save_index(p)| and |save_level(p)| should replace
 * the values of |sa_chain| and |sa_level| respectively.
 */

/* module 270 */

/* The global variable |cur_group| keeps track of what sort of group we are
 * currently in. Another global variable, |cur_boundary|, points to the
 * topmost |level_boundary| word. And |cur_level| is the current depth of
 * nesting. The routines are designed to preserve the condition that no entry
 * in the |save_stack| or in |eqtb| ever has a level greater than |cur_level|.
 */

/* module 271 */
memory_word *save_stack;
integer save_ptr; /* first unused entry on |save_stack| */ 
integer max_save_stack; /* maximum usage of save stack */ 
quarterword cur_level; /* current nesting level for groups */ 
group_code cur_group; /* current group type */ 
unsigned int cur_boundary; /* where the current level begins */


/* module 274 */

/* Procedure |new_save_level| is called when a group begins. The
 * argument is a group identification code like `|hbox_group|'. After
 * calling this routine, it is safe to put five more entries on |save_stack|.
 * 
 * In some cases integer-valued items are placed onto the
 * |save_stack| just below a |level_boundary| word, because this is a
 * convenient place to keep information that is supposed to ``pop up'' just
 * when the group has finished.
 * For example, when `\.{\\hbox to 100pt}\grp' is being treated, the 100pt
 * dimension is stored on |save_stack| just before |new_save_level| is
 * called.
 * 
 * We use the notation |saved(k)| to stand for an integer item that
 * appears in location |save_ptr+k| of the save stack.
 */

void
new_save_level (group_code c) {	/* begin a new level of grouping */
  check_full_save_stack;
  if (eTeX_ex) {
	saved (0) = line;
	incr (save_ptr);
  };
  save_type (save_ptr) = level_boundary;
  save_level (save_ptr) = cur_group;
  save_index (save_ptr) = cur_boundary;
  if (cur_level == max_quarterword)
    overflow ("grouping levels", max_quarterword - min_quarterword);
  /* quit if |(cur_level+1)| is too big to be stored in |eqtb| */
  cur_boundary = save_ptr;
  cur_group = c;
  if (tracing_groups > 0)
    group_trace (false);
  incr (cur_level);
  incr (save_ptr);
};


/* module 275 */

/* Just before an entry of |eqtb| is changed, the following procedure should
 * be called to update the other data structures properly. It is important
 * to keep in mind that reference counts in |mem| include references from
 * within |save_stack|, so these counts must be handled carefully.
 */
static void
eq_destroy (memory_word w) { /* gets ready to forget |w| */
  pointer q; /* |equiv| field of |w| */
  switch (eq_type_field (w)) {
  case call:
  case long_call:
  case outer_call:
  case long_outer_call:
	delete_token_ref (equiv_field (w));
	break;
  case glue_ref:
	delete_glue_ref (equiv_field (w));
	break;
  case shape_ref:
	q = equiv_field (w); /* we need to free a \.{\\parshape} block */
	if (q != null)
	  free_node (q, info (q) + info (q) + 1); /* such a block is |2n+1| words long, where |n=info(q)| */
	break;
  case box_ref:
	flush_node_list (equiv_field (w));
	break;
	/* begin expansion of Cases for |eq_destroy| */
	/* module 1779 */
	/* When a shorthand definition for an element of one of the sparse arrays
	 * is destroyed, we must reduce the reference count.
	 */
  case toks_register:
  case register_cmd:
	if ((equiv_field (w) < mem_bot) || (equiv_field (w) > lo_mem_stat_max))
	  delete_sa_ref (equiv_field (w));
	break;
  default:
	do_nothing;
  };
};


/* module 276 */

/* To save a value of |eqtb[p]| that was established at level |l|, we
 * can use the following subroutine.
 */
static void
eq_save (pointer p, quarterword l) { /* saves |eqtb[p]| */
  check_full_save_stack;
  if (l == level_zero) {
	save_type (save_ptr) = restore_zero;
  } else {
	save_stack[save_ptr] = eqtb[p];
	incr (save_ptr);
	save_type (save_ptr) = restore_old_value;
  };
  save_level (save_ptr) = l;
  save_index (save_ptr) = p;
  incr (save_ptr);
};

/* module 277 */

/* The procedure |eq_define| defines an |eqtb| entry having specified
 * |eq_type| and |equiv| fields, and saves the former value if appropriate.
 * This procedure is used only for entries in the first four regions of |eqtb|,
 * i.e., only for entries that have |eq_type| and |equiv| fields.
 * After calling this routine, it is safe to put four more entries on
 * |save_stack|, provided that there was room for four more entries before
 * the call, since |eq_save| makes the necessary test.
 */

#define ASSIGN_TRACE(vara,varb)   if ( tracing_assigns  > 0 )  restore_trace ( vara, varb);

void
eq_define (pointer p, quarterword t, halfword e) { /* new data for |eqtb| */
  if (eTeX_ex && (eq_type (p) == t) && (equiv (p) == e)) {
	ASSIGN_TRACE (p, "reassigning");
	eq_destroy (eqtb[p]);
	return;
  };
  ASSIGN_TRACE (p, "changing");
  if (eq_level (p) == cur_level) {
	eq_destroy (eqtb[p]);
  } else if (cur_level > level_one) {
	eq_save (p, eq_level (p));
  }
  eq_level (p) = cur_level;
  eq_type (p) = t;
  equiv (p) = e;
  ASSIGN_TRACE (p, "into");
};


/* module 278 */

/* The counterpart of |eq_define| for the remaining (fullword) positions in
 * |eqtb| is called |eq_word_define|. Since |xeq_level[p]>=level_one| for all
 * |p|, a `|restore_zero|' will never be used in this case.
 */
void
eq_word_define (pointer p, int w) {
  if (eTeX_ex && (eqtb[p].cint == w)) {
	ASSIGN_TRACE (p, "reassigning");
	return;
  };
  ASSIGN_TRACE (p, "changing");
  if (xeq_level[p] != cur_level) {
	eq_save (p, xeq_level[p]);
	xeq_level[p] = cur_level;
  };
  eqtb[p].cint = w;
  ASSIGN_TRACE (p, "into");
};


/* module 279 */

/* The |eq_define| and |eq_word_define| routines take care of local definitions.
 * 
 * Global definitions are done in almost the same way, but there is no need
 * to save old values, and the new value is associated with |level_one|.
 */
void
geq_define (pointer p, quarterword t, halfword e) {	/* global |eq_define| */
  ASSIGN_TRACE (p, "globally changing");
  eq_destroy (eqtb[p]);
  eq_level (p) = level_one;
  eq_type (p) = t;
  equiv (p) = e;
  ASSIGN_TRACE (p, "into");
};

void
geq_word_define (pointer p, int w) { /* global |eq_word_define| */
  ASSIGN_TRACE (p, "globally changing");
  eqtb[p].cint = w;
  xeq_level[p] = level_one;
  ASSIGN_TRACE (p, "into");
};


/* module 280 */

/* Subroutine |save_for_after| puts a token on the stack for save-keeping.  */
void
save_for_after (halfword t) {
  if (cur_level > level_one) {
	check_full_save_stack;
	save_type (save_ptr) = insert_token;
	save_level (save_ptr) = level_zero;
	save_index (save_ptr) = t;
	incr (save_ptr);
  };
};

#define RESTORE_TRACE(p,arg)  if (tracing_restores > 0) { restore_trace(p,arg);}

/* module 281 */

/* The |unsave| routine goes the other way, taking items off of |save_stack|.
 * This routine takes care of restoration when a level ends; everything
 * belonging to the topmost group is cleared off of the save stack.
 */
void 
unsave (void) { 	/* pops the top level off the save stack */
  pointer p;			/* position to be restored */
  quarterword l;		/* saved level, if in fullword regions of |eqtb| */
  halfword t;			/* saved value of |cur_tok| */
  boolean a;			/* have we already processed an \.{\\aftergroup} ? */
  l = 0; /* TH -Wall */
  a = false;
  if (cur_level > level_one) {
	decr (cur_level);
	/* begin expansion of Clear off top level from |save_stack| */
	/* module 282 */
	loop {
	  decr (save_ptr);
	  if (save_type (save_ptr) == level_boundary)
		goto DONE;
	  p = save_index (save_ptr);
	  if (save_type (save_ptr) == insert_token) {
	    /* begin expansion of Insert token |p| into \TeX's input */
	    /* module 326 */
	    t = cur_tok;
	    cur_tok = p;
	    if (a) {
		  p = get_avail ();
		  info (p) = cur_tok;
		  link (p) = loc;
		  loc = p;
		  start = p;
		  if (cur_tok < right_brace_limit) {
		    if (cur_tok < left_brace_limit) {
			  decr (align_state);
			} else {
			  incr (align_state);
			}
		  }
		} else {
		  back_input();
		  a = eTeX_ex;
		};
	    cur_tok = t;
	    /* end expansion of Insert token |p| into \TeX's input */
	  } else if (save_type (save_ptr) == restore_sa) {
	    sa_restore(p,save_ptr);
	  } else {
	    if (save_type (save_ptr) == restore_old_value) {
		  l = save_level (save_ptr);
		  decr (save_ptr);
		} else {
		  save_stack[save_ptr] = eqtb[undefined_control_sequence];
		}
	    /* begin expansion of Store \(s)|save_stack[save_ptr]| in |eqtb[p]|, 
	       unless |eqtb[p]| holds a global value */
		/* module 283 */
		/* A global definition, which sets the level to |level_one|,
		 * 
		 * will not be undone by |unsave|. If at least one global definition of
		 * |eqtb[p]| has been carried out within the group that just ended, the
		 * last such definition will therefore survive.
		 */
	    if ((p < int_base) || (p > eqtb_size)) {
		  if (eq_level (p) == level_one) {
		    eq_destroy (save_stack[save_ptr]);	/* destroy the saved value */
			RESTORE_TRACE (p, "retaining");
		  } else {
		    eq_destroy (eqtb[p]);	/* destroy the current value */
		    eqtb[p] = save_stack[save_ptr];	/* restore the saved value */
			RESTORE_TRACE (p, "restoring");
		  }
		} else if (xeq_level[p] != level_one) {
		  eqtb[p] = save_stack[save_ptr];
		  xeq_level[p] = l;
		  RESTORE_TRACE (p, "restoring");
		} else {
		  RESTORE_TRACE (p, "retaining");
		};
	  };
	  /* end expansion of Store \(s)|save_stack[save_ptr]| in |eqtb[p]|, ... */
	};
  DONE:
	if (tracing_groups > 0)
	  group_trace (true);
	if (grp_stack[in_open] == cur_boundary)
	  group_warning();		/* groups possibly not properly nested with files */
	cur_group = save_level (save_ptr);
	cur_boundary = save_index (save_ptr);
	if (eTeX_ex)
	  decr (save_ptr);
	/* end expansion of Clear off top level from |save_stack| */
  } else {
	confusion ("curlevel");
  } /* |unsave| is not used when |cur_group=bottom_level| */
};


/* module 284 */
static void
restore_trace (pointer p, char *s) {
  /* |eqtb[p]| has just been restored or retained */
  begin_diagnostic();
  print_char ('{');
  zprint_string (s);
  print_char (' ');
  show_eqtb (p);
  print_char ('}');
  end_diagnostic (false);
};

/* module 272 */

/* At this time it might be a good idea for the reader to review the introduction
 * to |eqtb| that was given above just before the long lists of parameter names.
 * Recall that the ``outer level'' of the program is |level_one|, since
 * undefined control sequences are assumed to be ``defined'' at |level_zero|.
 */

void
save_initialize (void) {
  save_ptr = 0;
  cur_level = level_one;
  cur_group = bottom_level;
  cur_boundary = 0;
  max_save_stack = 0;
}
