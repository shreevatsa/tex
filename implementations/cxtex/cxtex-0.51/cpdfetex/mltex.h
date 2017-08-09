



extern boolean mltex_p;

extern boolean mltex_enabled_p; /* enable character substitution */

extern integer accent_c, base_c, replace_c;
extern four_quarters ia_c, ib_c; /* accent and base character information */
extern real base_slant, accent_slant; /* amount of slant */
extern scaled base_x_height; /* accent is designed for characters of this height */
extern scaled base_width, base_height; /* height and width for base character */
extern scaled accent_width, accent_height; /* height and width for accent */
extern scaled delta; /* amount of right shift */


EXTERN integer  effective_char (boolean err_p, internal_font_number f, quarterword c);

EXTERN four_quarters effective_char_info (internal_font_number f, quarterword c);


/* module 1826 */

/* Get the character substitution information in |char_sub_code| for
 * the character |c|. The current code checks that the substition
 * exists and is valid and all substitution characters exist in the
 * font, so we can {\it not\/} substitute a character used in a
 * substitution. This simplifies the code because we have not to check
 * for cycles in all character substitution definitions.
 */
/* Get substitution information, check it, goto |found| if all is ok, otherwise goto |continue| */
#define get_substitution_info    if (qo (c) >= char_sub_def_min)\
			  if (qo (c) <= char_sub_def_max)\
			    if (char_list_exists (qo (c))) {\
				base_c = char_list_char (qo (c));\
				accent_c = char_list_accent (qo (c));\
				if ((font_ec[f] >= base_c))\
				  if ((font_bc[f] <= base_c))\
				    if ((font_ec[f] >= accent_c))\
				      if ((font_bc[f] <= accent_c)) {\
					  ia_c = char_info (f,qi (accent_c));\
					  ib_c = char_info (f,qi (base_c));\
					  if (char_exists (ib_c))\
					    if (char_exists (ia_c))\
					      goto FOUND;\
					};\
				begin_diagnostic();\
				print_nl_string  ("Missing character: Incomplete substitution ");\
				print_ASCII (qo (c));\
				zprint_string(" = ");\
				print_ASCII (accent_c);\
				zprint (' ');\
				print_ASCII (base_c);\
				zprint_string(" in font ");\
				slow_print (font_name[f]);\
				print_char ('!');\
				end_diagnostic (false);\
				goto CONTINUE; };\
			begin_diagnostic();\
			print_nl_string("Missing character: There is no ");\
			zprint_string("substitution for ");\
			print_ASCII (qo (c));\
			zprint_string(" in font ");\
			slow_print (font_name[f]);\
			print_char ('!');\
			end_diagnostic (false);\
			goto CONTINUE

/* module 1827 */

/* For |tracinglostchars>99| the substitution is shown in the log file. */
/* Print character substition tracing log */
#define print_substitution_log	if (tracing_lost_chars > 99) {\
			    begin_diagnostic();\
			    print_nl_string("Using character substitution: ");\
			    print_ASCII (qo (c));\
			    zprint_string(" = ");\
			    print_ASCII (accent_c);\
			    zprint (' ');\
			    print_ASCII (base_c);\
			    zprint_string(" in font ");\
			    slow_print (font_name[f]);\
			    print_char ('.');\
			    end_diagnostic (false); }

EXTERN void mltex_initialize (void);
