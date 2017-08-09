
#include "types.h"
#include "c-compat.h"

#include "globals.h"

/* forwards */
static boolean privileged (void);
static boolean its_all_over (void);
static void delete_last (void);
static void issue_message (void);
static void shift_case (void);
static void handle_right_brace (void);

/* module 1177 */
internal_font_number main_f; /* the current font */
four_quarters main_i; /* character information bytes for |cur_l| */
four_quarters main_j; /* ligature/kern command */
font_index main_k; /* index into |font_info| */
pointer main_p; /* temporary register for list manipulation */
int main_s; /* space factor value */
halfword bchar; /* right boundary character of current font, or |non_char| */
halfword false_bchar; /* nonexistent character matching |bchar|,  or |non_char| */
boolean cancel_boundary; /* should the left boundary be ignored? */
boolean ins_disc; /* should we insert a discretionary node? */


/* module comment 1174 */

/* 
 * We come now to the |main_control| routine, which contains the master
 * switch that causes all the various pieces of \TeX\ to do their things,
 * in the right order.
 * 
 * In a sense, this is the grand climax of the program: It applies all the
 * tools that we have worked so hard to construct. In another sense, this is
 * the messiest part of the program: It necessarily refers to other pieces
 * of code all over the place, so that a person can't fully understand what is
 * going on without paging back and forth to be reminded of conventions that
 * are defined elsewhere. We are now at the hub of the web, the central nervous
 * system that touches most of the other parts and ties them together.
 * 
 * The structure of |main_control| itself is quite simple. There's a label
 * called |BIG_SWITCH|, at which point the next token of input is fetched
 * using |get_x_token|. Then the program branches at high speed into one of
 * about 100 possible directions, based on the value of the current
 * mode and the newly fetched command code; the sum |abs(mode)+cur_cmd|
 * indicates what to do next. For example, the case `|vmode+letter|' arises
 * when a letter occurs in vertical mode (or internal vertical mode); this
 * case leads to instructions that initialize a new paragraph and enter
 * horizontal mode.
 * 
 * The big |case| statement that contains this multiway switch has been labeled
 * |reswitch|, so that the program can |goto reswitch| when the next token
 * has already been fetched. Most of the cases are quite short; they call
 * an ``action procedure'' that does the work for that case, and then they
 * either |goto reswitch| or they ``fall through'' to the end of the |case|
 * statement, which returns control back to |BIG_SWITCH|. Thus, |main_control|
 * is not an extremely large procedure, in spite of the multiplicity of things
 * it must do; it is small enough to be handled by \PASCAL\ compilers that put
 * severe restrictions on procedure size.
 * 
 * One case is singled out for special treatment, because it accounts for most
 * of \TeX's activities in typical applications. The process of reading simple
 * text and converting it into |char_node| records, while looking for ligatures
 * and kerns, is part of \TeX's ``inner loop''; the whole program runs
 * efficiently when its inner loop is fast, so this part has been written
 * with particular care.
 */

/* module 1179 */
#define adjust_space_factor  main_s   =  sf_code ( cur_chr );        \
    if (  main_s  == 1000  )   { space_factor   = 1000;              \
    } else  if (  main_s  < 1000  )  {                               \
      if (  main_s  > 0  )   space_factor   =  main_s ;}             \
      else if (  space_factor  < 1000  )  { space_factor   = 1000; } \
      else   space_factor   =  main_s

/* module 1180 */
/* If the current horizontal list is empty, the reference to |character(tail)|
 * here is not strictly legal, since |tail| will be a node freshly returned by
 * |get_avail|. But this should cause no problem on most implementations, and we
 * do want the inner loop to be fast.
 * 
 * A discretionary break is not inserted for an explicit hyphen when we are in
 * restricted horizontal mode. In particular, this avoids putting discretionary
 * nodes inside of other discretionaries.
 */
/* Make a ligature node, if |ligature_present|; insert a null discretionary, if appropriate */
#define pack_lig( arg ) {                                                    \
   main_p   =  new_ligature ( main_f , cur_l , link ( cur_q ));              \
   if (  lft_hit  )  { subtype ( main_p )  = 2 ; lft_hit   =  false ;};      \
   if (  arg  ) { if (  lig_stack  ==  null  )  {                            \
      incr ( subtype ( main_p ));                                            \
      rt_hit   =  false ;};};                                                \
   link ( cur_q )  =  main_p ;                                               \
   tail   =  main_p ;                                                        \
   ligature_present   =  false ;}

#define wrapup( arg )   if (  cur_l  <  non_char  )  {                       \
        if (  character ( tail ) ==  qi ( hyphen_char [ main_f ]) )  {       \
          if (  link ( cur_q ) >  null  )  { ins_disc   =  true ; }}         \
        if (  ligature_present  )   pack_lig ( arg );                        \
        if (  ins_disc  )  { ins_disc   =  false ;                           \
          if (  mode  > 0  )   tail_append ( new_disc ());};}


/* module 1177 */

/* The following part of the program was first written in a structured
 * manner, according to the philosophy that ``premature optimization is
 * the root of all evil.'' Then it was rearranged into pieces of
 * spaghetti so that the most common actions could proceed with little or
 * no redundancy.
 * 
 * The original unoptimized form of this algorithm resembles the
 * |reconstitute| procedure, which was described earlier in connection with
 * hyphenation. Again we have an implied ``cursor'' between characters
 * |cur_l| and |cur_r|. The main difference is that the |lig_stack| can now
 * contain a charnode as well as pseudo-ligatures; that stack is now
 * usually nonempty, because the next character of input (if any) has been
 * appended to it. In |main_control| we have
 * $$|cur_r|=\cases{|character(lig_stack)|,&if |lig_stack>null|;\cr
 * 
 * |font_bchar[cur_font]|,&otherwise;\cr}$$
 * except when |character(lig_stack)=font_false_bchar[cur_font]|.
 * Several additional global variables are needed.
 */

#define ANY_MODE( arg )           vmode  +  arg: case hmode  +  arg: case mmode  +  arg
#define NON_MATH( arg )           vmode  +  arg: case hmode  +  arg

void 
main_control (void) {	 /* governs \TeX's activities */
  int t; /* general-purpose temporary variable */ 
  if (every_job!= null)
	begin_token_list (every_job, every_job_text);
 BIG_SWITCH: get_x_token();
 RESWITCH:
  /* begin expansion of Give diagnostic information, if requested */
  /* module 1176 */
  /* When a new token has just been fetched at |BIG_SWITCH|, we have an
   * ideal place to monitor \TeX's activity.
   */  
  if (interrupt != 0)
	if (OK_to_interrupt) {
	  back_input();
	  check_interrupt;
	  goto BIG_SWITCH;
	};
#ifdef TEXMF_DEBUG
  if (panicking)
	check_mem (false);
#endif /* TEXMF_DEBUG */
  if (tracing_commands > 0)
	show_cur_cmd_chr();
  /* end expansion of Give diagnostic information, if requested */
  switch (abs (mode) + cur_cmd) {
  case hmode + letter:
  case hmode + other_char:
  case hmode + char_given:
	do_something;
	goto MAIN_LOOP;
  case hmode + char_num:
	scan_char_num();
	cur_chr = cur_val;
	goto MAIN_LOOP;
  case hmode + no_boundary:
	get_x_token();
	if ((cur_cmd == letter) || (cur_cmd == other_char)
		|| (cur_cmd == char_given) || (cur_cmd == char_num))
	  cancel_boundary = true;
	goto RESWITCH;
  case hmode + spacer:
	if (space_factor == 1000) {
	  goto APPEND_NORMAL_SPACE;
	} else {
	  app_space();
	}
	break;
  case hmode + ex_space:
  case mmode + ex_space:
	do_something;
	goto APPEND_NORMAL_SPACE;
	/* begin expansion of Cases of |main_control| that are not part of the inner loop */
	/* module 1190 */
	/* Whew---that covers the main loop. We can now proceed at a leisurely
	 * pace through the other combinations of possibilities.
	 */
  case ANY_MODE (relax):
  case vmode + spacer:
  case mmode + spacer:
  case mmode + no_boundary:
	do_something;
	break;
  case ANY_MODE (ignore_spaces):	
	/* Get the next non-blank non-call... */
	get_nblank_ncall;
    do_something;
	goto RESWITCH;
  case vmode + stop:
	if (its_all_over()) {
	  return; /* this is the only way out */
	} else {
	  do_something;
	}
	break;
	/* begin expansion of Forbidden cases detected in |main_control| */
	/* module 1193 */
	/* When erroneous situations arise, \TeX\ usually issues an error message
	 * specific to the particular error. For example, `\.{\\noalign}' should
	 * not appear in any mode, since it is recognized by the |align_peek| routine
	 * in all of its legitimate appearances; a special error message is given
	 * when `\.{\\noalign}' occurs elsewhere. But sometimes the most appropriate
	 * error message is simply that the user is not allowed to do what he or she
	 * has attempted. For example, `\.{\\moveleft}' is allowed only in vertical mode,
	 * and `\.{\\lower}' only in non-vertical modes. Such cases are enumerated
	 * here and in the other sections referred to under `See also \dots.'
	 */
	/* module 1243 */
	/* module 1256 */
	/* module 1289 */
  case vmode + vmove:
  case hmode + hmove:
  case mmode + hmove:
  case ANY_MODE (last_item):
  case vmode + vadjust:
  case vmode + ital_corr:
  case NON_MATH (eq_no):
  case ANY_MODE (mac_param):
	report_illegal_case(); 
	break;
	/* end expansion of Forbidden cases detected in |main_control| */
	/* begin expansion of Math-only cases in non-math modes, or vice versa */
	/* module 1191 */
	/* Here is a list of cases where the user has probably gotten into or out of math
	 * mode by mistake. \TeX\ will insert a dollar sign and rescan the current token.
	 */
  case NON_MATH (sup_mark):
  case NON_MATH (sub_mark):
  case NON_MATH (math_char_num):
  case NON_MATH (math_given):
  case NON_MATH (math_comp):
  case NON_MATH (delim_num):
  case NON_MATH (left_right):
  case NON_MATH (above):
  case NON_MATH (radical):
  case NON_MATH (math_style):
  case NON_MATH (math_choice):
  case NON_MATH (vcenter):
  case NON_MATH (non_script):
  case NON_MATH (mkern):
  case NON_MATH (limit_switch):
  case NON_MATH (mskip):
  case NON_MATH (math_accent):
  case mmode + endv:
  case mmode + par_end:
  case mmode + stop:
  case mmode + vskip:
  case mmode + un_vbox:
  case mmode + valign:
  case mmode + hrule:
	insert_dollar_sign();
	break;
	/* end expansion of Math-only cases in non-math modes, or vice versa */
	/* begin expansion of Cases of |main_control| that build boxes and lists */
	/* module 1200 */
	/* 
	 * The most important parts of |main_control| are concerned with \TeX's
	 * chief mission of box-making. We need to control the activities that put
	 * entries on vlists and hlists, as well as the activities that convert
	 * those lists into boxes. All of the necessary machinery has already been
	 * developed; it remains for us to ``push the buttons'' at the right times.
	 */	
	/* module 1201 */
	/* As an introduction to these routines, let's consider one of the simplest
	 * cases: What happens when `\.{\\hrule}' occurs in vertical mode, or
	 * `\.{\\vrule}' in horizontal mode or math mode? The code in |main_control|
	 * is short, since the |scan_rule_spec| routine already does most of what is
	 * required; thus, there is no need for a special action procedure.
	 * 
	 * Note that baselineskip calculations are disabled after a rule in vertical
	 * mode, by setting |prev_depth:=ignore_depth|.
	 */
  case vmode + hrule:
  case hmode + vrule:
  case mmode + vrule:
	tail_append (scan_rule_spec());
	if (abs (mode) == vmode) {
	  prev_depth = ignore_depth;
	} else if (abs (mode) == hmode) {
	  space_factor = 1000;
	};
	break;
	/* module 1202 */
	/* The processing of things like \.{\\hskip} and \.{\\vskip} is slightly
	 * more complicated. But the code in |main_control| is very short, since
	 * it simply calls on the action routine |append_glue|. Similarly, \.{\\kern}
	 * activates |append_kern|.
	 */
  case vmode + vskip:
  case hmode + hskip:
  case mmode + hskip:
  case mmode + mskip:
	append_glue();
	break;
  case ANY_MODE (kern):
  case mmode + mkern:
	append_kern();
	break;
	/* module 1207 */
	/* Many of the actions related to box-making are triggered by the appearance
	 * of braces in the input. For example, when the user says `\.{\\hbox}
	 * \.{to} \.{100pt\{$\langle\,\hbox{hlist}\,\rangle$\}}' in vertical mode,
	 * the information about the box size (100pt, |exactly|) is put onto |save_stack|
	 * with a level boundary word just above it, and |cur_group:=adjusted_hbox_group|;
	 * \TeX\ enters restricted horizontal mode to process the hlist. The right
	 * brace eventually causes |save_stack| to be restored to its former state,
	 * at which time the information about the box size (100pt, |exactly|) is
	 * available once again; a box is packaged and we leave restricted horizontal
	 * mode, appending the new box to the current list of the enclosing mode
	 * (in this case to the current list of vertical mode), followed by any
	 * vertical adjustments that were removed from the box by |hpack|.
	 * 
	 * The next few sections of the program are therefore concerned with the
	 * treatment of left and right curly braces.
	 */
	/* module 1208 */
	/* If a left brace occurs in the middle of a page or paragraph, it simply
	 * introduces a new level of grouping, and the matching right brace will not have
	 * such a drastic effect. Such grouping affects neither the mode nor the
	 * current list.
	 */
  case NON_MATH (left_brace):
	new_save_level (simple_group);
	break;
  case ANY_MODE (begin_group):
	new_save_level (semi_simple_group);
	break;
  case ANY_MODE (end_group):
	if (cur_group == semi_simple_group) {
	  unsave();
	} else {
	  off_save();
	};
	break;
	/* module 1212 */
	/* The routine for a |right_brace| character branches into many subcases,
	 * since a variety of things may happen, depending on |cur_group|. Some
	 * types of groups are not supposed to be ended by a right brace; error
	 * messages are given in hopes of pinpointing the problem. Most branches
	 * of this routine will be filled in later, when we are ready to understand
	 * them; meanwhile, we must prepare ourselves to deal with such errors.
	 */
  case ANY_MODE (right_brace):
	handle_right_brace();
	break;
	/* module 1218 */
	/* Constructions that require a box are started by calling |scan_box| with
	 * a specified context code. The |scan_box| routine verifies
	 * that a |make_box| command comes next and then it calls |begin_box|.
	 */
  case vmode + hmove:
  case hmode + vmove:
  case mmode + vmove:
	t = cur_chr;
	scan_normal_dimen;
	if (t == 0) {
	  scan_box (cur_val);
	} else {
	  scan_box (-cur_val);
	};
	break;
  case ANY_MODE (leader_ship):
	scan_box (leader_flag - a_leaders + cur_chr);
	break;
  case ANY_MODE (make_box):
	begin_box (0);
	break;
	/* module 1235 */
  case vmode + start_par:
	new_graf (cur_chr > 0);
	break;
  case vmode + letter:
  case vmode + other_char:
  case vmode + char_num:
  case vmode + char_given:
  case vmode + math_shift:
  case vmode + un_hbox:
  case vmode + vrule:
  case vmode + accent:
  case vmode + discretionary:
  case vmode + hskip:
  case vmode + valign:
  case vmode + ex_space:
  case vmode + no_boundary:
	back_input();
	new_graf (true);
	break;
	/* module 1237 */
  case hmode + start_par:
  case mmode + start_par:
	indent_in_hmode();
	break;
	/* module 1239 */
	/* A paragraph ends when a |par_end| command is sensed, or when we are in
	 * horizontal mode when reaching the right brace of vertical-mode routines
	 * like \.{\\vbox}, \.{\\insert}, or \.{\\output}.
	 */
  case vmode + par_end:
	normal_paragraph();
	if (mode > 0)
	  build_page();
	break;
  case hmode + par_end:
	if (align_state < 0)
	  off_save();	  /* this tries to recover from an alignment that didn't end properly */
	end_graf(); /* this takes us to the enclosing mode, if |mode>0| */
	if (mode == vmode)
	  build_page();
	break;
  case hmode + stop:
  case hmode + vskip:
  case hmode + hrule:
  case hmode + un_vbox:
  case hmode + halign:
	head_for_vmode();
	break;
	/* module 1242 */
	/* Insertion and adjustment and mark nodes are constructed by the following
	 * pieces of the program.
	 */
  case ANY_MODE (insert):
  case hmode + vadjust:
  case mmode + vadjust:
	begin_insert_or_adjust();
	break;
  case ANY_MODE (mark):
	make_mark();
	break;
	/* module 1247 */
	/* Penalty nodes get into a list via the |break_penalty| command. */
  case ANY_MODE (break_penalty):
	append_penalty();
	break;
	/* module 1249 */
	/* The |remove_item| command removes a penalty, kern, or glue node if it
	 * appears at the tail of the current list, using a brute-force linear scan.
	 * Like \.{\\lastbox}, this command is not allowed in vertical mode (except
	 * internal vertical mode), since the current list in vertical mode is sent
	 * to the page builder. But if we happen to be able to implement it in
	 * vertical mode, we do.
	 */
  case ANY_MODE (remove_item):
	delete_last();
	break;
	/* module 1254 */
	/* The |un_hbox| and |un_vbox| commands unwrap one of the 256 current boxes. */
  case vmode + un_vbox:
  case hmode + un_hbox:
  case mmode + un_hbox:
	unpackage();
	break;
	/* module 1257 */	
	/* Italic corrections are converted to kern nodes when the |ital_corr| command
	 * follows a character. In math mode the same effect is achieved by appending
	 * a kern of zero here, since italic corrections are supplied later.
	 */
  case hmode + ital_corr:
	append_italic_correction();
	break;
  case mmode + ital_corr:
	tail_append (new_kern (0));
	break;
	/* module 1261 */
  case hmode + discretionary:
  case mmode + discretionary:
	append_discretionary();
	break;
	/* module 1267 */
	/* We need only one more thing to complete the horizontal mode routines, namely
	 * the \.{\\accent} primitive.
	 */
  case hmode + accent:
	make_accent();
	break;
	/* module 1271 */
	/* When `\.{\\cr}' or `\.{\\span}' or a tab mark comes through the scanner
	 * into |main_control|, it might be that the user has foolishly inserted
	 * one of them into something that has nothing to do with alignment. But it is
	 * far more likely that a left brace or right brace has been omitted, since
	 * |get_next| takes actions appropriate to alignment only when `\.{\\cr}'
	 * or `\.{\\span}' or tab marks occur with |align_state=0|. The following
	 * program attempts to make an appropriate recovery.
	 */
  case ANY_MODE (car_ret):
  case ANY_MODE (tab_mark):
	align_error();
	break;
  case ANY_MODE (no_align):
	no_align_error();
	break;
  case ANY_MODE (omit):
	omit_error();
	break;
	/* module 1275 */
	/* An |align_group| code is supposed to remain on the |save_stack|
	 * during an entire alignment, until |fin_align| removes it.
	 * 
	 * A devious user might force an |endv| command to occur just about anywhere;
	 * we must defeat such hacks.
	 */
  case vmode + halign:
	init_align();
	break;
  case hmode + valign:
	/* begin expansion of Cases of |main_control| for |hmode+valign| */
	/* module 1649 */
	if (cur_chr > 0) {
	  if (eTeX_enabled (TeXXeT_en, cur_cmd, cur_chr))
		tail_append (new_math (0, cur_chr));
	} else {
	  init_align();
	}
	/* end expansion of Cases of |main_control| for |hmode+valign| */
	break;
  case mmode + halign:
	if (privileged()) {
	  if (cur_group == math_shift_group) {
		init_align();
	  } else {
		off_save();
	  }
	}
	break;
  case vmode + endv:
  case hmode + endv:
	do_endv();
	break;
	/* module 1279 */
	/* Finally, \.{\\endcsname} is not supposed to get through to |main_control|.  */
  case ANY_MODE (end_cs_name):
	cs_error();
	break;
	/* module 1282 */
	/* We get into math mode from horizontal mode when a `\.\$' (i.e., a
	 * |math_shift| character) is scanned. We must check to see whether this
	 * `\.\$' is immediately followed by another, in case display math mode is
	 * called for.
	 */
	case hmode + math_shift:
	  init_math();
	  break;
	  /* module 1285 */
	  /* We get into ordinary math mode from display math mode when `\.{\\eqno}' or
	   * `\.{\\leqno}' appears. In such cases |cur_chr| will be 0 or~1, respectively;
	   * the value of |cur_chr| is placed onto |save_stack| for safe keeping.
	   */
  case mmode + eq_no:
	if (privileged()) {
	  if (cur_group == math_shift_group) {
		start_eq_no();
	  } else {
		off_save();
	  }
	}
	break;
	/* module 1295 */
	/* Subformulas of math formulas cause a new level of math mode to be entered,
	 * on the semantic nest as well as the save stack. These subformulas arise in
	 * several ways: (1)~A left brace by itself indicates the beginning of a
	 * subformula that will be put into a box, thereby freezing its glue and
	 * preventing line breaks. (2)~A subscript or superscript is treated as a
	 * subformula if it is not a single character; the same applies to
	 * the nucleus of things like \.{\\underline}. (3)~The \.{\\left} primitive
	 * initiates a subformula that will be terminated by a matching \.{\\right}.
	 * The group codes placed on |save_stack| in these three cases are
	 * |math_group|, |math_group|, and |math_left_group|, respectively.
	 * 
	 * Here is the code that handles case (1); the other cases are not quite as
	 * trivial, so we shall consider them later.
	 */
  case mmode + left_brace:
	tail_append (new_noad());
	back_input();
	scan_math (nucleus (tail));
	break;
	/* module 1299 */
	/* The simplest math formula is, of course, `\.{\${ }\$}', when no noads are
	 * generated. The next simplest cases involve a single character, e.g.,
	 * `\.{\$x\$}'. Even though such cases may not seem to be very interesting,
	 * the reader can perhaps understand how happy the author was when `\.{\$x\$}'
	 * was first properly typeset by \TeX. The code in this section was used.
	 */
  case mmode + letter:
  case mmode + other_char:
  case mmode + char_given:
	set_math_char (ho (math_code (cur_chr)));
	break;
  case mmode + char_num:
	scan_char_num();
	cur_chr = cur_val;
	set_math_char (ho (math_code (cur_chr)));
	break;
  case mmode + math_char_num:
	scan_fifteen_bit_int();
	set_math_char (cur_val);
	break;
  case mmode + math_given:
	set_math_char (cur_chr);
	break;
  case mmode + delim_num:
	scan_twenty_seven_bit_int();
	set_math_char (cur_val / 4096);
	break;
	/* module 1303 */
  case mmode + math_comp:
	tail_append (new_noad());
	type (tail) = cur_chr;
	scan_math (nucleus (tail));
	break;
  case mmode + limit_switch:
	math_limit_switch();
	break;
	/* module 1307 */
  case mmode + radical:
	math_radical();
	break;
	/* module 1309 */
  case mmode + accent:
  case mmode + math_accent:
	math_ac();
	break;
	/* module 1312 */
  case mmode + vcenter:
	scan_spec (vcenter_group, false);
	normal_paragraph();
	push_nest();
	mode = -vmode;
	prev_depth = ignore_depth;
	if (every_vbox != null)
	  begin_token_list (every_vbox, every_vbox_text);
	break;
	/* module 1316 */
  case mmode + math_style:
	tail_append (new_style (cur_chr));
	break;
  case mmode + non_script:
	tail_append (new_glue (zero_glue));
	subtype (tail) = cond_math_glue;
	break;
  case mmode + math_choice:
	append_choices();
	break;
	/* module 1320 */
	/* Subscripts and superscripts are attached to the previous nucleus by the
	 * 
	 * action procedure called |sub_sup|. We use the facts that |sub_mark=sup_mark+1|
	 * and |subscr(p)=supscr(p)+1|.
	 */
  case mmode + sub_mark:
  case mmode + sup_mark:
	sub_sup();
	break;
	/* module 1325 */
  case mmode + above:
	math_fraction();
	break;
	/* module 1335 */
  case mmode + left_right:
	math_left_right();
	break;
	/* module 1338 */
	/* Here is the only way out of math mode. */
  case mmode + math_shift:
	if (cur_group == math_shift_group) {
	  after_math();
	} else {
	  off_save();
	}
	break;
	/* end expansion of Cases of |main_control| that build boxes and lists */
	/* begin expansion of Cases of |main_control| that don't depend on |mode| */
	/* module 1355 */
	/* Every prefix, and every command code that might or might not be prefixed,
	 * calls the action procedure |prefixed_command|. This routine accumulates
	 * a sequence of prefixes until coming to a non-prefix, then it carries out
	 * the command.
	 */
  case ANY_MODE (toks_register):
  case ANY_MODE (assign_toks):
  case ANY_MODE (assign_int):
  case ANY_MODE (assign_dimen):
  case ANY_MODE (assign_glue):
  case ANY_MODE (assign_mu_glue):
  case ANY_MODE (assign_font_dimen):
  case ANY_MODE (assign_font_int):
  case ANY_MODE (set_aux):
  case ANY_MODE (set_prev_graf):
  case ANY_MODE (set_page_dimen):
  case ANY_MODE (set_page_int):
  case ANY_MODE (set_box_dimen):
  case ANY_MODE (set_shape):
  case ANY_MODE (def_code):
  case ANY_MODE (def_family):
  case ANY_MODE (set_font):
  case ANY_MODE (def_font):
  case ANY_MODE (register_cmd):
  case ANY_MODE (advance):
  case ANY_MODE (multiply):
  case ANY_MODE (divide):
  case ANY_MODE (prefix):
  case ANY_MODE (let):
  case ANY_MODE (shorthand_def):
  case ANY_MODE (read_to_cs):
  case ANY_MODE (def):
  case ANY_MODE (set_box):
  case ANY_MODE (hyph_data):
  case ANY_MODE (set_interaction):
	prefixed_command();
	break;
	/* module 1413 */
  case ANY_MODE (after_assignment):
	get_token();
	after_token = cur_tok;
	break;
	/* module 1416 */
  case  ANY_MODE (after_group):
	get_token();
	save_for_after (cur_tok);
	break;
	/* module 1419 */
  case ANY_MODE (in_stream):
	open_or_close_in();
	break;
	/* module 1421 */
	/* The user can issue messages to the terminal, regardless of the
	 * current mode.
	 */
  case ANY_MODE (message):
	issue_message();
	break;
	/* module 1430 */
	/* The \.{\\uppercase} and \.{\\lowercase} commands are implemented by
	 * building a token list and then changing the cases of the letters in it.
	 */
  case ANY_MODE (case_shift):
	shift_case();
	break;
	/* module 1435 */
	/* We come finally to the last pieces missing from |main_control|, namely the
	 * `\.{\\show}' commands that are useful when debugging.
	 */
  case ANY_MODE (xray):
	show_whatever();
	break;
	/* end expansion of Cases of |main_control| that don't depend on |mode| */
	/* begin expansion of Cases of |main_control| that are for extensions to \TeX */
	/* module 1492 */
	/* When an |extension| command occurs in |main_control|, in any mode,
	 * the |do_extension| routine is called.
	 */
  case ANY_MODE (extension):
	do_extension();
	/* end expansion of Cases of |main_control| that are for extensions to \TeX */
	/* end expansion of Cases of |main_control| that are not part of the inner loop */
  }; /* of the big |case| statement */ 
  goto BIG_SWITCH;
 MAIN_LOOP:
  /* begin expansion of Append character |cur_chr| and the following characters (if~any)
	 to the current hlist in the current font; |goto reswitch| when a non-character has been fetched */
  /* module 1179 */
  /* We leave the |space_factor| unchanged if |sf_code(cur_chr)=0|; otherwise we
   * set it equal to |sf_code(cur_chr)|, except that it should never change
   * from a value less than 1000 to a value exceeding 1000. The most common
   * case is |sf_code(cur_chr)=1000|, so we want that case to be fast.
   * 
   * The overall structure of the main loop is presented here. Some program labels
   * are inside the individual sections.
   */  
  adjust_space_factor;
  main_f = cur_font;
  bchar = font_bchar[main_f];
  false_bchar = font_false_bchar[main_f];
  if (mode > 0)
	if (language != clang)
	  fix_language();
  fast_get_avail (lig_stack);
  font (lig_stack) = main_f;
  cur_l = qi (cur_chr);
  character (lig_stack) = cur_l;
  cur_q = tail;
  if (cancel_boundary) {
	cancel_boundary = false;
	main_k = non_address;
  } else {
	main_k = bchar_label[main_f];
  }
  if (main_k == non_address)
	goto MAIN_LOOP_MOVE2;  /* no left boundary processing */ 
  cur_r = cur_l;
  cur_l = non_char;
  goto MAIN_LIG_LOOP1;  /* begin with cursor after left boundary */ 
 MAIN_LOOP_WRAPUP:
  /* Make a ligature node, if |ligature_present|; insert a  null discretionary, if appropriate */
  wrapup (rt_hit);
 MAIN_LOOP_MOVE:
  /* begin expansion of If the cursor is immediately followed by the right boundary, |goto reswitch|;
	 if it's followed by an invalid character, |goto BIG_SWITCH|; otherwise move the cursor one step to 
	 the right and |goto MAIN_LIG_LOOP| */
  /* module 1181 */
  if (lig_stack == null)
	goto RESWITCH;
  cur_q = tail;
  cur_l = character (lig_stack);
 MAIN_LOOP_MOVE1:
  if (!is_char_node(lig_stack))
	goto MAIN_LOOP_MOVE_LIG;
 MAIN_LOOP_MOVE2:
  if ( (effective_char (false, main_f, cur_chr) > font_ec[main_f])
	   || 
	   (effective_char (false, main_f, cur_chr) < font_bc[main_f])) {
	char_warning (main_f, cur_chr);
	free_avail (lig_stack);
	goto BIG_SWITCH;
  };
  main_i = effective_char_info (main_f, cur_l);
  if (!char_exists (main_i)) {
	char_warning (main_f, cur_chr);
	free_avail (lig_stack);
	goto BIG_SWITCH;
  };
  tail_append (lig_stack);	/* |MAIN_LOOP_LOOKAHEAD| is next */
  /* end expansion of If the cursor is immediately followed by the right boundary, ...*/
 MAIN_LOOP_LOOKAHEAD:
  /* begin expansion of Look ahead for another character, or leave |lig_stack| empty if there's none there */
  /* module 1183 */
  /* The result of \.{\\char} can participate in a ligature or kern, so we must
   * look ahead for it.
   */
  get_next(); /* set only |cur_cmd| and |cur_chr|, for speed */
  if (cur_cmd == letter)
	goto MAIN_LOOP_LOOKAHEAD1;
  if (cur_cmd == other_char)
	goto MAIN_LOOP_LOOKAHEAD1;
  if (cur_cmd == char_given)
	goto MAIN_LOOP_LOOKAHEAD1;
  x_token(); /* now expand and set |cur_cmd|, |cur_chr|, |cur_tok| */
  if (cur_cmd == letter)
	goto MAIN_LOOP_LOOKAHEAD1;
  if (cur_cmd == other_char)
	goto MAIN_LOOP_LOOKAHEAD1;
  if (cur_cmd == char_given)
	goto MAIN_LOOP_LOOKAHEAD1;
  if (cur_cmd == char_num) {
	scan_char_num();
	cur_chr = cur_val;
	goto MAIN_LOOP_LOOKAHEAD1;
  };
  if (cur_cmd == no_boundary)
	bchar = non_char;
  cur_r = bchar;
  lig_stack = null;
  goto MAIN_LIG_LOOP;
 MAIN_LOOP_LOOKAHEAD1:
  adjust_space_factor;
  fast_get_avail (lig_stack);
  font (lig_stack) = main_f;
  cur_r = qi (cur_chr);
  character (lig_stack) = cur_r;
  if (cur_r == false_bchar)
	cur_r = non_char;	/* this prevents spurious ligatures */
  /* end expansion of Look ahead for another character, or leave |lig_stack| empty if there's none there */
 MAIN_LIG_LOOP:
  /* begin expansion of If there's a ligature/kern command relevant to |cur_l| and |cur_r|,
	 adjust the text appropriately; exit to |MAIN_LOOP_WRAPUP| */
  /* module 1184 */
  /* Even though comparatively few characters have a lig/kern program, several
   * of the instructions here count as part of \TeX's inner loop, since a
   * 
   * potentially long sequential search must be performed. For example, tests with
   * Computer Modern Roman showed that about 40 per cent of all characters
   * actually encountered in practice had a lig/kern program, and that about four
   * lig/kern commands were investigated for every such character.
   * 
   * At the beginning of this code we have |main_i=char_info(main_f,cur_l)|.
   */
  if (char_tag (main_i) != lig_tag)
	goto MAIN_LOOP_WRAPUP;
  main_k = lig_kern_start (main_f,main_i);
  main_j = font_info[main_k].qqqq;
  if (skip_byte (main_j) <= stop_flag)
	goto MAIN_LIG_LOOP2;
  main_k = lig_kern_restart (main_f,main_j);
 MAIN_LIG_LOOP1:
  main_j = font_info[main_k].qqqq;
 MAIN_LIG_LOOP2:
  if (next_char (main_j) == cur_r)
	if (skip_byte (main_j) <= stop_flag) {
	  /* begin expansion of Do ligature or kern command, returning to |MAIN_LIG_LOOP| or 
		 |MAIN_LOOP_WRAPUP| or |MAIN_LOOP_MOVE| */
	  /* module 1185 */
	  /* When a ligature or kern instruction matches a character, we know from
	   * |read_font_info| that the character exists in the font, even though we
	   * haven't verified its existence in the normal way.
	   * 
	   * This section could be made into a subroutine, if the code inside
	   * |main_control| needs to be shortened.
	   * 
	   * \chardef\?='174 % vertical line to indicate character retention
	   */
	  if (op_byte (main_j) >= kern_flag) {
		wrapup (rt_hit);
		tail_append (new_kern (char_kern (main_f, main_j)));
		goto MAIN_LOOP_MOVE;
	  };
	  if (cur_l == non_char) {
		lft_hit = true;
	  } else if (lig_stack == null) {
		rt_hit = true;
	  };
	  check_interrupt;	/* allow a way out in case there's an infinite ligature loop */
	  switch (op_byte (main_j)) {
	  case qi (1):
	  case qi (5):
		cur_l = rem_byte (main_j); /* \.{=:\?}, \.{=:\?>} */ 
		main_i = char_info (main_f,cur_l);
		ligature_present = true;
		break;
	  case qi (2):
	  case qi (6):
		cur_r = rem_byte (main_j); /* \.{\?=:}, \.{\?=:>} */ 
		if (lig_stack == null) { /* right boundary character is being consumed */
		  lig_stack = new_lig_item (cur_r);
		  bchar = non_char;
		} else if (is_char_node (lig_stack)) {	/* |link(lig_stack)=null| */
		  main_p = lig_stack;
		  lig_stack = new_lig_item (cur_r);
		  lig_ptr (lig_stack) = main_p;
		} else {
		  character (lig_stack) = cur_r;
		}
		break;
	  case qi (3):
		cur_r = rem_byte (main_j); /* \.{\?=:\?} */ 
		main_p = lig_stack;
		lig_stack = new_lig_item (cur_r);
		link (lig_stack) = main_p;
		break;
	  case qi (7):
	  case qi (11):
		wrapup (false); /* \.{\?=:\?>}, \.{\?=:\?>>} */ 
		cur_q = tail;
		cur_l = rem_byte (main_j);
		main_i = char_info (main_f,cur_l);
		ligature_present = true;
		break;
	  default:
		cur_l = rem_byte (main_j);
		ligature_present = true; /* \.{=:} */ 
		if (lig_stack == null) {
		  goto MAIN_LOOP_WRAPUP;
		} else {
		  do_something;
		  goto MAIN_LOOP_MOVE1;
		};
	  };
	  if (op_byte (main_j) > qi (4))
		if (op_byte (main_j) != qi (7))
		  goto MAIN_LOOP_WRAPUP;
	  if (cur_l < non_char)
		goto MAIN_LIG_LOOP;
	  main_k = bchar_label[main_f];
	  goto MAIN_LIG_LOOP1;
	};
  /* end expansion of Do ligature or kern command, returning to |main_lig_loop| or ... */
  if (skip_byte (main_j) == qi (0)) {
	incr (main_k);
  } else {
	if (skip_byte (main_j) >= stop_flag)
	  goto MAIN_LOOP_WRAPUP;
	main_k = main_k + qo (skip_byte (main_j)) + 1;
  };
  goto MAIN_LIG_LOOP1;
  /* end expansion of If there's a ligature/kern command relevant to |cur_l| and |cur_r|, ... */
 MAIN_LOOP_MOVE_LIG:
  /* begin expansion of Move the cursor past a pseudo-ligature, then 
	 |goto MAIN_LOOP_LOOKAHEAD| or |MAIN_LIG_LOOP| */
  /* module 1182 */
  /* Here we are at |MAIN_LOOP_move_lig|.
   * When we begin this code we have |cur_q=tail| and |cur_l=character(lig_stack)|.
   */
  main_p = lig_ptr (lig_stack);
  if (main_p > null)
	tail_append (main_p);
  temp_ptr = lig_stack;
  lig_stack = link (temp_ptr);
  free_node (temp_ptr, small_node_size);
  main_i = char_info (main_f,cur_l);
  ligature_present = true;
  if (lig_stack == null) {
	if (main_p > null) {
	  goto MAIN_LOOP_LOOKAHEAD;
	} else {
	  cur_r = bchar;
	} 
  } else {
	cur_r = character (lig_stack);
  }
  goto MAIN_LIG_LOOP;
  /* end expansion of Move the cursor past a pseudo-ligature, then ...*/
  /* end expansion of Append character |cur_chr| and the following characters .... */
 APPEND_NORMAL_SPACE:
  /* begin expansion of Append a normal inter-word space to the current list, then |goto BIG_SWITCH| */
  /* module 1186 */
  /* The occurrence of blank spaces is almost part of \TeX's inner loop,
   * 
   * since we usually encounter about one space for every five non-blank characters.
   * Therefore |main_control| gives second-highest priority to ordinary spaces.
   * 
   * When a glue parameter like \.{\\spaceskip} is set to `\.{0pt}', we will
   * see to it later that the corresponding glue specification is precisely
   * |zero_glue|, not merely a pointer to some specification that happens
   * to be full of zeroes. Therefore it is simple to test whether a glue parameter
   * is zero or~not.
   */
  if (space_skip == zero_glue) {
	/* Find the glue specification, |main_p|, for text spaces in the current font */
	find_glue_spec;
	temp_ptr = new_glue (main_p);
  } else {
	temp_ptr = new_param_glue (space_skip_code);
  }
  link (tail) = temp_ptr;
  tail = temp_ptr;
  goto BIG_SWITCH;
  /* end expansion of Append a normal inter-word space to the current list, then |goto BIG_SWITCH| */
}

/* module 1426 */

/* If \.{\\errmessage} occurs often in |scroll_mode|, without user-defined
 * \.{\\errhelp}, we don't want to give a long help message each time. So we
 * give a verbose explanation only once.
 */

boolean long_help_seen; /* has the long \.{\\errmessage} help been used? */


/* module 1178 */

/* The boolean variables of the main loop are normally false, and always reset
 * to false before the loop is left. That saves us the extra work of initializing
 * each time.
 */
void
control_initialize (void ) {
  ligature_present = false;
  cancel_boundary = false;
  lft_hit = false;
  rt_hit = false;
  ins_disc = false;
  /* module 1427 */
  long_help_seen = false;
}


/* module  1196 */

/* Some operations are allowed only in privileged modes, i.e., in cases
 * that |mode>0|. The |privileged| function is used to detect violations
 * of this rule; it issues an error message and returns |false| if the
 * current |mode| is negative.
 */
static boolean 
privileged (void) {
  if (mode > 0) {
	return true;
  } else {
	report_illegal_case();
	return false;
  };
};


/* module  1199 */

/* We don't want to leave |main_control| immediately when a |stop| command
 * is sensed, because it may be necessary to invoke an \.{\\output} routine
 * several times before things really grind to a halt. (The output routine
 * might even say `\.{\\gdef\\end\{...\}}', to prolong the life of the job.)
 * Therefore |its_all_over| is |true| only when the current page
 * and contribution list are empty, and when the last output was not a
 * ``dead cycle.''
 */
static boolean 
its_all_over (void) { /* do this when \.{\\end} or \.{\\dump} occurs */
  if (privileged()) {
	if ((page_head == page_tail) && (head == tail) && (dead_cycles == 0)) {
	  return true;
	};
	back_input();
	/* we will try to end again after ejecting residual material */
	tail_append (new_null_box());
	width (tail) = hsize;
	tail_append (new_glue (fill_glue));
	tail_append (new_penalty (-1073741824));
	build_page(); /* append \.{\\hbox to \\hsize\{\}\\vfill\\penalty-'10000000000} */
  };
  return false;
};



/* module 1213 */
static void 
handle_right_brace (void) {
  pointer p, q; /* for short-term use */ 
  scaled d; /* holds |split_max_depth| in |insert_group| */ 
  int f; /* holds |floating_penalty| in |insert_group| */
  switch (cur_group) {
  case simple_group:
	unsave();
	break;
  case bottom_level:
	print_err ("Too many }'s");
	help2 ("You've closed more groups than you opened.",
		   "Such booboos are generally harmless, so keep going.");
	error();
	break;
  case semi_simple_group:
  case math_shift_group:
  case math_left_group:
	extra_right_brace();
	break;
	/* begin expansion of Cases of |handle_right_brace| where a |right_brace| triggers a delayed action */
	/* module 1230 */
	/* When the right brace occurs at the end of an \.{\\hbox} or \.{\\vbox} or
	 * \.{\\vtop} construction, the |package| routine comes into action. We might
	 * also have to finish a paragraph that hasn't ended.
	 */
  case hbox_group:
	package (0);
	break;
  case adjusted_hbox_group:
	adjust_tail = adjust_head;
	pre_adjust_tail = pre_adjust_head;
	package (0);
	break;
  case vbox_group:
	end_graf();
	package (0);
	break;
  case vtop_group:
	end_graf();
	package (vtop_code);
	break;
	/* module 1245 */
  case insert_group:
	end_graf();
	q = split_top_skip;
	add_glue_ref (q);
	d = split_max_depth;
	f = floating_penalty;
	unsave();
	save_ptr = save_ptr - 2;
	/* now |saved(0)| is the insertion number, or 255 for |vadjust| */
	p = VPACK (link (head), 0, additional);
	pop_nest();
	if (saved (0) < 255) {
	  tail_append (get_node (ins_node_size));
	  type (tail) = ins_node;
	  subtype (tail) = qi (saved (0));
	  height (tail) = height (p) + depth (p);
	  ins_ptr (tail) = list_ptr (p);
	  split_top_ptr (tail) = q;
	  depth (tail) = d;
	  float_cost (tail) = f;
	} else {
	  tail_append(new_adjust_node(p,saved(1)));
	  delete_glue_ref (q);
	};
	free_node (p, box_node_size);
	if (nest_ptr == 0)
	  build_page();
	break;
  case output_group:
	/* begin expansion of Resume the page builder... */
	/* module 1171 */
	/* When the user's output routine finishes, it has constructed a vlist
	 * in internal vertical mode, and \TeX\ will do the following:
	 */
	if ((loc != null)|| ((token_type != output_text)&& (token_type != backed_up))) {
	  /* begin expansion of Recover from an unbalanced output routine */
	  /* module 1172 */
	  print_err ("Unbalanced output routine");
	  help2 ("Your sneaky output routine has problematic {'s and/or }'s.",
			 "I can't handle that very well; good luck.");
	  error();
	  do {
		get_token();
	  } while (loc != null);
	};	/* loops forever if reading from a file, since |null=min_halfword<=0| */
	/* end expansion of Recover from an unbalanced output routine */
	end_token_list(); /* conserve stack space in case more outputs are triggered */
	end_graf();
	unsave();
	output_active = false;
	insert_penalties = 0;
	/* begin expansion of Ensure that box 255 is empty after output */
	/* module 1173 */
	if (box (255) != null) {
	  print_err ("Output routine didn't use all of ");
	  print_esc_string ("box");
	  print_int (255);
	  help3 ("Your \\output commands should empty \\box255,",
			 "e.g., by saying `\\shipout\\box255'.",
			 "Proceed; I'll discard its present contents.");
	  box_error (255);
	};
	/* end expansion of Ensure that box 255 is empty after output */
	if (tail != head)	{ /* current list goes after heldover insertions */
	  link (page_tail) = link (head);
	  page_tail = tail;
	};
	if (link (page_head) != null)	{ /* and both go before heldover contributions */
	  if (link (contrib_head) == null)
		contrib_tail = page_tail;
	  link (page_tail) = link (contrib_head);
	  link (contrib_head) = link (page_head);
	  link (page_head) = null;
	  page_tail = page_head;
	};
	flush_node_list (page_disc);
	page_disc = null;
	pop_nest();
	build_page();
	/* end expansion of Resume the page builder... */
	break;
	/* module 1263 */
	/* The three discretionary lists are constructed somewhat as if they were
	 * hboxes. A~subroutine called |build_discretionary| handles the transitions.
	 * (This is sort of fun.)
	 */
  case disc_group:
	build_discretionary();
	break;
	/* module 1277 */
  case align_group:
	back_input();
	cur_tok = cs_token_flag + frozen_cr;
	print_err ("Missing ");
	print_esc_string ("cr");
	zprint_string(" inserted");
	help1  ("I'm guessing that you meant to end an alignment here.");
	ins_error();
	break;
	/* module 1278 */
  case  no_align_group:
	end_graf();
	unsave();
	align_peek();
	break;
	/* module 1313 */
  case vcenter_group:
	end_graf();
	unsave();
	save_ptr = save_ptr - 2;
	p = VPACK (link (head), saved (1), saved (0));
	pop_nest();
	tail_append (new_noad());
	type (tail) = vcenter_noad;
	math_type (nucleus (tail)) = sub_box;
	info (nucleus (tail)) = p;
	break;
	/* module 1318 */
  case math_choice_group:
	build_choices();
	break;
	/* module 1331 */
	/* Now at last we're ready to see what happens when a right brace occurs
	 * in a math formula. Two special cases are simplified here: Braces are effectively
	 * removed when they surround a single Ord without sub/superscripts, or when they
	 * surround an accent that is the nucleus of an Ord atom.
	 */
  case math_group:
	unsave();
	decr (save_ptr);
	math_type (saved (0)) = sub_mlist;
	p = fin_mlist (null);
	info (saved (0)) = p;
	if (p != null) {
	  if (link (p) == null) {
		if (type (p) == ord_noad) {
		  if (math_type (subscr (p)) == empty)
			if (math_type (supscr (p)) == empty) {
			  mem[saved (0)].hh = mem[nucleus (p)].hh;
			  free_node (p, noad_size);
			};
		} else if (type (p) == accent_noad) {
		  if (saved (0) == nucleus (tail)) {
			if (type (tail) == ord_noad) {
			  /* begin expansion of Replace the tail of the list by |p| */
			  /* module 1332 */
			  q = head;
			  while (link (q) != tail)
				q = link (q);
			  link (q) = p;
			  free_node (tail, noad_size);
			  tail = p;
			  /* end expansion of Replace the tail of the list by |p| */
			}			  
		  }
		}
	  }
	};
	break;
  default:
	confusion ("rightbrace");
  };
}


/* module 1250 */
/* When |delete_last| is called, |cur_chr| is the |type| of node that
 * will be deleted, if present.
 * A final \.{\\endM} node is temporarily removed.
 */
static void 
delete_last (void) {
  pointer p, q; /* run through the current list */ 
  quarterword m; /* the length of a replacement list */ 
  if ((mode == vmode) && (tail == head)) {
	/* begin expansion of Apologize for inability to do the operation now, 
	   unless \.{\\unskip} follows non-glue */
	/* module 1251 */
	if ((cur_chr != glue_node) || (last_glue != max_halfword)) {
	  you_cant();
	  help2 ("Sorry...I usually can't take things from the current page.",
			 "Try `I\\vskip-\\lastskip' instead.");
	  if (cur_chr == kern_node) {
		help_line[0] = "Try `I\\kern-\\lastkern' instead.";
	  } else if (cur_chr != glue_node)
		help_line[0] = "Perhaps you can make the output routine do it.";
	  error();
	};
	/* end expansion of Apologize for inability to do the operation now, ... */
  } else {
	if (!is_char_node (tail)) {
	  if ((type (tail) == math_node)&& (subtype (tail) == end_M_code))
		remove_end_M();
	  if (type (tail) == cur_chr) {
		q = head;
		do {
		  p = q;
		  if (!is_char_node (q))
			if (type (q) == disc_node) {
			  for (m = 1; m <= replace_count (q); m++)
				p = link (p);
			  if (p == tail)
				return;
			};
		  q = link (p);
		} while (q != tail);
		link (p) = null;
		flush_node_list (tail);
		tail = p;
	  };
	  if (LR_temp != null)
		insert_end_M();
	};
  };
}


/* module 1415 */

/* Here is a procedure that might be called `Get the next non-blank non-relax
 * non-call non-assignment token'.
 */
void 
do_assignments (void) {
  loop {
	/* Get the next non-blank non-relax non-call token */
	get_nblank_nrelax_ncall;
	if (cur_cmd <= max_non_prefixed_command)
	  return;
	set_box_allowed = false;
	prefixed_command();
	set_box_allowed = true;
  };
};

/* module 1424 */
static void 
issue_message (void) {
  unsigned char old_setting; /* holds |selector| setting */ 
  unsigned char c; /* identifies \.{\\message} and \.{\\errmessage} */
  str_number s; /* the message */ 
  c = cur_chr;
  link (garbage) = scan_toks (false, true);
  old_setting = selector;
  selector = new_string;
  token_show (def_ref);
  selector = old_setting;
  flush_list (def_ref);
  str_room (1);
  s = make_string();
  if (c == 0) {
	/* begin expansion of Print string |s| on the terminal */
	/* module 1425 */
	if (term_offset + length (s) > (unsigned)max_print_line - 2) {
	  print_ln();
	} else if ((term_offset > 0) || (file_offset > 0)) {
	  print_char (' ');
	}
	slow_print (s);
	update_terminal;
	/* end expansion of Print string |s| on the terminal */
  } else {
	/* begin expansion of Print string |s| as an error message */
	/* module 1428 */
	print_err ("");
	slow_print (s);
	if (err_help != null) {
	  use_err_help = true;
	} else if (long_help_seen) {
	  help1 ("(That was another \\errmessage.)");
	} else {
	  if (interaction < error_stop_mode)
		long_help_seen = true;
	  help4 ("This error message was generated by an \\errmessage",
			 "command, so I can't give any explicit help.",
			 "Pretend that you're Hercule Poirot: Examine all clues,",
			 "and deduce the truth by order and method.");
	};
	error();
	use_err_help = false;
  };
  /* end expansion of Print string |s| as an error message */
  flush_string;
};

/* module 1433 */
static void 
shift_case (void) {
  pointer b; /* |lc_code_base| or |uc_code_base| */ 
  pointer p; /* runs through the token list */ 
  halfword t; /* token */ 
  eight_bits c; /* character code */ 
  b = cur_chr;
  p = scan_toks (false, false);
  p = link (def_ref);
  while (p != null) {
	/* begin expansion of Change the case of the token in |p|, if a change is appropriate */
	/* module 1434 */
	/* When the case of a |chr_code| changes, we don't change the |cmd|.
	 * We also change active characters, using the fact that
	 * |cs_token_flag+active_base| is a multiple of~256.
	 */
	t = info (p);
	if (t < cs_token_flag + single_base) {
	  c = t % 256;
	  if (equiv (b + c) != 0)
		info (p) = t - c + equiv (b + c);
	};
	/* end expansion of Change the case of the token in |p|, if a change is appropriate */
	p = link (p);
  };
  back_list (link (def_ref));
  free_avail (def_ref); /* omit reference count */ 
};

