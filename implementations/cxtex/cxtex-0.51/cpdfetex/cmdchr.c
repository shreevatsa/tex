#include "types.h"
#include "c-compat.h"

#include "globals.h"


/* module 54 */
ASCII_code  trick_buf[ssup_error_line]; /* circular buffer for  pseudoprinting */ 
int trick_count; /* threshold for pseudoprinting, explained later */ 
int first_count; /* another variable for pseudoprinting */

/* module 297 */

/* 
 * Let's pause a moment now and try to look at the Big Picture.
 * The \TeX\ program consists of three main parts: syntactic routines,
 * semantic routines, and output routines. The chief purpose of the
 * syntactic routines is to deliver the user's input to the semantic routines,
 * one token at a time. The semantic routines act as an interpreter
 * responding to these tokens, which may be regarded as commands. And the
 * output routines are periodically called on to convert box-and-glue
 * lists into a compact set of instructions that will be sent
 * to a typesetter. We have discussed the basic data structures and utility
 * routines of \TeX, so we are good and ready to plunge into the real activity by
 * considering the syntactic routines.
 * 
 * Our current goal is to come to grips with the |get_next| procedure,
 * which is the keystone of \TeX's input mechanism. Each call of |get_next|
 * sets the value of three variables |cur_cmd|, |cur_chr|, and |cur_cs|,
 * representing the next input token.
 * $$\vbox{\halign{#\hfil\cr
 * \hbox{|cur_cmd| denotes a command code from the long list of codes
 * given above;}\cr
 * \hbox{|cur_chr| denotes a character code or other modifier of the command
 * code;}\cr
 * \hbox{|cur_cs| is the |eqtb| location of the current control sequence,}\cr
 * \hbox{\qquad if the current token was a control sequence,
 * otherwise it's zero.}\cr}}$$
 * Underlying this external behavior of |get_next| is all the machinery
 * necessary to convert from character files to tokens. At a given time we
 * may be only partially finished with the reading of several files (for
 * which \.{\\input} was specified), and partially finished with the expansion
 * of some user-defined macros and/or some macro parameters, and partially
 * finished with the generation of some text in a template for \.{\\halign},
 * and so on. When reading a character file, special characters must be
 * classified as math delimiters, etc.; comments and extra blank spaces must
 * be removed, paragraphs must be recognized, and control sequences must be
 * found in the hash table. Furthermore there are occasions in which the
 * scanning routines have looked ahead for a word like `\.{plus}' but only
 * part of that word was found, hence a few characters must be put back
 * into the input and scanned again.
 * 
 * To handle these situations, which might all be present simultaneously,
 * \TeX\ uses various stacks that hold information about the incomplete
 * activities, and there is a finite state control for each level of the
 * input mechanism. These stacks record the current state of an implicitly
 * recursive process, but the |get_next| procedure is not recursive.
 * Therefore it will not be difficult to translate these algorithms into
 * low-level languages that do not support recursion.
 */
eight_bits cur_cmd; /* current command set by |get_next| */ 
halfword cur_chr; /* operand of current command */ 
pointer cur_cs; /* control sequence found here, zero if none found */ 
halfword cur_tok; /* packed representative of |cur_cmd| and |cur_chr| */

/* module 298 */

/* The |print_cmd_chr| routine prints a symbolic interpretation of a
 * command code and its modifier. This is used in certain `\.{You can\'t}'
 * error messages, and in the implementation of diagnostic routines like
 * \.{\\show}.
 * 
 * The body of |print_cmd_chr| is a rather tedious listing of zprint
 * commands, and most of it is essentially an inverse to the |primitive|
 * routine that enters a \TeX\ primitive into |eqtb|. Therefore much of
 * this procedure appears elsewhere in the program,
 * together with the corresponding |primitive| calls.
 */
#define chr_cmd( arg ) { zprint_string ( arg ); print_ASCII ( chr_code );}

/* module 266 */
/* Each primitive has a corresponding inverse, so that it is possible to
 * display the cryptic numeric contents of |eqtb| in symbolic form.
 * Every call of |primitive| in this program is therefore accompanied by some
 * straightforward code that forms part of the |print_cmd_chr| routine
 * below.
 */

/*TH This is essentially a very trivial routine, and keeping track of the
 * web comments was a bit of a hassle. That's why I removed all of the
 * 'module such and such' and 'expansion of XX' comments from print_cmd_chr()
 */

void
print_cmd_chr (quarterword cmd, halfword chr_code) {
  int n;			/* temp variable */
  switch (cmd) {
  case left_brace:
    chr_cmd ("begin-group character ");
    break;
  case right_brace:
    chr_cmd ("end-group character ");
    break;
  case math_shift:
    chr_cmd ("math shift character ");
    break;
  case mac_param:
    chr_cmd ("macro parameter character ");
    break;
  case sup_mark:
    chr_cmd ("superscript character ");
    break;
  case sub_mark:
    chr_cmd ("subscript character ");
    break;
  case endv:
    zprint_string("end of alignment template");
    break;
  case spacer:
    chr_cmd ("blank space ");
    break;
  case letter:
    chr_cmd ("the letter ");
    break;
  case other_char:
    chr_cmd ("the character ");
    break;
  case assign_glue:
  case assign_mu_glue:
    if (chr_code < skip_base) {
      print_skip_param (chr_code - glue_base);
    } else {
      if (chr_code < mu_skip_base) {
	print_esc_string ("skip");
	print_int (chr_code - skip_base);
      } else {
	print_esc_string ("muskip");
	print_int (chr_code - mu_skip_base);
      };
    };
    break;
  case assign_toks:
	if (chr_code >= toks_base) {
	  print_esc_string ("toks");
	  print_int (chr_code - toks_base);
	} else {
	  switch (chr_code) {
	  case output_routine_loc:
		print_esc_string ("output");
		break;
	  case every_par_loc:
		print_esc_string ("everypar");
		break;
	  case every_math_loc:
		print_esc_string ("everymath");
		break;
	  case every_display_loc:
		print_esc_string ("everydisplay");
		break;
	  case every_hbox_loc:
		print_esc_string ("everyhbox");
		break;
	  case every_vbox_loc:
		print_esc_string ("everyvbox");
		break;
	  case every_job_loc:
		print_esc_string ("everyjob");
		break;
	  case every_cr_loc:
		print_esc_string ("everycr");
		break;
	  case every_eof_loc:
		print_esc_string ("everyeof");
		break;
	  case pdf_pages_attr_loc:
		print_esc_string ("pdfpagesattr");
		break;
	  case pdf_page_attr_loc:
		print_esc_string ("pdfpageattr");
		break;
	  case pdf_page_resources_loc:
		print_esc_string ("pdfpageresources");
		break;
	  default:
		print_esc_string ("errhelp");
	  };
	};
	break;
  case assign_int:
	if (chr_code < count_base) {
	  print_param (chr_code - int_base);
	} else {
	  print_esc_string ("count");
	  print_int (chr_code - count_base);
	};
	break;
  case assign_dimen:
	if (chr_code < scaled_base) {
	  print_length_param (chr_code - dimen_base);
	} else {
	  print_esc_string ("dimen");
	  print_int (chr_code - scaled_base);
	};
	break;
  case accent:
	print_esc_string ("accent");
	break;
  case advance:
	print_esc_string ("advance");
	break;
  case after_assignment:
	print_esc_string ("afterassignment");
	break;
  case after_group:
	print_esc_string ("aftergroup");
	break;
  case assign_font_dimen:
	print_esc_string ("fontdimen");
	break;
  case begin_group:
	print_esc_string ("begingroup");
	break;
  case break_penalty:
	print_esc_string ("penalty");
	break;
  case char_num:
	print_esc_string ("char");
	break;
  case cs_name:
	print_esc_string ("csname");
	break;
  case def_font:
	print_esc_string ("font");
	break;
  case delim_num:
	print_esc_string ("delimiter");
	break;
  case divide:
	print_esc_string ("divide");
	break;
  case end_cs_name:
	print_esc_string ("endcsname");
	break;
  case end_group:
	print_esc_string ("endgroup");
	break;
  case ex_space:
	print_esc_string ((" "));
	break;
  case expand_after:
	if (chr_code == 0) {
	  print_esc_string ("expandafter");
	} else {
	  print_esc_string ("unless");
	};
	break;
  case halign:
	print_esc_string ("halign");
	break;
  case hrule:
	print_esc_string ("hrule");
	break;
  case ignore_spaces:
	print_esc_string ("ignorespaces");
	break;
  case insert:
	print_esc_string ("insert");
	break;
  case ital_corr:
	print_esc_string (("/"));
	break;
  case mark:
	print_esc_string ("mark");
	if (chr_code > 0)
	  print_char ('s');
	break;
  case math_accent:
	print_esc_string ("mathaccent");
	break;
  case math_char_num:
	print_esc_string ("mathchar");
	break;
  case math_choice:
	print_esc_string ("mathchoice");
	break;
  case multiply:
	print_esc_string ("multiply");
	break;
  case no_align:
	print_esc_string ("noalign");
	break;
  case no_boundary:
	print_esc_string ("noboundary");
	break;
  case no_expand:
	print_esc_string ("noexpand");
	break;
  case non_script:
	print_esc_string ("nonscript");
	break;
  case omit:
	print_esc_string ("omit");
	break;
  case radical:
	print_esc_string ("radical");
	break;
  case read_to_cs:
	if (chr_code == 0) {
	  print_esc_string ("read");
	} else {
	  print_esc_string ("readline");
	};
	break;
  case relax:
	print_esc_string ("relax");
	break;
  case set_box:
	print_esc_string ("setbox");
	break;
  case set_prev_graf:
	print_esc_string ("prevgraf");
	break;
  case set_shape:
	switch (chr_code) {
	case par_shape_loc:
	  print_esc_string ("parshape");
	  break;
	case inter_line_penalties_loc:
	  print_esc_string ("interlinepenalties");
	  break;
	case club_penalties_loc:
	  print_esc_string ("clubpenalties");
	  break;
	case widow_penalties_loc:
	  print_esc_string ("widowpenalties");
	  break;
	case display_widow_penalties_loc:
	  print_esc_string ("displaywidowpenalties");
	};
	break;
  case the:
	if (chr_code == 0) {
	  print_esc_string ("the");
	} else if (chr_code == 1) {
	  print_esc_string ("unexpanded");
	} else {
	  print_esc_string ("detokenize");
	};
	break;
  case toks_register:
	print_esc_string ("toks");
	if (chr_code != mem_bot)
	  print_sa_num (chr_code);
	break;
  case vadjust:
	print_esc_string ("vadjust");
	break;
  case valign:
	if (chr_code == 0) {
	  print_esc_string ("valign");
	} else {
	  switch (chr_code) {
	  case begin_L_code:
		print_esc_string ("beginL");
		break;
	  case end_L_code:
		print_esc_string ("endL");
		break;
	  case begin_R_code:
		print_esc_string ("beginR");
		break;
	  default:
		print_esc_string ("endR");
	  }
	};
	break;
  case vcenter:
	print_esc_string ("vcenter");
	break;
  case vrule:
	print_esc_string ("vrule");
	break;
  case par_end:
	print_esc_string ("par");
	break;
  case input:
	if (chr_code == 0) {
	  print_esc_string ("input");
	} else if (chr_code == 2) {
	  print_esc_string ("scantokens");
	} else {
	  print_esc_string ("endinput");
	};
	break;
  case top_bot_mark:
	switch ((chr_code % marks_code)) {
	case first_mark_code:
	  print_esc_string ("firstmark");
	  break;
	case bot_mark_code:
	  print_esc_string ("botmark");
	  break;
	case split_first_mark_code:
	  print_esc_string ("splitfirstmark");
	  break;
	case split_bot_mark_code:
	  print_esc_string ("splitbotmark");
	  break;
	default:
	  print_esc_string ("topmark");
	};
	if (chr_code >= marks_code)
	  print_char ('s');
	break;
  case register_cmd:
	if ((chr_code < mem_bot) || (chr_code > lo_mem_stat_max)) {
	  cmd = sa_type (chr_code);
	} else {
	  cmd = chr_code - mem_bot;
	  chr_code = null;
	};
	if (cmd == int_val) {
	  print_esc_string ("count");
	} else if (cmd == dimen_val) {
	  print_esc_string ("dimen");
	} else if (cmd == glue_val) {
	  print_esc_string ("skip");
	} else {
	  print_esc_string ("muskip");
	};
	if (chr_code != null)
	  print_sa_num (chr_code);
	break;
  case set_aux:
	if (chr_code == vmode) {
	  print_esc_string ("prevdepth");
	} else {
	  print_esc_string ("spacefactor");
	};
	break;
  case set_page_int:
	if (chr_code == 0) {
	  print_esc_string ("deadcycles");
	} else if (chr_code == 2) {
	  print_esc_string ("interactionmode");
	} else {
	  print_esc_string ("insertpenalties");
	}
	break;
  case set_box_dimen:
	if (chr_code == width_offset) {
	  print_esc_string ("wd");
	} else if (chr_code == height_offset) {
	  print_esc_string ("ht");
	} else {
	  print_esc_string ("dp");
	}
	break;
  case last_item:
	switch (chr_code) {
	case int_val:
	  print_esc_string ("lastpenalty");
	  break;
	case dimen_val:
	  print_esc_string ("lastkern");
	  break;
	case glue_val:
	  print_esc_string ("lastskip");
	  break;
	case input_line_no_code:
	  print_esc_string ("inputlineno");
	  break;
	case last_node_type_code:
	  print_esc_string ("lastnodetype");
	  break;
	case eTeX_version_code:
	  print_esc_string ("eTeXversion");
	  break;
	case current_group_level_code:
	  print_esc_string ("currentgrouplevel");
	  break;
	case current_group_type_code:
	  print_esc_string ("currentgrouptype");
	  break;
	case current_if_level_code:
	  print_esc_string ("currentiflevel");
	  break;
	case current_if_type_code:
	  print_esc_string ("currentiftype");
	  break;
	case current_if_branch_code:
	  print_esc_string ("currentifbranch");
	  break;
	case font_char_wd_code:
	  print_esc_string ("fontcharwd");
	  break;
	case font_char_ht_code:
	  print_esc_string ("fontcharht");
	  break;
	case font_char_dp_code:
	  print_esc_string ("fontchardp");
	  break;
	case font_char_ic_code:
	  print_esc_string ("fontcharic");
	  break;
	case par_shape_length_code:
	  print_esc_string ("parshapelength");
	  break;
	case par_shape_indent_code:
	  print_esc_string ("parshapeindent");
	  break;
	case par_shape_dimen_code:
	  print_esc_string ("parshapedimen");
	  break;
	case eTeX_expr - int_val + int_val:
	  print_esc_string ("numexpr");
	  break;
	case eTeX_expr - int_val + dimen_val:
	  print_esc_string ("dimexpr");
	  break;
	case eTeX_expr - int_val + glue_val:
	  print_esc_string ("glueexpr");
	  break;
	case eTeX_expr - int_val + mu_val:
	  print_esc_string ("muexpr");
	  break;
	case glue_stretch_order_code:
	  print_esc_string ("gluestretchorder");
	  break;
	case glue_shrink_order_code:
	  print_esc_string ("glueshrinkorder");
	  break;
	case glue_stretch_code:
	  print_esc_string ("gluestretch");
	  break;
	case glue_shrink_code:
	  print_esc_string ("glueshrink");
	  break;
	case mu_to_glue_code:
	  print_esc_string ("mutoglue");
	  break;
	case glue_to_mu_code:
	  print_esc_string ("gluetomu");
	  break;
	case pdftex_version_code:
	  print_esc_string ("pdftexversion");
	  break;
	case pdf_last_obj_code:
	  print_esc_string ("pdflastobj");
	  break;
	case pdf_last_xform_code:
	  print_esc_string ("pdflastxform");
	  break;
	case pdf_last_ximage_code:
	  print_esc_string ("pdflastximage");
	  break;
	case pdf_last_ximage_pages_code:
	  print_esc_string ("pdflastximagepages");
	  break;
	case pdf_last_annot_code:
	  print_esc_string ("pdflastannot");
	  break;
	case pdf_last_x_pos_code:
	  print_esc_string ("pdflastxpos");
	  break;
	case pdf_last_y_pos_code:
	  print_esc_string ("pdflastypos");
	  break;
	case pdf_last_demerits_code:
	  print_esc_string ("pdflastdemerits");
	  break;
	case pdf_last_vbreak_penalty_code:
	  print_esc_string ("pdflastvbreakpenalty");
	  break;
	default:
	  print_esc_string ("badness");
	};
	break;
  case convert:
	switch (chr_code) {
	case number_code:
	  print_esc_string ("number");
	  break;
	case roman_numeral_code:
	  print_esc_string ("romannumeral");
	  break;
	case string_code:
	  print_esc_string ("string");
	  break;
	case meaning_code:
	  print_esc_string ("meaning");
	  break;
	case font_name_code:
	  print_esc_string ("fontname");
	  break;
	case eTeX_revision_code:
	  print_esc_string ("eTeXrevision");
	  break;
	case pdftex_revision_code:
	  print_esc_string ("pdftexrevision");
	  break;
	case pdf_font_name_code:
	  print_esc_string ("pdffontname");
	  break;
	case pdf_font_objnum_code:
	  print_esc_string ("pdffontobjnum");
	  break;
	case pdf_font_size_code:
	  print_esc_string ("pdffontsize");
	  break;
	default:
	  print_esc_string ("jobname");
	};
	break;
  case if_test:
	if (chr_code >= unless_code)
	  print_esc_string ("unless");
	switch (chr_code % unless_code) {
	case if_cat_code:
	  print_esc_string ("ifcat");
	  break;
	case if_int_code:
	  print_esc_string ("ifnum");
	  break;
	case if_dim_code:
	  print_esc_string ("ifdim");
	  break;
	case if_odd_code:
	  print_esc_string ("ifodd");
	  break;
	case if_vmode_code:
	  print_esc_string ("ifvmode");
	  break;
	case if_hmode_code:
	  print_esc_string ("ifhmode");
	  break;
	case if_mmode_code:
	  print_esc_string ("ifmmode");
	  break;
	case if_inner_code:
	  print_esc_string ("ifinner");
	  break;
	case if_void_code:
	  print_esc_string ("ifvoid");
	  break;
	case if_hbox_code:
	  print_esc_string ("ifhbox");
	  break;
	case if_vbox_code:
	  print_esc_string ("ifvbox");
	  break;
	case ifx_code:
	  print_esc_string ("ifx");
	  break;
	case if_eof_code:
	  print_esc_string ("ifeof");
	  break;
	case if_true_code:
	  print_esc_string ("iftrue");
	  break;
	case if_false_code:
	  print_esc_string ("iffalse");
	  break;
	case if_case_code:
	  print_esc_string ("ifcase");
	  break;
	case if_def_code:
	  print_esc_string ("ifdefined");
	  break;
	case if_cs_code:
	  print_esc_string ("ifcsname");
	  break;
	case if_font_char_code:
	  print_esc_string ("iffontchar");
	  break;
	default:
	  print_esc_string ("if");
	};
	break;
  case fi_or_else:
	if (chr_code == fi_code) {
	  print_esc_string ("fi");
	} else if (chr_code == or_code) {
	  print_esc_string ("or");
	} else {
	  print_esc_string ("else");
	}
	break;
  case tab_mark:
	if (chr_code == span_code) {
	  print_esc_string ("span");
	} else {
	  chr_cmd ("alignment tab character ");
	};
	break;
  case car_ret:
	if (chr_code == cr_code) {
	  print_esc_string ("cr");
	} else {
	  print_esc_string ("crcr");
	};
	break;
  case set_page_dimen:
	switch (chr_code) {
	case 0:
	  print_esc_string ("pagegoal");
	  break;
	case 1:
	  print_esc_string ("pagetotal");
	  break;
	case 2:
	  print_esc_string ("pagestretch");
	  break;
	case 3:
	  print_esc_string ("pagefilstretch");
	  break;
	case 4:
	  print_esc_string ("pagefillstretch");
	  break;
	case 5:
	  print_esc_string ("pagefilllstretch");
	  break;
	case 6:
	  print_esc_string ("pageshrink");
	  break;
	default:
	  print_esc_string ("pagedepth");
	};
	break;
  case stop:
	if (chr_code == 1) {
	  print_esc_string ("dump");
	} else {
	  print_esc_string ("end");
	};
	break;
  case hskip:
	switch (chr_code) {
	case skip_code:
	  print_esc_string ("hskip");
	  break;
	case fil_code:
	  print_esc_string ("hfil");
	  break;
	case fill_code:
	  print_esc_string ("hfill");
	  break;
	case ss_code:
	  print_esc_string ("hss");
	  break;
	default:
	  print_esc_string ("hfilneg");
	};
	break;
  case vskip:
	switch (chr_code) {
	case skip_code:
	  print_esc_string ("vskip");
	  break;
	case fil_code:
	  print_esc_string ("vfil");
	  break;
	case fill_code:
	  print_esc_string ("vfill");
	  break;
	case ss_code:
	  print_esc_string ("vss");
	  break;
	default:
	  print_esc_string ("vfilneg");
	};
	break;
  case mskip:
	print_esc_string ("mskip");
	break;
  case kern:
	print_esc_string ("kern");
	break;
  case mkern:
	print_esc_string ("mkern");
	break;
  case hmove:
	if (chr_code == 1) {
	  print_esc_string ("moveleft");
	} else {
	  print_esc_string ("moveright");
	};
	break;
  case vmove:
	if (chr_code == 1) {
	  print_esc_string ("raise");
	} else {
	  print_esc_string ("lower");
	};
	break;
  case make_box:
	switch (chr_code) {
	case box_code:
	  print_esc_string ("box");
	  break;
	case copy_code:
	  print_esc_string ("copy");
	  break;
	case last_box_code:
	  print_esc_string ("lastbox");
	  break;
	case vsplit_code:
	  print_esc_string ("vsplit");
	  break;
	case vtop_code:
	  print_esc_string ("vtop");
	  break;
	case vtop_code + vmode:
	  print_esc_string ("vbox");
	  break;
	default:
	  print_esc_string ("hbox");
	};
	break;
  case leader_ship:
	if (chr_code == a_leaders) {
	  print_esc_string ("leaders");
	} else if (chr_code == c_leaders) {
	  print_esc_string ("cleaders");
	} else if (chr_code == x_leaders) {
	  print_esc_string ("xleaders");
	} else {
	  print_esc_string ("shipout");
	};
	break;
  case start_par:
	if (chr_code == 0) {
	  print_esc_string ("noindent");
	} else {
	  print_esc_string ("indent");
	}
	break;
  case remove_item:
	if (chr_code == glue_node) {
	  print_esc_string ("unskip");
	} else if (chr_code == kern_node) {
	  print_esc_string ("unkern");
	} else {
	  print_esc_string ("unpenalty");
	}
	break;
  case un_hbox:
	if (chr_code == copy_code) {
	  print_esc_string ("unhcopy");
	} else {
	  print_esc_string ("unhbox");
	}
	break;
  case un_vbox:
	if (chr_code == copy_code) {
	  print_esc_string ("unvcopy");
	} else if (chr_code == last_box_code) {
	  print_esc_string ("pagediscards");
	} else if (chr_code == vsplit_code) {
	  print_esc_string ("splitdiscards");
	} else {
	  print_esc_string ("unvbox");
	};
	break;
  case discretionary:
	if (chr_code == 1) {
	  print_esc_string (("-"));
	} else {
	  print_esc_string ("discretionary");
	}
	break;
  case eq_no:
	if (chr_code == 1) {
	  print_esc_string ("leqno");
	} else {
	  print_esc_string ("eqno");
	};
	break;
  case math_comp:
	switch (chr_code){
	case ord_noad:
	  print_esc_string ("mathord");
	  break;
	case op_noad:
	  print_esc_string ("mathop");
	  break;
	case bin_noad:
	  print_esc_string ("mathbin");
	  break;
	case rel_noad:
	  print_esc_string ("mathrel");
	  break;
	case open_noad:
	  print_esc_string ("mathopen");
	  break;
	case close_noad:
	  print_esc_string ("mathclose");
	  break;
	case punct_noad:
	  print_esc_string ("mathpunct");
	  break;
	case inner_noad:
	  print_esc_string ("mathinner");
	  break;
	case under_noad:
	  print_esc_string ("underline");
	  break;
	default:
	  print_esc_string ("overline");
	};
	break;
  case limit_switch:
	if (chr_code == limits) {
	  print_esc_string ("limits");
	} else if (chr_code == no_limits) {
	  print_esc_string ("nolimits");
	} else {
	  print_esc_string ("displaylimits");
	}
	break;
  case math_style:
	print_style (chr_code);
	break;
  case above:
	switch (chr_code) {
	case over_code:
	  print_esc_string ("over");
	  break;
	case atop_code:
	  print_esc_string ("atop");
	  break;
	case delimited_code + above_code:
	  print_esc_string ("abovewithdelims");
	  break;
	case delimited_code + over_code:
	  print_esc_string ("overwithdelims");
	  break;
	case delimited_code + atop_code:
	  print_esc_string ("atopwithdelims");
	  break;
	default:
	  print_esc_string ("above");
	};
	break;
  case left_right:
	if (chr_code == left_noad) {
	  print_esc_string ("left");
	} else if (chr_code == middle_noad) {
	  print_esc_string ("middle");
	} else {
	  print_esc_string ("right");
	}
	break;
  case prefix:
	if (chr_code == 1) {
	  print_esc_string ("long");
	} else if (chr_code == 2) {
	  print_esc_string ("outer");
	} else if (chr_code == 8) {
	    print_esc_string ("protected");
	} else {
	  print_esc_string ("global");
	}
	break;
  case def:
	if (chr_code == 0) {
	  print_esc_string ("def");
	} else if (chr_code == 1) {
	  print_esc_string ("gdef");
	} else if (chr_code == 2) {
	  print_esc_string ("edef");
	} else {
	  print_esc_string ("xdef");
	}
	break;
  case let:
	if (chr_code != normal) {
	  print_esc_string ("futurelet");
	} else {
	  print_esc_string ("let");
	};
	break;
  case shorthand_def:
	switch (chr_code) {
	case char_def_code:
	  print_esc_string ("chardef");
	  break;
	case math_char_def_code:
	  print_esc_string ("mathchardef");
	  break;
	case count_def_code:
	  print_esc_string ("countdef");
	  break;
	case dimen_def_code:
	  print_esc_string ("dimendef");
	  break;
	case skip_def_code:
	  print_esc_string ("skipdef");
	  break;
	case mu_skip_def_code:
	  print_esc_string ("muskipdef");
	  break;
	case char_sub_def_code:
	  print_esc_string ("charsubdef");
	  break;
	default:
	  print_esc_string ("toksdef");
	};
	break;
  case char_given:
	print_esc_string ("char");
	print_hex (chr_code);
	break;
  case math_given:
	print_esc_string ("mathchar");
	print_hex (chr_code);
	break;
  case def_code:
	if (chr_code == cat_code_base) {
	  print_esc_string ("catcode");
	} else if (chr_code == math_code_base) {
	  print_esc_string ("mathcode");
	} else if (chr_code == lc_code_base) {
	  print_esc_string ("lccode");
	} else if (chr_code == uc_code_base) {
	  print_esc_string ("uccode");
	} else if (chr_code == sf_code_base) {
	  print_esc_string ("sfcode");
	}  else {
	  print_esc_string ("delcode");
	};
	break;
  case def_family:
	print_size (chr_code - math_font_base);
	break;
  case hyph_data:
	if (chr_code == 1) {
	  print_esc_string ("patterns");
	} else {
	  print_esc_string ("hyphenation");
	};
	break;
  case assign_font_int:
	switch (chr_code) {
	case 0:
	  print_esc_string ("hyphenchar");
	  break;
	case 1:
	  print_esc_string ("skewchar");
	  break;
	case lp_code_base:
	  print_esc_string ("lpcode");
	  break;
	case rp_code_base:
	  print_esc_string ("rpcode");
	  break;
	case ef_code_base:
	  print_esc_string ("efcode");
	  ;
	};
	break;
  case set_font: 
	zprint_string("select font ");
	slow_print (font_name[chr_code]);
	if (font_size[chr_code] != font_dsize[chr_code]) {
	  zprint_string(" at ");
	  print_scaled (font_size[chr_code]);
	  zprint_string("pt");
	};
	break;
  case set_interaction:
	switch (chr_code) {
	case batch_mode:
	  print_esc_string ("batchmode");
	  break;
	case nonstop_mode:
	  print_esc_string ("nonstopmode");
	  break;
	case scroll_mode:
	  print_esc_string ("scrollmode");
	  break;
	default:
	  print_esc_string ("errorstopmode");
	};
	break;
  case in_stream:
	if (chr_code == 0) {
	  print_esc_string ("closein");
	} else {
	  print_esc_string ("openin");
	};
	break;
  case message:
	if (chr_code == 0) {
	  print_esc_string ("message");
	} else {
	  print_esc_string ("errmessage");
	};
	break;
  case case_shift:
	if (chr_code == lc_code_base) {
	  print_esc_string ("lowercase");
	} else {
	  print_esc_string ("uppercase");
	};
	break;
  case xray:
	switch (chr_code) {
	case show_box_code:
	  print_esc_string ("showbox");
	  break;
	case show_the_code:
	  print_esc_string ("showthe");
	  break;
	case show_lists:
	  print_esc_string ("showlists");
	  break;
	case show_groups:
	  print_esc_string ("showgroups");
	  break;
	case show_tokens:
	  print_esc_string ("showtokens");
	  break;
	case show_ifs:
	  print_esc_string ("showifs");
	  break;
	default:
	  print_esc_string ("show");
	};
	break;
  case undefined_cs:
	zprint_string("undefined");
	break;
  case call:
  case long_call:
  case outer_call:
  case long_outer_call:
	n = cmd - call;
	if (info (link (chr_code)) == protected_token)
	  n = n + 4;
	if (odd (n / 4))
	  print_esc_string ("protected");
	if (odd (n))
	  print_esc_string ("long");
	if (odd (n / 2))
	  print_esc_string ("outer");
	if (n > 0)
	  print_char (' ');
	zprint_string("macro");
	break;
  case end_template:
	print_esc_string ("outer endtemplate");
	break;
  case extension:
	switch (chr_code) {
	case open_node:
	  print_esc_string ("openout");
	  break;
	case write_node:
	  print_esc_string ("write");
	  break;
	case close_node:
	  print_esc_string ("closeout");
	  break;
	case special_node:
	  print_esc_string ("special");
	  break;
	case immediate_code:
	  print_esc_string ("immediate");
	  break;
	case set_language_code:
	  print_esc_string ("setlanguage");
	  break;
	case pdf_literal_node:
	  print_esc_string ("pdfliteral");
	  break;
	case pdf_obj_code:
	  print_esc_string ("pdfobj");
	  break;
	case pdf_refobj_node:
	  print_esc_string ("pdfrefobj");
	  break;
	case pdf_xform_code:
	  print_esc_string ("pdfxform");
	  break;
	case pdf_refxform_node:
	  print_esc_string ("pdfrefxform");
	  break;
	case pdf_ximage_code:
	  print_esc_string ("pdfximage");
	  break;
	case pdf_refximage_node:
	  print_esc_string ("pdfrefximage");
	  break;
	case pdf_annot_node:
	  print_esc_string ("pdfannot");
	  break;
	case pdf_start_link_node:
	  print_esc_string ("pdfstartlink");
	  break;
	case pdf_end_link_node:
	  print_esc_string ("pdfendlink");
	  break;
	case pdf_outline_code:
	  print_esc_string ("pdfoutline");
	  break;
	case pdf_dest_node:
	  print_esc_string ("pdfdest");
	  break;
	case pdf_thread_node:
	  print_esc_string ("pdfthread");
	  break;
	case pdf_start_thread_node:
	  print_esc_string ("pdfstartthread");
	  break;
	case pdf_end_thread_node:
	  print_esc_string ("pdfendthread");
	  break;
	case pdf_save_pos_node:
	  print_esc_string ("pdfsavepos");
	  break;
	case pdf_snap_ref_point_node:
	  print_esc_string ("pdfsnaprefpoint");
	  break;
	case pdf_snap_x_node:
	  print_esc_string ("pdfsnapx");
	  break;
	case pdf_snap_y_node:
	  print_esc_string ("pdfsnapy");
	  break;
	case pdf_line_snap_x_code:
	  print_esc_string ("pdflinesnapx");
	  break;
	case pdf_line_snap_y_code:
	  print_esc_string ("pdflinesnapy");
	  break;
	case pdf_info_code:
	  print_esc_string ("pdfinfo");
	  break;
	case pdf_catalog_code:
	  print_esc_string ("pdfcatalog");
	  break;
	case pdf_names_code:
	  print_esc_string ("pdfnames");
	  break;
	case pdf_include_chars_code:
	  print_esc_string ("pdfincludechars");
	  break;
	case pdf_font_attr_code:
	  print_esc_string ("pdffontattr");
	  break;
	case pdf_font_expand_code:
	  print_esc_string ("pdffontexpand");
	  break;
	case pdf_map_file_code:
	  print_esc_string ("pdfmapfile");
	  break;
	case pdf_trailer_code:
	  print_esc_string ("pdftrailer");
	  break;
	default:
	  zprint_string("[unknown extension!]");
	};
	break;
  default:
	zprint_string("[unknown command code!]");
  };
}

int shown_mode; /* most recent mode shown by \.{\\tracingcommands} */

/* module 299 */

/* Here is a procedure that displays the current command.
 */
void
show_cur_cmd_chr (void) {
  int n;			/* level of \.{\\if...\\fi} nesting */
  int l;			/* line where \.{\\if} started */
  pointer p;
  begin_diagnostic();
  print_nl_string ("{");
  if (mode != shown_mode) {
    print_mode (mode);
    zprint_string(": ");
    shown_mode = mode;
  };
  print_cmd_chr (cur_cmd, cur_chr);
  if (tracing_ifs > 0)
    if (cur_cmd >= if_test)
      if (cur_cmd <= fi_or_else) {
	zprint_string(": ");
	if (cur_cmd == fi_or_else) {
	  print_cmd_chr (if_test, cur_if);
	  print_char (' ');
	  n = 0;
	  l = if_line;
	} else {
	  n = 1;
	  l = line;
	};
	p = cond_ptr;
	while (p != null) {
	  incr (n);
	  p = link (p);
	};
	zprint_string("(level ");
	print_int (n);
	print_char (')');
	print_if_line (l);
      };
  print_char ('}');
  end_diagnostic (false);
};


/* module 301 */
in_state_record *input_stack;
unsigned int input_ptr; /* first unused location of |input_stack| */ 
unsigned int max_in_stack; /* largest value of |input_ptr| when pushing */ 
in_state_record cur_input; /* the ``top'' input state, according to convention (1) */

/* module 304 */

/* Additional information about the current line is available via the
 * |index| variable, which counts how many lines of characters are present
 * in the buffer below the current level. We have |index=0| when reading
 * from the terminal and prompting the user for each line; then if the user types,
 * e.g., `\.{\\input paper}', we will have |index=1| while reading
 * the file \.{paper.tex}. However, it does not follow that |index| is the
 * same as the input stack pointer, since many of the levels on the input
 * stack may come from token lists. For example, the instruction `\.{\\input
 * paper}' might occur in a token list.
 * 
 * The global variable |in_open| is equal to the |index|
 * value of the highest non-token-list level. Thus, the number of partially read
 * lines in the buffer is |in_open+1|, and we have |in_open=index|
 * when we are not reading a token list.
 * 
 * If we are not currently reading from the terminal, or from an input
 * stream, we are reading from the file variable |input_file[index]|. We use
 * the notation |terminal_input| as a convenient abbreviation for |name=0|,
 * and |cur_file| as an abbreviation for |input_file[index]|.
 * 
 * The global variable |line| contains the line number in the topmost
 * open file, for use in error messages. If we are not reading from
 * the terminal, |line_stack[index]| holds the line number for the
 * enclosing level, so that |line| can be restored when the current
 * file has been read. Line numbers should never be negative, since the
 * negative of the current line number is used to identify the user's output
 * routine in the |mode_line| field of the semantic nest entries.
 * 
 * If more information about the input state is needed, it can be
 * included in small arrays like those shown here. For example,
 * the current page or segment number in the input file might be
 * put into a variable | page|, maintained for enclosing levels in
 * `\ignorespaces| page_stack:array[1..max_in_open] of integer|\unskip'
 * by analogy with |line_stack|.
 */
unsigned int in_open; /* the number of lines in the buffer, less one */ 
unsigned int open_parens; /* the number of open text files */ 
FILE **input_file;
integer line; /* current line number in the current source file */ 
integer *line_stack;

/* module 305 */

/* Users of \TeX\ sometimes forget to balance left and right braces properly,
 * and one of the ways \TeX\ tries to spot such errors is by considering an
 * input file as broken into subfiles by control sequences that
 * are declared to be \.{\\outer}.
 * 
 * A variable called |scanner_status| tells \TeX\ whether or not to complain
 * when a subfile ends. This variable has six possible values:
 * 
 * \yskip\hang|normal|, means that a subfile can safely end here without incident.
 * 
 * \yskip\hang|skipping|, means that a subfile can safely end here, but not a file,
 * because we're reading past some conditional text that was not selected.
 * 
 * \yskip\hang|defining|, means that a subfile shouldn't end now because a
 * macro is being defined.
 * 
 * \yskip\hang|matching|, means that a subfile shouldn't end now because a
 * macro is being used and we are searching for the end of its arguments.
 * 
 * \yskip\hang|aligning|, means that a subfile shouldn't end now because we are
 * not finished with the preamble of an \.{\\halign} or \.{\\valign}.
 * 
 * \yskip\hang|absorbing|, means that a subfile shouldn't end now because we are
 * reading a balanced token list for \.{\\message}, \.{\\write}, etc.
 * 
 * \yskip\noindent
 * If the |scanner_status| is not |normal|, the variable |warning_index| points
 * to the |eqtb| location for the relevant control sequence name to zprint
 * in an error message.
 */
unsigned char scanner_status; /* can a subfile end now? */ 
pointer warning_index; /* identifier relevant to non-|normal| scanner status */
pointer def_ref; /* reference count of token list being defined */

/* module 308 */

/* The |param_stack| is an auxiliary array used to hold pointers to the token
 * lists for parameters at the current level and subsidiary levels of input.
 * This stack is maintained with convention (2), and it grows at a different
 * rate from the others.
 */
pointer *param_stack; /* token list pointers for parameters */
unsigned int param_ptr; /* first unused entry in |param_stack| */ 
int max_param_stack; /* largest value of |param_ptr|, will be|<=param_size+9|*/

/* module 310 */

/* Thus, the ``current input state'' can be very complicated indeed; there
 * can be many levels and each level can arise in a variety of ways. The
 * |show_context| procedure, which is used by \TeX's error-reporting routine to
 * print out the current input state on all levels down to the most recent
 * line of characters from an input file, illustrates most of these conventions.
 * The global variable |base_ptr| contains the lowest level that was
 * displayed by this procedure.
 */
unsigned int base_ptr; /* shallowest level shown by |show_context| */


/* module 316 */

/* The following code sets up the print routines so that they will gather
 * the desired information.
 */
#define begin_pseudoprint { l = tally; tally = 0; selector = pseudo; trick_count = 1000000; }

/* module 311 */

/* The status at each level is indicated by printing two lines, where the first
 * line indicates what was read so far and the second line shows what remains
 * to be read. The context is cropped, if necessary, so that the first line
 * contains at most |half_error_line| characters, and the second contains
 * at most |error_line|. Non-current input levels whose |token_type| is
 * `|backed_up|' are shown only if they have not been fully read.
 */

/* comment 315 */

/* Here it is necessary to explain a little trick. We don't want to store a long
 * string that corresponds to a token list, because that string might take up
 * lots of memory; and we are printing during a time when an error message is
 * being given, so we dare not do anything that might overflow one of \TeX's
 * tables. So `pseudoprinting' is the answer: We enter a mode of printing
 * that stores characters into a buffer of length |error_line|, where character
 * $k+1$ is placed into \hbox{|trick_buf[k mod error_line]|} if
 * |k<trick_count|, otherwise character |k| is dropped. Initially we set
 * |tally:=0| and |trick_count:=1000000|; then when we reach the
 * point where transition from line 1 to line 2 should occur, we
 * set |first_count:=tally| and |trick_count:=@tmax@>(error_line,
 * tally+1+error_line-half_error_line)|. At the end of the
 * pseudoprinting, the values of |first_count|, |tally|, and
 * |trick_count| give us all the information we need to zprint the two lines,
 * and all of the necessary text is in |trick_buf|.
 * 
 * Namely, let |l| be the length of the descriptive information that appears
 * on the first line. The length of the context information gathered for that
 * line is |k=first_count|, and the length of the context information
 * gathered for line~2 is $m=\min(|tally|, |trick_count|)-k$. If |l+k<=h|,
 * where |h=half_error_line|, we zprint |trick_buf[0..k-1]| after the
 * descriptive information on line~1, and set |n:=l+k|; here |n| is the
 * length of line~1. If $l+k>h$, some cropping is necessary, so we set |n:=h|
 * and zprint `\.{...}' followed by
 * $$\hbox{|trick_buf[(l+k-h+3)..k-1]|,}$$
 * where subscripts of |trick_buf| are circular modulo |error_line|. The
 * second line consists of |n|~spaces followed by |trick_buf[k..(k+m-1)]|,
 * unless |n+m>error_line|; in the latter case, further cropping is done.
 * This is easier to program than to explain.
 */

void
print_tokenlist_type (halfword token_typ, halfword location, halfword macroname) {
  /* module 314 */
  switch (token_typ) {
  case parameter:
	print_nl_string("<argument> ");
	break;
  case u_template:
  case v_template:
	print_nl_string("<template> ");
	break;
  case backed_up:
	if (location == null) {
	  print_nl_string("<recently read> ");
	} else {
	  print_nl_string("<to be read again> ");
	};
	break;
  case inserted:
	print_nl_string("<inserted text> ");
	break;
  case macro:
	print_ln();
	print_cs (macroname);
	break;
  case output_text:
	print_nl_string("<output> ");
	break;
  case every_par_text:
	print_nl_string("<everypar> ");
	break;
  case every_math_text:
	print_nl_string("<everymath> ");
	break;
  case every_display_text:
	print_nl_string("<everydisplay> ");
	break;
  case every_hbox_text:
	print_nl_string("<everyhbox> ");
	break;
  case every_vbox_text:
	print_nl_string("<everyvbox> ");
	break;
  case every_job_text:
	print_nl_string("<everyjob> ");
	break;
  case every_cr_text:
	print_nl_string("<everycr> ");
	break;
  case every_eof_text:
	print_nl_string("<everyeof> ");
	break;
  case mark_text:
	print_nl_string("<mark> ");
	break;
  case write_text:
	print_nl_string("<write> ");
	break;
  case pdf_pages_attr_text:
	print_nl_string("<pdfpagesattr> ");
	break;
  case pdf_page_attr_text:
	print_nl_string("<pdfpageattr> ");
	break;
  case pdf_page_resources_text:
	print_nl_string("<pdfpageresources> ");
	break;
  default:
	print_nl_string ("?"); /* this should never happen */
  };
}

void
show_context (void) { /* prints where the scanner is */
  unsigned char old_setting;	/* saved |selector| setting */
  int nn;			/* number of contexts shown so far, less one */
  boolean bottom_line;		/* have we reached the final context to be shown? */
  /* begin expansion of Local variables for formatting calculations */
  /* module 315 */
  unsigned int i;		/* index into |buffer| */
  unsigned int j;		/* end of current line in |buffer| */
  unsigned int l = tally;		/* length of descriptive information on line 1 */
  int m;			/* context information gathered for line 2 */
  unsigned int n;		/* length of line 1 */
  int p;			/* starting or ending place in |trick_buf| */
  int q;			/* temporary index */
  /* end expansion of Local variables for formatting calculations */
  base_ptr = input_ptr;
  input_stack[base_ptr] = cur_input;	/* store current state */
  nn = -1;
  bottom_line = false;
  loop {
    cur_input = input_stack[base_ptr];	/* enter into the context */
    if ((state != token_list)) {
      if ((name > 19) || (base_ptr == 0)) {
		bottom_line = true;
	  }
	}
    if ((base_ptr == input_ptr) || bottom_line || (nn < error_context_lines)) {
	  /* begin expansion of Display the current context */
	  /* module 312 */
	  if ((base_ptr == input_ptr) || (state != token_list)
		  || (token_type != backed_up) || (loc != null)) {
	    /* we omit backed-up token lists that have already been read */
	    tally = 0; /* get ready to count characters */
	    old_setting = selector;
		if (state != token_list) {
		  /* begin expansion of Print location of current line */
		  /* module 313 */
		  /* This routine should be changed, if necessary, to give the best possible
		   * indication of where the current line resides in the input file.
		   * For example, on some systems it is best to zprint both a page and line number.
		   */
		  if (name <= 17) {
		    if (terminal_input) {
			  if (base_ptr == 0) {
				print_nl_string("<*>");
			  } else {
			    print_nl_string("<insert> ");
			  };
			} else {
			  print_nl_string("<read ");
			  if (name == 17) {
			    print_char ('*');
			  } else {
			    print_int (name - 1);
			  };
			  print_char ('>');
			};
		  } else if (index != in_open) { /* input from a pseudo file */
		    print_nl_string("l.");
		    print_int (line_stack[index + 1]);
		  } else {
		    print_nl_string("l.");
		    print_int (line);
		  };
		  print_char (' ');
		  /* end expansion of Print location of current line */
		  /* begin expansion of Pseudoprint the line */
		  /* module 318 */
		  /* But the trick is distracting us from our current goal, which is to
		   * understand the input state. So let's concentrate on the data structures that
		   * are being pseudoprinted as we finish up the |show_context| procedure.
		   */
		  begin_pseudoprint;
		  if (buffer[limit] == end_line_char) {
		    j = limit;
		  } else {
		    j = limit + 1;
		  }
		  /* determine the effective end of the line */
		  if (j > 0)
			for (i = start; i <= j - 1; i++) {
		      if (i == (unsigned)loc)
				set_trick_count;
		      zprint (buffer[i]);
		    };
		  /* end expansion of Pseudoprint the line */
		} else {
		  /* Print type of token list */
		  print_tokenlist_type(token_type,loc,name);
		  /* begin expansion of Pseudoprint the token list */
		  /* module 319 */
		  begin_pseudoprint;
		  if (token_type < macro) {
		    show_token_list (start, loc, 100000);
		  } else {
		    show_token_list (link (start), loc, 100000);	/* avoid reference count */
		  };
		  /* end expansion of Pseudoprint the token list */
		};
	    selector = old_setting; /* stop pseudoprinting */
	    /* begin expansion of Print two lines using the tricky pseudoprinted information */
		/* module 317 */
		/* And the following code uses the information after it has been gathered. */
	    if (trick_count == 1000000)
	      set_trick_count;   /* |set_trick_count| must be performed */
	    if (tally < trick_count) {
		  m = tally - first_count;
		} else {
		  m = trick_count - first_count;
		};
	    /* context on line 2 */
	    if (l + first_count <= (unsigned)half_error_line) {
		  p = 0;
		  n = l + first_count;
		} else {
		  zprint_string("...");
		  p = l + first_count - half_error_line + 3;
		  n = half_error_line;
		};
	    for (q = p; q <= first_count - 1; q++)
	      print_char (trick_buf[q % error_line]);
	    print_ln();
	    for (q = 1; q <= (int)n; q++)
	      print_char (' '); /* print |n| spaces to begin line~2 */
	    if (m + n <= (unsigned)error_line) {
		  p = first_count + m;
		} else {
		  p = first_count + (error_line - n - 3);
		};
	    for (q = first_count; q <= p - 1; q++)
	      print_char (trick_buf[q % error_line]);
	    if (m + n > (unsigned)error_line)
	      zprint_string("...");
	    /* end expansion of Print two lines using the tricky pseudoprinted information */
	    incr (nn);
	  };
	  /* end expansion of Display the current context */
	} else if (nn == error_context_lines) {
	  print_nl_string("...");
	  incr (nn); /* omitted if |error_context_lines<0| */
	};
    if (bottom_line)
      goto DONE;
    decr (base_ptr);
  };
 DONE:
  cur_input = input_stack[input_ptr];  /* restore original state */
}



/* module 323 */

/* Here is a procedure that starts a new level of token-list input, given
 * a token list |p| and its type |t|. If |t=macro|, the calling routine should
 * set |name| and |loc|.
 */
void
begin_token_list (pointer p, quarterword t) {
  push_input;
  state = token_list;
  start = p;
  token_type = t;
  if (t >= macro) {	/* the token list starts with a reference count */
	add_token_ref (p);
	if (t == macro) {
	  param_start = param_ptr;
	} else {
	  loc = link (p);
	  if (tracing_macros > 1) {
		begin_diagnostic();
		print_nl_string("");
		switch (t) {
		case mark_text:
		  print_esc_string ("mark");
		  break;
		case write_text:
		  print_esc_string ("write");
		  break;
		default:
		  print_cmd_chr (assign_toks, t - output_text + output_routine_loc);
		};
		zprint_string("->");
		token_show (p);
		end_diagnostic (false);
	  };
	};
  } else {
	loc = p;
  };
};


/* module 324 */

/* When a token list has been fully scanned, the following computations
 * should be done as we leave that level of input. The |token_type| tends
 * to be equal to either |backed_up| or |inserted| about 2/3 of the time.
 */
void
end_token_list (void) {	/* leave a token-list input level */
  if (token_type >= backed_up) { /* token list to be deleted */
      if (token_type <= inserted) {
		flush_list (start);
	  } else {
		delete_token_ref (start); /* update reference count */
		if (token_type == macro)	    /* parameters must be flushed */
		  while (param_ptr > (unsigned)param_start) {
			decr (param_ptr);
			flush_list (param_stack[param_ptr]);
	      };
	  };
  } else if (token_type == u_template) {
    if (align_state > 500000) {
	  align_state = 0;
	} else {
	  fatal_error  ("(interwoven alignment preambles are not allowed)");
	};
  };
  pop_input;
  check_interrupt;
};



/* module 325 */

/* Sometimes \TeX\ has read too far and wants to ``unscan'' what it has
 * seen. The |back_input| procedure takes care of this by putting the token
 * just scanned back into the input stream, ready to be read again. This
 * procedure can be used only if |cur_tok| represents the token to be
 * replaced. Some applications of \TeX\ use this procedure a lot,
 * so it has been slightly optimized for speed.
 */
void
back_input (void) {	/* undoes one token of input */
  pointer p;		/* a token list of length one */
  while ((state == token_list) && (loc == null) && (token_type != v_template))
    end_token_list();		/* conserve stack space */
  p = get_avail ();
  info (p) = cur_tok;
  if (cur_tok < right_brace_limit) {
    if (cur_tok < left_brace_limit) {
      decr (align_state);
    } else {
      incr (align_state);
    };
  }
  push_input;
  state = token_list;
  start = p;
  token_type = backed_up;
  loc = p;
  /* that was |back_list(p)|, without procedure overhead */
};

/* module 331 */
void
cmdchr_initialize (void) {
  input_ptr = 0;
  max_in_stack = 0;
  in_open = 0;
  open_parens = 0;
  max_buf_stack = 0;
  grp_stack[0] = 0;
  if_stack[0] = null;
  param_ptr = 0;
  max_param_stack = 0;
  first = buf_size;
  do {
    buffer[first] = 0;
    decr (first);
  } while (first != 0);
  scanner_status = normal;
  warning_index = null;
  first = 1;
  state = new_line;
  start = 1;
  index = 0;
  line = 0;
  name = 0;
  force_eof = false;
  shown_mode = 0;
}

void
show_token_list_debug (int p, int q, int l) {
  int m, c;			/* pieces of a token */
  ASCII_code match_chr;		/* character used in a `|match|' */
  ASCII_code n;			/* the highest parameter number, as an ASCII digit */
  match_chr = '#';
  n = '0';
  tally = q; /* nonsense but removes gcc warning */
  tally = 0;
  while ((p != null) && (tally < l)) {
      if ((p < hi_mem_min) || (p > mem_end)) {
		print_esc_string ("CLOBBERED.");
		return;
	  };
	  zprint_string("{m=");
      if (info (p) >= cs_token_flag) {
		print_int (info (p) - cs_token_flag);
		zprint_string(",n=");
		print_cs (info (p) - cs_token_flag);
	  } else {
		m = info (p) / 256;
		c = info (p) % 256;
		print_int(m);
		zprint_string(",n=");
		print_int(n);
	  };
	  zprint_string("}");
      p = link (p);
  };
  if (p != null)
    print_esc_string ("ETC.");
};




void
print_input_stack (void) {
  unsigned save_base_ptr;
  unsigned k;
  in_state_record rec;
  for (k=0; k<=max_in_stack;k++) {
	rec = input_stack[k];
	if (rec.state_field==0) { /* tokenlist */
	  fprintf (stdout, "input level %2d: toklist,index=%d,start=%d,loc=%d,limit=%d\n",
			   k,
			   rec.index_field,rec.loc_field,
			   rec.start_field,rec.limit_field);  
	  print_tokenlist_type (rec.index_field,rec.loc_field,rec.name_field);
	  fprintf (stdout, "-----------------\n");
	  show_token_list_debug(rec.start_field,null,maxint);
	  fprintf (stdout, "\n-----------------\n");
	} else if (rec.state_field==1) {
	  fprintf (stdout, "input level %2d: file   ,index=%d,start=%d,loc=%d,limit=%d,name=%d\n",
			 k,
			 rec.index_field,
			 rec.start_field,rec.loc_field,rec.limit_field,rec.name_field);  
	} else {
	  fprintf (stdout, "input level %2d: state=%d,index=%d,start=%d,loc=%d,limit=%d,name=%d\n",
			 k,
			 rec.state_field,rec.index_field,
			 rec.start_field,rec.loc_field,rec.limit_field,rec.name_field);  
	}
  }
  save_base_ptr = base_ptr;
  base_ptr = 0;
  show_context();
  base_ptr = save_base_ptr;
}
