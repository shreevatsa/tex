/* module 157 */

/* A |penalty_node| specifies the penalty associated with line or page
 * breaking, in its |penalty| field. This field is a fullword integer, but
 * the full range of integer values is not used: Any penalty |>=10000| is
 * treated as infinity, and no break will be allowed for such high values.
 * Similarly, any penalty |<=-10000| is treated as negative infinity, and a
 * break will be forced.
 */
#define penalty_node 12
#define inf_penalty  inf_bad
#define eject_penalty  (-inf_penalty)
#define penalty( arg )  mem [ arg  + 1 ]. cint

EXTERN pointer new_penalty (int m);


EXTERN void append_penalty (void);
