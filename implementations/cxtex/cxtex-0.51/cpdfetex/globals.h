
#define EXTERN extern

/* module 2 */

#define banner  "This is pdfeTeX, Version 3.141592", '-' , pdftex_version_string , '-' , eTeX_version_string
#define pdftex_version 111
#define pdftex_revision  'b'
#define pdftex_version_string  "1.11b"
#define TEX  ETEX
#define eTeX_version_string  "2.1"
#define eTeX_version 2
#define eTeX_revision  ".1"
#define eTeX_states 1


/* module 10 */

/* This \TeX\ implementation conforms to the rules of the {\sl Pascal User
 * Manual} published by Jensen and Wirth in 1975, except where system-dependent
 * 
 * code is necessary to make a useful system program, and except in another
 * respect where such conformity would unnecessarily obscure the meaning
 * and clutter up the code: We assume that |case| statements may include a
 * default case that applies if no matching label is found. Thus, we shall use
 * constructions like
 * $$\vbox{\halign{\ignorespaces#\hfil\cr
 * |case x of|\cr
 * 1: $\langle\,$code for $x=1\,\rangle$;\cr
 * 3: $\langle\,$code for $x=3\,\rangle$;\cr
 * |othercases| $\langle\,$code for |x<>1| and |x<>3|$\,\rangle$\cr
 * |endcases|\cr}}$$
 * since most \PASCAL\ compilers have plugged this hole in the language by
 * incorporating some sort of default mechanism. For example, the \ph\
 * compiler allows `|others|:' as a default label, and other \PASCAL s allow
 * syntaxes like `\&{else}' or `\&{otherwise}' or `\\{otherwise}:', etc. The
 * definitions of |othercases| and |endcases| should be changed to agree with
 * local conventions. Note that no semicolon appears before |endcases| in
 * this program, so the definition of |endcases| should include a semicolon
 * if the compiler wants one. (Of course, if no default mechanism is
 * available, the |case| statements of \TeX\ will have to be laboriously
 * extended by listing all remaining cases. People who are stuck with such
 * \PASCAL s have, in fact, done this, successfully but not happily!)
 */

/* module 11 */

#define file_name_size  FILENAME_MAX
#define ssup_error_line 255
#define ssup_max_strings 262143
#define ssup_trie_opcode 65535
#define ssup_trie_size 262143
#define ssup_hyph_size 65535
#define iinf_hyphen_size 610
#define max_font_max 2000
#define font_base 0


/* module 11 */

/* The following parameters can be changed at compile time to extend or
 * reduce \TeX's capacity. They may have different values in \.{INITEX} and
 * in production versions of \TeX.
 */

#define mem_bot ( 0 ) /* smallest index in the |mem| array dumped by \.{INITEX}; must not be less than |mem_min| 
						 Use |mem_bot=0| for compilers which cannot decrement pointers. */

#define inf_main_memory ( 2999 )
#define sup_main_memory ( 32000000 )
#define inf_max_strings ( 3000 )
#define sup_max_strings ( ssup_max_strings )
#define inf_strings_free ( 100 )
#define sup_strings_free ( sup_max_strings )
#define inf_buf_size ( 500 )
#define sup_buf_size ( 300000 )
#define inf_nest_size ( 40 )
#define sup_nest_size ( 4000 )
#define inf_max_in_open ( 6 )
#define sup_max_in_open ( 127 )
#define inf_param_size ( 60 )
#define sup_param_size ( 6000 )
#define inf_save_size ( 600 )
#define sup_save_size ( 40000 )
#define inf_stack_size ( 200 )
#define sup_stack_size ( 30000 )
#define inf_dvi_buf_size ( 800 )
#define sup_dvi_buf_size ( 65536 )
#define inf_font_mem_size ( 20000 )
#define sup_font_mem_size ( 1000000 )
#define sup_font_max ( max_font_max )
#define inf_font_max ( 50 ) /* could be smaller, but why? */
#define inf_pool_size ( 32000 )
#define sup_pool_size ( 40000000 )
#define inf_pool_free ( 1000 )
#define sup_pool_free ( sup_pool_size )
#define inf_string_vacancies ( 8000 )
#define sup_string_vacancies ( sup_pool_size-23000 )
#define sup_hyph_size ( ssup_hyph_size )
#define inf_hyph_size ( iinf_hyphen_size ) /* Must be not less than |hyph_prime|! */

/* module 18 */
#define text_char  ASCII_code

/* module 32 */
extern boolean ini_version; /* are we \.{INITEX}? */ 
extern integer main_memory; /* total memory words allocated in initex */
extern integer extra_mem_bot; /* |mem_min:=mem_bot-extra_mem_bot| except  in \.{INITEX} */ 
extern integer mem_min; /* smallest index in \TeX's internal |mem| array; must be 
			   |min_halfword| or more; must be equal to |mem_bot| in  \.{INITEX}, otherwise |<=mem_bot| */
extern integer mem_top; /* largest index in the |mem| array dumped by \.{INITEX}; 
			   must be substantially larger than |mem_bot|, equal to 
			   |mem_max| in \.{INITEX}, else not greater than |mem_max|*/
extern integer extra_mem_top; /* |mem_max:=mem_top+extra_mem_top| except  in \.{INITEX} */ 
extern integer mem_max; /* greatest index in \TeX's internal |mem| array; must be 
			   strictly less than |max_halfword|; must be equal to 
			   |mem_top| in \.{INITEX}, otherwise |>=mem_top| */
extern integer font_mem_size; /* number of words of |font_info| for all fonts */ 
extern integer font_max; /* maximum internal font number; ok to exceed 
					 |max_quarterword| and must be at most |font_base|+
					 |max_font_max| */
extern integer font_k; /* loop variable for initialization */ 
extern integer hyph_size; /* maximun number of hyphen exceptions */ 
extern integer buf_size; /* maximum number of characters simultaneously present in 
					 current lines of open files and in control sequences 
					 between \.{\\csname} and \.{\\endcsname}; must not 
					 exceed |max_halfword| */
extern integer stack_size; /* maximum number of simultaneous input sources */ 
extern integer max_in_open; /* maximum number of input files and error insertions  that can be going on simultaneously */
extern integer param_size; /* maximum number of simultaneous macro parameters */ 
extern integer nest_size; /* maximum number of semantic levels simultaneously  active */ 
extern integer save_size; /* space for saving values outside of current group; 
					  must be at most |max_halfword| */
extern integer dvi_buf_size; /* size of the output buffer; must be a multiple of 8*/ 

extern unsigned char k;


/* module 101 */
#define unity 65536
#define two 131072
/* module 108 */
#define inf_bad 10000

/* module 110 */

/* 
 * In order to make efficient use of storage space, \TeX\ bases its major data
 * structures on a |memory_word|, which contains either a (signed) integer,
 * possibly scaled, or a (signed) |glue_ratio|, or a small number of
 * fields that are one half or one quarter of the size used for storing
 * integers.
 * 
 * If |x| is a variable of type |memory_word|, it contains up to four
 * fields that can be referred to as follows:
 * $$\vbox{\halign{\hfil#&#\hfil&#\hfil\cr
 * |x|&.|cint|&(an |integer|)\cr
 * |x|&.|sc|\qquad&(a |scaled| integer)\cr
 * |x|&.|gr|&(a |glue_ratio|)\cr
 * |x.hh.lhfield|, |x.hh|&.|rh|&(two halfword fields)\cr
 * |x.hh.b0|, |x.hh.b1|, |x.hh|&.|rh|&(two quarterword fields, one halfword
 * 
 * field)\cr
 * |x.qqqq.b0|, |x.qqqq.b1|, |x.qqqq|&.|b2|, |x.qqqq.b3|\hskip-100pt
 * 
 * &\qquad\qquad\qquad(four quarterword fields)\cr}}$$
 * This is somewhat cumbersome to write, and not very readable either, but
 * macros will be used to make the notation shorter and more transparent.
 * The \PASCAL\ code below gives a formal definition of |memory_word| and
 * its subsidiary types, using packed variant records. \TeX\ makes no
 * assumptions about the relative positions of the fields within a word.
 * 
 * Since we are assuming 32-bit integers, a halfword must contain at least
 * 16 bits, and a quarterword must contain at least 8 bits.
 * 
 * But it doesn't hurt to have more bits; for example, with enough 36-bit
 * words you might be able to have |mem_max| as large as 262142, which is
 * eight times as much memory as anybody had during the first four years of
 * \TeX's existence.
 * 
 * N.B.: Valuable memory space will be dreadfully wasted unless \TeX\ is compiled
 * by a \PASCAL\ that packs all of the |memory_word| variants into
 * the space of a single integer. This means, for example, that |glue_ratio|
 * words should be |short_real| instead of |real| on some computers. Some
 * \PASCAL\ compilers will pack an integer whose subrange is `|0..255|' into
 * an eight-bit field, but others insist on allocating space for an additional
 * sign bit; on such systems you can get 256 values into a quarterword only
 * if the subrange is `|-128..127|'.
 * 
 * The present implementation tries to accommodate as many variations as possible,
 * so it makes few assumptions. If integers having the subrange
 * `|min_quarterword..max_quarterword|' can be packed into a quarterword,
 * and if integers having the subrange `|min_halfword..max_halfword|'
 * can be packed into a halfword, everything should work satisfactorily.
 * 
 * It is usually most efficient to have |min_quarterword=min_halfword=0|,
 * so one should try to achieve this unless it causes a severe problem.
 * The values defined here are recommended for most 32-bit computers.
 */
#define min_quarterword 0
#define max_quarterword 255
#define min_halfword  (-268435455)
#define max_halfword 268435455

/* module 112 */

/* The operation of adding or subtracting |min_quarterword| occurs quite
 * frequently in \TeX, so it is convenient to abbreviate this operation
 * by using the macros |qi| and |qo| for input and output to and from
 * quarterword format.
 * 
 * The inner loop of \TeX\ will run faster with respect to compilers
 * that don't optimize expressions like `|x+0|' and `|x-0|', if these
 * macros are simplified in the obvious way when |min_quarterword=0|.
 * So they have been simplified here in the obvious way.
 */
#define qi( arg )  arg
#define qo( arg )  arg
#define hi( arg )  arg
#define ho( arg )  arg

/* module 113 */
#define sc  cint

/* module 115 */
#define null  min_halfword

#include "print.h"
#include "tex_error.h"
#include "xordchr.h"
#include "help.h"
#include "tex_string.h"
#include "calc.h"
#include "mem.h"
#include "nest.h"
#include "hash.h"
#include "eqtb.h"
#include "save.h"
#include "mag.h"
#include "nodelist.h"
#include "tex_io.h"
#include "cmdchr.h"
#include "mark.h"
#include "tokens.h"
#include "call.h"
#include "scan.h"
#include "tokenlist.h"
#include "if.h"
#include "tfm.h"
#include "font.h"
#include "dvi.h"
#include "pdfbasic.h"
#include "pdflowlevel.h"
#include "pdfpag.h"
#include "pdfxref.h"
#include "pdffont.h"
#include "vf.h"
#include "pdfout.h"
#include "pack.h"
#include "tex_math.h"
#include "mathbuild.h"
#include "align.h"
#include "postlinebreak.h"
#include "hyphenate.h"
#include "linebreak.h"
#include "trie.h"
#include "vsplit.h"
#include "buildpage.h"
#include "glue.h"
#include "penalty.h"
#include "kern.h"
#include "par.h"
#include "box.h"
#include "rule.h"
#include "insert.h"
#include "lig.h"
#include "adjust.h"
#include "disc.h"
#include "control.h"
#include "prefix.h"
#include "show.h"
#include "dump.h"

#include "snap.h"
#include "xet.h"
#include "mltex.h"
#include "sa.h"
#include "etex.h"
#include "exten.h"
#include "thread.h"
#include "pdfproc.h"
