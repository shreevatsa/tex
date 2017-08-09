
#include "types.h"
#include "c-compat.h"
#include "globals.h"


/* module 144 */

/* The |new_ligature| function creates a ligature node having given
 * contents of the |font|, |character|, and |lig_ptr| fields. We also have
 * a |new_lig_item| function, which returns a two-word node having a given
 * |character| field. Such nodes are used for temporary processing as ligatures
 * are being created.
 */
pointer
new_ligature (internal_font_number f, quarterword c, pointer q) {
  pointer p;			/* the new node */
  p = get_node (small_node_size);
  type (p) = ligature_node;
  font (lig_char (p)) = f;
  character (lig_char (p)) = c;
  lig_ptr (p) = q;
  subtype (p) = 0;
  return p;
};

pointer
new_lig_item (quarterword c) {
  pointer p;			/* the new node */
  p = get_node (small_node_size);
  character (p) = c;
  lig_ptr (p) = null;
  return p;
};


