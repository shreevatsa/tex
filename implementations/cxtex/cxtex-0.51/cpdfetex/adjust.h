
/* module 142 */

/* An |adjust_node|, which occurs only in horizontal lists,
 * specifies material that will be moved out into the surrounding
 * vertical list; i.e., it is used to implement \TeX's `\.{\\vadjust}'
 * operation. The |adjust_ptr| field points to the vlist containing this
 * material.
 */
#define adjust_node 5
#define adjust_pre  subtype
#define adjust_ptr( arg )  mem [ arg  + 1 ]. cint

/* module 787 */
EXTERN pointer adjust_tail; /* tail of adjustment list */

/* module 797 */
EXTERN pointer pre_adjust_tail;

/* module 799 */
#define update_adjust_list( arg ) {   \
  link ( arg )  =  adjust_ptr ( p );  \
  while (  link ( arg )  !=  null  )  \
     arg   =  link ( arg ); }

EXTERN pointer hpack_adjust_node(pointer);
EXTERN pointer new_adjust_node (pointer, integer);

EXTERN void adjust_initialize (void);

