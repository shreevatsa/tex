
#define TeXXeT_code 0

/* module 616 */
#define reversed  (min_quarterword  + 1)
#define dlist  (min_quarterword  + 2)
#define left_to_right 0
#define right_to_left 1
#define reflected (1  -  cur_dir)

/* module 1646 */
#define TeXXeT_state  eTeX_state ( TeXXeT_code )
#define TeXXeT_en ( TeXXeT_state  > 0 )

/* module 1651 */
#define put_LR( arg ) { temp_ptr   =  get_avail();\
                        info ( temp_ptr )  =  arg;\
                        link ( temp_ptr )  =  LR_ptr;\
                         LR_ptr   =  temp_ptr ; }
#define push_LR( arg )  put_LR ( end_LR_type ( arg ))
#define pop_LR { temp_ptr   =  LR_ptr;\
                 LR_ptr   =  link ( temp_ptr );\
                  free_avail ( temp_ptr ); }

/* module 1662 */
#define LR_dir( arg ) ( subtype ( arg )  /  R_code )

EXTERN pointer LR_temp;
EXTERN pointer LR_ptr;
EXTERN int LR_problems;
EXTERN small_number cur_dir;

EXTERN void xet_initialize(void);

EXTERN void remove_end_M (void);
EXTERN void insert_end_M (void);

/* module 1658 */
/* Initialize the LR stack, this will never match */
#define initialize_LR_stack { put_LR (before); }


/* module 1661 */
/* Report LR problems */
#define report_LR_problems		  {                          \
		    print_ln();                                      \
		    print_nl_string("\\endL or \\endR problem (");   \
		    print_int (LR_problems / 10000);                 \
		    zprint_string(" missing, ");                     \
		    print_int (LR_problems % 10000);                 \
		    zprint_string(" extra");                         \
		    LR_problems = 0;                                 \
		  }




/* module 1663 */
/* Check for LR anomalies at the end of |hlist_out| */
#define check_LR_after_hlist_out {         \
  while (info (LR_ptr) != before) {        \
	if (info (LR_ptr) > L_code)            \
	  LR_problems = LR_problems + 10000;   \
	pop_LR;                                \
  };                                       \
  pop_LR;                                  \
}

/* module 1664 */
#define edge_node  style_node
#define edge_node_size  style_node_size
#define edge_dist( arg )  depth ( arg )

EXTERN pointer new_edge (small_number s, scaled w);
EXTERN pointer reverse (pointer this_box, pointer t);

/* module 1665 */
/* Cases of |hlist_out| that arise in mixed direction text only */
#define hlist_edge_node 	{                 \
		  cur_h = cur_h + width (p);          \
		  left_edge = cur_h + edge_dist (p);  \
		  cur_dir = subtype (p);              \
		}

/* module 1666 */

/* We detach the hlist, start a new one consisting of just one kern node,
 * append the reversed list, and set the width of the kern node.
 */
/* Reverse the complete hlist and set the subtype to |reversed| */
#define reverse_the_hlist { save_h = cur_h;\
		  temp_ptr = p;\
		  p = new_kern (0);\
		  link (prev_p) = p;\
		  cur_h = 0;\
		  link (p) = reverse (this_box, null);\
		  width (p) = -cur_h;\
		  cur_h = save_h;\
		  subtype (this_box) = reversed; }


/* module 1675 */
/* Check for LR anomalies at the end of |ship_out| */
#define check_LR_after_ship_out  {                            \
	      if (LR_problems > 0) {                              \
			report_LR_problems;                               \
			print_char (')');                                 \
			print_ln();                                       \
		  };                                                  \
	      if ((LR_ptr != null) || (cur_dir != left_to_right)) \
			confusion ("LR3");                                \
          }

