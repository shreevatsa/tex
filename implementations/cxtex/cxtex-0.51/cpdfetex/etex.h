
/* module 1592 */
#define eTeX_state_base  (int_base  +  eTeX_state_code)
#define eTeX_state( arg )  eqtb [ eTeX_state_base  +  arg ]. cint
#define eTeX_version_code  eTeX_int
#define eTeX_revision_code 6

/* module 1598 */
#define eTeX_ex ( eTeX_mode  == 1 )
EXTERN unsigned char eTeX_mode; /* identifies compatibility and extended mode */

EXTERN boolean eTeX_enabled (boolean b, quarterword j, halfword k);

/* module 1604 */
#define every_eof  equiv ( every_eof_loc )

/* module 1606 */
EXTERN boolean *eof_seen; /* has eof been seen? */
EXTERN void group_trace (boolean e);

/* module 1609 */
#define current_group_level_code  (eTeX_int  + 1)
#define current_group_type_code  (eTeX_int  + 2)

/* module 1612 */
#define current_if_level_code  (eTeX_int  + 3)
#define current_if_type_code  (eTeX_int  + 4)
#define current_if_branch_code  (eTeX_int  + 5)

/* module 1615 */
#define font_char_wd_code  eTeX_dim
#define font_char_ht_code  (eTeX_dim  + 1)
#define font_char_dp_code  (eTeX_dim  + 2)
#define font_char_ic_code  (eTeX_dim  + 3)

/* module 1618 */
#define par_shape_length_code  (eTeX_dim  + 4)
#define par_shape_indent_code  (eTeX_dim  + 5)
#define par_shape_dimen_code  (eTeX_dim  + 6)

/* module 1621 */
#define show_groups 4

/* module 1630 */
#define show_tokens 5

/* module 1635 */
#define show_ifs 6

/* module 1637 */
#define print_if_line(arg)   if (arg != 0)  { zprint_string (" entered on line "); print_int (arg);}

EXTERN void etex_initialize (void) ;
EXTERN void dump_etex_stuff (void) ;

EXTERN void pseudo_start (void);
EXTERN boolean pseudo_input (void);
EXTERN void pseudo_close (void);

/* module 1718 */
EXTERN save_pointer *grp_stack; /* initial |cur_boundary| */
EXTERN pointer *if_stack; /* initial |cond_ptr| */

EXTERN void group_warning (void);

/* module 1720 */

/* This code scans the input stack in order to determine the type of the
 * current input file.
 */
#define set_w_for_reporting if (tracing_nesting > 0) {          \
	  while ((input_stack[base_ptr].state_field == token_list)  \
		 || (input_stack[base_ptr].index_field > i))            \
	    decr (base_ptr);                                        \
	  if (input_stack[base_ptr].name_field > 17)                \
	    w = true; }

EXTERN void file_warning (void);

/* module 1746 */
#define glue_stretch_order_code  (eTeX_int  + 6)
#define glue_shrink_order_code  (eTeX_int  + 7)
#define glue_stretch_code  (eTeX_dim  + 7)
#define glue_shrink_code  (eTeX_dim  + 8)

/* module 1750 */
#define mu_to_glue_code  eTeX_glue
#define glue_to_mu_code  eTeX_mu

/* module 1759 */
EXTERN halfword max_reg_num; /* largest allowed register number */
EXTERN char *max_reg_help_line; /* first line of help message */

/* module 1787 */
#define active_node_size_extended 5
#define active_short( arg )  mem [ arg  + 3 ]. sc
#define active_glue( arg )  mem [ arg  + 4 ]. sc

EXTERN pointer last_line_fill; /* the |par_fill_skip| glue node of the new paragraph*/
EXTERN boolean  do_last_line_fit; /* special algorithm for last line of paragraph? */
EXTERN small_number  active_node_size; /* number of words in active nodes */
EXTERN scaled fill_width[3]; /* infinite stretch components of |par_fill_skip| */
EXTERN scaled  best_pl_short[tight_fit]; /* |shortfall| corresponding to  |minimal_demerits| */
EXTERN scaled best_pl_glue[tight_fit]; /* corresponding glue stretch or shrink */

/* module 1804 */
EXTERN pointer disc_ptr[(vsplit_code+1)]; /* list pointers */

/* module 1804 */
#define tail_page_disc  disc_ptr [ copy_code ]
#define page_disc  disc_ptr [ last_box_code ]
#define split_disc  disc_ptr [ vsplit_code ]

/* module 1809 */
#define inter_line_penalties_ptr  equiv ( inter_line_penalties_loc )
#define club_penalties_ptr  equiv ( club_penalties_loc )
#define widow_penalties_ptr  equiv ( widow_penalties_loc )
#define display_widow_penalties_ptr  equiv ( display_widow_penalties_loc )
