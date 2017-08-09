/* module 109 */

/* When \TeX\ ``packages'' a list into a box, it needs to calculate the
 * proportionality ratio by which the glue inside the box should stretch
 * or shrink. This calculation does not affect \TeX's decision making,
 * so the precise details of rounding, etc., in the glue calculation are not
 * of critical importance for the consistency of results on different computers.
 * 
 * We shall use the type |glue_ratio| for such proportionality ratios.
 * A glue ratio should take the same amount of memory as an
 * |integer| (usually 32 bits) if it is to blend smoothly with \TeX's
 * other data structures. Thus |glue_ratio| should be equivalent to
 * |short_real| in some implementations of \PASCAL. Alternatively,
 * it is possible to deal with glue ratios using nothing but fixed-point
 * arithmetic; see {\sl TUGboat \bf3},1 (March 1982), 10--27. (But the
 * routines cited there must be modified to allow negative glue ratios.)
 */
#define set_glue_ratio_zero( arg )  arg   = 0.0
#define set_glue_ratio_one( arg )  arg   = 1.0
#define zfloat( arg )  (arg + 0.0)
#define unfloat( arg )  arg


/* module 784 */

/* 
 * We're essentially done with the parts of \TeX\ that are concerned with
 * the input (|get_next|) and the output (|ship_out|). So it's time to
 * get heavily into the remaining part, which does the real work of typesetting.
 * 
 * After lists are constructed, \TeX\ wraps them up and puts them into boxes.
 * Two major subroutines are given the responsibility for this task: |hpack|
 * applies to horizontal lists (hlists) and |vpack| applies to vertical lists
 * (vlists). The main duty of |hpack| and |vpack| is to compute the dimensions
 * of the resulting boxes, and to adjust the glue if one of those dimensions
 * is pre-specified. The computed sizes normally enclose all of the material
 * inside the new box; but some items may stick out if negative glue is used,
 * if the box is overfull, or if a \.{\\vbox} includes other boxes that have
 * been shifted left.
 * 
 * The subroutine call |hpack(p,w,m)| returns a pointer to an |hlist_node|
 * for a box containing the hlist that starts at |p|. Parameter |w| specifies
 * a width; and parameter |m| is either `|exactly|' or `|additional|'. Thus,
 * |hpack(p,w,exactly)| produces a box whose width is exactly |w|, while
 * |hpack(p,w,additional)| yields a box whose width is the natural width plus
 * |w|. It is convenient to define a macro called `|natural|' to cover the
 * most common case, so that we can say |hpack(p,natural)| to get a box that
 * has the natural width of list |p|.
 * 
 * Similarly, |vpack(p,w,m)| returns a pointer to a |vlist_node| for a
 * box containing the vlist that starts at |p|. In this case |w| represents
 * a height instead of a width; the parameter |m| is interpreted as in |hpack|.
 */
#define exactly 0
#define additional 1

/* module 791 */
/* 
 * @# {constants used when calling |hpack| to deal with font expansion}
 */
#define cal_expand_ratio 2
#define subst_ex_font 3
#define substituted 3
#define left_pw( arg )  char_pw ( arg , left_side )
#define right_pw( arg )  char_pw ( arg , right_side )

EXTERN scaled char_pw (pointer p, small_number side);


/* module 786 */
EXTERN scaled total_stretch[5], total_shrink[5]; /* glue found by |hpack| or  |vpack| */
EXTERN int last_badness; /* badness of the most recently packaged box */

/* module 789 */
EXTERN integer font_expand_ratio;
EXTERN pointer last_leftmost_char;
EXTERN pointer last_rightmost_char;

/* module 803 */
/* Determine the stretch order */
#define determine_stretch_order           \
  if (total_stretch[filll] != 0) {        \
    o = filll;                            \
  } else if (total_stretch[fill] != 0) {  \
    o = fill;                             \
  } else if (total_stretch[fil] != 0) {   \
    o = fil;                              \
  } else {                                \
    o = normal;                           \
  }

/* module 809 */
/* Determine the shrink order */
#define determine_shrink_order            \
  if (total_shrink[filll] != 0) {         \
    o = filll;                            \
  } else if (total_shrink[fill] != 0) {   \
    o = fill;                             \
  } else if (total_shrink[fil] != 0) {    \
    o = fil;                              \
  } else {                                \
    o = normal;                           \
  }

/* module 805 */
EXTERN integer pack_begin_line; /* source file line where the current paragraph or 
								   alignment began; a negative value denotes  alignment */
EXTERN boolean check_expand_pars (internal_font_number f);
EXTERN scaled char_stretch (internal_font_number f, eight_bits c);
EXTERN scaled char_shrink (internal_font_number f, eight_bits c);
EXTERN scaled get_kern (internal_font_number f, eight_bits lc, eight_bits rc);
EXTERN scaled kern_stretch (pointer p);
EXTERN scaled kern_shrink (pointer p);
EXTERN void do_subst_font (pointer p, int ex_ratio);
EXTERN pointer new_margin_kern (scaled w, pointer p, small_number side);
EXTERN pointer hpack (pointer p, scaled w, small_number m);
EXTERN pointer vpackage (pointer p, scaled h, small_number m, scaled l);

#define VPACK(a,b,c)                 vpackage(a,b,c,max_dimen)

EXTERN void append_to_vlist (pointer b);
EXTERN void pack_initialize (void);
