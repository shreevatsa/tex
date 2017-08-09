

#include "types.h"
#include "c-compat.h"

#include "globals.h"


/* module 1819 */
/* 
 * The boolean variable |mltex_p| is set by web2c according to the given
 * command line option (or an entry in the configuration file) before any
 * \TeX{} function is called.
 */
boolean mltex_p;

/* module 1820 */

/* The boolean variable |mltex_enabled_p| is used to enable ML\TeX's
 * character substitution. It is initialised to |false|. When loading
 * a \.{FMT} it is set to the value of the boolean |mltex_p| saved in
 * the \.{FMT} file. Additionally it is set to the value of |mltex_p|
 * in Ini\TeX.
 */
boolean mltex_enabled_p; /* enable character substitution */

/* module 1825 */

/* The global variables for the code to substitute a virtual character
 * can be declared as local. Nonetheless we declare them as global to
 * avoid stack overflows because |hlist_out| can be called recursivly.
 */

integer accent_c, base_c, replace_c;
four_quarters ia_c, ib_c; /* accent and base character information */
real base_slant, accent_slant; /* amount of slant */
scaled base_x_height; /* accent is designed for characters of this height */
scaled base_width, base_height; /* height and width for base character */
scaled accent_width, accent_height; /* height and width for accent */
scaled delta; /* amount of right shift */


/* module 1822 */

/* The function |effective_char| computes the effective character with
 * respect to font information. The effective character is either the
 * base character part of a character substitution definition, if the
 * character does not exist in the font or the character itself.
 * 
 * Inside |effective_char| we can not use |char_info| because the macro
 * |char_info| uses |effective_char| calling this function a second time
 * with the same arguments.
 * 
 * If neither the character |c| exists in font |f| nor a character
 * substitution for |c| was defined, you can not use the function value
 * as a character offset in |char_info| because it will access an
 * undefined or invalid |font_info| entry! Therefore inside |char_info|
 * and in other places, |effective_char|'s boolean parameter |err_p| is
 * set to |true| to issue a warning and return the incorrect
 * replacement, but always existing character |font_bc[f]|.
 */
integer 
effective_char (boolean err_p, internal_font_number f, quarterword c) {
  int base_c; /* or |eightbits|: replacement base character */ 
  int result; /* or |quarterword| */ 
  result = c; /* return |c| unless it does not exist in the font */
  if (!mltex_enabled_p)
    goto FOUND;
  if (font_ec[f] >= qo (c))
    if (font_bc[f] <= qo (c))
      if (char_exists (orig_char_info (f, c)))	/* N.B.: not |char_info|(f,c) */
	goto FOUND;
  if (qo (c) >= char_sub_def_min)
    if (qo (c) <= char_sub_def_max)
      if (char_list_exists (qo (c))) {
		base_c = char_list_char (qo (c));
		result = qi (base_c);	/* return |base_c| */ 
		if (!err_p)
		  goto FOUND;
		if (font_ec[f] >= base_c)
		  if (font_bc[f] <= base_c)
			if (char_exists (orig_char_info (f, qi (base_c))))
			  goto FOUND;
	  };
  if (err_p) {			/* print error and return existing character? */
	begin_diagnostic();
	print_nl_string("Missing character: There is no ");
	zprint_string("substitution for ");
	print_ASCII (qo (c));
	zprint_string(" in font ");
	slow_print (font_name[f]);
	print_char ('!');
	end_diagnostic (false);
	result = qi (font_bc[f]);
	/* N.B.: not non-existing character |c|! */ 
  };
 FOUND:
  return result;
};

/* module 1823 */

/* The function |effective_char_info| is equivalent to |char_info|,
 * except it will return |null_character| if neither the character |c|
 * exists in font |f| nor is there a substitution definition for |c|.
 * (For these cases |char_info| using |effective_char| will access an
 * undefined or invalid |font_info| entry. See the documentation of
 * |effective_char| for more information.)
 */

four_quarters
effective_char_info (internal_font_number f, quarterword c) {
  four_quarters ci; /* character information bytes for |c| */ 
  int base_c; /* or |eightbits|: replacement base character */ 
  if (!mltex_enabled_p) {
	return orig_char_info (f, c);
  };
  if (font_ec[f] >= qo (c))
    if (font_bc[f] <= qo (c)) {
	  ci = orig_char_info (f, c); /* N.B.: not |char_info|(f,c) */ 
	  if (char_exists (ci)) {
	    return ci;
	  };
	};
  if (qo (c) >= char_sub_def_min)
    if (qo (c) <= char_sub_def_max)
      if (char_list_exists (qo (c))) {
		/* |effective_char_info:=char_info(f,qi(char_list_char(qo(c))));| */
		base_c = char_list_char (qo (c));
		if (font_ec[f] >= base_c)
		  if (font_bc[f] <= base_c) {
			ci = orig_char_info (f, qi (base_c)); /* N.B.: not |char_info|(f,c) */ 
			if (char_exists (ci)) {
			  return ci;
			};
	      };
	  };
  return null_character;
};

/* module 1821 */
void
mltex_initialize (void) {
  mltex_enabled_p = false;
}
