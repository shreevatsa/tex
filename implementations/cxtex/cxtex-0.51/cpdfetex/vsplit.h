
/* module 1114 */
#define active_height  active_width
#define cur_height  active_height [1 ]
#define set_height_zero( arg )  active_height [ arg ]  = 0

/* module 1119 */
#define deplorable 100000

/* module 1115 */
EXTERN integer last_vbreak_penalty;

/* module 1116 */
EXTERN scaled best_height_plus_depth; /* height of the best box, without stretching or shrinking */

EXTERN pointer prune_page_top (pointer p, boolean s);
EXTERN pointer vert_break (pointer p, scaled h, scaled d);
EXTERN pointer vsplit (halfword n, scaled h);

EXTERN void vsplit_initialize (void);
