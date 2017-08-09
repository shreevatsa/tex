
extern integer error_line; /* width of context lines on terminal error messages */ 
extern integer half_error_line; /* width of first lines of contexts in terminal error
						 messages; should be between 30 and |error_line-15| */


/* module 73 */
#define batch_mode 0
#define nonstop_mode 1
#define scroll_mode 2
#define error_stop_mode 3
#define unspecified_mode 4
#define print_err( arg )     {  if (  interaction  ==  error_stop_mode  )   wake_up_terminal ;\
                                print_nl_string ( "! " ); zprint_string ( arg ); }
/* module 75 */

/* \TeX\ is careful not to call |error| when the zprint |selector| setting
 * might be unusual. The only possible values of |selector| at the time of
 * error messages are
 * 
 * \yskip\hang|no_print| (when |interaction=batch_mode|
 * 
 * and |log_file| not yet open);
 * 
 * \hang|term_only| (when |interaction>batch_mode| and |log_file| not yet open);
 * \hang|log_only| (when |interaction=batch_mode| and |log_file| is open);
 * \hang|term_and_log| (when |interaction>batch_mode| and |log_file| is open).
 */
#define initialize_selector if (interaction == batch_mode) {         \
		    selector = no_print;  } else { selector = term_only;  }

/* module 76 */
#define spotless 0
#define warning_issued 1
#define error_message_issued 2
#define fatal_error_stop 3

EXTERN unsigned char history; /* has the source input been clean so far? */ 
EXTERN char error_count; /* the number of scrolled errors since the last 
					 paragraph ended */

/* module 81 */
#define do_final_end  { update_terminal ; ready_already   = 0 ;\
                        if ( ( history   !=  spotless )  && ( history   !=  warning_issued )) {\
                        uexit (1 ); } else { uexit (0 );}; }


/* module 93 */

#ifdef TEXMF_DEBUG
#define succumb {  if (  interaction  ==  error_stop_mode  )   interaction   =  scroll_mode ;\
                   if (  log_opened  )   error() ;\
                   if (  interaction  >  batch_mode  )   debug_help() ;\
                   history   =  fatal_error_stop ; jump_out() ;}
#else /* TEXMF_DEBUG */
#define succumb {  if (  interaction  ==  error_stop_mode  )   interaction   =  scroll_mode ;\
                   if (  log_opened  )   error() ;\
                   history   =  fatal_error_stop ; jump_out() ;}
#endif /* TEXMF_DEBUG */

/* module 96 */
#define check_interrupt {  if (  interrupt   != 0  )   pause_for_instructions() ;}



EXTERN int bad;

/* module 73 */
EXTERN unsigned char interaction; /* current level of interaction */ 
EXTERN unsigned char interaction_option; /* set from command line */

/* module 76 */
EXTERN boolean deletions_allowed; /* is it safe for |error| to call |get_token|? */
EXTERN boolean set_box_allowed; /* is it safe to do a \.{\\setbox} assignment? */

/* module 96 */
EXTERN integer interrupt; /* should \TeX\ pause for instructions? */ 
EXTERN boolean OK_to_interrupt; /* should interrupts be observed? */


EXTERN void jump_out (void);
EXTERN void error (void);
EXTERN void fatal_error (char *s);
EXTERN void overflow (char *s, int n);
EXTERN void confusion (char *s);

EXTERN void normalize_selector (void);
EXTERN void int_error (int n);
EXTERN void pause_for_instructions (void);
EXTERN void runaway (void);
EXTERN void clear_for_error_prompt (void);
EXTERN void pdf_error (str_number t, str_number p);
EXTERN void pdf_error_string(char *t, char *p);
EXTERN void pdf_warning (str_number t, str_number p, boolean append_nl);
EXTERN void pdf_warning_string (char *t, char *p, boolean append_nl);
EXTERN void mu_error (void);
EXTERN void back_error (void);
EXTERN void ins_error (void);

EXTERN void you_cant (void);
EXTERN void report_illegal_case (void);
EXTERN void extra_right_brace (void);

EXTERN void cs_error (void);
EXTERN void error_initialize (void);
EXTERN void off_save (void);


EXTERN pool_pointer edit_name_start; /* where the filename to switch to starts */
EXTERN int edit_name_length, edit_line; /* what line to start editing at */
