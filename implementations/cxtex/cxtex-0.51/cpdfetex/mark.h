
/* module 141 */

/* A |mark_node| has a |mark_ptr| field that points to the reference count
 * of a token list that contains the user's \.{\\mark} text.
 * In addition there is a |mark_class| field that contains the mark class.
 */
#define mark_node 4
#define small_node_size 2
#define mark_ptr( arg )  link ( arg  + 1 )
#define mark_class( arg )  info ( arg  + 1 )

/* module 382 */
#define marks_code 5
#define top_mark_code 0
#define first_mark_code 1
#define bot_mark_code 2
#define split_first_mark_code 3
#define split_bot_mark_code 4
#define top_mark  cur_mark [ top_mark_code ]
#define first_mark  cur_mark [ first_mark_code ]
#define bot_mark  cur_mark [ bot_mark_code ]
#define split_first_mark  cur_mark [ split_first_mark_code ]
#define split_bot_mark  cur_mark [ split_bot_mark_code ]


EXTERN pointer cur_mark[split_bot_mark_code]; /* token lists for marks */

EXTERN void mark_initialize (void);

EXTERN void print_mark (int p);

EXTERN boolean do_marks (small_number a, small_number l, pointer q);
EXTERN void make_mark (void);
