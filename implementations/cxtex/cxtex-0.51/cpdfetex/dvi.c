
#include "types.h"
#include "c-compat.h"
#include "globals.h"
#include "mainio.h"
#include "dviops.h"

/* module 583 */

/* 
 * The most important output produced by a run of \TeX\ is the ``device
 * independent'' (\.{DVI}) file that specifies where characters and rules
 * are to appear on printed pages. The form of these files was designed by
 * David R. Fuchs in 1979. Almost any reasonable typesetting device can be
 * 
 * driven by a program that takes \.{DVI} files as input, and dozens of such
 * \.{DVI}-to-whatever programs have been written. Thus, it is possible to
 * zprint the output of \TeX\ on many different kinds of equipment, using \TeX\
 * as a device-independent ``front end.''
 * 
 * A \.{DVI} file is a stream of 8-bit bytes, which may be regarded as a
 * series of commands in a machine-like language. The first byte of each command
 * is the operation code, and this code is followed by zero or more bytes
 * that provide parameters to the command. The parameters themselves may consist
 * of several consecutive bytes; for example, the `|set_rule|' command has two
 * parameters, each of which is four bytes long. Parameters are usually
 * regarded as nonnegative integers; but four-byte-long parameters,
 * and shorter parameters that denote distances, can be
 * either positive or negative. Such parameters are given in two's complement
 * notation. For example, a two-byte-long distance parameter has a value between
 * $-2^{15}$ and $2^{15}-1$. As in \.{TFM} files, numbers that occupy
 * more than one byte position appear in BigEndian order.
 * 
 * A \.{DVI} file consists of a ``preamble,'' followed by a sequence of one
 * or more ``pages,'' followed by a ``postamble.'' The preamble is simply a
 * |pre| command, with its parameters that define the dimensions used in the
 * file; this must come first. Each ``page'' consists of a |bop| command,
 * followed by any number of other commands that tell where characters are to
 * be placed on a physical page, followed by an |eop| command. The pages
 * appear in the order that \TeX\ generated them. If we ignore |nop| commands
 * and \\{fnt\_def} commands (which are allowed between any two commands in
 * the file), each |eop| command is immediately followed by a |bop| command,
 * or by a |post| command; in the latter case, there are no more pages in the
 * file, and the remaining bytes form the postamble. Further details about
 * the postamble will be explained later.
 * 
 * Some parameters in \.{DVI} commands are ``pointers.'' These are four-byte
 * quantities that give the location number of some other byte in the file;
 * the first byte is number~0, then comes number~1, and so on. For example,
 * one of the parameters of a |bop| command points to the previous |bop|;
 * this makes it feasible to read the pages in backwards order, in case the
 * results are being directed to a device that stacks its output face up.
 * Suppose the preamble of a \.{DVI} file occupies bytes 0 to 99. Now if the
 * first page occupies bytes 100 to 999, say, and if the second
 * page occupies bytes 1000 to 1999, then the |bop| that starts in byte 1000
 * points to 100 and the |bop| that starts in byte 2000 points to 1000. (The
 * very first |bop|, i.e., the one starting in byte 100, has a pointer of~$-1$.)
 */

/* module 584 */

/* The \.{DVI} format is intended to be both compact and easily interpreted
 * by a machine. Compactness is achieved by making most of the information
 * implicit instead of explicit. When a \.{DVI}-reading program reads the
 * commands for a page, it keeps track of several quantities: (a)~The current
 * font |f| is an integer; this value is changed only
 * by \\{fnt} and \\{fnt\_num} commands. (b)~The current position on the page
 * is given by two numbers called the horizontal and vertical coordinates,
 * |h| and |v|. Both coordinates are zero at the upper left corner of the page;
 * moving to the right corresponds to increasing the horizontal coordinate, and
 * moving down corresponds to increasing the vertical coordinate. Thus, the
 * coordinates are essentially Cartesian, except that vertical directions are
 * flipped; the Cartesian version of |(h,v)| would be |(h,-v)|. (c)~The
 * current spacing amounts are given by four numbers |w|, |x|, |y|, and |z|,
 * where |w| and~|x| are used for horizontal spacing and where |y| and~|z|
 * are used for vertical spacing. (d)~There is a stack containing
 * |(h,v,w,x,y,z)| values; the \.{DVI} commands |push| and |pop| are used to
 * change the current level of operation. Note that the current font~|f| is
 * not pushed and popped; the stack contains only information about
 * positioning.
 * 
 * The values of |h|, |v|, |w|, |x|, |y|, and |z| are signed integers having up
 * to 32 bits, including the sign. Since they represent physical distances,
 * there is a small unit of measurement such that increasing |h| by~1 means
 * moving a certain tiny distance to the right. The actual unit of
 * measurement is variable, as explained below; \TeX\ sets things up so that
 * its \.{DVI} output is in sp units, i.e., scaled points, in agreement with
 * all the |scaled| dimensions in \TeX's data structures.
 */

/* module 585 */

/* Here is a list of all the commands that may appear in a \.{DVI} file. Each
 * command is specified by its symbolic name (e.g., |bop|), its opcode byte
 * (e.g., 139), and its parameters (if any). The parameters are followed
 * by a bracketed number telling how many bytes they occupy; for example,
 * `|p[4]|' means that parameter |p| is four bytes long.
 * 
 * \yskip\hang|set_char_0| 0. Typeset character number~0 from font~|f|
 * such that the reference point of the character is at |(h,v)|. Then
 * increase |h| by the width of that character. Note that a character may
 * have zero or negative width, so one cannot be sure that |h| will advance
 * after this command; but |h| usually does increase.
 * 
 * \yskip\hang\\{set\_char\_1} through \\{set\_char\_127} (opcodes 1 to 127).
 * Do the operations of |set_char_0|; but use the character whose number
 * matches the opcode, instead of character~0.
 * 
 * \yskip\hang|set1| 128 |c[1]|. Same as |set_char_0|, except that character
 * number~|c| is typeset. \TeX82 uses this command for characters in the
 * range |128<=c<256|.
 * 
 * \yskip\hang| set2| 129 |c[2]|. Same as |set1|, except that |c|~is two
 * bytes long, so it is in the range |0<=c<65536|. \TeX82 never uses this
 * command, but it should come in handy for extensions of \TeX\ that deal
 * with oriental languages.
 * 
 * \yskip\hang| set3| 130 |c[3]|. Same as |set1|, except that |c|~is three
 * bytes long, so it can be as large as $2^{24}-1$. Not even the Chinese
 * language has this many characters, but this command might prove useful
 * in some yet unforeseen extension.
 * 
 * \yskip\hang| set4| 131 |c[4]|. Same as |set1|, except that |c|~is four
 * bytes long. Imagine that.
 * 
 * \yskip\hang|set_rule| 132 |a[4]| |b[4]|. Typeset a solid black rectangle
 * of height~|a| and width~|b|, with its bottom left corner at |(h,v)|. Then
 * set |h:=h+b|. If either |a<=0| or |b<=0|, nothing should be typeset. Note
 * that if |b<0|, the value of |h| will decrease even though nothing else happens.
 * See below for details about how to typeset rules so that consistency with
 * \MF\ is guaranteed.
 * 
 * \yskip\hang| put1| 133 |c[1]|. Typeset character number~|c| from font~|f|
 * such that the reference point of the character is at |(h,v)|. (The `put'
 * commands are exactly like the `set' commands, except that they simply put out a
 * character or a rule without moving the reference point afterwards.)
 * 
 * \yskip\hang| put2| 134 |c[2]|. Same as |set2|, except that |h| is not changed.
 * 
 * \yskip\hang| put3| 135 |c[3]|. Same as |set3|, except that |h| is not changed.
 * 
 * \yskip\hang| put4| 136 |c[4]|. Same as |set4|, except that |h| is not changed.
 * 
 * \yskip\hang|put_rule| 137 |a[4]| |b[4]|. Same as |set_rule|, except that
 * |h| is not changed.
 * 
 * \yskip\hang|nop| 138. No operation, do nothing. Any number of |nop|'s
 * may occur between \.{DVI} commands, but a |nop| cannot be inserted between
 * a command and its parameters or between two parameters.
 * 
 * \yskip\hang|bop| 139 $c_0[4]$ $c_1[4]$ $\ldots$ $c_9[4]$ $p[4]$. Beginning
 * of a page: Set |(h,v,w,x,y,z):=(0,0,0,0,0,0)| and set the stack empty. Set
 * the current font |f| to an undefined value. The ten $c_i$ parameters hold
 * the values of \.{\\count0} $\ldots$ \.{\\count9} in \TeX\ at the time
 * \.{\\shipout} was invoked for this page; they can be used to identify
 * pages, if a user wants to zprint only part of a \.{DVI} file. The parameter
 * |p| points to the previous |bop| in the file; the first
 * |bop| has $p=-1$.
 * 
 * \yskip\hang|eop| 140. End of page: Print what you have read since the
 * previous |bop|. At this point the stack should be empty. (The \.{DVI}-reading
 * programs that drive most output devices will have kept a buffer of the
 * material that appears on the page that has just ended. This material is
 * largely, but not entirely, in order by |v| coordinate and (for fixed |v|) by
 * |h|~coordinate; so it usually needs to be sorted into some order that is
 * appropriate for the device in question.)
 * 
 * \yskip\hang|push| 141. Push the current values of |(h,v,w,x,y,z)| onto the
 * top of the stack; do not change any of these values. Note that |f| is
 * not pushed.
 * 
 * \yskip\hang|pop| 142. Pop the top six values off of the stack and assign
 * them respectively to |(h,v,w,x,y,z)|. The number of pops should never
 * exceed the number of pushes, since it would be highly embarrassing if the
 * stack were empty at the time of a |pop| command.
 * 
 * \yskip\hang|right1| 143 |b[1]|. Set |h:=h+b|, i.e., move right |b| units.
 * The parameter is a signed number in two's complement notation, |-128<=b<128|;
 * if |b<0|, the reference point moves left.
 * 
 * \yskip\hang|right2| 144 |b[2]|. Same as |right1|, except that |b| is a
 * two-byte quantity in the range |-32768<=b<32768|.
 * 
 * \yskip\hang|right3| 145 |b[3]|. Same as |right1|, except that |b| is a
 * three-byte quantity in the range |@t$-2^{23}$@><=b<@t$2^{23}$@>|.
 * 
 * \yskip\hang|right4| 146 |b[4]|. Same as |right1|, except that |b| is a
 * four-byte quantity in the range |@t$-2^{31}$@><=b<@t$2^{31}$@>|.
 * 
 * \yskip\hang|w0| 147. Set |h:=h+w|; i.e., move right |w| units. With luck,
 * this parameterless command will usually suffice, because the same kind of motion
 * will occur several times in succession; the following commands explain how
 * |w| gets particular values.
 * 
 * \yskip\hang|w1| 148 |b[1]|. Set |w:=b| and |h:=h+b|. The value of |b| is a
 * signed quantity in two's complement notation, |-128<=b<128|. This command
 * changes the current |w|~spacing and moves right by |b|.
 * 
 * \yskip\hang| w2| 149 |b[2]|. Same as |w1|, but |b| is two bytes long,
 * |-32768<=b<32768|.
 * 
 * \yskip\hang| w3| 150 |b[3]|. Same as |w1|, but |b| is three bytes long,
 * |@t$-2^{23}$@><=b<@t$2^{23}$@>|.
 * 
 * \yskip\hang| w4| 151 |b[4]|. Same as |w1|, but |b| is four bytes long,
 * |@t$-2^{31}$@><=b<@t$2^{31}$@>|.
 * 
 * \yskip\hang|x0| 152. Set |h:=h+x|; i.e., move right |x| units. The `|x|'
 * commands are like the `|w|' commands except that they involve |x| instead
 * of |w|.
 * 
 * \yskip\hang|x1| 153 |b[1]|. Set |x:=b| and |h:=h+b|. The value of |b| is a
 * signed quantity in two's complement notation, |-128<=b<128|. This command
 * changes the current |x|~spacing and moves right by |b|.
 * 
 * \yskip\hang| x2| 154 |b[2]|. Same as |x1|, but |b| is two bytes long,
 * |-32768<=b<32768|.
 * 
 * \yskip\hang| x3| 155 |b[3]|. Same as |x1|, but |b| is three bytes long,
 * |@t$-2^{23}$@><=b<@t$2^{23}$@>|.
 * 
 * \yskip\hang| x4| 156 |b[4]|. Same as |x1|, but |b| is four bytes long,
 * |@t$-2^{31}$@><=b<@t$2^{31}$@>|.
 * 
 * \yskip\hang|down1| 157 |a[1]|. Set |v:=v+a|, i.e., move down |a| units.
 * The parameter is a signed number in two's complement notation, |-128<=a<128|;
 * if |a<0|, the reference point moves up.
 * 
 * \yskip\hang| down2| 158 |a[2]|. Same as |down1|, except that |a| is a
 * two-byte quantity in the range |-32768<=a<32768|.
 * 
 * \yskip\hang| down3| 159 |a[3]|. Same as |down1|, except that |a| is a
 * three-byte quantity in the range |@t$-2^{23}$@><=a<@t$2^{23}$@>|.
 * 
 * \yskip\hang| down4| 160 |a[4]|. Same as |down1|, except that |a| is a
 * four-byte quantity in the range |@t$-2^{31}$@><=a<@t$2^{31}$@>|.
 * 
 * \yskip\hang|y0| 161. Set |v:=v+y|; i.e., move down |y| units. With luck,
 * this parameterless command will usually suffice, because the same kind of motion
 * will occur several times in succession; the following commands explain how
 * |y| gets particular values.
 * 
 * \yskip\hang|y1| 162 |a[1]|. Set |y:=a| and |v:=v+a|. The value of |a| is a
 * signed quantity in two's complement notation, |-128<=a<128|. This command
 * changes the current |y|~spacing and moves down by |a|.
 * 
 * \yskip\hang| y2| 163 |a[2]|. Same as |y1|, but |a| is two bytes long,
 * |-32768<=a<32768|.
 * 
 * \yskip\hang| y3| 164 |a[3]|. Same as |y1|, but |a| is three bytes long,
 * |@t$-2^{23}$@><=a<@t$2^{23}$@>|.
 * 
 * \yskip\hang| y4| 165 |a[4]|. Same as |y1|, but |a| is four bytes long,
 * |@t$-2^{31}$@><=a<@t$2^{31}$@>|.
 * 
 * \yskip\hang|z0| 166. Set |v:=v+z|; i.e., move down |z| units. The `|z|' commands
 * are like the `|y|' commands except that they involve |z| instead of |y|.
 * 
 * \yskip\hang|z1| 167 |a[1]|. Set |z:=a| and |v:=v+a|. The value of |a| is a
 * signed quantity in two's complement notation, |-128<=a<128|. This command
 * changes the current |z|~spacing and moves down by |a|.
 * 
 * \yskip\hang| z2| 168 |a[2]|. Same as |z1|, but |a| is two bytes long,
 * |-32768<=a<32768|.
 * 
 * \yskip\hang| z3| 169 |a[3]|. Same as |z1|, but |a| is three bytes long,
 * |@t$-2^{23}$@><=a<@t$2^{23}$@>|.
 * 
 * \yskip\hang| z4| 170 |a[4]|. Same as |z1|, but |a| is four bytes long,
 * |@t$-2^{31}$@><=a<@t$2^{31}$@>|.
 * 
 * \yskip\hang|fnt_num_0| 171. Set |f:=0|. Font 0 must previously have been
 * defined by a \\{fnt\_def} instruction, as explained below.
 * 
 * \yskip\hang\\{fnt\_num\_1} through \\{fnt\_num\_63} (opcodes 172 to 234). Set
 * |f:=1|, \dots, \hbox{|f:=63|}, respectively.
 * 
 * \yskip\hang|fnt1| 235 |k[1]|. Set |f:=k|. \TeX82 uses this command for font
 * numbers in the range |64<=k<256|.
 * 
 * \yskip\hang| fnt2| 236 |k[2]|. Same as |fnt1|, except that |k|~is two
 * bytes long, so it is in the range |0<=k<65536|. \TeX82 never generates this
 * command, but large font numbers may prove useful for specifications of
 * color or texture, or they may be used for special fonts that have fixed
 * numbers in some external coding scheme.
 * 
 * \yskip\hang| fnt3| 237 |k[3]|. Same as |fnt1|, except that |k|~is three
 * bytes long, so it can be as large as $2^{24}-1$.
 * 
 * \yskip\hang| fnt4| 238 |k[4]|. Same as |fnt1|, except that |k|~is four
 * bytes long; this is for the really big font numbers (and for the negative ones).
 * 
 * \yskip\hang|xxx1| 239 |k[1]| |x[k]|. This command is undefined in
 * general; it functions as a $(k+2)$-byte |nop| unless special \.{DVI}-reading
 * programs are being used. \TeX82 generates |xxx1| when a short enough
 * \.{\\special} appears, setting |k| to the number of bytes being sent. It
 * is recommended that |x| be a string having the form of a keyword followed
 * by possible parameters relevant to that keyword.
 * 
 * \yskip\hang| xxx2| 240 |k[2]| |x[k]|. Like |xxx1|, but |0<=k<65536|.
 * 
 * \yskip\hang| xxx3| 241 |k[3]| |x[k]|. Like |xxx1|, but |0<=k<@t$2^{24}$@>|.
 * 
 * \yskip\hang|xxx4| 242 |k[4]| |x[k]|. Like |xxx1|, but |k| can be ridiculously
 * large. \TeX82 uses |xxx4| when sending a string of length 256 or more.
 * 
 * \yskip\hang|fnt_def1| 243 |k[1]| |c[4]| |s[4]| |d[4]| |a[1]| |l[1]| |n[a+l]|.
 * Define font |k|, where |0<=k<256|; font definitions will be explained shortly.
 * 
 * \yskip\hang| fnt_def2| 244 |k[2]| |c[4]| |s[4]| |d[4]| |a[1]| |l[1]| |n[a+l]|.
 * Define font |k|, where |0<=k<65536|.
 * 
 * \yskip\hang| fnt_def3| 245 |k[3]| |c[4]| |s[4]| |d[4]| |a[1]| |l[1]| |n[a+l]|.
 * Define font |k|, where |0<=k<@t$2^{24}$@>|.
 * 
 * \yskip\hang| fnt_def4| 246 |k[4]| |c[4]| |s[4]| |d[4]| |a[1]| |l[1]| |n[a+l]|.
 * Define font |k|, where |@t$-2^{31}$@><=k<@t$2^{31}$@>|.
 * 
 * \yskip\hang|pre| 247 |i[1]| |num[4]| |den[4]| |mag[4]| |k[1]| |x[k]|.
 * Beginning of the preamble; this must come at the very beginning of the
 * file. Parameters |i|, |num|, |den|, |mag|, |k|, and |x| are explained below.
 * 
 * \yskip\hang|post| 248. Beginning of the postamble, see below.
 * 
 * \yskip\hang|post_post| 249. Ending of the postamble, see below.
 * 
 * \yskip\noindent Commands 250--255 are undefined at the present time.
 */

/* module 587 */

/* The preamble contains basic information about the file as a whole. As
 * stated above, there are six parameters:
 * $$\hbox{| i[1]| | num[4]| | den[4]| | mag[4]| | k[1]| | x[k]|.}$$
 * The |i| byte identifies \.{DVI} format; currently this byte is always set
 * to~2. (The value |i=3| is currently used for an extended format that
 * allows a mixture of right-to-left and left-to-right typesetting.
 * Some day we will set |i=4|, when \.{DVI} format makes another
 * incompatible change---perhaps in the year 2048.)
 * 
 * The next two parameters, |num| and |den|, are positive integers that define
 * the units of measurement; they are the numerator and denominator of a
 * fraction by which all dimensions in the \.{DVI} file could be multiplied
 * in order to get lengths in units of $10^{-7}$ meters. Since $\rm 7227{pt} =
 * 254{cm}$, and since \TeX\ works with scaled points where there are $2^{16}$
 * sp in a point, \TeX\ sets
 * $|num|/|den|=(254\cdot10^5)/(7227\cdot2^{16})=25400000/473628672$.
 * 
 * The |mag| parameter is what \TeX\ calls \.{\\mag}, i.e., 1000 times the
 * desired magnification. The actual fraction by which dimensions are
 * multiplied is therefore $|mag|\cdot|num|/1000|den|$. Note that if a \TeX\
 * source document does not call for any `\.{true}' dimensions, and if you
 * change it only by specifying a different \.{\\mag} setting, the \.{DVI}
 * file that \TeX\ creates will be completely unchanged except for the value
 * of |mag| in the preamble and postamble. (Fancy \.{DVI}-reading programs allow
 * users to override the |mag|~setting when a \.{DVI} file is being printed.)
 * 
 * Finally, |k| and |x| allow the \.{DVI} writer to include a comment, which is not
 * interpreted further. The length of comment |x| is |k|, where |0<=k<256|.
 */
#define id_byte 2

/* module 588 */

/* Font definitions for a given font number |k| contain further parameters
 * $$\hbox{|c[4]| |s[4]| |d[4]| |a[1]| |l[1]| |n[a+l]|.}$$
 * The four-byte value |c| is the check sum that \TeX\ found in the \.{TFM}
 * file for this font; |c| should match the check sum of the font found by
 * programs that read this \.{DVI} file.
 * 
 * Parameter |s| contains a fixed-point scale factor that is applied to
 * the character widths in font |k|; font dimensions in \.{TFM} files and
 * other font files are relative to this quantity, which is called the
 * ``at size'' elsewhere in this documentation. The value of |s| is
 * always positive and less than $2^{27}$. It is given in the same units
 * as the other \.{DVI} dimensions, i.e., in sp when \TeX82 has made the
 * file. Parameter |d| is similar to |s|; it is the ``design size,'' and
 * (like~|s|) it is given in \.{DVI} units. Thus, font |k| is to be used
 * at $|mag|\cdot s/1000d$ times its normal size.
 * 
 * The remaining part of a font definition gives the external name of the font,
 * which is an ASCII string of length |a+l|. The number |a| is the length
 * of the ``area'' or directory, and |l| is the length of the font name itself;
 * the standard local system font area is supposed to be used when |a=0|.
 * The |n| field contains the area in its first |a| bytes.
 * 
 * Font definitions must appear before the first use of a particular font number.
 * Once font |k| is defined, it must not be defined again; however, we
 * shall see below that font definitions appear in the postamble as well as
 * in the pages, so in this sense each font number is defined exactly twice,
 * if at all. Like |nop| commands, font definitions can
 * appear before the first |bop|, or between an |eop| and a |bop|.
 */

/* module 589 */

/* Sometimes it is desirable to make horizontal or vertical rules line up
 * precisely with certain features in characters of a font. It is possible to
 * guarantee the correct matching between \.{DVI} output and the characters
 * generated by \MF\ by adhering to the following principles: (1)~The \MF\
 * characters should be positioned so that a bottom edge or left edge that is
 * supposed to line up with the bottom or left edge of a rule appears at the
 * reference point, i.e., in row~0 and column~0 of the \MF\ raster. This
 * ensures that the position of the rule will not be rounded differently when
 * the pixel size is not a perfect multiple of the units of measurement in
 * the \.{DVI} file. (2)~A typeset rule of height $a>0$ and width $b>0$
 * should be equivalent to a \MF-generated character having black pixels in
 * precisely those raster positions whose \MF\ coordinates satisfy
 * |0<=x<@t$\alpha$@>b| and |0<=y<@t$\alpha$@>a|, where $\alpha$ is the number
 * of pixels per \.{DVI} unit.
 */

/* module 590 */

/* The last page in a \.{DVI} file is followed by `|post|'; this command
 * introduces the postamble, which summarizes important facts that \TeX\ has
 * accumulated about the file, making it possible to zprint subsets of the data
 * with reasonable efficiency. The postamble has the form
 * $$\vbox{\halign{\hbox{#\hfil}\cr
 * 
 * |post| |p[4]| |num[4]| |den[4]| |mag[4]| |l[4]| |u[4]| |s[2]| |t[2]|\cr
 * 
 * $\langle\,$font definitions$\,\rangle$\cr
 * 
 * |post_post| |q[4]| |i[1]| 223's$[{\G}4]$\cr}}$$
 * Here |p| is a pointer to the final |bop| in the file. The next three
 * parameters, |num|, |den|, and |mag|, are duplicates of the quantities that
 * appeared in the preamble.
 * 
 * Parameters |l| and |u| give respectively the height-plus-depth of the tallest
 * page and the width of the widest page, in the same units as other dimensions
 * of the file. These numbers might be used by a \.{DVI}-reading program to
 * position individual ``pages'' on large sheets of film or paper; however,
 * the standard convention for output on normal size paper is to position each
 * page so that the upper left-hand corner is exactly one inch from the left
 * and the top. Experience has shown that it is unwise to design \.{DVI}-to-printer
 * software that attempts cleverly to center the output; a fixed position of
 * the upper left corner is easiest for users to understand and to work with.
 * Therefore |l| and~|u| are often ignored.
 * 
 * Parameter |s| is the maximum stack depth (i.e., the largest excess of
 * |push| commands over |pop| commands) needed to process this file. Then
 * comes |t|, the total number of pages (|bop| commands) present.
 * 
 * The postamble continues with font definitions, which are any number of
 * \\{fnt\_def} commands as described above, possibly interspersed with |nop|
 * commands. Each font number that is used in the \.{DVI} file must be defined
 * exactly twice: Once before it is first selected by a \\{fnt} command, and once
 * in the postamble.
 */

/* module 591 */

/* The last part of the postamble, following the |post_post| byte that
 * signifies the end of the font definitions, contains |q|, a pointer to the
 * |post| command that started the postamble. An identification byte, |i|,
 * comes next; this currently equals~2, as in the preamble.
 * 
 * The |i| byte is followed by four or more bytes that are all equal to
 * the decimal number 223 (i.e., oct(337) in octal). \TeX\ puts out four to seven of
 * these trailing bytes, until the total length of the file is a multiple of
 * four bytes, since this works out best on machines that pack four bytes per
 * word; but any number of 223's is allowed, as long as there are at least four
 * of them. In effect, 223 is a sort of signature that is added at the very end.
 * 
 * This curious way to finish off a \.{DVI} file makes it feasible for
 * \.{DVI}-reading programs to find the postamble first, on most computers,
 * even though \TeX\ wants to write the postamble last. Most operating
 * systems permit random access to individual words or bytes of a file, so
 * the \.{DVI} reader can start at the end and skip backwards over the 223's
 * until finding the identification byte. Then it can back up four bytes, read
 * |q|, and move to byte |q| of the file. This byte should, of course,
 * contain the value 248 (|post|); now the postamble can be read, so the
 * \.{DVI} reader can discover all the information needed for typesetting the
 * pages. Note that it is also possible to skip through the \.{DVI} file at
 * reasonably high speed to locate a particular page, if that proves
 * desirable. This saves a lot of time, since \.{DVI} files used in production
 * jobs tend to be large.
 * 
 * Unfortunately, however, standard \PASCAL\ does not include the ability to
 * 
 * access a random position in a file, or even to determine the length of a file.
 * Almost all systems nowadays provide the necessary capabilities, so \.{DVI}
 * format has been designed to work most efficiently with modern operating systems.
 * But if \.{DVI} files have to be processed under the restrictions of standard
 * \PASCAL, one can simply read them from front to back, since the necessary
 * header information is present in the preamble and in the font definitions.
 * (The |l| and |u| and |s| and |t| parameters, which appear only in the
 * postamble, are ``frills'' that are handy but not absolutely necessary.)
 */

/* module 592 */

/* 
 * After considering \TeX's eyes and stomach, we come now to the bowels.
 * 
 * The |ship_out| procedure is given a pointer to a box; its mission is
 * to describe that box in \.{DVI} form, outputting a ``page'' to |dvi_file|.
 * The \.{DVI} coordinates $(h,v)=(0,0)$ should correspond to the upper left
 * corner of the box being shipped.
 * 
 * Since boxes can be inside of boxes inside of boxes, the main work of
 * |ship_out| is done by two mutually recursive routines, |hlist_out|
 * and |vlist_out|, which traverse the hlists and vlists inside of horizontal
 * and vertical boxes.
 * 
 * As individual pages are being processed, we need to accumulate
 * information about the entire set of pages, since such statistics must be
 * reported in the postamble. The global variables |total_pages|, |max_v|,
 * |max_h|, |max_push|, and |last_bop| are used to record this information.
 * 
 * The variable |doing_leaders| is |true| while leaders are being output.
 * The variable |dead_cycles| contains the number of times an output routine
 * has been initiated since the last |ship_out|.
 * 
 * A few additional global variables are also defined here for use in
 * |vlist_out| and |hlist_out|. They could have been local variables, but
 * that would waste stack space when boxes are deeply nested, since the
 * values of these variables are not needed during recursive calls.
 */

integer total_pages; /* the number of pages that have been shipped out */
scaled max_v; /* maximum height-plus-depth of pages shipped so far */
scaled max_h; /* maximum width of pages shipped so far */
int max_push; /* deepest nesting of |push| commands encountered so far */
int last_bop; /* location of previous |bop| in the \.{DVI} output */
int dead_cycles; /* recent outputs that didn't ship anything out */
boolean doing_leaders; /* are we inside a leader box? */
/* character and font in current |char_node| */
quarterword c;
internal_font_number f;
scaled rule_ht, rule_dp, rule_wd; /* size of current rule being output */
pointer g; /* current glue specification */
int lq, lr; /* quantities used in calculations for leaders */

/* module 594 */

/* The \.{DVI} bytes are output to a buffer instead of being written directly
 * to the output file. This makes it possible to reduce the overhead of
 * subroutine calls, thereby measurably speeding up the computation, since
 * output of \.{DVI} bytes is part of \TeX's inner loop. And it has another
 * advantage as well, since we can change instructions in the buffer in order to
 * make the output more compact. For example, a `|down2|' command can be
 * changed to a `|y2|', thereby making a subsequent `|y0|' command possible,
 * saving two bytes.
 * 
 * The output buffer is divided into two parts of equal size; the bytes found
 * in |dvi_buf[0..half_buf-1]| constitute the first half, and those in
 * |dvi_buf[half_buf..dvi_buf_size-1]| constitute the second. The global
 * variable |dvi_ptr| points to the position that will receive the next
 * output byte. When |dvi_ptr| reaches |dvi_limit|, which is always equal
 * to one of the two values |half_buf| or |dvi_buf_size|, the half buffer that
 * is about to be invaded next is sent to the output and |dvi_limit| is
 * changed to its other value. Thus, there is always at least a half buffer's
 * worth of information present, except at the very beginning of the job.
 * 
 * Bytes of the \.{DVI} file are numbered sequentially starting with 0;
 * the next byte to be generated will be number |dvi_offset+dvi_ptr|.
 * A byte is present in the buffer only if its number is |>=dvi_gone|.
 */


/* module 595 */

/* Some systems may find it more efficient to make |dvi_buf| a |packed|
 * array, since output of four bytes at once may be facilitated.
 */

eight_bits *dvi_buf; /* buffer for \.{DVI} output */
int half_buf; /* half of |dvi_buf_size| */
int dvi_limit; /* end of the current half buffer */
int dvi_ptr; /* the next available buffer address */
int dvi_offset; /* |dvi_buf_size| times the number of times the 
				   output buffer has been fully emptied */
integer dvi_gone; /* the number of bytes already output to |dvi_file| */

/* module 597 */

/* The actual output of |dvi_buf[a..b]| to |dvi_file| is performed by calling
 * |write_dvi(a,b)|. For best results, this procedure should be optimized to
 * run as fast as possible on each particular system, since it is part of
 * \TeX's inner loop. It is safe to assume that |a| and |b+1| will both be
 * multiples of 4 when |write_dvi(a,b)| is called; therefore it is possible on
 * many machines to use efficient methods to pack four bytes per word and to
 * output an array of words with one system call.
 * 
 * In C, we use a macro to call |fwrite| or |write| directly, writing all
 * the bytes in one shot. Much better even than writing four
 * bytes at a time.
 */

#define write_dvi(a, b)                                                 \
  if (fwrite ((char *) &dvi_buf[a], sizeof (dvi_buf[a]),                \
                 (int) ((b) - (a) + 1),dvi_file)                       \
      != (unsigned) ((b) - (a) + 1))                                         \
    FATAL_PERROR ("fwrite");


/* module 598 */
#define dvi_out( arg ) { dvi_buf [ dvi_ptr ]  =  arg ; incr ( dvi_ptr );\
                         if (  dvi_ptr  ==  dvi_limit  )   dvi_swap() ;}


/* module 598 */

/* To put a byte in the buffer without paying the cost of invoking a procedure
 * each time, we use the macro |dvi_out|.
 */
void
dvi_swap (void) { /* outputs half of the buffer */
  if (dvi_limit == dvi_buf_size) {
	write_dvi (0, half_buf - 1);
	dvi_limit = half_buf;
	dvi_offset = dvi_offset + dvi_buf_size;
	dvi_ptr = 0;
  } else {
	write_dvi (half_buf, dvi_buf_size - 1);
	dvi_limit = dvi_buf_size;
  };
  dvi_gone = dvi_gone + half_buf;
};


/* module 600 */

/* The |dvi_four| procedure outputs four bytes in two's complement notation,
 * without risking arithmetic overflow.
 */
void
dvi_four (int x) {
  if (x >= 0) {
    dvi_out (x / 16777216);
  } else {
	x = x + 1073741824;
	x = x + 1073741824;
	dvi_out ((x / 16777216) + 128);
  };
  x = x % 16777216;
  dvi_out (x / 65536);
  x = x % 65536;
  dvi_out (x / 256);
  dvi_out (x % 256);
}


/* module 601 */

/* A mild optimization of the output is performed by the |dvi_pop|
 * routine, which issues a |pop| unless it is possible to cancel a
 * `|push| |pop|' pair. The parameter to |dvi_pop| is the byte address
 * following the old |push| that matches the new |pop|.
 */
void
dvi_pop (int l) {
  if ((l == dvi_offset + dvi_ptr) && (dvi_ptr > 0)) {
    decr (dvi_ptr);
  } else {
    dvi_out (pop);
  }
}


/* module 602 */

/* Here's a procedure that outputs a font definition. Since \TeX82 uses at
 * most 256 different fonts per job, |fnt_def1| is always used as the command code.
 */
void
dvi_font_def (internal_font_number f) {
  pool_pointer k; /* index into |str_pool| */ 
  if (f <= 256 + font_base) {
	dvi_out (fnt_def1);
	dvi_out (f - font_base - 1);
  } else {
	dvi_out (fnt_def1 + 1);
	dvi_out ((f - font_base - 1) / 256);
	dvi_out ((f - font_base - 1) % 256);
  };
  dvi_out (qo (font_check[f].b0));
  dvi_out (qo (font_check[f].b1));
  dvi_out (qo (font_check[f].b2));
  dvi_out (qo (font_check[f].b3));
  dvi_four (font_size[f]);
  dvi_four (font_dsize[f]);
  dvi_out (length (font_area[f]));
  dvi_out (length (font_name[f]));
  /* begin expansion of Output the font name whose internal number is |f| */
  /* module 603 */
  for (k = str_start[font_area[f]]; k <= str_start[font_area[f] + 1] - 1; k++)
    dvi_out (str_pool[k]);
  for (k = str_start[font_name[f]]; k <= str_start[font_name[f] + 1] - 1; k++)
    dvi_out (str_pool[k]);
  /* end expansion of Output the font name whose internal number is |f| */
};


/* module 604 */

/* Versions of \TeX\ intended for small computers might well choose to omit
 * the ideas in the next few parts of this program, since it is not really
 * necessary to optimize the \.{DVI} code by making use of the |w0|, |x0|,
 * |y0|, and |z0| commands. Furthermore, the algorithm that we are about to
 * describe does not pretend to give an optimum reduction in the length
 * of the \.{DVI} code; after all, speed is more important than compactness.
 * But the method is surprisingly effective, and it takes comparatively little
 * time.
 * 
 * We can best understand the basic idea by first considering a simpler problem
 * that has the same essential characteristics. Given a sequence of digits,
 * say $3\,1\,4\,1\,5\,9\,2\,6\,5\,3\,5\,8\,9$, we want to assign subscripts
 * $d$, $y$, or $z$ to each digit so as to maximize the number of ``$y$-hits''
 * and ``$z$-hits''; a $y$-hit is an instance of two appearances of the same
 * digit with the subscript $y$, where no $y$'s intervene between the two
 * appearances, and a $z$-hit is defined similarly. For example, the sequence
 * above could be decorated with subscripts as follows:
 * $$3_z\,1_y\,4_d\,1_y\,5_y\,9_d\,2_d\,6_d\,5_y\,3_z\,5_y\,8_d\,9_d.$$
 * There are three $y$-hits ($1_y\ldots1_y$ and $5_y\ldots5_y\ldots5_y$) and
 * one $z$-hit ($3_z\ldots3_z$); there are no $d$-hits, since the two appearances
 * of $9_d$ have $d$'s between them, but we don't count $d$-hits so it doesn't
 * matter how many there are. These subscripts are analogous to the \.{DVI}
 * commands called \\{down}, $y$, and $z$, and the digits are analogous to
 * different amounts of vertical motion; a $y$-hit or $z$-hit corresponds to
 * the opportunity to use the one-byte commands |y0| or |z0| in a \.{DVI} file.
 * 
 * \TeX's method of assigning subscripts works like this: Append a new digit,
 * say $\delta$, to the right of the sequence. Now look back through the
 * sequence until one of the following things happens: (a)~You see
 * $\delta_y$ or $\delta_z$, and this was the first time you encountered a
 * $y$ or $z$ subscript, respectively. Then assign $y$ or $z$ to the new
 * $\delta$; you have scored a hit. (b)~You see $\delta_d$, and no $y$
 * subscripts have been encountered so far during this search. Then change
 * the previous $\delta_d$ to $\delta_y$ (this corresponds to changing a
 * command in the output buffer), and assign $y$ to the new $\delta$; it's
 * another hit. (c)~You see $\delta_d$, and a $y$ subscript has been seen
 * but not a $z$. Change the previous $\delta_d$ to $\delta_z$ and assign
 * $z$ to the new $\delta$. (d)~You encounter both $y$ and $z$ subscripts
 * before encountering a suitable $\delta$, or you scan all the way to the
 * front of the sequence. Assign $d$ to the new $\delta$; this assignment may
 * be changed later.
 * 
 * The subscripts $3_z\,1_y\,4_d\ldots\,$ in the example above were, in fact,
 * produced by this procedure, as the reader can verify. (Go ahead and try it.)
 */

/* module 605 */

/* In order to implement such an idea, \TeX\ maintains a stack of pointers
 * to the \\{down}, $y$, and $z$ commands that have been generated for the
 * current page. And there is a similar stack for \\{right}, |w|, and |x|
 * commands. These stacks are called the down stack and right stack, and their
 * top elements are maintained in the variables |down_ptr| and |right_ptr|.
 * 
 * Each entry in these stacks contains four fields: The |width| field is
 * the amount of motion down or to the right; the |location| field is the
 * byte number of the \.{DVI} command in question (including the appropriate
 * |dvi_offset|); the |link| field points to the next item below this one
 * on the stack; and the |info| field encodes the options for possible change
 * in the \.{DVI} command.
 */
#define movement_node_size 3
#define location( arg )  mem [ arg  + 2 ]. cint

pointer down_ptr, right_ptr; /* heads of the down and right stacks */

/* module 608 */

/* The |info| fields in the entries of the down stack or the right stack
 * have six possible settings: |y_here| or |z_here| mean that the \.{DVI}
 * command refers to |y| or |z|, respectively (or to |w| or |x|, in the
 * case of horizontal motion); |yz_OK| means that the \.{DVI} command is
 * \\{down} (or \\{right}) but can be changed to either |y| or |z| (or
 * to either |w| or |x|); |y_OK| means that it is \\{down} and can be changed
 * to |y| but not |z|; |z_OK| is similar; and |d_fixed| means it must stay
 * \\{down}.
 * 
 * The four settings |yz_OK|, |y_OK|, |z_OK|, |d_fixed| would not need to
 * be distinguished from each other if we were simply solving the
 * digit-subscripting problem mentioned above. But in \TeX's case there is
 * a complication because of the nested structure of |push| and |pop|
 * commands. Suppose we add parentheses to the digit-subscripting problem,
 * redefining hits so that $\delta_y\ldots \delta_y$ is a hit if all $y$'s between
 * the $\delta$'s are enclosed in properly nested parentheses, and if the
 * parenthesis level of the right-hand $\delta_y$ is deeper than or equal to
 * that of the left-hand one. Thus, `(' and `)' correspond to `|push|'
 * and `|pop|'. Now if we want to assign a subscript to the final 1 in the
 * sequence
 * $$2_y\,7_d\,1_d\,(\,8_z\,2_y\,8_z\,)\,1$$
 * we cannot change the previous $1_d$ to $1_y$, since that would invalidate
 * the $2_y\ldots2_y$ hit. But we can change it to $1_z$, scoring a hit
 * since the intervening $8_z$'s are enclosed in parentheses.
 * 
 * The program below removes movement nodes that are introduced after a |push|,
 * before it outputs the corresponding |pop|.
 */
#define y_here 1
#define z_here 2
#define yz_OK 3
#define y_OK 4
#define z_OK 5
#define d_fixed 6

/* module 611 */
#define none_seen 0
#define y_seen 6
#define z_seen 12

/* module 607 */

/* Here is a subroutine that produces a \.{DVI} command for some specified
 * downward or rightward motion. It has two parameters: |w| is the amount
 * of motion, and |o| is either |down1| or |right1|. We use the fact that
 * the command codes have convenient arithmetic properties: |y1-down1=w1-right1|
 * and |z1-down1=x1-right1|.
 */
void
movement (scaled w, eight_bits o) {
  small_number mstate; /* have we seen a |y| or |z|? */ 
  pointer p, q; /* current and top nodes on the stack */ 
  int k; /* index into |dvi_buf|, modulo |dvi_buf_size| */ 
  q = get_node (movement_node_size); /* new node for the top of the stack */ 
  width (q) = w;
  location (q) = dvi_offset + dvi_ptr;
  if (o == down1) {
	link (q) = down_ptr;
	down_ptr = q;
  } else {
	link (q) = right_ptr;
	right_ptr = q;
  };
  /* begin expansion of Look at the other stack entries until deciding what sort 
	 of \.{DVI} command to generate; |goto found| if node |p| is a ``hit'' */
  /* module 611 */
  /* As we search through the stack, we are in one of three states,
   * |y_seen|, |z_seen|, or |none_seen|, depending on whether we have
   * encountered |y_here| or |z_here| nodes. These states are encoded as
   * multiples of 6, so that they can be added to the |info| fields for quick
   * decision-making.
   */
  p = link (q);
  mstate = none_seen;
  while (p != null) {
	if (width (p) == w) {
	  /* begin expansion of Consider a node with matching width; |goto found| if it's a hit */
	  /* module 612 */
	  /* We might find a valid hit in a |y| or |z| byte that is already gone
	   * from the buffer. But we can't change bytes that are gone forever; ``the
	   * moving finger writes, $\ldots\,\,$.''
	   */
	  switch (mstate + info (p)) {
	  case none_seen + yz_OK:
	  case none_seen + y_OK:
	  case z_seen + yz_OK:
	  case z_seen + y_OK:
		if (location (p) < dvi_gone) {
		  goto NOT_FOUND;
		} else {
		  /* begin expansion of Change buffered instruction to |y| or |w| and |goto found| */
		  /* module 613 */
		  k = location (p) - dvi_offset;
		  if (k < 0)
			k = k + dvi_buf_size;
		  dvi_buf[k] = dvi_buf[k] + y1 - down1;
		  info (p) = y_here;
		  goto FOUND;
		  /* end expansion of Change buffered instruction to |y| or |w| and |goto found| */
		}
	  case none_seen + z_OK:
	  case y_seen + yz_OK:
	  case y_seen + z_OK:
		if (location (p) < dvi_gone) {
		  goto NOT_FOUND;
		} else {
		  /* begin expansion of Change buffered instruction to |z| or |x| and |goto found| */
		  /* module 614 */
		  k = location (p) - dvi_offset;
		  if (k < 0)
			k = k + dvi_buf_size;
		  dvi_buf[k] = dvi_buf[k] + z1 - down1;
		  info (p) = z_here;
		  goto FOUND;
		  /* end expansion of Change buffered instruction to |z| or |x| and |goto found| */
		};
	  case none_seen + y_here:
	  case none_seen + z_here:
	  case y_seen + z_here:
	  case z_seen + y_here:
		do_something;
		goto FOUND;
	  default:
		do_nothing;
	  }
	  /* end expansion of Consider a node with matching width; |goto found| if it's a hit */
	} else {
	  switch (mstate + info (p)) {
	  case none_seen + y_here:
	    mstate = y_seen;
	    break;
	  case none_seen + z_here:
	    mstate = z_seen;
	    break;
	  case y_seen + z_here:
	  case z_seen + y_here:
		do_something;
	    goto NOT_FOUND;
	  default:
	    do_nothing;
	  };
    };
	p = link (p);
  };
 NOT_FOUND:
  /* end expansion of Look at the other stack entries until deciding what sor... */
  /* begin expansion of Generate a |down| or |right| command for |w| and |return| */
  /* module 610 */
  info (q) = yz_OK;
  if (abs (w) >= 8388608) {
	dvi_out (o + 3); /* |down4| or |right4| */ 
	dvi_four (w);
	return;
  };
  if (abs (w) >= 32768) {
	dvi_out (o + 2); /* |down3| or |right3| */ 
	if (w < 0)
	  w = w + 16777216;
	dvi_out (w / 65536);
	w = w % 65536;
	goto L2;
  };
  if (abs (w) >= 128) {
	dvi_out (o + 1);/* |down2| or |right2| */ 
	if (w < 0)
	  w = w + 65536;
	goto L2;
  };
  dvi_out (o); /* |down1| or |right1| */ 
  if (w < 0)
    w = w + 256;
  goto L1;
 L2:
  dvi_out (w / 256);
 L1:
  dvi_out (w % 256);
  return;
  /* end expansion of Generate a |down| or |right| command for |w| and |return| */
 FOUND:
  /* begin expansion of Generate a |y0| or |z0| command in order 
	 to reuse a previous appearance of~|w| */
  /* module 609 */
  /* When the |movement| procedure gets to the label |found|, the value of
   * |info(p)| will be either |y_here| or |z_here|. If it is, say, |y_here|,
   * the procedure generates a |y0| command (or a |w0| command), and marks
   * all |info| fields between |q| and |p| so that |y| is not OK in that range.
   */
  info (q) = info (p);
  if (info (q) == y_here) {
	dvi_out (o + y0 - down1); /* |y0| or |w0| */ 
	while (link (q) != p) {
	  q = link (q);
	  switch (info (q)) {
	  case yz_OK:
		info (q) = z_OK;
		break;
	  case y_OK:
		info (q) = d_fixed;
		break;
	  default:
		do_nothing;
	  };
	};
  } else {
	dvi_out (o + z0 - down1); /* |z0| or |x0| */ 
	while (link (q) != p) {
	  q = link (q);
	  switch (info (q)) {
	  case yz_OK:
		info (q) = y_OK;
		break;
	  case z_OK:
		info (q) = d_fixed;
		break;
	  default:
		do_nothing;
	  };
	};
  };
  /* end expansion of Generate a |y0| or |z0| command in order to re.... */
}

/* module 616 */

/* The actual distances by which we want to move might be computed as the
 * sum of several separate movements. For example, there might be several
 * glue nodes in succession, or we might want to move right by the width of
 * some box plus some amount of glue. More importantly, the baselineskip
 * distances are computed in terms of glue together with the depth and
 * height of adjacent boxes, and we want the \.{DVI} file to lump these
 * three quantities together into a single motion.
 * 
 * Therefore, \TeX\ maintains two pairs of global variables: |dvi_h| and |dvi_v|
 * are the |h| and |v| coordinates corresponding to the commands actually
 * output to the \.{DVI} file, while |cur_h| and |cur_v| are the coordinates
 * corresponding to the current state of the output routines. Coordinate
 * changes will accumulate in |cur_h| and |cur_v| without being reflected
 * in the output, until such a change becomes necessary or desirable; we
 * can call the |movement| procedure whenever we want to make |dvi_h=cur_h|
 * or |dvi_v=cur_v|.
 * 
 * The current font reflected in the \.{DVI} output is called |dvi_f|;
 * there is no need for a `\\{cur\_f}' variable.
 * 
 * The depth of nesting of |hlist_out| and |vlist_out| is called |cur_s|;
 * this is essentially the depth of |push| commands in the \.{DVI} output.
 * 
 * For mixed direction text (\TeXXeT) the current text direction is called
 * |cur_dir|. As the box being shipped out will never be used again and
 * soon be recycled, we can simply reverse any R-text (i.e., right-to-left)
 * segments of hlist nodes as well as complete hlist nodes embedded in such
 * segments. Moreover this can be done iteratively rather than recursively.
 * There are, however, two complications related to leaders that require
 * some additional bookkeeping: (1)~One and the same hlist node might be
 * used more than once (but never inside both L- and R-text); and
 * (2)~leader boxes inside hlists must be aligned with respect to the left
 * edge of the original hlist.
 * 
 * A math node is changed into a kern node whenever the text direction
 * remains the same, it is replaced by an |edge_node| if the text direction
 * changes; the subtype of an an |hlist_node| inside R-text is changed to
 * |reversed| once its hlist has been reversed.
 */
#define synch_h   if (cur_h != dvi_h)  { movement (cur_h - dvi_h, right1); dvi_h = cur_h ;}
#define synch_v   if (cur_v != dvi_v)  { movement (cur_v - dvi_v, down1); dvi_v = cur_v ;}

scaled dvi_h, dvi_v; /* a \.{DVI} reader program thinks we are here */
scaled cur_h, cur_v; /* \TeX\ thinks we are here */
internal_font_number dvi_f; /* the current font */
int cur_s; /* current depth of output box nesting, initially $-1$ */

/* module 615 */

/* In case you are wondering when all the movement nodes are removed from
 * \TeX's memory, the answer is that they are recycled just before
 * |hlist_out| and |vlist_out| finish outputting a box. This restores the
 * down and right stacks to the state they were in before the box was output,
 * except that some |info|'s may have become more restrictive.
 */
void
prune_movements (int l) { /* delete movement nodes with |location>=l| */
  pointer p; /* node being deleted */ 
  while (down_ptr != null) {
	if (location (down_ptr) < l)
	  goto DONE;
	p = down_ptr;
	down_ptr = link (p);
	free_node (p, movement_node_size);
  };
 DONE:
  while (right_ptr != null) {
	if (location (right_ptr) < l)
	  return;
	p = right_ptr;
	right_ptr = link (p);
	free_node (p, movement_node_size);
  };
}

/* module 618 */

/* When |hlist_out| is called, its duty is to output the box represented
 * by the |hlist_node| pointed to by |temp_ptr|. The reference point of that
 * box has coordinates |(cur_h,cur_v)|.
 * 
 * Similarly, when |vlist_out| is called, its duty is to output the box represented
 * by the |vlist_node| pointed to by |temp_ptr|. The reference point of that
 * box has coordinates |(cur_h,cur_v)|.
 */
/* |hlist_out| and |vlist_out| are mutually recursive */


/* module 619 */

/* The recursive procedures |hlist_out| and |vlist_out| each have local variables
 * |save_h| and |save_v| to hold the values of |dvi_h| and |dvi_v| just before
 * entering a new level of recursion. In effect, the values of |save_h| and
 * |save_v| on \TeX's run-time stack correspond to the values of |h| and |v|
 * that a \.{DVI}-reading program will push onto its coordinate stack.
 */

void 
hlist_out (void) {
  /* output an |hlist_node| box */
  scaled base_line;      /* the baseline coordinate for this box */ 
  scaled left_edge;      /* the left coordinate for this box */ 
  scaled save_h, save_v; /* what |dvi_h| and |dvi_v| should pop to */ 
  pointer this_box;      /* pointer to containing box */ 
  unsigned char g_sign;  /* selects type of glue */ 
  pointer p;             /* current position in the hlist */ 
  int save_loc;          /* \.{DVI} byte location upon entry */ 
  pointer leader_box;    /* the leader box being replicated */ 
  scaled leader_wd;      /* width of leader box being replicated */ 
  scaled lx;             /* extra space between leader boxes */ 
  boolean outer_doing_leaders; /* were we doing leaders? */ 
  scaled edge;           /* right edge of sub-box or leader space */ 
  pointer prev_p;        /* one step behind |p| */ 
  this_box = temp_ptr;
  g_sign = glue_sign (this_box);
  p = list_ptr (this_box);
  incr (cur_s);
  if (cur_s > 0)
	dvi_out (push);
  if (cur_s > max_push)
	max_push = cur_s;
  save_loc = dvi_offset + dvi_ptr;
  base_line = cur_v;
  prev_p = this_box + list_offset;
  if (eTeX_ex) {
	/* Initialize the LR stack */
	initialize_LR_stack;
	if (subtype (this_box) == dlist) {
	  if (cur_dir == right_to_left) {
		cur_dir = left_to_right;
		cur_h = cur_h - width (this_box);
	  } else {
		subtype (this_box) = min_quarterword; 
	  }
	}
	if ((cur_dir == right_to_left) && (subtype (this_box) != reversed))
	  /* Reverse the complete hlist and set the subtype to |reversed| */
	  reverse_the_hlist;
  };
  left_edge = cur_h;
  while (p != null) {
	/* begin expansion of Output node |p| for |hlist_out| and move to the next node, 
	   maintaining the condition |cur_v=base_line| */
	/* module 620 */
	/* We ought to give special care to the efficiency of one part of |hlist_out|,
	 * since it belongs to \TeX's inner loop. When a |char_node| is encountered,
	 * we save a little time by processing several nodes in succession until
	 * reaching a non-|char_node|. The program uses the fact that |set_char_0=0|.
	 * 
	 * In ML\TeX{} this part looks for the existence of a substitution
	 * definition for a character |c|, if |c| does not exist in the font,
	 * and create appropriate \.{DVI} commands. Former versions of ML\TeX{}
	 * have spliced appropriate character, kern, and box nodes into the
	 * horizontal list.
	 * %
	 * % 91/05/08 \charsubdefmax bug detected by Bernd Raichle
	 * Because the user can change character substitions or
	 * \.{\\charsubdefmax} on the fly, we have to test a again
	 * for valid substitutions.
	 * %
	 * % 93/10/29 \leaders bug detected by Eberhard Mattes
	 * (Additional it is necessary to be careful---if leaders are used
	 * the current hlist is normally traversed more than once!)
	 */
  RESWITCH:
	if (is_char_node (p)) {
	  synch_h;
	  synch_v;
	  do {
		f = font (p);
		c = character (p);
		if (f != dvi_f) {
		  /* begin expansion of Change font |dvi_f| to |f| */
		  /* module 621 */
		  if (!font_used[f]) {
			dvi_font_def (f);
			font_used[f] = true;
		  };
		  if (f <= 64 + font_base) {
			dvi_out (f - font_base - 1 + fnt_num_0);
		  } else if (f <= 256 + font_base) {
			dvi_out (fnt1);
			dvi_out (f - font_base - 1);
		  } else {
			dvi_out (fnt1 + 1);
			dvi_out ((f - font_base - 1) / 256);
			dvi_out ((f - font_base - 1) % 256);
		  };
		  dvi_f = f;
		};
		/* end expansion of Change font |dvi_f| to |f| */
		if (font_ec[f] >= qo (c))
		  if (font_bc[f] <= qo (c))
			if (char_exists (orig_char_info (f,c))) {/* N.B.: not |char_info| */
			  if (c >= qi (128))
				dvi_out (set1);
			  dvi_out (qo (c));
			  cur_h = cur_h + char_width (f, orig_char_info (f,c));
			  goto CONTINUE;
			};
		if (mltex_enabled_p)
		  /* begin expansion of Output a substitution, |goto continue| if not possible */
		  /* module 1824 */
		  /* This code is called for a virtual character |c| in |hlist_out|
		   * during |ship_out|. It tries to built a character substitution
		   * construct for |c| generating appropriate \.{DVI} code using the
		   * character substitution definition for this character. If a valid
		   * character substitution exists \.{DVI} code is created as if
		   * |make_accent| was used. In all other cases the status of the
		   * substituion for this character has been changed between the creation
		   * of the character node in the hlist and the output of the page---the
		   * created \.{DVI} code will be correct but the visual result will be
		   * undefined.
		   * 
		   * Former ML\TeX\ versions have replaced the character node by a
		   * sequence of character, box, and accent kern nodes splicing them into
		   * the original horizontal list. This version does not do this to avoid
		   * a)~a memory overflow at this processing stage, b)~additional code to
		   * add a pointer to the previous node needed for the replacement, and
		   * c)~to avoid wrong code resulting in anomalies because of the use
		   * within a \.{\\leaders} box.
		   */
		  {
			/* begin expansion of Get substitution information, check it, goto |found| if all 
			   is ok, otherwise goto |continue| */
			get_substitution_info;
			/* end expansion of Get substitution information, check it, goto |found| ..*/
		  FOUND:
			/* begin expansion of Print character substition tracing log */
			print_substitution_log;
			/* end expansion of Print character substition tracing log */
			/* begin expansion of Rebuild character using substitution information */
			/* module 1828 */
			/* This outputs the accent and the base character given in the
			 * substitution. It uses code virtually identical to the |make_accent|
			 * procedure, but without the node creation steps.
			 * 
			 * Additionally if the accent character has to be shifted vertically it
			 * does {\it not\/} create the same code. The original routine in
			 * |make_accent| and former versions of ML\TeX{} creates a box node
			 * resulting in |push| and |pop| operations, whereas this code simply
			 * produces vertical positioning operations. This can influence the
			 * pixel rounding algorithm in some \.{DVI} drivers---and therefore will
			 * probably be changed in one of the next ML\TeX{} versions.
			 */
			base_x_height = x_height (f);
			base_slant = slant (f) / FLOAT_CONSTANT (65536);
			accent_slant = base_slant; /* slant of accent character font */ 
			base_width = char_width (f, ib_c);
			base_height = char_height (f, height_depth (ib_c));
			accent_width = char_width (f, ia_c);
			accent_height = char_height (f, height_depth (ia_c)); 
			/* compute necessary horizontal shift (don't forget slant) */
			delta = round ((base_width - accent_width) / FLOAT_CONSTANT (2)	 + base_height
						   * base_slant - base_x_height * accent_slant);
			dvi_h = cur_h;
			/* update |dvi_h|, similar to the last statement in module 620 */
			/* 1. For centering/horizontal shifting insert a kern node. */
			cur_h = cur_h + delta;
			synch_h;
			/* 2. Then insert the accent character possibly shifted up or down. */
			if (((base_height != base_x_height) && (accent_height > 0))) {
			  /* the accent must be shifted up or down */
			  cur_v = base_line + (base_x_height - base_height);
			  synch_v;
			  if (accent_c >= 128)
				dvi_out (set1);
			  dvi_out (accent_c);
			  cur_v = base_line;
			} else {
			  synch_v;
			  if (accent_c >= 128)
				dvi_out (set1);
			  dvi_out (accent_c);
			};
			cur_h = cur_h + accent_width;
			dvi_h = cur_h;
			/* 3. For centering/horizontal shifting insert another kern node. */
			cur_h = cur_h + (-accent_width - delta);
			/* 4. Output the base character. */ 
			synch_h;
			synch_v;
			if (base_c >= 128)
			  dvi_out (set1);
			dvi_out (base_c);
			cur_h = cur_h + base_width;
			dvi_h = cur_h;	/* update of |dvi_h| is unnecessary, will be set in module 620 */
			/* end expansion of Rebuild character using substitution information */
		  };
		/* end expansion of Output a substitution, |goto continue| if not possible */
	  CONTINUE:
		prev_p = link (prev_p); /* N.B.: not |prev_p:=p|, |p| might be |lig_trick| */
		p = link (p);
	  } while (is_char_node (p));
	  dvi_h = cur_h;
	}  else {
	  /* begin expansion of Output the non-|char_node| |p| for |hlist_out| and move to the next node */
	  /* module 622 */
	  switch (type (p)) {
	  case hlist_node:
	  case vlist_node:
		/* begin expansion of Output a box in an hlist */
		/* module 623 */
		if (list_ptr (p) == null) {
		  cur_h = cur_h + width (p);
		} else {
		  save_h = dvi_h;
		  save_v = dvi_v;
		  cur_v = base_line + shift_amount (p); /* shift the box down */ 
		  temp_ptr = p;
		  edge = cur_h + width (p);
		  if (cur_dir == right_to_left)
			cur_h = edge;
		  if (type (p) == vlist_node) {
			vlist_out();
		  } else {
			hlist_out();
		  }
		  dvi_h = save_h;
		  dvi_v = save_v;
		  cur_h = edge;
		  cur_v = base_line;
		};
		/* end expansion of Output a box in an hlist */
		break;
	  case rule_node:
		{
		  rule_ht = height (p);
		  rule_dp = depth (p);
		  rule_wd = width (p);
		  goto FIN_RULE;
		};
		break;
	  case whatsit_node:
		/* begin expansion of Output the whatsit node |p| in an hlist */
		/* module 1563 */
		out_what (p);
		/* end expansion of Output the whatsit node |p| in an hlist */
		break;
	  case glue_node:
		/* begin expansion of Move right or output leaders */
		/* module 625 */
		g = glue_ptr (p);
		rule_wd = width (g);
		if (g_sign != normal)
		    add_glue (rule_wd);
		rule_wd = rule_wd;
		if (subtype (p) >= a_leaders) {
		  /* begin expansion of Output leaders in an hlist, |goto FIN_RULE| if a rule or to |NEXTP| if done */
		  /* module 626 */
		  leader_box = leader_ptr (p);
		  if (type (leader_box) == rule_node) {
			rule_ht = height (leader_box);
			rule_dp = depth (leader_box);
			goto FIN_RULE;
		  };
		  leader_wd = width (leader_box);
		  if ((leader_wd > 0) && (rule_wd > 0)) {
			rule_wd = rule_wd + 10; /* compensate for floating-point rounding */
			if (cur_dir == right_to_left)
			  cur_h = cur_h - 10;
			edge = cur_h + rule_wd;
			lx = 0;
			/* begin expansion of Let |cur_h| be the position of the first box, and set 
			   |leader_wd+lx| to the spacing between corresponding parts of boxes */
			/* module 627 */
			/* The calculations related to leaders require a bit of care. First, in the
			 * case of |a_leaders| (aligned leaders), we want to move |cur_h| to
			 * |left_edge| plus the smallest multiple of |leader_wd| for which the result
			 * is not less than the current value of |cur_h|; i.e., |cur_h| should become
			 * $|left_edge|+|leader_wd|\times\lceil
			 * (|cur_h|-|left_edge|)/|leader_wd|\rceil$. The program here should work in
			 * all cases even though some implementations of \PASCAL\ give nonstandard
			 * results for the |div| operation when |cur_h| is less than |left_edge|.
			 * 
			 * In the case of |c_leaders| (centered leaders), we want to increase |cur_h|
			 * by half of the excess space not occupied by the leaders; and in the
			 * case of |x_leaders| (expanded leaders) we increase |cur_h|
			 * by $1/(q+1)$ of this excess space, where $q$ is the number of times the
			 * leader box will be replicated. Slight inaccuracies in the division might
			 * accumulate; half of this rounding error is placed at each end of the leaders.
			 */
			if (subtype (p) == a_leaders) {
			  save_h = cur_h;
			  cur_h = left_edge + leader_wd * ((cur_h - left_edge) / leader_wd);
			  if (cur_h < save_h)
				cur_h = cur_h + leader_wd;
			} else {
			  lq = rule_wd / leader_wd; /* the number of box copies */ 
			  lr = rule_wd % leader_wd; /* the remaining space */
			  if (subtype (p) == c_leaders) {
				cur_h = cur_h + (lr / 2);
			  } else {
				lx = lr / (lq + 1);
				cur_h = cur_h + ((lr - (lq - 1) * lx) / 2);
			  };
			}
			/* end expansion of Let |cur_h| be the position of the first box, ...*/
			while (cur_h + leader_wd <= edge) {
			  /* begin expansion of Output a leader box at |cur_h|, then advance |cur_h| by |leader_wd+lx| */
			  /* module 628 */
			  /* The `\\{synch}' operations here are intended to decrease the number of
			   * bytes needed to specify horizontal and vertical motion in the \.{DVI} output.
			   */
			  cur_v = base_line + shift_amount (leader_box);
			  synch_v;
			  save_v = dvi_v;
			  synch_h;
			  save_h = dvi_h;
			  temp_ptr = leader_box;
			  if (cur_dir == right_to_left)
				cur_h = cur_h + leader_wd;
			  outer_doing_leaders = doing_leaders;
			  doing_leaders = true;
			  if (type (leader_box) == vlist_node) {
				vlist_out();
			  } else {
				hlist_out();
			  }
			  doing_leaders = outer_doing_leaders;
			  dvi_v = save_v;
			  dvi_h = save_h;
			  cur_v = base_line;
			  cur_h = save_h + leader_wd + lx;
			  /* end expansion of Output a leader box at |cur_h|, then advance |cur_h| by |leader_wd+lx| */
			};
			if (cur_dir == right_to_left) {
			  cur_h = edge;
			} else {
			  cur_h = edge - 10;
			}
			goto NEXTP;
		  };
		};
		/* end expansion of Output leaders in an hlist, |goto FIN_RULE| if a rule or to |NEXTP| if done */
		do_something;
		goto MOVE_PAST;
		/* end expansion of Move right or output leaders */
		break;
	  case kern_node:
	  case math_node:
		cur_h = cur_h + width (p);
		break;
	  case margin_kern_node:
		cur_h = cur_h + width (p);
		break;
	  case ligature_node:
		/* begin expansion of Make node |p| look like a |char_node| and |goto reswitch| */
		p = mimic_char_node (p);
		goto RESWITCH;
	  case edge_node:
		/* Cases of |hlist_out| that arise in mixed direction text only */
		hlist_edge_node;
		break;
	  default:
		do_nothing;
	  };
	  do_something;
	  goto NEXTP;
	FIN_RULE:
	  /* begin expansion of Output a rule in an hlist */
	  /* module 624 */
	  if (is_running (rule_ht))
		rule_ht = height (this_box);
	  if (is_running (rule_dp))
		rule_dp = depth (this_box);
	  rule_ht = rule_ht + rule_dp; /* this is the rule thickness */ 
	  if ((rule_ht > 0) && (rule_wd > 0)) { /* we don't output empty rules */
		synch_h;
		cur_v = base_line + rule_dp;
		synch_v;
		dvi_out (set_rule);
		dvi_four (rule_ht);
		dvi_four (rule_wd);
		cur_v = base_line;
		dvi_h = dvi_h + rule_wd;
	  };
	  /* end expansion of Output a rule in an hlist */
	MOVE_PAST:
	  cur_h = cur_h + rule_wd;
	NEXTP:
	  prev_p = p;
	  p = link (p);
	}
	/* end expansion of Output the non-|char_node| |p| for |hlist_out| and move to the next node */
	/* end expansion of Output node |p| for |hlist_out| and move to the next node,... */
  };
  if (eTeX_ex) {
	/* Check for LR anomalies at the end of |hlist_out| */
	check_LR_after_hlist_out;
	if (subtype (this_box) == dlist)
	  cur_dir = right_to_left;
  };
  prune_movements (save_loc);
  if (cur_s > 0)
	dvi_pop (save_loc);
  decr (cur_s);
};


/* module 629 */

/* The |vlist_out| routine is similar to |hlist_out|, but a bit simpler.
 */
void 
vlist_out (void) {	/* output a |vlist_node| box */
  scaled left_edge; /* the left coordinate for this box */ 
  scaled top_edge; /* the top coordinate for this box */ 
  scaled save_h, save_v; /* what |dvi_h| and |dvi_v| should pop to */ 
  pointer this_box; /* pointer to containing box */ 
  unsigned char g_sign; /* selects type of glue */ 
  pointer p; /* current position in the vlist */ 
  int save_loc; /* \.{DVI} byte location upon entry */ 
  pointer leader_box; /* the leader box being replicated */ 
  scaled leader_ht; /* height of leader box being replicated */ 
  scaled lx; /* extra space between leader boxes */ 
  boolean outer_doing_leaders; /* were we doing leaders? */ 
  scaled edge; /* bottom boundary of leader space */ 
  this_box = temp_ptr;
  g_sign = glue_sign (this_box);
  p = list_ptr (this_box);
  incr (cur_s);
  if (cur_s > 0)
	dvi_out (push);
  if (cur_s > max_push)
	max_push = cur_s;
  save_loc = dvi_offset + dvi_ptr;
  left_edge = cur_h;
  cur_v = cur_v - height (this_box);
  top_edge = cur_v;
  while (p != null) {
	/* begin expansion of Output node |p| for |vlist_out| and move to the 
	   next node, maintaining the condition |cur_h=left_edge| */
	/* module 630 */
	if (is_char_node (p)) {
	  confusion ("vlistout");
	} else {
	  /* begin expansion of Output the non-|char_node| |p| for |vlist_out| */
	  /* module 631 */
	  switch (type (p)) {
	  case hlist_node:
	  case vlist_node:
		/* begin expansion of Output a box in a vlist */
		/* module 632 */
		/* The |synch_v| here allows the \.{DVI} output to use one-byte commands
		 * for adjusting |v| in most cases, since the baselineskip distance will
		 * usually be constant.
		 */
		if (list_ptr (p) == null) {
		  cur_v = cur_v + height (p) + depth (p);
		} else {
		  cur_v = cur_v + height (p);
		  synch_v;
		  save_h = dvi_h;
		  save_v = dvi_v;
		  if (cur_dir == right_to_left) {
			cur_h = left_edge - shift_amount (p);
		  } else {
			cur_h = left_edge + shift_amount (p); /* shift the box right */ 
		  }
		  temp_ptr = p;
		  if (type (p) == vlist_node) {
			vlist_out();
		  } else {
			hlist_out();
		  }
		  dvi_h = save_h;
		  dvi_v = save_v;
		  cur_v = save_v + depth (p);
		  cur_h = left_edge;
		  /* end expansion of Output a box in a vlist */
		};
		break;
	  case rule_node:
		rule_ht = height (p);
		rule_dp = depth (p);
		rule_wd = width (p);
		goto FIN_RULE;
	  case whatsit_node:
		/* begin expansion of Output the whatsit node |p| in a vlist */
		/* module 1562 */
		out_what (p);
		/* end expansion of Output the whatsit node |p| in a vlist */
		break;
	  case glue_node:
		/* begin expansion of Move down or output leaders */
		/* module 634 */
		g = glue_ptr (p);
		rule_ht = width (g);
		if (g_sign != normal)
		  add_glue (rule_ht);
		rule_ht = rule_ht;
		if (subtype (p) >= a_leaders) {
		  /* begin expansion of Output leaders in a vlist, |goto FIN_RULE| 
			 if a rule or to |NEXTP| if done */
		  /* module 635 */
		  leader_box = leader_ptr (p);
		  if (type (leader_box) == rule_node) {
			rule_wd = width (leader_box);
			rule_dp = 0;
			goto FIN_RULE;
		  };
		  leader_ht = height (leader_box) + depth (leader_box);
		  if ((leader_ht > 0) && (rule_ht > 0)) {
			rule_ht = rule_ht + 10; /* compensate for floating-point rounding */
			edge = cur_v + rule_ht;
			lx = 0;
			/* begin expansion of Let |cur_v| be the position of the first box, 
			   and set |leader_ht+lx| to the spacing between corresponding parts of boxes */
			/* module 636 */
			if (subtype (p) == a_leaders) {
			  save_v = cur_v;
			  cur_v = top_edge +leader_ht * ((cur_v - top_edge) / leader_ht);
			  if (cur_v < save_v)
				cur_v = cur_v + leader_ht;
			} else {
			  lq = rule_ht / leader_ht; /* the number of box copies */ 
			  lr = rule_ht % leader_ht; /* the remaining space */
			  if (subtype (p) == c_leaders) {
				cur_v = cur_v + (lr / 2);
			  } else {
				lx = lr / (lq + 1);
				cur_v = cur_v + ((lr - (lq - 1) * lx) / 2);
			  };
			}
			/* end expansion of Let |cur_v| be the position of the first box, ..*/
			while (cur_v + leader_ht <= edge) {
			  /* begin expansion of Output a leader box at |cur_v|, then advance |cur_v| by |leader_ht+lx| */
			  /* module 637 */
			  /* When we reach this part of the program, |cur_v| indicates the top of a
			   * leader box, not its baseline.
			   */
			  if (cur_dir == right_to_left) {
				cur_h = left_edge - shift_amount (leader_box);
			  } else {
				cur_h = left_edge + shift_amount (leader_box); 
			  }
			  synch_h;
			  save_h = dvi_h;
			  cur_v = cur_v + height (leader_box);
			  synch_v;
			  save_v = dvi_v;
			  temp_ptr = leader_box;
			  outer_doing_leaders = doing_leaders;
			  doing_leaders = true;
			  if (type (leader_box) == vlist_node) {
				vlist_out();
			  } else {
				hlist_out();
			  }
			  doing_leaders = outer_doing_leaders;
			  dvi_v = save_v;
			  dvi_h = save_h;
			  cur_h = left_edge;
			  cur_v = save_v - height (leader_box) + leader_ht + lx;
			  /* end expansion of Output a leader box at |cur_v|, then advance |cur_v| by |leader_ht+lx| */
			};
			cur_v = edge - 10;
			goto NEXTP;
		  };
		  /* end expansion of Output leaders in a vlist, |goto FIN_RULE| if a rule or to |NEXTP| if done */
		}
		do_something;
		goto MOVE_PAST;
		/* end expansion of Move down or output leaders */
	  case kern_node:
		cur_v = cur_v + width (p);
		break;
	  default:
		do_nothing;
	  };
	  do_something;
	  goto NEXTP;
	FIN_RULE:
	  /* begin expansion of Output a rule in a vlist, |goto NEXTP| */
	  /* module 633 */
	  if (is_running (rule_wd))
		rule_wd = width (this_box);
	  rule_ht = rule_ht + rule_dp; /* this is the rule thickness */ 
	  cur_v = cur_v + rule_ht;
	  if ((rule_ht > 0) && (rule_wd > 0)) { /* we don't output empty rules */
		if (cur_dir == right_to_left)
		  cur_h = cur_h - rule_wd;
		synch_h;
		synch_v;
		dvi_out (put_rule);
		dvi_four (rule_ht);
		dvi_four (rule_wd);
		cur_h = left_edge;
	  };
	  goto NEXTP;
	  /* end expansion of Output a rule in a vlist, |goto NEXTP| */
	MOVE_PAST:
	  cur_v = cur_v + rule_ht;
	  /* end expansion of Output the non-|char_node| |p| for |vlist_out| */
	};
  NEXTP:
	p = link (p);
	/* end expansion of Output node |p| for |vlist_out| and move to the next node... */
  };
  prune_movements (save_loc);
  if (cur_s > 0)
	dvi_pop (save_loc);
  decr (cur_s);
}


/* module 638 */

/* The |hlist_out| and |vlist_out| procedures are now complete, so we are
 * ready for the |dvi_ship_out| routine that gets them started in the first place.
 */
void 
dvi_ship_out (pointer p) { /* output the box |p| */
  int page_loc; /* location of the current |bop| */ 
  unsigned char j, k; /* indices to first ten count registers */ 
  pool_pointer s; /* index into |str_pool| */ 
  unsigned char old_setting; /* saved |selector| setting */ 
  if (tracing_output > 0) {
	print_nl_string("");
	print_ln();
	zprint_string("Completed box being shipped out");
  };
  if (term_offset > (unsigned)max_print_line - 9) {
	print_ln();
  } else if ((term_offset > 0) || (file_offset > 0)) {
	print_char (' ');
  }
  print_char ('[');
  j = 9;
  while ((count (j) == 0) && (j > 0))
	decr (j);
  for (k = 0; k <= j; k++) {
	print_int (count (k));
	if (k < j)
	  print_char ('.');
  };
  update_terminal;
  if (tracing_output > 0) {
	print_char (']');
	begin_diagnostic();
	show_box (p);
	end_diagnostic (true);
  };
  /* begin expansion of Ship box |p| out */
  /* module 640 */
  /* begin expansion of Update the values of |max_h| and |max_v|; but if the page is too large, |goto done| */
  /* module 641 */
  /* Sometimes the user will generate a huge page because other error messages
   * are being ignored. Such pages are not output to the \.{dvi} file, since they
   * may confuse the printing software.
   */
  if ((height (p) > max_dimen) || (depth (p) > max_dimen)
	  || (height (p) + depth (p) + v_offset > max_dimen)
	  || (width (p) + h_offset > max_dimen)) {
	print_err ("Huge page cannot be shipped out");
	help2 ("The page just created is more than 18 feet tall or",
		   "more than 18 feet wide, so I suspect something went wrong.");
	error();
	if (tracing_output <= 0) {
	  begin_diagnostic();
	  print_nl_string("The following box has been deleted:");
	  show_box (p);
	  end_diagnostic (true);
	};
	goto DONE;
  };
  if (height (p) + depth (p) + v_offset > max_v)
	max_v = height (p) + depth (p) + v_offset;
  if (width (p) + h_offset > max_h)
	max_h = width (p) + h_offset;
  /* end expansion of Update the values of |max_h| and |max_v|; but if the page is too large, |goto done| */
  /* begin expansion of Initialize variables as |ship_out| begins */
  /* module 617 */
  dvi_h = 0;
  dvi_v = 0;
  cur_h = h_offset;
  dvi_f = null_font;
  ensure_dvi_open;
  if (total_pages == 0) {
	dvi_out (pre);
	dvi_out (id_byte); /* output the preamble */ 
	dvi_four (25400000);
	dvi_four (473628672); /* conversion ratio for sp */ 
	prepare_mag();
	dvi_four (mag); /* magnification factor is frozen */ 
	  old_setting = selector;
	  selector = new_string;
	  zprint_string(" TeX output ");
	  print_int (year);
	  print_char ('.');
	  print_two (month);
	  print_char ('.');
	  print_two (day);
	  print_char (':');
	  print_two (tex_time / 60);
	  print_two (tex_time % 60);
	  selector = old_setting;
	  dvi_out (cur_length);
	  for (s = str_start[str_ptr]; s <= pool_ptr - 1; s++)
		dvi_out (str_pool[s]);
	  pool_ptr = str_start[str_ptr]; /* flush the current string */ 
  }
  /* end expansion of Initialize variables as |ship_out| begins */
  page_loc = dvi_offset + dvi_ptr;
  dvi_out (bop);
  for (k = 0; k <= 9; k++)
	dvi_four (count (k));
  dvi_four (last_bop);
  last_bop = page_loc;
  cur_v = height (p) + v_offset;
  temp_ptr = p;
  if (type (p) == vlist_node) {
	vlist_out();
  } else {
	hlist_out();
  }
  dvi_out (eop);
  incr (total_pages);
  cur_s = -1;
 DONE:
/* end expansion of Ship box |p| out */
  if (eTeX_ex)
    /* Check for LR anomalies at the end of |ship_out| */
    check_LR_after_ship_out;
  if (tracing_output <= 0)
	print_char (']');
  dead_cycles = 0;
  update_terminal;  /* progress report */
  /* begin expansion of Flush the box from memory, showing statistics if requested */
  /* module 639 */
  if (tracing_stats > 1) {
	print_nl_string("Memory usage before: ");
	print_int (var_used);
	print_char ('&');
	print_int (dyn_used);
	print_char (';');
  };
  flush_node_list (p);
  if (tracing_stats > 1) {
	zprint_string(" after: ");
	print_int (var_used);
	print_char ('&');
	print_int (dyn_used);
	zprint_string("; still untouched: ");
	print_int (hi_mem_min - lo_mem_max - 1);
	print_ln();
  };
  /* end expansion of Flush the box from memory, showing statistics if requested */
};


/* module 593 */
/* module 596 */
/* Initially the buffer is all in one piece; we will output half of it only
 * after it first fills up.
 */
/* module 606 */
void
dvi_initialize (void) {
  total_pages = 0;
  max_v = 0;
  max_h = 0;
  max_push = 0;
  last_bop = -1;
  doing_leaders = false;
  dead_cycles = 0;
  cur_s = -1;

  half_buf = dvi_buf_size / 2;
  dvi_limit = dvi_buf_size;
  dvi_ptr = 0;
  dvi_offset = 0;
  dvi_gone = 0;

  down_ptr = null;
  right_ptr = null;
}

/* module 642 */
/* At the end of the program, we must finish things off by writing the
 * post\-amble. If |total_pages=0|, the \.{DVI} file was never opened.
 * If |total_pages>=65536|, the \.{DVI} file will lie.
 * 
 * An integer variable |k| will be declared for use by this routine.
 */
void
finish_dvi_file (void) {
  while (cur_s > -1) {
	if (cur_s > 0) {
	  dvi_out (pop);
	} else {
	  dvi_out (eop);
	  incr (total_pages);
	};
	decr (cur_s);
  };
  if (total_pages == 0) {
	print_nl_string("No pages of output.");
  } else {
	dvi_out (post); /* beginning of the postamble */
	dvi_four (last_bop);
	last_bop = dvi_offset + dvi_ptr - 5; /* |post| location */ 
	dvi_four (25400000);
	dvi_four (473628672); /* conversion ratio for sp */ 
	prepare_mag();
	dvi_four (mag); /* magnification factor */ 
	dvi_four (max_v);
	dvi_four (max_h);
	dvi_out (max_push / 256);
	dvi_out (max_push % 256);
	dvi_out ((total_pages / 256) % 256);
	dvi_out (total_pages % 256);
	/* begin expansion of Output the font definitions for all fonts that were used */
	/* module 643 */
	while (font_ptr > font_base) {
	  if (font_used[font_ptr])
		dvi_font_def (font_ptr);
	  decr (font_ptr);
	};
	/* end expansion of Output the font definitions for all fonts that were used */
	dvi_out (post_post);
	dvi_four (last_bop);
	dvi_out (id_byte);
	k = 4 + ((dvi_buf_size - dvi_ptr) % 4); /* the number of 223's */ 
	while (k > 0) {
	  dvi_out (223);
	  decr (k);
	};
	/* begin expansion of Empty the last bytes out of |dvi_buf| */
	/* module 599 */
	/* Here is how we clean out the buffer when \TeX\ is all through; |dvi_ptr|
	 * will be a multiple of~4.
	 */
	if (dvi_limit == half_buf)
	  write_dvi (half_buf, dvi_buf_size - 1);
	if (dvi_ptr > 0)
	  write_dvi (0, dvi_ptr - 1);
	/* end expansion of Empty the last bytes out of |dvi_buf| */
	print_nl_string("Output written on ");
	slow_print (output_file_name);
	zprint_string(" (");
	print_int (total_pages);
	if (total_pages != 1) {
	  zprint_string(" pages");
	} else {
	  zprint_string(" page");
	}
	zprint_string(", ");
	print_int (dvi_offset + dvi_ptr);
	zprint_string(" bytes).");
	b_close (dvi_file);
  };
}

/* module 1564 */

/* After all this preliminary shuffling, we come finally to the routines
 * that actually send out the requested data. Let's do \.{\\special} first
 * (it's easier).
 */
void 
special_out (pointer p) {
  unsigned char old_setting; /* holds print |selector| */ 
  pool_pointer k; /* index into |str_pool| */ 
  synch_h;
  synch_v;
  old_setting = selector;
  selector = new_string;
  show_token_list (link (write_tokens (p)), null, pool_size - pool_ptr);
  selector = old_setting;
  str_room (1);
  if (cur_length < 256) {
	dvi_out (xxx1);
	dvi_out (cur_length);
  } else {
	dvi_out (xxx4);
	dvi_four (cur_length);
  };
  for (k = str_start[str_ptr]; k <= pool_ptr - 1; k++)
    dvi_out (str_pool[k]);
  pool_ptr = str_start[str_ptr]; /* erase the string */ 
};


