

EXTERN integer pdf_last_x_pos;
EXTERN integer pdf_last_y_pos;
EXTERN integer pdf_snap_x_pos;
EXTERN integer pdf_snap_y_pos;
EXTERN pointer pdf_line_snap_x;
EXTERN pointer pdf_line_snap_y;

EXTERN void snap_initialize (void);

EXTERN pointer new_snap_node (small_number);

EXTERN void prepend_line_snap_nodes (void);

EXTERN void do_snap (pointer);
