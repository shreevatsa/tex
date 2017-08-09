


/* string fixes: */

#define null_string 256

#define primitive_str(a,b,c)                       \
        if(noninit) {                              \
          slow_make_tex_string(a);                 \
         } else {                                  \
          primitive (slow_make_tex_string(a),b,c); \
         }

#define primitive_text(a,b) text(a) = slow_make_tex_string(b)

extern integer max_strings; /* maximum number of strings; must not exceed  |max_halfword| */ 
extern integer strings_free; /* strings available after format loaded */ 
extern integer string_vacancies; /* the minimum number of characters that should be available .. */
extern integer pool_size; /* maximum number of characters in strings, including all .. */
extern integer pool_free; /* pool space free after format loaded */ 


/* module 38 */
/*#define si( arg )  arg*/
/*#define so( arg )  arg*/

/* module 39 */
EXTERN packed_ASCII_code *str_pool; /* the characters */ 
EXTERN pool_pointer *str_start; /* the starting pointers */ 
EXTERN pool_pointer pool_ptr; /* first unused position in |str_pool| */
EXTERN str_number str_ptr; /* number of the current string being created */
EXTERN pool_pointer init_pool_ptr; /* the starting value of |pool_ptr| */
EXTERN str_number init_str_ptr; /* the starting value of |str_ptr| */

/* module 40 */

/* Several of the elementary string operations are performed using \.{WEB}
 * macros instead of \PASCAL\ procedures, because many of the
 * operations are done quite frequently and we want to avoid the
 * overhead of procedure calls. For example, here is
 * a simple macro that computes the length of a string.
 */
#define length(arg) (str_start[arg+1] -  str_start[arg])

/* module 41 */

/* The length of the current string is called |cur_length|:
 */
#define cur_length ( pool_ptr  -  str_start [ str_ptr ])

/* module 42 */

/* Strings are created by appending character codes to |str_pool|.
 * The |append_char| macro, defined here, does not check to see if the
 * value of |pool_ptr| has gotten too high; this test is supposed to be
 * made before |append_char| is used. There is also a |flush_char|
 * macro, which erases the last character appended.
 * 
 * To test if there is room to append |l| more characters to |str_pool|,
 * we shall write |str_room(l)|, which aborts \TeX\ and gives an
 * apologetic error message if there isn't enough room.
 */
#define append_char( arg ) { str_pool [ pool_ptr ]  =  arg ; incr ( pool_ptr );}
#define flush_char  decr ( pool_ptr )

#define str_room(arg) {                                                            \
  if (pool_ptr + arg > pool_size) {                                                \
    str_pool = xrealloc_array(str_pool,sizeof(packed_ASCII_code)*pool_size*2);     \
	if (str_pool == NULL) {                                                        \
	  overflow ("pool size", pool_size - init_pool_ptr);                           \
	} else {                                                                       \
      pool_size = pool_size*2;                                                     \
    }                                                                              \
  }                                                                                \
}

/* this is perhaps not as useful as the realloc in str_room. */

#define strings_room(arg) {                                                       \
  if (str_ptr + arg > max_strings) {                                              \
    if (max_strings*2 > max_halfword) {                                           \
     overflow ("number of strings", max_strings - init_str_ptr);                  \
    } else {                                                                      \
      str_start = xrealloc_array(str_start,sizeof(pool_pointer)*max_strings*2);   \
      if (str_start == NULL) {                                                    \
        overflow ("number of strings", max_strings - init_str_ptr);               \
      } else {                                                                    \
        max_strings= max_strings*2;                                               \
      }                                                                           \
    }                                                                             \
  }                                                                               \
}


/* module 44 */

/* To destroy the most recently made string, we say |flush_string|.
 */
#define flush_string { decr ( str_ptr ); pool_ptr   =  str_start [ str_ptr ];}

EXTERN str_number make_string (void);
EXTERN boolean str_eq_buf (str_number, int);
EXTERN boolean str_eq_str (str_number, str_number);
EXTERN str_number search_string (str_number);
EXTERN str_number slow_make_string (void);
EXTERN boolean get_strings_started(void);
EXTERN str_number   slow_make_tex_string (char *);
EXTERN string       gettexstring (str_number);
EXTERN void         checkpoolpointer (pool_pointer, size_t);

EXTERN void flush_str (str_number);
EXTERN boolean str_in_str (str_number, char *, int);
EXTERN boolean str_less_str (str_number, str_number);
EXTERN boolean substr_of_str (char *, char *);
EXTERN str_number get_nullstr (void);


EXTERN void print_current_string (void);
