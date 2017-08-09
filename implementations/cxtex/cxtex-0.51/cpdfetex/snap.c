
#include "types.h"
#include "c-compat.h"
#include "globals.h"

/* module 1529 */

/* Extensions for getting possions and snapping.
 */

#define snap_node_size 2

integer pdf_last_x_pos;
integer pdf_last_y_pos;
integer pdf_snap_x_pos;
integer pdf_snap_y_pos;
pointer pdf_line_snap_x;
pointer pdf_line_snap_y;

/* module 1530 */
void
snap_initialize (void) {
  pdf_line_snap_x = null;
  pdf_line_snap_y = null;
}


/* module 1534 */
#define prepend_snap_node( arg )                               \
  if ( (arg != null)  && (width (snap_glue_ptr (arg)) != 0)) { \
     r = copy_node_list (arg);                                 \
     link (r) = list_ptr (just_box);                           \
     list_ptr (just_box) =  r ;                                \
  }



/* module 1534 */
pointer 
new_snap_node (small_number s) {
  pointer p;
  scan_glue (glue_val);
  if (width (cur_val) < 0)
	pdf_error_string("ext1","negative snap glue");
  p = get_node (snap_node_size);
  type (p) = whatsit_node;
  subtype (p) = s;
  link (p) = null;
  snap_glue_ptr (p) = cur_val;
  return p;
};

void 
prepend_line_snap_nodes (void) {
  pointer r;
  prepend_snap_node (pdf_line_snap_x);
  prepend_snap_node (pdf_line_snap_y);
};


void 
do_snap (pointer p) {
  scaled gap_amount, stretch_amount, shrink_amount;
  scaled cur_point, last_point, NEXTPoint;
  gap_amount = width (snap_glue_ptr (p));
  if (stretch_order (snap_glue_ptr (p)) > normal) {
	stretch_amount = max_dimen;
  } else {
	stretch_amount = stretch (snap_glue_ptr (p));
  }
  if (shrink_order (snap_glue_ptr (p)) > normal) {
	shrink_amount = max_dimen;
  } else {
	shrink_amount = shrink (snap_glue_ptr (p));
  }
  if (subtype (p) == pdf_snap_x_node) {
	cur_point = cur_h;
	last_point = pdf_snap_x_pos + gap_amount * ((cur_point - pdf_snap_x_pos) / gap_amount);
  } else {
	cur_point = cur_v;
	last_point = pdf_snap_y_pos + gap_amount * ((cur_point - pdf_snap_y_pos) / gap_amount);
  };
  NEXTPoint = last_point + gap_amount;
  if ((cur_point - last_point > shrink_amount) && (NEXTPoint - cur_point > stretch_amount))
	return;
  if (cur_point - last_point > shrink_amount) {
	cur_point = NEXTPoint;
  } else if (NEXTPoint - cur_point > stretch_amount) {
	cur_point = last_point;
  } else if (cur_point - last_point <= NEXTPoint - cur_point) {
	cur_point = last_point;
  } else {
	cur_point = NEXTPoint;
  };
  if (subtype (p) == pdf_snap_x_node) {
	cur_h = cur_point;
  } else {
	cur_v = cur_point;
  }
};


