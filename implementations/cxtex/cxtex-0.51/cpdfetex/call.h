
EXTERN unsigned int global_long_state; /* governs the acceptance of \.{\\par} */
#define set_long_state(a) global_long_state=a
#define get_long_state()  global_long_state

EXTERN void macro_call (void);
EXTERN void expand(void);
EXTERN void insert_relax (void);

/* module 373 */
#define complain_missing_csname { print_err ("Missing ");\
			print_esc_string ("endcsname");\
			zprint_string(" inserted");\
			help2 ("The control sequence marked <to be read again> should",\
				   "not appear between \\csname and \\endcsname.");\
			back_error(); }
