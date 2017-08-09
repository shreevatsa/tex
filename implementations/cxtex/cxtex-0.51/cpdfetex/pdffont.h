

/* mark a char in font */
#define pdf_mark_char(f, c) pdf_char_used[f][c/8] |= (1<<(c%8))

#define get_lp_code(f, c) \
    (pdf_font_lp_base[f] < 0 ? 0 : pdf_mem[pdf_font_lp_base[f] + c])

#define get_rp_code(f, c) \
    (pdf_font_rp_base[f] < 0 ? 0 : pdf_mem[pdf_font_rp_base[f] + c])

#define get_ef_code(f, c) \
    (pdf_font_ef_base[f] < 0 ? 0 : pdf_mem[pdf_font_ef_base[f] + c])


/* module 675 */
#define new_font_type 0
#define virtual_font_type 1
#define real_font_type 2
#define subst_font_type 3
#define pdf_init_font( arg ) { tmp_f   =  arg ; pdf_create_font_obj() ;}
#define pdf_check_vf( arg ) { tmp_f   =  arg ; do_vf() ;  \
                     if (pdf_font_type [ arg ] ==  virtual_font_type  )\
                     pdf_error_string( "font" , "command cannot be used with virtual font" );}

#define pdf_check_new_font( arg )   if (  pdf_font_type [ arg ] ==  new_font_type  ) \
                              { tmp_f   =  arg ; do_vf() ;}


/* module 676 */
extern eight_bits *pdf_font_type;/* the type of font */
extern str_number *pdf_font_attr;/* pointer to additional attributes */
extern internal_font_number *pdf_font_link;/* link to expanded fonts */
extern integer *pdf_font_stretch;/* limit of stretching */
extern integer *pdf_font_shrink;/* limit of shrinking */
extern integer *pdf_font_step;/* amount of one step */
extern integer *pdf_font_expand_factor;
extern integer *pdf_font_expand_ratio;
extern integer *pdf_font_lp_base;
extern integer *pdf_font_rp_base;
extern integer *pdf_font_ef_base;
extern internal_font_number tmp_f; /* for use with |pdf_init_font| */

/* module 677 */

#define copy_char_settings( arg )                        \
  if ( ( arg [ k ] < 0 )  && ( arg [ f ] >= 0 ) )  {     \
    i   =  pdf_get_mem (256 );                           \
  for ( j = 0 ; j <=  255 ; j++)                         \
    pdf_mem [i+j] = pdf_mem [arg[f]+j]; arg [k]  =  i ;}


EXTERN void set_lp_code (internal_font_number f, eight_bits c, int i);
EXTERN void set_rp_code (internal_font_number f, eight_bits c, int i);
EXTERN void set_ef_code (internal_font_number f, eight_bits c, int i);

EXTERN internal_font_number read_expand_font (internal_font_number f, scaled e);
EXTERN void set_expand_param (internal_font_number k, internal_font_number f, int e);
EXTERN integer get_expand_value (internal_font_number f, int e, int max_expand);
EXTERN integer get_expand_factor (internal_font_number f);
EXTERN internal_font_number new_ex_font (internal_font_number f, scaled e);
EXTERN internal_font_number expand_font (internal_font_number f, int e);
EXTERN void do_expand_font (void);

/* module 678 */
#define pdf_print_resname_prefix   if (  pdf_resname_prefix   != 0  )\
                                           pdf_print ( pdf_resname_prefix )
EXTERN void pdf_use_font (internal_font_number f, int fontnum);
EXTERN void pdf_create_font_obj (void);

/* module 680 */
extern char_used_array *pdf_char_used;	/* to mark used chars
								   TH pdf_char_used[][] ? */
extern char_map_array *pdf_char_map;	/* where to map chars 0..32 
								   TH pdf_char_map[][] ? */
extern scaled *pdf_font_size;/* used size of font in PDF file */
extern integer *pdf_font_num;/* mapping between internal font number in \TeX\ 
						 and font name defined in resources in PDF file */
extern integer *pdf_font_map;/* index in table of font mappings */
extern pointer pdf_font_list;/* list of used fonts in current page */
extern str_number pdf_resname_prefix;/* global prefix of resources name */
extern str_number last_tokens_string; /* the number of the most recently string
								  created by |tokens_to_string| */

EXTERN void pdffont_initialize (void);
EXTERN void pdffont_xmalloc (integer);
EXTERN void pdffont_initialize_init (integer);
