
/* module 592 */
EXTERN integer total_pages; /* the number of pages that have been shipped out */
EXTERN scaled max_v; /* maximum height-plus-depth of pages shipped so far */
EXTERN scaled max_h; /* maximum width of pages shipped so far */
EXTERN int dead_cycles; /* recent outputs that didn't ship anything out */
EXTERN boolean doing_leaders; /* are we inside a leader box? */

EXTERN quarterword c;
EXTERN internal_font_number f; /* character and font in current |char_node| */

EXTERN pointer g; /* current glue specification */
EXTERN scaled rule_ht, rule_dp, rule_wd; /* size of current rule being output */
EXTERN int lq, lr; /* quantities used in calculations for leaders */

/* module 593 */
EXTERN void dvi_initialize (void);

/* module 595 */
EXTERN eight_bits *dvi_buf; /* buffer for \.{DVI} output */

/* module 615 */
EXTERN void hlist_out (void);

/* module 616 */
EXTERN scaled cur_h, cur_v; /* \TeX\ thinks we are here */
EXTERN int cur_s; /* current depth of output box nesting, initially $-1$ */

/* module 629 */
EXTERN void vlist_out (void);

/* module 638 */
EXTERN void dvi_ship_out (pointer p);

/* module 642 */
EXTERN void finish_dvi_file(void);

/* module 1564 */
EXTERN void special_out (pointer p);
