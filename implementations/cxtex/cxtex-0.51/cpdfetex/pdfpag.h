/* module 665 */
#define pdf_x( arg ) (( arg ) -  pdf_origin_h )
#define pdf_y( arg ) ( pdf_origin_v  - ( arg ))

EXTERN internal_font_number pdf_f; /* the current font in PDF output page */
EXTERN scaled pdf_h; /* current horizontal coordinate in PDF output page */
EXTERN scaled pdf_v; /* current vertical coordinate in PDF output page */
EXTERN scaled pdf_last_h; /* last horizontal coordinate in PDF output page */
EXTERN scaled pdf_last_v; /* last vertical coordinate in PDF output page */
EXTERN scaled pdf_origin_h; /* current horizontal origin in PDF output page */
EXTERN scaled pdf_origin_v; /* current vertical origin in PDF output page */
EXTERN boolean pdf_font_changed;/* current font has been changed? */
EXTERN scaled min_bp_val;
EXTERN integer fixed_pk_resolution;
EXTERN integer fixed_decimal_digits;
EXTERN integer pk_scale_factor;

EXTERN void pdf_set_origin (void) ;
EXTERN void pdf_begin_text (void);
EXTERN void pdf_end_string (void) ;
EXTERN void pdf_print_font_tag (internal_font_number f);
EXTERN void pdf_begin_string (internal_font_number f);
EXTERN void pdf_end_text (void) ;
EXTERN void pdf_set_rule (scaled x, scaled y, scaled w, scaled h);
EXTERN void pdf_rectangle (scaled, scaled , scaled , scaled);
