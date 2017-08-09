
/* module 138 */

/* A |rule_node| stands for a solid black rectangle; it has |width|,
 * |depth|, and |height| fields just as in an |hlist_node|. However, if
 * any of these dimensions is $-2^{30}$, the actual value will be determined
 * by running the rule up to the boundary of the innermost enclosing box.
 * This is called a ``running dimension.'' The |width| is never running in
 * an hlist; the |height| and |depth| are never running in a~vlist.
 */
#define rule_node 2
#define rule_node_size 4
#define null_flag  (-1073741824)
#define is_running( arg ) ( arg  ==  null_flag )

EXTERN pointer new_rule (void);


EXTERN void print_rule_dimen (scaled d);
