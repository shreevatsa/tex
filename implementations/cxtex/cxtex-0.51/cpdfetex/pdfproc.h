
/* module 1489 */
#define pdf_literal_node 6
#define pdf_obj_code 7
#define pdf_refobj_node 8
#define pdf_xform_code 9
#define pdf_refxform_node 10
#define pdf_ximage_code 11
#define pdf_refximage_node 12
#define pdf_annot_node 13
#define pdf_start_link_node 14
#define pdf_end_link_node 15
#define pdf_outline_code 16
#define pdf_dest_node 17
#define pdf_thread_node 18
#define pdf_start_thread_node 19
#define pdf_end_thread_node 20
#define pdf_save_pos_node 21
#define pdf_snap_ref_point_node 22
#define pdf_snap_x_node 23
#define pdf_snap_y_node 24
#define pdf_line_snap_x_code 25
#define pdf_line_snap_y_code 26
#define pdf_info_code 27
#define pdf_catalog_code 28
#define pdf_names_code 29
#define pdf_font_attr_code 30
#define pdf_include_chars_code 31
#define pdf_font_expand_code 32
#define pdf_map_file_code 33
#define pdf_trailer_code 34

/* module 1500 */

/* The following macros are needed for further manipulation with whatsit nodes
 * for \pdfTeX{} extensions (copying, destroying etc.)
 */
#define add_action_ref( arg )  incr ( pdf_action_refcount ( arg ))

#define delete_action_ref(arg)  {                          \
  if (pdf_action_refcount (arg) ==  null)  {               \
    if (pdf_action_type (arg) ==  pdf_action_user) {       \
      delete_token_ref (pdf_action_user_tokens (arg)) ;    \
    } else  {  if (pdf_action_file (arg) != null)          \
        delete_token_ref ( pdf_action_file ( arg ));       \
	  if (pdf_action_type (arg) ==  pdf_action_page) {     \
       delete_token_ref ( pdf_action_page_tokens ( arg )); \
	  } else { if (pdf_action_named_id (arg) > 0)          \
          delete_token_ref ( pdf_action_id ( arg ));       \
     }; free_node ( arg , pdf_action_size );               \
   }; } else { decr ( pdf_action_refcount ( arg )); }; }

/* module 1501 */
#define scan_pdf_ext_toks  scan_toks ( false , true );

EXTERN void check_pdfoutput (char *s);

/* module 1503 */
extern integer pdf_last_obj;

EXTERN void pdf_check_obj (int t, int n);

/* module 1507 */
extern integer pdf_last_xform;

/* module 1510 */
extern integer pdf_last_ximage;
extern integer pdf_last_ximage_pages;
extern pointer alt_rule;
extern integer pdf_last_pdf_box_spec;

EXTERN void scale_image (int n);

EXTERN void scan_pdf_box_spec (void);
EXTERN void scan_alt_rule (void);
EXTERN void scan_image (void);

/* module 1515 */

EXTERN integer find_obj (int t, int i, small_number byname);
EXTERN integer get_obj (int t, int i, small_number byname);

EXTERN pointer scan_action (void);

EXTERN void new_annot_whatsit (small_number w, small_number s);

/* module 1517 */
EXTERN integer pdf_last_annot;

EXTERN integer outline_list_count (pointer p);

EXTERN void warn_dest_dup (int id, small_number byname, char *s1,char *s2);

EXTERN void pdf_include_chars (void);

/* module 1576 */
extern scaled cur_page_width; /* width of page being shipped */
extern scaled cur_page_height; /* height of page being shipped */
extern scaled cur_h_offset; /* horizontal offset of page being shipped */
extern scaled cur_v_offset; /* vertical offset of page being shipped */
extern pointer pdf_obj_list; /* list of objects in the current page */
extern pointer pdf_xform_list; /* list of forms in the current page */
extern pointer pdf_ximage_list; /* list of images in the current page */
extern int pdf_link_level; /* depth of nesting of box containing link annotation */
extern pointer last_link; /* pointer to the last link annotation */
extern scaled pdf_link_ht, pdf_link_dp,  pdf_link_wd; /* dimensions of the last link annotation */
extern pointer last_thread; /* pointer to the last thread */
extern scaled pdf_thread_ht, pdf_thread_dp, pdf_thread_wd; /* dimensions of the last thread */
extern halfword pdf_last_thread_id;/* identifier of the last thread */
extern boolean pdf_last_thread_named_id;/* is identifier of the last thread named */
extern int pdf_thread_level; /* depth of nesting of box containing the last thread */
extern pointer pdf_annot_list; /* list of annotations in the current page */
extern pointer pdf_link_list; /* list of link annotations in the current page */
extern pointer pdf_dest_list; /* list of destinations in the current page */
extern pointer pdf_bead_list; /* list of thread beads in the current page */
extern int pdf_obj_count; /* counter of objects */
extern int pdf_xform_count; /* counter of forms */
extern int pdf_ximage_count; /* counter of images */
extern int pdf_cur_form; /* the form being output */
extern int pdf_first_outline, pdf_last_outline,   pdf_parent_outline;
extern scaled pdf_xform_width,pdf_xform_height,pdf_xform_depth; /* dimension of the current form */
extern pointer pdf_info_toks; /* additional keys of Info dictionary */
extern pointer pdf_catalog_toks; /* additional keys of Catalog dictionary */
extern int pdf_catalog_openaction;
extern pointer pdf_names_toks; /* additional keys of Names dictionary */
extern int pdf_dest_names_ptr; /* first unused position in |dest_names| */
extern integer dest_names_size; /* maximum number of names in name tree of PDF output file */
extern dest_name_entry *dest_names;
extern integer image_orig_x, image_orig_y; /* origin of cropped pdf images */
extern pointer link_level_stack; /* stack to save |pdf_link_level| */
extern pointer pdf_trailer_toks; /* additional keys of Trailer dictionary */

EXTERN void pdfproc_initialize (void);

EXTERN void write_action (pointer p);

/* module 1579 */
#define link_level( arg )  info ( arg )
#define link_ptr( arg )  info ( arg  + 1 )


EXTERN integer open_subentries (pointer p);

EXTERN void set_rect_dimens (pointer p, pointer parent_box, scaled x, scaled y, scaled w, scaled h, scaled d, scaled margin);
EXTERN void do_annot (pointer p, pointer parent_box, scaled x, scaled y);
EXTERN void do_link (pointer p, pointer parent_box, scaled x, scaled y);
EXTERN void end_link (void);
EXTERN void append_link (pointer parent_box, scaled x, scaled y);
EXTERN void do_dest (pointer p, pointer parent_box, scaled x, scaled y);
EXTERN void out_form (pointer p);
EXTERN void out_image (pointer p);



/* module 1583 */
extern boolean is_shipping_page; /* set to |shipping_page| when |pdf_ship_out|  starts */
