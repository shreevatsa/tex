

#include "types.h"
#include "c-compat.h"
#include "globals.h"

/* forward */

void get_preamble_token (void);

/* module 309 */

/* The input routines must also interact with the processing of
 * \.{\\halign} and \.{\\valign}, since the appearance of tab marks and
 * \.{\\cr} in certain places is supposed to trigger the beginning of special
 * \<v_j> template text in the scanner. This magic is accomplished by an
 * |align_state| variable that is increased by~1 when a `\.{\char'173}' is
 * scanned and decreased by~1 when a `\.{\char'175}' is scanned. The |align_state|
 * is nonzero during the \<u_j> template, after which it is set to zero; the
 * \<v_j> template begins when a tab mark or \.{\\cr} occurs at a time that
 * |align_state=0|.
 */
integer align_state; /* group level with respect to current alignment */

/* module 912 */

/* 
 * It's sort of a miracle whenever \.{\\halign} and \.{\\valign} work, because
 * they cut across so many of the control structures of \TeX.
 * 
 * Therefore the
 * present page is probably not the best place for a beginner to start reading
 * this program; it is better to master everything else first.
 * 
 * Let us focus our thoughts on an example of what the input might be, in order
 * to get some idea about how the alignment miracle happens. The example doesn't
 * do anything useful, but it is sufficiently general to indicate all of the
 * special cases that must be dealt with; please do not be disturbed by its
 * apparent complexity and meaninglessness.
 * $$\vbox{\halign{\.{#}\hfil\cr
 * {}\\tabskip 2pt plus 3pt\cr
 * {}\\halign to 300pt\{u1\#v1\&\cr
 * \hskip 50pt\\tabskip 1pt plus 1fil u2\#v2\&\cr
 * \hskip 50pt u3\#v3\\cr\cr
 * \hskip 25pt a1\&\\omit a2\&\\vrule\\cr\cr
 * \hskip 25pt \\noalign\{\\vskip 3pt\}\cr
 * \hskip 25pt b1\\span b2\\cr\cr
 * \hskip 25pt \\omit\&c2\\span\\omit\\cr\}\cr}}$$
 * Here's what happens:
 * 
 * \yskip
 * (0) When `\.{\\halign to 300pt\{}' is scanned, the |scan_spec| routine
 * places the 300pt dimension onto the |save_stack|, and an |align_group|
 * code is placed above it. This will make it possible to complete the alignment
 * when the matching `\.\}' is found.
 * 
 * (1) The preamble is scanned next. Macros in the preamble are not expanded,
 * 
 * except as part of a tabskip specification. For example, if \.{u2} had been
 * a macro in the preamble above, it would have been expanded, since \TeX\
 * must look for `\.{minus...}' as part of the tabskip glue. A ``preamble list''
 * is constructed based on the user's preamble; in our case it contains the
 * following seven items:
 * $$\vbox{\halign{\.{#}\hfil\qquad&(#)\hfil\cr
 * {}\\glue 2pt plus 3pt&the tabskip preceding column 1\cr
 * {}\\alignrecord, width $-\infty$&preamble info for column 1\cr
 * {}\\glue 2pt plus 3pt&the tabskip between columns 1 and 2\cr
 * {}\\alignrecord, width $-\infty$&preamble info for column 2\cr
 * {}\\glue 1pt plus 1fil&the tabskip between columns 2 and 3\cr
 * {}\\alignrecord, width $-\infty$&preamble info for column 3\cr
 * {}\\glue 1pt plus 1fil&the tabskip following column 3\cr}}$$
 * These ``alignrecord'' entries have the same size as an |unset_node|,
 * since they will later be converted into such nodes. However, at the
 * moment they have no |type| or |subtype| fields; they have |info| fields
 * instead, and these |info| fields are initially set to the value |end_span|,
 * for reasons explained below. Furthermore, the alignrecord nodes have no
 * |height| or |depth| fields; these are renamed |u_part| and |v_part|,
 * and they point to token lists for the templates of the alignment.
 * For example, the |u_part| field in the first alignrecord points to the
 * token list `\.{u1}', i.e., the template preceding the `\.\#' for column~1.
 * 
 * (2) \TeX\ now looks at what follows the \.{\\cr} that ended the preamble.
 * It is not `\.{\\noalign}' or `\.{\\omit}', so this input is put back to
 * be read again, and the template `\.{u1}' is fed to the scanner. Just
 * before reading `\.{u1}', \TeX\ goes into restricted horizontal mode.
 * Just after reading `\.{u1}', \TeX\ will see `\.{a1}', and then (when the
 * {\.\&} is sensed) \TeX\ will see `\.{v1}'. Then \TeX\ scans an |endv|
 * token, indicating the end of a column. At this point an |unset_node| is
 * created, containing the contents of the current hlist (i.e., `\.{u1a1v1}').
 * The natural width of this unset node replaces the |width| field of the
 * alignrecord for column~1; in general, the alignrecords will record the
 * maximum natural width that has occurred so far in a given column.
 * 
 * (3) Since `\.{\\omit}' follows the `\.\&', the templates for column~2
 * are now bypassed. Again \TeX\ goes into restricted horizontal mode and
 * makes an |unset_node| from the resulting hlist; but this time the
 * hlist contains simply `\.{a2}'. The natural width of the new unset box
 * is remembered in the |width| field of the alignrecord for column~2.
 * 
 * (4) A third |unset_node| is created for column 3, using essentially the
 * mechanism that worked for column~1; this unset box contains `\.{u3\\vrule
 * v3}'. The vertical rule in this case has running dimensions that will later
 * extend to the height and depth of the whole first row, since each |unset_node|
 * in a row will eventually inherit the height and depth of its enclosing box.
 * 
 * (5) The first row has now ended; it is made into a single unset box
 * comprising the following seven items:
 * $$\vbox{\halign{\hbox to 325pt{\qquad\.{#}\hfil}\cr
 * {}\\glue 2pt plus 3pt\cr
 * {}\\unsetbox for 1 column: u1a1v1\cr
 * {}\\glue 2pt plus 3pt\cr
 * {}\\unsetbox for 1 column: a2\cr
 * {}\\glue 1pt plus 1fil\cr
 * {}\\unsetbox for 1 column: u3\\vrule v3\cr
 * {}\\glue 1pt plus 1fil\cr}}$$
 * The width of this unset row is unimportant, but it has the correct height
 * and depth, so the correct baselineskip glue will be computed as the row
 * is inserted into a vertical list.
 * 
 * (6) Since `\.{\\noalign}' follows the current \.{\\cr}, \TeX\ appends
 * additional material (in this case \.{\\vskip 3pt}) to the vertical list.
 * While processing this material, \TeX\ will be in internal vertical
 * mode, and |no_align_group| will be on |save_stack|.
 * 
 * (7) The next row produces an unset box that looks like this:
 * $$\vbox{\halign{\hbox to 325pt{\qquad\.{#}\hfil}\cr
 * {}\\glue 2pt plus 3pt\cr
 * {}\\unsetbox for 2 columns: u1b1v1u2b2v2\cr
 * {}\\glue 1pt plus 1fil\cr
 * {}\\unsetbox for 1 column: {\rm(empty)}\cr
 * {}\\glue 1pt plus 1fil\cr}}$$
 * The natural width of the unset box that spans columns 1~and~2 is stored
 * in a ``span node,'' which we will explain later; the |info| field of the
 * alignrecord for column~1 now points to the new span node, and the |info|
 * of the span node points to |end_span|.
 * 
 * (8) The final row produces the unset box
 * $$\vbox{\halign{\hbox to 325pt{\qquad\.{#}\hfil}\cr
 * {}\\glue 2pt plus 3pt\cr
 * {}\\unsetbox for 1 column: {\rm(empty)}\cr
 * {}\\glue 2pt plus 3pt\cr
 * {}\\unsetbox for 2 columns: u2c2v2\cr
 * {}\\glue 1pt plus 1fil\cr}}$$
 * A new span node is attached to the alignrecord for column 2.
 * 
 * (9) The last step is to compute the true column widths and to change all the
 * unset boxes to hboxes, appending the whole works to the vertical list that
 * encloses the \.{\\halign}. The rules for deciding on the final widths of
 * each unset column box will be explained below.
 * 
 * \yskip\noindent
 * Note that as \.{\\halign} is being processed, we fearlessly give up control
 * to the rest of \TeX. At critical junctures, an alignment routine is
 * called upon to step in and do some little action, but most of the time
 * these routines just lurk in the background. It's something like
 * post-hypnotic suggestion.
 */

/* module 913 */

/* We have mentioned that alignrecords contain no |height| or |depth| fields.
 * Their |glue_sign| and |glue_order| are pre-empted as well, since it
 * is necessary to store information about what to do when a template ends.
 * This information is called the |extra_info| field.
 */
#define u_part( arg )  mem [ arg  +  height_offset ]. cint
#define v_part( arg )  mem [ arg  +  depth_offset ]. cint
#define extra_info( arg )  info ( arg  +  list_offset )


/* module 914 */

/* Alignments can occur within alignments, so a small stack is used to access
 * the alignrecord information. At each level we have a |preamble| pointer,
 * indicating the beginning of the preamble list; a |cur_align| pointer,
 * indicating the current position in the preamble list; a |cur_span| pointer,
 * indicating the value of |cur_align| at the beginning of a sequence of
 * spanned columns; a |cur_loop| pointer, indicating the tabskip glue before
 * an alignrecord that should be copied next if the current list is extended;
 * and the |align_state| variable, which indicates the nesting of braces so
 * that \.{\\cr} and \.{\\span} and tab marks are properly intercepted.
 * There also are pointers |cur_head| and |cur_tail| to the head and tail
 * of a list of adjustments being moved out from horizontal mode to
 * vertical~mode.
 * 
 * The current values of these seven quantities appear in global variables;
 * when they have to be pushed down, they are stored in 5-word nodes, and
 * |align_ptr| points to the topmost such node.
 */
pointer cur_align;/* current position in preamble list */
pointer cur_span;/* start of currently spanned columns in preamble list */
pointer cur_loop;/* place to copy when extending a periodic preamble */
pointer align_ptr;/* most recently pushed-down alignment stack node */
pointer cur_head, cur_tail;/* adjustment list pointers */
pointer cur_pre_head, cur_pre_tail;/* pre-adjustment list pointers */

/* module 915 */

/* The |align_state| and |preamble| variables are initialized elsewhere. */
void align_initialize (void) {
  align_state = 1000000;
  align_ptr = null;
  cur_align = null;
  cur_span = null;
  cur_loop = null;
  cur_head = null;
  cur_tail = null;
  cur_pre_head = null;
  cur_pre_tail = null;
}

/* module 916 */

/* Alignment stack maintenance is handled by a pair of trivial routines
 * called |push_alignment| and |pop_alignment|.
 */
void 
push_alignment (void) {
  pointer p;  /* the new alignment stack node */ 
  p = get_node (align_stack_node_size);
  link (p) = align_ptr;
  info (p) = cur_align;
  llink (p) = preamble;
  rlink (p) = cur_span;
  mem[p + 2].cint = cur_loop;
  mem[p + 3].cint = align_state;
  info (p + 4) = cur_head;
  link (p + 4) = cur_tail;
  info (p + 5) = cur_pre_head;
  link (p + 5) = cur_pre_tail;
  align_ptr = p;
  cur_head = get_avail();
};


void 
pop_alignment (void) {
  pointer p;  /* the top alignment stack node */ 
  free_avail (cur_head);
  p = align_ptr;
  cur_tail = link (p + 4);
  cur_head = info (p + 4);
  cur_pre_tail = link (p + 5);
  cur_pre_head = info (p + 5);
  align_state = mem[p + 3].cint;
  cur_loop = mem[p + 2].cint;
  cur_span = rlink (p);
  preamble = llink (p);
  cur_align = info (p);
  align_ptr = link (p);
  free_node (p, align_stack_node_size);
};

/* module 917 */

/* \TeX\ has eight procedures that govern alignments: |init_align| and
 * |fin_align| are used at the very beginning and the very end; |init_row| and
 * |fin_row| are used at the beginning and end of individual rows; |init_span|
 * is used at the beginning of a sequence of spanned columns (possibly involving
 * only one column); |init_col| and |fin_col| are used at the beginning and
 * end of individual columns; and |align_peek| is used after \.{\\cr} to see
 * whether the next item is \.{\\noalign}.
 * 
 * We shall consider these routines in the order they are first used during
 * the course of a complete \.{\\halign}, namely |init_align|, |align_peek|,
 * |init_row|, |init_span|, |init_col|, |fin_col|, |fin_row|, |fin_align|.
 */

/* module 918 */

/* When \.{\\halign} or \.{\\valign} has been scanned in an appropriate
 * mode, \TeX\ calls |init_align|, whose task is to get everything off to a
 * good start. This mostly involves scanning the preamble and putting its
 * information into the preamble list.
 */

void init_align (void) {
  pointer save_cs_ptr; /* |warning_index| value for error messages */ 
  pointer p; /* for short-term temporary use */ 
  save_cs_ptr = cur_cs; /* \.{\\halign} or \.{\\valign}, usually */ 
  push_alignment();
  align_state = -1000000; /* enter a new alignment level */
  /* begin expansion of Check for improper alignment in displayed math */
  /* module 920 */
  /* When \.{\\halign} is used as a displayed formula, there should be
   * no other pieces of mlists present.
   */
  if ((mode == mmode) && ((tail != head) || (incompleat_noad != null))) {
    print_err ("Improper ");
    print_esc_string ("halign");
    zprint_string(" inside $$'s");
    help3  ("Displays can use special alignments (like \\eqalignno)",
	    "only if nothing but the alignment itself is between $$'s.",
	    "So I've deleted the formulas that preceded this alignment.");
    error();
    flush_math();
  };
  /* end expansion of Check for improper alignment in displayed math */
  push_nest();  /* enter a new semantic level */
  /* begin expansion of Change current mode to |-vmode| for \.{\\halign}, |-hmode| for \.{\\valign} */
  /* module 919 */
  /* In vertical modes, |prev_depth| already has the correct value. But
   * if we are in |mmode| (displayed formula mode), we reach out to the
   * enclosing vertical mode for the |prev_depth| value that produces the
   * correct baseline calculations.
   */
  if (mode == mmode) {
    mode = -vmode;
    prev_depth = nest[nest_ptr - 2].aux_field.sc;
  }
  else if (mode > 0) {
    negate (mode);
  }
  /* end expansion of Change current mode to |-vmode| for \.{\\halign}, |-hmode| for \.{\\valign} */
  scan_spec (align_group, false);
  /* begin expansion of Scan the preamble and record it in the |preamble| list */
  /* module 921 */
  preamble = null;
  cur_align = align_head;
  cur_loop = null;
  scanner_status = aligning;
  warning_index = save_cs_ptr;
  align_state = -1000000; /* at this point, |cur_cmd=left_brace| */ 
  loop {
    /* begin expansion of Append the current tabskip glue to the preamble list */
    /* module 922 */
    link (cur_align) = new_param_glue (tab_skip_code);
    cur_align = link (cur_align);
	/* end expansion of Append the current tabskip glue to the preamble list */
    if (cur_cmd == car_ret)
      goto DONE;    /* \.{\\cr} ends the preamble */
    /* begin expansion of Scan preamble text until |cur_cmd| is |tab_mark| or |car_ret|, 
       looking for changes in the tabskip glue; append an alignrecord to the preamble list */
    /* module 923 */
    /* begin expansion of Scan the template \<u_j>, putting the resulting token list in |hold_head| */
    /* module 927 */
    /* Spaces are eliminated from the beginning of a template.
     */
    p = hold_head;
    link (p) = null;
    loop {
      get_preamble_token();
      if (cur_cmd == mac_param)
		goto DONE1;
      if ((cur_cmd <= car_ret) && (cur_cmd >= tab_mark) && (align_state == -1000000))
		if ((p == hold_head) && (cur_loop == null) && (cur_cmd == tab_mark)) {
		  cur_loop = cur_align;
		} else {
		  print_err ("Missing # inserted in alignment preamble");
		  help3 ("There should be exactly one # between &'s, when an",
				 "\\halign or \\valign is being set up. In this case you had",
				 "none, so I've put one in; maybe that will work.");
		  back_error();
		  goto DONE1;
		} else if ((cur_cmd != spacer) || (p != hold_head)) {
		  link (p) = get_avail();
		  p = link (p);
		  info (p) = cur_tok;
		};
    };
  DONE1:
    /* end expansion of Scan the template \<u_j>, putting the resulting token list in |hold_head| */
    link (cur_align) = new_null_box();
    cur_align = link (cur_align); /* a new alignrecord */ 
    info (cur_align) = end_span;
    width (cur_align) = null_flag;
    u_part (cur_align) = link (hold_head);
    /* begin expansion of Scan the template \<v_j>, putting the resulting token list in |hold_head| */
    /* module 928 */
    p = hold_head;
    link (p) = null;
    loop {
    CONTINUE:
      get_preamble_token();
      if ((cur_cmd <= car_ret) && (cur_cmd >= tab_mark) && (align_state == -1000000))
	goto DONE2;
      if (cur_cmd == mac_param) {
	print_err ("Only one # is allowed per tab");
	help3  ("There should be exactly one # between &'s, when an",
		"\\halign or \\valign is being set up. In this case you had",
		"more than one, so I'm ignoring all but the first.");
	error();
	goto CONTINUE;
      };
      link (p) = get_avail();
      p = link (p);
      info (p) = cur_tok;
    };
  DONE2:
    link (p) = get_avail();
    p = link (p);
    info (p) = end_template_token;	/* put \.{\\endtemplate} at the end */
    /* end expansion of Scan the template \<v_j>, putting the resulting token list in |hold_head| */
    v_part (cur_align) = link (hold_head);
    /* end expansion of Scan preamble text until |cur_cmd| is |tab_mark| or |car_ret|,...*/
  };
 DONE:
  scanner_status = normal;
  /* end expansion of Scan the preamble and record it in the |preamble| list */
  new_save_level (align_group);
  if (every_cr != null)
    begin_token_list (every_cr, every_cr_text);
  align_peek(); /* look for \.{\\noalign} or \.{\\omit} */ 
};

/* module 926 */

/* The preamble is copied directly, except that \.{\\tabskip} causes a change
 * to the tabskip glue, thereby possibly expanding macros that immediately
 * follow it. An appearance of \.{\\span} also causes such an expansion.
 * 
 * Note that if the preamble contains `\.{\\global\\tabskip}', the `\.{\\global}'
 * token survives in the preamble and the `\.{\\tabskip}' defines new
 * tabskip glue (locally).
 */
void 
get_preamble_token (void) {
 RESTART:
  get_token();
  while ((cur_chr == span_code) && (cur_cmd == tab_mark)) {
    get_token();/* this token will be expanded once */ 
    if (cur_cmd > max_command) {
      expand();
      get_token();
    };
  };
  if (cur_cmd == endv)
    fatal_error ("(interwoven alignment preambles are not allowed)");
  if ((cur_cmd == assign_glue) && (cur_chr == glue_base + tab_skip_code)) {
    scan_optional_equals();
    scan_glue (glue_val);
    if (global_defs > 0) {
      geq_define (glue_base + tab_skip_code, glue_ref, cur_val);
    } else {
      eq_define (glue_base + tab_skip_code, glue_ref, cur_val);
    }
    goto RESTART;
  };
};

/* module 930 */

/* To start a row (i.e., a `row' that rhymes with `dough' but not with `bough'),
 * we enter a new semantic level, copy the first tabskip glue, and change
 * from internal vertical mode to restricted horizontal mode or vice versa.
 * The |space_factor| and |prev_depth| are not used on this semantic level,
 * but we clear them to zero just to be tidy.
 */

/* module 931 */

/* The parameter to |init_span| is a pointer to the alignrecord where the
 * next column or group of columns will begin. A new semantic level is
 * entered, so that the columns will generate a list for subsequent packaging.
 */

void 
init_span (pointer p) {
  push_nest();
  if (mode == -hmode) {
    space_factor = 1000;
  } else {
    prev_depth = ignore_depth;
    normal_paragraph();
  };
  cur_span = p;
};

void 
init_row (void) {
  push_nest();
  mode = (-hmode - vmode) - mode;
  if (mode == -hmode) {
    space_factor = 0;
  } else {
    prev_depth = 0;
  }
  tail_append (new_glue (glue_ptr (preamble)));
  subtype (tail) = tab_skip_code + 1;
  cur_align = link (preamble);
  cur_tail = cur_head;
  cur_pre_tail = cur_pre_head;
  init_span (cur_align);
};


/* module 932 */

/* When a column begins, we assume that |cur_cmd| is either |omit| or else
 * the current token should be put back into the input until the \<u_j>
 * template has been scanned. (Note that |cur_cmd| might be |tab_mark| or
 * |car_ret|.) We also assume that |align_state| is approximately 1000000 at
 * this time. We remain in the same mode, and start the template if it is
 * called for.
 */
void 
init_col (void) {
  extra_info (cur_align) = cur_cmd;
  if (cur_cmd == omit) {
    align_state = 0;
  } else {
    back_input();
    begin_token_list (u_part (cur_align), u_template);
  };  /* now |align_state=1000000| */ 
};


/* module 933 */

/* The scanner sets |align_state| to zero when the \<u_j> template ends. When
 * a subsequent \.{\\cr} or \.{\\span} or tab mark occurs with |align_state=0|,
 * the scanner activates the following code, which fires up the \<v_j> template.
 * We need to remember the |cur_chr|, which is either |cr_cr_code|, |cr_code|,
 * |span_code|, or a character code, depending on how the column text has ended.
 * 
 * This part of the program had better not be activated when the preamble
 * to another alignment is being scanned, or when no alignment preamble is active.
 */

void 
insert_v_part (void) {
  if ((scanner_status == aligning) || (cur_align == null))
	fatal_error ("(interwoven alignment preambles are not allowed)");
  cur_cmd = extra_info (cur_align);
  extra_info (cur_align) = cur_chr;
  if (cur_cmd == omit) {
	begin_token_list (omit_template, v_template);
  } else {
	begin_token_list (v_part (cur_align), v_template);
  };
  align_state = 1000000;
}

/* module 935 */

/* When the |endv| command at the end of a \<v_j> template comes through the
 * scanner, things really start to happen; and it is the |fin_col| routine
 * that makes them happen. This routine returns |true| if a row as well as a
 * column has been finished.
 */
boolean 
fin_col (void) {
  pointer p; /* the alignrecord after the current one */ 
  pointer q, r; /* temporary pointers for list manipulation */ 
  pointer s; /* a new span node */ 
  pointer u; /* a new unset box */ 
  scaled w; /* natural width */ 
  glue_ord o; /* order of infinity */ 
  halfword n; /* span counter */ 
  if (cur_align == null)
    confusion ("endv");
  q = link (cur_align);
  if (q == null)
    confusion ("endv");
  if (align_state < 500000)
    fatal_error  ("(interwoven alignment preambles are not allowed)");
  p = link (q);
  /* begin expansion of If the preamble list has been traversed, check that the row has ended */
  /* module 936 */
  if ((p == null) && (extra_info (cur_align) < cr_code)) {
    if (cur_loop != null) {
      /* begin expansion of Lengthen the preamble periodically */
      /* module 937 */
      link (q) = new_null_box();
      p = link (q); /* a new alignrecord */ 
      info (p) = end_span;
      width (p) = null_flag;
      cur_loop = link (cur_loop);
      /* begin expansion of Copy the templates from node |cur_loop| into node |p| */
      /* module 938 */
      q = hold_head;
      r = u_part (cur_loop);
      while (r != null) {
		link (q) = get_avail();
		q = link (q);
		info (q) = info (r);
		r = link (r);
      };
      link (q) = null;
      u_part (p) = link (hold_head);
      q = hold_head;
      r = v_part (cur_loop);
      while (r != null) {
		link (q) = get_avail();
		q = link (q);
		info (q) = info (r);
		r = link (r);
      };
      link (q) = null;
      v_part (p) = link (hold_head);
      /* end expansion of Copy the templates from node |cur_loop| into node |p| */	    
      cur_loop = link (cur_loop);
      link (p) = new_glue (glue_ptr (cur_loop));
      /* end expansion of Lengthen the preamble periodically */
      } else {
		print_err ("Extra alignment tab has been changed to ");
		print_esc_string ("cr");
		help3 ("You have given more \\span or & marks than there were",
			   "in the preamble to the \\halign or \\valign now in progress.",
			   "So I'll assume that you meant to type \\cr instead.");
		extra_info (cur_align) = cr_code;
		error();
      }
    /* end expansion of If the preamble list has been traversed, check that the row has ended */
  };
  if (extra_info (cur_align) != span_code) {
    unsave();
    new_save_level (align_group);
    /* begin expansion of Package an unset box for the current column and record its width */
    /* module 940 */
    if (mode == -hmode) {
      adjust_tail = cur_tail;
      pre_adjust_tail = cur_pre_tail;
      u = hpack (link (head), 0, additional);
      w = width (u);
      cur_tail = adjust_tail;
      adjust_tail = null;
      cur_pre_tail = pre_adjust_tail;
      pre_adjust_tail = null;
    } else {
      u = vpackage (link (head), 0, additional, 0);
      w = height (u);
    };
    n = min_quarterword; /* this represents a span count of 1 */ 
    if (cur_span !=cur_align) {
      /* begin expansion of Update width entry for spanned columns */
      /* module 942 */
      q = cur_span;
      do {
	incr (n);
	q = link (link (q));
      } while (q != cur_align);
      if (n > max_quarterword)
	confusion ("256 spans");	/* this can happen, but won't */ 
      q = cur_span;
      while (link (info (q)) < n)
	q = info (q);
      if (link (info (q)) > n) {
	s = get_node (span_node_size);
	info (s) = info (q);
	link (s) = n;
	info (q) = s;
	width (s) = w;
      } else if (width (info (q)) < w) {
	width (info (q)) = w;
      }
      /* end expansion of Update width entry for spanned columns */
    } else if (w > width (cur_align)) {
      width (cur_align) = w;
    }
    type (u) = unset_node;
    span_count (u) = n;
    /* Determine the stretch order */
	determine_stretch_order;
    glue_order (u) = o;
    glue_stretch (u) = total_stretch[o];
    /* Determine the shrink order */
    determine_shrink_order;
    glue_sign (u) = o;
    glue_shrink (u) = total_shrink[o];
    pop_nest();
    link (tail) = u;
    tail = u;
    /* end expansion of Package an unset box for the current column and record its width */
    /* begin expansion of Copy the tabskip glue between columns */
    /* module 939 */
    tail_append (new_glue (glue_ptr (link (cur_align))));
    subtype (tail) = tab_skip_code + 1;
    /* end expansion of Copy the tabskip glue between columns */
    if (extra_info (cur_align) >= cr_code){
      return true;
    };
    init_span (p);
  };
  align_state = 1000000;
  do {
    get_x_or_protected();
  } while (cur_cmd == spacer);
  cur_align = p;
  init_col();
  return false;
};


/* module 943 */

/* At the end of a row, we append an unset box to the current vlist (for
 * \.{\\halign}) or the current hlist (for \.{\\valign}). This unset box
 * contains the unset boxes for the columns, separated by the tabskip glue.
 * Everything will be set later.
 */
void 
fin_row (void) {
  pointer p; /* the new unset box */ 
  if (mode == -hmode) {
    p = hpack (link (head), 0, additional);
    pop_nest();
    if (cur_pre_head != cur_pre_tail)
      append_list (cur_pre_head,cur_pre_tail);
    append_to_vlist (p);
    if (cur_head != cur_tail)
      append_list (cur_head,cur_tail);
  } else {
    p = VPACK (link (head), 0, additional);
    pop_nest();
    link (tail) = p;
    tail = p;
    space_factor = 1000;
  };
  type (p) = unset_node;
  glue_stretch (p) = 0;
  if (every_cr != null)
    begin_token_list (every_cr, every_cr_text);
  align_peek();
}; /* note that |glue_shrink(p)=0| since |glue_shrink==shift_amount| */

/* module 944 */

/* Finally, we will reach the end of the alignment, and we can breathe a
 * sigh of relief that memory hasn't overflowed. All the unset boxes will now be
 * set so that the columns line up, taking due account of spanned columns.
 */
void 
fin_align (void) {
  pointer p, q, r, s, u, v; /* registers for the list operations */ 
  scaled t, w; /* width of column */ 
  scaled o; /* shift offset for unset boxes */ 
  halfword n; /* matching span amount */ 
  scaled rule_save; /* temporary storage for |overfull_rule| */ 
  memory_word aux_save; /* temporary storage for |aux| */ 
  if (cur_group != align_group)
    confusion ("align1");
  unsave(); /* that |align_group| was for individual entries */
  if (cur_group != align_group)
    confusion ("align0");
  unsave(); /* that |align_group| was for the whole alignment */
  if (nest[nest_ptr - 1].mode_field == mmode) {
    o = display_indent;
  } else {
    o = 0;
  }
  /* begin expansion of Go through the preamble list, determining the column widths 
     and changing the alignrecords to dummy unset boxes */
  /* module 945 */
  /* It's time now to dismantle the preamble list and to compute the column
   * widths. Let $w_{ij}$ be the maximum of the natural widths of all entries
   * that span columns $i$ through $j$, inclusive. The alignrecord for column~$i$
   * contains $w_{ii}$ in its |width| field, and there is also a linked list of
   * the nonzero $w_{ij}$ for increasing $j$, accessible via the |info| field;
   * these span nodes contain the value $j-i-1+|min_quarterword|$ in their
   * |link| fields. The values of $w_{ii}$ were initialized to |null_flag|, which
   * we regard as $-\infty$.
   * 
   * The final column widths are defined by the formula
   * $$w_j=\max_{1\L i\L j}\biggl( w_{ij}-\sum_{i\L k<j}(t_k+w_k)\biggr),$$
   * where $t_k$ is the natural width of the tabskip glue between columns
   * $k$ and~$k+1$. However, if $w_{ij}=-\infty$ for all |i| in the range
   * |1<=i<=j| (i.e., if every entry that involved column~|j| also involved
   * column~|j+1|), we let $w_j=0$, and we zero out the tabskip glue after
   * column~|j|.
   * 
   * \TeX\ computes these values by using the following scheme: First $w_1=w_{11}$.
   * Then replace $w_{2j}$ by $\max(w_{2j},w_{1j}-t_1-w_1)$, for all $j>1$.
   * Then $w_2=w_{22}$. Then replace $w_{3j}$ by $\max(w_{3j},w_{2j}-t_2-w_2)$
   * for all $j>2$; and so on. If any $w_j$ turns out to be $-\infty$, its
   * value is changed to zero and so is the next tabskip.
   */
  q = link (preamble);
  do {
    flush_list (u_part (q));
    flush_list (v_part (q));
    p = link (link (q));
    if (width (q) == null_flag) {
      /* begin expansion of Nullify |width(q)| and the tabskip glue following this column */
      /* module 946 */
      width (q) = 0;
      r = link (q);
      s = glue_ptr (r);
      if (s != zero_glue) {
		add_glue_ref (zero_glue);
		delete_glue_ref (s);
		glue_ptr (r) = zero_glue;
      };
    };
    /* end expansion of Nullify |width(q)| and the tabskip glue following this column */
    if (info (q) != end_span) {
      /* begin expansion of Merge the widths in the span nodes of |q| with those of
		 |p|, destroying the span nodes of |q| */
      /* module 947 */
      /* Merging of two span-node lists is a typical exercise in the manipulation of
       * linearly linked data structures. The essential invariant in the following
       * |repeat| loop is that we want to dispense with node |r|, in |q|'s list,
       * and |u| is its successor; all nodes of |p|'s list up to and including |s|
       * have been processed, and the successor of |s| matches |r| or precedes |r|
       * or follows |r|, according as |link(r)=n| or |link(r)>n| or |link(r)<n|.
       */
      t = width (q) + width (glue_ptr (link (q)));
      r = info (q);
      s = end_span;
      info (s) = p;
      n = min_quarterword + 1;
      do {
		width (r) = width (r) - t;
		u = info (r);
		while (link (r) > n) {
		  s = info (s);
		  n = link (info (s)) + 1;
		};
		if (link (r) < n) {
		  info (r) = info (s);
		  info (s) = r;
		  decr (link (r));
		  s = r;
		} else {
		  if (width (r) > width (info (s)))
			width (info (s)) = width (r);
		  free_node (r, span_node_size);
		};
		r = u;
      } while (r != end_span);
    };
    /* end expansion of Merge the widths in the span nodes of |q| with those of |p|, destroying the span nodes of |q| */
    type (q) = unset_node;
    span_count (q) = min_quarterword;
    height (q) = 0;
    depth (q) = 0;
    glue_order (q) = normal;
    glue_sign (q) = normal;
    glue_stretch (q) = 0;
    glue_shrink (q) = 0;
    q = p;
  } while (q != null);
  /* end expansion of Go through the preamble list, determining the column 
     widths and changing the alignrecords to dummy unset boxes */
  /* begin expansion of Package the preamble list, to determine the actual tabskip glue 
     amounts, and let |p| point to this prototype box */
  /* module 948 */
  /* Now the preamble list has been converted to a list of alternating unset
   * boxes and tabskip glue, where the box widths are equal to the final
   * column sizes. In case of \.{\\valign}, we change the widths to heights,
   * so that a correct error message will be produced if the alignment is
   * overfull or underfull.
   */
  save_ptr = save_ptr - 2;
  pack_begin_line = -mode_line;
  if (mode == -vmode) {
    rule_save = overfull_rule;
    overfull_rule = 0; /* prevent rule from being packaged */ 
    p = hpack (preamble, saved (1), saved (0));
    overfull_rule = rule_save;
  } else {
    q = link (preamble);
    do {
      height (q) = width (q);
      width (q) = 0;
      q = link (link (q));
    } while (q != null);
    p = VPACK (preamble, saved (1), saved (0));
    q = link (preamble);
    do {
      width (q) = height (q);
      height (q) = 0;
      q = link (link (q));
    } while (q != null);
  };
  pack_begin_line = 0;
  /* end expansion of Package the preamble list, to determine the actual ...*/
  /* begin expansion of Set the glue in all the unset boxes of the current list */
  /* module 949 */
  q = link (head);
  s = head;
  while (q != null) {
    if (!is_char_node (q)) {
      if (type (q) == unset_node) {
		/* begin expansion of Set the unset box |q| and the unset boxes in it */
		/* module 951 */
		/* The unset box |q| represents a row that contains one or more unset boxes,
		 * depending on how soon \.{\\cr} occurred in that row.
		 */
		if (mode == -vmode) {
		  type (q) = hlist_node;
		  width (q) = width (p);
		  if (nest[nest_ptr - 1].mode_field == mmode)
			subtype (q) = dlist; /* for |ship_out| */ 
		} else {
		  type (q) = vlist_node;
		  height (q) = height (p);
		};
		glue_order (q) = glue_order (p);
		glue_sign (q) = glue_sign (p);
		glue_set (q) = glue_set (p);
		shift_amount (q) = o;
		r = link (list_ptr (q));
		s = link (list_ptr (p));
		do {
		  /* begin expansion of Set the glue in node |r| and change it from an unset node */
		  /* module 952 */
		  /* A box made from spanned columns will be followed by tabskip glue nodes and
		   * by empty boxes as if there were no spanning. This permits perfect alignment
		   * of subsequent entries, and it prevents values that depend on floating point
		   * arithmetic from entering into the dimensions of any boxes.
		   */
		  n = span_count (r);
		  t = width (s);
		  w = t;
		  u = hold_head;
		  subtype (r) = min_quarterword; /* for |ship_out| */ 
		  while (n > min_quarterword) {
			decr (n);
			/* begin expansion of Append tabskip glue and an empty box to list |u|, and 
			   update |s| and |t| as the prototype nodes are passed */
			/* module 953 */
			s = link (s);
			v = glue_ptr (s);
			link (u) = new_glue (v);
			u = link (u);
			subtype (u) = tab_skip_code + 1;
			t = t + width (v);
			if (glue_sign (p) == stretching) {
			  if (stretch_order (v) == glue_order (p))
				t =t + round (zfloat (glue_set (p)) * stretch (v));
			} else if (glue_sign (p) == shrinking) {
			  if (shrink_order (v) == glue_order (p))
				t = t -  round (zfloat (glue_set (p)) * shrink (v));
			};
			s = link (s);
			link (u) = new_null_box();
			u = link (u);
			t = t + width (s);
			if (mode == -vmode){
			  width (u) = width (s);
			} else {
			  type (u) = vlist_node;
			  height (u) = width (s);
			}
			/* end expansion of Append tabskip glue and an empty box to list |u|, and update |s|..*/
		  };
		  if (mode == -vmode) {
			/* begin expansion of Make the unset node |r| into an |hlist_node| of width |w|,
			   setting the glue as if the width were |t| */
			/* module 954 */
			height (r) = height (q);
			depth (r) = depth (q);
			if (t == width (r)) {
			  glue_sign (r) = normal;
			  glue_order (r) = normal;
			  set_glue_ratio_zero (glue_set (r));
			}  else if (t > width (r)) {
			  glue_sign (r) = stretching;
			  if (glue_stretch (r) == 0) {
				set_glue_ratio_zero (glue_set (r));
			  } else {
				glue_set (r) = unfloat ((t - width (r)) / (double) glue_stretch (r));
			  }
			} else {
			  glue_order (r) = glue_sign (r);
			  glue_sign (r) = shrinking;
			  if (glue_shrink (r) == 0) {
				set_glue_ratio_zero (glue_set (r));
			  } else if ((glue_order (r) == normal) && (width (r) - t > glue_shrink (r))) {
				set_glue_ratio_one (glue_set (r));
			  } else {
				glue_set (r) = unfloat ((width (r) -t) / (double)  glue_shrink (r));
			  }
			};
			width (r) = w;
			type (r) = hlist_node;
			/* end expansion of Make the unset node |r| into an |hlist_node| of width  ...*/
		  } else {
			/* begin expansion of Make the unset node |r| into a |vlist_node| of height 
			   |w|, setting the glue as if the height were |t| */
			/* module 955 */
			width (r) = width (q);
			if (t == height (r)) {
			  glue_sign (r) = normal;
			  glue_order (r) = normal;
			  set_glue_ratio_zero (glue_set (r));
			} else if (t > height (r)) {
			  glue_sign (r) = stretching;
			  if (glue_stretch (r) == 0) {
				set_glue_ratio_zero (glue_set (r));
			  } else {
				glue_set (r) = unfloat ((t -height (r)) / (double)  glue_stretch (r));
			  }
			} else {
			  glue_order (r) = glue_sign (r);
			  glue_sign (r) = shrinking;
			  if (glue_shrink (r) == 0) {
				set_glue_ratio_zero (glue_set (r));
			  } else if ((glue_order (r) == normal) && (height (r) - t > glue_shrink (r))) {
				set_glue_ratio_one (glue_set (r));
			  } else {
				glue_set (r) = unfloat ((height (r) - t) / (double) glue_shrink (r));
			  }
			};
			height (r) = w;
			type (r) = vlist_node;
		  };
		  /* end expansion of Make the unset node |r| into a |vlist_node| of height |w|, .. */
		  shift_amount (r) = 0;
		  if (u != hold_head) {	/* append blank boxes to account for spanned nodes */
			link (u) = link (r);
			link (r) = link (hold_head);
			r = u;
		  };
		  /* end expansion of Set the glue in node |r| and change it from an unset node */
		  r = link (link (r));
		  s = link (link (s));
		} while (r != null);
		/* end expansion of 951 */
      } else if (type (q) == rule_node) {
		/* begin expansion of Make the running dimensions in rule |q| extend to the 
		   boundaries of the alignment */
		/* module 950 */
		if (is_running (width (q)))
		  width (q) = width (p);
		if (is_running (height (q)))
		  height (q) = height (p);
		if (is_running (depth (q)))
		  depth (q) = depth (p);
		if (o != 0) {
		  r = link (q);
		  link (q) = null;
		  q = hpack (q, 0, additional);
		  shift_amount (q) = o;
		  link (q) = r;
		  link (s) = q;
		};
      }
      /* end expansion of Make the running dimensions in rule |q| extend to .. */
    };
    s = q;
    q = link (q);
  };
  /* end expansion of Set the glue in all the unset boxes of the current list */
  flush_node_list (p);
  pop_alignment();
  /* begin expansion of Insert the \(c)current list into its environment */
  /* module 956 */
  /* We now have a completed alignment, in the list that starts at |head|
   * and ends at |tail|. This list will be merged with the one that encloses
   * it. (In case the enclosing mode is |mmode|, for displayed formulas,
   * we will need to insert glue before and after the display; that part of the
   * program will be deferred until we're more familiar with such operations.)
   * 
   * In horizontal mode, the |clang| part of |aux| is undefined; an over-cautious
   * \PASCAL\ runtime system may complain about this.
   */
  aux_save = aux;
  p = link (head);
  q = tail;
  pop_nest();
  if (mode == mmode) {
    /* begin expansion of Finish an alignment in a display */
    /* module 1351 */
    /* When \.{\\halign} appears in a display, the alignment routines operate
     * essentially as they do in vertical mode. Then the following program is
     * activated, with |p| and |q| pointing to the beginning and end of the
     * resulting list, and with |aux_save| holding the |prev_depth| value.
     */
    do_assignments();
    if (cur_cmd != math_shift) {
      /* begin expansion of Pontificate about improper alignment in display */
      /* module 1352 */
      print_err ("Missing $$ inserted");
      help2 ("Displays can use special alignments (like \\eqalignno)",
			 "only if nothing but the alignment itself is between $$'s.");
      back_error();
      /* end expansion of Pontificate about improper alignment in display */
    } else {
      /* Check that another \.\$ follows */
	  check_for_dollar;
    };
    flush_node_list (LR_box);
    pop_nest();
    tail_append (new_penalty (pre_display_penalty));
    tail_append (new_param_glue (above_display_skip_code));
    link (tail) = p;
    if (p != null)
      tail = q;
    tail_append (new_penalty (post_display_penalty));
    tail_append (new_param_glue (below_display_skip_code));
    prev_depth = aux_save.sc;
    resume_after_display();
    /* end expansion of Finish an alignment in a display */
  } else {
    aux = aux_save;
    link (tail) = p;
    if (p != null)
      tail = q;
    if (mode == vmode)
      build_page();
  };
  /* end expansion of Insert the \(c)current list into its environment */
}

/* module 929 */

/* The tricky part about alignments is getting the templates into the
 * scanner at the right time, and recovering control when a row or column
 * is finished.
 * 
 * We usually begin a row after each \.{\\cr} has been sensed, unless that
 * \.{\\cr} is followed by \.{\\noalign} or by the right brace that terminates
 * the alignment. The |align_peek| routine is used to look ahead and do
 * the right thing; it either gets a new row started, or gets a \.{\\noalign}
 * started, or finishes off the alignment.
 */
void 
align_peek (void) {
 RESTART:
  align_state = 1000000;
  do {
    get_x_or_protected();
  } while (cur_cmd == spacer);
  if (cur_cmd == no_align) {
    scan_left_brace();
    new_save_level (no_align_group);
    if (mode == -vmode)
      normal_paragraph();
  } else if (cur_cmd == right_brace) {
    fin_align();
  } else if ((cur_cmd == car_ret) && (cur_chr == cr_cr_code)) {
    goto RESTART;	/* ignore \.{\\crcr} */
  } else {
    init_row(); /* start a new row */ 
    init_col(); /* start a new column and replace what we peeked at */ 
  };
};


/* module 1272 */

void 
align_error (void) {
  if (abs (align_state) > 2) {
    /* begin expansion of Express consternation over the fact that no alignment is in progress */
    /* module 1273 */
    print_err ("Misplaced ");
    print_cmd_chr (cur_cmd, cur_chr);
    if (cur_tok == tab_token + '&') {
      help6 ("I can't figure out why you would want to use a tab mark",
	     "here. If you just want an ampersand, the remedy is",
	     "simple: Just type `I\\&' now. But if some right brace",
	     "up above has ended a previous alignment prematurely,",
	     "you're probably due for more error messages, and you",
	     "might try typing `S' now just to see what is salvageable.");
    } else {
      help5 ("I can't figure out why you would want to use a tab mark",
	     "or \\cr or \\span just now. If something like a right brace",
	     "up above has ended a previous alignment prematurely,",
	     "you're probably due for more error messages, and you",
	     "might try typing `S' now just to see what is salvageable.");
    };
    error();
    /* end expansion of Express consternation over the fact that no alignment is in progress */
  } else {
    back_input();
    if (align_state < 0) {
      print_err ("Missing { inserted");
      incr (align_state);
      cur_tok = left_brace_token + '{';
    } else {
      print_err ("Missing } inserted");
      decr (align_state);
      cur_tok = right_brace_token + '}';
    };
    help3  ("I've put in what seems to be necessary to fix",
	    "the current column of the current alignment.",
	    "Try to go on, since this might almost work.");
    ins_error();
  };
};

/* module  1274 */

/* The help messages here contain a little white lie, since \.{\\noalign}
 * and \.{\\omit} are allowed also after `\.{\\noalign\{...\}}'.
 */
void 
no_align_error (void) {
  print_err ("Misplaced ");
  print_esc_string ("noalign");
  help2 ("I expect to see \\noalign only after the \\cr of",
	 "an alignment. Proceed, and I'll ignore this case.");
  error();
};

void 
omit_error (void) {
  print_err ("Misplaced ");
  print_esc_string ("omit");
  help2 ("I expect to see \\omit only after tab marks or the \\cr of",
	 "an alignment. Proceed, and I'll ignore this case.");
  error();
};

/* module 1276 */

/* We've now covered most of the abuses of \.{\\halign} and \.{\\valign}.
 * Let's take a look at what happens when they are used correctly.
 */

void do_endv (void) {
  base_ptr = input_ptr;
  input_stack[base_ptr] = cur_input;
  while ((input_stack[base_ptr].index_field != v_template)
	 && (input_stack[base_ptr].loc_field == null)
	 && (input_stack[base_ptr].state_field == token_list))
    decr (base_ptr);
  if ((input_stack[base_ptr].index_field != v_template)
      || (input_stack[base_ptr].loc_field != null)
      || (input_stack[base_ptr].state_field != token_list))
    fatal_error ("(interwoven alignment preambles are not allowed)");
  if (cur_group == align_group) {
    end_graf();
    if (fin_col())
      fin_row();
  } else {
    off_save();
  }
};

void
align_initialize_init (void) {
  /* module 934 */
  /* The token list |omit_template| just referred to is a constant token
   * list that contains the special control sequence \.{\\endtemplate} only.
   */
  info (omit_template) = end_template_token;	/* |link(omit_template)=null| */
  /* module 941 */
  /* A span node is a 2-word record containing |width|, |info|, and |link|
   * fields. The |link| field is not really a link, it indicates the number of
   * spanned columns; the |info| field points to a span node for the same
   * starting column, having a greater extent of spanning, or to |end_span|,
   * which has the largest possible |link| field; the |width| field holds the
   * largest natural width corresponding to a particular set of spanned columns.
   * 
   * A list of the maximum widths so far, for spanned columns starting at a
   * given column, begins with the |info| field of the alignrecord for that
   * column.
   */
  link (end_span) = max_quarterword + 1;
  info (end_span) = null;
}
