
#include "types.h"
#include "c-compat.h"
#include "globals.h"

/* module 139 */

/* A new rule node is delivered by the |new_rule| function. It
 * makes all the dimensions ``running,'' so you have to change the
 * ones that are not allowed to run.
 */
pointer
new_rule (void) {
  pointer p;			/* the new node */
  p = get_node (rule_node_size);
  type (p) = rule_node;
  subtype (p) = 0;		/* the |subtype| is not used */
  width (p) = null_flag;
  depth (p) = null_flag;
  height (p) = null_flag;
  return p;
};


void
print_rule_dimen (scaled d) { /* prints dimension in rule node */
  if (is_running (d)) {
	print_char ('*');
  } else {
	print_scaled (d);
  }
}

