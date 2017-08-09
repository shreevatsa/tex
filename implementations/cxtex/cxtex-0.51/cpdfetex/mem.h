
/* module 118 */
#define link( arg )  mem [arg].hh.rh
#define info( arg )  mem [arg].hh.lhfield

/* module 121 */

/* Conversely, a one-word node is recycled by calling |free_avail|.
 * This routine is part of \TeX's ``inner loop,'' so we want it to be fast.
 */
#define free_avail(arg) { link (arg) = avail; avail = arg;  decr (dyn_used); }

/* module 122 */

/* There's also a |fast_get_avail| routine, which saves the procedure-call
 * overhead at the expense of extra programming. This routine is used in
 * the places that would otherwise account for the most calls of |get_avail|.
 */

#define fast_get_avail( arg )  { \
   arg = avail;                  \
   if (arg == null) {            \
     arg = get_avail();          \
   } else {                      \
     avail = link (arg);         \
     link (arg) = null;          \
     incr ( dyn_used );          \
   };                            \
 }

/* module 124 */
#define empty_flag  max_halfword
#define is_empty(arg) (link (arg) == empty_flag)
#define node_size  info
#define llink(arg)  info (arg + 1)
#define rlink(arg)  link (arg + 1)

EXTERN pointer temp_ptr;
EXTERN memory_word *yzmem;
EXTERN memory_word *mem;
EXTERN pointer lo_mem_max;
EXTERN pointer hi_mem_min;
EXTERN integer var_used, dyn_used;
EXTERN pointer avail;
EXTERN pointer mem_end;
EXTERN pointer rover;

EXTERN pointer get_avail (void);
EXTERN void flush_list (pointer p);
EXTERN pointer get_node (int s);
EXTERN void free_node (pointer p, halfword s);
EXTERN void sort_avail (void);

/* block 11: */

/* module 162 */

/* 
 * Some areas of |mem| are dedicated to fixed usage, since static allocation is
 * more efficient than dynamic allocation when we can get away with it. For
 * example, locations |mem_bot| to |mem_bot+3| are always used to store the
 * specification for glue that is `\.{0pt plus 0pt minus 0pt}'. The
 * following macro definitions accomplish the static allocation by giving
 * symbolic names to the fixed positions. Static variable-size nodes appear
 * in locations |mem_bot| through |lo_mem_stat_max|, and static single-word nodes
 * appear in locations |hi_mem_stat_min| through |mem_top|, inclusive. It is
 * harmless to let |lig_trick| and |garbage| share the same location of |mem|.
 */
#define zero_glue  mem_bot
#define fil_glue  (zero_glue  +  glue_spec_size)
#define fill_glue  (fil_glue  +  glue_spec_size)
#define ss_glue  (fill_glue  +  glue_spec_size)
#define fil_neg_glue  (ss_glue  +  glue_spec_size)
#define lo_mem_stat_max  (fil_neg_glue  +  glue_spec_size  - 1)
#define page_ins_head  mem_top
#define contrib_head  (mem_top  - 1)
#define page_head  (mem_top  - 2)
#define temp_head  (mem_top  - 3)
#define hold_head  (mem_top  - 4)
#define adjust_head  (mem_top  - 5)
#define active  (mem_top  - 7)
#define align_head  (mem_top  - 8)
#define end_span  (mem_top  - 9)
#define omit_template  (mem_top  - 10)
#define null_list  (mem_top  - 11)
#define lig_trick  (mem_top  - 12)
#define garbage  (mem_top  - 12)
#define backup_head  (mem_top  - 13)
#define pre_adjust_head  (mem_top  - 14)
#define hi_mem_stat_min  (mem_top  - 14)
#define hi_mem_stat_usage 15

EXTERN boolean panicking;

EXTERN void mem_initialize(void);

EXTERN void check_mem (boolean print_locs);
EXTERN void search_mem (pointer p);

EXTERN void mem_initialize_init(void);
