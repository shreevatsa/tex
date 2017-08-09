

/* module 54 */
extern ASCII_code  trick_buf[ssup_error_line]; /* circular buffer for pseudoprinting */ 
extern int trick_count; /* threshold for pseudoprinting, explained later */ 
extern int first_count; /* another variable for pseudoprinting */


/* module 207 */

/* 
 * Before we can go any further, we need to define symbolic names for the internal
 * code numbers that represent the various commands obeyed by \TeX. These codes
 * are somewhat arbitrary, but not completely so. For example, the command
 * codes for character types are fixed by the language, since a user says,
 * e.g., `\.{\\catcode \`\\\${} = 3}' to make \.{\char'44} a math delimiter,
 * and the command code |math_shift| is equal to~3. Some other codes have
 * been made adjacent so that |case| statements in the program need not consider
 * cases that are widely spaced, or so that |case| statements can be replaced
 * by |if| statements.
 * 
 * At any rate, here is the list, for future reference. First come the
 * ``catcode'' commands, several of which share their numeric codes with
 * ordinary commands when the catcode cannot emerge from \TeX's scanning routine.
 */
#define escape 0
#define relax 0
#define left_brace 1
#define right_brace 2
#define math_shift 3
#define tab_mark 4
#define car_ret 5
#define out_param 5
#define mac_param 6
#define sup_mark 7
#define sub_mark 8
#define ignore 9
#define endv 9
#define spacer 10
#define letter 11
#define other_char 12
#define active_char 13
#define par_end 13
#define match 13
#define comment 14
#define end_match 14
#define stop 14
#define invalid_char 15
#define delim_num 15
#define max_char_code 15

/* module 208 */

/* Next are the ordinary run-of-the-mill command codes. Codes that are
 * |min_internal| or more represent internal quantities that might be
 * expanded by `\.{\\the}'.
 */
#define char_num 16
#define math_char_num 17
#define mark 18
#define xray 19
#define make_box 20
#define hmove 21
#define vmove 22
#define un_hbox 23
#define un_vbox 24
#define remove_item 25
#define hskip 26
#define vskip 27
#define mskip 28
#define kern 29
#define mkern 30
#define leader_ship 31
#define halign 32
#define valign 33
#define no_align 34
#define vrule 35
#define hrule 36
#define insert 37
#define vadjust 38
#define ignore_spaces 39
#define after_assignment 40
#define after_group 41
#define break_penalty 42
#define start_par 43
#define ital_corr 44
#define accent 45
#define math_accent 46
#define discretionary 47
#define eq_no 48
#define left_right 49
#define math_comp 50
#define limit_switch 51
#define above 52
#define math_style 53
#define math_choice 54
#define non_script 55
#define vcenter 56
#define case_shift 57
#define message 58
#define extension 59
#define in_stream 60
#define begin_group 61
#define end_group 62
#define omit 63
#define ex_space 64
#define no_boundary 65
#define radical 66
#define end_cs_name 67
#define min_internal 68
#define char_given 68
#define math_given 69
#define last_item 70
#define max_non_prefixed_command 70

/* module 209 */

/* The next codes are special; they all relate to mode-independent
 * assignment of values to \TeX's internal registers or tables.
 * Codes that are |max_internal| or less represent internal quantities
 * that might be expanded by `\.{\\the}'.
 */
#define toks_register 71
#define assign_toks 72
#define assign_int 73
#define assign_dimen 74
#define assign_glue 75
#define assign_mu_glue 76
#define assign_font_dimen 77
#define assign_font_int 78
#define set_aux 79
#define set_prev_graf 80
#define set_page_dimen 81
#define set_page_int 82
#define set_box_dimen 83
#define set_shape 84
#define def_code 85
#define def_family 86
#define set_font 87
#define def_font 88
#define register_cmd 89
#define max_internal 89
#define advance 90
#define multiply 91
#define divide 92
#define prefix 93
#define let 94
#define shorthand_def 95
#define read_to_cs 96
#define def 97
#define set_box 98
#define hyph_data 99
#define set_interaction 100
#define max_command 100


/* module 302 */

/* We've already defined the special variable |loc==cur_input.loc_field|
 * in our discussion of basic input-output routines. The other components of
 * |cur_input| are defined in the same way:
 */
#define state  cur_input . state_field
#define index  cur_input . index_field
#define start  cur_input . start_field
#define limit  cur_input . limit_field
#define name  cur_input . name_field

/* module 303 */

/* Let's look more closely now at the control variables
 * (|state|,~|index|,~|start|,~|loc|,~|limit|,~|name|),
 * assuming that \TeX\ is reading a line of characters that have been input
 * from some file or from the user's terminal. There is an array called
 * |buffer| that acts as a stack of all lines of characters that are
 * currently being read from files, including all lines on subsidiary
 * levels of the input stack that are not yet completed. \TeX\ will return to
 * the other lines when it is finished with the present input file.
 * 
 * (Incidentally, on a machine with byte-oriented addressing, it might be
 * appropriate to combine |buffer| with the |str_pool| array,
 * letting the buffer entries grow downward from the top of the string pool
 * and checking that these two tables don't bump into each other.)
 * 
 * The line we are currently working on begins in position |start| of the
 * buffer; the next character we are about to read is |buffer[loc]|; and
 * |limit| is the location of the last character present. If |loc>limit|,
 * the line has been completely read. Usually |buffer[limit]| is the
 * |end_line_char|, denoting the end of a line, but this is not
 * true if the current line is an insertion that was entered on the user's
 * terminal in response to an error message.
 * 
 * The |name| variable is a string number that designates the name of
 * the current file, if we are reading a text file. It is zero if we
 * are reading from the terminal; it is |n+1| if we are reading from
 * input stream |n|, where |0<=n<=16|. (Input stream 16 stands for
 * an invalid stream number; in such cases the input is actually from
 * the terminal, under control of the procedure |read_toks|.)
 * Finally |18<=name<=19| indicates that we are reading a pseudo file
 * created by the \.{\\scantokens} command.
 * 
 * The |state| variable has one of three values, when we are scanning such
 * files:
 * $$\baselineskip 15pt\vbox{\halign{#\hfil\cr
 * 1) |state=mid_line| is the normal state.\cr
 * 2) |state=skip_blanks| is like |mid_line|, but blanks are ignored.\cr
 * 3) |state=new_line| is the state at the beginning of a line.\cr}}$$
 * These state values are assigned numeric codes so that if we add the state
 * code to the next character's command code, we get distinct values. For
 * example, `|mid_line+spacer|' stands for the case that a blank
 * space character occurs in the middle of a line when it is not being
 * ignored; after this case is processed, the next value of |state| will
 * be |skip_blanks|.
 */
#define mid_line 1
#define skip_blanks (2  +  max_char_code)
#define new_line (3  +  max_char_code  +  max_char_code)

/* module 304 */
#define terminal_input ( name  == 0 )
#define cur_file  input_file [ index ]

/* module 305 */
#define skipping 1
#define defining 2
#define matching 3
#define aligning 4
#define absorbing 5

/* module 307 */

/* However, all this discussion about input state really applies only to the
 * case that we are inputting from a file. There is another important case,
 * namely when we are currently getting input from a token list. In this case
 * |state=token_list|, and the conventions about the other state variables
 * are different:
 * 
 * \yskip\hang|loc| is a pointer to the current node in the token list, i.e.,
 * the node that will be read next. If |loc=null|, the token list has been
 * fully read.
 * 
 * \yskip\hang|start| points to the first node of the token list; this node
 * may or may not contain a reference count, depending on the type of token
 * list involved.
 * 
 * \yskip\hang|token_type|, which takes the place of |index| in the
 * discussion above, is a code number that explains what kind of token list
 * is being scanned.
 * 
 * \yskip\hang|name| points to the |eqtb| address of the control sequence
 * being expanded, if the current token list is a macro.
 * 
 * \yskip\hang|param_start|, which takes the place of |limit|, tells where
 * the parameters of the current macro begin in the |param_stack|, if the
 * current token list is a macro.
 * 
 * \yskip\noindent The |token_type| can take several values, depending on
 * where the current token list came from:
 * 
 * \yskip
 * \hang|parameter|, if a parameter is being scanned;
 * \hang|u_template|, if the \<u_j> part of an alignment template is being scanned;
 * \hang|v_template|, if the \<v_j> part of an alignment template is being scanned;
 * \hang|backed_up|, if the token list being scanned has been inserted as `to be read again'.
 * \hang|inserted|, if the token list being scanned has been inserted as
 *    the text expansion of a \.{\\count} or similar variable;
 * \hang|macro|, if a user-defined control sequence is being scanned;
 * \hang|output_text|, if an \.{\\output} routine is being scanned;
 * \hang|every_par_text|, if the text of \.{\\everypar} is being scanned;
 * \hang|every_math_text|, if the text of \.{\\everymath} is being scanned;
 * \hang|every_display_text|, if the text of \.{\\everydisplay} is being scanned;
 * \hang|every_hbox_text|, if the text of \.{\\everyhbox} is being scanned;
 * \hang|every_vbox_text|, if the text of \.{\\everyvbox} is being scanned;
 * \hang|every_job_text|, if the text of \.{\\everyjob} is being scanned;
 * \hang|every_cr_text|, if the text of \.{\\everycr} is being scanned;
 * \hang|mark_text|, if the text of a \.{\\mark} is being scanned;
 * \hang|write_text|, if the text of a \.{\\write} is being scanned.
 * 
 * \yskip\noindent
 * The codes for |output_text|, |every_par_text|, etc., are equal to a constant
 * plus the corresponding codes for token list parameters |output_routine_loc|,
 * |every_par_loc|, etc. The token list begins with a reference count if and
 * only if |token_type>=macro|.
 */
#define token_list 0
#define token_type  index
#define param_start  limit
#define parameter 0
#define u_template 1
#define v_template 2
#define backed_up 3
#define inserted 4
#define macro 5
#define output_text 6
#define every_par_text 7
#define every_math_text 8
#define every_display_text 9
#define every_hbox_text 10
#define every_vbox_text 11
#define every_job_text 12
#define every_cr_text 13
#define every_eof_text 14
#define mark_text 15
#define write_text 16
#define pdf_pages_attr_text 17
#define pdf_page_attr_text 18
#define pdf_page_resources_text 19

/* module 316 */
#define set_trick_count { first_count = tally; trick_count = tally + 1 + error_line - half_error_line;\
                          if (  trick_count  <  error_line  )   trick_count   =  error_line ;}

/* module 321 */

/* 
 * The following subroutines change the input status in commonly needed ways.
 * 
 * First comes |push_input|, which stores the current state and creates a
 * new level (having, initially, the same properties as the old).
 */
#define push_input {  if (  input_ptr  >   (unsigned)max_in_stack  )  { max_in_stack   =  input_ptr ;\
                         if (  input_ptr  ==  (unsigned)stack_size  )   overflow ( "input stack size" , stack_size );};\
                    	input_stack [ input_ptr ]  =  cur_input ;  \
                        incr ( input_ptr );}
/* module 322 */

/* And of course what goes up must come down.
 */
#define pop_input { decr ( input_ptr ); cur_input   =  input_stack [ input_ptr ]; }

/* module 323 */
#define back_list( arg )  begin_token_list ( arg , backed_up )
#define ins_list( arg )  begin_token_list ( arg , inserted )


EXTERN eight_bits cur_cmd; /* current command set by |get_next| */ 
EXTERN halfword cur_chr; /* operand of current command */ 
EXTERN pointer cur_cs; /* control sequence found here, zero if none found */ 
EXTERN halfword cur_tok; /* packed representative of |cur_cmd| and |cur_chr| */

EXTERN in_state_record *input_stack;
EXTERN unsigned int input_ptr; /* first unused location of |input_stack| */ 
EXTERN unsigned int max_in_stack; /* largest value of |input_ptr| when pushing */ 
EXTERN in_state_record cur_input; /* the ``top'' input state, according to convention (1) */

EXTERN unsigned int in_open; /* the number of lines in the buffer, less one */ 
EXTERN unsigned int open_parens; /* the number of open text files */ 
EXTERN FILE **input_file;
EXTERN integer line; /* current line number in the current source file */ 
EXTERN integer *line_stack;

EXTERN unsigned char scanner_status; /* can a subfile end now? */ 
EXTERN pointer warning_index; /* identifier relevant to non-|normal| scanner status */
EXTERN pointer def_ref; /* reference count of token list being defined */

EXTERN pointer *param_stack; /* token list pointers for parameters */
EXTERN unsigned int param_ptr; /* first unused entry in |param_stack| */ 
EXTERN int max_param_stack; /* largest value of |param_ptr|, will be|<=param_size+9|*/

EXTERN unsigned int base_ptr; /* shallowest level shown by |show_context| */

EXTERN void print_cmd_chr (quarterword cmd, halfword chr_code);
EXTERN void show_cur_cmd_chr (void);

EXTERN void show_context (void);

EXTERN void begin_token_list (pointer p, quarterword t);
EXTERN void end_token_list (void);

EXTERN void back_input (void);

EXTERN void cmdchr_initialize (void);

EXTERN void print_input_stack (void);
