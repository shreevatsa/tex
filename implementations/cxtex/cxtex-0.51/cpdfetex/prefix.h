

EXTERN void prefixed_command (void);

/* module 1359 */
#define global ( a  >= 4 )

#define DEFINE(a,b,c)              {if(global) { geq_define (a,b,c);}\
                                   else { eq_define (a,b,c);}}

#define WORD_DEFINE(a,b)           {if(global) { geq_word_define (a,b);}\
                                   else { eq_word_define (a,b);}}




/* module 1367 */
#define char_def_code 0
#define math_char_def_code 1
#define count_def_code 2
#define dimen_def_code 3
#define skip_def_code 4
#define mu_skip_def_code 5
#define toks_def_code 6
#define char_sub_def_code 7

/* module 1399 */
#define lp_code_base 2
#define rp_code_base 3
#define ef_code_base 4

EXTERN void new_font (small_number a);
EXTERN void new_interaction (void);


/* module 1411 */
EXTERN halfword after_token; /* zero, or a saved token */

EXTERN void prefix_initialize(void);
