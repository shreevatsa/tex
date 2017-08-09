

/* module 697 */
#define output_one_char( arg ) {                              \
   pdf_check_new_font ( f );                                  \
   if (pdf_font_type [f] ==  virtual_font_type) {             \
      do_vf_packet ( f , arg );                               \
   } else  {                                                  \
      pdf_begin_string (f);                                   \
      pdf_print_char (f, arg );                               \
      adv_char_width (f, char_width (f,char_info(f,arg)));    \
   };                                                         \
  }



EXTERN integer              *vfpacketbase;

EXTERN void do_vf (void);
EXTERN void do_vf_packet  (internal_font_number f, eight_bits c);
EXTERN void vf_initialize (void);
EXTERN void vf_xmalloc (integer size);
