
/* module 32 */
#define term_in  stdin
#define term_out  stdout
/* module 33 */

/* Here is how to open the terminal files. |t_open_out| does nothing.
 * |t_open_in|, on the other hand, does the work of ``rescanning,'' or getting
 * any command line arguments the user has provided. It's defined in C.
 */
#define t_open_out

/* module 34 */

/* Sometimes it is necessary to synchronize the input/output mixture that
 * happens on the user's terminal, and three system-dependent
 * procedures are used for this
 * purpose. The first of these, |update_terminal|, is called when we want
 * to make sure that everything we have output to the terminal so far has
 * actually left the computer's internal buffers and been sent.
 * The second, |clear_terminal|, is called when we wish to cancel any
 * input that the user may have typed ahead (since we are about to
 * issue an unexpected error message). The third, |wake_up_terminal|,
 * is supposed to revive the terminal if the user has disabled it by
 * some instruction to the operating system. The following macros show how
 * these operations can be specified with {\mc UNIX}. |update_terminal|
 * does an |fflush|. |clear_terminal| is redefined
 * to do nothing, since the user should control the terminal.
 */
#define update_terminal  fflush ( term_out )
#define clear_terminal  do_something
#define wake_up_terminal  do_something

/* module 36 */

/* Different systems have different ways to get started. But regardless of
 * what conventions are adopted, the routine that initializes the terminal
 * should satisfy the following specifications:
 * 
 * \yskip\textindent{1)}It should open file |term_in| for input from the
 * terminal. (The file |term_out| will already be open for output to the
 * terminal.)
 * 
 * \textindent{2)}If the user has given a command line, this line should be
 * considered the first line of terminal input. Otherwise the
 * user should be prompted with `\.{**}', and the first line of input
 * should be whatever is typed in response.
 * 
 * \textindent{3)}The first line of input, which might or might not be a
 * command line, should appear in locations |first| to |last-1| of the
 * |buffer| array.
 * 
 * \textindent{4)}The global variable |loc| should be set so that the
 * character to be read next by \TeX\ is in |buffer[loc]|. This
 * character should not be blank, and we should have |loc<last|.
 * 
 * \yskip\noindent(It may be necessary to prompt the user several times
 * before a non-blank line comes in. The prompt is `\.{**}' instead of the
 * later `\.*' because the meaning is slightly different: `\.{\\input}' need
 * not be typed immediately after~`\.{**}'.)
 */
#define loc  cur_input . loc_field

EXTERN FILE *log_file; /* transcript of \TeX\ session */ 

/* module 71 */
#define prompt_input( arg ) { wake_up_terminal ; zprint_string ( arg ); term_input() ;}

EXTERN text_char *name_of_file;
EXTERN unsigned int name_length;

EXTERN ASCII_code *buffer;
EXTERN unsigned int first;
EXTERN unsigned int last;
EXTERN integer max_buf_stack; 

EXTERN void realloc_buffer (void);

#define input_ln(stream, flag) input_line (stream)


EXTERN boolean      input_line (FILE *f);

EXTERN boolean init_terminal (void);

EXTERN void term_input (void);

EXTERN str_number   getjobname(void);
EXTERN str_number   make_full_name_string(void);

EXTERN void begin_file_reading (void);
EXTERN void end_file_reading (void);

EXTERN str_number *source_filename_stack;
EXTERN str_number *full_source_filename_stack;

/* module 519 */
#define append_to_name( arg ) { c   =  arg ; incr ( k );  if (  k  <=  file_name_size  )   {\
                                name_of_file [ k ]  =  Xchr (c);} }
/* module 529 */
#define pack_cur_name  pack_file_name ( cur_name , cur_area , cur_ext )

EXTERN str_number cur_name; /* name of file just scanned */ 
EXTERN str_number cur_area; /* file area just scanned, or \.{""} */ 
EXTERN str_number cur_ext; /* file extension just scanned, or \.{""} */

EXTERN pool_pointer area_delimiter; /* the most recent `\./', if any */ 
EXTERN pool_pointer ext_delimiter; /* the most recent `\..', if any */

EXTERN integer format_default_length;
EXTERN char *TEX_format_default;

EXTERN boolean name_in_progress; /* is a file name being scanned? */ 
EXTERN str_number jobname; /* principal file name */ 
EXTERN boolean log_opened; /* has the transcript file been opened? */


EXTERN void begin_name (void);
EXTERN boolean more_name (ASCII_code c);
EXTERN void end_name (void);
EXTERN void pack_file_name (str_number n, str_number a, str_number e);
EXTERN void pack_buffered_name (small_number n, int a, int b);
EXTERN str_number make_name_string (void);
EXTERN str_number a_make_name_string ();
EXTERN str_number b_make_name_string ();
EXTERN str_number w_make_name_string ();
EXTERN void pack_job_name (char *s);
EXTERN void prompt_file_name (char *ss,char *es);

/* module 532 */
#define log_name  texmf_log_name
#define ensure_dvi_open   if (  output_file_name  == 0  )  {  if (jobname  == 0  )   open_log_file() ;\
                           pack_job_name ( ".dvi" ); while(   ! b_open_out ( dvi_file ) )\
                             prompt_file_name ( "file name for output" , ".dvi");\
                             output_file_name   =  b_make_name_string ( dvi_file );}

EXTERN FILE *dvi_file; /* the device-independent output goes here */ 
EXTERN str_number output_file_name; /* full name of the output file */ 
EXTERN str_number log_name; /* full name of the log file */


EXTERN void open_log_file (void);
EXTERN void ensure_pdf_open (void);
EXTERN void open_or_close_in (void);
EXTERN void close_files_and_terminate (void);


EXTERN void open_out_what (pointer p, small_number j);
EXTERN void close_out_what (FILE *j);

EXTERN void read_first_line (small_number m);
EXTERN void read_next_line (small_number m);

EXTERN boolean input_next_line (void);
EXTERN void start_input (void);

EXTERN int read_terminal ( void );

EXTERN void scan_file_name (void);

/* module 1812 */
EXTERN boolean stop_at_space; /* whether |more_name| returns false for space */

