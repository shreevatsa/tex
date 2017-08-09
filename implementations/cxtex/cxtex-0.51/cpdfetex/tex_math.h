
/* module 147 */
#define math_node 9
#define before 0
#define after 1
#define M_code 2
#define begin_M_code  (M_code  +  before)
#define end_M_code  (M_code  +  after)
#define L_code 4
#define begin_L_code  (L_code  +  begin_M_code)
#define end_L_code  (L_code  +  end_M_code)
#define R_code  (L_code  +  L_code)
#define begin_R_code  (R_code  +  begin_M_code)
#define end_R_code  (R_code  +  end_M_code)
#define end_LR( arg )  odd ( subtype ( arg ))
#define end_LR_type( arg ) ( L_code  * ( subtype ( arg )  /  L_code ) +  end_M_code )
#define begin_LR_type( arg ) ( arg  -  after  +  before )

EXTERN pointer new_math (scaled w, small_number s);


/* module 825 */

/* Each noad is four or more words long. The first word contains the |type|
 * and |subtype| and |link| fields that are already so familiar to us; the
 * second, third, and fourth words are called the noad's |nucleus|, |subscr|,
 * and |supscr| fields.
 * 
 * Consider, for example, the simple formula `\.{\$x\^2\$}', which would be
 * parsed into an mlist containing a single element called an |ord_noad|.
 * The |nucleus| of this noad is a representation of `\.x', the |subscr| is
 * empty, and the |supscr| is a representation of `\.2'.
 * 
 * The |nucleus|, |subscr|, and |supscr| fields are further broken into
 * subfields. If |p| points to a noad, and if |q| is one of its principal
 * fields (e.g., |q=subscr(p)|), there are several possibilities for the
 * subfields, depending on the |math_type| of |q|.
 * 
 * \yskip\hang|math_type(q)=math_char| means that |fam(q)| refers to one of
 * the sixteen font families, and |character(q)| is the number of a character
 * within a font of that family, as in a character node.
 * 
 * \yskip\hang|math_type(q)=math_text_char| is similar, but the character is
 * unsubscripted and unsuperscripted and it is followed immediately by another
 * character from the same font. (This |math_type| setting appears only
 * briefly during the processing; it is used to suppress unwanted italic
 * corrections.)
 * 
 * \yskip\hang|math_type(q)=empty| indicates a field with no value (the
 * corresponding attribute of noad |p| is not present).
 * 
 * \yskip\hang|math_type(q)=sub_box| means that |info(q)| points to a box
 * node (either an |hlist_node| or a |vlist_node|) that should be used as the
 * value of the field. The |shift_amount| in the subsidiary box node is the
 * amount by which that box will be shifted downward.
 * 
 * \yskip\hang|math_type(q)=sub_mlist| means that |info(q)| points to
 * an mlist; the mlist must be converted to an hlist in order to obtain
 * the value of this field.
 * 
 * \yskip\noindent In the latter case, we might have |info(q)=null|. This
 * is not the same as |math_type(q)=empty|; for example, `\.{\$P\_\{\}\$}'
 * and `\.{\$P\$}' produce different results (the former will not have the
 * ``italic correction'' added to the width of |P|, but the ``script skip''
 * will be added).
 * 
 * The definitions of subfields given here are evidently wasteful of space,
 * since a halfword is being used for the |math_type| although only three
 * bits would be needed. However, there are hardly ever many noads present at
 * once, since they are soon converted to nodes that take up even more space,
 * so we can afford to represent them in whatever way simplifies the
 * programming.
 */
#define noad_size 4
#define nucleus( arg )  (arg  + 1)
#define supscr( arg )  (arg  + 2)
#define subscr( arg )  (arg  + 3)
#define math_type  link
#define fam  font
#define math_char 1
#define sub_box 2
#define sub_mlist 3
#define math_text_char 4

/* module 826 */

/* Each portion of a formula is classified as Ord, Op, Bin, Rel, Ope,
 * Clo, Pun, or Inn, for purposes of spacing and line breaking. An
 * |ord_noad|, |op_noad|, |bin_noad|, |rel_noad|, |open_noad|, |close_noad|,
 * |punct_noad|, or |inner_noad| is used to represent portions of the various
 * types. For example, an `\.=' sign in a formula leads to the creation of a
 * |rel_noad| whose |nucleus| field is a representation of an equals sign
 * (usually |fam=0|, |character= oct(75)|). A formula preceded by \.{\\mathrel}
 * also results in a |rel_noad|. When a |rel_noad| is followed by an
 * |op_noad|, say, and possibly separated by one or more ordinary nodes (not
 * noads), \TeX\ will insert a penalty node (with the current |rel_penalty|)
 * just after the formula that corresponds to the |rel_noad|, unless there
 * already was a penalty immediately following; and a ``thick space'' will be
 * inserted just before the formula that corresponds to the |op_noad|.
 * 
 * A noad of type |ord_noad|, |op_noad|, \dots, |inner_noad| usually
 * has a |subtype=normal|. The only exception is that an |op_noad| might
 * have |subtype=limits| or |no_limits|, if the normal positioning of
 * limits has been overridden for this operator.
 */
#define ord_noad  (unset_node  + 3)
#define op_noad  (ord_noad  + 1)
#define bin_noad  (ord_noad  + 2)
#define rel_noad  (ord_noad  + 3)
#define open_noad  (ord_noad  + 4)
#define close_noad  (ord_noad  + 5)
#define punct_noad  (ord_noad  + 6)
#define inner_noad  (ord_noad  + 7)
#define limits 1
#define no_limits 2

/* module 827 */

/* A |radical_noad| is five words long; the fifth word is the |left_delimiter|
 * field, which usually represents a square root sign.
 * 
 * A |fraction_noad| is six words long; it has a |right_delimiter| field
 * as well as a |left_delimiter|.
 * 
 * Delimiter fields are of type |four_quarters|, and they have four subfields
 * called |small_fam|, |small_char|, |large_fam|, |large_char|. These subfields
 * represent variable-size delimiters by giving the ``small'' and ``large''
 * starting characters, as explained in Chapter~17 of {\sl The \TeX book}.
 * 
 * A |fraction_noad| is actually quite different from all other noads. Not
 * only does it have six words, it has |thickness|, |denominator|, and
 * |numerator| fields instead of |nucleus|, |subscr|, and |supscr|. The
 * |thickness| is a scaled value that tells how thick to make a fraction
 * rule; however, the special value |default_code| is used to stand for the
 * |default_rule_thickness| of the current size. The |numerator| and
 * |denominator| point to mlists that define a fraction; we always have
 * $$\hbox{|math_type(numerator)=math_type(denominator)=sub_mlist|}.$$ The
 * |left_delimiter| and |right_delimiter| fields specify delimiters that will
 * be placed at the left and right of the fraction. In this way, a
 * |fraction_noad| is able to represent all of \TeX's operators \.{\\over},
 * \.{\\atop}, \.{\\above}, \.{\\overwithdelims}, \.{\\atopwithdelims}, and
 * 
 * \.{\\abovewithdelims}.
 */
#define left_delimiter( arg )  (arg  + 4)
#define right_delimiter( arg )  (arg  + 5)
#define radical_noad  (inner_noad  + 1)
#define radical_noad_size 5
#define fraction_noad  (radical_noad  + 1)
#define fraction_noad_size 6
#define small_fam( arg )  mem [ arg ]. qqqq . b0
#define small_char( arg )  mem [ arg ]. qqqq . b1
#define large_fam( arg )  mem [ arg ]. qqqq . b2
#define large_char( arg )  mem [ arg ]. qqqq . b3
#define thickness  width
#define default_code 1073741824
#define numerator  supscr
#define denominator  subscr


/* module 828 */
EXTERN two_halves empty_field;
EXTERN four_quarters null_delimiter;

EXTERN void math_initialize (void);

EXTERN pointer new_noad (void);

/* module 831 */

/* A few more kinds of noads will complete the set: An |under_noad| has its
 * nucleus underlined; an |over_noad| has it overlined. An |accent_noad| places
 * an accent over its nucleus; the accent character appears as
 * |fam(accent_chr(p))| and |character(accent_chr(p))|. A |vcenter_noad|
 * centers its nucleus vertically with respect to the axis of the formula;
 * in such noads we always have |math_type(nucleus(p))=sub_box|.
 * 
 * And finally, we have |left_noad| and |right_noad| types, to implement
 * \TeX's \.{\\left} and \.{\\right} as well as \eTeX's \.{\\middle}.
 * The |nucleus| of such noads is
 * replaced by a |delimiter| field; thus, for example, `\.{\\left(}' produces
 * a |left_noad| such that |delimiter(p)| holds the family and character
 * codes for all left parentheses. A |left_noad| never appears in an mlist
 * except as the first element, and a |right_noad| never appears in an mlist
 * except as the last element; furthermore, we either have both a |left_noad|
 * and a |right_noad|, or neither one is present. The |subscr| and |supscr|
 * fields are always |empty| in a |left_noad| and a |right_noad|.
 */
#define under_noad  (fraction_noad  + 1)
#define over_noad  (under_noad  + 1)
#define accent_noad  (over_noad  + 1)
#define accent_noad_size 5
#define accent_chr( arg )  (arg  + 4)
#define vcenter_noad  (accent_noad  + 1)
#define left_noad  (vcenter_noad  + 1)
#define right_noad  (left_noad  + 1)
#define delimiter  nucleus
#define middle_noad 1
#define scripts_allowed( arg ) ( type ( arg ) >=  ord_noad )  && ( type ( arg ) <  left_noad )


/* module 832 */
#define style_node  (unset_node  + 1)
#define style_node_size 3
#define display_style 0
#define text_style 2
#define script_style 4
#define script_script_style 6
#define cramped 1

EXTERN pointer new_style (small_number s);
EXTERN pointer new_choice (void);

/* module 833 */
#define choice_node  (unset_node  + 2)
#define display_mlist( arg )  info ( arg  + 1 )
#define text_mlist( arg )  link ( arg  + 1 )
#define script_mlist( arg )  info ( arg  + 2 )
#define script_script_mlist( arg )  link ( arg  + 2 )

EXTERN void print_fam_and_char (pointer p);
EXTERN void print_delimiter (pointer p);
EXTERN void print_subsidiary_data (pointer p, ASCII_code c);
EXTERN void print_style (int c);

/* module 843 */
#define text_size 0
#define script_size 16
#define script_script_size 32

/* module 844 */
#define math_quad(a)  font_info [6 + param_base[fam_fnt (2 + a)]].sc
#define total_mathsy_params 22

/* module 845 */
#define total_mathex_params 13

EXTERN void flush_math (void);

/* module 863 */
EXTERN pointer cur_mlist;/* beginning of mlist to be translated */
EXTERN small_number cur_style;/* style code at current place in the list */
EXTERN boolean mlist_penalties;/* should |mlist_to_hlist| insert penalties? */

EXTERN void mlist_to_hlist (void);

