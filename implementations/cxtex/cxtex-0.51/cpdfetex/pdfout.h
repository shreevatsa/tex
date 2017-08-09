

EXTERN void pdf_special (pointer p);
EXTERN str_number tokens_to_string (pointer p);
EXTERN void pdf_print_toks (pointer p);
EXTERN void pdf_print_toks_ln (pointer p);

EXTERN void pdf_hlist_out (void);
EXTERN void pdf_vlist_out (void);
EXTERN void pdf_ship_out (pointer p, boolean shipping_page);

/* module 737 */
#define delete_toks( arg ) { delete_token_ref ( arg ); arg   =  null ;}

EXTERN void pdf_write_obj (int n);
EXTERN void flush_whatsit_node (pointer p, small_number s);
EXTERN void pdf_write_image (int n);
EXTERN void pdf_print_rect_spec (pointer r);

EXTERN void ship_out (pointer p);

EXTERN void sort_dest_names (int l, int r);
EXTERN void pdf_fix_dest (int k);
EXTERN void pdf_print_info (void);

/* module 781 */
EXTERN str_number pdftex_banner; /* the complete banner */

EXTERN void finish_pdf_file (void);
