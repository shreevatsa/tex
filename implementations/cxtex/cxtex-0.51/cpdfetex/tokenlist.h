/* module 289 */

/* 
 * A \TeX\ token is either a character or a control sequence, and it is
 * 
 * represented internally in one of two ways: (1)~A character whose ASCII
 * code number is |c| and whose command code is |m| is represented as the
 * number $2^8m+c$; the command code is in the range |1<=m<=14|. (2)~A control
 * sequence whose |eqtb| address is |p| is represented as the number
 * |cs_token_flag+p|. Here |cs_token_flag=@t$2^{12}-1$@>| is larger than
 * $2^8m+c$, yet it is small enough that |cs_token_flag+p< max_halfword|;
 * thus, a token fits comfortably in a halfword.
 * 
 * A token |t| represents a |left_brace| command if and only if
 * |t<left_brace_limit|; it represents a |right_brace| command if and only if
 * we have |left_brace_limit<=t<right_brace_limit|; and it represents a |match| or
 * |end_match| command if and only if |match_token<=t<=end_match_token|.
 * The following definitions take care of these token-oriented constants
 * and a few others.
 */
#define cs_token_flag 4095
#define left_brace_token 256
#define left_brace_limit 512
#define right_brace_token 512
#define right_brace_limit 768
#define math_shift_token 768
#define tab_token 1024
#define out_param_token 1280
#define space_token 2592
#define letter_token 2816
#define other_token 3072
#define match_token 3328
#define end_match_token 3584
#define protected_token 3585


EXTERN void show_token_list (int p, int q, int l);
EXTERN void token_show (pointer p);
EXTERN void print_meaning (void);

/* section 27: */
/* module 468 */
#define number_code 0
#define roman_numeral_code 1
#define string_code 2
#define meaning_code 3
#define font_name_code 4
#define job_name_code 5
/* module 480 */
#define closed 2
#define just_open 1

EXTERN pointer str_toks (pool_pointer b);
EXTERN pointer the_toks (void);
EXTERN void ins_the_toks (void);
EXTERN void conv_toks (void);
EXTERN pointer scan_toks (boolean macro_def, boolean xpand);

EXTERN FILE *read_file[16]; /* used for \.{\\read} */ 
EXTERN unsigned char read_open[17]; /* state of |read_file[n]| */


EXTERN void read_toks (int n, pointer r, halfword j);
EXTERN pointer concat_tokens (pointer q, pointer r);

EXTERN void tokenlist_initialize (void);
