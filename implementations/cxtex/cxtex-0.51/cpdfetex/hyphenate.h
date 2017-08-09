
/* module 1044 */
EXTERN unsigned char hyf[65]; /* odd values indicate discretionary hyphens */

/* module 1049 */
EXTERN small_number hyphen_passed; /* first hyphen in a ligature, if any */

/* module 1051 */
EXTERN halfword cur_l, cur_r; /* characters before and after the cursor */
EXTERN pointer cur_q; /* where a ligature should be detached */
EXTERN pointer lig_stack; /* unfinished business to the right of the cursor */
EXTERN boolean ligature_present; /* should a ligature node be made for |cur_l|? */
EXTERN boolean lft_hit, rt_hit; /* did we hit a ligature with a boundary character? */


/* module 1052 */
#define set_cur_r {           \
   if (j < n)  {              \
     cur_r = qi (hu[j + 1]);  \
   } else {                   \
     cur_r = bchar;           \
   }                          \
   if (odd (hyf [j])) {       \
     cur_rh  =  hchar;        \
   } else {                   \
     cur_rh  =  non_char;     \
   }                          \
 }

/* module 1065 */
#define trie_link(arg)  trie_trl [ arg ]
#define trie_char(arg)  trie_trc [ arg ]
#define trie_op(arg)  trie_tro [ arg ]

/* module 1078 */
#define set_cur_lang   if (language <= 0) { \
    cur_lang = 0;                           \
  } else if (language > 255) {              \
    cur_lang = 0;                           \
  } else {                                  \
   cur_lang = language;                     \
  }

EXTERN small_number  reconstitute (small_number j, small_number n, halfword bchar, halfword hchar);

EXTERN void  hyphenate (void);
