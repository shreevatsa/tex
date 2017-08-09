
/* module 653 */

/* 
 * We use the similiar subroutines to handle the output buffer for
 * PDF output. When compress is used, the state of writing to buffer
 * is held in |zip_write_state|. We must write the header of PDF
 * output file in initialization to ensure that it will be the first
 * written bytes.
 */

#define pdf_buf_size ( 16384 ) /* size of the PDF buffer */

/* module 654 */
#define pdf_offset ( pdf_gone  +  pdf_ptr )
#define no_zip 0
#define zip_writing 1
#define zip_finish 2
#define pdf_quick_out(arg) { pdf_buf [pdf_ptr] = arg; incr (pdf_ptr); }
#define pdf_room( arg ) {                              \
   if (  pdf_buf_size  -  arg  < 0  )                  \
     overflow ( "PDF output buffer" , pdf_buf_size );  \
   if (  arg  +  pdf_ptr  >=  pdf_buf_size  )          \
     pdf_flush() ;                                     \
 }
#define pdf_out( arg ) {  if (pdf_ptr + 1 >= pdf_buf_size ) pdf_flush();\
                          pdf_quick_out ( arg );}

EXTERN integer fixed_output; /* fixed output format */
EXTERN FILE *pdf_file; /* the PDF output file */
EXTERN eight_bits pdf_buf[pdf_buf_size]; /* the PDF buffer */
EXTERN integer pdf_ptr; /* pointer to the first unused byte in the PDF buffer */
EXTERN integer pdf_gone; /* number of bytes that were flushed to output */
EXTERN integer pdf_save_offset; /* to save |pdf_offset| */
EXTERN integer zip_write_state; /* which state of compression we are in */
EXTERN integer fixed_pdf_minor_version; /* fixed minor part of the pdf version */
EXTERN boolean  pdf_minor_version_has_been_written; /* flag if the pdf version has been written */

EXTERN void pdflowlevel_initialize (void);

EXTERN void check_and_set_pdfoptionpdfminorversion (void);

EXTERN void pdf_flush (void) ;
EXTERN void pdf_begin_stream (void);
EXTERN void pdf_end_stream (void);

EXTERN void remove_last_space (void);
EXTERN void pdf_print_octal (integer n);
EXTERN void pdf_print_char (internal_font_number f, integer c);
EXTERN void pdf_print (str_number s);
EXTERN void pdf_print_string (char * s);

EXTERN void literal (str_number s, boolean reset_origin, boolean is_special, boolean warn);
EXTERN void pdf_print_int (integer n);
EXTERN void pdf_print_two (integer n);

/* module 660 */
#define pdf_new_line_char 10
#define pdf_print_nl  pdf_out ( pdf_new_line_char )
#define pdf_print_ln( arg ) { pdf_print ( arg ); pdf_print_nl ;}
#define pdf_print_ln_string( arg ) { pdf_print_string ( arg ); pdf_print_nl ;}
#define pdf_print_int_ln( arg ) { pdf_print_int ( arg ); pdf_print_nl ;}

/* module 661 */
#define max_integer 2147483647
#define call_func( arg ) {  if (  arg   != 0  )   do_nothing; }

EXTERN scaled one_bp; /* scaled value corresponds to 1bp */
EXTERN scaled one_hundred_bp; /* scaled value corresponds to 100bp */
EXTERN scaled one_hundred_inch; /* scaled value corresponds to 100in */
EXTERN integer ten_pow[10]; /* $10^0..10^9$ */
EXTERN integer scaled_out; /* amount of |scaled| that was taken out in |divide_scaled| */
EXTERN boolean init_pdf_output;

EXTERN void pdf_print_real (integer m, integer d);
EXTERN void pdf_print_bp (scaled s);
EXTERN void pdf_print_mag_bp (scaled s);

