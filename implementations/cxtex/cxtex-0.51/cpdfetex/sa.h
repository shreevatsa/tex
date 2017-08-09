
/* module 1760 */
#define box_val 4
#define mark_val 6
#define dimen_val_limit 32
#define mu_val_limit 64
#define box_val_limit 80
#define tok_val_limit 96
#define index_node_size 9
#define sa_index  type
#define sa_used  subtype

/* module 1761 */
#define sa_mark  sa_root [mark_val]

/* module 1764 */
#define get_sa_ptr   if (odd (i)) {  cur_ptr = link (q  + (i / 2) + 1); } else { cur_ptr = info (q  + (i / 2) + 1); }
#define put_sa_ptr(arg)  if (odd (i)) { link (q + (i / 2) + 1) = arg; } else { info (q + (i / 2) + 1)  =  arg; }
#define add_sa_ptr { put_sa_ptr ( cur_ptr ); incr ( sa_used ( q ));}
#define delete_sa_ptr { put_sa_ptr ( null ); decr ( sa_used ( q ));}

/* module 1765 */
#define sa_lev  sa_used
#define pointer_node_size 2
#define sa_type( arg ) ( sa_index ( arg )  / 16 )
#define sa_ref( arg )  info ( arg  + 1 )
#define sa_ptr( arg )  link ( arg  + 1 )
#define word_node_size 3
#define sa_num  sa_ptr
#define sa_int( arg )  mem [ arg  + 2 ]. cint
#define sa_dim( arg )  mem [ arg  + 2 ]. sc
#define mark_class_node_size 4
#define fetch_box( arg )   if (  cur_val  < 256  )   { arg   =  box ( cur_val ); }\
  else  { find_sa_element ( box_val , cur_val , false );\
  if (  cur_ptr  ==  null  )   { arg   =  null; }   else   arg   =  sa_ptr ( cur_ptr );}

/* module 1766 */
#define add_sa_ref( arg )  incr ( sa_ref ( arg ))
#define change_box( arg )   if (  cur_val  < 256  )   {box ( cur_val )  =  arg;}   else   set_sa_box ( arg )
#define set_sa_box( arg ) { find_sa_element ( box_val , cur_val , false );  if (  cur_ptr   !=  null  ) \
                               { sa_ptr ( cur_ptr )  =  arg ; add_sa_ref ( cur_ptr ); delete_sa_ref ( cur_ptr );};}

/* module 1770 */
#define vsplit_init 0
#define fire_up_init 1
#define fire_up_done 2
#define destroy_marks 3
#define sa_top_mark( arg )  info ( arg  + 1 )
#define sa_first_mark( arg )  link ( arg  + 1 )
#define sa_bot_mark( arg )  info ( arg  + 2 )
#define sa_split_first_mark( arg )  link ( arg  + 2 )
#define sa_split_bot_mark( arg )  info ( arg  + 3 )

/* module 1782 */
#define sa_loc  sa_ref

/* module 1761 */
EXTERN pointer sa_root[(mark_val+1)]; /* roots of sparse arrays */
EXTERN pointer cur_ptr; /* value returned by |new_index| and |find_sa_element| */

EXTERN void sa_initialize(void);
EXTERN void sa_initialize_init(void);

EXTERN void delete_sa_ref (pointer q);
EXTERN void show_sa (pointer p, char *s);
EXTERN void sa_save (pointer p);
EXTERN void sa_destroy (pointer p);
EXTERN void sa_def (pointer p, halfword e);
EXTERN void sa_w_def (pointer p, int w);
EXTERN void gsa_def (pointer p, halfword e);
EXTERN void gsa_w_def (pointer p, int w);
EXTERN void sa_restore (pointer, pointer);
EXTERN void find_sa_element (small_number t, halfword n, boolean w);
EXTERN void print_sa_num (pointer q);

#define sa_define(a,b)             if (e) {\
                                     if (global) { gsa_def (a,b); }\
                                     else { sa_def (a,b) ; }};\
                                   if (!e) DEFINE

#define sa_word_define(a,b)        if (e) {\
                                     if (global) { gsa_w_def (a,b); }\
                                     else { sa_w_def (a,b); }\
                                   } else { WORD_DEFINE (a,b); }


