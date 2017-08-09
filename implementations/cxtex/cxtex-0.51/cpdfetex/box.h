
/* module 135 */

/* An |hlist_node| stands for a box that was made from a horizontal list.
 * Each |hlist_node| is seven words long, and contains the following fields
 * (in addition to the mandatory |type| and |link|, which we shall not
 * mention explicitly when discussing the other node types): The |height| and
 * |width| and |depth| are scaled integers denoting the dimensions of the
 * box. There is also a |shift_amount| field, a scaled integer indicating
 * how much this box should be lowered (if it appears in a horizontal list),
 * or how much it should be moved to the right (if it appears in a vertical
 * list). There is a |list_ptr| field, which points to the beginning of the
 * list from which this box was fabricated; if |list_ptr| is |null|, the box
 * is empty. Finally, there are three fields that represent the setting of
 * the glue: |glue_set(p)| is a word of type |glue_ratio| that represents
 * the proportionality constant for glue setting; |glue_sign(p)| is
 * |stretching| or |shrinking| or |normal| depending on whether or not the
 * glue should stretch or shrink or remain rigid; and |glue_order(p)|
 * specifies the order of infinity to which glue setting applies (|normal|,
 * |fil|, |fill|, or |filll|). The |subtype| field is not used.
 */
#define hlist_node 0
#define box_node_size 7
#define width_offset 1
#define depth_offset 2
#define height_offset 3
#define width( arg )  mem [ arg  +  width_offset ]. sc
#define depth( arg )  mem [ arg  +  depth_offset ]. sc
#define height( arg )  mem [ arg  +  height_offset ]. sc
#define shift_amount( arg )  mem [ arg  + 4 ]. sc
#define list_offset 5
#define list_ptr( arg )  link ( arg  +  list_offset )
#define glue_order( arg )  subtype ( arg  +  list_offset )
#define glue_sign( arg )  type ( arg  +  list_offset )
#define normal 0
#define stretching 1
#define shrinking 2
#define glue_offset 6
#define glue_set( arg )  mem[arg+glue_offset].gr

EXTERN pointer new_null_box (void);

/* module 137 */

/* A |vlist_node| is like an |hlist_node| in all respects except that it
 * contains a vertical list.
 */
#define vlist_node 1


/* module 181 */
EXTERN integer depth_threshold; /* maximum nesting depth in box displays */ 
EXTERN int breadth_max; /* maximum number of items shown at the same list level */

/* module 1216 */
#define box_flag 1073741824
#define global_box_flag 1073774592
#define ship_out_flag 1073807360
#define leader_flag 1073807361
#define box_code 0
#define copy_code 1
#define last_box_code 2
#define vsplit_code 3
#define vtop_code 4

EXTERN void show_box (pointer);
EXTERN void begin_box (int);
EXTERN void scan_box (int);
EXTERN void package (small_number);
EXTERN void unpackage (void);
