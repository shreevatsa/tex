
/* module 22 */

/* Some of the ASCII codes without visible characters have been given symbolic
 * names in this program because they are used with a special meaning.
 */
#define null_code 0
#define carriage_return 13
#define invalid_code 127

#define chr(x)          (x)
#define ord(x)          (x)

/* all of TeX's routines now use the Web2c |Xchr| and |Xord| macros,
 * making it easier to possibly change the underlying structure, if
 * desired at some point in the future.
 */

#define Xchr(x) xchr[x]
#define Xord(x) xord[x]

/* module 49 */
#define char_cannot_be_printed  !is_printable[k]
EXTERN boolean is_printable[256]; /* use \.{\^\^} notation? */
EXTERN ASCII_code xord[256];
EXTERN text_char xchr[256];

EXTERN void xordchr_initialize(void);

