
/* module 1296 */
#define fam_in_range (( cur_fam  >= 0 )  && ( cur_fam  < 16 ))

/* module 1323 */
#define above_code 0
#define over_code 1
#define atop_code 2
#define delimited_code 3

/* module 1342 */
/* \TeX\ gets to the following part of the program when the first `\.\$' ending
 * a display has been scanned.
 */
#define check_for_dollar 	get_x_token();                                \
  if (cur_cmd != math_shift) {                                            \
	print_err ("Display math should end with $$");                        \
	help2 ("The `$' that I just saw supposedly matches a previous `$$'.", \
		   "So I shall assume that you typed `$$' both times.");          \
	back_error();                                                         \
  }

EXTERN void just_reverse (pointer p);
EXTERN void init_math (void);
EXTERN void start_eq_no (void);
EXTERN void scan_math (pointer p);
EXTERN void set_math_char (int c);
EXTERN void math_limit_switch (void);
EXTERN void scan_delimiter (pointer p, boolean r);
EXTERN void math_radical (void);
EXTERN void math_ac (void);

EXTERN void append_choices (void);
EXTERN pointer fin_mlist (pointer p);
EXTERN void build_choices (void);

EXTERN void sub_sup (void);
EXTERN void math_fraction (void);
EXTERN void math_left_right (void);

EXTERN void app_display (pointer j, pointer b, scaled d);
EXTERN void after_math (void);
EXTERN void insert_dollar_sign (void);

