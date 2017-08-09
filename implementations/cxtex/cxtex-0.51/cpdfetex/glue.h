
/* module 149 */

/* A |glue_node| represents glue in a list. However, it is really only
 * a pointer to a separate glue specification, since \TeX\ makes use of the
 * fact that many essentially identical nodes of glue are usually present.
 * If |p| points to a |glue_node|, |glue_ptr(p)| points to
 * another packet of words that specify the stretch and shrink components, etc.
 * 
 * Glue nodes also serve to represent leaders; the |subtype| is used to
 * distinguish between ordinary glue (which is called |normal|) and the three
 * kinds of leaders (which are called |a_leaders|, |c_leaders|, and |x_leaders|).
 * The |leader_ptr| field points to a rule node or to a box node containing the
 * leaders; it is set to |null| in ordinary glue nodes.
 * 
 * Many kinds of glue are computed from \TeX's ``skip'' parameters, and
 * it is helpful to know which parameter has led to a particular glue node.
 * Therefore the |subtype| is set to indicate the source of glue, whenever
 * it originated as a parameter. We will be defining symbolic names for the
 * parameter numbers later (e.g., |line_skip_code=0|, |baseline_skip_code=1|,
 * etc.); it suffices for now to say that the |subtype| of parametric glue
 * will be the same as the parameter number, plus~one.
 * 
 * In math formulas there are two more possibilities for the |subtype| in a
 * glue node: |mu_glue| denotes an \.{\\mskip} (where the units are scaled \.{mu}
 * instead of scaled \.{pt}); and |cond_math_glue| denotes the `\.{\\nonscript}'
 * feature that cancels the glue node immediately following if it appears
 * in a subscript.
 */
#define glue_node 10
#define cond_math_glue 98
#define mu_glue 99
#define a_leaders 100
#define c_leaders 101
#define x_leaders 102
#define glue_ptr  llink
#define leader_ptr  rlink

/* module 150 */
#define glue_spec_size 4
#define glue_ref_count( arg )  link ( arg )
#define stretch( arg )  mem [ arg  + 2 ]. sc
#define shrink( arg )  mem [ arg  + 3 ]. sc
#define stretch_order  type
#define shrink_order  subtype
#define fil 1
#define fill 2
#define filll 3

EXTERN pointer new_spec (pointer p);
EXTERN pointer new_param_glue (small_number n);
EXTERN pointer new_glue (pointer q);
EXTERN pointer new_skip_param (small_number n);

/* module 224 */
/* Region 3 of |eqtb| contains the 256 \.{\\skip} registers, as well as the
 * glue parameters defined here. It is important that the ``muskip''
 * parameters have larger numbers than the others.
 */

/* module 1203 */
#define fil_code 0
#define fill_code 1
#define ss_code 2
#define fil_neg_code 3
#define skip_code 4
#define mskip_code 5

EXTERN void print_glue (scaled d, int order, char *s);
EXTERN void print_spec (int p, char *s);
EXTERN void app_space (void);
EXTERN void append_glue (void);

/* module 1645 */
#define add_glue( arg )  arg   =  arg  +  do_glue ( this_box , p )

EXTERN scaled do_glue (pointer this_box, pointer p);
