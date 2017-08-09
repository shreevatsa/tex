
/* module 644 */
/* 
 * When \pdfTeX{} starts without ``init'', it reads a number of parameters
 * from the config file before starting input. The values from the config file
 * thus overwrite \TeX{} parameters that have been set in the format. We want
 * to use the codes corresponding to \pdfTeX{} parameters
 * (e.g.~|cfg_output_code|) in~C as well, so we must define the following
 * constants.
 */
#define cfg_output_code ( pdf_output_code )
#define cfg_adjust_spacing_code ( pdf_adjust_spacing_code )
#define cfg_compress_level_code ( pdf_compress_level_code )
#define cfg_decimal_digits_code ( pdf_decimal_digits_code )
#define cfg_move_chars_code ( pdf_move_chars_code )
#define cfg_image_resolution_code ( pdf_image_resolution_code )
#define cfg_pk_resolution_code ( pdf_pk_resolution_code )
#define cfg_unique_resname_code ( pdf_unique_resname_code )
#define cfg_protrude_chars_code ( pdf_protrude_chars_code )
#define cfg_h_origin_code ( pdf_h_origin_code )
#define cfg_v_origin_code ( pdf_v_origin_code )
#define cfg_page_height_code ( pdf_page_height_code )
#define cfg_page_width_code ( pdf_page_width_code )
#define cfg_link_margin_code ( pdf_link_margin_code )
#define cfg_dest_margin_code ( pdf_dest_margin_code )
#define cfg_thread_margin_code ( pdf_thread_margin_code )
#define cfg_pdf12_compliant_code ( pdf_thread_margin_code+1 )
#define cfg_pdf13_compliant_code ( cfg_pdf12_compliant_code+1 )
#define cfg_pdf_minor_version_code ( pdf_option_pdf_minor_version_code )
#define cfg_always_use_pdf_pagebox_code ( 71 )
/* must be the same as the definition in epdf.h */
#define cfg_pdf_option_pdf_inclusion_errorlevel_code ( pdf_option_pdf_inclusion_errorlevel_code )

/* module 645 */
EXTERN void do_mag_cfg_dimen_pars (void);

/* module 646 */

EXTERN void read_values_from_config_file (void);

/* module 647 */
#define flushable(arg) ( arg  ==  str_ptr  - 1 )

#define flush_fontname_k(arg) if ((arg != font_name [k]) &&  \
  (flushable (arg) || flushable (font_name [k]))) {          \
    if (flushable (arg)) { arg = font_name [k]; }            \
    else  { font_name [k] = arg ; flush_string ;}            \
  }

#define is_valid_char(arg) ((font_bc[f] <= arg)                      \
                            && (arg <= font_ec[f])                   \
                            && char_exists (orig_char_info(f,arg)))

/* module 648 */

#define active_width_field( arg )  active_width [ arg ]
#define cur_active_width_field( arg )  cur_active_width [ arg ]
#define break_width_field( arg )  break_width [ arg ]
#define prev_active_width_field( arg )  prev_active_width [ arg ]

/* module 649 */

/* Sometimes it is neccesary to allocate memory for PDF output that cannot
 * be deallocated then, so we use |pdf_mem| for this purpose.
 */
#define inf_pdf_mem_size ( 32000 ) /* min size of the |pdf_mem| array */
#define sup_pdf_mem_size ( 524288 ) /* max size of the |pdf_mem| array */

EXTERN pointer get_nullcs (void);
EXTERN pointer get_nullptr (void);

EXTERN void short_display_n (int p, int m);
EXTERN integer show_line_breaking (void);
EXTERN pointer mi (pointer p);
EXTERN pointer ml (pointer p);

/* module 650 */
EXTERN integer pdf_mem_size;
EXTERN integer *pdf_mem;
EXTERN integer pdf_mem_ptr;

EXTERN void pdfbasic_initialize (void) ;

EXTERN integer pdf_get_mem (int s);
