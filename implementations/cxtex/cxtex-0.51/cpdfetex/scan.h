
/* module 404 */
#define get_nblank_nrelax_ncall  do { get_x_token(); } while ((cur_cmd == spacer) || (cur_cmd == relax))

/* module 406 */
#define get_nblank_ncall   do { get_x_token(); }  while (cur_cmd == spacer)

/* module 410 */
#define int_val 0
#define dimen_val 1
#define glue_val 2
#define mu_val 3
#define ident_val 4
#define tok_val 5
#define pdftex_revision_code 7
#define pdf_font_name_code 8
#define pdf_font_objnum_code 9
#define pdf_font_size_code 10
EXTERN integer cur_val; /* value returned by numeric scanners */ 
EXTERN unsigned char cur_val_level; /* the ``level'' of this value */

/* module 416 */
#define last_node_type_code  (glue_val  + 1)
#define input_line_no_code  (glue_val  + 2)
#define badness_code  (glue_val  + 3)
#define pdftex_version_code  (glue_val  + 4)
#define pdf_last_obj_code  (glue_val  + 5)
#define pdf_last_xform_code  (glue_val  + 6)
#define pdf_last_ximage_code  (glue_val  + 7)
#define pdf_last_ximage_pages_code  (glue_val  + 8)
#define pdf_last_annot_code  (glue_val  + 9)
#define pdf_last_x_pos_code  (glue_val  + 10)
#define pdf_last_y_pos_code  (glue_val  + 11)
#define pdf_last_demerits_code  (glue_val  + 12)
#define pdf_last_vbreak_penalty_code  (glue_val  + 13)
#define eTeX_int  (glue_val  + 14)
#define eTeX_dim  (eTeX_int  + 8)
#define eTeX_glue  (eTeX_dim  + 9)
#define eTeX_mu  (eTeX_glue  + 1)
#define eTeX_expr  (eTeX_mu  + 1)

/* module 421 */
#define max_dimen 1073741823


/* module 438 */
#define octal_token  (other_token  +  '\'')
#define hex_token  (other_token  +  '"')
#define alpha_token  (other_token  +  '`')
#define point_token  (other_token  +  '.')
#define continental_point_token  (other_token  +  ',')

EXTERN small_number radix; /* |scan_int| sets this to 8, 10, 16, or zero */

/* module 443 */
#define scan_optional_space  { get_x_token(); if (cur_cmd != spacer) back_input(); };


/* module 445 */
#define infinity 2147483647
#define zero_token  (other_token  +  '0')
#define A_token  (letter_token  +  'A')
#define other_A_token  (other_token  +  'A')

/* module 447 */
EXTERN glue_ord cur_order; /* order of infinity found by |scan_dimen| */

/* module 448 */
#define scan_normal_dimen  scan_dimen ( false , false , false )


EXTERN void scan_char_num (void);
EXTERN void scan_dimen (boolean mu, boolean inf, boolean shortcut);
EXTERN void scan_eight_bit_int (void);
EXTERN void scan_expr (void); 
EXTERN void scan_fifteen_bit_int (void);
EXTERN void scan_four_bit_int (void);
EXTERN void scan_general_text (void);
EXTERN void scan_glue (small_number level);
EXTERN void scan_initialize (void);
EXTERN void scan_int (void);
EXTERN boolean scan_keyword (char * str);
EXTERN void scan_left_brace (void);
EXTERN void scan_mu_glue (void);
EXTERN void scan_normal_glue (void);
EXTERN void scan_optional_equals (void);
EXTERN void scan_register_num (void);
EXTERN pointer scan_rule_spec (void);
EXTERN void scan_something_internal (small_number level, boolean negative);
EXTERN void scan_spec (group_code c, boolean three_codes);
EXTERN void scan_twenty_seven_bit_int (void);


