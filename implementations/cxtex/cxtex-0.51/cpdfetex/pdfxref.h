
/* module 668 */

/* The first field contains information representing identifier of this object.
 * It is usally a number for most of object types, but it may be a string number
 * for named destination or named thread.
 * 
 * The second field of |obj_entry| contains link to the next
 * object in |obj_tab| if this object in linked in a list.
 * 
 * The third field holds the byte offset of the object in the output PDF file.
 * Objects that have been not written yet have this field set to zero. However
 * sometimes we have to use this field to store some info before the object is
 * written out.
 * 
 * The last field usually represents the pointer to some auxiliary data
 * structure depending on the object type; however it may be used as a counter as
 * well.
 */

#define inf_obj_tab_size ( 32000 ) /* min size of the cross-reference table for PDF output */
#define sup_obj_tab_size ( 8388607 ) /* max size of the cross-reference table for PDF output */
#define inf_dest_names_size ( 10000 ) /* min size of the destination names table for PDF output */
#define sup_dest_names_size ( 131072 ) /* max size of the destination names table for PDF output */
#define pdf_pdf_box_spec_media ( 0 )
#define pdf_pdf_box_spec_crop ( 1 )
#define pdf_pdf_box_spec_bleed ( 2 )
#define pdf_pdf_box_spec_trim ( 3 )
#define pdf_pdf_box_spec_art ( 4 )

#define obj_info( arg )  obj_tab [ arg ]. int0
#define obj_link( arg )  obj_tab [ arg ]. int1
#define obj_offset( arg )  obj_tab [ arg ]. int2
#define obj_aux( arg )  obj_tab [ arg ]. int3
#define is_obj_written( arg ) ( obj_offset ( arg )  != 0 )
#define obj_type_others 0
#define obj_type_page 1
#define obj_type_pages 2
#define obj_type_font 3
#define obj_type_outline 4
#define obj_type_dest 5
#define obj_type_obj 6
#define obj_type_xform 7
#define obj_type_ximage 8
#define obj_type_thread 9
#define head_tab_max  obj_type_thread
#define pages_tree_kids_max 6
#define name_tree_kids_max 32768
#define pdf_left( arg )  mem [ arg  + 1 ]. sc
#define pdf_top( arg )  mem [ arg  + 2 ]. sc
#define pdf_right( arg )  mem [ arg  + 3 ]. sc
#define pdf_bottom( arg )  mem [ arg  + 4 ]. sc
#define pdf_width( arg )  mem [ arg  + 1 ]. sc
#define pdf_height( arg )  mem [ arg  + 2 ]. sc
#define pdf_depth( arg )  mem [ arg  + 3 ]. sc
#define pdf_literal_data( arg )  link ( arg  + 1 )
#define pdf_literal_direct( arg )  info ( arg  + 1 )
#define pdf_refobj_node_size 2
#define pdf_obj_objnum( arg )  info ( arg  + 1 )
#define obj_data_ptr  obj_aux
#define pdfmem_obj_size 4
#define obj_obj_data( arg )  pdf_mem [ obj_data_ptr ( arg ) + 0 ]
#define obj_obj_is_stream( arg )  pdf_mem [ obj_data_ptr ( arg ) + 1 ]
#define obj_obj_stream_attr( arg )  pdf_mem [ obj_data_ptr ( arg ) + 2 ]
#define obj_obj_is_file( arg )  pdf_mem [ obj_data_ptr ( arg ) + 3 ]
#define pdf_refxform_node_size 5
#define pdf_xform_objnum( arg )  info ( arg  + 4 )
#define pdfmem_xform_size 6
#define obj_xform_width( arg )  pdf_mem [ obj_data_ptr ( arg ) + 0 ]
#define obj_xform_height( arg )  pdf_mem [ obj_data_ptr ( arg ) + 1 ]
#define obj_xform_depth( arg )  pdf_mem [ obj_data_ptr ( arg ) + 2 ]
#define obj_xform_box( arg )  pdf_mem [ obj_data_ptr ( arg ) + 3 ]
#define obj_xform_attr( arg )  pdf_mem [ obj_data_ptr ( arg ) + 4 ]
#define obj_xform_resources( arg )  pdf_mem [ obj_data_ptr ( arg ) + 5 ]
#define pdf_refximage_node_size 5
#define pdf_ximage_objnum( arg )  info ( arg  + 4 )
#define pdfmem_ximage_size 5
#define obj_ximage_width( arg )  pdf_mem [ obj_data_ptr ( arg ) + 0 ]
#define obj_ximage_height( arg )  pdf_mem [ obj_data_ptr ( arg ) + 1 ]
#define obj_ximage_depth( arg )  pdf_mem [ obj_data_ptr ( arg ) + 2 ]
#define obj_ximage_attr( arg )  pdf_mem [ obj_data_ptr ( arg ) + 3 ]
#define obj_ximage_data( arg )  pdf_mem [ obj_data_ptr ( arg ) + 4 ]
#define obj_annot_ptr  obj_aux
#define pdf_annot_node_size 7
#define pdf_annot_data( arg )  info ( arg  + 5 )
#define pdf_link_attr( arg )  info ( arg  + 5 )
#define pdf_link_action( arg )  link ( arg  + 5 )
#define pdf_annot_objnum( arg )  mem [ arg  + 6 ]. cint
#define pdf_action_page 0
#define pdf_action_goto 1
#define pdf_action_thread 2
#define pdf_action_user 3
#define pdf_action_size 3
#define pdf_action_type  type
#define pdf_action_named_id  subtype
#define pdf_action_id  link
#define pdf_action_file( arg )  info ( arg  + 1 )
#define pdf_action_new_window( arg )  link ( arg  + 1 )
#define pdf_action_page_tokens( arg )  info ( arg  + 2 )
#define pdf_action_user_tokens( arg )  info ( arg  + 2 )
#define pdf_action_refcount( arg )  link ( arg  + 2 )
#define pdfmem_outline_size 8
#define obj_outline_count  obj_info
#define obj_outline_ptr  obj_aux
#define obj_outline_title( arg )  pdf_mem [ obj_outline_ptr ( arg )]
#define obj_outline_parent( arg )  pdf_mem [ obj_outline_ptr ( arg ) + 1 ]
#define obj_outline_prev( arg )  pdf_mem [ obj_outline_ptr ( arg ) + 2 ]
#define obj_outline_next( arg )  pdf_mem [ obj_outline_ptr ( arg ) + 3 ]
#define obj_outline_first( arg )  pdf_mem [ obj_outline_ptr ( arg ) + 4 ]
#define obj_outline_last( arg )  pdf_mem [ obj_outline_ptr ( arg ) + 5 ]
#define obj_outline_action_objnum( arg )  pdf_mem [ obj_outline_ptr ( arg ) + 6 ]
#define obj_outline_attr( arg )  pdf_mem [ obj_outline_ptr ( arg ) + 7 ]
#define pdf_dest_xyz 0
#define pdf_dest_fit 1
#define pdf_dest_fith 2
#define pdf_dest_fitv 3
#define pdf_dest_fitb 4
#define pdf_dest_fitbh 5
#define pdf_dest_fitbv 6
#define pdf_dest_fitr 7
#define obj_dest_ptr  obj_aux
#define pdf_dest_node_size 7
#define pdf_dest_type( arg )  type ( arg  + 5 )
#define pdf_dest_named_id( arg )  subtype ( arg  + 5 )
#define pdf_dest_id( arg )  link ( arg  + 5 )
#define pdf_dest_xyz_zoom( arg )  info ( arg  + 6 )
#define pdf_dest_objnum( arg )  link ( arg  + 6 )
#define pdf_thread_node_size 7
#define pdf_thread_named_id( arg )  subtype ( arg  + 5 )
#define pdf_thread_id( arg )  link ( arg  + 5 )
#define pdf_thread_attr( arg )  info ( arg  + 6 )
#define obj_thread_first  obj_aux
#define pdfmem_bead_size 5
#define obj_bead_ptr  obj_aux
#define obj_bead_rect( arg )  pdf_mem [ obj_bead_ptr ( arg )]
#define obj_bead_page( arg )  pdf_mem [ obj_bead_ptr ( arg ) + 1 ]
#define obj_bead_next( arg )  pdf_mem [ obj_bead_ptr ( arg ) + 2 ]
#define obj_bead_prev( arg )  pdf_mem [ obj_bead_ptr ( arg ) + 3 ]
#define obj_bead_attr( arg )  pdf_mem [ obj_bead_ptr ( arg ) + 4 ]
#define obj_bead_data  obj_bead_rect
#define snap_glue_ptr( arg )  info ( arg  + 1 )


/* module 669 */
extern integer obj_tab_size;
extern obj_entry *obj_tab;
extern integer head_tab[(head_tab_max+1)];
extern integer obj_ptr;/* objects counter */
extern integer pdf_last_pages;/* pointer to most recently generated pages object */
extern integer pdf_last_page;/* pointer to most recently generated page object */
extern integer pdf_last_stream;/* pointer to most recently generated stream */
extern integer pdf_stream_length;/* length of most recently generated stream */
extern integer pdf_stream_length_offset; /* file offset of the last stream length */
extern integer ff; /* for use with |set_ff| */

EXTERN void pdfxref_initialize (void);

/* module 671 */
#define pdf_append_list( a,b ) { b  =  append_ptr (b,a);}

#define set_ff( arg ) {  if (  pdf_font_num [ arg ] < 0  )   {\
                             ff   =  -  pdf_font_num [ arg ];}\
                         else   ff   =  arg ;}

EXTERN void append_dest_name (str_number s, integer n);

EXTERN void pdf_create_obj (integer t, integer i);
EXTERN integer pdf_new_objnum (void);
EXTERN void pdf_begin_obj (integer i);
EXTERN void pdf_end_obj (void);
EXTERN void pdf_begin_dict (integer i);
EXTERN void pdf_end_dict (void);
EXTERN void pdf_new_obj (integer t, integer i);
EXTERN void pdf_new_dict (integer t, integer i);
EXTERN pointer append_ptr (pointer p, integer i);
EXTERN pointer pdf_lookup_list (pointer p, integer i);

/* module 673 */
EXTERN integer pdf_image_procset;/* collection of image types used in current page/form */
EXTERN boolean pdf_text_procset;/* mask of used ProcSet's in the current page/form */

EXTERN void pdf_print_fw_int (integer n, integer w);
EXTERN void pdf_int_entry (char *s, integer v);
EXTERN void pdf_int_entry_ln (char *s, integer v);
EXTERN void pdf_indirect_string (char *s, integer o);
EXTERN void pdf_indirect_string_ln (char *s, integer o);
EXTERN void pdf_print_str (str_number s);
EXTERN void pdf_print_str_ln (str_number s);
EXTERN void pdf_str_entry (char *s, str_number v);
EXTERN void pdf_str_entry_ln (char  *s, char *v);


