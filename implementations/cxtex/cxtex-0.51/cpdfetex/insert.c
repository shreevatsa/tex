
#include "types.h"
#include "c-compat.h"
#include "globals.h"


/* module 1244 */
void 
begin_insert_or_adjust (void) {
  if (cur_cmd == vadjust) {
	cur_val = 255;
  } else {
	scan_eight_bit_int();
	if (cur_val == 255) {
	  print_err ("You can't ");
	  print_esc_string ("insert");
	  print_int (255);
	  help1 ("I'm changing to \\insert0; box 255 is special.");
	  error();
	  cur_val = 0;
	};
  };
  saved (0) = cur_val;
  if ((cur_cmd == vadjust) && scan_keyword ("pre")) {
	saved (1) = 1;
  } else {
	saved (1) = 0;
  }
  save_ptr = save_ptr + 2;
  new_save_level (insert_group);
  scan_left_brace();
  normal_paragraph();
  push_nest();
  mode = -vmode;
  prev_depth = ignore_depth;
};

