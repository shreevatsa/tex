/* module 1125 */
#define inserts_only 1
#define box_there 2

/* module 1126 */
#define page_ins_node_size 4
#define inserting 0
#define split_up 1
#define broken_ptr( arg )  link ( arg  + 1 )
#define broken_ins( arg )  info ( arg  + 1 )
#define last_ins_ptr( arg )  link ( arg  + 2 )
#define best_ins_ptr( arg )  info ( arg  + 2 )

/* module 1127 */
#define page_goal  page_so_far [0]
#define page_total  page_so_far [1]
#define page_shrink  page_so_far [6]
#define page_depth  page_so_far [7]

/* module 1140 */
#define contrib_tail  nest [0 ]. tail_field

/* module 1125 */
EXTERN pointer page_tail; /* the final node on the current page */
EXTERN unsigned char page_contents; /* what is on the current page so far? */
EXTERN scaled page_max_depth; /* maximum box depth on page being built */
EXTERN pointer best_page_break; /* break here to get the best page known so far */
EXTERN int least_page_cost; /* the score for this currently best page */
EXTERN scaled best_size; /* its |page_goal| */

/* module 1127 */
EXTERN scaled page_so_far[8]; /* height and glue of the current page */
EXTERN pointer last_glue; /* used to implement \.{\\lastskip} */
EXTERN int last_penalty; /* used to implement \.{\\lastpenalty} */
EXTERN scaled last_kern; /* used to implement \.{\\lastkern} */
EXTERN int last_node_type; /* used to implement \.{\\lastnodetype} */
EXTERN int insert_penalties; /* sum of the penalties for held-over insertions */

/* module 1134 */
EXTERN boolean output_active; /* are we in the midst of an output routine? */

EXTERN void buildpage_initialize_init (void);
EXTERN void buildpage_initialize (void);
EXTERN void start_new_page (void);

EXTERN void print_totals (void);
EXTERN void box_error (eight_bits n);
EXTERN void build_page (void);
