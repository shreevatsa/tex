/* module 211 */
#define vmode 1
#define hmode  (vmode  +  max_command  + 1)
#define mmode  (hmode  +  max_command  + 1)

/* module 212 */
#define ignore_depth  (-65536000)

/* module 213 */
#define mode             cur_list.mode_field
#define head             cur_list.head_field
#define tail             cur_list.tail_field
#define eTeX_aux         cur_list.eTeX_aux_field
#define LR_save          eTeX_aux
#define LR_box           eTeX_aux
#define delim_ptr        eTeX_aux
#define prev_graf        cur_list.pg_field
#define aux              cur_list.aux_field
#define prev_depth       aux.sc
#define space_factor     aux.hh.lhfield
#define clang            aux.hh.rh
#define incompleat_noad  aux.cint
#define mode_line        cur_list.ml_field


EXTERN list_state_record *nest;
EXTERN unsigned int nest_ptr;
EXTERN unsigned int max_nest_stack;
EXTERN list_state_record cur_list;


/* module 214 */

/* Here is a common way to make the current list grow:
 */
#define tail_append( arg ) { \
    link ( tail )  =  arg ;  \
    tail   =  link ( tail ); \
  }

EXTERN void print_mode (int m);
EXTERN void push_nest (void);
EXTERN void pop_nest (void);
EXTERN void show_activities (void);
EXTERN void nest_initialize(void);
EXTERN void alter_prev_graf(void);

EXTERN void show_save_groups (void);
