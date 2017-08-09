
/* module 358 */
#define no_expand_flag 257

/* module 360 */
#define end_line_char_inactive ( end_line_char  < 0 ) || ( end_line_char  > 255 )

/* module 371 */

/* The |expand| procedure and some other routines that construct token
 * lists find it convenient to use the following macros, which are valid only if
 * the variables |p| and |q| are reserved for token-list building.
 */
#define store_new_token(arg) { \
   q = get_avail();            \
   link (p) = q;               \
   info (q) = arg;             \
   p = q;                      \
 }

#define fast_store_new_token(arg) { \
   fast_get_avail (q);              \
   link (p) = q;                    \
   info (q) = arg;                  \
   p = q;                           \
 }


EXTERN pointer par_loc; /* location of `\.{\\par}' in |eqtb| */
EXTERN halfword par_token; /* token representing `\.{\\par}' */

EXTERN boolean force_eof; /* should the next \.{\\input} be aborted early? */


EXTERN void get_next (void);
EXTERN void get_token (void);
EXTERN void get_x_token (void);
EXTERN void x_token (void);
EXTERN void get_x_or_protected (void);
EXTERN void firm_up_the_line (void);


