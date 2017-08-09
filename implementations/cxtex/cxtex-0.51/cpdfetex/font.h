
/* module 134 */

/* A | char_node|, which represents a single character, is the most important
 * kind of node because it accounts for the vast majority of all boxes.
 * Special precautions are therefore taken to ensure that a |char_node| does
 * not take up much memory space. Every such node is one word long, and in fact
 * it is identifiable by this property, since other kinds of nodes have at least
 * two words, and they appear in |mem| locations less than |hi_mem_min|.
 * This makes it possible to omit the |type| field in a |char_node|, leaving
 * us room for two bytes that identify a |font| and a |character| within
 * that font.
 * 
 * Note that the format of a |char_node| allows for up to 256 different
 * fonts and up to 256 characters per font; but most implementations will
 * probably limit the total number of fonts to fewer than 75 per job,
 * and most fonts will stick to characters whose codes are
 * less than 128 (since higher codes
 * are more difficult to access on most keyboards).
 * 
 * Extensions of \TeX\ intended for oriental languages will need even more
 * than $256\times256$ possible characters, when we consider different sizes
 * 
 * and styles of type. It is suggested that Chinese and Japanese fonts be
 * handled by representing such characters in two consecutive |char_node|
 * entries: The first of these has |font=font_base|, and its |link| points
 * to the second;
 * the second identifies the font and the character dimensions.
 * The saving feature about oriental characters is that most of them have
 * the same box dimensions. The |character| field of the first |char_node|
 * is a ``\\{charext}'' that distinguishes between graphic symbols whose
 * dimensions are identical for typesetting purposes. (See the \MF\ manual.)
 * Such an extension of \TeX\ would not be difficult; further details are
 * left to the reader.
 * 
 * In order to make sure that the |character| code fits in a quarterword,
 * \TeX\ adds the quantity |min_quarterword| to the actual code.
 * 
 * Character nodes appear only in horizontal lists, never in vertical lists.
 */
#define is_char_node( arg ) ( arg  >=  hi_mem_min )
#define font  type
#define character  subtype

/* module 544 */

/* The |tag| field in a |char_info_word| has four values that explain how to
 * interpret the |remainder| field.
 * 
 * \yskip\hangg|tag=0| (|no_tag|) means that |remainder| is unused.\par
 * \hangg|tag=1| (|lig_tag|) means that this character has a ligature/kerning
 * program starting at position |remainder| in the |lig_kern| array.\par
 * \hangg|tag=2| (|list_tag|) means that this character is part of a chain of
 * characters of ascending sizes, and not the largest in the chain. The
 * |remainder| field gives the character code of the next larger character.\par
 * \hangg|tag=3| (|ext_tag|) means that this character code represents an
 * extensible character, i.e., a character that is built up of smaller pieces
 * so that it can be made arbitrarily large. The pieces are specified in
 * | exten[remainder]|.\par
 * \yskip\noindent
 * Characters with |tag=2| and |tag=3| are treated as characters with |tag=0|
 * unless they are used in special circumstances in math formulas. For example,
 * the \.{\\sum} operation looks for a |list_tag|, and the \.{\\left}
 * operation looks for both |list_tag| and |ext_tag|.
 */
#define no_tag 0
#define lig_tag 1
#define list_tag 2
#define ext_tag 3

/* module 545 */

/* The |lig_kern| array contains instructions in a simple programming language
 * that explains what to do for special letter pairs. Each word in this array is a
 * | lig_kern_command| of four bytes.
 * 
 * \yskip\hang first byte: |skip_byte|, indicates that this is the final program
 * 
 * step if the byte is 128 or more, otherwise the next step is obtained by
 * 
 * skipping this number of intervening steps.\par
 * \hang second byte: |next_char|, ``if |next_char| follows the current character,
 * 
 * then perform the operation and stop, otherwise continue.''\par
 * \hang third byte: |op_byte|, indicates a ligature step if less than~128,
 * 
 * a kern step otherwise.\par
 * \hang fourth byte: |remainder|.\par
 * \yskip\noindent
 * In a kern step, an
 * additional space equal to |kern[256*(op_byte-128)+remainder]| is inserted
 * between the current character and |next_char|. This amount is
 * often negative, so that the characters are brought closer together
 * by kerning; but it might be positive.
 * 
 * There are eight kinds of ligature steps, having |op_byte| codes $4a+2b+c$ where
 * $0\le a\le b+c$ and $0\le b,c\le1$. The character whose code is
 * |remainder| is inserted between the current character and |next_char|;
 * then the current character is deleted if $b=0$, and |next_char| is
 * deleted if $c=0$; then we pass over $a$~characters to reach the next
 * current character (which may have a ligature/kerning program of its own).
 * 
 * If the very first instruction of the |lig_kern| array has |skip_byte=255|,
 * the |next_char| byte is the so-called right boundary character of this font;
 * the value of |next_char| need not lie between |bc| and~|ec|.
 * If the very last instruction of the |lig_kern| array has |skip_byte=255|,
 * there is a special ligature/kerning program for a left boundary character,
 * beginning at location |256*op_byte+remainder|.
 * The interpretation is that \TeX\ puts implicit boundary characters
 * before and after each consecutive string of characters from the same font.
 * These implicit characters do not appear in the output, but they can affect
 * ligatures and kerning.
 * 
 * If the very first instruction of a character's |lig_kern| program has
 * |skip_byte>128|, the program actually begins in location
 * |256*op_byte+remainder|. This feature allows access to large |lig_kern|
 * arrays, because the first instruction must otherwise
 * appear in a location |<=255|.
 * 
 * Any instruction with |skip_byte>128| in the |lig_kern| array must satisfy
 * the condition
 * $$\hbox{|256*op_byte+remainder<nl|.}$$
 * If such an instruction is encountered during
 * normal program execution, it denotes an unconditional halt; no ligature
 * or kerning command is performed.
 */
#define stop_flag  qi (128 )
#define kern_flag  qi (128 )
#define skip_byte( arg )  arg . b0
#define next_char( arg )  arg . b1
#define op_byte( arg )  arg . b2
#define rem_byte( arg )  arg . b3

/* module 546 */

/* Extensible characters are specified by an | extensible_recipe|, which
 * consists of four bytes called | top|, | mid|, | bot|, and | rep| (in this
 * order). These bytes are the character codes of individual pieces used to
 * build up a large symbol. If |top|, |mid|, or |bot| are zero, they are not
 * present in the built-up result. For example, an extensible vertical line is
 * like an extensible bracket, except that the top and bottom pieces are missing.
 * 
 * Let $T$, $M$, $B$, and $R$ denote the respective pieces, or an empty box
 * if the piece isn't present. Then the extensible characters have the form
 * $TR^kMR^kB$ from top to bottom, for some |k>=0|, unless $M$ is absent;
 * in the latter case we can have $TR^kB$ for both even and odd values of~|k|.
 * The width of the extensible character is the width of $R$; and the
 * height-plus-depth is the sum of the individual height-plus-depths of the
 * components used, since the pieces are butted together in a vertical list.
 */
#define ext_top( arg )  arg . b0
#define ext_mid( arg )  arg . b1
#define ext_bot( arg )  arg . b2
#define ext_rep( arg )  arg . b3

/* module 547 */

/* The final portion of a \.{TFM} file is the |param| array, which is another
 * sequence of |fix_word| values.
 * 
 * \yskip\hang|param[1]=slant| is the amount of italic slant, which is used
 * to help position accents. For example, |slant=.25| means that when you go
 * up one unit, you also go .25 units to the right. The |slant| is a pure
 * number; it's the only |fix_word| other than the design size itself that is
 * not scaled by the design size.
 * 
 * \hang|param[2]=space| is the normal spacing between words in text.
 * Note that character |" "| in the font need not have anything to do with
 * blank spaces.
 * 
 * \hang|param[3]=space_stretch| is the amount of glue stretching between words.
 * 
 * \hang|param[4]=space_shrink| is the amount of glue shrinking between words.
 * 
 * \hang|param[5]=x_height| is the size of one ex in the font; it is also
 * the height of letters for which accents don't have to be raised or lowered.
 * 
 * \hang|param[6]=quad| is the size of one em in the font.
 * 
 * \hang|param[7]=extra_space| is the amount added to |param[2]| at the
 * ends of sentences.
 * 
 * \yskip\noindent
 * If fewer than seven parameters are present, \TeX\ sets the missing parameters
 * to zero. Fonts used for math symbols are required to have
 * additional parameter information, which is explained later.
 */
#define slant_code 1
#define space_code 2
#define space_stretch_code 3
#define space_shrink_code 4
#define x_height_code 5
#define quad_code 6
#define extra_space_code 7

/* module 549 */
#define non_char  qi (256 )
#define non_address 0

/* module 554 */

/* Of course we want to define macros that suppress the detail of how font
 * information is actually packed, so that we don't have to write things like
 * $$\hbox{|font_info[width_base[f]+font_info[char_base[f]+c].qqqq.b0].sc|}$$
 * too often. The \.{WEB} definitions here make |char_info(f,c)| the
 * |four_quarters| word of font information corresponding to character
 * |c| of font |f|. If |q| is such a word, |char_width(f,q)| will be
 * the character's width; hence the long formula above is at least
 * abbreviated to
 * $$\hbox{|char_width(f,char_info(f,c))|.}$$
 * Usually, of course, we will fetch |q| first and look at several of its
 * fields at the same time.
 * 
 * The italic correction of a character will be denoted by
 * |char_italic(f,q)|, so it is analogous to |char_width|. But we will get
 * at the height and depth in a slightly different way, since we usually want
 * to compute both height and depth if we want either one. The value of
 * |height_depth(q)| will be the 8-bit quantity
 * $$b=|height_index|\times16+|depth_index|,$$ and if |b| is such a byte we
 * will write |char_height(f,b)| and |char_depth(f,b)| for the height and
 * depth of the character |c| for which |q=char_info(f,c)|. Got that?
 * 
 * The tag field will be called |char_tag(q)|; the remainder byte will be
 * called |rem_byte(q)|, using a macro that we have already defined above.
 * 
 * Access to a character's |width|, |height|, |depth|, and |tag| fields is
 * part of \TeX's inner loop, so we want these macros to produce code that is
 * as fast as possible under the circumstances.
 * 
 * ML\TeX{} will assume that a character |c| exists iff either exists in
 * the current font or a character substitution definition for this
 * character was defined using \.{\\charsubdef}. To avoid the
 * distinction between these two cases, ML\TeX{} introduces the notion
 * ``effective character'' of an input character |c|. If |c| exists in
 * the current font, the effective character of |c| is the character |c|
 * itself. If it doesn't exist but a character substitution is defined,
 * the effective character of |c| is the base character defined in the
 * character substitution. If there is an effective character for a
 * non-existing character |c|, the ``virtual character'' |c| will get
 * appended to the horizontal lists.
 * 
 * The effective character is used within |char_info| to access
 * appropriate character descriptions in the font. For example, when
 * calculating the width of a box, ML\TeX{} will use the metrics of the
 * effective characters. For the case of a substitution, ML\TeX{} uses
 * the metrics of the base character, ignoring the metrics of the accent
 * character.
 * 
 * If character substitutions are changed, it will be possible that a
 * character |c| neither exists in a font nor there is a valid character
 * substitution for |c|. To handle these cases |effective_char| should
 * be called with its first argument set to |true| to ensure that it
 * will still return an existing character in the font. If neither |c|
 * nor the substituted base character in the current character
 * substitution exists, |effective_char| will output a warning and
 * return the character |font_bc[f]| (which is incorrect, but can not be
 * changed within the current framework).
 * 
 * Sometimes character substitutions are unwanted, therefore the
 * original definition of |char_info| can be used using the macro
 * |orig_char_info|. Operations in which character substitutions should
 * be avoided are, for example, loading a new font and checking the font
 * metric information in this font, and character accesses in math mode.
 */

#define char_list_exists(a)  (char_sub_code(a) >  hi (0))

#define char_list_accent(a)  (ho(char_sub_code(a)) / 256)
#define char_list_char(a)    (ho(char_sub_code(a)) % 256)

#define char_info(a,b)       font_info [char_base [a] + effective_char (true, a, b)].qqqq

#define orig_char_info(a,b)  font_info [char_base[a] + b].qqqq

#define char_width(a,b)      font_info [width_base [a] +  b. b0].sc

#define char_exists( arg )   (arg.b0 > min_quarterword)

#define char_italic(a,b)     font_info [italic_base [a] + (qo(b.b2)) / 4].sc

#define height_depth( arg )  qo (arg.b1)

#define char_height(a,b)     font_info [height_base[a] + (b) / 16].sc
#define char_depth(a,b)      font_info [depth_base [a] + (b) % 16].sc

#define char_tag( arg )      ((qo(arg.b2)) % 4)

/* module 557 */

/* Here are some macros that help process ligatures and kerns.
 * We write |char_kern(f,j)| to find the amount of kerning specified by
 * kerning command~|j| in font~|f|. If |j| is the |char_info| for a character
 * with a ligature/kern program, the first instruction of that program is either
 * |i=font_info[lig_kern_start(f,j)]| or |font_info[lig_kern_restart(f,i)]|,
 * depending on whether or not |skip_byte(i)<=stop_flag|.
 * 
 * The constant |kern_base_offset| should be simplified, for \PASCAL\ compilers
 * that do not do local optimization.
 */

#define char_kern(a,b)         font_info[kern_base[a]+256*op_byte(b) + rem_byte(b)].sc
#define kern_base_offset       256 * (128 + min_quarterword)
#define lig_kern_start(a,b)    lig_kern_base [a] + rem_byte(b)
#define lig_kern_restart(a,b)  lig_kern_base [a] + 256 * op_byte(b) + rem_byte(b) + 32768 - kern_base_offset

/* module 558 */
#define param(a,b)        font_info [a + param_base [b]].sc
#define slant(a)          param ( slant_code,a)
#define space(a)          param ( space_code,a)
#define space_stretch(a)  param ( space_stretch_code,a)
#define space_shrink(a)   param ( space_shrink_code,a)
#define x_height(a)       param ( x_height_code,a)
#define quad(a)           param ( quad_code,a)
#define extra_space(a)    param ( extra_space_code,a)



/* module 549 */
extern fmemory_word *font_info; /* the big collection of font data */ 
extern font_index fmem_ptr; /* first unused word of |font_info| */
extern internal_font_number font_ptr; /* largest internal font number in use */
extern four_quarters *font_check; /* check sum */
extern scaled *font_size; /* ``at'' size */
extern scaled *font_dsize; /* ``design'' size */
extern font_index *font_params; /* how many font parameters are present */
extern str_number *font_name; /* name of the font */
extern str_number *font_area; /* area of the font */
extern eight_bits *font_bc; /* beginning (smallest) character code */
extern eight_bits *font_ec; /* ending (largest) character code */
extern pointer *font_glue; /* glue specification for interword space, 
					   |null| if not allocated */
extern boolean  *font_used; /* has a character from this font actually 
						appeared in the output? */
extern integer *hyphen_char; /* current \.{\\hyphenchar} values */
extern integer *skew_char; /* current \.{\\skewchar} values */
extern font_index *bchar_label; /* start of |lig_kern| program for left boundary 
							character, |non_address| if there is none */
extern nine_bits *font_bchar; /* right boundary character, |non_char| if there 
						  is none */
extern nine_bits  *font_false_bchar; /* |font_bchar| if it doesn't exist in the 
								 font, otherwise |non_char| */

/* module 550 */
extern integer *char_base; /* base addresses for |char_info| */
extern integer *width_base; /* base addresses for widths */
extern integer *height_base; /* base addresses for heights */
extern integer *depth_base; /* base addresses for depths */
extern integer *italic_base; /* base addresses for italic corrections */
extern integer *lig_kern_base; /* base addresses for ligature/kerning programs */
extern integer *kern_base; /* base addresses for kerns */
extern integer *exten_base; /* base addresses for extensible recipes */
extern integer *param_base; /* base addresses for font parameters */

/* module 555 */
extern four_quarters null_character; /* nonexistent character information */

EXTERN void scan_font_ident (void);
EXTERN void find_font_dimen (boolean writing);


EXTERN void char_warning (internal_font_number f, eight_bits c);
EXTERN pointer new_character (internal_font_number f, eight_bits c);

EXTERN void font_initialize (void);
EXTERN void font_xmalloc (integer);
EXTERN void font_initialize_init (void);

EXTERN void append_italic_correction (void);
EXTERN void make_accent (void);
EXTERN void print_font_and_char (int p);


EXTERN internal_font_number get_null_font (void);
EXTERN scaled get_x_height (internal_font_number f);
EXTERN scaled get_char_width (internal_font_number f, eight_bits c);
EXTERN scaled get_char_height (internal_font_number f, eight_bits c);
EXTERN scaled get_char_depth (internal_font_number f, eight_bits c);
EXTERN scaled get_quad (internal_font_number f);
EXTERN scaled get_slant (internal_font_number f);
EXTERN internal_font_number new_null_font (void);
EXTERN internal_font_number get_tfm_num (str_number s);

EXTERN void adv_char_width (internal_font_number f, scaled w);


/* module 267 */

/* We will deal with the other primitives later, at some point in the program
 * where their |eq_type| and |equiv| values are more meaningful. For example,
 * the primitives for math mode will be loaded when we consider the routines
 * that deal with formulas. It is easy to find where each particular
 * primitive was treated by looking in the index at the end; for example, the
 * section where |"radical"| entered |eqtb| is listed under `\.{\\radical}
 * primitive'. (Primitives consisting of a single nonalphabetic character,
 * 
 * like `\.{\\/}', are listed under `Single-character primitives'.)
 * 
 * Meanwhile, this is a convenient place to catch up on something we were unable
 * to do before the hash table was defined:
 */

#define font_id_text(arg)  text ( font_id_base  +  arg )

#define print_font_ident(pp) {                          \
    zprint ('/');                                       \
    zprint (font_name[font (pp)]);                      \
    if (font_size[font (pp)] != font_dsize[font (pp)]) {\
	  zprint ('@');                                     \
	  print_scaled (font_size[font (pp)]);              \
	  zprint_string ("pt");                             \
    };                                                  \
    zprint ('/');                                       \
  }

/* module 1187 */

/* Having |font_glue| allocated for each text font saves both time and memory.
 * If any of the three spacing parameters are subsequently changed by the
 * use of \.{\\fontdimen}, the |find_font_dimen| procedure deallocates the
 * |font_glue| specification allocated here.
 */
/* that's |space(cur_font)| and |space_stretch(cur_font)| and |space_shrink(cur_font)| */
/* Find the glue specification, |main_p|, for text spaces in the current font */
#define find_glue_spec		  {                    \
    main_p = font_glue[cur_font];                  \
    if (main_p == null) {                          \
	  main_p = new_spec (zero_glue);               \
	  main_k = param_base[cur_font] + space_code;  \
	  width (main_p) = font_info[main_k].sc;       \
      stretch (main_p) = font_info[main_k + 1].sc; \
      shrink (main_p) = font_info[main_k + 2].sc;  \
	  font_glue[cur_font] = main_p;                \
    };                                             \
  }


/* module 794 */
EXTERN pointer  mimic_char_node (pointer pp) ;
