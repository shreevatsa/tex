/* module 961 */

/* When looking for optimal line breaks, \TeX\ creates a ``break node'' for
 * each break that is {\sl feasible}, in the sense that there is a way to end
 * a line at the given place without requiring any line to stretch more than
 * a given tolerance. A break node is characterized by three things: the position
 * of the break (which is a pointer to a |glue_node|, |math_node|, |penalty_node|,
 * or |disc_node|); the ordinal number of the line that will follow this
 * breakpoint; and the fitness classification of the line that has just
 * ended, i.e., |tight_fit|, |decent_fit|, |loose_fit|, or |very_loose_fit|.
 */
#define tight_fit 3
#define loose_fit 1
#define very_loose_fit 0
#define decent_fit 2

/* module 963 */

/* An active node for a given breakpoint contains six fields:
 * 
 * \yskip\hang|link| points to the next node in the list of active nodes; the
 *                              last active node has |link=last_active|.
 * \yskip\hang|break_node| points to the passive node associated with this breakpoint.
 * \yskip\hang|line_number| is the number of the line that follows this breakpoint.
 * \yskip\hang|fitness| is the fitness classification of the line ending at this breakpoint.
 * \yskip\hang|type| is either |hyphenated| or |unhyphenated|, depending on
 *                                     whether this breakpoint is a |disc_node|.
 * \yskip\hang|total_demerits| is the minimum possible sum of demerits over all
 * lines leading from the beginning of the paragraph to this breakpoint.
 * 
 * \yskip\noindent
 * The value of |link(active)| points to the first active node on a linked list
 * of all currently active nodes. This list is in order by |line_number|,
 * except that nodes with |line_number>easy_line| may be in any order relative
 * to each other.
 */
#define active_node_size_normal 3
#define fitness  subtype
#define break_node  rlink
#define line_number  llink
#define total_demerits( arg )  mem [ arg  + 2 ]. cint
#define unhyphenated 0
#define hyphenated 1
#define last_active  active

/* module 965 */
#define passive_node_size 2
#define cur_break  rlink
#define prev_break  llink
#define serial  info

/* module 966 */

/* The active list also contains ``delta'' nodes that help the algorithm
 * compute the badness of individual lines. Such nodes appear only between two
 * active nodes, and they have |type=delta_node|. If |p| and |r| are active nodes
 * and if |q| is a delta node between them, so that |link(p)=q| and |link(q)=r|,
 * then |q| tells the space difference between lines in the horizontal list that
 * start after breakpoint |p| and lines that start after breakpoint |r|. In
 * other words, if we know the length of the line that starts after |p| and
 * ends at our current position, then the corresponding length of the line that
 * starts after |r| is obtained by adding the amounts in node~|q|. A delta node
 * contains six scaled numbers, since it must record the net change in glue
 * stretchability with respect to all orders of infinity. The natural width
 * difference appears in |mem[q+1].sc|; the stretch differences in units of
 * pt, fil, fill, and filll appear in |mem[q+2..q+5].sc|; and the shrink difference
 * appears in |mem[q+6].sc|. The |subtype| field of a delta node is not used.
 */
#define delta_node_size 9
#define delta_node 2

/* module 967 */
#define do_all_six(arg)  arg (1); arg (2); arg (3); arg (4); arg (5); arg (6)

/* module 973 */
#define discardable( arg )                                         \
   !(is_char_node (arg) ||                                         \
     non_discardable (arg) ||                                      \
     (( type(arg) == kern_node) && (subtype (arg) != explicit)) || \
     (type (arg) == margin_kern_node))

/* module 977 */
#define awful_bad 1073741823

/* module 983 */
#define add_char_stretch(a,b)  a = a + char_stretch (f, b)
#define add_char_shrink(a,b)   a = a + char_shrink (f, b)

/* module 1032 */
#define append_list(a,b) { link (tail) = link (a); tail = b; }


/*  module 958 */
EXTERN pointer just_box;/* the |hlist_node| for the last line of the new paragraph */

/* module 967 */
EXTERN scaled active_width[9];/* distance from first active node to~|cur_p| */
EXTERN scaled cur_active_width[9];/* distance from current active node */
EXTERN scaled break_width[9];/* length being computed after current break */
EXTERN pointer prev_p;/* make |prev_p| accessible out of |line_break| */
EXTERN pointer prev_char_p;/* pointer to the previous char of an implicit kern */
EXTERN pointer prev_legal; /* the previous legal breakpoint */
EXTERN scaled prev_active_width[9]; /* to save |active_width| corresponding to |prev_legal| */
EXTERN pointer rejected_cur_p;/* the last |cur_p| that has been rejected */
EXTERN int max_stretch_ratio;/* maximal stretch ratio of expanded fonts */
EXTERN int max_shrink_ratio;/* maximal shrink ratio of expanded fonts */
EXTERN int cur_font_step; /* the current step of expanded fonts */

/* module 972 */
EXTERN pointer cur_p; /* the current breakpoint under consideration */

/* module 977 */
EXTERN pointer  best_place[tight_fit]; /* how to achieve |minimal_demerits| */

/* module 991 */
EXTERN halfword last_special_line; /* line numbers |>last_special_line| 
							   all have the same width */
EXTERN scaled first_width; /* the width of all lines |<=last_special_line|, 
					   if no \.{\\parshape} has been specified */
EXTERN scaled second_width; /* the width of all lines |>last_special_line| */
EXTERN scaled first_indent; /* left margin to go with |first_width| */
EXTERN scaled second_indent; /* left margin to go with |second_width| */

/* module 1016 */
EXTERN pointer best_bet; /* use this passive node and its predecessors */
EXTERN int fewest_demerits; /* the demerits associated with |best_bet| */
EXTERN halfword best_line; /* line number following the last line of the new paragraph */

/* module 1036 */
EXTERN unsigned int hc[66]; /* word to be hyphenated */
EXTERN small_number hn; /* the number of positions occupied in |hc| */
EXTERN pointer ha, hb; /* nodes |ha..hb| should be replaced by the hyphenated result */
EXTERN internal_font_number hf; /* font number of the letters in |hc| */
EXTERN unsigned int hu[64]; /* like |hc|, before conversion to lowercase */
EXTERN int hyf_char; /* hyphen character of the relevant font */
EXTERN ASCII_code cur_lang; /* current hyphenation table of interest */
EXTERN int l_hyf, r_hyf; /* limits on fragment sizes */
EXTERN halfword hyf_bchar; /* boundary character after $c_n$ */

EXTERN pointer  prev_rightmost (pointer s, pointer e) ;

EXTERN void line_break (boolean d);

EXTERN void new_hyph_exceptions (void);

EXTERN void linebreak_initialize_init (void);
