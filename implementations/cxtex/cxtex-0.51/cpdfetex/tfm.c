
#include "types.h"
#include "c-compat.h"


#include "globals.h"


#include "mainio.h"


/* module 539 */
/* 
 * \TeX\ gets its knowledge about fonts from font metric files, also called
 * \.{TFM} files; the `\.T' in `\.{TFM}' stands for \TeX,
 * but other programs know about them too.
 * 
 * The information in a \.{TFM} file appears in a sequence of 8-bit bytes.
 * Since the number of bytes is always a multiple of 4, we could
 * also regard the file as a sequence of 32-bit words, but \TeX\ uses the
 * byte interpretation. The format of \.{TFM} files was designed by
 * Lyle Ramshaw in 1980. The intent is to convey a lot of different kinds
 * of information in a compact but useful form.
 */
FILE *tfm_file;


/* module 560 */

/* \TeX\ checks the information of a \.{TFM} file for validity as the
 * file is being read in, so that no further checks will be needed when
 * typesetting is going on. The somewhat tedious subroutine that does this
 * is called |read_font_info|. It has four parameters: the user font
 * identifier~|u|, the file name and area strings |nom| and |aire|, and the
 * ``at'' size~|s|. If |s|~is negative, it's the negative of a scale factor
 * to be applied to the design size; |s=-1000| is the normal case.
 * Otherwise |s| will be substituted for the design size; in this
 * case, |s| must be positive and less than $2048\rm\,pt$
 * (i.e., it must be less than $2^{27}$ when considered as an integer).
 * 
 * The subroutine opens and closes a global file variable called |tfm_file|.
 * It returns the value of the internal font number that was just loaded.
 * If an error is detected, an error message is issued and no font
 * information is stored; |null_font| is returned in this case.
 */


/* module 560 */
#define abort  goto  BAD_TFM

/* module 561 */
#define start_font_error_message              \
   print_err ("Font ");                       \
   sprint_cs (u);                             \
   print_char ('=');                          \
   print_file_name (nom, aire, null_string);  \
   if (s >= 0)  {                             \
     zprint_string (" at ");                  \
     print_scaled (s);                        \
     zprint_string ("pt");                    \
   } else if (s != -1000)  {                  \
     zprint_string (" scaled " );             \
     print_int (-s);                          \
   }

/* module 563b */

/* 
 * This new section optimizes \.{TFM} file reading by reading
 * all bytes in the TFM file at once. The amount of changes needed to
 * the program below is minimal, and it removes a large number of calls
 * to getc().  Nothing can go wrong, since \.{TFM} files cannot be 
 * larger than |2*15| bytes, which is small enough to declare statically.
 *
 * |cur_tfm_byte starts| at -1 so that read_sixteen can start with
 * an fget. This removes the 13 plain 'fget' source code lines
 * that were required due to false start created by PASCAL file IO.
 */
#define max_tfm_size  (32768)
#define fget cur_tfm_byte++
#define fbyte tfm_buffer[cur_tfm_byte]
#define eof_reached (cur_tfm_byte >= max_tfm_byte)

unsigned char tfm_buffer[max_tfm_size]; /* a global \.{TFM} bytes array */
halfword max_tfm_byte; /* index where |feof(tfm_file)| became true */
int cur_tfm_byte;      /* current \.{TFM} byte */

boolean
read_tfm_file (void) {
  size_t readbytes;
  cur_tfm_byte=-1;
  rewind(tfm_file); /* rewind. this 'undoes' the getc() done by open_input */
  readbytes = fread(&tfm_buffer,sizeof(unsigned char),max_tfm_size,tfm_file);
  if (feof(tfm_file) && readbytes > 0) {
	max_tfm_byte = (halfword) readbytes;
  } else {
	max_tfm_byte = 0;
	return false;
  }
  return true;
}


/* module 564 */

/* Note: A malformed \.{TFM} file might be shorter than it claims to be;
 * thus |eof(tfm_file)| might be true when |read_font_info| refers to
 * |tfm_file^| or when it says |get(tfm_file)|. If such circumstances
 * cause system error messages, you will have to defeat them somehow,
 * for example by defining |fget| to be `\ignorespaces|begin get(tfm_file);|
 * |if eof(tfm_file) then abort; end|\unskip'.
 */
/*TH defined above
 #define fget  tfmtemp   =  getc ( tfm_file )
 #define fbyte  tfmtemp
*/


#define read_sixteen( arg ) {       \
    fget ;                          \
    arg   =  fbyte ;                \
    if (  arg  > 127  )   abort ;   \
    fget ;                          \
    arg   =  arg * 256  +  fbyte ;  \
  }

#define store_four_quarters(arg) {  \
    fget;                           \
    a = fbyte; 						\
    qw.b0 = qi (a); 				\
    fget; 							\
    b = fbyte; 						\
    qw.b1 = qi (b); 				\
    fget; 							\
    c = fbyte; 						\
    qw.b2 = qi (c); 				\
    fget; 							\
    d = fbyte; 						\
    qw.b3 = qi (d); 				\
    arg = qw;						\
  }

/* module 570 */
#define check_byte_range(arg) {  if ((arg < bc) || (arg > ec))  { abort; } }

#define current_character_being_worked_on  k + bc - fmem_ptr

/* module 571 */
#define store_scaled(arg) {                                       \
  fget; 														  \
  a = fbyte; 													  \
  fget; 														  \
  b = fbyte; 													  \
  fget; 														  \
  c = fbyte; 													  \
  fget; 														  \
  d = fbyte; 													  \
  sw = (((((d * z) / 256) + (c * z)) / 256)  + (b * z)) / beta;	  \
  if (a == 0)  {												  \
   arg = sw;													  \
  } else if (a == 255)  { 										  \
	arg = sw - alpha; 											  \
  } else { 														  \
	abort;														  \
  } 															  \
}

/* module 573 */
#define check_existence(arg) {     \
    check_byte_range (arg);        \
    qw = orig_char_info (f, arg);  \
    if (! char_exists (qw)) abort ;\
  }

/* module 576 */
#define adjust(arg)  arg[f]  =  qo (arg[f])

/* module 540 */

/* The first 24 bytes (6 words) of a \.{TFM} file contain twelve 16-bit
 * integers that give the lengths of the various subsequent portions
 * of the file. These twelve integers are, in order:
 * $$\vbox{\halign{\hfil#&$\null=\null$#\hfil\cr
 * |lf|&length of the entire file, in words;\cr
 * |lh|&length of the header data, in words;\cr
 * |bc|&smallest character code in the font;\cr
 * |ec|&largest character code in the font;\cr
 * |nw|&number of words in the width table;\cr
 * |nh|&number of words in the height table;\cr
 * |nd|&number of words in the depth table;\cr
 * |ni|&number of words in the italic correction table;\cr
 * |nl|&number of words in the lig/kern table;\cr
 * |nk|&number of words in the kern table;\cr
 * |ne|&number of words in the extensible character table;\cr
 * |np|&number of font parameter words.\cr}}$$
 * They are all nonnegative and less than $2^{15}$. We must have |bc-1<=ec<=255|,
 * and
 * $$\hbox{|lf=6+lh+(ec-bc+1)+nw+nh+nd+ni+nl+nk+ne+np|.}$$
 * Note that a font may contain as many as 256 characters (if |bc=0| and |ec=255|),
 * and as few as 0 characters (if |bc=ec+1|).
 * 
 * Incidentally, when two or more 8-bit bytes are combined to form an integer of
 * 16 or more bits, the most significant bytes appear first in the file.
 * This is called BigEndian order.
 */

/* module 541 */

/* The rest of the \.{TFM} file may be regarded as a sequence of ten data
 * arrays having the informal specification
 * $$\def\arr$[#1]#2${\&{array} $[#1]$ \&{of} #2}
 * \vbox{\halign{\hfil\\{#}&$\,:\,$\arr#\hfil\cr
 * header&|[0..lhfield-1]@t\\{stuff}@>|\cr
 * char\_info&|[bc..ec]char_info_word|\cr
 * width&|[0..nw-1]fix_word|\cr
 * height&|[0..nh-1]fix_word|\cr
 * depth&|[0..nd-1]fix_word|\cr
 * italic&|[0..ni-1]fix_word|\cr
 * lig\_kern&|[0..nl-1]lig_kern_command|\cr
 * kern&|[0..nk-1]fix_word|\cr
 * exten&|[0..ne-1]extensible_recipe|\cr
 * param&|[1..np]fix_word|\cr}}$$
 * The most important data type used here is a | fix_word|, which is
 * a 32-bit representation of a binary fraction. A |fix_word| is a signed
 * quantity, with the two's complement of the entire word used to represent
 * negation. Of the 32 bits in a |fix_word|, exactly 12 are to the left of the
 * binary point; thus, the largest |fix_word| value is $2048-2^{-20}$, and
 * the smallest is $-2048$. We will see below, however, that all but two of
 * the |fix_word| values must lie between $-16$ and $+16$.
 */

/* module 542 */

/* The first data array is a block of header information, which contains
 * general facts about the font. The header must contain at least two words,
 * |header[0]| and |header[1]|, whose meaning is explained below.
 * Additional header information of use to other software routines might
 * also be included, but \TeX82 does not need to know about such details.
 * For example, 16 more words of header information are in use at the Xerox
 * Palo Alto Research Center; the first ten specify the character coding
 * scheme used (e.g., `\.{XEROX text}' or `\.{TeX math symbols}'), the next five
 * give the font identifier (e.g., `\.{HELVETICA}' or `\.{CMSY}'), and the
 * last gives the ``face byte.'' The program that converts \.{DVI} files
 * to Xerox printing format gets this information by looking at the \.{TFM}
 * file, which it needs to read anyway because of other information that
 * is not explicitly repeated in \.{DVI}~format.
 * 
 * \yskip\hang|header[0]| is a 32-bit check sum that \TeX\ will copy into
 * the \.{DVI} output file. Later on when the \.{DVI} file is printed,
 * possibly on another computer, the actual font that gets used is supposed
 * to have a check sum that agrees with the one in the \.{TFM} file used by
 * \TeX. In this way, users will be warned about potential incompatibilities.
 * (However, if the check sum is zero in either the font file or the \.{TFM}
 * file, no check is made.) The actual relation between this check sum and
 * the rest of the \.{TFM} file is not important; the check sum is simply an
 * identification number with the property that incompatible fonts almost
 * always have distinct check sums.
 * 
 * \yskip\hang|header[1]| is a |fix_word| containing the design size of
 * the font, in units of \TeX\ points. This number must be at least 1.0; it is
 * fairly arbitrary, but usually the design size is 10.0 for a ``10 point''
 * font, i.e., a font that was designed to look best at a 10-point size,
 * whatever that really means. When a \TeX\ user asks for a font
 * `\.{at} $\delta$ \.{pt}', the effect is to override the design size
 * and replace it by $\delta$, and to multiply the $x$ and~$y$ coordinates
 * of the points in the font image by a factor of $\delta$ divided by the
 * design size. {\sl All other dimensions in the\/ \.{TFM} file are
 * |fix_word|\kern-1pt\ numbers in design-size units}, with the exception of
 * |param[1]| (which denotes the slant ratio). Thus, for example, the value
 * of |param[6]|, which defines the \.{em} unit, is often the |fix_word| value
 * $2^{20}=1.0$, since many fonts have a design size equal to one em.
 * The other dimensions must be less than 16 design-size units in absolute
 * value; thus, |header[1]| and |param[1]| are the only |fix_word|
 * entries in the whole \.{TFM} file whose first byte might be something
 * besides 0 or 255.
 */

/* module 543 */

/* Next comes the |char_info| array, which contains one | char_info_word|
 * per character. Each word in this part of the file contains six fields
 * packed into four bytes as follows.
 * 
 * \yskip\hang first byte: | width_index| (8 bits)\par
 * \hang second byte: | height_index| (4 bits) times 16, plus | depth_index|
 * 
 * (4~bits)\par
 * \hang third byte: | italic_index| (6 bits) times 4, plus | tag|
 * 
 * (2~bits)\par
 * \hang fourth byte: | remainder| (8 bits)\par
 * \yskip\noindent
 * The actual width of a character is \\{width}|[width_index]|, in design-size
 * units; this is a device for compressing information, since many characters
 * have the same width. Since it is quite common for many characters
 * to have the same height, depth, or italic correction, the \.{TFM} format
 * imposes a limit of 16 different heights, 16 different depths, and
 * 64 different italic corrections.
 * 
 * The italic correction of a character has two different uses.
 * (a)~In ordinary text, the italic correction is added to the width only if
 * the \TeX\ user specifies `\.{\\/}' after the character.
 * (b)~In math formulas, the italic correction is always added to the width,
 * except with respect to the positioning of subscripts.
 * 
 * Incidentally, the relation $\\{width}[0]=\\{height}[0]=\\{depth}[0]=
 * \\{italic}[0]=0$ should always hold, so that an index of zero implies a
 * value of zero. The |width_index| should never be zero unless the
 * character does not exist in the font, since a character is valid if and
 * only if it lies between |bc| and |ec| and has a nonzero |width_index|.
 */

EXTERN unsigned char tfmtemp; /* extra temp var */

internal_font_number
read_font_info (pointer u, str_number nom, str_number aire, scaled s) {
  /* input a \.{TFM} file */
  font_index k; /* index into |font_info| */ 
  boolean file_opened; /* was |tfm_file| successfully opened? */ 
  halfword lf, lh, bc, ec, nw, nh, nd, ni, nl, nk, ne, np; /* sizes of subfiles */ 
  internal_font_number f; /* the new font's number */ 
  internal_font_number g; /* the number to return */ 
  eight_bits a, b, c, d; /* byte variables */ 
  four_quarters qw;
  scaled sw; /* accumulators */ 
  int bch_label; /* left boundary start location, or infinity */ 
  halfword bchar; /* right boundary character, or 256 */ 
  scaled z; /* the design size or the ``at'' size */ 
  int alpha;
  unsigned char beta; /* auxiliary quantities used in fixed-point multiplication */ 
  g =  null_font;
  /* begin expansion of Read and check the font data; |abort| if the \.{TFM} file is malformed; 
	 if there's no room for this font, say so and |goto done|; 
	 otherwise |incr(font_ptr)| and |goto done| */
  /* module 562 */
  /* begin expansion of Open |tfm_file| for input */
  /* module 563 */
  file_opened = false;
  /* |kpse_find_file| will append the |".tfm"|, and avoid searching the disk before 
	 the font alias files as well. */
  pack_file_name (nom, aire,null_string);
  if (!tfm_b_open_in (tfm_file))
    abort;
  file_opened = true;
  /* module 563b */
  if (!read_tfm_file())
  	abort;
  /* end expansion of Open |tfm_file| for input */
  /* begin expansion of Read the {\.{TFM}} size fields */
  /* module 565 */
  read_sixteen (lf);
  read_sixteen (lh);
  read_sixteen (bc);
  read_sixteen (ec);
  if ((bc > ec + 1) || (ec > 255)) {
	do_something;
	abort;
  }
  if (bc > 255)	{	/* |bc=256| and |ec=255| */
	bc = 1;
	ec = 0;
  };
  read_sixteen (nw);
  read_sixteen (nh);
  read_sixteen (nd);
  read_sixteen (ni);
  read_sixteen (nl);
  read_sixteen (nk);
  read_sixteen (ne);
  read_sixteen (np);
  if (lf != 6 + lh + (ec - bc + 1) + nw + nh + nd + ni + nl + nk + ne + np)
	abort;
  /* end expansion of Read the {\.{TFM}} size fields */
  /* begin expansion of Use size fields to allocate font information */
  /* module 566 */
  /* The preliminary settings of the index-offset variables |char_base|,
   * |width_base|, |lig_kern_base|, |kern_base|, and |exten_base| will be
   * corrected later by subtracting |min_quarterword| from them; and we will
   * subtract 1 from |param_base| too. It's best to forget about such anomalies
   * until later.
   */
  lf = lf - 6 - lh; /* |lf| words should be loaded into |font_info| */ 
  if (np < 7)
    lf = lf + 7 - np; /* at least seven parameters will appear */ 
  if ((font_ptr == font_max) || (fmem_ptr + lf > font_mem_size)) {
	/* begin expansion of Apologize for not loading the font, |goto done| */
	/* module 567 */
	start_font_error_message;
	zprint_string(" not loaded: Not enough room left");
	help4("I'm afraid I won't be able to make use of this font,",
		  "because my memory for character-size data is too small.",
		  "If you're really stuck, ask a wizard to enlarge me.",
		  "Or maybe try `I\\font<same font id>=<name of loaded font>'.");
	error();
	goto DONE;
	/* end expansion of Apologize for not loading the font, |goto done| */
  };
  f = font_ptr + 1;
  char_base[f] = fmem_ptr - bc;
  width_base[f] = char_base[f] + ec + 1;
  height_base[f] = width_base[f] + nw;
  depth_base[f] = height_base[f] + nh;
  italic_base[f] = depth_base[f] + nd;
  lig_kern_base[f] = italic_base[f] + ni;
  kern_base[f] = lig_kern_base[f] + nl - kern_base_offset;
  exten_base[f] = kern_base[f] + kern_base_offset + nk;
  param_base[f] = exten_base[f] + ne;
  /* end expansion of Use size fields to allocate font information */
  /* begin expansion of Read the {\.{TFM}} header */
  /* module 568 */
  /* Only the first two words of the header are needed by \TeX82. */
  if (lh < 2)
	abort;
  store_four_quarters (font_check[f]);
  read_sixteen (z); /* this rejects a negative design size */ 
  fget;
  z = z * 256 + fbyte;
  fget;
  z = (z * 16) + (fbyte / 16);
  if (z < unity)
	abort;
  while (lh > 2) {
	fget;
	fget;
	fget;
	fget;
	decr (lh);
	/* ignore the rest of the header */ 
  };
  font_dsize[f] = z;
  if (s != -1000) {
	if (s >= 0) {
	  z = s;
	} else {
	  z = xn_over_d (z, -s, 1000); 
	} 
  }
  font_size[f] = z;
  /* end expansion of Read the {\.{TFM}} header */
  /* begin expansion of Read character data */
  /* module 569 */
  for (k = fmem_ptr; k <= width_base[f] - 1; k++) {
	store_four_quarters (font_info[k].qqqq);
	if ((a >= nw) || (b / 16 >= nh) || (b % 16 >= nd) || (c / 4 >= ni)) {
	  do_something;
	  abort;
	}
	switch (c % 4) {
	case lig_tag:
	  if (d >= nl) {
	    abort;
	  }
	  do_something;
	  break;
	case ext_tag:
	  if (d >= ne) {
	    abort;
	  }
	  do_something;
	  break;
	case list_tag:
	  /* begin expansion of Check for charlist cycle */
	  /* module 570 */
	  /* We want to make sure that there is no cycle of characters linked together
	   * by |list_tag| entries, since such a cycle would get \TeX\ into an endless
	   * loop. If such a cycle exists, the routine here detects it when processing
	   * the largest character code in the cycle.
	   */
	  check_byte_range (d);
	  while (d < current_character_being_worked_on) {
		qw = orig_char_info (f, d);
		/* N.B.: not |qi(d)|, since |char_base[f]| hasn't been adjusted yet */
		if (char_tag (qw) != list_tag)
		  goto NOT_FOUND;
		d = qo (rem_byte (qw)); /* next character on the list */ 
	  };
	  if (d == current_character_being_worked_on) {
		abort;	/* yes, there's a cycle */ 
	  }
	NOT_FOUND:
	  do_something;
	  /* end expansion of Check for charlist cycle */
	  break;
	default:
	  do_nothing; /* |no_tag| */
	};
  };
  /* end expansion of Read character data */
  /* begin expansion of Read box dimensions */
  /* module 571 */
  /* A |fix_word| whose four bytes are $(a,b,c,d)$ from left to right represents
   * the number
   * $$x=\left\{\vcenter{\halign{$#$,\hfil\qquad&if $#$\hfil\cr
   * b\cdot2^{-4}+c\cdot2^{-12}+d\cdot2^{-20}&a=0;\cr
   * -16+b\cdot2^{-4}+c\cdot2^{-12}+d\cdot2^{-20}&a=255.\cr}}\right.$$
   * (No other choices of |a| are allowed, since the magnitude of a number in
   * design-size units must be less than 16.) We want to multiply this
   * quantity by the integer~|z|, which is known to be less than $2^{27}$.
   * If $|z|<2^{23}$, the individual multiplications $b\cdot z$,
   * $c\cdot z$, $d\cdot z$ cannot overflow; otherwise we will divide |z| by 2,
   * 4, 8, or 16, to obtain a multiplier less than $2^{23}$, and we can
   * compensate for this later. If |z| has thereby been replaced by
   * $|z|^\prime=|z|/2^e$, let $\beta=2^{4-e}$; we shall compute
   * $$\lfloor(b+c\cdot2^{-8}+d\cdot2^{-16})\,z^\prime/\beta\rfloor$$
   * if $a=0$, or the same quantity minus $\alpha=2^{4+e}z^\prime$ if $a=255$.
   * This calculation must be done exactly, in order to guarantee portability
   * of \TeX\ between computers.
   */
  /* begin expansion of Replace |z| by $|z|^\prime$ and compute $\alpha,\beta$ */
  /* module 572 */
  alpha = 16;
  while (z >= 8388608) {
	z = z / 2;
	alpha = alpha + alpha;
  };
  beta = 256 / alpha;
  alpha = alpha * z;
  /* end expansion of Replace |z| by $|z|^\prime$ and compute $\alpha,\beta$ */
  for (k = width_base[f]; k <= lig_kern_base[f] - 1; k++)
	store_scaled (font_info[k].sc);
  if (font_info[width_base[f]].sc != 0)
	abort; /* \\{width}[0] must be zero */ 
  if (font_info[height_base[f]].sc != 0)
	abort; /* \\{height}[0] must be zero */ 
  if (font_info[depth_base[f]].sc != 0)
	abort; /* \\{depth}[0] must be zero */ 
  if (font_info[italic_base[f]].sc != 0)
	abort; /* \\{italic}[0] must be zero */ 
  /* end expansion of Read box dimensions */
  /* begin expansion of Read ligature/kern program */
  /* module 573 */
  bch_label = 32767;
  bchar = 256;
  if (nl > 0) {
	for (k = lig_kern_base[f]; k <= kern_base[f] + kern_base_offset - 1; k++) {
	  store_four_quarters (font_info[k].qqqq);
	  if (a > 128) {
		if (256 * c + d >= nl)
		  abort;
		if (a == 255)
		  if (k == lig_kern_base[f])
			bchar = b;
	  } else {
		if (b != bchar)
		  check_existence (b);
		if (c < 128) {
		  check_existence (d);	/* check ligature */
		} else if (256 * (c - 128) + d >= nk) {
		  abort; /* check kern */ 
		};
		if (a < 128)
		  if (k - lig_kern_base[f] + a + 1 >= nl)
			abort;
	  };
	};
	if (a == 255)
	  bch_label = 256 * c + d;
  };
  for (k = kern_base[f] + kern_base_offset; k <= exten_base[f] - 1; k++)
    store_scaled (font_info[k].sc);
  /* end expansion of Read ligature/kern program */
  /* begin expansion of Read extensible character recipes */
  /* module 574 */
  for (k = exten_base[f]; k <= param_base[f] - 1; k++) {
	store_four_quarters (font_info[k].qqqq);
	if (a != 0)
	  check_existence (a);
	if (b != 0)
	  check_existence (b);
	if (c != 0)
	  check_existence (c);
	check_existence (d);
  };
  /* end expansion of Read extensible character recipes */
  /* begin expansion of Read font parameters */
  /* module 575 */
  /* We check to see that the \.{TFM} file doesn't end prematurely; but
   * no error message is given for files having more than |lf| words.
   */
  for (k=1; k <= np; k++)
	if (k == 1) {		/* the |slant| parameter is a pure number */
	  fget;
	  sw = fbyte;
	  if (sw > 127)
		sw = sw - 256;
	  fget;
	  sw = sw * 256 + fbyte;
	  fget;
	  sw = sw * 256 + fbyte;
	  fget;
	  font_info[param_base[f]].sc = (sw * 16) + (fbyte / 16);
	} else {
	  store_scaled (font_info[param_base[f] + k - 1].sc);
	}
  if (eof_reached)
	abort;
  for (k = np + 1; k <= 7; k++)
	font_info[param_base[f] + k - 1].sc = 0;
  /* end expansion of Read font parameters */
  /* begin expansion of Make final adjustments and |goto done| */
  /* module 576 */
  /* Now to wrap it up, we have checked all the necessary things about the \.{TFM}
   * file, and all we need to do is put the finishing touches on the data for
   * the new font.
   */
  if (np >= 7) {
    font_params[f] = np;
  } else {
    font_params[f] = 7;
  }
  hyphen_char[f] = default_hyphen_char;
  skew_char[f] = default_skew_char;
  if (bch_label < nl) {
    bchar_label[f] = bch_label + lig_kern_base[f];
  } else {
    bchar_label[f] = non_address;
  }
  font_bchar[f] = qi (bchar);
  font_false_bchar[f] = qi (bchar);
  if (bchar <= ec)
    if (bchar >= bc) {
	  qw = orig_char_info (f, bchar);
	  /* N.B.: not |qi(bchar)| */ 
	  if (char_exists (qw))
		font_false_bchar[f] = non_char;
	};
  font_name[f] = nom;
  font_area[f] = aire;
  font_bc[f] = bc;
  font_ec[f] = ec;
  font_glue[f] = null;
  adjust (char_base);
  adjust (width_base);
  adjust (lig_kern_base);
  adjust (kern_base);
  adjust (exten_base);
  decr (param_base[f]);
  fmem_ptr = fmem_ptr + lf;
  font_ptr = f;
  g = f;
  goto DONE;
  /* end expansion of Make final adjustments and |goto done| */
  /* end expansion of Read and check the font data; |abort| if the \.{TFM} file is ma...*/
 BAD_TFM:
  /* begin expansion of Report that the font won't be loaded */
  /* module 561 */
  /* There are programs called \.{TFtoPL} and \.{PLtoTF} that convert
   * between the \.{TFM} format and a symbolic property-list format
   * that can be easily edited. These programs contain extensive
   * diagnostic information, so \TeX\ does not have to bother giving
   * precise details about why it rejects a particular \.{TFM} file.
   */
  start_font_error_message;
  if (file_opened) {
    zprint_string(" not loadable: Bad metric (TFM) file");
  } else {
    zprint_string(" not loadable: Metric (TFM) file not found");
  }
  help5 ("I wasn't able to read the size data for this font,",
		"so I will ignore the font specification.",
        "[Wizards can fix TFM files using TFtoPL/PLtoTF.]",
		"You might try inserting a different font spec;",
        "e.g., type `I\\font<same font id>=<substitute font name>'.");
  error();
  /* end expansion of Report that the font won't be loaded */
 DONE:
  if (file_opened)
    b_close (tfm_file);
  return g;
}

