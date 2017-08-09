
/* module 72 */

/* 
 * When something anomalous is detected, \TeX\ typically does something like this:
 * $$\vbox{\halign{#\hfil\cr
 * |print_err("Something anomalous has been detected");|\cr
 * |help3("This is the first line of my offer to help.")|\cr
 * |("This is the second line. I'm trying to")|\cr
 * |("explain the best way for you to proceed.");|\cr
 * |error;|\cr}}$$
 * A two-line help message would be given using |help2|, etc.; these informal
 * helps should use simple vocabulary that complements the words used in the
 * official error message that was printed. (Outside the U.S.A., the help
 * messages should preferably be translated into the local vernacular. Each
 * line of help is at most 60 characters long, in the present implementation,
 * so that |max_print_line| will not be exceeded.)
 * 
 * The |print_err| procedure supplies a `\.!' before the official message,
 * and makes sure that the terminal is awake if a stop is going to occur.
 * The |error| procedure supplies a `\..' after the official message, then it
 * shows the location of the error; and if |interaction=error_stop_mode|,
 * it also enters into a dialog with the user, during which time the help
 * message may be printed.
 */

#define help0                help_ptr   = 0
#define help1(a)           { help_ptr   = 1 ; help_line[0]=a; }
#define help2(a,b)         { help_ptr   = 2 ; help_line[1]=a; help_line[0]=b; }
#define help3(a,b,c)       { help_ptr   = 3 ; help_line[2]=a; help_line[1]=b; help_line[0]=c; }
#define help4(a,b,c,d)     { help_ptr   = 4 ; help_line[3]=a; help_line[2]=b; help_line[1]=c;\
                              help_line[0]=d; }
#define help5(a,b,c,d,e)   { help_ptr   = 5 ; help_line[4]=a; help_line[3]=b; help_line[2]=c;\
                              help_line[1]=d;  help_line[0]=e; }
#define help6(a,b,c,d,e,f) { help_ptr   = 6 ; help_line[5]=a; help_line[4]=b; help_line[3]=c;\
                              help_line[2]=d;  help_line[1]=e; help_line[0]=f; }

/* module 79 */

EXTERN char *help_line[6];
EXTERN unsigned char help_ptr;
EXTERN boolean use_err_help;

EXTERN void give_err_help (void);
EXTERN void debug_help (void);
EXTERN void help_initialize (void);



