/* module 487 */
#define unless_code 32
#define if_char_code 0
#define if_cat_code 1
#define if_int_code 2
#define if_dim_code 3
#define if_odd_code 4
#define if_vmode_code 5
#define if_hmode_code 6
#define if_mmode_code 7
#define if_inner_code 8
#define if_void_code 9
#define if_hbox_code 10
#define if_vbox_code 11
#define ifx_code 12
#define if_eof_code 13
#define if_true_code 14
#define if_false_code 15
#define if_case_code 16

/* module 1707 */
#define if_def_code 17
#define if_cs_code 18
#define if_font_char_code 19

/* module 489 */
#define if_node_size 2
#define if_line_field( arg )  mem [ arg  + 1 ]. cint
#define if_code 1
#define fi_code 2
#define else_code 3
#define or_code 4

EXTERN pointer cond_ptr; /* top of the condition stack */ 
EXTERN unsigned char if_limit; /* upper bound on |fi_or_else| codes */ 
EXTERN small_number cur_if; /* type of conditional being worked on */ 
EXTERN int if_line; /* line where that conditional began */
EXTERN integer skip_line; /* skipping began here */

EXTERN void conditional (void);
EXTERN void finish_conditional (void);
EXTERN void if_initialize (void);
