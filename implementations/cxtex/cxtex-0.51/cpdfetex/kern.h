
/* module 155 */

/* A |kern_node| has a |width| field to specify a (normally negative)
 * amount of spacing. This spacing correction appears in horizontal lists
 * between letters like A and V when the font designer said that it looks
 * better to move them closer together or further apart. A kern node can
 * also appear in a vertical list, when its `|width|' denotes additional
 * spacing in the vertical direction. The |subtype| is either |normal| (for
 * kerns inserted from font information or math mode calculations) or |explicit|
 * (for kerns inserted from \.{\\kern} and \.{\\/} commands) or |acc_kern|
 * (for kerns inserted from non-math accents) or |mu_glue| (for kerns
 * inserted from \.{\\mkern} specifications in math formulas).
 */
#define kern_node 11
#define explicit 1
#define acc_kern 2
#define margin_kern_node 40
#define margin_kern_node_size 3
#define margin_char( arg )  info ( arg  + 2 )
#define left_side 0
#define right_side 1

EXTERN pointer new_kern (scaled w);


EXTERN void append_kern (void);
