
/* module 159 */

/* You might think that we have introduced enough node types by now. Well,
 * almost, but there is one more: An |unset_node| has nearly the same format
 * as an |hlist_node| or |vlist_node|; it is used for entries in \.{\\halign}
 * or \.{\\valign} that are not yet in their final form, since the box
 * dimensions are their ``natural'' sizes before any glue adjustment has been
 * made. The |glue_set| word is not present; instead, we have a |glue_stretch|
 * field, which contains the total stretch of order |glue_order| that is
 * present in the hlist or vlist being boxed.
 * Similarly, the |shift_amount| field is replaced by a |glue_shrink| field,
 * containing the total shrink of order |glue_sign| that is present.
 * The |subtype| field is called |span_count|; an unset box typically
 * contains the data for |qo(span_count)+1| columns.
 * Unset nodes will be changed to box nodes when alignment is completed.
 */
#define unset_node 13
#define glue_stretch( arg )  mem [ arg  +  glue_offset ]. sc
#define glue_shrink  shift_amount
#define span_count  subtype


/* module 309 */
EXTERN integer align_state; /* group level with respect to current alignment */

EXTERN void align_initialize (void);
EXTERN void align_initialize_init (void);

/* module 914 */
#define preamble  link ( align_head )
#define align_stack_node_size 6

EXTERN void init_align (void);

/* module 924 */
#define span_code 256
#define cr_code 257
#define cr_cr_code  (cr_code  + 1)
#define end_template_token  (cs_token_flag  +  frozen_end_template)

EXTERN void align_peek (void);

/* module 941 */
#define span_node_size 2

EXTERN void fin_row (void);

EXTERN boolean fin_col (void);
EXTERN void insert_v_part (void);

EXTERN void align_error (void);
EXTERN void no_align_error (void);
EXTERN void omit_error (void);
EXTERN void do_endv (void);
