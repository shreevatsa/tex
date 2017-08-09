/* module 143 */

/* A |ligature_node|, which occurs only in horizontal lists, specifies
 * a character that was fabricated from the interaction of two or more
 * actual characters. The second word of the node, which is called the
 * |lig_char| word, contains |font| and |character| fields just as in a
 * |char_node|. The characters that generated the ligature have not been
 * forgotten, since they are needed for diagnostic messages and for
 * hyphenation; the |lig_ptr| field points to a linked list of character
 * nodes for all original characters that have been deleted. (This list
 * might be empty if the characters that generated the ligature were
 * retained in other nodes.)
 * 
 * The |subtype| field is 0, plus 2 and/or 1 if the original source of the
 * ligature included implicit left and/or right boundaries.
 */
#define ligature_node 6
#define lig_char( arg )  (arg  + 1)
#define lig_ptr( arg )  link ( lig_char ( arg ))

EXTERN pointer new_ligature (internal_font_number f, quarterword c, pointer q);
EXTERN pointer new_lig_item (quarterword c);

/* module 1054 */
#define wrap_lig( arg )                           \
  if (ligature_present)  {                        \
    p = new_ligature (hf, cur_l, link (cur_q));   \
    if (lft_hit) {                                \
      subtype (p) = 2;                            \
      lft_hit = false;                            \
    };                                            \
    if (arg) {                                    \
      if (lig_stack == null)  {                   \
        incr (subtype (p));                       \
        rt_hit = false;                           \
      };                                          \
    }                                             \
    link (cur_q) = p;                             \
    t = p ;                                       \
    ligature_present = false;                     \
    }

#define pop_lig_stack {                          \
   if (lig_ptr (lig_stack) >  null)  {           \
     link (t)  =  lig_ptr ( lig_stack );         \
     t = link (t);                               \
     incr (j);                                   \
   };                                            \
   p = lig_stack ;                               \
   lig_stack = link (p);                         \
   free_node (p, small_node_size );              \
   if (lig_stack == null)   { set_cur_r; }       \
   else { cur_r   =  character ( lig_stack );}   \
 }
