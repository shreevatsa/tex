
#include "types.h"
#include "c-compat.h"
#include "globals.h"

/* module 220 */

/* 
 * Now that we have studied the data structures for \TeX's semantic routines,
 * we ought to consider the data structures used by its syntactic routines. In
 * other words, our next concern will be
 * the tables that \TeX\ looks at when it is scanning
 * what the user has written.
 * 
 * The biggest and most important such table is called |eqtb|. It holds the
 * current ``equivalents'' of things; i.e., it explains what things mean
 * or what their current values are, for all quantities that are subject to
 * the nesting structure provided by \TeX's grouping mechanism. There are six
 * parts to |eqtb|:
 * 
 * \yskip\hangg 1) |eqtb[active_base..(hash_base-1)]| holds the current
 * equivalents of single-character control sequences.
 * 
 * \yskip\hangg 2) |eqtb[hash_base..(glue_base-1)]| holds the current
 * equivalents of multiletter control sequences.
 * 
 * \yskip\hangg 3) |eqtb[glue_base..(local_base-1)]| holds the current
 * equivalents of glue parameters like the current baselineskip.
 * 
 * \yskip\hangg 4) |eqtb[local_base..(int_base-1)]| holds the current
 * equivalents of local halfword quantities like the current box registers,
 * the current ``catcodes,'' the current font, and a pointer to the current
 * paragraph shape.
 * Additionally region~4 contains the table with ML\TeX's character
 * substitution definitions.
 * 
 * \yskip\hangg 5) |eqtb[int_base..(dimen_base-1)]| holds the current
 * equivalents of fullword integer parameters like the current hyphenation
 * penalty.
 * 
 * \yskip\hangg 6) |eqtb[dimen_base..eqtb_size]| holds the current equivalents
 * of fullword dimension parameters like the current hsize or amount of
 * hanging indentation.
 * 
 * \yskip\noindent Note that, for example, the current amount of
 * baselineskip glue is determined by the setting of a particular location
 * in region~3 of |eqtb|, while the current meaning of the control sequence
 * `\.{\\baselineskip}' (which might have been changed by \.{\\def} or
 * \.{\\let}) appears in region~2.
 */


/* module 253 */

/* The last two regions of |eqtb| have fullword values instead of the
 * three fields |eq_level|, |eq_type|, and |equiv|. An |eq_type| is unnecessary,
 * but \TeX\ needs to store the |eq_level| information in another array
 * called |xeq_level|.
 */
memory_word *eqtb;
quarterword xeq_level[eqtb_size];

/* module 237 */

/* We can print the symbolic name of an integer parameter as follows.
 */
void
print_param (int n) {
  switch (n)
    {
    case pretolerance_code:
      print_esc_string ("pretolerance");
      break;
    case tolerance_code:
      print_esc_string ("tolerance");
      break;
    case line_penalty_code:
      print_esc_string ("linepenalty");
      break;
    case hyphen_penalty_code:
      print_esc_string ("hyphenpenalty");
      break;
    case ex_hyphen_penalty_code:
      print_esc_string ("exhyphenpenalty");
      break;
    case club_penalty_code:
      print_esc_string ("clubpenalty");
      break;
    case widow_penalty_code:
      print_esc_string ("widowpenalty");
      break;
    case display_widow_penalty_code:
      print_esc_string ("displaywidowpenalty");
      break;
    case broken_penalty_code:
      print_esc_string ("brokenpenalty");
      break;
    case bin_op_penalty_code:
      print_esc_string ("binoppenalty");
      break;
    case rel_penalty_code:
      print_esc_string ("relpenalty");
      break;
    case pre_display_penalty_code:
      print_esc_string ("predisplaypenalty");
      break;
    case post_display_penalty_code:
      print_esc_string ("postdisplaypenalty");
      break;
    case inter_line_penalty_code:
      print_esc_string ("interlinepenalty");
      break;
    case double_hyphen_demerits_code:
      print_esc_string ("doublehyphendemerits");
      break;
    case final_hyphen_demerits_code:
      print_esc_string ("finalhyphendemerits");
      break;
    case adj_demerits_code:
      print_esc_string ("adjdemerits");
      break;
    case mag_code:
      print_esc_string ("mag");
      break;
    case delimiter_factor_code:
      print_esc_string ("delimiterfactor");
      break;
    case looseness_code:
      print_esc_string ("looseness");
      break;
    case time_code:
      print_esc_string ("time");
      break;
    case day_code:
      print_esc_string ("day");
      break;
    case month_code:
      print_esc_string ("month");
      break;
    case year_code:
      print_esc_string ("year");
      break;
    case show_box_breadth_code:
      print_esc_string ("showboxbreadth");
      break;
    case show_box_depth_code:
      print_esc_string ("showboxdepth");
      break;
    case hbadness_code:
      print_esc_string ("hbadness");
      break;
    case vbadness_code:
      print_esc_string ("vbadness");
      break;
    case pausing_code:
      print_esc_string ("pausing");
      break;
    case tracing_online_code:
      print_esc_string ("tracingonline");
      break;
    case tracing_macros_code:
      print_esc_string ("tracingmacros");
      break;
    case tracing_stats_code:
      print_esc_string ("tracingstats");
      break;
    case tracing_paragraphs_code:
      print_esc_string ("tracingparagraphs");
      break;
    case tracing_pages_code:
      print_esc_string ("tracingpages");
      break;
    case tracing_output_code:
      print_esc_string ("tracingoutput");
      break;
    case tracing_lost_chars_code:
      print_esc_string ("tracinglostchars");
      break;
    case tracing_commands_code:
      print_esc_string ("tracingcommands");
      break;
    case tracing_restores_code:
      print_esc_string ("tracingrestores");
      break;
    case uc_hyph_code:
      print_esc_string ("uchyph");
      break;
    case output_penalty_code:
      print_esc_string ("outputpenalty");
      break;
    case max_dead_cycles_code:
      print_esc_string ("maxdeadcycles");
      break;
    case hang_after_code:
      print_esc_string ("hangafter");
      break;
    case floating_penalty_code:
      print_esc_string ("floatingpenalty");
      break;
    case global_defs_code:
      print_esc_string ("globaldefs");
      break;
    case cur_fam_code:
      print_esc_string ("fam");
      break;
    case escape_char_code:
      print_esc_string ("escapechar");
      break;
    case default_hyphen_char_code:
      print_esc_string ("defaulthyphenchar");
      break;
    case default_skew_char_code:
      print_esc_string ("defaultskewchar");
      break;
    case end_line_char_code:
      print_esc_string ("endlinechar");
      break;
    case new_line_char_code:
      print_esc_string ("newlinechar");
      break;
    case language_code:
      print_esc_string ("language");
      break;
    case left_hyphen_min_code:
      print_esc_string ("lefthyphenmin");
      break;
    case right_hyphen_min_code:
      print_esc_string ("righthyphenmin");
      break;
    case holding_inserts_code:
      print_esc_string ("holdinginserts");
      break;
    case error_context_lines_code:
      print_esc_string ("errorcontextlines");
      break;
    case char_sub_def_min_code:
      print_esc_string ("charsubdefmin");
      break;
    case char_sub_def_max_code:
      print_esc_string ("charsubdefmax");
      break;
    case tracing_char_sub_def_code:
      print_esc_string ("tracingcharsubdef");
      break;
    case pdf_output_code:
      print_esc_string ("pdfoutput");
      break;
    case pdf_adjust_spacing_code:
      print_esc_string ("pdfadjustspacing");
      break;
    case pdf_compress_level_code:
      print_esc_string ("pdfcompresslevel");
      break;
    case pdf_decimal_digits_code:
      print_esc_string ("pdfdecimaldigits");
      break;
    case pdf_move_chars_code:
      print_esc_string ("pdfmovechars");
      break;
    case pdf_image_resolution_code:
      print_esc_string ("pdfimageresolution");
      break;
    case pdf_pk_resolution_code:
      print_esc_string ("pdfpkresolution");
      break;
    case pdf_unique_resname_code:
      print_esc_string ("pdfuniqueresname");
      break;
    case pdf_protrude_chars_code:
      print_esc_string ("pdfprotrudechars");
      break;
    case pdf_avoid_overfull_code:
      print_esc_string ("pdfavoidoverfull");
      break;
    case pdf_max_penalty_code:
      print_esc_string ("pdfmaxpenalty");
      break;
    case pdf_min_penalty_code:
      print_esc_string ("pdfminpenalty");
      break;
    case pdf_option_pdf_minor_version_code:
      print_esc_string ("pdfoptionpdfminorversion");
      break;
    case pdf_option_always_use_pdfpagebox_code:
      print_esc_string ("pdfoptionalwaysusepdfpagebox");
      break;
    case pdf_option_pdf_inclusion_errorlevel_code:
      print_esc_string ("pdfoptionpdfinclusionerrorlevel");
      break;
	  /* begin expansion of Cases for |print_param| */
	  /* module 1605 */
    case tracing_assigns_code:
      print_esc_string ("tracingassigns");
      break;
    case tracing_groups_code:
      print_esc_string ("tracinggroups");
      break;
    case tracing_ifs_code:
      print_esc_string ("tracingifs");
      break;
    case tracing_scan_tokens_code:
      print_esc_string ("tracingscantokens");
      break;
    case tracing_nesting_code:
      print_esc_string ("tracingnesting");
      break;
    case pre_display_direction_code:
      print_esc_string ("predisplaydirection");
      break;
    case last_line_fit_code:
      print_esc_string ("lastlinefit");
      break;
    case saving_vdiscards_code:
      print_esc_string ("savingvdiscards");
      break;
    case saving_hyph_codes_code:
      print_esc_string ("savinghyphcodes");
      break;
    case eTeX_state_code + TeXXeT_code:
      print_esc_string ("TeXXeTstate");
      break;
    default:
      zprint_string("[unknown integer parameter!]");
    };
}

integer 
zgetintpar (int p) {
  return int_par (p);
};

/* module 225 */

/* Sometimes we need to convert \TeX's internal code numbers into symbolic
 * form. The |print_skip_param| routine gives the symbolic name of a glue
 * parameter.
 */
void
print_skip_param (int n) {
  switch (n)
    {
    case line_skip_code:
      print_esc_string ("lineskip");
      break;
    case baseline_skip_code:
      print_esc_string ("baselineskip");
      break;
    case par_skip_code:
      print_esc_string ("parskip");
      break;
    case above_display_skip_code:
      print_esc_string ("abovedisplayskip");
      break;
    case below_display_skip_code:
      print_esc_string ("belowdisplayskip");
      break;
    case above_display_short_skip_code:
      print_esc_string ("abovedisplayshortskip");
      break;
    case below_display_short_skip_code:
      print_esc_string ("belowdisplayshortskip");
      break;
    case left_skip_code:
      print_esc_string ("leftskip");
      break;
    case right_skip_code:
      print_esc_string ("rightskip");
      break;
    case top_skip_code:
      print_esc_string ("topskip");
      break;
    case split_top_skip_code:
      print_esc_string ("splittopskip");
      break;
    case tab_skip_code:
      print_esc_string ("tabskip");
      break;
    case space_skip_code:
      print_esc_string ("spaceskip");
      break;
    case xspace_skip_code:
      print_esc_string ("xspaceskip");
      break;
    case par_fill_skip_code:
      print_esc_string ("parfillskip");
      break;
    case thin_mu_skip_code:
      print_esc_string ("thinmuskip");
      break;
    case med_mu_skip_code:
      print_esc_string ("medmuskip");
      break;
    case thick_mu_skip_code:
      print_esc_string ("thickmuskip");
      break;
    default:
      zprint_string ("[unknown glue parameter!]");
    };
}


/* module 247 */

/* The final region of |eqtb| contains the dimension parameters defined
 * here, and the 256 \.{\\dimen} registers.
 */
void
print_length_param (int n) {
  switch (n)
    {
    case par_indent_code:
      print_esc_string ("parindent");
      break;
    case math_surround_code:
      print_esc_string ("mathsurround");
      break;
    case line_skip_limit_code:
      print_esc_string ("lineskiplimit");
      break;
    case hsize_code:
      print_esc_string ("hsize");
      break;
    case vsize_code:
      print_esc_string ("vsize");
      break;
    case max_depth_code:
      print_esc_string ("maxdepth");
      break;
    case split_max_depth_code:
      print_esc_string ("splitmaxdepth");
      break;
    case box_max_depth_code:
      print_esc_string ("boxmaxdepth");
      break;
    case hfuzz_code:
      print_esc_string ("hfuzz");
      break;
    case vfuzz_code:
      print_esc_string ("vfuzz");
      break;
    case delimiter_shortfall_code:
      print_esc_string ("delimitershortfall");
      break;
    case null_delimiter_space_code:
      print_esc_string ("nulldelimiterspace");
      break;
    case script_space_code:
      print_esc_string ("scriptspace");
      break;
    case pre_display_size_code:
      print_esc_string ("predisplaysize");
      break;
    case display_width_code:
      print_esc_string ("displaywidth");
      break;
    case display_indent_code:
      print_esc_string ("displayindent");
      break;
    case overfull_rule_code:
      print_esc_string ("overfullrule");
      break;
    case hang_indent_code:
      print_esc_string ("hangindent");
      break;
    case h_offset_code:
      print_esc_string ("hoffset");
      break;
    case v_offset_code:
      print_esc_string ("voffset");
      break;
    case emergency_stretch_code:
      print_esc_string ("emergencystretch");
      break;
    case pdf_h_origin_code:
      print_esc_string ("pdfhorigin");
      break;
    case pdf_v_origin_code:
      print_esc_string ("pdfvorigin");
      break;
    case pdf_page_width_code:
      print_esc_string ("pdfpagewidth");
      break;
    case pdf_page_height_code:
      print_esc_string ("pdfpageheight");
      break;
    case pdf_link_margin_code:
      print_esc_string ("pdflinkmargin");
      break;
    case pdf_dest_margin_code:
      print_esc_string ("pdfdestmargin");
      break;
    case pdf_thread_margin_code:
      print_esc_string ("pdfthreadmargin");
      break;
    default:
      zprint_string("[unknown dimen parameter!]");
    };
};


void
show_eqtb (pointer n) {
  if (n < active_base) {
	print_char ('?');		/* this can't happen */
  } else if (n < glue_base) {
	/* begin expansion of Show equivalent |n|, in region 1 or 2 */
	/* module 223 */
	/* Here is a routine that displays the current meaning of an |eqtb| entry
	 * in region 1 or~2. (Similar routines for the other regions will appear
	 * below.)
	 */
	sprint_cs (n);
	print_char ('=');
	print_cmd_chr (eq_type (n), equiv (n));
	if (eq_type (n) >= call) {
	  print_char (':');
	  show_token_list (link (equiv (n)), null, 32);
	};
	/* end expansion of Show equivalent |n|, in region 1 or 2 */
  } else if (n < local_base) {
	/* begin expansion of Show equivalent |n|, in region 3 */
	/* module 229 */
	if (n < skip_base) {
	  print_skip_param (n - glue_base);
	  print_char ('=');
	  if (n < glue_base + thin_mu_skip_code) {
		print_spec (equiv (n),"pt");
	  } else {
		print_spec (equiv (n),"mu");
	  };
	} else if (n < mu_skip_base) {
	  print_esc_string ("skip");
	  print_int (n - skip_base);
	  print_char ('=');
	  print_spec (equiv (n),"pt");
	} else {
	  print_esc_string ("muskip");
	  print_int (n - mu_skip_base);
	  print_char ('=');
	  print_spec (equiv (n),"mu");
	}
	/* end expansion of Show equivalent |n|, in region 3 */
  } else if (n < int_base) {
	/* begin expansion of Show equivalent |n|, in region 4 */
	/* module 233 */
	if (n < token_base) {
	  print_cmd_chr (set_shape, n);
	  print_char ('=');
	  if (equiv (n) == null) {
		print_char ('0');
	  } else if (n > par_shape_loc) {
		print_int (penalty (equiv (n)));
		print_char (' ');
		print_int (penalty (equiv (n) + 1));
		if (penalty (equiv (n)) > 1)
		  print_esc_string ("ETC.");
	  } else {
		print_int (info (par_shape_ptr));
	  };
	} else if (n < toks_base) {
	  print_cmd_chr (assign_toks, n);
	  print_char ('=');
	  if (equiv (n) != null)
		show_token_list (link (equiv (n)), null, 32);
	} else if (n < box_base) {
	  print_esc_string ("toks");
	  print_int (n - toks_base);
	  print_char ('=');
	  if (equiv (n) != null)
		show_token_list (link (equiv (n)), null, 32);
	} else if (n < cur_font_loc) {
	  print_esc_string ("box");
	  print_int (n - box_base);
	  print_char ('=');
	  if (equiv (n) == null) {
		zprint_string("void");
	  } else {
		depth_threshold = 0;
		breadth_max = 1;
		show_node_list (equiv (n));
	  };
	} else if (n < cat_code_base) {
	  /* begin expansion of Show the font identifier in |eqtb[n]| */
	  /* module 234 */
	  if (n == cur_font_loc) {
		zprint_string("current font");
	  } else if (n < math_font_base + 16) {
		print_esc_string ("textfont");
		print_int (n - math_font_base);
	  } else if (n < math_font_base + 32) {
		print_esc_string ("scriptfont");
		print_int (n - math_font_base - 16);
	  } else {
		print_esc_string ("scriptscriptfont");
		print_int (n - math_font_base - 32);
	  };
	  print_char ('=');
	  print_esc (hash[font_id_base + equiv (n)].rh);	/* that's |font_id_text(equiv(n))| */
	  /* end expansion of Show the font identifier in |eqtb[n]| */
	} else if (n < math_code_base) {
	  /* begin expansion of Show the halfword code in |eqtb[n]| */
	  /* module 235 */
	  if (n < lc_code_base) {
		print_esc_string ("catcode");
		print_int (n - cat_code_base);
	  } else if (n < uc_code_base) {
		print_esc_string ("lccode");
		print_int (n - lc_code_base);
	  } else if (n < sf_code_base) {
		print_esc_string ("uccode");
		print_int (n - uc_code_base);
	  } else {
		print_esc_string ("sfcode");
		print_int (n - sf_code_base);
	  };
	  print_char ('=');
	  print_int (equiv (n));
	} else {
	  print_esc_string ("mathcode");
	  print_int (n - math_code_base);
	  print_char ('=');
	  print_int (ho (equiv (n)));
	};
	/* end expansion of Show the halfword code in |eqtb[n]| */
	/* end expansion of Show equivalent |n|, in region 4 */
  } else if (n < dimen_base) {
	/* begin expansion of Show equivalent |n|, in region 5 */
	/* module 242 */
	if (n < count_base) {
	  print_param (n - int_base);
	} else if (n < del_code_base) {
	  print_esc_string ("count");
	  print_int (n - count_base);
	} else {
	  print_esc_string ("delcode");
	  print_int (n - del_code_base);
	};
	print_char ('=');
	print_int (eqtb[n].cint);
	/* end expansion of Show equivalent |n|, in region 5 */
  } else if (n <= eqtb_size) {
	/* begin expansion of Show equivalent |n|, in region 6 */
	/* module 251 */
	if (n < scaled_base) {
	  print_length_param (n - dimen_base);
	} else {
	  print_esc_string ("dimen");
	  print_int (n - scaled_base);
	};
	print_char ('=');
	print_scaled (eqtb[n].sc);
	zprint_string("pt");
	/* end expansion of Show equivalent |n|, in region 6 */
  } else {
	print_char ('?');	/* this can't happen either */
  };
}


/* module 254 */

void
eqtb_initialize (void) {
  int k;
  for (k = int_base; k <= eqtb_size; k++)
    xeq_level[k] = level_one;
}



/* module 222
 *
 * Many locations in |eqtb| have symbolic names. The purpose of the next
 * paragraphs is to define these names, and to set up the initial values of the
 * equivalents.
 * 
 * In the first region we have 256 equivalents for ``active characters'' that
 * act as control sequences, followed by 256 equivalents for single-character
 * control sequences.
 * 
 * Then comes region~2, which corresponds to the hash table that we will
 * define later. The maximum address in this region is used for a dummy
 * control sequence that is perpetually undefined. There also are several
 * locations for control sequences that are perpetually defined
 * (since they are used in error recovery).
 *
 * module 228
 * 
 * All glue parameters and registers are initially `\.{0pt plus0pt minus0pt}'. 
 *
 * module 232
 *
 * We initialize most things to null or undefined values. An undefined font
 * is represented by the internal code |font_base|.
 * 
 * However, the character code tables are given initial values based on the
 * conventional interpretation of ASCII code. These initial values should
 * not be changed when \TeX\ is adapted for use with non-English languages;
 * all changes to the initialization conventions should be made in format
 * packages, not in \TeX\ itself, so that global interchange of formats is
 * possible.
 *
 * module 240
 *
 * The integer parameters should really be initialized by a macro package;
 * the following initialization does the minimum to keep \TeX\ from
 * complete failure.
 */

void
eqtb_initialize_init (void) {
  int k;
  /* module 222 */
  eq_type (undefined_control_sequence) = undefined_cs;
  equiv (undefined_control_sequence) = null;
  eq_level (undefined_control_sequence) = level_zero;
  for (k = active_base; k <= eqtb_size; k++)
	eqtb[k] = eqtb[undefined_control_sequence];
  /* module 228 */
  equiv (glue_base) = zero_glue;
  eq_level (glue_base) = level_one;
  eq_type (glue_base) = glue_ref;
  for (k = glue_base + 1; k <= local_base - 1; k++)
	eqtb[k] = eqtb[glue_base];
  glue_ref_count (zero_glue) = glue_ref_count (zero_glue) + local_base - glue_base;
  /* module 232 */
  par_shape_ptr = null;
  eq_type (par_shape_loc) = shape_ref;
  eq_level (par_shape_loc) = level_one;
  for (k = par_shape_loc + 1; k <= token_base - 1; k++)
	eqtb[k] = eqtb[par_shape_loc];
  for (k = output_routine_loc; k <= toks_base + 255; k++)
	eqtb[k] = eqtb[undefined_control_sequence];
  box (0) = null;
  eq_type (box_base) = box_ref;
  eq_level (box_base) = level_one;
  for (k = box_base + 1; k <= box_base + 255; k++)
	eqtb[k] = eqtb[box_base];
  cur_font = null_font;
  eq_type (cur_font_loc) = data;
  eq_level (cur_font_loc) = level_one;
  for (k = math_font_base; k <= math_font_base + 47; k++)
	eqtb[k] = eqtb[cur_font_loc];
  equiv (cat_code_base) = 0;
  eq_type (cat_code_base) = data;
  eq_level (cat_code_base) = level_one;
  for (k = cat_code_base + 1; k <= int_base - 1; k++)
	eqtb[k] = eqtb[cat_code_base];
  for (k = 0; k <= 255; k++) {
	cat_code (k) = other_char;
	math_code (k) = hi (k);
	sf_code (k) = 1000;
  };
  cat_code (carriage_return) = car_ret;
  cat_code (' ') = spacer;
  cat_code ('\\') = escape;
  cat_code ('%') = comment;
  cat_code (invalid_code) = invalid_char;
  cat_code (null_code) = ignore;
  for (k = 0; k <= 9; k++)
	math_code (k) = hi (k + var_code);
  for (k = 'A'; k <= 'Z'; k++) {
	cat_code (k) = letter;
	cat_code (k + 'a' - 'A') = letter;
	math_code (k) = hi (k + var_code + 256);
	math_code (k + 'a' - 'A') = hi (k + 'a' - 'A' + var_code + 256);
	lc_code (k) = k + 'a' - 'A';
	lc_code (k + 'a' - 'A') = k + 'a' - 'A';
	uc_code (k) = k;
	uc_code (k + 'a' - 'A') = k;
	sf_code (k) = 999;
  };
  /* module 240 */
  for (k = int_base; k <= del_code_base - 1; k++)
	eqtb[k].cint = 0;
  char_sub_def_min = 256;
  char_sub_def_max = -1;
  /* allow \.{\\charsubdef} for char 0 *//* |tracing_char_sub_def:=0| is already done */
  mag = 1000;
  tolerance = 10000;
  hang_after = 1;
  max_dead_cycles = 25;
  escape_char = '\\';
  end_line_char = carriage_return;
  for (k = 0; k <= 255; k++)
	del_code (k) = -1;
  del_code ('.') = 0;
  /* this null delimiter is used in error recovery */
  /* module 250 */
  for (k = dimen_base; k <= eqtb_size; k++)
	eqtb[k].sc = 0;
}


