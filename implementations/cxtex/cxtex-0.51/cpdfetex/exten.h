
/* module 146 */

/* A |whatsit_node| is a wild card reserved for extensions to \TeX. The
 * |subtype| field in its first word says what `\\{whatsit}' it is, and
 * implicitly determines the node size (which must be 2 or more) and the
 * format of the remaining words. When a |whatsit_node| is encountered
 * in a list, special actions are invoked; knowledgeable people who are
 * careful not to mess up the rest of \TeX\ are able to make \TeX\ do new
 * things by adding code at the end of the program. For example, there
 * might be a `\TeX nicolor' extension to specify different colors of ink,
 * 
 * and the whatsit node might contain the desired parameters.
 * 
 * The present implementation of \TeX\ treats the features associated with
 * `\.{\\write}' and `\.{\\special}' as if they were extensions, in order to
 * illustrate how such routines might be coded. We shall defer further
 * discussion of extensions until the end of this program.
 */
#define whatsit_node 8


/* module 1486 */

/* First let's consider the format of whatsit nodes that are used to represent
 * the data associated with \.{\\write} and its relatives. Recall that a whatsit
 * has |type=whatsit_node|, and the |subtype| is supposed to distinguish
 * different kinds of whatsits. Each node occupies two or more words; the
 * exact number is immaterial, as long as it is readily determined from the
 * |subtype| or other data.
 * 
 * We shall introduce five |subtype| values here, corresponding to the
 * control sequences \.{\\openout}, \.{\\write}, \.{\\closeout}, \.{\\special}, and
 * \.{\\setlanguage}. The second word of I/O whatsits has a |write_stream| field
 * that identifies the write-stream number (0 to 15, or 16 for out-of-range and
 * positive, or 17 for out-of-range and negative).
 * In the case of \.{\\write} and \.{\\special}, there is also a field that
 * points to the reference count of a token list that should be sent. In the
 * case of \.{\\openout}, we need three words and three auxiliary subfields
 * to hold the string numbers for name, area, and extension.
 */
#define write_node_size 2
#define open_node_size 3
#define open_node 0
#define write_node 1
#define close_node 2
#define special_node 3
#define language_node 4
#define what_lang( arg )  link ( arg  + 1 )
#define what_lhm( arg )  type ( arg  + 1 )
#define what_rhm( arg )  subtype ( arg  + 1 )
#define write_tokens( arg )  link ( arg  + 1 )
#define write_stream( arg )  info ( arg  + 1 )
#define open_name( arg )  link ( arg  + 1 )
#define open_area( arg )  info ( arg  + 2 )
#define open_ext( arg )  link ( arg  + 2 )

/* module 1487 */
EXTERN FILE * write_file[16];
EXTERN boolean write_open[18];

/* module 1489 */
#define immediate_code 4
#define set_language_code 5

/* module 1490 */
EXTERN pointer write_loc; /* |eqtb| address of \.{\\write} */

EXTERN void do_extension (void);

EXTERN void new_whatsit (small_number s, small_number w);
EXTERN void new_write_whatsit (small_number w);

EXTERN void print_write_whatsit (char *s, pointer p);

/* module 1558 */
#define adv_past( arg )   if (  subtype ( arg ) ==  language_node  )  {\
   cur_lang   =  what_lang ( arg ); l_hyf   =  what_lhm ( arg ); r_hyf   =  what_rhm ( arg );}

EXTERN void write_out (pointer p);

/* module 1567 */
#define end_write_token  cs_token_flag  +  end_write

EXTERN void out_what (pointer p);


EXTERN void fix_language (void);

EXTERN void exten_initialize (void);

EXTERN boolean shell_enabled_p;

/* module 1553 */
/* begin expansion of Make a partial copy of the whatsit node |p| and make 
   |r| point to it; set |words| to the number of initial words not yet copied */
#define do_a_partial_whatsit_copy               \
		switch (subtype (p)) {                  \
		case open_node:                         \
		  r = get_node (open_node_size);        \
		  words = open_node_size;               \
		  break;					            \
		case write_node:					    \
		case special_node:					    \
		  r = get_node (write_node_size);       \
		  add_token_ref (write_tokens (p));	    \
		  words = write_node_size;			    \
		  break;							    \
		case close_node:                        \
		case language_node:					    \
		  r = get_node (small_node_size);	    \
		  words = small_node_size;			    \
		  break;                                \
		case pdf_literal_node:					\
		  r = get_node (write_node_size);		\
		  add_token_ref (pdf_literal_data (p));	\
		  words = write_node_size;				\
		  break;								\
		case pdf_refobj_node:					\
		  r = get_node (pdf_refobj_node_size);	\
		  words = pdf_refobj_node_size;			\
		  break;								\
		case pdf_refxform_node:					\
		  r = get_node (pdf_refxform_node_size);\
		  words = pdf_refxform_node_size;         \
		  break;								  \
		case pdf_refximage_node:				  \
		  r = get_node (pdf_refximage_node_size); \
		  words = pdf_refximage_node_size;		  \
		  break;								  \
		case pdf_annot_node:					  \
		  r = get_node (pdf_annot_node_size);	  \
		  add_token_ref (pdf_annot_data (p));	  \
		  words = pdf_annot_node_size;			  \
		  break;								  \
		case pdf_start_link_node:				  \
		  r = get_node (pdf_annot_node_size);         \
		  pdf_height (r) = pdf_height (p);			  \
		  pdf_depth (r) = pdf_depth (p);			  \
		  pdf_width (r) = pdf_width (p);			  \
		  pdf_link_attr (r) = pdf_link_attr (p);	  \
		  if (pdf_link_attr (r) != null)			  \
			add_token_ref (pdf_link_attr (r));		  \
		  pdf_link_action (r) = pdf_link_action (p);  \
		  add_action_ref (pdf_link_action (r));		  \
		  break;									  \
		case pdf_end_link_node:						  \
		  r = get_node (small_node_size);			  \
		  break;                                      \
		case pdf_dest_node:							  \
		  r = get_node (pdf_dest_node_size);		  \
		  if (pdf_dest_named_id (p) > 0)			  \
			add_token_ref (pdf_dest_id (p));		  \
		  words = pdf_dest_node_size;				  \
		  break;									  \
		case pdf_thread_node:						  \
		case pdf_start_thread_node:					  \
		  r = get_node (pdf_thread_node_size);		  \
		  if (pdf_thread_named_id (p) > 0)			  \
			add_token_ref (pdf_thread_id (p));		  \
		  if (pdf_thread_attr (p) != null)            \
			add_token_ref (pdf_thread_attr (p));	  \
		  words = pdf_thread_node_size;				  \
		  break;									  \
		case pdf_end_thread_node:					  \
		  r = get_node (small_node_size);			  \
		  break;									  \
		case pdf_save_pos_node:						  \
		case pdf_snap_ref_point_node:				  \
		  r = get_node (small_node_size);			  \
		  break;									  \
		case pdf_snap_x_node:						  \
		case pdf_snap_y_node:                         \
		  add_glue_ref (snap_glue_ptr (p));           \
		  r = get_node (small_node_size);             \
		  words = small_node_size;                    \
		  break;                                      \
		default:                                      \
		  confusion ("ext2");                         \
		}
