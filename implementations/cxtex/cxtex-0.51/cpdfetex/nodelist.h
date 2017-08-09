
/* module 133 */

/* 
 * From the computer's standpoint, \TeX's chief mission is to create
 * horizontal and vertical lists. We shall now investigate how the elements
 * of these lists are represented internally as nodes in the dynamic memory.
 * 
 * A horizontal or vertical list is linked together by |link| fields in
 * the first word of each node. Individual nodes represent boxes, glue,
 * penalties, or special things like discretionary hyphens; because of this
 * variety, some nodes are longer than others, and we must distinguish different
 * kinds of nodes. We do this by putting a `|type|' field in the first word,
 * together with the link and an optional `|subtype|'.
 */
#define type( arg )  mem [ arg ]. hh . b0
#define subtype( arg )  mem [ arg ]. hh . b1

/* module 148 */

/* \TeX\ makes use of the fact that |hlist_node|, |vlist_node|,
 * |rule_node|, |ins_node|, |mark_node|, |adjust_node|, |ligature_node|,
 * |disc_node|, |whatsit_node|, and |math_node| are at the low end of the
 * type codes, by permitting a break at glue in a list if and only if the
 * |type| of the previous node is less than |math_node|. Furthermore, a
 * node is discarded after a break if its type is |math_node| or~more.
 */
#define precedes_break( arg ) ( type ( arg ) <  math_node )
#define non_discardable( arg ) ( type ( arg ) <  math_node )

/* module 160 */

/* In fact, there are still more types coming. When we get to math formula
 * processing we will see that a |style_node| has |type=14|; and a number
 * of larger type codes will also be defined, for use in math mode only.
 */


/* module 161 */

/* Warning: If any changes are made to these data structure layouts, such as
 * changing any of the node sizes or even reordering the words of nodes,
 * the |copy_node_list| procedure and the memory initialization code
 * below may have to be changed. Such potentially dangerous parts of the
 * program are listed in the index under `data structure assumptions'.
 * 
 * However, other references to the nodes are made symbolically in terms of
 * the \.{WEB} macro definitions above, so that format changes will leave
 * \TeX's other algorithms intact.
 */

/* block 12: */

EXTERN integer font_in_short_display;

EXTERN void short_display (pointer p);
EXTERN pointer print_short_node (pointer p);

EXTERN void show_node_list (int p);

/* module 180 */

/* Since boxes can be inside of boxes, |show_node_list| is inherently recursive,
 * 
 * up to a given maximum number of levels. The history of nesting is indicated
 * by the current string, which will be printed at the beginning of each line;
 * the length of this string, namely |cur_length|, is the depth of nesting.
 * 
 * Recursive calls on |show_node_list| therefore use the following pattern:
 */
#define node_list_display( arg ) { append_char ( '.' ); show_node_list ( arg ); flush_char ;}

/* module 200 */
#define token_ref_count( arg )  info ( arg )

EXTERN void delete_token_ref (pointer p);

/* module 201 */
#define fast_delete_glue_ref(arg)  {             \
  if (  glue_ref_count ( arg ) ==  null  ) {     \
  free_node ( arg , glue_spec_size ); } else {   \
  decr ( glue_ref_count ( arg ));} }


EXTERN void delete_glue_ref (pointer p);

EXTERN void flush_node_list (pointer p);

/* module 203 */

/* 
 * Another recursive operation that acts on boxes is sometimes needed: The
 * procedure |copy_node_list| returns a pointer to another node list that has
 * the same structure and meaning as the original. Note that since glue
 * specifications and token lists have reference counts, we need not make
 * copies of them. Reference counts can never get too large to fit in a
 * halfword, since each pointer to a node is in a different memory address,
 * and the total number of memory addresses fits in a halfword.
 * 
 * (Well, there actually are also references from outside |mem|; if the
 * |save_stack| is made arbitrarily large, it would theoretically be possible
 * to break \TeX\ by overflowing a reference count. But who would want to do that?)
 */
#define add_token_ref( arg )  incr ( token_ref_count ( arg ))
#define add_glue_ref( arg )  incr ( glue_ref_count ( arg ))

EXTERN pointer copy_node_list (pointer p);


EXTERN integer node_type (pointer p);
