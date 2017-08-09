
EXTERN unsigned char selector;
EXTERN unsigned char old_setting;

EXTERN unsigned char dig[22];
EXTERN int tally;
EXTERN unsigned int term_offset;
EXTERN unsigned int file_offset;

EXTERN integer max_print_line;

/* module 54 */
/* module comment 54 */

/* 
 * Messages that are sent to a user's terminal and to the transcript-log file
 * are produced by several `|zprint|' procedures. These procedures will
 * direct their output to a variety of places, based on the setting of
 * the global variable |selector|, which has the following possible
 * values:
 * 
 * \yskip
 * \hang |term_and_log|, the normal setting, prints on the terminal and on the
 * 
 * transcript file.
 * 
 * \hang |log_only|, prints only on the transcript file.
 * 
 * \hang |term_only|, prints only on the terminal.
 * 
 * \hang |no_print|, doesn't zprint at all. This is used only in rare cases
 * 
 * before the transcript file is open.
 * 
 * \hang |pseudo|, puts output into a cyclic buffer that is used
 * 
 * by the |show_context| routine; when we get to that routine we shall discuss
 * 
 * the reasoning behind this curious mode.
 * 
 * \hang |new_string|, appends the output to the current string in the
 * 
 * string pool.
 * 
 * \hang 0 to 15, prints on one of the sixteen files for \.{\\write} output.
 * 
 * \yskip
 * \noindent The symbolic names `|term_and_log|', etc., have been assigned
 * numeric codes that satisfy the convenient relations |no_print+1=term_only|,
 * |no_print+2=log_only|, |term_only+2=log_only+1=term_and_log|.
 * 
 * Three additional global variables, |tally| and |term_offset| and
 * |file_offset|, record the number of characters that have been printed
 * since they were most recently cleared to zero. We use |tally| to record
 * the length of (possibly very long) stretches of printing; |term_offset|
 * and |file_offset|, on the other hand, keep track of how many characters
 * have appeared so far on the current line that has been output to the
 * terminal or to the transcript file, respectively.
 */


#define no_print 16
#define term_only 17
#define log_only 18
#define term_and_log 19
#define pseudo 20
#define new_string 21
#define max_selector 21
/* module 56 */

/* Macro abbreviations for output to the terminal and to the log file are
 * defined here for convenience. Some systems need special conventions
 * for terminal output, and it is possible to adhere to those conventions
 * by changing |wterm|, |wterm_ln|, and |wterm_cr| in this section.
 */
#define wterm( arg )  zwrite ( term_out , arg )
#define wlog( arg )  zwrite ( log_file , arg )

#define wterm_string(arg)  fprintf ( term_out , "%s", arg )
#define wlog_string(arg)  fprintf ( log_file , "%s", arg )

#define wterm_char(arg)  fprintf ( term_out , "%c", arg )
#define wlog_char(arg)  fprintf ( log_file , "%c", arg )

#define wterm_cr  fprintf ( term_out , "\n")
#define wlog_cr  fprintf ( log_file , "\n")

/* module 60 */
#define slow_print  zprint
/* module 68 */

/* Old versions of \TeX\ needed a procedure called |print_ASCII| whose function
 * is now subsumed by |zprint|. We retain the old name here as a possible aid to
 * future software arch\ae ologists.
 */
#define print_ASCII  zprint


EXTERN void print_ln (void);
EXTERN void print_visible_char (ASCII_code s);
EXTERN void print_char (ASCII_code s);
EXTERN void zprint_string (char *s);
EXTERN void zprint (int s);
EXTERN void print_nl (str_number s);
EXTERN void print_nl_string (char *s);
EXTERN void print_esc (str_number s);
EXTERN void print_esc_string (char * s);
EXTERN void print_the_digs (eight_bits k);
EXTERN void print_int (int n);
EXTERN void print_cs (int p);
EXTERN void sprint_cs (pointer p);
EXTERN void print_file_name (int n, int a, int e);
EXTERN void print_size (int s);
EXTERN void print_csnames (int hstart, int hfinish);
EXTERN void print_two (int n);
EXTERN void print_hex (int n);
EXTERN void print_roman_int (int n);
EXTERN void print_scaled (scaled s);
EXTERN void print_word (memory_word w);

EXTERN void begin_diagnostic (void);
EXTERN void end_diagnostic (boolean blank_line);


EXTERN void print_initialize (void);
