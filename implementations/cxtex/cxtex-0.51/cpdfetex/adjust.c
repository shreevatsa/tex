
#include "types.h"
#include "c-compat.h"
#include "globals.h"

/* module 787 */

/* If the global variable |adjust_tail| is non-null, the |hpack| routine
 * also removes all occurrences of |ins_node|, |mark_node|, and |adjust_node|
 * items and appends the resulting material onto the list that ends at
 * location |adjust_tail|.
 */
pointer adjust_tail; /* tail of adjustment list */

/* module 797 */

/* Although node |q| is not necessarily the immediate predecessor of node |p|,
 * it always points to some node in the list preceding |p|. Thus, we can delete
 * nodes by moving |q| when necessary. The algorithm takes linear time, and the
 * extra computation does not intrude on the inner loop unless it is necessary
 * to make a deletion.
 */
pointer pre_adjust_tail;

void
adjust_initialize (void) {
  /* module 788 */
  adjust_tail = null;
  /* module 798 */
  pre_adjust_tail = null;
}

/* a new routine, to remove some complexity from hpack */

pointer
hpack_adjust_node (pointer p) {
  if (adjust_pre (p) != 0) {
	update_adjust_list (pre_adjust_tail);
  } else {
	update_adjust_list (adjust_tail);
  }
  p = link (p);
  return p;
}

/* another new routine, to create a new adjust node. Most 
 * node types have a dedicated new_XX routine in TeX, but
 * some didn't. This is one of them (called from control)
 */

pointer
new_adjust_node (pointer p, integer i) {
  pointer q;
  q = get_node (small_node_size);
  type (q) = adjust_node;
  adjust_pre (q) = i; /* the |subtype| is used for |adjust_pre| */
  adjust_ptr (q) = list_ptr (p);
  return q;
}
