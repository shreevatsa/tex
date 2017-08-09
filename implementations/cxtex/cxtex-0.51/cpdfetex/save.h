
/* module 268 */
#define save_type( arg )  save_stack [ arg ]. hh . b0
#define save_level( arg )  save_stack [ arg ]. hh . b1
#define save_index( arg )  save_stack [ arg ]. hh . rh
#define restore_old_value 0
#define restore_zero 1
#define insert_token 2
#define level_boundary 3
#define restore_sa 4

/* module 269 */
#define bottom_level 0
#define simple_group 1
#define hbox_group 2
#define adjusted_hbox_group 3
#define vbox_group 4
#define vtop_group 5
#define align_group 6
#define no_align_group 7
#define output_group 8
#define math_group 9
#define disc_group 10
#define insert_group 11
#define vcenter_group 12
#define math_choice_group 13
#define semi_simple_group 14
#define math_shift_group 15
#define math_left_group 16
#define max_group_code 16

/* module 271 */
EXTERN memory_word *save_stack;
EXTERN integer save_ptr; /* first unused entry on |save_stack| */ 
EXTERN integer max_save_stack; /* maximum usage of save stack */ 
EXTERN quarterword cur_level; /* current nesting level for groups */ 
EXTERN group_code cur_group; /* current group type */ 
EXTERN unsigned int cur_boundary; /* where the current level begins */

/* module 273 */

/* The following macro is used to test if there is room for up to seven more
 * entries on |save_stack|. By making a conservative test like this, we can
 * get by with testing for overflow in only a few places.
 */
#define check_full_save_stack                                                   \
  if (save_ptr > max_save_stack)  {                                             \
    max_save_stack = save_ptr;                                                  \
    if (max_save_stack > save_size - 7) {								        \
	  save_stack = xrealloc_array(save_stack,sizeof(memory_word)*save_size*2);  \
	  if(save_stack==NULL) {                                                    \
        overflow ( "save size" , save_size );                                   \
      } else {															        \
        save_size = 2*save_size;                                                \
	  }                                                                         \
    }                                                                           \
  }

/* module 274 */
#define saved( arg )  save_stack [ save_ptr  +  arg ]. cint

EXTERN void new_save_level (group_code c);

EXTERN void eq_define (pointer p, quarterword t, halfword e);
EXTERN void eq_word_define (pointer p, int w);
EXTERN void geq_define (pointer p, quarterword t, halfword e);
EXTERN void geq_word_define (pointer p, int w);

EXTERN void save_for_after (halfword t);

EXTERN void save_initialize (void);
EXTERN void unsave(void);
