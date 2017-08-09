
#include "types.h"
#include "c-compat.h"
#include "globals.h"

/* module 158 */

/* Anyone who has been reading the last few sections of the program will
 * be able to guess what comes next.
 */
pointer
new_penalty (int m) {
  pointer p;			/* the new node */
  p = get_node (small_node_size);
  type (p) = penalty_node;
  subtype (p) = 0;		/* the |subtype| is not used */
  penalty (p) = m;
  return p;
};


/* module 1248 */
void 
append_penalty (void) {
  scan_int();
  tail_append (new_penalty (cur_val));
  if (mode == vmode)
	build_page();
};

