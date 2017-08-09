
#include "types.h"
#include "c-compat.h"
#include "globals.h"

#include "mainio.h"

/* module 19 */

/* module 20 */

/* The \TeX\ processor converts between ASCII code and
 * the user's external character set by means of arrays |xord| and |xchr|
 * that are analogous to \PASCAL's |ord| and |chr| functions.
 */

ASCII_code xord[256]; /* specifies conversion of input characters */
text_char xchr[256]; /* specifies conversion of output characters */

/* module 21 */

/* Since we are assuming that our \PASCAL\ system is able to read and
 * write the visible characters of standard ASCII (although not
 * necessarily using the ASCII codes to represent them), the following
 * assignment statements initialize the standard part of the |xchr| array
 * properly, without needing any system-dependent changes. On the other
 * hand, it is possible to implement \TeX\ with less complete character
 * sets, and in such cases it will be necessary to change something here.
 */

/* module 23 */

/* The ASCII code is ``standard'' only to a certain extent, since many
 * computer installations have found it advantageous to have ready access
 * to more than 94 printing characters. Appendix~C of {\sl The \TeX book\/}
 * gives a complete specification of the intended correspondence between
 * characters and \TeX's internal representation.
 * 
 * If \TeX\ is being used
 * on a garden-variety \PASCAL\ for which only standard ASCII
 * codes will appear in the input and output files, it doesn't really matter
 * what codes are specified in |xchr[0.. oct(37)]|, but the safest policy is to
 * blank everything out by using the code shown below.
 * 
 * However, other settings of |xchr| will make \TeX\ more friendly on
 * computers that have an extended character set, so that users can type things
 * like `\.^^Z' instead of `\.{\\ne}'. People with extended character sets can
 * assign codes arbitrarily, giving an |xchr| equivalent to whatever
 * characters the users of \TeX\ are allowed to have in their input files.
 * It is best to make the codes correspond to the intended interpretations as
 * shown in Appendix~C whenever possible; but this is not necessary. For
 * example, in countries with an alphabet of more than 26 letters, it is
 * usually best to map the additional letters into codes less than~ oct(40).
 * To get the most ``permissive'' character set, change |' '| on the
 * right of these assignment statements to |chr(i)|.
 */

/* module 49 */
/* The first 128 strings will contain 95 standard ASCII characters, and the
 * other 33 characters will be printed in three-symbol form like `\.{\^\^A}'
 * unless a system-dependent change is made here. Installations that have
 * an extended character set, where for example |xchr[ oct(32)]=@t\.{\'^^Z\'}@>|,
 * would like string oct(32) to be printed as the single character oct(32)
 * instead of the
 * three characters oct(136), oct(136), oct(132) (\.{\^\^Z}). On the other hand,
 * even people with an extended character set will want to represent string
 * oct(15) by \.{\^\^M}, since oct(15) is |carriage_return|; the idea is to
 * produce visible strings instead of tabs or line-feeds or carriage-returns
 * or bell-rings or characters that are treated anomalously in text files.
 * 
 * Unprintable characters of codes 128--255 are, similarly, rendered
 * \.{\^\^80}--\.{\^\^ff}.
 * 
 * The boolean expression defined here should be |true| unless \TeX\
 * internal code number~|k| corresponds to a non-troublesome visible
 * symbol in the local character set. An appropriate formula for the
 * extended character set recommended in {\sl The \TeX book\/} would, for
 * example, be `|k in [0, oct(10).. oct(12), oct(14), oct(15), oct(33), oct(177).. oct(377)]|'.
 * If character |k| cannot be printed, and |k< oct(200)|, then character |k+ oct(100)| or
 * |k- oct(100)| must be printable; moreover, ASCII codes |[ oct(41).. oct(46),
 * 
 * oct(60).. oct(71), oct(141).. oct(146), oct(160).. oct(171)]| must be printable.
 * Thus, at least 80 printable characters are needed.
 */

boolean is_printable[256]; /* use \.{\^\^} notation? */



/* Set up the xchr, xord, and is_printable arrays for TeX, allowing a
   translation table specified at runtime via an external file.  By
   default, no characters are translated (all 256 simply map to
   themselves) and only printable ASCII is_printable.  We must
   initialize xord at the same time as xchr, and not use the
   ``system-independent'' code in tex.web, because we want
   settings in the tcx file to override the defaults, and not simply
   assign everything in numeric order.  */

void
xordchr_initialize (void) {
  unsigned c;
  for (c = 0; c <= 255; c++) {
    xchr[c] = xord[c] = c;
    is_printable[c] = ((32 <= c && c <= 126) || (128 <= c && c <= 255));
  }
}


