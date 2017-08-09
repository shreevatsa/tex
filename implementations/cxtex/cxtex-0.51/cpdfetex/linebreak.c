#include "types.h"
#include "c-compat.h"


#include "globals.h"

#define add_kern_stretch(a,b)  a  =  a  +  kern_stretch ( b )
#define add_kern_shrink(a,b)  a  =  a  +  kern_shrink ( b )

#define sub_kern_stretch(a,b)  a  =  a  -  kern_stretch ( b )
#define sub_kern_shrink(a,b)  a  =  a  -  kern_shrink ( b )

/* module 790 */
#define cal_margin_kern_var( arg ) {           \
 character (cp) = character (arg);             \
 font (cp) = font (arg);                       \
 do_subst_font (cp, 1000);                     \
 if (font (cp) != font (arg))                  \
  margin_kern_stretch = margin_kern_stretch    \
    + left_pw (arg) - left_pw (cp);            \
 font (cp) = font (arg);                       \
 do_subst_font (cp, -1000);                    \
 if (font (cp) != font (arg))                  \
  margin_kern_shrink = margin_kern_shrink      \
    + left_pw (cp) - left_pw (arg);  }



/* module 957 */

/* 
 * We come now to what is probably the most interesting algorithm of \TeX:
 * the mechanism for choosing the ``best possible'' breakpoints that yield
 * the individual lines of a paragraph. \TeX's line-breaking algorithm takes
 * a given horizontal list and converts it to a sequence of boxes that are
 * appended to the current vertical list. In the course of doing this, it
 * creates a special data structure containing three kinds of records that are
 * not used elsewhere in \TeX. Such nodes are created while a paragraph is
 * being processed, and they are destroyed afterwards; thus, the other parts
 * of \TeX\ do not need to know anything about how line-breaking is done.
 * 
 * The method used here is based on an approach devised by Michael F. Plass and
 * the author in 1977, subsequently generalized and improved by the same two
 * people in 1980. A detailed discussion appears in {\sl SOFTWARE---Practice
 * \AM\ Experience \bf11} (1981), 1119--1184, where it is shown that the
 * line-breaking problem can be regarded as a special case of the problem of
 * computing the shortest path in an acyclic network. The cited paper includes
 * numerous examples and describes the history of line breaking as it has been
 * practiced by printers through the ages. The present implementation adds two
 * new ideas to the algorithm of 1980: memory space requirements are considerably
 * reduced by using smaller records for inactive nodes than for active ones,
 * and arithmetic overflow is avoided by using ``delta distances'' instead of
 * keeping track of the total distance from the beginning of the paragraph to the
 * current point.
 */

/* module 958 */

/* The |line_break| procedure should be invoked only in horizontal mode; it
 * leaves that mode and places its output into the current vlist of the
 * enclosing vertical mode (or internal vertical mode).
 * There is one explicit parameter: |d| is true for partial paragraphs
 * preceding display math mode; in this case the amount of additional
 * penalty inserted before the final line is |display_widow_penalty|
 * instead of |widow_penalty|.
 * 
 * There are also a number of implicit parameters: The hlist to be broken
 * starts at |link(head)|, and it is nonempty. The value of |prev_graf| in the
 * enclosing semantic level tells where the paragraph should begin in the
 * sequence of line numbers, in case hanging indentation or \.{\\parshape}
 * are in use; |prev_graf| is zero unless this paragraph is being continued
 * after a displayed formula. Other implicit parameters, such as the
 * |par_shape_ptr| and various penalties to use for hyphenation, etc., appear
 * in |eqtb|.
 * 
 * After |line_break| has acted, it will have updated the current vlist and the
 * value of |prev_graf|. Furthermore, the global variable |just_box| will
 * point to the final box created by |line_break|, so that the width of this
 * line can be ascertained when it is necessary to decide whether to use
 * |above_display_skip| or |above_display_short_skip| before a displayed formula.
 */

pointer just_box;/* the |hlist_node| for the last line of the new paragraph */


/* module 962 */

/* The algorithm essentially determines the best possible way to achieve
 * each feasible combination of position, line, and fitness. Thus, it answers
 * questions like, ``What is the best way to break the opening part of the
 * paragraph so that the fourth line is a tight line ending at such-and-such
 * a place?'' However, the fact that all lines are to be the same length
 * after a certain point makes it possible to regard all sufficiently large
 * line numbers as equivalent, when the looseness parameter is zero, and this
 * makes it possible for the algorithm to save space and time.
 * 
 * An ``active node'' and a ``passive node'' are created in |mem| for each
 * feasible breakpoint that needs to be considered. Active nodes are three
 * words long and passive nodes are two words long. We need active nodes only
 * for breakpoints near the place in the paragraph that is currently being
 * examined, so they are recycled within a comparatively short time after
 * they are created.
 */


/* module 965 */

/* The passive node for a given breakpoint contains only four fields:
 * 
 * \yskip\hang|link| points to the passive node created just before this one,
 * if any, otherwise it is |null|.
 * 
 * \yskip\hang|cur_break| points to the position of this breakpoint in the
 * horizontal list for the paragraph being broken.
 * 
 * \yskip\hang|prev_break| points to the passive node that should precede this
 * one in an optimal path to this breakpoint.
 * 
 * \yskip\hang|serial| is equal to |n| if this passive node is the |n|th
 * one created during the current pass. (This field is used only when
 * printing out detailed statistics about the line-breaking calculations.)
 * 
 * \yskip\noindent
 * There is a global variable called |passive| that points to the most
 * recently created passive node. Another global variable, |printed_node|,
 * is used to help zprint out the paragraph when detailed information about
 * the line-breaking computation is being displayed.
 */

pointer passive;/* most recent node on passive list */
pointer printed_node;/* most recent node that has been printed */
halfword pass_number; /* the number of passive nodes allocated on this pass */

/* module 967 */

/* As the algorithm runs, it maintains a set of six delta-like registers
 * for the length of the line following the first active breakpoint to the
 * current position in the given hlist. When it makes a pass through the
 * active list, it also maintains a similar set of six registers for the
 * length following the active breakpoint of current interest. A third set
 * holds the length of an empty line (namely, the sum of \.{\\leftskip} and
 * \.{\\rightskip}); and a fourth set is used to create new delta nodes.
 * 
 * When we pass a delta node we want to do operations like
 * $$\hbox{\ignorespaces|for
 * k:=1 to 6 do cur_active_width[k]:=cur_active_width[k]+mem[q+k].sc|};$$ and we
 * want to do this without the overhead of |for| loops. The |do_all_six|
 * macro makes such six-tuples convenient.
 */
#define do_seven_eight( arg )   if (  pdf_adjust_spacing  > 1  )  { arg (7 ); arg (8 );}
#define do_all_eight( arg )  do_all_six ( arg ); do_seven_eight ( arg )
#define do_one_seven_eight( arg )  arg (1 ); do_seven_eight ( arg )
#define total_font_stretch  cur_active_width [7 ]
#define total_font_shrink  cur_active_width [8 ]
#define save_active_width( arg )  prev_active_width [ arg ]  =  active_width [ arg ]
#define restore_active_width( arg )  active_width [ arg ]  =  prev_active_width [ arg ]

scaled active_width[9];/* distance from first active node to~|cur_p| */
scaled cur_active_width[9];/* distance from current active node */
scaled background[9];/* length of an ``empty'' line */
scaled break_width[9];/* length being computed after current break */
boolean auto_breaking;/* make |auto_breaking| accessible out of |line_break| */
pointer prev_p;/* make |prev_p| accessible out of |line_break| */
pointer first_p;/* to access the first node of the paragraph */
pointer prev_char_p;/* pointer to the previous char of an implicit kern */
boolean try_prev_break; /* force break at the previous legal breakpoint? */
pointer prev_legal; /* the previous legal breakpoint */
pointer prev_prev_legal; /* to save |prev_p| corresponding to |prev_legal| */
boolean prev_auto_breaking; /* to save |auto_breaking| corresponding 
							   to |prev_legal| */
scaled prev_active_width[9]; /* to save |active_width| corresponding to 
								|prev_legal| */
pointer rejected_cur_p;/* the last |cur_p| that has been rejected */
boolean before_rejected_cur_p;/* |cur_p| is still before |rejected_cur_p|? */
int max_stretch_ratio;/* maximal stretch ratio of expanded fonts */
int max_shrink_ratio;/* maximal shrink ratio of expanded fonts */
int cur_font_step; /* the current step of expanded fonts */

/* module 969 */

/* Glue nodes in a horizontal list that is being paragraphed are not supposed to
 * include ``infinite'' shrinkability; that is why the algorithm maintains
 * four registers for stretching but only one for shrinking. If the user tries to
 * introduce infinite shrinkability, the shrinkability will be reset to finite
 * and an error message will be issued. A boolean variable |no_shrink_error_yet|
 * prevents this error message from appearing more than once per paragraph.
 */
/* module 969 */
#define check_shrinkage( arg )   if ( ( shrink_order ( arg )  !=  normal )  && ( shrink ( arg )  != 0 ) )  { arg   =  finite_shrink ( arg );}

boolean no_shrink_error_yet; /* have we complained about infinite shrinkage? */

/* module 972 */

/* A pointer variable |cur_p| runs through the given horizontal list as we look
 * for breakpoints. This variable is global, since it is used both by |line_break|
 * and by its subprocedure |try_break|.
 * 
 * Another global variable called |threshold| is used to determine the feasibility
 * of individual lines: breakpoints are feasible if there is a way to reach
 * them without creating lines whose badness exceeds |threshold|. (The
 * badness is compared to |threshold| before penalties are added, so that
 * penalty values do not affect the feasibility of breakpoints, except that
 * no break is allowed when the penalty is 10000 or more.) If |threshold|
 * is 10000 or more, all legal breaks are considered feasible, since the
 * |badness| function specified above never returns a value greater than~10000.
 * 
 * Up to three passes might be made through the paragraph in an attempt to find at
 * least one set of feasible breakpoints. On the first pass, we have
 * |threshold=pretolerance| and |second_pass=final_pass=false|.
 * If this pass fails to find a
 * feasible solution, |threshold| is set to |tolerance|, |second_pass| is set
 * |true|, and an attempt is made to hyphenate as many words as possible.
 * If that fails too, we add |emergency_stretch| to the background
 * stretchability and set |final_pass=true|.
 */

pointer cur_p; /* the current breakpoint under consideration */
boolean second_pass; /* is this our second attempt to break this paragraph? */
boolean final_pass; /* is this our final attempt to break this paragraph? */
int threshold; /* maximum badness on feasible lines */

/* module 973 */
#define copy_to_cur_active( arg )  cur_active_width [arg]  =  active_width [arg]

/* module 976 */
#define update_width(arg)  cur_active_width [arg] = cur_active_width [arg] + mem [r + arg].sc

/* module 977 */

/* As we consider various ways to end a line at |cur_p|, in a given line number
 * class, we keep track of the best total demerits known, in an array with
 * one entry for each of the fitness classifications. For example,
 * |minimal_demerits[tight_fit]| contains the fewest total demerits of feasible
 * line breaks ending at |cur_p| with a |tight_fit| line; |best_place[tight_fit]|
 * points to the passive node for the break before~|cur_p| that achieves such
 * an optimum; and |best_pl_line[tight_fit]| is the |line_number| field in the
 * active node corresponding to |best_place[tight_fit]|. When no feasible break
 * sequence is known, the |minimal_demerits| entries will be equal to
 * |awful_bad|, which is $2^{30}-1$. Another variable, |minimum_demerits|,
 * keeps track of the smallest value in the |minimal_demerits| array.
 */

integer minimal_demerits[tight_fit]; /* best total demerits known for current
										line class and position, given 
										the fitness */
integer minimum_demerits; /* best total demerits known for current 
							 line class and position */
pointer  best_place[tight_fit]; /* how to achieve |minimal_demerits| */
halfword best_pl_line[tight_fit]; /* corresponding line number */

/* module 981 */
#define set_break_width_to_background( arg )  break_width [ arg ]  =  background [ arg ]

/* module 983 */
#define sub_char_stretch(arg,b)  arg = arg - char_stretch (f, b)
#define sub_char_shrink(arg,b)   arg = arg - char_shrink  (f, b)

/* module 983 */

/* When |cur_p| is a discretionary break, the length of a line ``from |cur_p| to
 * |cur_p|'' has to be defined properly so that the other calculations work out.
 * Suppose that the pre-break text at |cur_p| has length $l_0$, the post-break
 * text has length $l_1$, and the replacement text has length |l|. Suppose
 * also that |q| is the node following the replacement text. Then length of a
 * line from |cur_p| to |q| will be computed as $\gamma+\beta(q)-\alpha(|cur_p|)$,
 * where $\beta(q)=\beta(|cur_p|)-l_0+l$. The actual length will be the background
 * plus $l_1$, so the length from |cur_p| to |cur_p| should be $\gamma+l_0+l_1-l$.
 * If the post-break text of the discretionary is empty, a break may also
 * discard~|q|; in that unusual case we subtract the length of~|q| and any
 * other nodes that will be discarded after the discretionary break.
 * 
 * The value of $l_0$ need not be computed, since |line_break| will put
 * it into the global variable |disc_width| before calling |try_break|.
 */
scaled disc_width[9]; /* the length of discretionary material preceding a break */

#define reset_disc_width( arg )  disc_width [ arg ]  = 0
#define add_disc_width_to_break_width( arg )  break_width [ arg ]  =  break_width [ arg ] +  disc_width [ arg ]
#define add_disc_width_to_active_width( arg )  active_width [ arg ]  =  active_width [ arg ] +  disc_width [ arg ]
#define sub_disc_width_from_active_width( arg )  active_width [ arg ]  =  active_width [ arg ] -  disc_width [ arg ]


/* module 987 */
#define convert_to_break_width(arg)  mem[prev_r+arg].sc = mem[prev_r+arg].sc - cur_active_width[arg] + break_width[arg]
#define store_break_width( arg )  active_width [ arg ]  =  break_width [ arg ]
#define new_delta_to_break_width( arg )  mem [ q  +  arg ]. sc   =  break_width [ arg ] -  cur_active_width [ arg ]

/* module 988 */
#define new_delta_from_break_width( arg )  mem [ q  +  arg ]. sc   =  cur_active_width [ arg ] -  break_width [ arg ]


/* module 991 */

/* The length of lines depends on whether the user has specified
 * \.{\\parshape} or \.{\\hangindent}. If |par_shape_ptr| is not null, it
 * points to a $(2n+1)$-word record in |mem|, where the |info| in the first
 * word contains the value of |n|, and the other $2n$ words contain the left
 * margins and line lengths for the first |n| lines of the paragraph; the
 * specifications for line |n| apply to all subsequent lines. If
 * |par_shape_ptr=null|, the shape of the paragraph depends on the value of
 * |n=hang_after|; if |n>=0|, hanging indentation takes place on lines |n+1|,
 * |n+2|, \dots, otherwise it takes place on lines 1, \dots, $\vert
 * n\vert$. When hanging indentation is active, the left margin is
 * |hang_indent|, if |hang_indent>=0|, else it is 0; the line length is
 * $|hsize|-\vert|hang_indent|\vert$. The normal setting is
 * |par_shape_ptr=null|, |hang_after=1|, and |hang_indent=0|.
 * Note that if |hang_indent=0|, the value of |hang_after| is irrelevant.
 */

halfword easy_line; /* line numbers |>easy_line| are equivalent in break nodes */
halfword last_special_line; /* line numbers |>last_special_line| all have the same width */
scaled first_width; /* the width of all lines |<=last_special_line|, 
					   if no \.{\\parshape} has been specified */
scaled second_width; /* the width of all lines |>last_special_line| */
scaled first_indent; /* left margin to go with |first_width| */
scaled second_indent; /* left margin to go with |second_width| */

/* module 1004 */
#define combine_two_deltas( arg )  mem [prev_r + arg].sc = mem[prev_r + arg].sc + mem[r + arg].sc
#define downdate_width( arg )  cur_active_width [arg] = cur_active_width [arg] - mem [prev_r + arg].sc

/* module 1005 */
#define update_active( arg )  active_width [arg] = active_width [arg] + mem[r + arg].sc

/* module 1008 */
#define store_background( arg )  active_width [arg] = background [arg]

/* module 1010 */
#define act_width  active_width [1]
#define kern_break {                                 \
    if (!is_char_node(link(cur_p)) && auto_breaking) \
      if (type (link(cur_p)) == glue_node)           \
        try_break (0 , unhyphenated);                \
     act_width = act_width + width (cur_p);          \
   }

/* module 1016 */

/* The forced line break at the paragraph's end will reduce the list of
 * breakpoints so that all active nodes represent breaks at |cur_p=null|.
 * On the first pass, we insist on finding an active node that has the
 * correct ``looseness.'' On the final pass, there will be at least one active
 * node, and we will match the desired looseness as well as we can.
 * 
 * The global variable |best_bet| will be set to the active node for the best
 * way to break the paragraph, and a few other variables are used to
 * help determine what is best.
 */

pointer best_bet; /* use this passive node and its predecessors */
int fewest_demerits; /* the demerits associated with |best_bet| */
halfword best_line; /* line number following the last line of the 
					   new paragraph */
int actual_looseness; /* the difference between |line_number(best_bet)| 
						 and the optimum |best_line| */
integer line_diff; /* the difference between the current line number 
					  and the optimum |best_line| */

/* module 1036 */

/* The letters $c_1\ldots c_n$ that are candidates for hyphenation are placed
 * into an array called |hc|; the number |n| is placed into |hn|; pointers to
 * nodes $p_{a-1}$ and~$p_b$ in the description above are placed into variables
 * |ha| and |hb|; and the font number is placed into |hf|.
 */

unsigned int hc[66]; /* word to be hyphenated */
small_number hn; /* the number of positions occupied in |hc| */
pointer ha, hb; /* nodes |ha..hb| should be replaced by the hyphenated 
				   result */
internal_font_number hf; /* font number of the letters in |hc| */
unsigned int hu[64]; /* like |hc|, before conversion to lowercase */
int hyf_char; /* hyphen character of the relevant font */
ASCII_code cur_lang, init_cur_lang; /* current hyphenation table of interest */
int l_hyf, r_hyf, init_l_hyf, init_r_hyf; /* limits on fragment sizes */
halfword hyf_bchar; /* boundary character after $c_n$ */

/* module 968 */

/* Let's state the principles of the delta nodes more precisely and concisely,
 * so that the following programs will be less obscure. For each legal
 * breakpoint~|p| in the paragraph, we define two quantities $\alpha(p)$ and
 * $\beta(p)$ such that the length of material in a line from breakpoint~|p|
 * to breakpoint~|q| is $\gamma+\beta(q)-\alpha(p)$, for some fixed $\gamma$.
 * Intuitively, $\alpha(p)$ and $\beta(q)$ are the total length of material from
 * the beginning of the paragraph to a point ``after'' a break at |p| and to a
 * point ``before'' a break at |q|; and $\gamma$ is the width of an empty line,
 * namely the length contributed by \.{\\leftskip} and \.{\\rightskip}.
 * 
 * Suppose, for example, that the paragraph consists entirely of alternating
 * boxes and glue skips; let the boxes have widths $x_1\ldots x_n$ and
 * let the skips have widths $y_1\ldots y_n$, so that the paragraph can be
 * represented by $x_1y_1\ldots x_ny_n$. Let $p_i$ be the legal breakpoint
 * at $y_i$; then $\alpha(p_i)=x_1+y_1+\cdots+x_i+y_i$, and $\beta(p_i)=
 * x_1+y_1+\cdots+x_i$. To check this, note that the length of material from
 * $p_2$ to $p_5$, say, is $\gamma+x_3+y_3+x_4+y_4+x_5=\gamma+\beta(p_5)
 * -\alpha(p_2)$.
 * 
 * The quantities $\alpha$, $\beta$, $\gamma$ involve glue stretchability and
 * shrinkability as well as a natural width. If we were to compute $\alpha(p)$
 * and $\beta(p)$ for each |p|, we would need multiple precision arithmetic, and
 * the multiprecise numbers would have to be kept in the active nodes.
 * \TeX\ avoids this problem by working entirely with relative differences
 * or ``deltas.'' Suppose, for example, that the active list contains
 * $a_1\,\delta_1\,a_2\,\delta_2\,a_3$, where the |a|'s are active breakpoints
 * and the $\delta$'s are delta nodes. Then $\delta_1=\alpha(a_1)-\alpha(a_2)$
 * and $\delta_2=\alpha(a_2)-\alpha(a_3)$. If the line breaking algorithm is
 * currently positioned at some other breakpoint |p|, the |active_width| array
 * contains the value $\gamma+\beta(p)-\alpha(a_1)$. If we are scanning through
 * the list of active nodes and considering a tentative line that runs from
 * $a_2$ to~|p|, say, the |cur_active_width| array will contain the value
 * $\gamma+\beta(p)-\alpha(a_2)$. Thus, when we move from $a_2$ to $a_3$,
 * we want to add $\alpha(a_2)-\alpha(a_3)$ to |cur_active_width|; and this
 * is just $\delta_2$, which appears in the active list between $a_2$ and
 * $a_3$. The |background| array contains $\gamma$. The |break_width| array
 * will be used to calculate values of new delta nodes when the active
 * list is being updated.
 */


/* module 959 */

/* Since |line_break| is a rather lengthy procedure---sort of a small world unto
 * itself---we must build it up little by little, somewhat more cautiously
 * than we have done with the simpler procedures of \TeX. Here is the
 * general outline.
 */

/* module 970 */
pointer 
finite_shrink (pointer p) {	/* recovers from infinite shrinkage */
  pointer q; /* new glue specification */ 
  if (no_shrink_error_yet) {
	no_shrink_error_yet = false;
	print_err ("Infinite glue shrinkage found in a paragraph");
	help5 ("The paragraph just ended includes some glue that has",
		   "infinite shrinkability, e.g., `\\hskip 0pt minus 1fil'.",
		   "Such glue doesn't belong there---it allows a paragraph",
		   "of any length to fit on one line. But it's safe to proceed,",
		   "since the offensive shrinkability has been made finite.");
	error();
  };
  q = new_spec (p);
  shrink_order (q) = normal;
  delete_glue_ref (p);
  return q;
};


/* module 973 */
pointer 
prev_rightmost (pointer s, pointer e) { 
  pointer prev_rightmost_r;
  pointer p;
  prev_rightmost_r = null;
  p = s;
  if (p == null)
	return prev_rightmost_r;
  while (link (p) != e) {
	p = link (p);
	if (p == null)
	  return prev_rightmost_r;
  };
  return p;
};
		  
scaled 
total_pw (pointer q, pointer p) {
  pointer l, r; /* s*/
  int n;
  if (break_node (q) == null) {
	l = first_p;
  } else {
	l = cur_break (break_node (q));
  }
  r = prev_rightmost (prev_p, p); /*  get |link(r)=p|  */ 
  if (r != null) {
	if ((type (r) == disc_node) && (type (p) == disc_node) && (pre_break (p) == null))	 {
	  /*  I cannot remember when this case happens but I encountered it once  */
	  /*  find the predecessor of |r|  */
	  if (r == prev_p)	{ /*  |link(prev_p)=p| and |prev_p| is also a |disc_node|  */
		/*  start from the leftmost node  */
		r = prev_rightmost (l, p);
	  } else {
		r = prev_rightmost (prev_p, p);
	  }
	} else if ((p != null) && (type (p) == disc_node) && (pre_break (p) != null)) {	
	  /*  a |disc_node| with non-empty |pre_break|, protrude the last char  */
	  r = pre_break (p);
	  while (link (r) != null)
		r = link (r);
	};
  };
 RESWITCH:
  while ((l != null) && discardable (l))
	l = link (l);
  if ((l != null) && (type (l) == disc_node)) {
	if (post_break (l) != null) {
	  l = post_break (l);
	} else {
	  n = replace_count (l);
	  l = link (l);
	  while (n > 0) {
		if (link (l) != null)
		  l = link (l);
		decr (n);
	  };
	};
	do_something; 
	goto RESWITCH;
  };
  return left_pw (l) + right_pw (r);
};


/* module 973 */

/* The heart of the line-breaking procedure is `|try_break|', a subroutine
 * that tests if the current breakpoint |cur_p| is feasible, by running
 * through the active list to see what lines of text can be made from active
 * nodes to~|cur_p|. If feasible breaks are possible, new break nodes are
 * created. If |cur_p| is too far from an active node, that node is
 * deactivated.
 * 
 * The parameter |pi| to |try_break| is the penalty associated
 * with a break at |cur_p|; we have |pi=eject_penalty| if the break is forced,
 * and |pi=inf_penalty| if the break is illegal.
 * 
 * The other parameter, |break_type|, is set to |hyphenated| or |unhyphenated|,
 * depending on whether or not the current break is at a |disc_node|. The
 * end of a paragraph is also regarded as `|hyphenated|'; this case is
 * distinguishable by the condition |cur_p=null|.
 */

/*TH fairly large sections of try_break are in two separate include files,
  to keep this C file understandable (try_break is approx 900 lines of code) */
  
void 
try_break (int pi, small_number break_type) {
  pointer r; /* runs through the active list */ 
  pointer prev_r; /* stays a step behind |r| */ 
  halfword old_l; /* maximum line number in current equivalence class of lines */
  boolean no_break_yet; /* have we found a feasible break at |cur_p|? */ 
  boolean can_try_prev_break; /* can we try to break at the previous breakpoint? */ 
  scaled margin_kern_stretch;
  scaled margin_kern_shrink;
  pointer lp, rp, cp;
  /* begin expansion of Other local variables for |try_break| */
  /* module 974 */
  pointer prev_prev_r; /* a step behind |prev_r|, if |type(prev_r)=delta_node| */
  pointer s; /* runs through nodes ahead of |cur_p| */ 
  pointer q; /* points to a new node being created */ 
  pointer v; /* points to a glue specification or a node ahead of |cur_p| */
  integer t; /* node count, if |cur_p| is a discretionary node */
  internal_font_number f; /* used in character width calculation */ 
  halfword l; /* line number of current active node */ 
  boolean node_r_stays_active; /* should node |r| remain in the active list? */ 
  scaled line_width; /* the current line will be justified to this width */
  unsigned char fit_class; /* possible fitness class of test line */ 
  halfword b; /* badness of test line */ 
  int d; /* demerits of test line */ 
  boolean artificial_demerits; /* has |d| been forced to zero? */ 
  pointer save_link; /* temporarily holds value of |link(cur_p)| */ 
  scaled shortfall; /* used in badness calculations */
  /* module 1789 */
  scaled g; /* glue stretch or shrink of test line, adjustment for last line */
  /* end expansion of Other local variables for |try_break| */
  g=0; line_width=0; prev_prev_r=null; /*TH -Wall*/
  /* begin expansion of Make sure that |pi| is in the proper range */
  /* module 975 */
  if (try_prev_break && (pi <= inf_penalty))
	pi = eject_penalty;
  can_try_prev_break = false;
  if (abs (pi) >= inf_penalty) {
	if (pi > 0) {
	  return; /* this breakpoint is inhibited by infinite penalty */
	} else {
	  pi = eject_penalty;	/* this breakpoint will be forced */
	}
  }
  /* end expansion of Make sure that |pi| is in the proper range */
  no_break_yet = true;
  prev_r = active;
  old_l = 0;
  do_all_eight (copy_to_cur_active);
  while (1) {
  CONTINUE:
	r = link (prev_r);
	/* begin expansion of If node |r| is of type |delta_node|, update |cur_active_width|,
	   set |prev_r| and |prev_prev_r|, then |goto continue| */
	/* module 976 */
	/* The following code uses the fact that |type(last_active)<>delta_node|. */
	if (type (r) == delta_node) {
	  do_all_eight (update_width);
	  prev_prev_r = prev_r;
	  prev_r = r;
	  goto CONTINUE;
	};
	/* end expansion of If node |r| is of type |delta_node|, update |cur_active_width|, ...*/
	/* begin If a line number class has ended, create new active nodes for the best feasible 
	   breaks in that class; then |return| if |r=last_active|, otherwise compute the new |line_width| */
	/* module 979 */
	/* The first part of the following code is part of \TeX's inner loop, so
	 * we don't want to waste any time. The current active node, namely node |r|,
	 * contains the line number that will be considered next. At the end of the
	 * list we have arranged the data structure so that |r=last_active| and
	 * |line_number(last_active)>old_l|.
	 */
	l = line_number (r);
	if (l > old_l) { /* now we are no longer in the inner loop */
	  if ((minimum_demerits < awful_bad) && ((old_l != easy_line) || (r == last_active))) {
		/* begin expansion of Create new active nodes for the best feasible breaks just found */
/* module 980 */

/* It is not necessary to create new active nodes having |minimal_demerits|
 * greater than
 * |minimum_demerits+abs(adj_demerits)|, since such active nodes will never
 * be chosen in the final paragraph breaks. This observation allows us to
 * omit a substantial number of feasible breakpoints from further consideration.
 */
{
if (no_break_yet) {
  /* begin expansion of Compute the values of |break_width| */
  /* module 981 */
  /* When we insert a new active node for a break at |cur_p|, suppose this
   * new node is to be placed just before active node |a|; then we essentially
   * want to insert `$\delta\,|cur_p|\,\delta^\prime$' before |a|, where
   * $\delta=\alpha(a)-\alpha(|cur_p|)$ and $\delta^\prime=\alpha(|cur_p|)-\alpha(a)$
   * in the notation explained above. The |cur_active_width| array now holds
   * $\gamma+\beta(|cur_p|)-\alpha(a)$; so $\delta$ can be obtained by
   * subtracting |cur_active_width| from the quantity $\gamma+\beta(|cur_p|)-
   * \alpha(|cur_p|)$. The latter quantity can be regarded as the length of a
   * line ``from |cur_p| to |cur_p|''; we call it the |break_width| at |cur_p|.
   * 
   * The |break_width| is usually negative, since it consists of the background
   * (which is normally zero) minus the width of nodes following~|cur_p| that are
   * eliminated after a break. If, for example, node |cur_p| is a glue node, the
   * width of this glue is subtracted from the background; and we also look
   * ahead to eliminate all subsequent glue and penalty and kern and math
   * nodes, subtracting their widths as well.
   * 
   * Kern nodes do not disappear at a line break unless they are |explicit|.
   */
  no_break_yet = false;
  do_all_eight (set_break_width_to_background);
  s = cur_p;
  if (break_type > unhyphenated)
	if (cur_p != null) {
	  /* begin expansion of Compute the discretionary |break_width| values */
	  /* begin expansion of 984 */
	  t = replace_count (cur_p);
	  v = cur_p;
	  s = post_break (cur_p);
	  while (t > 0) {
		decr (t);
		v = link (v);
		/* begin expansion of Subtract the width of node |v| from |break_width| */
		/* module 985 */
		/* Replacement texts and discretionary texts are supposed to contain
		 * only character nodes, kern nodes, ligature nodes, and box or rule nodes.
		 */
		if (is_char_node (v)) {
		  f = font (v);
		  break_width[1] = break_width[1] - char_width (f, char_info (f, character (v)));
		  if ((pdf_adjust_spacing > 1) && check_expand_pars (f)) {
			prev_char_p = v;
			sub_char_stretch (break_width[7],character (v));
			sub_char_shrink (break_width[8],character (v));
		  };
		} else {
		  switch (type (v)) {
		  case ligature_node:
			f = font (lig_char (v));
			break_width[1] =break_width[1] -char_width (f, char_info(f, character(lig_char(v))));
			if ((pdf_adjust_spacing > 1)&& check_expand_pars (f)){
			  prev_char_p = v;
			  sub_char_stretch (break_width[7],character (lig_char (v)));
			  sub_char_shrink  (break_width[8],character (lig_char (v)));
			};
			break;
		  case hlist_node:
		  case vlist_node:
		  case rule_node:
		  case kern_node:
			break_width[1] = break_width[1] - width (v);
			if ((type (v) == kern_node) && (pdf_adjust_spacing >1) && (subtype (v) == normal)) {
			  sub_kern_stretch(break_width[7],v);
			  sub_kern_shrink (break_width[8],v);
			};
			break;
		  default:
			confusion ("disc1");
		  };
		  /* end expansion of Subtract the width of node |v| from |break_width| */
		};
	  };
	  while (s != null) {
		/* begin expansion of Add the width of node |s| to |break_width| */
		/* module 986 */
		if (is_char_node (s)) {
		  f = font (s);
		  break_width[1] = break_width[1] + char_width (f, char_info (f, character (s)));
		  if ((pdf_adjust_spacing > 1)&& check_expand_pars (f))  {
			prev_char_p = s;
			add_char_stretch(break_width[7], character (s));
			add_char_shrink (break_width[8], character (s));
		  };
		} else {
		  switch (type (s)) {
		  case ligature_node:
			f = font (lig_char (s));
			break_width[1] = break_width[1] +char_width (f, char_info(f, character(lig_char(s))));
			if ((pdf_adjust_spacing > 1)&& check_expand_pars (f)){
			  prev_char_p = s;
			  add_char_stretch (break_width[7], character (lig_char (s)));
			  add_char_shrink  (break_width[8], character (lig_char (s)));
			};
			break;
		  case hlist_node:
		  case vlist_node:
		  case rule_node:
		  case kern_node:
			break_width[1] =break_width[1] + width (s);
			if ((type (s) == kern_node)&& (pdf_adjust_spacing > 1)  && (subtype (s) == normal)){
			  add_kern_stretch (break_width[7],s);
			  add_kern_shrink  (break_width[8],s);
			};
			break;
		  default:
			confusion ("disc2");
		  };
		  /* end expansion of Add the width of node |s| to |break_width| */
		};
		s = link (s);
	  };
	  do_one_seven_eight (add_disc_width_to_break_width);
	  if (post_break (cur_p) == null)
		s = link (v); /* nodes may be discardable after the break */ 
	};
  /* end expansion of Compute the discretionary |break_width| values */
  while (s != null) {
	if (is_char_node (s))
	  goto DONE;
	switch (type (s)) {
	case glue_node:
	  /* begin expansion of Subtract glue from |break_width| */
	  /* module 982 */
	  v = glue_ptr (s);
	  break_width[1] =break_width[1] - width (v);
	  break_width[2 + stretch_order (v)] =  break_width[2 + stretch_order (v)] - stretch (v);
	  break_width[6] = break_width[6] - shrink (v);
	  /* end expansion of Subtract glue from |break_width| */
	  break;
	case penalty_node:
	  do_something;
	  break;
	case math_node:
	  break_width[1] = break_width[1] - width (s);
	  break;
	case kern_node:
	  if (subtype (s) != explicit) {
		goto DONE;
	  } else {
		break_width[1] = break_width[1] - width (s);
	  }
	  break;
	default:
	  do_something;
	  goto DONE;
	};
	s = link (s);
  };
 DONE: 
  do_nothing; 
};
/* end expansion of Compute the values of |break_width| */
/* begin expansion of Insert a delta node to prepare for breaks at |cur_p| */
/* module 987 */
/* We use the fact that |type(active)<>delta_node|.
 */
if (type (prev_r) == delta_node)	{/* modify an existing delta node */
  do_all_eight (convert_to_break_width);
} else if (prev_r == active)	{ /* no delta node needed at the beginning */
  do_all_eight (store_break_width);
} else {
  q = get_node (delta_node_size);
  link (q) = r;
  type (q) = delta_node;
  subtype (q) = 0; /* the |subtype| is not used */
  do_all_eight (new_delta_to_break_width);
  link (prev_r) = q;
  prev_prev_r = prev_r;
  prev_r = q;
};
/* end expansion of Insert a delta node to prepare for breaks at |cur_p| */
if (abs (adj_demerits) >= awful_bad - minimum_demerits) {
  minimum_demerits = awful_bad - 1;
} else {
  minimum_demerits = minimum_demerits + abs (adj_demerits);
}
for (fit_class = very_loose_fit; fit_class <= tight_fit; fit_class++){
  if (minimal_demerits[fit_class] <= minimum_demerits) {
	/* begin expansion of Insert a new active node from |best_place[fit_class]| to |cur_p| */
	/* module 989 */
	/* When we create an active node, we also create the corresponding
	 * passive node.
	 */
	q = get_node (passive_node_size);
	link (q) = passive;
	passive = q;
	cur_break (q) = cur_p;
	incr (pass_number);
	serial (q) = pass_number;
	prev_break (q) = best_place[fit_class];
	q = get_node (active_node_size);
	break_node (q) = passive;
	line_number (q) = best_pl_line[fit_class] + 1;
	fitness (q) = fit_class;
	type (q) = break_type;
	total_demerits (q) = minimal_demerits[fit_class];
	if (do_last_line_fit) {
	  /* begin expansion of Store \(a)additional data in the new active node */
	  /* module 1796 */
	  /* Here we save these data in the active node representing a potential
	   * line break.
	   */
	  active_short (q) = best_pl_short[fit_class];
	  active_glue (q) = best_pl_glue[fit_class];
	  /* end expansion of Store \(a)additional data in the new active node */
	};
	link (q) = r;
	link (prev_r) = q;
	prev_r = q;
	if (tracing_paragraphs > 0) {
	  /* begin expansion of Print a symbolic description of the new break node */
	  /* module 990 */
	  print_nl_string("@@");
	  print_int (serial (passive));
	  zprint_string(": line ");
	  print_int (line_number (q) - 1);
	  print_char ('.');
	  print_int (fit_class);
	  if (break_type == hyphenated)
		print_char ('-');
	  zprint_string(" t=");
	  print_int (total_demerits (q));
	  if (do_last_line_fit) {
		/* begin expansion of Print additional data in the new active node */
		/* module 1797 */
		zprint_string(" s=");
		print_scaled (active_short (q));
		if (cur_p == null) {
		  zprint_string(" a=");
		} else {
		  zprint_string(" g=");
		}
		print_scaled (active_glue (q));
	  };
	  /* end expansion of Print additional data in the new active node */
	  zprint_string(" -> @@");
	  if (prev_break (passive) == null) {
		print_char ('0');
	  } else {
		print_int (serial (prev_break (passive)));
	  }
	};
	/* end expansion of Print a symbolic description of the new break node */
  };
  /* end expansion of Insert a new active node from |best_place[fit_class]| to |cur_p| */
  minimal_demerits[fit_class] = awful_bad;
};
minimum_demerits = awful_bad;
/* begin expansion of Insert a delta node to prepare for the next active node */
/* module 988 */
/* When the following code is performed, we will have just inserted at
 * least one active node before |r|, so |type(prev_r)<>delta_node|.
 */
if (r != last_active) {
  q = get_node (delta_node_size);
  link (q) = r;
  type (q) = delta_node;
  subtype (q) = 0; /* the |subtype| is not used */
  do_all_eight (new_delta_from_break_width);
  link (prev_r) = q;
  prev_prev_r = prev_r;
  prev_r = q;
};
/* end expansion of Insert a delta node to prepare for the next active node */
}


		/* end expansion of Create new active nodes for the best feasible breaks just found */
	  }
	  if (r == last_active) {
	    goto EXIT;
	  }
	  /* begin expansion of Compute the new line width */
	  /* module 994 */
	  /* When we come to the following code, we have just encountered the first
	   * active node~|r| whose |line_number| field contains |l|. Thus we want to
	   * compute the length of the $l\mskip1mu$th line of the current paragraph. Furthermore,
	   * we want to set |old_l| to the last number in the class of line numbers
	   * equivalent to~|l|.
	   */
	  if (l > easy_line) {
		line_width = second_width;
		old_l = max_halfword - 1;
	  } else {
		old_l = l;
		if (l > last_special_line) {
		  line_width = second_width;
		} else if (par_shape_ptr == null) {
		  line_width = first_width;
		} else {
		  line_width = mem[par_shape_ptr + 2 * l].sc;
		}
	  }
	  /* end expansion of Compute the new line width */
	};
	/* end expansion of If a line number class has ended, create new active nodes ...*/

	/* begin expansion of Consider the demerits for a line from |r| to |cur_p|; deactivate node |r| 
	   if it should no longer be active; then |goto continue| if a line from |r| to |cur_p| is 
	   infeasible, otherwise record a new feasible break */

/* module 995 */
/* The remaining part of |try_break| deals with the calculation of
 * demerits for a break from |r| to |cur_p|.
 * 
 * The first thing to do is calculate the badness, |b|. This value will always
 * be between zero and |inf_bad+1|; the latter value occurs only in the
 * case of lines from |r| to |cur_p| that cannot shrink enough to fit the necessary
 * width. In such cases, node |r| will be deactivated.
 * We also deactivate node~|r| when a break at~|cur_p| is forced, since future
 * breaks must go through a forced break.
 */
{
  artificial_demerits = false;
  shortfall = line_width - cur_active_width[1]; /* we're this much too short */
  if (pdf_protrude_chars > 1)
	shortfall = shortfall + total_pw (r, cur_p);
  if ((pdf_adjust_spacing > 1) && (shortfall != 0)) {
	margin_kern_stretch = 0;
	margin_kern_shrink = 0;
	if (pdf_protrude_chars > 1) { /* module 790 */
	  lp = last_leftmost_char;
	  rp = last_rightmost_char;
	  fast_get_avail (cp);
	  if (lp != null)
		cal_margin_kern_var (lp);
	  if (rp != null)
		cal_margin_kern_var (rp);
	  free_avail (cp); /* end 790 */
	};
	if ((shortfall > 0) && ((total_font_stretch + margin_kern_stretch) > 0)) {
	  if ((total_font_stretch + margin_kern_stretch) > shortfall) {
		shortfall = ((total_font_stretch +margin_kern_stretch) / (max_stretch_ratio / cur_font_step)) / 2;
	  } else {
		shortfall =  shortfall - (total_font_stretch + margin_kern_stretch);
	  }
	} else if ((shortfall < 0) && ((total_font_shrink + margin_kern_shrink) > 0)) {
	  if ((total_font_shrink + margin_kern_shrink) > -shortfall) {
		shortfall = -((total_font_shrink +margin_kern_shrink) / (max_shrink_ratio /cur_font_step)) / 2;
	  } else {
		shortfall =  shortfall + (total_font_shrink + margin_kern_shrink);
	  }
	};
  };
  if (shortfall > 0)
	/* Set the value of |b| to the badness for stretching the line, .. */
	/* module 996 */
	/* When a line must stretch, the available stretchability can be found in the
	 * subarray |cur_active_width[2..5]|, in units of points, fil, fill, and filll.
	 * 
	 * The present section is part of \TeX's inner loop, and it is most often performed
	 * when the badness is infinite; therefore it is worth while to make a quick
	 * test for large width excess and small stretchability, before calling the
	 * |badness| subroutine.
	 */
	if ((cur_active_width[3] != 0)|| (cur_active_width[4] != 0)|| (cur_active_width[5] != 0)) {
	  if (do_last_line_fit)	{
		if (cur_p == null) {	/* the last line of a paragraph */
		  /* begin expansion of Perform computations for last line and |goto found| */
		  /* module 1791 */
		  /* Here we compute the adjustment |g| and badness |b| for a line from |r|
		   * to the end of the paragraph. When any of the criteria for adjustment is
		   * violated we fall through to the normal algorithm.
		   * 
		   * The last line must be too short, and have infinite stretch entirely due
		   * to |par_fill_skip|.
		   */
		  if ((active_short (r) == 0) || (active_glue (r) <= 0)) {
			do_something;  
			goto NOT_FOUND; 
		  }
		  /* previous line was neither stretched nor shrunk, or was infinitely bad */
		  if ((cur_active_width[3] != fill_width[0]) 
			  || (cur_active_width[4] != fill_width[1])
			  || (cur_active_width[5] != fill_width[2])) {
			do_something;  
			goto NOT_FOUND; 
		  }
		  /* infinite stretch of this line not entirely due to |par_fill_skip| */
		  if (active_short (r) > 0) {
			g = cur_active_width[2];
		  } else {
			g = cur_active_width[6];
		  }
		  if (g <= 0)
			goto NOT_FOUND;	  /* no finite stretch resp.\ no shrink */
		  arith_error = false;
		  g = fract (g, active_short (r), active_glue (r), max_dimen);
		  if (last_line_fit < 1000)
			g = fract (g, last_line_fit, 1000, max_dimen);
		  if (arith_error) {
			if (active_short (r) > 0) {
			  g = max_dimen;
			} else {
			  g = -max_dimen; 
			}
		  }
		  if (g > 0) {
			/* begin expansion of Set the value of |b| to the badness of the last line
			   for stretching, compute the corresponding |fit_class, and |goto found|| */
			/* module 1792 */
			/* These badness computations are rather similar to those of the standard
			 * algorithm, with the adjustment amount |g| replacing the |shortfall|.
			 */
			if (g > shortfall)
			  g = shortfall;
			if (g > 7230584)
			  if (cur_active_width[2] < 1663497) {
				b = inf_bad;
				fit_class = very_loose_fit;
				goto FOUND;
			  };
			b = badness (g, cur_active_width[2]);
			if (b > 12){ 
			  if (b > 99){ 
				fit_class = very_loose_fit;
			  } else {
				fit_class = loose_fit;
			  } 
			} else {
			  fit_class = decent_fit;
			}
			goto FOUND;
			/* end expansion of Set the value of |b| to the badness of ....*/
		  }  else if (g < 0) {
			/* begin expansion of Set the value of |b| to the badness of the last line for 
			   shrinking, compute the corresponding |fit_class, and |goto found|| */
			/* module 1793 */
			if (-g > cur_active_width[6])
			  g = -cur_active_width[6];
			b = badness (-g, cur_active_width[6]);
			if (b > 12) {
			  fit_class = tight_fit;
			} else {
			  fit_class = decent_fit;
			}
			goto FOUND;
		  }
		  /* end expansion of Set the value of |b| to the badness of the last  ...*/
		  ;
		NOT_FOUND: 
		  do_nothing; 
		};
		/* end expansion of Perform computations for last line and |goto found| */
		shortfall = 0;
	  };
	  b = 0;
	  fit_class = decent_fit; /* infinite stretch */ 
	} else {
	  if (shortfall > 7230584)
		if (cur_active_width[2] < 1663497) {
		  b = inf_bad;
		  fit_class = very_loose_fit;
		  goto DONE1;
		};
	  b = badness (shortfall, cur_active_width[2]);
	  if (b > 12) {
		if (b > 99) {
		  fit_class = very_loose_fit;
		} else {
		  fit_class = loose_fit;
		} 
	  } else {
		fit_class = decent_fit;
	  }
	DONE1: 
	  do_nothing; 
	} else {
	  /* Set the value of |b| to the badness for shrinking the line, .. */
	  /* module 997 */	  
	  /* Shrinkability is never infinite in a paragraph;
	   * we can shrink the line from |r| to |cur_p| by at most |cur_active_width[6]|.
	   */
	  if (-shortfall > cur_active_width[6]) {
			b = inf_bad + 1;
		  } else {
			b = badness (-shortfall, cur_active_width[6]);
		  };
		  if (b > 12) {
			fit_class = tight_fit;
		  } else {
			fit_class = decent_fit;
		  };
	};
  if (do_last_line_fit) {
	/* begin expansion of Adjust \(t)the additional data for last line */
	/* module 1794 */
	/* Vanishing values of |shortfall| and |g| indicate that the last line is
	 * not adjusted.
	 */
	if (cur_p == null)
	  shortfall = 0;
	if (shortfall > 0) {
	  g = cur_active_width[2];
	} else  if (shortfall < 0) {
	  g = cur_active_width[6] ;
	} else {
	  g = 0;
	}
	/* end expansion of Adjust \(t)the additional data for last line */
  };
 FOUND:
  if ((b > inf_bad) || (pi == eject_penalty)) {
	/* Prepare to deactivate node~|r|, and |goto deactivate| unless 
	   there is a reason to consider lines of text from |r| to |cur_p| */
	/* module 998 */
	/* During the final pass, we dare not lose all active nodes, lest we lose
	 * touch with the line breaks already found. The code shown here makes sure
	 * that such a catastrophe does not happen, by permitting overfull boxes as
	 * a last resort. This particular part of \TeX\ was a source of several subtle
	 * bugs before the correct program logic was finally discovered; readers
	 * who seek to ``improve'' \TeX\ should therefore think thrice before daring
	 * to make any changes here.
	 */
	if (final_pass && (minimum_demerits == awful_bad)
			&& (link (r) == last_active) && (prev_r == active)) {
		  if ((pdf_avoid_overfull > 0) && (b > inf_bad)
			  && (prev_legal != null) && (prev_legal != cur_p)) {
			if (try_prev_break)
			  confusion ("overfull box recovery");
			rejected_cur_p = cur_p;
			can_try_prev_break = true;
			goto EXIT;
		  };
		  artificial_demerits = true; /* set demerits zero, this break is forced */ 
		} else if (b > threshold) {
		  goto DEACTIVATE;
		}
		node_r_stays_active = false;
  } else {
	prev_r = r;
	if (b > threshold) {
	  goto CONTINUE;
	}
	node_r_stays_active = true;
  };
  /* begin expansion of Record a new feasible break */
  /* module 999 + 1003 */
  /* When we get to this part of the code, the line from |r| to |cur_p| is
   * feasible, its badness is~|b|, and its fitness classification is |fit_class|.
   * We don't want to make an active node for this break yet, but we will
   * compute the total demerits and record them in the |minimal_demerits| array,
   * if such a break is the current champion among all ways to get to |cur_p|
   * in a given line-number class and fitness class.
   */
  {
	if (artificial_demerits) {
	d = 0;
  } else {
	d = line_penalty + b;
	if (abs (d) >= 10000) {
	  d = 100000000;
	} else {
	  d = d * d;
	}
	if (pi != 0) {
	  if (pi > 0) {
		d = d + pi * pi;
	  } else {
		if (pi > eject_penalty)
		  d = d - pi * pi; 
	  } 
	}
	if ((break_type == hyphenated) && (type (r) == hyphenated)) {
	  if (cur_p != null) {
		d = d + double_hyphen_demerits;
	  } else {
		d = d + final_hyphen_demerits; 
	  }
	}
	if (abs (intcast (fit_class) - intcast (fitness (r))) > 1)
	  d = d + adj_demerits;
  };
  if (tracing_paragraphs > 0) {
	/* begin expansion of Print a symbolic description of this feasible break */
	/* module 1000 */
	if (printed_node != cur_p) {
	  /* begin expansion of Print the list between |printed_node| and |cur_p|, 
		 then set |printed_node:=cur_p| */
	  /* module 1001 */
	  print_nl_string("");
	  if (cur_p == null) {
		short_display (link (printed_node));
	  } else {
		save_link = link (cur_p);
		link (cur_p) = null;
		print_nl_string("");
		short_display (link (printed_node));
		link (cur_p) = save_link;
	  };
	  printed_node = cur_p;
	  /* end expansion of Print the list between |printed_node| and |cur_p| .. */
	};
	print_nl_string ("@");
	if (cur_p == null) {
	  print_esc_string ("par");
	} else if (type (cur_p) != glue_node) {
	  if (type (cur_p) == penalty_node) {
		print_esc_string ("penalty");
	  } else if (type (cur_p) == disc_node) {
		print_esc_string ("discretionary");
	  } else if (type (cur_p) == kern_node) {
		print_esc_string ("kern");
	  } else {
		print_esc_string ("math");
	  }
	};
	zprint_string(" via @@");
	if (break_node (r) == null) {
	  print_char ('0');
	} else {
	  print_int (serial (break_node (r)));
	}
	zprint_string(" b=");
	if (b > inf_bad) {
	  print_char ('*');
	} else {
	  print_int (b);
	}
	zprint_string(" p=");
	print_int (pi);
	zprint_string(" d=");
	if (artificial_demerits) {
	  print_char ('*');
	} else {
	  print_int (d);
	}
  };
  /* end expansion of Print a symbolic description of this feasible break */
  d = d + total_demerits (r);
  /* this is the minimum total demerits from the beginning to |cur_p| via |r| */
  if (d <= minimal_demerits[fit_class]) {
	minimal_demerits[fit_class] = d;
	best_place[fit_class] = break_node (r);
	best_pl_line[fit_class] = l;
	if (do_last_line_fit) {
	  /* begin expansion of Store \(a)additional data for this feasible break */
	  /* module 1795 */
	  /* For each feasible break we record the shortfall and glue stretch or
	   * shrink (or adjustment).
	   */
	  best_pl_short[fit_class] = shortfall;
	  best_pl_glue[fit_class] = g;
	};
	/* end expansion of Store \(a)additional data for this feasible break */
	if (d < minimum_demerits)
	  minimum_demerits = d;
  };
}
  /* end expansion of Record a new feasible break */
  if (node_r_stays_active) {
	do_something;
	goto CONTINUE; 
  }
  /* |prev_r| has been set to |r| */ 
 DEACTIVATE:
  /* begin expansion of Deactivate node |r| */
/* module 1004 + 1005 */

/* When an active node disappears, we must delete an adjacent delta node if the
 * active node was at the beginning or the end of the active list, or if it
 * was surrounded by delta nodes. We also must preserve the property that
 * |cur_active_width| represents the length of material from |link(prev_r)|
 * to~|cur_p|.
 *
 * The following code uses the fact that |type(last_active)<>delta_node|. If the
 * active list has just become empty, we do not need to update the
 * |active_width| array, since it will be initialized when an active
 * node is next inserted.
 */
link (prev_r) = link (r);
free_node (r, active_node_size);
if (prev_r == active) {
  r = link (active);
  if (type (r) == delta_node) {
	do_all_eight (update_active);
	do_all_eight (copy_to_cur_active);
	link (active) = link (r);
	free_node (r, delta_node_size);
  };
} else if (type (prev_r) == delta_node) {
  r = link (prev_r);
  if (r == last_active) {
	do_all_eight (downdate_width);
	link (prev_prev_r) = last_active;
	free_node (prev_r, delta_node_size);
	prev_r = prev_prev_r;
  } else if (type (r) == delta_node) {
	do_all_eight (update_width);
	do_all_eight (combine_two_deltas);
	link (prev_r) = link (r);
	free_node (r, delta_node_size);
  };
};
  /* end expansion of Deactivate node |r| */
}
    /* end expansion of Consider the demerits for a line from |r| to |cur_p|; deactivate n ..*/
  }
 EXIT:
  if (can_try_prev_break) {
	try_prev_break = true;
  } else {
	do_nothing;
	/* begin expansion of Update the value of |printed_node| for symbolic displays */
	/* module 1002 */
	/* When the data for a discretionary break is being displayed, we will have
	 * printed the |pre_break| and |post_break| lists; we want to skip over the
	 * third list, so that the discretionary data will not appear twice. The
	 * following code is performed at the very end of |try_break|.
	 */
	if (cur_p == printed_node) {
	  if (cur_p != null) {
		if (type (cur_p) == disc_node) {
		  t = replace_count (cur_p);
		  while (t > 0) {
			decr (t);
			printed_node = link (printed_node);
		  };
		};
	  }
	}
	/* end expansion of Update the value of |printed_node| for symbolic displays */
	if (try_prev_break) {
	  prev_legal = null;
	  try_prev_break = false;
	} else {
	  if (pi < inf_penalty) {
		prev_legal = cur_p;
		prev_prev_legal = prev_p;
		prev_auto_breaking = auto_breaking;
		do_all_eight (save_active_width);
	  };
	  if (before_rejected_cur_p && (cur_p == rejected_cur_p))
		before_rejected_cur_p = false;
	};
  };
};


/* module 1009 */
#define cleanup_memory 	q = link (active);\
	while (q != last_active) {\
	  cur_p = link (q);\
	  if (type (q) == delta_node) {\
		free_node (q, delta_node_size);\
	  } else {\
		free_node (q, active_node_size);\
	  }\
	  q = cur_p;\
	};\
	q = passive;\
	while (q != null) {\
	  cur_p = link (q);\
	  free_node (q, passive_node_size);\
	  q = cur_p;\
	}


void 
line_break (boolean d) {
  /* begin expansion of Local variables for line breaking */
  /* module 1006 */
  pointer q, r, s, prev_s; /* miscellaneous nodes of temporary interest */
  internal_font_number f; /* used when calculating character widths */
  /* module 1037 */
  /* Hyphenation routines need a few more local variables.
   */
  small_number j; /* an index into |hc| or |hu| */ 
  unsigned char c; /* character being considered for hyphenation */
  /* end expansion of Local variables for line breaking */
  pack_begin_line = mode_line;  /* this is for over/underfull box messages */
  /* begin expansion of Get ready to start line breaking */
  /* module 960 */
  /* The first task is to move the list from |head| to |temp_head| and go
   * into the enclosing semantic level. We also append the \.{\\parfillskip}
   * glue to the end of the paragraph, removing a space (or other glue node) if
   * it was there, since spaces usually precede blank lines and instances of
   * `\.{\$\$}'. The |par_fill_skip| is preceded by an infinite penalty, so
   * it will never be considered as a potential breakpoint.
   * 
   * This code assumes that a |glue_node| and a |penalty_node| occupy the
   * same number of |mem|~words.
   */
  link (temp_head) = link (head);
  if (is_char_node (tail)) {
	tail_append (new_penalty (inf_penalty));
  } else if (type (tail) != glue_node) {
	tail_append (new_penalty (inf_penalty));
  } else {
	type (tail) = penalty_node;
	delete_glue_ref (glue_ptr (tail));
	flush_node_list (leader_ptr (tail));
	penalty (tail) = inf_penalty;
  };
  link (tail) = new_param_glue (par_fill_skip_code);
  last_line_fill = link (tail);
  init_cur_lang = prev_graf % 65536;
  init_l_hyf = prev_graf / 4194304;
  init_r_hyf = (prev_graf / 65536) % 64;
  pop_nest();

  /* module 971 */
  no_shrink_error_yet = true;
  check_shrinkage (left_skip);
  check_shrinkage (right_skip);
  q = left_skip;
  r = right_skip;
  background[1] = width (q) + width (r);
  background[2] = 0;
  background[3] = 0;
  background[4] = 0;
  background[5] = 0;
  background[2 + stretch_order (q)] = stretch (q);
  background[2 + stretch_order (r)] = background[2 + stretch_order (r)] + stretch (r);
  background[6] = shrink (q) + shrink (r);
  if (pdf_adjust_spacing > 1) {
	background[7] = 0;
	background[8] = 0;
	max_stretch_ratio = -1;
	max_shrink_ratio = -1;
	cur_font_step = -1;
	prev_char_p = null;
  };
  /* begin expansion of Check for special treatment of last line of paragraph */
  /* module 1788 */
  /* The new algorithm for the last line requires that the stretchability
   * |par_fill_skip| is infinite and the stretchability of |left_skip| plus
   * |right_skip| is finite.
   */
  do_last_line_fit = false;
  active_node_size = active_node_size_normal; /* just in case */ 
  if (last_line_fit > 0) {
	q = glue_ptr (last_line_fill);
	if ((stretch (q) > 0) && (stretch_order (q) > normal))
	  if ((background[3] == 0) && (background[4] == 0) && (background[5] == 0)) {
		do_last_line_fit = true;
		active_node_size = active_node_size_extended;
		fill_width[0] = 0;
		fill_width[1] = 0;
		fill_width[2] = 0;
		fill_width[stretch_order (q) - 1] = stretch (q);
	  };
  };
  /* end expansion of Check for special treatment of last line of paragraph */
  /* module 978 */
  minimum_demerits = awful_bad;
  minimal_demerits[tight_fit] = awful_bad;
  minimal_demerits[decent_fit] = awful_bad;
  minimal_demerits[loose_fit] = awful_bad;
  minimal_demerits[very_loose_fit] = awful_bad;

  /* module 992 */
  /* We compute the values of |easy_line| and the other local variables relating
   * to line length when the |line_break| procedure is initializing itself.
   */
  if (par_shape_ptr == null)
	if (hang_indent == 0) {
	  last_special_line = 0;
	  second_width = hsize;
	  second_indent = 0;
	} else {
	  /* begin expansion of Set line length parameters in preparation for hanging indentation */
	  /* module 993 */
	  last_special_line = abs (hang_after);
	  if (hang_after < 0) {
		first_width = hsize - abs (hang_indent);
		if (hang_indent >= 0) {
		  first_indent = hang_indent;
		} else {
		  first_indent = 0;
		}
		second_width = hsize;
		second_indent = 0;
	  } else {
		first_width = hsize;
		first_indent = 0;
		second_width = hsize - abs (hang_indent);
		if (hang_indent >= 0) {
		  second_indent = hang_indent;
		} else {
		  second_indent = 0;
		}
	  };
	  /* end expansion of Set line length parameters in preparation for hanging indentation */
	} else {
	  last_special_line = info (par_shape_ptr) - 1;
	  second_width = mem[par_shape_ptr + 2 * (last_special_line + 1)].sc;
	  second_indent = mem[par_shape_ptr + 2 * last_special_line + 1].sc;
	};
  if (looseness == 0) {
	easy_line = last_special_line;
  } else {
	easy_line = max_halfword;
  }
  /* end expansion of Get ready to start line breaking */

  /* begin expansion of Find optimal breakpoints */
  /* module 1007 */
  /* 
   * So far we have gotten a little way into the |line_break| routine, having
   * covered its important |try_break| subroutine. Now let's consider the
   * rest of the process.
   * 
   * The main loop of |line_break| traverses the given hlist,
   * starting at |link(temp_head)|, and calls |try_break| at each legal
   * breakpoint. A variable called |auto_breaking| is set to true except
   * within math formulas, since glue nodes are not legal breakpoints when
   * they appear in formulas.
   * 
   * The current node of interest in the hlist is pointed to by |cur_p|. Another
   * variable, |prev_p|, is usually one step behind |cur_p|, but the real
   * meaning of |prev_p| is this: If |type(cur_p)=glue_node| then |cur_p| is a legal
   * breakpoint if and only if |auto_breaking| is true and |prev_p| does not
   * point to a glue node, penalty node, explicit kern node, or math node.
   * 
   * The following declarations provide for a few other local variables that are
   * used in special calculations.
   */
  /* The `\ignorespaces|loop|\unskip' in the following code is performed at most
   * thrice per call of |line_break|, since it is actually a pass over the
   * entire paragraph.
   */
  threshold = pretolerance;
  if (threshold >= 0) {
	if (tracing_paragraphs > 0) {
	  begin_diagnostic();
	  print_nl_string("@firstpass");
	};
	second_pass = false;
	final_pass = false;
  } else {
	threshold = tolerance;
	second_pass = true;
	final_pass = ((emergency_stretch <= 0) && (pdf_avoid_overfull <= 0));
	if (tracing_paragraphs > 0)
	  begin_diagnostic();
  };
  loop {
	if (threshold > inf_bad)
	  threshold = inf_bad;
	if (second_pass) {
	  /* begin expansion of Initialize for hyphenating a paragraph */
	  /* module 1035 */
	  /* 
	   * When the line-breaking routine is unable to find a feasible sequence of
	   * breakpoints, it makes a second pass over the paragraph, attempting to
	   * hyphenate the hyphenatable words. The goal of hyphenation is to insert
	   * discretionary material into the paragraph so that there are more
	   * potential places to break.
	   * 
	   * The general rules for hyphenation are somewhat complex and technical,
	   * because we want to be able to hyphenate words that are preceded or
	   * followed by punctuation marks, and because we want the rules to work
	   * for languages other than English. We also must contend with the fact
	   * that hyphens might radically alter the ligature and kerning structure
	   * of a word.
	   * 
	   * A sequence of characters will be considered for hyphenation only if it
	   * belongs to a ``potentially hyphenatable part'' of the current paragraph.
	   * This is a sequence of nodes $p_0p_1\ldots p_m$ where $p_0$ is a glue node,
	   * $p_1\ldots p_{m-1}$ are either character or ligature or whatsit or
	   * implicit kern nodes, and $p_m$ is a glue or penalty or insertion or adjust
	   * or mark or whatsit or explicit kern node. (Therefore hyphenation is
	   * disabled by boxes, math formulas, and discretionary nodes already inserted
	   * by the user.) The ligature nodes among $p_1\ldots p_{m-1}$ are effectively
	   * expanded into the original non-ligature characters; the kern nodes and
	   * whatsits are ignored. Each character |c| is now classified as either a
	   * nonletter (if |lc_code(c)=0|), a lowercase letter (if
	   * |lc_code(c)=c|), or an uppercase letter (otherwise); an uppercase letter
	   * is treated as if it were |lc_code(c)| for purposes of hyphenation. The
	   * characters generated by $p_1\ldots p_{m-1}$ may begin with nonletters; let
	   * $c_1$ be the first letter that is not in the middle of a ligature. Whatsit
	   * nodes preceding $c_1$ are ignored; a whatsit found after $c_1$ will be the
	   * terminating node $p_m$. All characters that do not have the same font as
	   * $c_1$ will be treated as nonletters. The |hyphen_char| for that font
	   * must be between 0 and 255, otherwise hyphenation will not be attempted.
	   * \TeX\ looks ahead for as many consecutive letters $c_1\ldots c_n$ as
	   * possible; however, |n| must be less than 64, so a character that would
	   * otherwise be $c_{64}$ is effectively not a letter. Furthermore $c_n$ must
	   * not be in the middle of a ligature. In this way we obtain a string of
	   * letters $c_1\ldots c_n$ that are generated by nodes $p_a\ldots p_b$, where
	   * |1<=a<=b+1<=m|. If |n>=l_hyf+r_hyf|, this string qualifies for hyphenation;
	   * however, |uc_hyph| must be positive, if $c_1$ is uppercase.
	   * 
	   * The hyphenation process takes place in three stages. First, the candidate
	   * sequence $c_1\ldots c_n$ is found; then potential positions for hyphens
	   * are determined by referring to hyphenation tables; and finally, the nodes
	   * $p_a\ldots p_b$ are replaced by a new sequence of nodes that includes the
	   * discretionary breaks found.
	   * 
	   * Fortunately, we do not have to do all this calculation very often, because
	   * of the way it has been taken out of \TeX's inner loop. For example, when
	   * the second edition of the author's 700-page book {\sl Seminumerical
	   * Algorithms} was typeset by \TeX, only about 1.2 hyphenations needed to be
	   * 
	   * tried per paragraph, since the line breaking algorithm needed to use two
	   * passes on only about 5 per cent of the paragraphs.
	   */
#ifdef INIT
	  if (trie_not_ready)
		init_trie();
#endif /* INIT */
	  cur_lang = init_cur_lang;
	  l_hyf = init_l_hyf;
	  r_hyf = init_r_hyf;
	  set_hyph_index;
	}
	/* end expansion of Initialize for hyphenating a paragraph */

	/* begin expansion of Create an active breakpoint representing the beginning of the paragraph */
	/* module 1008 */
	/* The active node that represents the starting point does not need a
	 * corresponding passive node.
	 */
	q = get_node (active_node_size);
	type (q) = unhyphenated;
	fitness (q) = decent_fit;
	link (q) = last_active;
	break_node (q) = null;
	line_number (q) = prev_graf + 1;
	total_demerits (q) = 0;
	link (active) = q;
	if (do_last_line_fit) {
	  /* begin expansion of Initialize additional fields of the first active node */
	  /* module 1790 */
	  /* Here we initialize the additional fields of the first active node
	   * representing the beginning of the paragraph.
	   */
	  active_short (q) = 0;
	  active_glue (q) = 0;
	  /* end expansion of Initialize additional fields of the first active node */
	};
	do_all_eight (store_background);
	passive = null;
	printed_node = temp_head;
	pass_number = 0;
	font_in_short_display = null_font;
	/* end expansion of Create an active breakpoint representing the beginning of the paragraph */
	cur_p = link (temp_head);
	auto_breaking = true;
	prev_p = cur_p; /* glue at beginning is not a legal breakpoint */
	prev_char_p = null;
	prev_legal = null;
	rejected_cur_p = null;
	try_prev_break = false;
	before_rejected_cur_p = false;
	first_p = cur_p; 
	/* to access the first node of paragraph as the first active node has |break_node=null| */
	while ((cur_p != null) && (link (active) != last_active)) {
	  /* begin expansion of Call |try_break| if |cur_p| is a legal breakpoint; 
		 on the second pass, also try to hyphenate the next word, if |cur_p| is a glue node; 
		 then advance |cur_p| to the next node of the paragraph that could possibly be a legal breakpoint */
	  /* module 1010 */
	  /* Here is the main switch in the |line_break| routine, where legal breaks
	   * are determined. As we move through the hlist, we need to keep the |active_width|
	   * array up to date, so that the badness of individual lines is readily calculated
	   * by |try_break|. It is convenient to use the short name |act_width| for
	   * the component of active width that represents real width as opposed to glue.
	   */
	  if (is_char_node (cur_p)) {
		/* begin expansion of Advance \(c)|cur_p| to the node following the present string of characters */
		/* module 1011 */
		/* The code that passes over the characters of words in a paragraph is
		 * part of \TeX's inner loop, so it has been streamlined for speed. We use
		 * the fact that `\.{\\parfillskip}' glue appears at the end of each paragraph;
		 * it is therefore unnecessary to check if |link(cur_p)=null| when |cur_p| is a
		 * character node.
		 */
		prev_p = cur_p;
		do {
		  f = font (cur_p);
		  act_width =  act_width + char_width (f, char_info (f, character (cur_p)));
		  if ((pdf_adjust_spacing > 1) && check_expand_pars (f)) {
			prev_char_p = cur_p;
			add_char_stretch (active_width[7], character (cur_p));
			add_char_shrink (active_width[8], character (cur_p));
		  };
		  cur_p = link (cur_p);
		} while (is_char_node (cur_p));
		/* end expansion of Advance \(c)|cur_p| to the node following the present string of characters */
	  };
	  switch (type (cur_p)) {
	  case hlist_node:
	  case vlist_node:
	  case rule_node:
		act_width = act_width + width (cur_p);
		break;
	  case whatsit_node:
		/* begin expansion of Advance \(p)past a whatsit node in the \(l)|line_break| loop */
		/* module 1558 */
		adv_past (cur_p);
		if ((subtype (cur_p) == pdf_refxform_node) || (subtype (cur_p) == pdf_refximage_node))
		  act_width = act_width + pdf_width (cur_p);
		/* end expansion of Advance \(p)past a whatsit node in the \(l)|line_break| loop */
		break;
	  case glue_node:
		/* begin expansion of If node |cur_p| is a legal breakpoint, call |try_break|; 
		   then update the active widths by including the glue in |glue_ptr(cur_p)| */
		/* module 1012 */
		/* When node |cur_p| is a glue node, we look at |prev_p| to see whether or not
		 * a breakpoint is legal at |cur_p|, as explained above.
		 */
		if (auto_breaking) {
		  if (is_char_node (prev_p)) {
			try_break (0, unhyphenated);
		  } else if (precedes_break (prev_p)) {
			try_break (0, unhyphenated);
		  } else if ((type (prev_p) == kern_node) && (subtype (prev_p) != explicit))
			try_break (0, unhyphenated);
		};
		check_shrinkage (glue_ptr (cur_p));
		q = glue_ptr (cur_p);
		act_width = act_width + width (q);
		active_width[2 + stretch_order (q)] = active_width[2 + stretch_order (q)] + stretch (q);
		active_width[6] = active_width[6] + shrink (q);
		/* end expansion of If node |cur_p| is a legal breakpoint, call |try_break|;... */
		if (second_pass && auto_breaking && !(before_rejected_cur_p || (cur_p == rejected_cur_p))) {
		  /* begin expansion of Try to hyphenate the following word */
		  /* module 1038 */
		  /* When the following code is activated, the |line_break| procedure is in its
		   * second pass, and |cur_p| points to a glue node.
		   */
		  prev_s = cur_p;
		  s = link (prev_s);
		  if (s != null) {
			/* begin expansion of Skip to node |ha|, or |goto done1| if no hyphenation should be attempted */
			/* module 1040 */
			/* The first thing we need to do is find the node |ha| just before the
			 * first letter.
			 */
			loop {
			  if (is_char_node (s)) {
				c = qo (character (s));
				hf = font (s);
			  } else if (type (s) == ligature_node) {
				if (lig_ptr (s) == null) {
				  goto CONTINUE;
				} else {
				  q = lig_ptr (s);
				  c = qo (character (q));
				  hf = font (q);
				}
			  } else if ((type (s) == kern_node) && (subtype (s) == normal)) {
				goto CONTINUE;
			  } else if (type (s) == whatsit_node) {
				/* begin expansion of Advance \(p)past a whatsit node in the \(p)pre-hyphenation loop */
				/* module 1559 */
				if (subtype (s) == language_node) {
				  cur_lang = what_lang (s);
				  l_hyf = what_lhm (s);
				  r_hyf = what_rhm (s);
				  set_hyph_index;
				};
				/* end expansion of Advance \(p)past a whatsit node in the \(p)pre-hyphenation loop */
				goto CONTINUE;
			  } else {
				do_something;   
				goto DONE1; 
			  }
			  set_lc_code (c);
			  if (hc[0] != 0) {
				if ((hc[0] == c) || (uc_hyph > 0)) {
				  do_something;  
				  goto DONE2;
				} else { 
				  do_something;  
				  goto DONE1; 
				}
			  }
			CONTINUE:
			  prev_s = s;
			  s = link (prev_s);
			};
		  DONE2:
			hyf_char = hyphen_char [hf];
			if (hyf_char < 0)
			  goto DONE1;
			if (hyf_char > 255)
			  goto DONE1;
			ha = prev_s;
			/* end expansion of Skip to node |ha|, or |goto done1| if no hyphenation should be attempted */
			if (l_hyf + r_hyf > 63)
			  goto DONE1;
			/* begin expansion of Skip to node |hb|, putting letters into |hu| and |hc| */
			/* module 1041 */
			/* The word to be hyphenated is now moved to the |hu| and |hc| arrays. */
			hn = 0;
			loop {
			  if (is_char_node (s)) {
				if (font (s) != hf)
				  goto DONE3;
				hyf_bchar = character (s);
				c = qo (hyf_bchar);
				set_lc_code (c);
				if (hc[0] == 0)
				  goto DONE3;
				if (hn == 63)
				  goto DONE3;
				hb = s;
				incr (hn);
				hu[hn] = c;
				hc[hn] = hc[0];
				hyf_bchar = non_char;
			  } else if (type (s) == ligature_node) {
				/* begin expansion of Move the characters of a ligature node to |hu| and |hc|;
				   but |goto done3| if they are not all letters */
				/* module 1042 */
				/* We let |j| be the index of the character being stored when a ligature node
				 * is being expanded, since we do not want to advance |hn| until we are sure
				 * that the entire ligature consists of letters. Note that it is possible
				 * to get to |done3| with |hn=0| and |hb| not set to any value.
				 */
				if (font (lig_char (s)) != hf)
				  goto DONE3;
				j = hn;
				q = lig_ptr (s);
				if (q > null)
				  hyf_bchar = character (q);
				while (q > null) {
				  c = qo (character (q));
				  set_lc_code (c);
				  if (hc[0] == 0)
					goto DONE3;
				  if (j == 63)
					goto DONE3;
				  incr (j);
				  hu[j] = c;
				  hc[j] = hc[0];
				  q = link (q);
				};
				hb = s;
				hn = j;
				if (odd (subtype (s))) {
				  hyf_bchar = font_bchar[hf];
				} else {
				  hyf_bchar = non_char;
				}
				/* end expansion of Move the characters of a ligature node to |hu| and |hc|;... */
			  } else if ((type (s) == kern_node) && (subtype (s) == normal)) {
				hb = s;
				hyf_bchar = font_bchar[hf];
			  } else {
				do_something;  
				goto DONE3; 
			  }
			  s = link (s);
			};
		  DONE3:
			/* end expansion of Skip to node |hb|, putting letters into |hu| and |hc| */
			/* begin expansion of Check that the nodes following |hb| permit hyphenation and 
			   that at least |l_hyf+r_hyf| letters have been found, otherwise |goto done1| */
			/* module 1043 */
			if (hn < l_hyf + r_hyf)
			  goto DONE1; /* |l_hyf| and |r_hyf| are |>=1| */ 
			loop {
			  if (!(is_char_node (s))) {
				switch (type (s)) {
				case ligature_node:
				  do_something;
				  break;
				case kern_node:
				  if (subtype (s) != normal) 
					goto DONE4; 
				  do_something;  
				  break;
				case whatsit_node:
				case glue_node:
				case penalty_node:
				case ins_node:
				case adjust_node:
				case mark_node:
				  do_something;  
				  goto DONE4;
				default:
				  do_something;  
				  goto DONE1;
				};
			  }
			  s = link (s);
			};
		  DONE4:
			/* end expansion of Check that the nodes following |hb| permit hyphenation and tha ...*/
			hyphenate();
		  };
		DONE1: 
		  do_nothing;
		  /* end expansion of Try to hyphenate the following word */
		};
		break;
	  case kern_node:
		if (subtype (cur_p) == explicit) {
		  kern_break;
		} else {
		  act_width = act_width + width (cur_p);
		  if ((pdf_adjust_spacing > 1) && (subtype (cur_p) == normal)) {
			add_kern_stretch (active_width[7],cur_p);
			add_kern_shrink (active_width[8],cur_p);
		  };
		};
		break;
	  case ligature_node:
		f = font (lig_char (cur_p));
		act_width =act_width + char_width (f, char_info (f, character (lig_char (cur_p))));
		if ((pdf_adjust_spacing > 1) && check_expand_pars (f)) {
		  prev_char_p = cur_p;
		  add_char_stretch (active_width[7], character(lig_char(cur_p)));
		add_char_shrink (active_width[8], character(lig_char(cur_p)));
		};
		break;
	  case disc_node:
		/* begin expansion of Try to break after a discretionary fragment, then |goto done5| */
		/* module 1013 */
		/* The following code knows that discretionary texts contain
		 * only character nodes, kern nodes, box nodes, rule nodes, and ligature nodes.
		 */
		{
		  s = pre_break (cur_p);
		  do_one_seven_eight (reset_disc_width);
		  if (s == null) {
			try_break (ex_hyphen_penalty, hyphenated);
		  } else {
			do {
			  /* begin expansion of Add the width of node |s| to |disc_width| */
			  /* module 1014 */
			  if (is_char_node (s)) {
				f = font (s);
				disc_width[1] = disc_width[1] + char_width (f, char_info (f, character (s)));
				if ((pdf_adjust_spacing > 1) && check_expand_pars (f)) {
				  prev_char_p = s;
				  add_char_stretch (disc_width[7], character (s));
				  add_char_shrink (disc_width[8], character (s));
				};
			  } else {
				switch (type (s)) {
				case ligature_node:
				  f = font (lig_char (s));
				  disc_width[1] = disc_width[1] + char_width (f, char_info (f, character (lig_char (s))));
				  if ((pdf_adjust_spacing > 1) && check_expand_pars (f))  {
					prev_char_p = s;
					add_char_stretch (disc_width [7], character (lig_char (s)));
					add_char_shrink (disc_width [8], character (lig_char (s)));
				  };
				  break;
				case hlist_node:
				case vlist_node:
				case rule_node:
				case kern_node:
				  disc_width[1] = disc_width[1] + width (s);
				  if ((type (s) == kern_node) && (pdf_adjust_spacing > 1) && (subtype (s) == normal)) {
					add_kern_stretch (disc_width[7],s);
					add_kern_shrink (disc_width[8],s);
				  };
				  break;
				default:
				  confusion ("disc3");
				};
				/* end expansion of Add the width of node |s| to |disc_width| */
			  };
			  s = link (s);
			} while (s != null);
			do_one_seven_eight (add_disc_width_to_active_width);
			try_break (hyphen_penalty, hyphenated);
			do_one_seven_eight (sub_disc_width_from_active_width);
		  };
		  r = replace_count (cur_p);
		  s = link (cur_p);
		  while (r > 0) {
			/* begin expansion of Add the width of node |s| to |act_width| */
			/* module 1015 */
			if (is_char_node (s)) {
			  f = font (s);
			  act_width = act_width + char_width (f, char_info (f, character (s)));
			  if ((pdf_adjust_spacing > 1) && check_expand_pars (f)) {
				prev_char_p = s;
				add_char_stretch (active_width[7], character (s));
				add_char_shrink (active_width[8], character (s));
			  };
			} else {
			  switch (type (s)) {
			  case ligature_node:
				f = font (lig_char (s));
				act_width =act_width + char_width (f, char_info (f, character (lig_char (s))));
				if ((pdf_adjust_spacing > 1)&& check_expand_pars (f))  {
				  prev_char_p = s;
				  add_char_stretch (active_width[7], character (lig_char (s)));
				  add_char_shrink (active_width[8], character (lig_char (s)));
				};
				break;
			  case hlist_node:
			  case vlist_node:
			  case rule_node:
			  case kern_node:
				act_width = act_width + width (s);
				if ((type (s) == kern_node) && (pdf_adjust_spacing > 1) && (subtype (s) == normal)) {
				  add_kern_stretch (active_width[7],s);
				  add_kern_shrink (active_width[8],s);
				};
				break;
			  default:
				confusion ("disc4");
			  };
			  /* end expansion of Add the width of node |s| to |act_width| */
			};
			decr (r);
			s = link (s);
		  };
		  prev_p = cur_p;
		  cur_p = s;
		  goto DONE5;
		};
		/* end expansion of Try to break after a discretionary fragment, then |goto done5| */
		break;
	  case math_node:
		if (subtype (cur_p) < L_code)
		  auto_breaking = end_LR (cur_p);
		kern_break;
		break;
	  case penalty_node:
		try_break (penalty (cur_p), unhyphenated);
		break;
	  case mark_node:
	  case ins_node:
	  case adjust_node:
		do_something;
		break;
	  default:
		confusion ("paragraph");
	  };
	  prev_p = cur_p;
	  cur_p = link (cur_p);
	DONE5:
	  if (try_prev_break) {
		if (pdf_avoid_overfull > 1) {
		  print_ln();
		  print_nl_string ("Overfull \\hbox detected at breakpoint:");
		  print_ln();
		  short_display_n (prev_p, 10);
		  print_ln();
		  print_nl_string ("Trying to break at the previous legal breakpoint:");
		  print_ln();
		  short_display_n (prev_legal, 10);
		  print_ln();
		};
		cur_p = prev_legal;
		prev_p = prev_prev_legal;
		auto_breaking = prev_auto_breaking;
		do_all_eight (restore_active_width);
		prev_legal = null;
		before_rejected_cur_p = true;
	  };
	};
	/* end expansion of Call |try_break| if |cur_p| is a legal breakpoint; ... */
	if (cur_p == null) {
	  /* begin expansion of Try the final line break at the end of the paragraph,
		 and |goto done| if the desired breakpoints have been found */
	  /* module 1017 */
	  try_break (eject_penalty, hyphenated);
	  if (try_prev_break)
		goto DONE5;
	  if (link (active) != last_active) {
		/* begin expansion of Find an active node with fewest demerits */
		/* module 1018 */
		r = link (active);
		fewest_demerits = awful_bad;
		do {
		  if (type (r) != delta_node)
			if (total_demerits (r) < fewest_demerits) {
			  fewest_demerits = total_demerits (r);
			  best_bet = r;
			};
		  r = link (r);
		} while (r != last_active);
		best_line = line_number (best_bet);
		/* end expansion of Find an active node with fewest demerits */
		if (looseness == 0)
		  goto DONE;
		/* begin expansion of Find the best active node for the desired looseness */
		/* module 1019 */
		/* The adjustment for a desired looseness is a slightly more complicated
		 * version of the loop just considered. Note that if a paragraph is broken
		 * into segments by displayed equations, each segment will be subject to the
		 * looseness calculation, independently of the other segments.
		 */
		{
		  r = link (active);
		  actual_looseness = 0;
		  do {
			if (type (r) != delta_node){
			  line_diff = intcast (line_number (r)) - intcast (best_line);
			  if (((line_diff < actual_looseness) && (looseness <= line_diff))
				  || ((line_diff > actual_looseness) && (looseness >= line_diff))) {
				best_bet = r;
				actual_looseness = line_diff;
				fewest_demerits = total_demerits (r);
			  } else if ((line_diff == actual_looseness) && (total_demerits (r) < fewest_demerits)) {
				best_bet = r;
				fewest_demerits = total_demerits (r);
			  };
			};
			r = link (r);
		  } while (r != last_active);
		  best_line = line_number (best_bet);
		  /* end expansion of Find the best active node for the desired looseness */
		};
		if ((actual_looseness == looseness) || final_pass) {
		  do_something;  goto DONE; }
	  };
	  /* end expansion of Try the final line break at the end of the paragraph,... */
	};
	/* Clean up the memory by removing the break nodes */
	cleanup_memory;
	if (!second_pass) {
	  if (tracing_paragraphs > 0)
		print_nl_string("@secondpass");
	  threshold = tolerance;
	  second_pass = true;
	  final_pass = ((emergency_stretch <= 0) && (pdf_avoid_overfull <= 0));
	} else { /* if at first you don't succeed, \dots */
	  if (tracing_paragraphs > 0)
		print_nl_string("@emergencypass");
	  if (pdf_avoid_overfull <= 0)
		background[2] = background[2] + emergency_stretch;
	  final_pass = true;
	};
  };
 DONE:
  if (tracing_paragraphs > 0) {
	end_diagnostic (true);
	normalize_selector();
  };
  if (do_last_line_fit) {
	/* begin expansion of Adjust \(t)the final line of the paragraph */
	/* module 1798 */
	/* Here we either reset |do_last_line_fit| or adjust the |par_fill_skip|
	 * glue.
	 */
	if (active_short (best_bet) == 0) {
	  do_last_line_fit = false;
	} else {
	  q = new_spec (glue_ptr (last_line_fill));
	  delete_glue_ref (glue_ptr (last_line_fill));
	  width (q) = width (q) + active_short (best_bet) - active_glue (best_bet);
	  stretch (q) = 0;
	  glue_ptr (last_line_fill) = q;
	};
	/* end expansion of Adjust \(t)the final line of the paragraph */
	/* end expansion of Find optimal breakpoints */
  };
  /* begin expansion of Break the paragraph at the chosen breakpoints, justify the resulting
	 lines to the correct widths, and append them to the current vertical list */
  /* module 1020 */
  /* Once the best sequence of breakpoints has been found (hurray), we call on the
   * procedure |post_line_break| to finish the remainder of the work.
   * (By introducing this subprocedure, we are able to keep |line_break|
   * from getting extremely long.)
   */
  post_line_break (d);
  /* end expansion of Break the paragraph at the chosen breakpoints, justify ...*/
  /* Clean up the memory by removing the break nodes */
  cleanup_memory;
  pack_begin_line = 0;
};



/* module 1078 */

/* We have now completed the hyphenation routine, so the |line_break| procedure
 * is finished at last. Since the hyphenation exception table is fresh in our
 * minds, it's a good time to deal with the routine that adds new entries to it.
 * 
 * When \TeX\ has scanned `\.{\\hyphenation}', it calls on a procedure named
 * |new_hyph_exceptions| to do the right thing.
 */
void 
new_hyph_exceptions (void) {  /* enters new exceptions */
  unsigned char n; /* length of current word; not always a |small_number| */
  unsigned char j; /* an index into |hc| */ 
  hyph_pointer h; /* an index into |hyph_word| and |hyph_list| */ 
  str_number k; /* an index into |str_start| */ 
  pointer p; /* head of a list of hyphen positions */ 
  pointer q;  /* used when creating a new node for list |p| */ 
  str_number s; /* t*/ /* strings being compared or stored */ 
  pool_pointer u, v; /* indices into |str_pool| */ 
  scan_left_brace(); /* a left brace must follow \.{\\hyphenation} */ 
  set_cur_lang;
#ifdef INIT
  if (trie_not_ready) {
	hyph_index = 0;
	goto NOT_FOUND1;
  };
#endif /* INIT */
  set_hyph_index;
 NOT_FOUND1:
  /* begin expansion of Enter as many hyphenation exceptions as are listed, until coming to a right brace; then |return| */
  /* module 1079 */
  n = 0;
  p = null;
  loop {
	get_x_token();
  RESWITCH:
	switch (cur_cmd) {
	case letter:
	case other_char:
	case char_given:
	  /* begin expansion of Append a new letter or hyphen */
	  /* module 1081 */
	  if (cur_chr == '-') {
		/* begin expansion of Append the value |n| to list |p| */
		/* module 1082 */
		if (n < 63) {
		  q = get_avail();
		  link (q) = p;
		  info (q) = n;
		  p = q;
		};
		/* end expansion of Append the value |n| to list |p| */
	  } else {
		set_lc_code (cur_chr);
		if (hc[0] == 0) {
		  print_err ("Not a letter");
		  help2  ("Letters in \\hyphenation words must have \\lccode>0.",
				  "Proceed; I'll ignore the character I just read.");
		  error();
		} else if (n < 63) {
		  incr (n);
		  hc[n] = hc[0];
		};
		/* end expansion of Append a new letter or hyphen */
	  };
	  break;
	case char_num:
	  scan_char_num();
	  cur_chr = cur_val;
	  cur_cmd = char_given;
	  goto RESWITCH;
	  break;
	case spacer:
	case right_brace:
	  {
		if (n > 1) {
		  /* begin expansion of Enter a hyphenation exception */
		  /* module 1083 */
		  incr (n);
		  hc[n] = cur_lang;
		  str_room (n);
		  h = 0;
		  for (j = 1; j <= n; j++) {
			h = (h + h + hc[j]) % hyph_prime;
			append_char (hc[j]);
		  };
		  s = make_string();
		  /* begin expansion of Insert the \(p)pair |(s,p)| into the exception table */
		  /* module 1084 */
		  if (hyph_next <= hyph_prime)
			while ((hyph_next > 0) && (hyph_word[hyph_next - 1] > 0))
			  decr (hyph_next);
		  if ((hyph_count == hyph_size) || (hyph_next == 0))
			overflow ("exception dictionary", hyph_size);
		  incr (hyph_count);
		  while (hyph_word[h] != 0) {
			/* begin expansion of If the string |hyph_word[h]| is less than \(or)or equal to |s|, 
			   interchange |(hyph_word[h],hyph_list[h])| with |(s,p)| */
			/* module 1085 */
			/* This is now a simple hash list, not an ordered one, so the module title is no longer descriptive. */
			k = hyph_word[h];
			if (length (k) != length (s))
			  goto NOT_FOUND;
			u = str_start[k];
			v = str_start[s];
			do {
			  if (str_pool[u] != str_pool[v])
				goto NOT_FOUND;
			  incr (u);
			  incr (v);
			} while (!(u == str_start[k + 1])); /* repeat hyphenation exception; flushing old data */
			flush_string;
			s = hyph_word[h]; /* avoid |slow_make_string|! */
			decr (hyph_count); /*  We could also |flush_list(hyph_list[h]);|, but it interferes with \.{trip.log}.  */
			goto FOUND;
		  NOT_FOUND:
			/* end expansion of If the string |hyph_word[h]| is less than \(or)or equal to.. */
			if (hyph_link[h] == 0) {
			  hyph_link[h] = hyph_next;
			  if (hyph_next >= hyph_size)
				hyph_next = hyph_prime;
			  if (hyph_next > hyph_prime)
				incr (hyph_next);
			};
			h = hyph_link[h] - 1;
		  };
		FOUND:
		  hyph_word[h] = s;
		  hyph_list[h] = p;
		  /* end expansion of Insert the \(p)pair |(s,p)| into the exception table */
		  /* end expansion of Enter a hyphenation exception */
		};
		if (cur_cmd == right_brace)
		  return;
		n = 0;
		p = null;
	  };
	  break;
	default:
	  /* begin expansion of Give improper \.{\\hyphenation} error */
	  /* module 1080 */
	  print_err ("Improper ");
	  print_esc_string ("hyphenation");
	  zprint_string(" will be flushed");
	  help2  ("Hyphenation exceptions must contain only letters",
			  "and hyphens. But continue; I'll forgive and forget.");
	  error();
	  /* end expansion of Give improper \.{\\hyphenation} error */
	};
  };
  /* end expansion of Enter as many hyphenation exceptions as are listed, until coming to a right brace; then |return| */
};


/* module 964 */
void
linebreak_initialize_init (void) {
  type (last_active) = hyphenated;
  line_number (last_active) = max_halfword;
  subtype (last_active) = 0;	/* the |subtype| is never examined by the algorithm */
}

