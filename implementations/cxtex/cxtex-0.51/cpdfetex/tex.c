
#include "types.h"
#include "c-compat.h"
#include "globals.h"
#include "main.h"
#include "mainio.h"

/* % e-TeX is copyright (C) 1994,98 by the NTS team; all rights are reserved.
 * % Copying of this file is authorized only if (1) you are a member of the
 * % NTS team, or if (2) you make absolutely no changes to your copy.
 * % (Programs such as PATCHWEB, TIE, or WEBMERGE allow the application of
 * % several change files to tex.web; the master files tex.web and etex.ch
 * % should stay intact.)
 * 
 * % See etex_gen.tex for hints on how to install this program.
 * % And see etripman.tex for details about how to validate it.
 * 
 * % e-TeX and NTS are trademarks of the NTS group.
 * % TeX is a trademark of the American Mathematical Society.
 * % METAFONT is a trademark of Addison-Wesley Publishing Company.
 * 
 * % This program is directly derived from Donald E. Knuth's TeX;
 * % the change history which follows and the reward offered for finders of
 * % bugs refer specifically to TeX; they should not be taken as referring
 * % to e-TeX, although the change history is relevant in that it
 * % demonstrates the evolutionary path followed. This program is not TeX;
 * % that name is reserved strictly for the program which is the creation
 * % and sole responsibility of Professor Knuth.
 * 
 * % A preliminary version of TeX--XeT was released in April 1992.
 * % TeX--XeT version 1.0 was released in June 1992,
 * % version 1.1 prevented arith overflow in glue computation (Oct 1992).
 * % A preliminary e-TeX version 0.95 was operational in March 1994.
 * % Version 1.0beta was released in May 1995.
 * % Version 1.01beta fixed bugs in just_copy and every_eof (December 1995).
 * % Version 1.02beta allowed 256 mark classes (March 1996).
 * % Version 1.1 changed \group{type,level} -> \currentgroup{type,level},
 * % first public release (October 1996).
 * % Version 2.0 development was started in March 1997;
 * % fixed a ligature-\beginR bug in January 1998;
 * % was released in March 1998.
 * % Version 2.1 fixed a marks bug (when min_halfword<>0) (January 1999).
 * 
 * % Although considerable effort has been expended to make the e-TeX program
 * % correct and reliable, no warranty is implied; the authors disclaim any
 * % obligation or liability for damages, including but not limited to
 * % special, indirect, or consequential damages arising out of or in
 * % connection with the use or performance of this software. This work has
 * % been a ``labor of love'' and the authors hope that users enjoy it.
 * 
 */

/* 
 * This is \eTeX, a program derived from and extending the capabilities of
 * \TeX, a document compiler intended to produce typesetting of high
 * quality.
 * The \PASCAL\ program that follows is the definition of \TeX82, a standard
 * 
 * version of \TeX\ that is designed to be highly portable so that identical output
 * will be obtainable on a great variety of computers.
 * 
 * The main purpose of the following program is to explain the algorithms of \TeX\
 * as clearly as possible. As a result, the program will not necessarily be very
 * efficient when a particular \PASCAL\ compiler has translated it into a
 * particular machine language. However, the program has been written so that it
 * can be tuned to run efficiently in a wide variety of operating environments
 * by making comparatively few changes. Such flexibility is possible because
 * the documentation that follows is written in the \.{WEB} language, which is
 * at a higher level than \PASCAL; the preprocessing step that converts \.{WEB}
 * to \PASCAL\ is able to introduce most of the necessary refinements.
 * Semi-automatic translation to other languages is also feasible, because the
 * program below does not make extensive use of features that are peculiar to
 * \PASCAL.
 * 
 * A large piece of software like \TeX\ has inherent complexity that cannot
 * be reduced below a certain level of difficulty, although each individual
 * part is fairly simple by itself. The \.{WEB} language is intended to make
 * the algorithms as readable as possible, by reflecting the way the
 * individual program pieces fit together and by providing the
 * cross-references that connect different parts. Detailed comments about
 * what is going on, and about why things were done in certain ways, have
 * been liberally sprinkled throughout the program. These comments explain
 * features of the implementation, but they rarely attempt to explain the
 * \TeX\ language itself, since the reader is supposed to be familiar with
 * {\sl The \TeX book}.
 */

/* Different \PASCAL s have slightly different conventions, and the present
 * program expresses \TeX\ in terms of the \PASCAL\ that was
 * available to the author in 1982. Constructions that apply to
 * this particular compiler, which we shall call \ph, should help the
 * reader see how to make an appropriate interface for other systems
 * if necessary. (\ph\ is Charles Hedrick's modification of a compiler
 * 
 * for the DECsystem-10 that was originally developed at the University of
 * Hamburg; cf.\ {\sl SOFTWARE---Practice \AM\ Experience \bf6} (1976),
 * 29--42. The \TeX\ program below is intended to be adaptable, without
 * extensive changes, to most other versions of \PASCAL, so it does not fully
 * use the admirable features of \ph. Indeed, a conscious effort has been
 * made here to avoid using several idiosyncratic features of standard
 * \PASCAL\ itself, so that most of the code can be translated mechanically
 * into other high-level languages. For example, the `\&{with}' and `\\{new}'
 * features are not used, nor are pointer types, set types, or enumerated
 * scalar types; there are no `\&{var}' parameters, except in the case of files;
 * there are no tag fields on variant records; there are no assignments
 * |real:=integer|; no procedures are declared local to other procedures.)
 * 
 * The portions of this program that involve system-dependent code, where
 * changes might be necessary because of differences between \PASCAL\ compilers
 * and/or differences between
 * operating systems, can be identified by looking at the sections whose
 * numbers are listed under `system dependencies' in the index. Furthermore,
 * the index entries for `dirty \PASCAL' list all places where the restrictions
 * of \PASCAL\ have not been followed perfectly, for one reason or another.
 * 
 * Incidentally, \PASCAL's standard |round| function can be problematical,
 * because it disagrees with the IEEE floating-point standard.
 * Many implementors have
 * therefore chosen to substitute their own home-grown rounding procedure.
 */

/* module 2 */

/* The present implementation has a long ancestry, beginning in the summer
 * of~1977, when Michael~F. Plass and Frank~M. Liang designed and coded
 * a prototype
 * 
 * based on some specifications that the author had made in May of that year.
 * This original proto\TeX\ included macro definitions and elementary
 * manipulations on boxes and glue, but it did not have line-breaking,
 * page-breaking, mathematical formulas, alignment routines, error recovery,
 * or the present semantic nest; furthermore,
 * it used character lists instead of token lists, so that a control sequence
 * like \.{\\halign} was represented by a list of seven characters. A
 * complete version of \TeX\ was designed and coded by the author in late
 * 1977 and early 1978; that program, like its prototype, was written in the
 * {\mc SAIL} language, for which an excellent debugging system was
 * available. Preliminary plans to convert the {\mc SAIL} code into a form
 * somewhat like the present ``web'' were developed by Luis Trabb~Pardo and
 * the author at the beginning of 1979, and a complete implementation was
 * created by Ignacio~A. Zabala in 1979 and 1980. The \TeX82 program, which
 * 
 * was written by the author during the latter part of 1981 and the early
 * part of 1982, also incorporates ideas from the 1979 implementation of
 * 
 * \TeX\ in {\mc MESA} that was written by Leonidas Guibas, Robert Sedgewick,
 * and Douglas Wyatt at the Xerox Palo Alto Research Center. Several hundred
 * refinements were introduced into \TeX82 based on the experiences gained with
 * the original implementations, so that essentially every part of the system
 * has been substantially improved. After the appearance of ``Version 0'' in
 * September 1982, this program benefited greatly from the comments of
 * many other people, notably David~R. Fuchs and Howard~W. Trickey.
 * A final revision in September 1989 extended the input character set to
 * eight-bit codes and introduced the ability to hyphenate words from
 * different languages, based on some ideas of Michael~J. Ferguson.
 * 
 * No doubt there still is plenty of room for improvement, but the author
 * is firmly committed to keeping \TeX82 ``frozen'' from now on; stability
 * and reliability are to be its main virtues.
 * 
 * On the other hand, the \.{WEB} description can be extended without changing
 * the core of \TeX82 itself, and the program has been designed so that such
 * extensions are not extremely difficult to make.
 * The |banner| string defined here should be changed whenever \TeX\
 * undergoes any modifications, so that it will be clear which version of
 * \TeX\ might be the guilty party when a problem arises.
 * 
 * This program contains code for various features extending \TeX,
 * therefore this program is called `\eTeX' and not
 * `\TeX'; the official name `\TeX' by itself is reserved
 * for software systems that are fully compatible with each other.
 * A special test suite called the ``\.{TRIP} test'' is available for
 * helping to determine whether a particular implementation deserves to be
 * known as `\TeX' [cf.~Stanford Computer Science report CS1027,
 * November 1984].
 * 
 * ML\TeX{} will add new primitives changing the behaviour of \TeX. The
 * |banner| string has to be changed. We do not change the |banner|
 * string, but will output an additional line to make clear that this is
 * a modified \TeX{} version.
 * 
 * A similar test suite called the ``\.{e-TRIP} test'' is available for
 * helping to determine whether a particular implementation deserves to be
 * known as `\eTeX'.
 */

/* module 4 */

/* The program begins with a normal \PASCAL\ program heading, whose
 * components will be filled in later, using the conventions of \.{WEB}.
 * 
 * For example, the portion of the program called `\X\glob:Global
 * variables\X' below will be replaced by a sequence of variable declarations
 * that starts in $\section\glob$ of this documentation. In this way, we are able
 * to define each individual global variable when we are prepared to
 * understand what it means; we do not have to define all of the globals at
 * once. Cross references in $\section\glob$, where it says ``See also
 * sections \gglob, \dots,'' also make it possible to look at the set of
 * all global variables, if desired. Similar remarks apply to the other
 * portions of the program heading.
 */

/* module 5 */

/* The overall \TeX\ program begins with the heading just shown, after which
 * comes a bunch of procedure declarations and function declarations.
 * Finally we will get to the main program, which begins with the
 * comment `|start_here|'. If you want to skip down to the
 * main program now, you can look up `|start_here|' in the index.
 * But the author suggests that the best way to understand this program
 * is to follow pretty much the order of \TeX's components as they appear in the
 * \.{WEB} description you are now reading, since the present ordering is
 * intended to combine the advantages of the ``bottom up'' and ``top down''
 * approaches to the problem of understanding a somewhat complicated system.
 */

/* module 7 */

/* Some of the code below is intended to be used only when diagnosing the
 * strange behavior that sometimes occurs when \TeX\ is being installed or
 * when system wizards are fooling around with \TeX\ without quite knowing
 * what they are doing. Such code will not normally be compiled; it is
 * delimited by the codewords `$|debug|\ldots|gubed|$', with apologies
 * to people who wish to preserve the purity of English.
 * 
 * Similarly, there is some conditional code delimited by
 * `$|stat|\ldots|tats|$' that is intended for use when statistics are to be
 * kept about \TeX's memory usage. The |stat| $\ldots$ |tats| code also
 * implements diagnostic information for \.{\\tracingparagraphs} and
 * \.{\\tracingpages}.
 */

/* module 8 */

/* This program has two important variations: (1) There is a long and slow
 * version called \.{INITEX}, which does the extra calculations needed to
 * 
 * initialize \TeX's internal tables; and (2)~there is a shorter and faster
 * production version, which cuts the initialization to a bare minimum.
 * Parts of the program that are needed in (1) but not in (2) are delimited by
 * the codewords `$|init|\ldots|tini|$' for declarations and by the codewords
 * `$|Init|\ldots|Tini|$' for executable code. This distinction is helpful for
 * implementations where a run-time switch differentiates between the two
 * versions of the program.
 */

/* module 9 */

/* If the first character of a \PASCAL\ comment is a dollar sign,
 * \ph\ treats the comment as a list of ``compiler directives'' that will
 * affect the translation of this program into machine language. The
 * directives shown below specify full checking and inclusion of the \PASCAL\
 * debugger when \TeX\ is being debugged, but they cause range checking and other
 * redundant code to be eliminated when the production system is being generated.
 * Arithmetic overflow will be detected in all cases.
 */

/* begin expansion of Global variables */

boolean ini_version; /* are we \.{INITEX}? */ 
integer main_memory; /* total memory words allocated in initex */
integer extra_mem_bot; /* |mem_min:=mem_bot-extra_mem_bot| except 
						  in \.{INITEX} */ 
integer mem_min; /* smallest index in \TeX's internal |mem| array; must be 
					|min_halfword| or more; must be equal to |mem_bot| in 
					\.{INITEX}, otherwise |<=mem_bot| */
integer mem_top; /* largest index in the |mem| array dumped by \.{INITEX}; 
		    must be substantially larger than |mem_bot|, equal to 
		    |mem_max| in \.{INITEX}, else not greater than |mem_max|*/
integer extra_mem_top; /* |mem_max:=mem_top+extra_mem_top| except 
						  in \.{INITEX} */ 
integer mem_max; /* greatest index in \TeX's internal |mem| array; must be 
					strictly less than |max_halfword|; must be equal to 
					|mem_top| in \.{INITEX}, otherwise |>=mem_top| */
integer font_mem_size; /* number of words of |font_info| for all fonts */ 
integer font_max; /* maximum internal font number; ok to exceed 
					 |max_quarterword| and must be at most |font_base|+
					 |max_font_max| */
integer font_k; /* loop variable for initialization */ 
integer hyph_size; /* maximun number of hyphen exceptions */ 
integer buf_size; /* maximum number of characters simultaneously present in 
					 current lines of open files and in control sequences 
					 between \.{\\csname} and \.{\\endcsname}; must not 
					 exceed |max_halfword| */
integer stack_size; /* maximum number of simultaneous input sources */ 
integer max_in_open; /* maximum number of input files and error insertions 
						that can be going on simultaneously */
integer param_size; /* maximum number of simultaneous macro parameters */ 
integer nest_size; /* maximum number of semantic levels simultaneously 
					  active */ 
integer save_size; /* space for saving values outside of current group; 
					  must be at most |max_halfword| */
integer dvi_buf_size; /* size of the output buffer; must be a multiple of 8*/ 

unsigned char k; /*TH: ^*(^&%%$$#. this is a scratch variable, but it is referenced
				   from pdftex's writettf.c */
/* end expansion of Global variables */


/* module 19 */

/* The original \PASCAL\ compiler was designed in the late 60s, when six-bit
 * character sets were common, so it did not make provision for lowercase
 * letters. Nowadays, of course, we need to deal with both capital and small
 * letters in a convenient way, especially in a program for typesetting;
 * so the present specification of \TeX\ has been written under the assumption
 * that the \PASCAL\ compiler and run-time system permit the use of text files
 * with more than 64 distinguishable characters. More precisely, we assume that
 * the character set contains at least the letters and symbols associated
 * with ASCII codes oct(40) through oct(176); all of these characters are now
 * available on most computer terminals.
 * 
 * Since we are dealing with more characters than were present in the first
 * \PASCAL\ compilers, we have to decide what to call the associated data
 * type. Some \PASCAL s use the original name |char| for the
 * characters in text files, even though there now are more than 64 such
 * characters, while other \PASCAL s consider |char| to be a 64-element
 * subrange of a larger data type that has some other name.
 * 
 * TH outdated:
 * In order to accommodate this difference, we shall use the name |text_char|
 * to stand for the data type of the characters that are converted to and
 * from |ASCII_code| when they are input and output. We shall also assume
 * that |text_char| consists of the elements |chr(first_text_char)| through
 * |chr(last_text_char)|, inclusive. The following definitions should be
 * adjusted if necessary.
 */


void
initialize (void) {				
  /* this procedure gets things started properly */
  /* begin expansion of Initialize whatever \TeX\ might access */
  /* module 8 */
  /* begin expansion of Set initial values of key variables */
  xordchr_initialize();
  error_initialize();
  help_initialize();
  mem_initialize();
  nest_initialize();
  start_new_page();
  eqtb_initialize();
  /*  hash_initialize();*/
  save_initialize();
  mag_initialize();
  mark_initialize();
  scan_initialize();
  tokenlist_initialize();
  if_initialize();
  font_initialize();
  dvi_initialize();
  pdfbasic_initialize();
  pdflowlevel_initialize();
  pdfxref_initialize();
  pdffont_initialize();
  vf_initialize();
  pack_initialize();
  adjust_initialize();
  math_initialize();
  align_initialize();
  trie_initialize();
  buildpage_initialize();
  control_initialize();
  prefix_initialize();
  dump_initialize();
  exten_initialize();
  snap_initialize();
  pdfproc_initialize();
  xet_initialize();
  etex_initialize();
  sa_initialize();
  vsplit_initialize();
  mltex_initialize();
  /* end expansion of Set initial values of key variables */
  if (ini_version) {
      /*  begin expansion of Initialize table entries (done by \.{INITEX} only) */
      mem_initialize_init();
      /* begin expansion of Initialize the special list heads and constant nodes */
	  align_initialize_init();
      linebreak_initialize_init();
      buildpage_initialize_init();
      /* end expansion of Initialize the special list heads and constant nodes */
      avail = null;
      mem_end = mem_top;
      hi_mem_min = hi_mem_stat_min; /* initialize the one-word memory */
      var_used = lo_mem_stat_max + 1 - mem_bot;
      dyn_used = hi_mem_stat_usage; /* initialize statistics */
	  eqtb_initialize_init();
	  hash_initialize_init();
	  trie_initialize_init();
      /* module 1361 */
      primitive_text (frozen_protection,"inaccessible");
      /* module 1446 */
      if (ini_version)
		format_ident = slow_make_tex_string(" (INITEX)");
	  /* module 1565 */
	  /* To write a token list, we must run it through \TeX's scanner, expanding
	   * macros and \.{\\the} and \.{\\number}, etc. This might cause runaways,
	   * if a delimited macro parameter isn't matched, and runaways would be
	   * extremely confusing since we are calling on \TeX's scanner in the middle
	   * of a \.{\\shipout} command. Therefore we will put a dummy control sequence as
	   * a ``stopper,'' right after the token list. This control sequence is
	   * artificially defined to be \.{\\outer}.
	   */
      primitive_text (end_write,"endwrite");
      eq_level (end_write) = level_one;
      eq_type (end_write) = outer_call;
      equiv (end_write) = null;
      /* module 1599 */
      eTeX_mode = 0; /* initially we are in compatibility mode */
      /* begin expansion of Initialize variables for \eTeX\ compatibility mode */
      /* module 1757 */
      max_reg_num = 255;
      max_reg_help_line = "A register number must be between 0 and 255.";
      /* end expansion of Initialize variables for \eTeX\ compatibility mode */
	  sa_initialize_init();
      /* end expansion of Initialize table entries (done by \.{INITEX} only) */
  }
  /* end expansion of Initialize whatever \TeX\ might access */
};


/* module 241 */

/* The following procedure, which is called just before \TeX\ initializes its
 * input and output, establishes the initial values of the date and time.
 * It calls a macro-defined |date_and_time| routine. |date_and_time|
 * in turn is a C macro, which calls |get_date_and_time|, passing
 * it the addresses of the day, month, etc., so they can be set by the
 * routine. |get_date_and_time| also sets up interrupt catching if that
 * is conditionally compiled in the C code.
 */
#define fix_date_and_time  \
  get_date_and_time (address_of(tex_time), address_of(day), address_of(month), address_of(year))

/* module 1475 */

/* 
 * This is it: the part of \TeX\ that executes all those procedures we have
 * written.
 * 
 * Well---almost. Let's leave space for a few more routines that we may
 * have forgotten.
 */

/* module 1480 */

/* We get to the |final_cleanup| routine when \.{\\end} or \.{\\dump} has
 * been scanned and |its_all_over|\kern-2pt.
 */
void final_cleanup (void) {
  small_number c; /* 0 for \.{\\end}, 1 for \.{\\dump} */ 
  c = cur_chr;
  if (jobname == 0)
	open_log_file();
  while (input_ptr > 0)
	if (state == token_list) {
	  end_token_list();
	} else {
	  end_file_reading();
	}
  while (open_parens > 0) {
	zprint_string(" )");
	decr (open_parens);
  };
  if (cur_level > level_one) {
	print_nl_string ("(");
	print_esc_string ("end occurred ");
	zprint_string("inside a group at level ");
	print_int (cur_level - level_one);
	print_char (')');
	if (eTeX_ex)
	  show_save_groups();
  };
  while (cond_ptr != null) {
	print_nl_string ("(");
	print_esc_string ("end occurred ");
	zprint_string("when ");
	print_cmd_chr (if_test, cur_if);
	if (if_line != 0) {
	  zprint_string(" on line ");
	  print_int (if_line);
	};
	zprint_string(" was incomplete)");
	if_line = if_line_field (cond_ptr);
	cur_if = subtype (cond_ptr);
	temp_ptr = cond_ptr;
	cond_ptr = link (cond_ptr);
	free_node (temp_ptr, if_node_size);
  };
  if (history != spotless)
	if (((history == warning_issued) || (interaction < error_stop_mode)))
	  if (selector == term_and_log) {
		selector = term_only;
		print_nl_string ("(see the transcript file for additional information)");
		selector = term_and_log;
	  };
  if (c == 1) {
	if (ini_version) {
	  for (c = top_mark_code; c <= split_bot_mark_code; c++)
		if (cur_mark[c] != null)
		  delete_token_ref (cur_mark[c]);
	  if (sa_mark != null)
		if (do_marks (destroy_marks, 0, sa_mark))
		  sa_mark = null;
	  for (c = last_box_code; c <= vsplit_code; c++)
		flush_node_list (disc_ptr[c]);
	  store_fmt_file();
	  return;
	}
	print_nl_string ("(\\dump is performed only by INITEX)");
	return;
  };
};




/* module 1481 */


/* there was an ifdef INIT here, but the procedure is
 * always needed, because of the string inits for the primitive
 * names */

void init_prim (int noninit) {	/* initialize all the primitives */
  set_no_new_control_sequence(false);
  first = 0;
  /* begin expansion of Put each... */
  /* module 226 */
  /* The symbolic names for glue parameters are put into \TeX's hash table
   * by using the routine called |primitive|, defined below. Let us enter them
   * now, so that we don't have to list all those parameter names anywhere else.
   */
  primitive_str("lineskip", assign_glue, glue_base + line_skip_code);
  primitive_str("baselineskip", assign_glue, glue_base + baseline_skip_code);
  primitive_str("parskip", assign_glue, glue_base + par_skip_code);
  primitive_str("abovedisplayskip", assign_glue, glue_base + above_display_skip_code);
  primitive_str("belowdisplayskip", assign_glue, glue_base + below_display_skip_code);
  primitive_str("abovedisplayshortskip", assign_glue,glue_base + above_display_short_skip_code);
  primitive_str("belowdisplayshortskip", assign_glue,glue_base + below_display_short_skip_code);
  primitive_str("leftskip", assign_glue,glue_base + left_skip_code);
  primitive_str("rightskip", assign_glue,glue_base + right_skip_code);
  primitive_str("topskip", assign_glue,glue_base + top_skip_code);
  primitive_str("splittopskip", assign_glue,glue_base + split_top_skip_code);
  primitive_str("tabskip", assign_glue,glue_base + tab_skip_code);
  primitive_str("spaceskip", assign_glue,glue_base + space_skip_code);
  primitive_str("xspaceskip", assign_glue,glue_base + xspace_skip_code);
  primitive_str("parfillskip", assign_glue,glue_base + par_fill_skip_code);
  primitive_str("thinmuskip", assign_mu_glue,glue_base + thin_mu_skip_code);
  primitive_str("medmuskip", assign_mu_glue,glue_base + med_mu_skip_code);
  primitive_str("thickmuskip", assign_mu_glue, glue_base + thick_mu_skip_code);
  /* module 230 */
  /* Region 4 of |eqtb| contains the local quantities defined here. The
   * bulk of this region is taken up by five tables that are indexed by eight-bit
   * characters; these tables are important to both the syntactic and semantic
   * portions of \TeX. There are also a bunch of special things like font and
   * token parameters, as well as the tables of \.{\\toks} and \.{\\box}
   * registers.
   */
  primitive_str("output", assign_toks, output_routine_loc);
  primitive_str("everypar", assign_toks, every_par_loc);
  primitive_str("everymath", assign_toks, every_math_loc);
  primitive_str("everydisplay", assign_toks,every_display_loc);
  primitive_str("everyhbox", assign_toks,every_hbox_loc);
  primitive_str("everyvbox", assign_toks,every_vbox_loc);
  primitive_str("everyjob", assign_toks, every_job_loc);
  primitive_str("everycr", assign_toks, every_cr_loc);
  primitive_str("pdfpagesattr", assign_toks,pdf_pages_attr_loc);
  primitive_str("pdfpageattr", assign_toks,pdf_page_attr_loc);
  primitive_str("pdfpageresources", assign_toks,pdf_page_resources_loc);
  primitive_str("errhelp", assign_toks, err_help_loc);
  /* module 238 */
  /* The integer parameter names must be entered into the hash table.
   */
  primitive_str("pretolerance", assign_int,int_base + pretolerance_code);
  primitive_str("tolerance", assign_int,int_base + tolerance_code);
  primitive_str("linepenalty", assign_int,int_base + line_penalty_code);
  primitive_str("hyphenpenalty", assign_int,int_base + hyphen_penalty_code);
  primitive_str("exhyphenpenalty", assign_int,int_base + ex_hyphen_penalty_code);
  primitive_str("clubpenalty", assign_int,int_base + club_penalty_code);
  primitive_str("widowpenalty", assign_int,int_base + widow_penalty_code);
  primitive_str("displaywidowpenalty", assign_int,int_base + display_widow_penalty_code);
  primitive_str("brokenpenalty", assign_int,int_base + broken_penalty_code);
  primitive_str("binoppenalty", assign_int,int_base + bin_op_penalty_code);
  primitive_str("relpenalty", assign_int,int_base + rel_penalty_code);
  primitive_str("predisplaypenalty", assign_int, int_base + pre_display_penalty_code);
  primitive_str("postdisplaypenalty", assign_int, int_base + post_display_penalty_code);
  primitive_str("interlinepenalty", assign_int,int_base + inter_line_penalty_code);
  primitive_str("doublehyphendemerits", assign_int,int_base + double_hyphen_demerits_code);
  primitive_str("finalhyphendemerits", assign_int, int_base + final_hyphen_demerits_code);
  primitive_str("adjdemerits", assign_int,int_base + adj_demerits_code);
  primitive_str("mag", assign_int, int_base + mag_code);
  primitive_str("delimiterfactor", assign_int,int_base + delimiter_factor_code);
  primitive_str("looseness", assign_int,int_base + looseness_code);
  primitive_str("time", assign_int,int_base + time_code);
  primitive_str("day", assign_int, int_base + day_code);
  primitive_str("month", assign_int,int_base + month_code);
  primitive_str("year", assign_int,int_base + year_code);
  primitive_str("showboxbreadth", assign_int,int_base + show_box_breadth_code);
  primitive_str("showboxdepth", assign_int,int_base + show_box_depth_code);
  primitive_str("hbadness", assign_int,int_base + hbadness_code);
  primitive_str("vbadness", assign_int,int_base + vbadness_code);
  primitive_str("pausing", assign_int, int_base + pausing_code);
  primitive_str("tracingonline", assign_int, int_base + tracing_online_code);
  primitive_str("tracingmacros", assign_int, int_base + tracing_macros_code);
  primitive_str("tracingstats", assign_int, int_base + tracing_stats_code);
  primitive_str("tracingparagraphs", assign_int,int_base + tracing_paragraphs_code);
  primitive_str("tracingpages", assign_int, int_base + tracing_pages_code);
  primitive_str("tracingoutput", assign_int,int_base + tracing_output_code);
  primitive_str("tracinglostchars", assign_int,int_base + tracing_lost_chars_code);
  primitive_str("tracingcommands", assign_int,int_base + tracing_commands_code);
  primitive_str("tracingrestores", assign_int,int_base + tracing_restores_code);
  primitive_str("uchyph", assign_int, int_base + uc_hyph_code);
  primitive_str("outputpenalty", assign_int,int_base + output_penalty_code);
  primitive_str("maxdeadcycles", assign_int,int_base + max_dead_cycles_code);
  primitive_str("hangafter", assign_int,int_base + hang_after_code);
  primitive_str("floatingpenalty", assign_int,int_base + floating_penalty_code);
  primitive_str("globaldefs", assign_int,int_base + global_defs_code);
  primitive_str("fam", assign_int,int_base + cur_fam_code);
  primitive_str("escapechar", assign_int,int_base + escape_char_code);
  primitive_str("defaulthyphenchar", assign_int,int_base + default_hyphen_char_code);
  primitive_str("defaultskewchar", assign_int,int_base + default_skew_char_code);
  primitive_str("endlinechar", assign_int,int_base + end_line_char_code);
  primitive_str("newlinechar", assign_int,int_base + new_line_char_code);
  primitive_str("language", assign_int, int_base + language_code);
  primitive_str("lefthyphenmin", assign_int,int_base + left_hyphen_min_code);
  primitive_str("righthyphenmin", assign_int,int_base + right_hyphen_min_code);
  primitive_str("holdinginserts", assign_int,int_base + holding_inserts_code);
  primitive_str("errorcontextlines", assign_int,int_base + error_context_lines_code);
  if (mltex_p) {
	mltex_enabled_p = true; /* enable character substitution */ 
#ifdef ENABLE_CHARSUBDEFMIN
	/* remove the if-clause to enable \.{\\charsubdefmin} */
	  primitive_str("charsubdefmin", assign_int,int_base + char_sub_def_min_code); 
#endif
	primitive_str("charsubdefmax", assign_int,int_base + char_sub_def_max_code);
	primitive_str("tracingcharsubdef", assign_int,int_base + tracing_char_sub_def_code);
  };
  primitive_str("pdfoutput", assign_int,int_base + pdf_output_code);
  primitive_str("pdfadjustspacing", assign_int,int_base + pdf_adjust_spacing_code);
  primitive_str("pdfcompresslevel", assign_int,int_base + pdf_compress_level_code);
  primitive_str("pdfdecimaldigits", assign_int,int_base + pdf_decimal_digits_code);
  primitive_str("pdfmovechars", assign_int,int_base + pdf_move_chars_code);
  primitive_str("pdfimageresolution", assign_int,int_base + pdf_image_resolution_code);
  primitive_str("pdfpkresolution", assign_int,int_base + pdf_pk_resolution_code);
  primitive_str("pdfuniqueresname", assign_int,int_base + pdf_unique_resname_code);
  primitive_str("pdfprotrudechars", assign_int,int_base + pdf_protrude_chars_code);
  primitive_str("pdfavoidoverfull", assign_int,int_base + pdf_avoid_overfull_code);
  primitive_str("pdfmaxpenalty", assign_int,int_base + pdf_max_penalty_code);
  primitive_str("pdfminpenalty", assign_int,int_base + pdf_min_penalty_code);
  primitive_str("pdfoptionpdfminorversion",assign_int,int_base + pdf_option_pdf_minor_version_code);
  primitive_str("pdfoptionalwaysusepdfpagebox",assign_int,int_base +pdf_option_always_use_pdfpagebox_code);
  primitive_str("pdfoptionpdfinclusionerrorlevel",assign_int,int_base +pdf_option_pdf_inclusion_errorlevel_code);
  /* module 248 */
  primitive_str("parindent", assign_dimen,dimen_base + par_indent_code);
  primitive_str("mathsurround", assign_dimen,dimen_base + math_surround_code);
  primitive_str("lineskiplimit", assign_dimen,dimen_base + line_skip_limit_code);
  primitive_str("hsize", assign_dimen,dimen_base + hsize_code);
  primitive_str("vsize", assign_dimen, dimen_base + vsize_code);
  primitive_str("maxdepth", assign_dimen, dimen_base + max_depth_code);
  primitive_str("splitmaxdepth", assign_dimen,dimen_base + split_max_depth_code);
  primitive_str("boxmaxdepth", assign_dimen, dimen_base + box_max_depth_code);
  primitive_str("hfuzz", assign_dimen, dimen_base + hfuzz_code);
  primitive_str("vfuzz", assign_dimen, dimen_base + vfuzz_code);
  primitive_str("delimitershortfall", assign_dimen, dimen_base + delimiter_shortfall_code);
  primitive_str("nulldelimiterspace", assign_dimen, dimen_base + null_delimiter_space_code);
  primitive_str("scriptspace", assign_dimen, dimen_base + script_space_code);
  primitive_str("predisplaysize", assign_dimen, dimen_base + pre_display_size_code);
  primitive_str("displaywidth", assign_dimen, dimen_base + display_width_code);
  primitive_str("displayindent", assign_dimen, dimen_base + display_indent_code);
  primitive_str("overfullrule", assign_dimen, dimen_base + overfull_rule_code);
  primitive_str("hangindent", assign_dimen, dimen_base + hang_indent_code);
  primitive_str("hoffset", assign_dimen, dimen_base + h_offset_code);
  primitive_str("voffset", assign_dimen, dimen_base + v_offset_code);
  primitive_str("emergencystretch", assign_dimen, dimen_base + emergency_stretch_code);
  primitive_str("pdfhorigin", assign_dimen, dimen_base + pdf_h_origin_code);
  primitive_str("pdfvorigin", assign_dimen, dimen_base + pdf_v_origin_code);
  primitive_str("pdfpagewidth", assign_dimen, dimen_base + pdf_page_width_code);
  primitive_str("pdfpageheight", assign_dimen, dimen_base + pdf_page_height_code);
  primitive_str("pdflinkmargin", assign_dimen, dimen_base + pdf_link_margin_code);
  primitive_str("pdfdestmargin", assign_dimen, dimen_base + pdf_dest_margin_code);
  primitive_str("pdfthreadmargin", assign_dimen, dimen_base + pdf_thread_margin_code);
  /* module 265 */
  /* Many of \TeX's primitives need no |equiv|, since they are identifiable
   * by their |eq_type| alone. These primitives are loaded into the hash table
   * as follows:
   */
  if(!noninit) {
	primitive(' ', ex_space, 0);
	primitive('/', ital_corr, 0);
  }
  primitive_str("accent", accent, 0);
  primitive_str("advance", advance, 0);
  primitive_str("afterassignment", after_assignment, 0);
  primitive_str("aftergroup", after_group, 0);
  primitive_str("begingroup", begin_group, 0);
  primitive_str("char", char_num, 0);
  primitive_str("csname", cs_name, 0);
  primitive_str("delimiter", delim_num, 0);
  primitive_str("divide", divide, 0);
  primitive_str("endcsname", end_cs_name, 0);
  primitive_str("endgroup", end_group, 0);
  if(!noninit) {
	primitive_text (frozen_end_group,"endgroup");
	eqtb[frozen_end_group] = eqtb[cur_val];
  }
  primitive_str("expandafter", expand_after, 0);
  primitive_str("font", def_font, 0);
  primitive_str("fontdimen", assign_font_dimen, 0);
  primitive_str("halign", halign, 0);
  primitive_str("hrule", hrule, 0);
  primitive_str("ignorespaces", ignore_spaces, 0);
  primitive_str("insert", insert, 0);
  primitive_str("mark", mark, 0);
  primitive_str("mathaccent", math_accent, 0);
  primitive_str("mathchar", math_char_num, 0);
  primitive_str("mathchoice", math_choice, 0);
  primitive_str("multiply", multiply, 0);
  primitive_str("noalign", no_align, 0);
  primitive_str("noboundary", no_boundary, 0);
  primitive_str("noexpand", no_expand, 0);
  primitive_str("nonscript", non_script, 0);
  primitive_str("omit", omit, 0);
  primitive_str("parshape", set_shape, par_shape_loc);
  primitive_str("penalty", break_penalty, 0);
  primitive_str("prevgraf", set_prev_graf, 0);
  primitive_str("radical", radical, 0);
  primitive_str("read", read_to_cs, 0);
  primitive_str("relax", relax, 256); /* cf.\ |scan_file_name| */ 
  if(!noninit) {
	primitive_text (frozen_relax,"relax");
	eqtb[frozen_relax] = eqtb[cur_val];
  }
  primitive_str("setbox", set_box, 0);
  primitive_str("the", the, 0);
  primitive_str("toks", toks_register, mem_bot);
  primitive_str("vadjust", vadjust, 0);
  primitive_str("valign", valign, 0);
  primitive_str("vcenter", vcenter, 0);
  primitive_str("vrule", vrule, 0);
  /* module 334 */
  primitive_str("par", par_end, 256); /* cf. |scan_file_name| */ 
  if(!noninit) {
	par_loc = cur_val;
	par_token = cs_token_flag + par_loc;
  }
  /* module 376 */
  /* The processing of \.{\\input} involves the |start_input| subroutine,
   * which will be declared later; the processing of \.{\\endinput} is trivial.
   */
  primitive_str("input", input, 0);
  primitive_str("endinput", input, 1);
  /* module 384 */
  primitive_str("topmark", top_bot_mark, top_mark_code);
  primitive_str("firstmark", top_bot_mark, first_mark_code);
  primitive_str("botmark", top_bot_mark, bot_mark_code);
  primitive_str("splitfirstmark", top_bot_mark, split_first_mark_code);
  primitive_str("splitbotmark", top_bot_mark, split_bot_mark_code);
  /* module 411 */
  /* The hash table is initialized with `\.{\\count}', `\.{\\dimen}', `\.{\\skip}',
   * and `\.{\\muskip}' all having |register| as their command code; they are
   * distinguished by the |chr_code|, which is either |int_val|, |dimen_val|,
   * |glue_val|, or |mu_val| more than |mem_bot| (dynamic variable-size nodes
   * cannot have these values)
   */
  primitive_str("count", register_cmd, mem_bot + int_val);
  primitive_str("dimen", register_cmd, mem_bot + dimen_val);
  primitive_str("skip", register_cmd, mem_bot + glue_val);
  primitive_str("muskip", register_cmd, mem_bot + mu_val);
  /* module 416 */
  /* Users refer to `\.{\\the\\spacefactor}' only in horizontal
   * mode, and to `\.{\\the\\prevdepth}' only in vertical mode; so we put the
   * associated mode in the modifier part of the |set_aux| command.
   * The |set_page_int| command has modifier 0 or 1, for `\.{\\deadcycles}' and
   * `\.{\\insertpenalties}', respectively. The |set_box_dimen| command is
   * modified by either |width_offset|, |height_offset|, or |depth_offset|.
   * And the |last_item| command is modified by either |int_val|, |dimen_val|,
   * |glue_val|, |input_line_no_code|, |badness_code|, or one of the other codes for
   * \pdfTeX\ or \eTeX\ extensions.
   */
  primitive_str("spacefactor", set_aux, hmode);
  primitive_str("prevdepth", set_aux, vmode);
  primitive_str("deadcycles", set_page_int, 0);
  primitive_str("insertpenalties", set_page_int, 1);
  primitive_str("wd", set_box_dimen, width_offset);
  primitive_str("ht", set_box_dimen, height_offset);
  primitive_str("dp", set_box_dimen, depth_offset);
  primitive_str("lastpenalty", last_item, int_val);
  primitive_str("lastkern", last_item, dimen_val);
  primitive_str("lastskip", last_item, glue_val);
  primitive_str("inputlineno", last_item,input_line_no_code);
  primitive_str("badness", last_item, badness_code);
  primitive_str("pdftexversion", last_item,pdftex_version_code);
  primitive_str("pdflastobj", last_item,pdf_last_obj_code);
  primitive_str("pdflastxform", last_item,pdf_last_xform_code);
  primitive_str("pdflastximage", last_item,pdf_last_ximage_code);
  primitive_str("pdflastximagepages", last_item,pdf_last_ximage_pages_code);
  primitive_str("pdflastannot", last_item,pdf_last_annot_code);
  primitive_str("pdflastxpos", last_item,pdf_last_x_pos_code);
  primitive_str("pdflastypos", last_item,pdf_last_y_pos_code);
  primitive_str("pdflastdemerits", last_item,pdf_last_demerits_code);
  primitive_str("pdflastvbreakpenalty", last_item,pdf_last_vbreak_penalty_code);
  primitive_str("pdftexrevision", convert,pdftex_revision_code);
  primitive_str("pdffontname", convert,pdf_font_name_code);
  primitive_str("pdffontobjnum", convert,pdf_font_objnum_code);
  primitive_str("pdffontsize", convert,pdf_font_size_code);
  /* module 468 */
  /* The primitives \.{\\number}, \.{\\romannumeral}, \.{\\string}, \.{\\meaning},
   * \.{\\fontname}, and \.{\\jobname} are defined as follows.
   */
  primitive_str("number", convert, number_code);
  primitive_str("romannumeral", convert,roman_numeral_code);
  primitive_str("string", convert, string_code);
  primitive_str("meaning", convert, meaning_code);
  primitive_str("fontname", convert, font_name_code);
  primitive_str("jobname", convert, job_name_code);
  /* module 487 */
  /* We consider now the way \TeX\ handles various kinds of \.{\\if} commands.
   */
  primitive_str("if", if_test, if_char_code);
  primitive_str("ifcat", if_test, if_cat_code);
  primitive_str("ifnum", if_test, if_int_code);
  primitive_str("ifdim", if_test, if_dim_code);
  primitive_str("ifodd", if_test, if_odd_code);
  primitive_str("ifvmode", if_test, if_vmode_code);
  primitive_str("ifhmode", if_test, if_hmode_code);
  primitive_str("ifmmode", if_test, if_mmode_code);
  primitive_str("ifinner", if_test, if_inner_code);
  primitive_str("ifvoid", if_test, if_void_code);
  primitive_str("ifhbox", if_test, if_hbox_code);
  primitive_str("ifvbox", if_test, if_vbox_code);
  primitive_str("ifx", if_test, ifx_code);
  primitive_str("ifeof", if_test, if_eof_code);
  primitive_str("iftrue", if_test, if_true_code);
  primitive_str("iffalse", if_test, if_false_code);
  primitive_str("ifcase", if_test, if_case_code);
  /* module 491 */
  primitive_str("fi", fi_or_else, fi_code);
  if(!noninit) {
	primitive_text (frozen_fi,"fi");
	eqtb[frozen_fi] = eqtb[cur_val];
  }
  primitive_str("or", fi_or_else, or_code);
  primitive_str("else", fi_or_else, else_code);
  /* module 553 */
  primitive_str("nullfont", set_font, null_font);
  if(!noninit) {
	primitive_text (frozen_null_font,"nullfont");
	eqtb[frozen_null_font] = eqtb[cur_val];
  }
  /* module 924 */
  /* We enter `\.{\\span}' into |eqtb| with |tab_mark| as its command code,
   * and with |span_code| as the command modifier. This makes \TeX\ interpret it
   * essentially the same as an alignment delimiter like `\.\&', yet it is
   * recognizably different when we need to distinguish it from a normal delimiter.
   * It also turns out to be useful to give a special |cr_code| to `\.{\\cr}',
   * and an even larger |cr_cr_code| to `\.{\\crcr}'.
   * 
   * The end of a template is represented by two ``frozen'' control sequences
   * called \.{\\endtemplate}. The first has the command code |end_template|, which
   * is |>outer_call|, so it will not easily disappear in the presence of errors.
   * The |get_x_token| routine converts the first into the second, which has |endv|
   * as its command code.
   */
  primitive_str("span", tab_mark, span_code);
  primitive_str("cr", car_ret, cr_code);
  if(!noninit) {
	primitive_text (frozen_cr,"cr");
	eqtb[frozen_cr] = eqtb[cur_val];
  }
  primitive_str("crcr", car_ret, cr_cr_code);
  if(!noninit) {
	primitive_text (frozen_end_template,"endtemplate");
	primitive_text (frozen_endv,"endtemplate");
	eq_type (frozen_endv) = endv;
	equiv (frozen_endv) = null_list;
	eq_level (frozen_endv) = level_one;
	eqtb[frozen_end_template] = eqtb[frozen_endv];
	eq_type (frozen_end_template) = end_template;
  }
  /* module 1128 */
  primitive_str("pagegoal", set_page_dimen, 0);
  primitive_str("pagetotal", set_page_dimen, 1);
  primitive_str("pagestretch", set_page_dimen, 2);
  primitive_str("pagefilstretch", set_page_dimen, 3);
  primitive_str("pagefillstretch", set_page_dimen, 4);
  primitive_str("pagefilllstretch", set_page_dimen, 5);
  primitive_str("pageshrink", set_page_dimen, 6);
  primitive_str("pagedepth", set_page_dimen, 7);
  /* module 1197 */
  /* Either \.{\\dump} or \.{\\end} will cause |main_control| to enter the
   * endgame, since both of them have `|stop|' as their command code.
   */
  primitive_str("end", stop, 0);
  primitive_str("dump", stop, 1);
  /* module 1203 */
  /* The |hskip| and |vskip| command codes are used for control sequences
   * like \.{\\hss} and \.{\\vfil} as well as for \.{\\hskip} and \.{\\vskip}.
   * The difference is in the value of |cur_chr|.
   */
  primitive_str("hskip", hskip, skip_code);
  primitive_str("hfil", hskip, fil_code);
  primitive_str("hfill", hskip, fill_code);
  primitive_str("hss", hskip, ss_code);
  primitive_str("hfilneg", hskip, fil_neg_code);
  primitive_str("vskip", vskip, skip_code);
  primitive_str("vfil", vskip, fil_code);
  primitive_str("vfill", vskip, fill_code);
  primitive_str("vss", vskip, ss_code);
  primitive_str("vfilneg", vskip, fil_neg_code);
  primitive_str("mskip", mskip, mskip_code);
  primitive_str("kern", kern, explicit);
  primitive_str("mkern", mkern, mu_glue);
  /* module 1216 */
  /* Now let's turn to the question of how \.{\\hbox} is treated. We actually
   * need to consider also a slightly larger context, since constructions like
   * `\.{\\setbox3=}\penalty0\.{\\hbox...}' and
   * `\.{\\leaders}\penalty0\.{\\hbox...}' and
   * `\.{\\lower3.8pt\\hbox...}'
   * are supposed to invoke quite
   * different actions after the box has been packaged. Conversely,
   * constructions like `\.{\\setbox3=}' can be followed by a variety of
   * different kinds of boxes, and we would like to encode such things in an
   * efficient way.
   * 
   * In other words, there are two problems: To represent the context of a box,
   * and to represent its type.
   * 
   * The first problem is solved by putting a ``context code'' on the |save_stack|,
   * just below the two entries that give the dimensions produced by |scan_spec|.
   * The context code is either a (signed) shift amount, or it is a large
   * integer |>=box_flag|, where |box_flag=@t$2^{30}$@>|. Codes |box_flag| through
   * |global_box_flag-1| represent `\.{\\setbox0}' through `\.{\\setbox32767}';
   * codes |global_box_flag| through |ship_out_flag-1| represent
   * `\.{\\global\\setbox0}' through `\.{\\global\\setbox32767}';
   * code |ship_out_flag| represents `\.{\\shipout}'; and codes |leader_flag|
   * through |leader_flag+2| represent `\.{\\leaders}', `\.{\\cleaders}',
   * and `\.{\\xleaders}'.
   * 
   * The second problem is solved by giving the command code |make_box| to all
   * control sequences that produce a box, and by using the following |chr_code|
   * values to distinguish between them: |box_code|, |copy_code|, |last_box_code|,
   * |vsplit_code|, |vtop_code|, |vtop_code+vmode|, and |vtop_code+hmode|,
   * where the latter two are used denote \.{\\vbox} and \.{\\hbox}, respectively.
   */
  primitive_str("moveleft", hmove, 1);
  primitive_str("moveright", hmove, 0);
  primitive_str("raise", vmove, 1);
  primitive_str("lower", vmove, 0);
  primitive_str("box", make_box, box_code);
  primitive_str("copy", make_box, copy_code);
  primitive_str("lastbox", make_box, last_box_code);
  primitive_str("vsplit", make_box, vsplit_code);
  primitive_str("vtop", make_box, vtop_code);
  primitive_str("vbox", make_box, vtop_code + vmode);
  primitive_str("hbox", make_box, vtop_code + hmode);
  primitive_str("shipout", leader_ship, a_leaders - 1); /* |ship_out_flag=leader_flag-1| */
  primitive_str("leaders", leader_ship, a_leaders);
  primitive_str("cleaders", leader_ship, c_leaders);
  primitive_str("xleaders", leader_ship, x_leaders);
  /* module 1233 */
  /* A paragraph begins when horizontal-mode material occurs in vertical mode,
   * or when the paragraph is explicitly started by `\.{\\indent}' or
   * `\.{\\noindent}'.
   */
  primitive_str("indent", start_par, 1);
  primitive_str("noindent", start_par, 0);
  /* module 1252 */
  primitive_str("unpenalty", remove_item, penalty_node);
  primitive_str("unkern", remove_item, kern_node);
  primitive_str("unskip", remove_item, glue_node);
  primitive_str("unhbox", un_hbox, box_code);
  primitive_str("unhcopy", un_hbox, copy_code);
  primitive_str("unvbox", un_vbox, box_code);
  primitive_str("unvcopy", un_vbox, copy_code);
  /* module 1259 */
  /* Discretionary nodes are easy in the common case `\.{\\-}', but in the
   * general case we must process three braces full of items.
   */
  if(!noninit) {
	primitive('-', discretionary, 1);
  }
  primitive_str("discretionary", discretionary, 0);
  /* module 1286 */
  primitive_str("eqno", eq_no, 0);
  primitive_str("leqno", eq_no, 1);
  /* module 1301 */
  /* Primitive math operators like \.{\\mathop} and \.{\\underline} are given
   * the command code |math_comp|, supplemented by the noad type that they
   * generate.
   */
  primitive_str("mathord", math_comp, ord_noad);
  primitive_str("mathop", math_comp, op_noad);
  primitive_str("mathbin", math_comp, bin_noad);
  primitive_str("mathrel", math_comp, rel_noad);
  primitive_str("mathopen", math_comp, open_noad);
  primitive_str("mathclose", math_comp, close_noad);
  primitive_str("mathpunct", math_comp, punct_noad);
  primitive_str("mathinner", math_comp, inner_noad);
  primitive_str("underline", math_comp, under_noad);
  primitive_str("overline", math_comp, over_noad);
  primitive_str("displaylimits", limit_switch, normal);
  primitive_str("limits", limit_switch, limits);
  primitive_str("nolimits", limit_switch, no_limits);
  /* module 1314 */
  /* The routine that inserts a |style_node| holds no surprises.
   */
  primitive_str("displaystyle", math_style, display_style);
  primitive_str("textstyle", math_style, text_style);
  primitive_str("scriptstyle", math_style, script_style);
  primitive_str("scriptscriptstyle", math_style,script_script_style);
  /* module 1323 */
  /* An operation like `\.{\\over}' causes the current mlist to go into a
   * state of suspended animation: |incompleat_noad| points to a |fraction_noad|
   * that contains the mlist-so-far as its numerator, while the denominator
   * is yet to come. Finally when the mlist is finished, the denominator will
   * go into the incompleat fraction noad, and that noad will become the
   * whole formula, unless it is surrounded by `\.{\\left}' and `\.{\\right}'
   * delimiters.
   */
  primitive_str("above", above, above_code);
  primitive_str("over", above, over_code);
  primitive_str("atop", above, atop_code);
  primitive_str("abovewithdelims", above,delimited_code + above_code);
  primitive_str("overwithdelims", above,delimited_code + over_code);
  primitive_str("atopwithdelims", above,delimited_code + atop_code);
  /* module 1333 */
  /* We have dealt with all constructions of math mode except `\.{\\left}' and
   * `\.{\\right}', so the picture is completed by the following sections of
   * the program.
   */
  primitive_str("left", left_right, left_noad);
  primitive_str("right", left_right, right_noad);
  if(!noninit) {
	primitive_text (frozen_right, "right");
	eqtb[frozen_right] = eqtb[cur_val];
  }
  /* module 1353 */
  /* 
   * The long |main_control| procedure has now been fully specified, except for
   * certain activities that are independent of the current mode. These activities
   * do not change the current vlist or hlist or mlist; if they change anything,
   * it is the value of a parameter or the meaning of a control sequence.
   * 
   * Assignments to values in |eqtb| can be global or local. Furthermore, a
   * control sequence can be defined to be `\.{\\long}', `\.{\\protected}',
   * or `\.{\\outer}', and it might or might not be expanded. The prefixes
   * `\.{\\global}', `\.{\\long}', `\.{\\protected}',
   * and `\.{\\outer}' can occur in any order. Therefore we assign binary numeric
   * codes, making it possible to accumulate the union of all specified prefixes
   * by adding the corresponding codes. (\PASCAL's |set| operations could also
   * have been used.)
   */
  primitive_str("long", prefix, 1);
  primitive_str("outer", prefix, 2);
  primitive_str("global", prefix, 4);
  primitive_str("def", def, 0);
  primitive_str("gdef", def, 1);
  primitive_str("edef", def, 2);
  primitive_str("xdef", def, 3);
  /* module 1364 */
  /* Both \.{\\let} and \.{\\futurelet} share the command code |let|.
   */
  primitive_str("let", let, normal);
  primitive_str("futurelet", let, normal + 1);
  /* module 1367 */
  /* A \.{\\chardef} creates a control sequence whose |cmd| is |char_given|;
   * a \.{\\mathchardef} creates a control sequence whose |cmd| is |math_given|;
   * and the corresponding |chr| is the character code or math code. A \.{\\countdef}
   * or \.{\\dimendef} or \.{\\skipdef} or \.{\\muskipdef} creates a control
   * sequence whose |cmd| is |assign_int| or \dots\ or |assign_mu_glue|, and the
   * corresponding |chr| is the |eqtb| location of the internal register in question.
   */
  primitive_str("chardef", shorthand_def, char_def_code);
  primitive_str("mathchardef", shorthand_def,math_char_def_code);
  primitive_str("countdef", shorthand_def,count_def_code);
  primitive_str("dimendef", shorthand_def,dimen_def_code);
  primitive_str("skipdef", shorthand_def, skip_def_code);
  primitive_str("muskipdef", shorthand_def,mu_skip_def_code);
  primitive_str("toksdef", shorthand_def, toks_def_code);
  if (mltex_p) {
	primitive_str("charsubdef", shorthand_def,char_sub_def_code);
  };
  /* module 1375 */
  /* The various character code tables are changed by the |def_code| commands,
   * and the font families are declared by |def_family|.
   */
  primitive_str("catcode", def_code, cat_code_base);
  primitive_str("mathcode", def_code, math_code_base);
  primitive_str("lccode", def_code, lc_code_base);
  primitive_str("uccode", def_code, uc_code_base);
  primitive_str("sfcode", def_code, sf_code_base);
  primitive_str("delcode", def_code, del_code_base);
  primitive_str("textfont", def_family, math_font_base);
  primitive_str("scriptfont", def_family,math_font_base + script_size);
  primitive_str("scriptscriptfont", def_family,math_font_base + script_script_size);
  /* module 1395 */
  /* New hyphenation data is loaded by the |hyph_data| command.
   */
  primitive_str("hyphenation", hyph_data, 0);
  primitive_str("patterns", hyph_data, 1);
  /* module 1399 */
  primitive_str("hyphenchar", assign_font_int, 0);
  primitive_str("skewchar", assign_font_int, 1);
  primitive_str("lpcode", assign_font_int, lp_code_base);
  primitive_str("rpcode", assign_font_int, rp_code_base);
  primitive_str("efcode", assign_font_int, ef_code_base);
  /* module 1407 */
  primitive_str("batchmode", set_interaction, batch_mode);
  primitive_str("nonstopmode", set_interaction, nonstop_mode);
  primitive_str("scrollmode", set_interaction, scroll_mode);
  primitive_str("errorstopmode", set_interaction, error_stop_mode);
  /* module 1417 */
  /* Files for \.{\\read} are opened and closed by the |in_stream| command.
   */
  primitive_str("openin", in_stream, 1);
  primitive_str("closein", in_stream, 0);
  /* module 1422 */
  primitive_str("message", message, 0);
  primitive_str("errmessage", message, 1);
  /* module 1431 */
  primitive_str("lowercase", case_shift, lc_code_base);
  primitive_str("uppercase", case_shift, uc_code_base);
  /* module 1436 */
  primitive_str("show", xray, show_code);
  primitive_str("showbox", xray, show_box_code);
  primitive_str("showthe", xray, show_the_code);
  primitive_str("showlists", xray, show_lists);
  /* module 1489 */
  /* Extensions might introduce new command codes; but it's best to use
   * |extension| with a modifier, whenever possible, so that |main_control|
   * stays the same.
   */
  primitive_str("openout", extension, open_node);
  primitive_str("write", extension, write_node);
  if(!noninit) {
	write_loc = cur_val;
  }
  primitive_str("closeout", extension, close_node);
  primitive_str("special", extension, special_node);
  if(!noninit) {
	primitive_text (frozen_special,"special");
	eqtb[frozen_special] = eqtb[cur_val];
  }
  primitive_str("immediate", extension, immediate_code);
  primitive_str("setlanguage", extension,set_language_code);
  primitive_str("pdfliteral", extension,pdf_literal_node);
  primitive_str("pdfobj", extension, pdf_obj_code);
  primitive_str("pdfrefobj", extension, pdf_refobj_node);
  primitive_str("pdfxform", extension, pdf_xform_code);
  primitive_str("pdfrefxform", extension,pdf_refxform_node);
  primitive_str("pdfximage", extension, pdf_ximage_code);
  primitive_str("pdfrefximage", extension,pdf_refximage_node);
  primitive_str("pdfannot", extension, pdf_annot_node);
  primitive_str("pdfstartlink", extension,pdf_start_link_node);
  primitive_str("pdfendlink", extension,pdf_end_link_node);
  primitive_str("pdfoutline", extension,pdf_outline_code);
  primitive_str("pdfdest", extension, pdf_dest_node);
  primitive_str("pdfthread", extension, pdf_thread_node);
  primitive_str("pdfstartthread", extension,pdf_start_thread_node);
  primitive_str("pdfendthread", extension,pdf_end_thread_node);
  primitive_str("pdfsavepos", extension,pdf_save_pos_node);
  primitive_str("pdfsnaprefpoint", extension,pdf_snap_ref_point_node);
  primitive_str("pdfsnapx", extension, pdf_snap_x_node);
  primitive_str("pdfsnapy", extension, pdf_snap_y_node);
  primitive_str("pdflinesnapx", extension,pdf_line_snap_x_code);
  primitive_str("pdflinesnapy", extension,pdf_line_snap_y_code);
  primitive_str("pdfinfo", extension, pdf_info_code);
  primitive_str("pdfcatalog", extension,pdf_catalog_code);
  primitive_str("pdfnames", extension, pdf_names_code);
  primitive_str("pdfincludechars", extension,pdf_include_chars_code);
  primitive_str("pdffontattr", extension,pdf_font_attr_code);
  primitive_str("pdffontexpand", extension,pdf_font_expand_code);
  primitive_str("pdfmapfile", extension,pdf_map_file_code);
  primitive_str("pdftrailer", extension,pdf_trailer_code);
  /* end expansion of Put each... */
  set_no_new_control_sequence(true);
};

void 
init_etex_prim(void) {
  int noninit = 0;
  /* module 1592 */
  /* The \eTeX\ features available in extended mode are grouped into two
   * categories: (1)~Some of them are permanently enabled and have no
   * semantic effect as long as none of the additional primitives are
   * executed. (2)~The remaining \eTeX\ features are optional and can be
   * individually enabled and disabled. For each optional feature there is
   * an \eTeX\ state variable named \.{\\...state}; the feature is enabled,
   * resp.\ disabled by assigning a positive, resp.\ non-positive value to
   * that integer.
   */
  primitive_str("lastnodetype", last_item,last_node_type_code);
  primitive_str("eTeXversion", last_item, eTeX_version_code);
  primitive_str("eTeXrevision", convert, eTeX_revision_code);
  /* module 1603 */
  /* First we implement the additional \eTeX\ parameters in the table of
   * equivalents.
   */
  primitive_str("everyeof", assign_toks, every_eof_loc);
  primitive_str("tracingassigns", assign_int,int_base + tracing_assigns_code);
  primitive_str("tracinggroups", assign_int, int_base + tracing_groups_code);
  primitive_str("tracingifs", assign_int, int_base + tracing_ifs_code);
  primitive_str("tracingscantokens", assign_int,int_base + tracing_scan_tokens_code);
  primitive_str("tracingnesting", assign_int, int_base + tracing_nesting_code);
  primitive_str("predisplaydirection", assign_int,int_base + pre_display_direction_code);
  primitive_str("lastlinefit", assign_int,int_base + last_line_fit_code);
  primitive_str("savingvdiscards", assign_int,int_base + saving_vdiscards_code);
  primitive_str("savinghyphcodes", assign_int,int_base + saving_hyph_codes_code);
  /* module 1609 */
  /* The \.{\\currentgrouplevel} and \.{\\currentgrouptype} commands return
   * the current level of grouping and the type of the current group
   * respectively.
   */
  primitive_str("currentgrouplevel", last_item, current_group_level_code);
  primitive_str("currentgrouptype", last_item, current_group_type_code);
  /* module 1612 */
  /* The \.{\\currentiflevel}, \.{\\currentiftype}, and
   * \.{\\currentifbranch} commands return the current level of conditionals
   * and the type and branch of the current conditional.
   */
  primitive_str("currentiflevel", last_item, current_if_level_code);
  primitive_str("currentiftype", last_item, current_if_type_code);
  primitive_str("currentifbranch", last_item,current_if_branch_code);
  /* module 1615 */
  /* The \.{\\fontcharwd}, \.{\\fontcharht}, \.{\\fontchardp}, and
   * \.{\\fontcharic} commands return information about a character in a
   * font.
   */
  primitive_str("fontcharwd", last_item,font_char_wd_code);
  primitive_str("fontcharht", last_item,font_char_ht_code);
  primitive_str("fontchardp", last_item,font_char_dp_code);
  primitive_str("fontcharic", last_item,font_char_ic_code);
  /* module 1618 */
  /* The \.{\\parshapedimen}, \.{\\parshapeindent}, and \.{\\parshapelength}
   * commands return the indent and length parameters of the current
   * \.{\\parshape} specification.
   */
  primitive_str("parshapelength", last_item,par_shape_length_code);
  primitive_str("parshapeindent", last_item,par_shape_indent_code);
  primitive_str("parshapedimen", last_item,par_shape_dimen_code);
  /* module 1621 */
  /* The \.{\\showgroups} command displays all currently active grouping
   * levels.
   */
  primitive_str("showgroups", xray, show_groups);
  /* module 1630 */
  /* The \.{\\showtokens} command displays a token list.
   */
  primitive_str("showtokens", xray, show_tokens);
  /* module 1632 */
  /* The \.{\\unexpanded} primitive prevents expansion of tokens much as
   * the result from \.{\\the} applied to a token variable. The
   * \.{\\detokenize} primitive converts a token list into a list of
   * character tokens much as if the token list were written to a file. We
   * use the fact that the command modifiers for \.{\\unexpanded} and
   * \.{\\detokenize} are odd whereas those for \.{\\the} and \.{\\showthe}
   * are even.
   */
  primitive_str("unexpanded", the, 1);
  primitive_str("detokenize", the, show_tokens);
  /* module 1635 */
  /* The \.{\\showifs} command displays all currently active conditionals.
   */
  primitive_str("showifs", xray, show_ifs);
  /* module 1638 */
  /* The \.{\\interactionmode} primitive allows to query and set the
   * interaction mode.
   */
  primitive_str("interactionmode",set_page_int, 2);
  /* module 1643 */
  /* The |middle| feature of \eTeX\ allows one ore several \.{\\middle}
   * delimiters to appear between \.{\\left} and \.{\\right}.
   */
  primitive_str("middle", left_right, middle_noad);
  /* module 1647 */
  primitive_str ("TeXXeTstate", assign_int, eTeX_state_base + TeXXeT_code);
  primitive_str ("beginL", valign, begin_L_code);
  primitive_str ("endL", valign, end_L_code);
  primitive_str ("beginR", valign, begin_R_code);
  primitive_str ("endR", valign, end_R_code);
  /* module 1692 */
  /* The |scan_tokens| feature of \eTeX\ defines the \.{\\scantokens}
   * primitive.
   */
  primitive_str("scantokens", input, 2);
  /* module 1704 */
  primitive_str("readline", read_to_cs, 1);
  /* module 1707 */
  /* Here we define the additional conditionals of \eTeX\ as well as the
   * \.{\\unless} prefix.
   */
  primitive_str("unless", expand_after, 1);
  primitive_str("ifdefined", if_test, if_def_code);
  primitive_str("ifcsname", if_test, if_cs_code);
  primitive_str("iffontchar", if_test,if_font_char_code);
  /* module 1715 */
  /* The |protected| feature of \eTeX\ defines the \.{\\protected} prefix
   * command for macro definitions. Such macros are protected against
   * expansions when lists of expanded tokens are built, e.g., for \.{\\edef}
   * or during \.{\\write}.
   */
  primitive_str("protected", prefix, 8);
  /* module 1723 */
  /* Here are the additional \eTeX\ primitives for expressions.
   */
  primitive_str("numexpr", last_item,eTeX_expr - int_val + int_val);
  primitive_str("dimexpr", last_item,eTeX_expr - int_val + dimen_val);
  primitive_str("glueexpr", last_item, eTeX_expr - int_val + glue_val);
  primitive_str("muexpr", last_item, eTeX_expr - int_val + mu_val);
  /* module 1746 */
  /* The \.{\\gluestretch}, \.{\\glueshrink}, \.{\\gluestretchorder}, and
   * \.{\\glueshrinkorder} commands return the stretch and shrink components
   * and their orders of ``infinity'' of a glue specification.
   */
  primitive_str("gluestretchorder", last_item, glue_stretch_order_code);
  primitive_str("glueshrinkorder", last_item, glue_shrink_order_code);
  primitive_str("gluestretch", last_item, glue_stretch_code);
  primitive_str("glueshrink", last_item, glue_shrink_code);
  /* module 1750 */
  /* The \.{\\mutoglue} and \.{\\gluetomu} commands convert ``math'' glue
   * into normal glue and vice versa; they allow to manipulate math glue with
   * \.{\\gluestretch} etc.
   */
  primitive_str("mutoglue", last_item, mu_to_glue_code);
  primitive_str("gluetomu", last_item, glue_to_mu_code);
  /* module 1754 */
  /* \eTeX\ (in extended mode) supports 32768 (i.e., $2^{15}$) count,
   * dimen, skip, muskip, box, and token registers. As in \TeX\ the first
   * 256 registers of each kind are realized as arrays in the table of
   * equivalents; the additional registers are realized as tree structures
   * built from variable-size nodes with individual registers existing only
   * when needed. Default values are used for nonexistent registers: zero
   * for count and dimen values, |zero_glue| for glue (skip and muskip)
   * values, void for boxes, and |null| for token lists (and current marks
   * discussed below).
   * 
   * Similarly there are 32768 mark classes; the command \.{\\marks}|n|
   * creates a mark node for a given mark class |0<=n<=32767| (where
   * \.{\\marks0} is synonymous to \.{\\mark}). The page builder (actually
   * the |fire_up| routine) and the |vsplit| routine maintain the current
   * values of |top_mark|, |first_mark|, |bot_mark|, |split_first_mark|, and
   * |split_bot_mark| for each mark class. They are accessed as
   * \.{\\topmarks}|n| etc., and \.{\\topmarks0} is again synonymous to
   * \.{\\topmark}. As in \TeX\ the five current marks for mark class zero
   * are realized as |cur_mark| array. The additional current marks are
   * again realized as tree structure with individual mark classes existing
   * only when needed.
   */
  primitive_str("marks", mark, marks_code);
  primitive_str("topmarks", top_bot_mark, top_mark_code + marks_code);
  primitive_str("firstmarks", top_bot_mark,first_mark_code + marks_code);
  primitive_str("botmarks", top_bot_mark, bot_mark_code + marks_code);
  primitive_str("splitfirstmarks",top_bot_mark,split_first_mark_code + marks_code);
  primitive_str("splitbotmarks", top_bot_mark,split_bot_mark_code + marks_code);
  /* module 1806 */
  /* The \.{\\pagediscards} and \.{\\splitdiscards} commands share the
   * command code |un_vbox| with \.{\\unvbox} and \.{\\unvcopy}, they are
   * distinguished by their |chr_code| values |last_box_code| and
   * |vsplit_code|. These |chr_code| values are larger than |box_code| and
   * |copy_code|.
   */
  primitive_str("pagediscards", un_vbox, last_box_code);
  primitive_str("splitdiscards", un_vbox, vsplit_code);
  /* module 1809 */
  /* The \.{\\interlinepenalties}, \.{\\clubpenalties}, \.{\\widowpenalties},
   * and \.{\\displaywidowpenalties} commands allow to define arrays of
   * penalty values to be used instead of the corresponding single values.
   */
  primitive_str("interlinepenalties", set_shape, inter_line_penalties_loc);
  primitive_str("clubpenalties", set_shape, club_penalties_loc);
  primitive_str("widowpenalties", set_shape, widow_penalties_loc);
  primitive_str("displaywidowpenalties", set_shape, display_widow_penalties_loc);
};

/* module 1476 */

/* We have noted that there are two versions of \TeX82. One, called \.{INITEX},
 * 
 * has to be run first; it initializes everything from scratch, without
 * reading a format file, and it has the capability of dumping a format file.
 * The other one is called `\.{VIRTEX}'; it is a ``virgin'' program that needs
 * 
 * to input a format file in order to get started. \.{VIRTEX} typically has
 * more memory capacity than \.{INITEX}, because it does not need the space
 * consumed by the auxiliary hyphenation tables and the numerous calls on
 * |primitive|, etc.
 * 
 * The \.{VIRTEX} program cannot read a format file instantaneously, of course;
 * the best implementations therefore allow for production versions of \TeX\ that
 * not only avoid the loading routine for \PASCAL\ object code, they also have
 * a format file pre-loaded. This is impossible to do if we stick to standard
 * \PASCAL; but there is a simple way to fool many systems into avoiding the
 * initialization, as follows:\quad(1)~We declare a global integer variable
 * called |ready_already|. The probability is negligible that this
 * variable holds any particular value like 314159 when \.{VIRTEX} is first
 * loaded.\quad(2)~After we have read in a format file and initialized
 * everything, we set |ready_already:=314159|.\quad(3)~Soon \.{VIRTEX}
 * will zprint `\.*', waiting for more input; and at this point we
 * interrupt the program and save its core image in some form that the
 * operating system can reload speedily.\quad(4)~When that core image is
 * activated, the program starts again at the beginning; but now
 * |ready_already=314159| and all the other global variables have
 * their initial values too. The former chastity has vanished!
 * 
 * In other words, if we allow ourselves to test the condition
 * |ready_already=314159|, before |ready_already| has been
 * assigned a value, we can avoid the lengthy initialization. Dirty tricks
 * rarely pay off so handsomely.
 * 
 * On systems that allow such preloading, the standard program called \.{TeX}
 * should be the one that has \.{plain} format preloaded, since that agrees
 * with {\sl The \TeX book}. Other versions, e.g., \.{AmSTeX}, should also
 * 
 * be provided for commonly used formats.
 */
integer ready_already; /* a sacrifice of purity for economy */

/* module 1477 */
/* Now this is really it: \TeX\ starts and ends here.
 * 
 * The initial test involving |ready_already| should be deleted if the
 * \PASCAL\ runtime system is smart enough to detect such a ``mistake.''
 */

#define const_chk(arg,infarg,suparg) {  if (  arg  <  infarg  )   { arg   =  infarg; }\
               else {   if (  arg  >  suparg )  {  arg   =  suparg; } } }

#define setup_bound_var(a,b,c)  setup_bound_variable (address_of (c), b , a )

int
main_body (void) {	 /* |start_here| */
  int paranoid;
  /* Bounds that may be set from the configuration file. We want the user to be able to specify 
     the names with underscores, but \.{TANGLE} removes underscores, so we're stuck giving the
     names twice, once as a string, once as the identifier. How ugly. */
  /* then, someone writes a version of  \.{TANGLE} that retains underscores. Goody, the code
	 just got even uglier, because now the \.{WEB2C} library is the limiting factor. Cool. */
  setup_bound_var (250000,"main_memory",main_memory); /* |memory_word|s for |mem| in \.{INITEX} */
  setup_bound_var (0,"extra_mem_top",extra_mem_top); /* increase high mem in \.{VIRTEX} */
  setup_bound_var (0,"extra_mem_bot",extra_mem_bot); /* increase low mem in \.{VIRTEX} */
  setup_bound_var (50000,"pool_size",pool_size);
  setup_bound_var (750,"string_vacancies",string_vacancies);
  setup_bound_var (500,"pool_free",pool_free); /* min pool avail after fmt */
  setup_bound_var (300,"max_strings",max_strings);
  setup_bound_var (100,"strings_free",strings_free);
  setup_bound_var (100000,"font_mem_size",font_mem_size);
  setup_bound_var (500,"font_max",font_max);
  setup_bound_var (20000,"trie_size",trie_size);   /* if |ssup_trie_size| increases, recompile */
  setup_bound_var (659,"hyph_size",hyph_size);
  setup_bound_var (3000,"buf_size",buf_size);
  setup_bound_var (50,"nest_size",nest_size);
  setup_bound_var (15,"max_in_open",max_in_open);
  setup_bound_var (60,"param_size",param_size);
  setup_bound_var (4000,"save_size",save_size);
  setup_bound_var (300,"stack_size",stack_size);
  setup_bound_var (16384,"dvi_buf_size",dvi_buf_size);
  setup_bound_var (79,"error_line",error_line);
  setup_bound_var (50,"half_error_line",half_error_line);
  setup_bound_var (79,"max_print_line",max_print_line);
  setup_bound_var (65536,"obj_tab_size",obj_tab_size);
  setup_bound_var (65536,"pdf_mem_size",pdf_mem_size);
  setup_bound_var (20000,"dest_names_size",dest_names_size);
  const_chk (main_memory,inf_main_memory,sup_main_memory);
#ifdef INIT
  if (ini_version) {
    extra_mem_top = 0;
    extra_mem_bot = 0;
  }
#endif /* INIT */
  if (extra_mem_bot > sup_main_memory)
    extra_mem_bot = sup_main_memory;
  if (extra_mem_top > sup_main_memory)
    extra_mem_top = sup_main_memory;
  mem_top = mem_bot + main_memory;
  mem_min = mem_bot;
  mem_max = mem_top;
  /* Check other constants against their sup and inf. */
  const_chk (trie_size,inf_trie_size,sup_trie_size);
  const_chk (hyph_size,inf_hyph_size,sup_hyph_size);
  const_chk (buf_size,inf_buf_size,sup_buf_size);
  const_chk (nest_size,inf_nest_size,sup_nest_size);
  const_chk (max_in_open,inf_max_in_open,sup_max_in_open);
  const_chk (param_size,inf_param_size,sup_param_size);
  const_chk (save_size,inf_save_size,sup_save_size);
  const_chk (stack_size,inf_stack_size,sup_stack_size);
  const_chk (dvi_buf_size,inf_dvi_buf_size,sup_dvi_buf_size);
  const_chk (pool_size,inf_pool_size,sup_pool_size);
  const_chk (string_vacancies,inf_string_vacancies,sup_string_vacancies);
  const_chk (pool_free,inf_pool_free,sup_pool_free);
  const_chk (max_strings,inf_max_strings,sup_max_strings);
  const_chk (strings_free,inf_strings_free,sup_strings_free);
  const_chk (font_mem_size,inf_font_mem_size,sup_font_mem_size);
  const_chk (font_max,inf_font_max,sup_font_max);
  const_chk (obj_tab_size,inf_obj_tab_size,sup_obj_tab_size);
  const_chk (pdf_mem_size,inf_pdf_mem_size,sup_pdf_mem_size);
  const_chk (dest_names_size,inf_dest_names_size,sup_dest_names_size);
  if (error_line > ssup_error_line)
    error_line = ssup_error_line;
  /* array memory allocation */ 
  buffer = xmalloc_array (ASCII_code, buf_size);
  nest = xmalloc_array (list_state_record, nest_size);
  save_stack = xmalloc_array (memory_word, save_size);
  input_stack = xmalloc_array (in_state_record,stack_size);
  input_file = xmalloc_array (FILE *, max_in_open);
  line_stack = xmalloc_array (integer, max_in_open);
  eof_seen = xmalloc_array (boolean, max_in_open);
  grp_stack = xmalloc_array (save_pointer, max_in_open);
  if_stack = xmalloc_array (pointer, max_in_open);
  source_filename_stack = xmalloc_array (str_number, max_in_open);
  full_source_filename_stack = xmalloc_array (str_number, max_in_open);
  param_stack = xmalloc_array (halfword, param_size);
  dvi_buf = xmalloc_array (eight_bits, dvi_buf_size);
  hyph_word = xmalloc_array (str_number, hyph_size);
  hyph_list = xmalloc_array (halfword, hyph_size);
  hyph_link = xmalloc_array (hyph_pointer, hyph_size);
  obj_tab = xmalloc_array (obj_entry, obj_tab_size);
  pdf_mem = xmalloc_array (integer, pdf_mem_size);
  dest_names = xmalloc_array (dest_name_entry, dest_names_size);
#ifdef INIT
  if (ini_version) {
    yzmem = xmalloc_array (memory_word, mem_top - mem_bot);
    mem = yzmem - mem_bot; /* Some compilers require |mem_bot=0| */
    eqtb = xmalloc_array (memory_word, eqtb_size);
    font_info = xmalloc_array (fmemory_word, font_mem_size);
  }
#endif /* INIT */
  hash_initialize();
  /* strings init is needed always ... */
  str_start = xmalloc_array (pool_pointer, max_strings);
  str_pool = xmalloc_array (packed_ASCII_code, pool_size);
  history = fatal_error_stop; /* in case we quit during initialization */
  t_open_out;   /* open the terminal for output */ 
  if (ready_already == 314159)
    goto START_OF_TEX;
  /* begin expansion of Check the ``constant'' values... */
  /* module 14 */
  /* Later on we will say `\ignorespaces|if mem_max>=max_halfword then bad:=14|',
   * or something similar. (We can't do that until |max_halfword| has been defined.)
   */
  bad = 0;
  paranoid = 0; /* this is for gcc, so that there are no compares between
				   bare integers: "XXX will never be executed" */ 
  if ((half_error_line < 30) || (half_error_line > error_line - 15))
    bad = 1;
  if (max_print_line < 60)
    bad = 2;
  if (dvi_buf_size % 8 != 0)
    bad = 3;
  if (mem_bot + 1100 > mem_top)
    bad = 4;
  if (paranoid || (hash_prime > HASH_SIZE))
    bad = 5;
  if (max_in_open >= 128)
    bad = 6;
  if (mem_top < 256 + 11)
    bad = 7; /* we will want |null_list>255| */
  /* module 111 */  
  /* Here are the inequalities that the quarterword and halfword values
   * must satisfy (or rather, the inequalities that they mustn't satisfy):
   */
#ifdef INIT
  if ((mem_min != mem_bot) || (mem_max != mem_top))
    bad = 10;
#endif /* INIT */
  if ((mem_min > mem_bot) || (mem_max < mem_top))
    bad = 10;
  if (paranoid || ((min_quarterword > 0) || (max_quarterword < 127)))
    bad = 11;
  if (paranoid || ((min_halfword > 0) || (max_halfword < 32767)))
    bad = 12;
  if (paranoid || ((min_quarterword < min_halfword)|| (max_quarterword > max_halfword)))
    bad = 13;
  if ((mem_min < min_halfword)|| (mem_max >= max_halfword)|| (mem_bot - mem_min > max_halfword + 1))
    bad = 14;
  if (paranoid || ((max_font_max < min_halfword)|| (max_font_max > max_halfword)))
    bad = 15;
  if (font_max > font_base + max_font_max)
    bad = 16;
  if ((save_size > max_halfword)|| (max_strings > max_halfword))
    bad = 17;
  if (buf_size > max_halfword)
    bad = 18;
  if (paranoid || (max_quarterword - min_quarterword < 255))
    bad = 19;
  /* module 290 */
  if (cs_token_flag + eqtb_size > max_halfword)
    bad = 21;
  /* module 522 */
  if (paranoid || (format_default_length > file_name_size))
    bad = 31;
  /* module 1394 */
  /* Here's something that isn't quite so obvious. It guarantees that
   * |info(par_shape_ptr)| can hold any positive~|n| for which |get_node(2*n+1)|
   * doesn't overflow the memory capacity.
   */
  if (2 * max_halfword < mem_top - mem_min)
    bad = 41;
  /* end expansion of Check the ``constant'' values... */
  if (bad > 0) {
    fprintf(term_out,"%s%s%ld\n", "Ouch---my internal constants have been clobbered!", "---case ", (integer) bad);
    goto FINAL_END;
  };
  /* get_strings_started is needed always and before initialize  */
  if (!(get_strings_started()))
    goto FINAL_END;
  initialize(); /* set global variables to their starting values */
  if (ini_version) {
    init_prim(0); /* call |primitive| for each primitive */
    init_str_ptr = str_ptr;
    init_pool_ptr = pool_ptr;
    fix_date_and_time;
  } else {
	init_prim(1);
  }
  ready_already = 314159;
 START_OF_TEX:
  /* begin expansion of Initialize the output routines */
  print_initialize ();
  /* module 61 */ 
  /* Here is the very first thing that \TeX\ prints: a headline that identifies
   * the version number and format package. The |term_offset| variable is temporarily
   * incorrect, but the discrepancy is not serious since we assume that the banner
   * and format identifier together will occupy at most |max_print_line|
   * character positions.
   */
  fprintf ( term_out , "%s%c%s%c%s",banner);
  wterm_string (version_string);
  if (format_ident > 0)
    slow_print (format_ident);
  print_ln();
  update_terminal;
  /* module 528 */
  /* Initially |jobname=0|; it becomes nonzero as soon as the true name is known.
   * We have |jobname=0| if and only if the `\.{log}' file has not been opened,
   * except of course for a short time just after |jobname| has become nonzero.
   */
  jobname = 0;
  name_in_progress = false;
  log_opened = false;
  /* module 533 */
  output_file_name = 0;
  /* end expansion of Initialize the output routines */
  /* begin expansion of Get the first line of input and prepare to start */
  /* module 1482 */
  /* When we begin the following code, \TeX's tables may still contain garbage;
   * the strings might not even be present. Thus we must proceed cautiously to get
   * bootstrapped in.
   * 
   * But when we finish this part of the program, \TeX\ is ready to call on the
   * |main_control| routine to do its work.
   */
  {
    /* begin expansion of Initialize the input routines */
    cmdchr_initialize();
    /* next 4 lines where part of the above proc initially */
    if (!init_terminal())
      goto FINAL_END;
    limit = last;
    first = last + 1;    /* |init_terminal| has set |loc| and |last| */
    /* end expansion of Initialize the input routines */
    /* begin expansion of Enable \eTeX, if requested */
    /* module 1591 */
	/* 
     * The program has two modes of operation: (1)~In \TeX\ compatibility mode
     * it fully deserves the name \TeX\ and there are neither extended features
     * nor additional primitive commands. There are, however, a few
     * modifications that would be legitimate in any implementation of \TeX\
     * such as, e.g., preventing inadequate results of the glue to \.{DVI}
     * unit conversion during |ship_out|. (2)~In extended mode there are
     * additional primitive commands and the extended features of \eTeX\ are
     * available.
     * 
     * The distinction between these two modes of operation initially takes
     * place when a `virgin' \.{eINITEX} starts without reading a format file.
     * Later on the values of all \eTeX\ state variables are inherited when
     * \.{eVIRTEX} (or \.{eINITEX}) reads a format file.
     * 
     * The code below is designed to work for cases where `$|init|\ldots|tini|$'
     * is a run-time switch.
     */    
    if ((buffer[loc] == '*') && (format_ident == slow_make_tex_string(" (INITEX)"))) {
      set_no_new_control_sequence (false);
      /* begin expansion of Generate all \eTeX\ primitives */
      init_etex_prim();
      /* end expansion of Generate all \eTeX\ primitives */
      incr (loc);
      eTeX_mode = 1; /* enter extended mode */
      /* begin expansion of Initialize variables for \eTeX\ extended mode */  
      /* module 1758 */
      max_reg_num = 32767;
      max_reg_help_line = "A register number must be between 0 and 32767.";
      /* end expansion of Initialize variables for \eTeX\ extended mode */
	}
    if (!is_no_new_control_sequence()){	/* just entered extended mode ? */
      set_no_new_control_sequence (true);
    } else {
      /* end expansion of Enable \eTeX, if requested */
      if ((format_ident == 0) || (buffer[loc] == '&') || dump_line) {
		if (format_ident != 0)
		  initialize(); /* erase preloaded format */ 
		if (!(open_fmt_file()))
		  goto FINAL_END;
		if (!(load_fmt_file())) {
		  w_close (fmt_file);
		  goto FINAL_END;
		};
		w_close (fmt_file);
		while ((loc < limit) && (buffer[loc] == ' '))
		  incr (loc);
      };
    }
    if (eTeX_ex)
      wterm_string("entering extended mode\n");
    if (end_line_char_inactive) {
      decr (limit);
    } else {
      buffer[limit] = end_line_char;
    }
    if (mltex_enabled_p) {
      wterm_string ("MLTeX v2.2 enabled\n");
    };
    fix_date_and_time;
    if (trie_not_ready) { /* initex without format loaded */
      trie_xmalloc(trie_size);
      /* Allocate and initialize font arrays */
      font_xmalloc(font_max);
      pdffont_xmalloc(font_max);
	  vf_xmalloc(font_max);
      pdffont_initialize_init(font_max);
	  font_initialize_init();
    };
    font_used = xmalloc_array (boolean, font_max);
    for (font_k = font_base; font_k <= font_max; font_k++)
      font_used[font_k] = false;
    /* Compute the magic offset */ /* not used */
    /* begin expansion of Initialize the print |selector|... */
    initialize_selector;
    /* end expansion of Initialize the print |selector|... */
    if ((loc < limit) && (cat_code (buffer[loc]) != escape)) {
	  start_input(); /* \.{\\input} assumed */
	} 
    /* begin expansion of Read values from config file if necessary */
    read_values_from_config_file();
    /* end expansion of Read values from config file if necessary */
  };
  /* end expansion of Get the first line of input and prepare to start */
  history = spotless; /* ready to go! */ 
  main_control(); /* come to life */ 
  final_cleanup(); /* prepare for death */ 
  close_files_and_terminate();
 FINAL_END:
  update_terminal ; 
  ready_already   = 0 ;
  if ( ( history   !=  spotless )  && ( history   !=  warning_issued )) {
	return 1 ; 
  } else { 
	return 0 ;
  }; 
};


/* module 1829 */

/* 
 * This section should be replaced, if necessary, by any special
 * modifications of the program
 * that are necessary to make \TeX\ work at a particular installation.
 * It is usually best to design your change file so that all changes to
 * previous sections preserve the section numbering; then everybody's version
 * will be consistent with the published program. More extensive changes,
 * which introduce new sections, can be inserted here; then only the index
 * itself will get a new section number.
 */


