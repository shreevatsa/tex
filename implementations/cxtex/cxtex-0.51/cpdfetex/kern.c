

#include "types.h"
#include "c-compat.h"


#include "globals.h"

/* module 156 */

/* The |new_kern| function creates a kern node having a given width.
 */
pointer
new_kern (scaled w) {
  pointer p;			/* the new node */
  p = get_node (small_node_size);
  type (p) = kern_node;
  subtype (p) = normal;
  width (p) = w;
  return p;
};


/* module 1206 */

void append_kern (void) {
  quarterword s; /* |subtype| of the kern node */ 
  s = cur_chr;
  scan_dimen (s == mu_glue, false, false);
  tail_append (new_kern (cur_val));
  subtype (tail) = s;
};


