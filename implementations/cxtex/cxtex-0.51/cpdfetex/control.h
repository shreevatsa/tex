
/* module 1177 */
EXTERN font_index main_k; /* index into |font_info| */
EXTERN pointer main_p; /* temporary register for list manipulation */
EXTERN halfword bchar; /* right boundary character of current font, or |non_char| */
EXTERN halfword false_bchar; /* nonexistent character matching |bchar|, or |non_char| */

EXTERN void main_control (void);

EXTERN void control_initialize (void);

EXTERN void do_assignments (void);


