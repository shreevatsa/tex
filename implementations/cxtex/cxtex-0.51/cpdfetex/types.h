

typedef long int integer;
typedef double glue_ratio;
typedef double real;

#include <kpathsea/types.h> /* string etc. */

#include <stdio.h> /* FILE * */

/* module 17 */
/* 
 * In order to make \TeX\ readily portable to a wide variety of
 * computers, all of its input text is converted to an internal eight-bit
 * code that includes standard ASCII, the ``American Standard Code for
 * Information Interchange.'' This conversion is done immediately when each
 * character is read in. Conversely, characters are converted from ASCII to
 * the user's external representation just before they are output to a
 * text file.
 * 
 * Such an internal code is relevant to users of \TeX\ primarily because it
 * governs the positions of characters in the fonts. For example, the
 * character `\.A' has ASCII code $65= oct(101)$, and when \TeX\ typesets
 * this letter it specifies character number 65 in the current font.
 * If that font actually has `\.A' in a different position, \TeX\ doesn't
 * know what the real position is; the program that does the actual printing from
 * \TeX's device-independent files is responsible for converting from ASCII to
 * a particular font encoding.
 * 
 * \TeX's internal code also defines the value of constants
 * that begin with a reverse apostrophe; and it provides an index to the
 * \.{\\catcode}, \.{\\mathcode}, \.{\\uccode}, \.{\\lccode}, and \.{\\delcode}
 * tables.
 */

/* module 18 */

/* Characters of text that have been converted to \TeX's internal form
 * are said to be of type |ASCII_code|, which is a subrange of the integers.
 */

typedef unsigned char ASCII_code; /* eight-bit numbers */

/* module 25 */
/* 
 * The bane of portability is the fact that different operating systems treat
 * input and output quite differently, perhaps because computer scientists
 * have not given sufficient attention to this problem. People have felt somehow
 * that input and output are not part of ``real'' programming. Well, it is true
 * that some kinds of programming are more fun than others. With existing
 * input/output conventions being so diverse and so messy, the only sources of
 * joy in such parts of the code are the rare occasions when one can find a
 * way to make the program a little less bad than it might have been. We have
 * two choices, either to attack I/O now and get it over with, or to postpone
 * I/O until near the end. Neither prospect is very attractive, so let's
 * get it over with.
 * 
 * The basic operations we need to do are (1)~inputting and outputting of
 * text, to or from a file or the user's terminal; (2)~inputting and
 * outputting of eight-bit bytes, to or from a file; (3)~instructing the
 * operating system to initiate (``open'') or to terminate (``close'') input or
 * output from a specified file; (4)~testing whether the end of an input
 * file has been reached.
 * 
 * \TeX\ needs to deal with two kinds of files.
 * We shall use the term |alpha_file| for a file that contains textual data,
 * and the term |byte_file| for a file that contains eight-bit binary information.
 * These two types turn out to be the same on many computers, but
 * sometimes there is a significant distinction, so we shall be careful to
 * distinguish between them. Standard protocols for transferring
 * such files from computer to computer, via high-speed networks, are
 * now becoming available to more and more communities of users.
 * 
 * The program actually makes use also of a third kind of file, called a
 * |word_file|, when dumping and reloading base information for its own
 * initialization. We shall define a word file later; but it will be possible
 * for us to specify simple operations on word files before they are defined.
 */
typedef unsigned char eight_bits; /* unsigned one-byte quantity */ 
/* typedef text alpha_file; *//* files that contain textual data */ 
/* typedef text byte_file; *//* files that contain binary data */

/* module 38 */
/* 
 * Control sequence names and diagnostic messages are variable-length strings
 * of eight-bit characters. Since \PASCAL\ does not have a well-developed string
 * mechanism, \TeX\ does all of its string processing by homegrown methods.
 * 
 * Elaborate facilities for dynamic strings are not needed, so all of the
 * necessary operations can be handled with a simple data structure.
 * The array |str_pool| contains all of the (eight-bit) ASCII codes in all
 * of the strings, and the array |str_start| contains indices of the starting
 * points of each string. Strings are referred to by integer numbers, so that
 * string number |s| comprises the characters |str_pool[j]| for
 * |str_start[s]<=j<str_start[s+1]|. Additional integer variables
 * |pool_ptr| and |str_ptr| indicate the number of entries used so far
 * in |str_pool| and |str_start|, respectively; locations
 * |str_pool[pool_ptr]| and |str_start[str_ptr]| are
 * ready for the next string to be allocated.
 * 
 * String numbers 0 to 255 are reserved for strings that correspond to single
 * ASCII characters. This is in accordance with the conventions of \.{WEB},
 * 
 * which converts single-character strings into the ASCII code number of the
 * single character involved, while it converts other strings into integers
 * and builds a string pool file. Thus, when the string constant \.{"."} appears
 * in the program below, \.{WEB} converts it into the integer 46, which is the
 * ASCII code for a period, while \.{WEB} will convert a string like \.{"hello"}
 * into some integer greater than~255. String number 46 will presumably be the
 * single character `\..'; but some ASCII codes have no standard visible
 * representation, and \TeX\ sometimes needs to be able to zprint an arbitrary
 * ASCII character, so the first 256 strings are used to specify exactly what
 * should be printed for each of the 256 possibilities.
 * 
 * Elements of the |str_pool| array must be ASCII codes that can actually
 * be printed; i.e., they must have an |xchr| equivalent in the local
 * character set. (This restriction applies only to preloaded strings,
 * not to those generated dynamically by the user.)
 * 
 * Some \PASCAL\ compilers won't pack integers into a single byte unless the
 * integers lie in the range |-128..127|. To accommodate such systems
 * we access the string pool only via macros that can easily be redefined.
 */
typedef integer pool_pointer; /* for variables that point into |str_pool| */
typedef integer str_number;  /* for variables that point into |str_start| */
typedef unsigned char packed_ASCII_code; /* elements of |str_pool| array */


/* module 99 */

/* 
 * The principal computations performed by \TeX\ are done entirely in terms of
 * integers less than $2^{31}$ in magnitude; and divisions are done only when both
 * dividend and divisor are nonnegative. Thus, the arithmetic specified in this
 * program can be carried out in exactly the same way on a wide variety of
 * computers, including some small ones. Why? Because the arithmetic
 * calculations need to be spelled out precisely in order to guarantee that
 * \TeX\ will produce identical output on different machines. If some
 * quantities were rounded differently in different implementations, we would
 * find that line breaks and even page breaks might occur in different places.
 * Hence the arithmetic of \TeX\ has been designed with care, and systems that
 * claim to be implementations of \TeX82 should follow precisely the
 * calculations as they appear in the present program.
 * 
 * (Actually there are three places where \TeX\ uses |div| with a possibly negative
 * numerator. These are harmless; see |div| in the index. Also if the user
 * sets the \.{\\time} or the \.{\\year} to a negative value, some diagnostic
 * information will involve negative-numerator division. The same remarks
 * apply for |mod| as well as for |div|.)
 */

/* module 101 */

/* Fixed-point arithmetic is done on {\sl scaled integers\/} that are multiples
 * of $2^{-16}$. In other words, a binary point is assumed to be sixteen bit
 * positions from the right end of a binary computer word.
 */

typedef integer scaled; /* this type is used for scaled integers */ 
typedef unsigned int  nonnegative_integer; /* $0\L x<2^{31}$ */ 
typedef unsigned char small_number; /* this type is self-explanatory */

/* module 113 */
/* The reader should study the following definitions closely:  */
typedef unsigned char quarterword;
typedef int halfword;
typedef unsigned char two_choices;  /* used when there are two variants in a record */ 
typedef unsigned char  four_choices; /* used when there are four variants in a record */
typedef union
{
  struct
  {
#ifdef WORDS_BIGENDIAN
    halfword RH, LH;
#else
    halfword LH, RH;
#endif
  } v;

  struct
  {				/* Make B0,B1 overlap the most significant bytes of LH.  */
#ifdef WORDS_BIGENDIAN
    halfword junk;
    short B0, B1;
#else				/* not WORDS_BIGENDIAN */
    /* If 32-bit memory words, have to do something.  */
    short B1, B0;
#endif				/* LittleEndian */
  } u;
} two_halves;

typedef struct
{
  struct
  {
#ifdef WORDS_BIGENDIAN
    quarterword B0, B1, B2, B3;
#else
    quarterword B3, B2, B1, B0;
#endif
  } u;
} four_quarters;

typedef union
{
  glue_ratio gr;
  two_halves hh;
#ifdef WORDS_BIGENDIAN
  integer cint;
  four_quarters qqqq;
#else				/* not WORDS_BIGENDIAN */
  struct
  {
#if defined (TeX) && !defined (SMALLTeX) || defined (MF) && !defined (SMALLMF) || defined (MP) && !defined (SMALLMP)
    halfword junk;
#endif				/* big {TeX,MF,MP} */
    integer CINT;
  } u;

  struct
  {
#if defined (TeX) && !defined (SMALLTeX) || defined (MF) && !defined (SMALLMF) || defined (MP) && !defined (SMALLMP)
    halfword junk;
#endif				/* big {TeX,MF,MP} */
    four_quarters QQQQ;
  } v;
#endif				/* not WORDS_BIGENDIAN */
} memory_word;


/* fmemory_word for font_list; needs to be only four bytes.  This saves
   significant space in the .fmt files. */

typedef union
{
#ifdef WORDS_BIGENDIAN
  integer cint;
  four_quarters qqqq;
#else				/* not WORDS_BIGENDIAN */
  struct
  {
    integer CINT;
  } u;

  struct
  {
    four_quarters QQQQ;
  } v;
#endif				/* not WORDS_BIGENDIAN */
} fmemory_word;

/* To keep the original structure accesses working, we must go through
   the extra names C forced us to introduce.  */
#define	b0 u.B0
#define	b1 u.B1
#define	b2 u.B2
#define	b3 u.B3

#define rh v.RH
#define lhfield	v.LH

#ifndef WORDS_BIGENDIAN
#define cint u.CINT
#define qqqq v.QQQQ
#endif

/* module 115 */
//#define pointer  halfword
typedef halfword pointer;

/* module 150 */

/* A glue specification has a halfword reference count in its first word,
 * 
 * representing |null| plus the number of glue nodes that point to it (less one).
 * Note that the reference count appears in the same position as
 * the |link| field in list nodes; this is the field that is initialized
 * to |null| when a node is allocated, and it is also the field that is flagged
 * by |empty_flag| in empty nodes.
 * 
 * Glue specifications also contain three |scaled| fields, for the |width|,
 * |stretch|, and |shrink| dimensions. Finally, there are two one-byte
 * fields called |stretch_order| and |shrink_order|; these contain the
 * orders of infinity (|normal|, |fil|, |fill|, or |filll|)
 * corresponding to the stretch and shrink values.
 */

typedef unsigned char glue_ord; /* infinity to the 0, 1, 2, or 3 power */

/* module 212 */

/* The state of affairs at any semantic level can be represented by
 * five values:
 * 
 * \yskip\hang|mode| is the number representing the semantic mode, as
 * just explained.
 * 
 * \yskip\hang|head| is a |pointer| to a list head for the list being built;
 * |link(head)| therefore points to the first element of the list, or
 * to |null| if the list is empty.
 * 
 * \yskip\hang|tail| is a |pointer| to the final node of the list being
 * built; thus, |tail=head| if and only if the list is empty.
 * 
 * \yskip\hang|prev_graf| is the number of lines of the current paragraph that
 * have already been put into the present vertical list.
 * 
 * \yskip\hang|aux| is an auxiliary |memory_word| that gives further information
 * that is needed to characterize the situation.
 * 
 * \yskip\noindent
 * In vertical mode, |aux| is also known as |prev_depth|; it is the scaled
 * value representing the depth of the previous box, for use in baseline
 * calculations, or it is |<=-1000|pt if the next box on the vertical list is to
 * be exempt from baseline calculations. In horizontal mode, |aux| is also
 * known as |space_factor| and |clang|; it holds the current space factor used in
 * spacing calculations, and the current language used for hyphenation.
 * (The value of |clang| is undefined in restricted horizontal mode.)
 * In math mode, |aux| is also known as |incompleat_noad|; if
 * not |null|, it points to a record that represents the numerator of a
 * generalized fraction for which the denominator is currently being formed
 * in the current list.
 * 
 * There is also a sixth quantity, |mode_line|, which correlates
 * the semantic nest with the user's input; |mode_line| contains the source
 * line number at which the current level of nesting was entered. The negative
 * of this line number is the |mode_line| at the level of the
 * user's output routine.
 * 
 * A seventh quantity, |eTeX_aux|, is used by the extended features \eTeX.
 * In vertical modes it is known as |LR_save| and holds the LR stack when a
 * paragraph is interrupted by a displayed formula. In display math mode
 * it is known as |LR_box| and holds a pointer to a prototype box for the
 * display. In math mode it is known as |delim_ptr| and points to the most
 * recent |left_noad| or |middle_noad| of a |math_left_group|.
 * 
 * In horizontal mode, the |prev_graf| field is used for initial language data.
 * 
 * The semantic nest is an array called |nest| that holds the |mode|, |head|,
 * |tail|, |prev_graf|, |aux|, and |mode_line| values for all semantic levels
 * below the currently active one. Information about the currently active
 * level is kept in the global quantities |mode|, |head|, |tail|, |prev_graf|,
 * |aux|, and |mode_line|, which live in a \PASCAL\ record that is ready to
 * be pushed onto |nest| if necessary.
 */
typedef struct
{
  int mode_field;
  pointer head_field, tail_field;
  pointer eTeX_aux_field;
  int pg_field, ml_field;
  memory_word aux_field;
} list_state_record;


/* module 269 */

/* Here are the group codes that are used to discriminate between different
 * kinds of groups. They allow \TeX\ to decide what special actions, if any,
 * should be performed when a group ends.
 * \def\grp{\.{\char'173...\char'175}}
 * 
 * Some groups are not supposed to be ended by right braces. For example,
 * the `\.\$' that begins a math formula causes a |math_shift_group| to
 * be started, and this should be terminated by a matching `\.\$'. Similarly,
 * a group that starts with \.{\\left} should end with \.{\\right}, and
 * one that starts with \.{\\begingroup} should end with \.{\\endgroup}.
 */

typedef unsigned char group_code; /* |save_level| for a level boundary */

/* module 300 */

/* 
 * This implementation of
 * \TeX\ uses two different conventions for representing sequential stacks.
 * 
 * \yskip\hangg 1) If there is frequent access to the top entry, and if the
 * stack is essentially never empty, then the top entry is kept in a global
 * variable (even better would be a machine register), and the other entries
 * appear in the array $\\{stack}[0\to(\\{ptr}-1)]$. For example, the
 * semantic stack described above is handled this way, and so is the input
 * stack that we are about to study.
 * 
 * \yskip\hangg 2) If there is infrequent top access, the entire stack contents
 * are in the array $\\{stack}[0\to(\\{ptr}-1)]$. For example, the |save_stack|
 * is treated this way, as we have seen.
 * 
 * \yskip\noindent
 * The state of \TeX's input mechanism appears in the input stack, whose
 * entries are records with six fields, called |state|, |index|, |start|, |loc|,
 * |limit|, and |name|. This stack is maintained with
 * convention~(1), so it is declared in the following way:
 */

typedef struct {
  quarterword state_field, index_field;
  halfword start_field, loc_field, limit_field, name_field;
} in_state_record;

/* module 548 */

/* So that is what \.{TFM} files hold. Since \TeX\ has to absorb such information
 * about lots of fonts, it stores most of the data in a large array called
 * |font_info|. Each item of |font_info| is a |memory_word|; the |fix_word|
 * data gets converted into |scaled| entries, while everything else goes into
 * words of type |four_quarters|.
 * 
 * When the user defines \.{\\font\\f}, say, \TeX\ assigns an internal number
 * to the user's font~\.{\\f}. Adding this number to |font_id_base| gives the
 * |eqtb| location of a ``frozen'' control sequence that will always select
 * the font.
 */

typedef integer internal_font_number; /* |font| in a |char_node| */ 
typedef int font_index;               /* index into |font_info| */ 
typedef unsigned int nine_bits;


/* module 667 */
/* 
 * The cross-reference table |obj_tab| is an array of |obj_tab_size| of
 * |tab_entry|. Each entry contains four integer fields and represents an object
 * in PDF file whose object number is the index of this entry in |obj_tab|.
 * Objects in |obj_tab| maybe linked into list; objects in such a linked list have
 * the same type.
 */
typedef struct {
  int int0, int1, int2, int3;
} obj_entry;

/* module 679 */

/* We need to hold information about used characters in each font for partial downloading. */

typedef eight_bits char_used_array[32];
typedef eight_bits char_map_array[33]; /* move chars in range 0..32 */
typedef unsigned int vf_stack_index; /* an index into the stack */ 
typedef struct {
  scaled stack_h, stack_v, stack_w, stack_x, stack_y, stack_z;
} vf_stack_record;


/* module 1064 */

/* The patterns are stored in a compact table that is also efficient for
 * retrieval, using a variant of ``trie memory'' [cf.\ {\sl The Art of
 * Computer Programming \bf3} (1973), 481--505]. We can find each pattern
 * $p_1\ldots p_k$ by letting $z_0$ be one greater than the relevant language
 * index and then, for |1<=i<=k|,
 * setting |@t$z_i$@>:=trie_link@t$(z_{i-1})+p_i$@>|; the pattern will be
 * identified by the number $z_k$. Since all the pattern information is
 * packed together into a single |trie_link| array, it is necessary to
 * prevent confusion between the data from inequivalent patterns, so another
 * table is provided such that |trie_char@t$(z_i)=p_i$@>| for all |i|. There
 * is also a table |trie_op|$(z_k)$ to identify the numbers $n_0\ldots n_k$
 * associated with $p_1\ldots p_k$.
 * 
 * The theory that comparatively few different number sequences $n_0\ldots n_k$
 * actually occur, since most of the |n|'s are generally zero, seems to fail
 * at least for the large German hyphenation patterns.
 * Therefore the number sequences cannot any longer be encoded in such a way
 * that |trie_op|$(z_k)$ is only one byte long.
 * We have introduced a new constant |max_trie_op| for the maximum allowable
 * hyphenation operation code value; |max_trie_op| might be different for
 * \TeX\ and \.{INITEX} and must not exceed |max_halfword|.
 * An opcode will occupy a halfword if |max_trie_op| exceeds |max_quarterword|
 * or a quarterword otherwise.
 * 
 * If |trie_op(@t$z_k$@>)<>min_trie_op|, when $p_1\ldots p_k$ has matched
 * the letters in |hc[(l-k+1)..l@,]| of language |t|,
 * we perform all of the required operations
 * for this pattern by carrying out the following little program: Set
 * |v:=trie_op(@t$z_k$@>)|. Then set |v:=v+op_start[t]|,
 * |hyf[l-hyf_distance[v]]:=@tmax@>(hyf[l-hyf_distance[v]], hyf_num[v])|,
 * and |v:=hyf_next[v]|; repeat, if necessary, until |v=min_trie_op|.
 */

typedef integer trie_pointer; /* an index into |trie| */ 
typedef integer trie_opcode; /* a trie opcode */

/* module 1069 */

/* The exception table that is built by \TeX's \.{\\hyphenation} primitive is
 * organized as an ordered hash table [cf.\ Amble and Knuth, {\sl The Computer
 * 
 * Journal\/ \bf17} (1974), 135--142] using linear probing. If $\alpha$ and
 * $\beta$ are words, we will say that $\alpha<\beta$ if $\vert\alpha\vert<
 * \vert\beta\vert$ or if $\vert\alpha\vert=\vert\beta\vert$ and
 * $\alpha$ is lexicographically smaller than $\beta$. (The notation $\vert
 * \alpha\vert$ stands for the length of $\alpha$.) The idea of ordered hashing
 * is to arrange the table so that a given word $\alpha$ can be sought by computing
 * a hash address $h=h(\alpha)$ and then looking in table positions |h|, |h-1|,
 * \dots, until encountering the first word $\L\alpha$. If this word is
 * different from $\alpha$, we can conclude that $\alpha$ is not in the table.
 * This is a clever scheme which saves the need for a hash link array.
 * However, it is difficult to increase the size of the hyphen exception
 * arrays. To make this easier, the ordered hash has been replaced by
 * a simple hash, using an additional array |hyph_link|. The value
 * |0| in |hyph_link[k]| means that there are no more entries corresponding
 * to the specific hash chain. When |hyph_link[k]>0|, the next entry in
 * the hash chain is |hyph_link[k]-1|. This value is used because the
 * arrays start at |0|.
 * 
 * The words in the table point to lists in |mem| that specify hyphen positions
 * in their |info| fields. The list for $c_1\ldots c_n$ contains the number |k| if
 * the word $c_1\ldots c_n$ has a discretionary hyphen between $c_k$ and
 * $c_{k+1}$.
 */

typedef integer hyph_pointer; /* index into hyphen exceptions hash table; 
									  enlarging this requires changing (un)dump code */
/* module 1575 */

/* Shiping out PDF mark. */
typedef struct {
  str_number objname;
  int objnum;
} dest_name_entry;


/* module 1624 */
typedef unsigned int save_pointer; /* index into |save_stack| */

