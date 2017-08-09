
/* module 210 */

/* The remaining command codes are extra special, since they cannot get through
 * \TeX's scanner to the main control routine. They have been given values higher
 * than |max_command| so that their special nature is easily discernible.
 * The ``expandable'' commands come first.
 */
#define undefined_cs     (max_command  + 1)
#define expand_after     (max_command  + 2)
#define no_expand        (max_command  + 3)
#define input            (max_command  + 4)
#define if_test          (max_command  + 5)
#define fi_or_else       (max_command  + 6)
#define cs_name          (max_command  + 7)
#define convert          (max_command  + 8)
#define the              (max_command  + 9)
#define top_bot_mark     (max_command  + 10)
#define call             (max_command  + 11)
#define long_call        (max_command  + 12)
#define outer_call       (max_command  + 13)
#define long_outer_call  (max_command  + 14)
#define end_template     (max_command  + 15)
#define dont_expand      (max_command  + 16)
#define glue_ref         (max_command  + 17)
#define shape_ref        (max_command  + 18)
#define box_ref          (max_command  + 19)
#define data             (max_command  + 20)

/* module 221 */

/* Each entry in |eqtb| is a |memory_word|. Most of these words are of type
 * |two_halves|, and subdivided into three fields:
 * 
 * \yskip\hangg 1) The |eq_level| (a quarterword) is the level of grouping at
 * which this equivalent was defined. If the level is |level_zero|, the
 * equivalent has never been defined; |level_one| refers to the outer level
 * (outside of all groups), and this level is also used for global
 * definitions that never go away. Higher levels are for equivalents that
 * will disappear at the end of their group. 
 *
 * \yskip\hangg 2) The |eq_type| (another quarterword) specifies what kind of
 * entry this is. There are many types, since each \TeX\ primitive like
 * \.{\\hbox}, \.{\\def}, etc., has its own special code. The list of
 * command codes above includes all possible settings of the |eq_type| field.
 * 
 * \yskip\hangg 3) The |equiv| (a halfword) is the current equivalent value.
 * This may be a font number, a pointer into |mem|, or a variety of other
 * things.
 */
#define eq_level_field( arg )  arg.hh.b1
#define eq_type_field( arg )   arg.hh.b0
#define equiv_field( arg )     arg.hh.rh
#define eq_level( arg )        eq_level_field ( eqtb [ arg ])
#define eq_type( arg )         eq_type_field ( eqtb [ arg ])
#define equiv( arg )           equiv_field ( eqtb [ arg ])

#define level_zero             min_quarterword
#define level_one              (level_zero  + 1)

/* module 222 */
#define active_base                 1
#define single_base                 (active_base  + 256)
#define null_cs                     (single_base  + 256)
#define hash_base                   (null_cs  + 1)
#define frozen_control_sequence     (hash_base  +  HASH_SIZE)
#define frozen_protection           frozen_control_sequence
#define frozen_cr                   (frozen_control_sequence  + 1)
#define frozen_end_group            (frozen_control_sequence  + 2)
#define frozen_right                (frozen_control_sequence  + 3)
#define frozen_fi                   (frozen_control_sequence  + 4)
#define frozen_end_template         (frozen_control_sequence  + 5)
#define frozen_endv                 (frozen_control_sequence  + 6)
#define frozen_relax                (frozen_control_sequence  + 7)
#define end_write                   (frozen_control_sequence  + 8)
#define frozen_dont_expand          (frozen_control_sequence  + 9)
#define frozen_special              (frozen_control_sequence  + 10)
#define frozen_null_font            (frozen_control_sequence  + 11)
#define font_id_base                (frozen_null_font  -  font_base)
#define undefined_control_sequence  (frozen_null_font  +  max_font_max  + 1)
#define glue_base                   (undefined_control_sequence  + 1)

/* module 224 */
#define line_skip_code 0
#define baseline_skip_code 1
#define par_skip_code 2
#define above_display_skip_code 3
#define below_display_skip_code 4
#define above_display_short_skip_code 5
#define below_display_short_skip_code 6
#define left_skip_code 7
#define right_skip_code 8
#define top_skip_code 9
#define split_top_skip_code 10
#define tab_skip_code 11
#define space_skip_code 12
#define xspace_skip_code 13
#define par_fill_skip_code 14
#define thin_mu_skip_code 15
#define med_mu_skip_code 16
#define thick_mu_skip_code 17
#define glue_pars 18
#define skip_base  (glue_base  +  glue_pars)
#define mu_skip_base  (skip_base  + 256)
#define local_base  (mu_skip_base  + 256)
#define skip( arg )  equiv ( skip_base  +  arg )
#define mu_skip( arg )  equiv ( mu_skip_base  +  arg )
#define glue_par( arg )  equiv ( glue_base  +  arg )
#define line_skip  glue_par ( line_skip_code )
#define baseline_skip  glue_par ( baseline_skip_code )
#define par_skip  glue_par ( par_skip_code )
#define above_display_skip  glue_par ( above_display_skip_code )
#define below_display_skip  glue_par ( below_display_skip_code )
#define above_display_short_skip  glue_par ( above_display_short_skip_code )
#define below_display_short_skip  glue_par ( below_display_short_skip_code )
#define left_skip  glue_par ( left_skip_code )
#define right_skip  glue_par ( right_skip_code )
#define top_skip  glue_par ( top_skip_code )
#define split_top_skip  glue_par ( split_top_skip_code )
#define tab_skip  glue_par ( tab_skip_code )
#define space_skip  glue_par ( space_skip_code )
#define xspace_skip  glue_par ( xspace_skip_code )
#define par_fill_skip  glue_par ( par_fill_skip_code )
#define thin_mu_skip  glue_par ( thin_mu_skip_code )
#define med_mu_skip  glue_par ( med_mu_skip_code )
#define thick_mu_skip  glue_par ( thick_mu_skip_code )

/* module 230 */
#define par_shape_loc  local_base
#define inter_line_penalties_loc  (local_base  + 1)
#define club_penalties_loc  (local_base  + 2)
#define widow_penalties_loc  (local_base  + 3)
#define display_widow_penalties_loc  (local_base  + 4)
#define token_base  (local_base  + 5)
#define output_routine_loc  token_base
#define every_par_loc  (token_base  + 1)
#define every_math_loc  (token_base  + 2)
#define every_display_loc  (token_base  + 3)
#define every_hbox_loc  (token_base  + 4)
#define every_vbox_loc  (token_base  + 5)
#define every_job_loc  (token_base  + 6)
#define every_cr_loc  (token_base  + 7)
#define every_eof_loc  (token_base  + 8)
#define pdf_pages_attr_loc  (token_base  + 9)
#define pdf_page_attr_loc  (token_base  + 10)
#define pdf_page_resources_loc  (token_base  + 11)
#define err_help_loc  (token_base  + 12)
#define toks_base  (token_base  + 13)
#define box_base  (toks_base  + 256)
#define cur_font_loc  (box_base  + 256)
#define math_font_base  (cur_font_loc  + 1)
#define cat_code_base  (math_font_base  + 48)
#define lc_code_base  (cat_code_base  + 256)
#define uc_code_base  (lc_code_base  + 256)
#define sf_code_base  (uc_code_base  + 256)
#define math_code_base  (sf_code_base  + 256)
#define char_sub_code_base  (math_code_base  + 256)
#define int_base  (char_sub_code_base  + 256)
#define par_shape_ptr  equiv ( par_shape_loc )
#define output_routine  equiv ( output_routine_loc )
#define every_par  equiv ( every_par_loc )
#define every_math  equiv ( every_math_loc )
#define every_display  equiv ( every_display_loc )
#define every_hbox  equiv ( every_hbox_loc )
#define every_vbox  equiv ( every_vbox_loc )
#define every_job  equiv ( every_job_loc )
#define every_cr  equiv ( every_cr_loc )
#define pdf_pages_attr  equiv ( pdf_pages_attr_loc )
#define pdf_page_attr  equiv ( pdf_page_attr_loc )
#define pdf_page_resources  equiv ( pdf_page_resources_loc )
#define err_help  equiv ( err_help_loc )
#define toks( arg )  equiv ( toks_base  +  arg )
#define box( arg )  equiv ( box_base  +  arg )
#define cur_font  equiv ( cur_font_loc )
#define fam_fnt( arg )  equiv ( math_font_base  +  arg )
#define cat_code( arg )  equiv ( cat_code_base  +  arg )
#define lc_code( arg )  equiv ( lc_code_base  +  arg )
#define uc_code( arg )  equiv ( uc_code_base  +  arg )
#define sf_code( arg )  equiv ( sf_code_base  +  arg )
#define math_code( arg )  equiv ( math_code_base  +  arg )
#define char_sub_code( arg )  equiv ( char_sub_code_base  +  arg )

/* module 232 */
#define null_font  font_base
#define var_code 28672

/* module 236 */
/* Region 5 of |eqtb| contains the integer parameters and registers defined
 * here, as well as the |del_code| table. The latter table differs from the
 * |cat_code..math_code| tables that precede it, since delimiter codes are
 * fullword integers while the other kinds of codes occupy at most a
 * halfword. This is what makes region~5 different from region~4. We will
 * store the |eq_level| information in an auxiliary array of quarterwords
 * that will be defined later.
 */
#define pretolerance_code 0
#define tolerance_code 1
#define line_penalty_code 2
#define hyphen_penalty_code 3
#define ex_hyphen_penalty_code 4
#define club_penalty_code 5
#define widow_penalty_code 6
#define display_widow_penalty_code 7
#define broken_penalty_code 8
#define bin_op_penalty_code 9
#define rel_penalty_code 10
#define pre_display_penalty_code 11
#define post_display_penalty_code 12
#define inter_line_penalty_code 13
#define double_hyphen_demerits_code 14
#define final_hyphen_demerits_code 15
#define adj_demerits_code 16
#define mag_code 17
#define delimiter_factor_code 18
#define looseness_code 19
#define time_code 20
#define day_code 21
#define month_code 22
#define year_code 23
#define show_box_breadth_code 24
#define show_box_depth_code 25
#define hbadness_code 26
#define vbadness_code 27
#define pausing_code 28
#define tracing_online_code 29
#define tracing_macros_code 30
#define tracing_stats_code 31
#define tracing_paragraphs_code 32
#define tracing_pages_code 33
#define tracing_output_code 34
#define tracing_lost_chars_code 35
#define tracing_commands_code 36
#define tracing_restores_code 37
#define uc_hyph_code 38
#define output_penalty_code 39
#define max_dead_cycles_code 40
#define hang_after_code 41
#define floating_penalty_code 42
#define global_defs_code 43
#define cur_fam_code 44
#define escape_char_code 45
#define default_hyphen_char_code 46
#define default_skew_char_code 47
#define end_line_char_code 48
#define new_line_char_code 49
#define language_code 50
#define left_hyphen_min_code 51
#define right_hyphen_min_code 52
#define holding_inserts_code 53
#define error_context_lines_code 54
#define pdf_output_code 55
#define pdf_adjust_spacing_code 56
#define pdf_compress_level_code 57
#define pdf_decimal_digits_code 58
#define pdf_move_chars_code 59
#define pdf_image_resolution_code 60
#define pdf_pk_resolution_code 61
#define pdf_unique_resname_code 62
#define pdf_protrude_chars_code 63
#define pdf_avoid_overfull_code 64
#define pdf_max_penalty_code 65
#define pdf_min_penalty_code 66
#define char_sub_def_min_code 67
#define char_sub_def_max_code 68
#define tracing_char_sub_def_code 69
#define pdf_option_pdf_minor_version_code 70
#define pdf_option_always_use_pdfpagebox_code 71
#define pdf_option_pdf_inclusion_errorlevel_code 72
#define int_pars 73
#define tracing_assigns_code  int_pars
#define tracing_groups_code  (int_pars  + 1)
#define tracing_ifs_code  (int_pars  + 2)
#define tracing_scan_tokens_code  (int_pars  + 3)
#define tracing_nesting_code  (int_pars  + 4)
#define pre_display_direction_code  (int_pars  + 5)
#define last_line_fit_code  (int_pars  + 6)
#define saving_vdiscards_code  (int_pars  + 7)
#define saving_hyph_codes_code  (int_pars  + 8)
#define eTeX_state_code  (int_pars  + 9)
#define count_base  (int_base  +  eTeX_state_code  +  eTeX_states)
#define del_code_base  (count_base  + 256)
#define dimen_base  (del_code_base  + 256)
#define del_code( arg )  eqtb [ del_code_base  +  arg ]. cint
#define count( arg )  eqtb [ count_base  +  arg ]. cint
#define int_par( arg )  eqtb [ int_base  +  arg ]. cint
#define pretolerance  int_par ( pretolerance_code )
#define tolerance  int_par ( tolerance_code )
#define line_penalty  int_par ( line_penalty_code )
#define hyphen_penalty  int_par ( hyphen_penalty_code )
#define ex_hyphen_penalty  int_par ( ex_hyphen_penalty_code )
#define club_penalty  int_par ( club_penalty_code )
#define widow_penalty  int_par ( widow_penalty_code )
#define display_widow_penalty  int_par ( display_widow_penalty_code )
#define broken_penalty  int_par ( broken_penalty_code )
#define bin_op_penalty  int_par ( bin_op_penalty_code )
#define rel_penalty  int_par ( rel_penalty_code )
#define pre_display_penalty  int_par ( pre_display_penalty_code )
#define post_display_penalty  int_par ( post_display_penalty_code )
#define inter_line_penalty  int_par ( inter_line_penalty_code )
#define double_hyphen_demerits  int_par ( double_hyphen_demerits_code )
#define final_hyphen_demerits  int_par ( final_hyphen_demerits_code )
#define adj_demerits  int_par ( adj_demerits_code )
#define mag  int_par ( mag_code )
#define delimiter_factor  int_par ( delimiter_factor_code )
#define looseness  int_par ( looseness_code )
#define tex_time  int_par ( time_code )
#define day  int_par ( day_code )
#define month  int_par ( month_code )
#define year  int_par ( year_code )
#define show_box_breadth  int_par ( show_box_breadth_code )
#define show_box_depth  int_par ( show_box_depth_code )
#define hbadness  int_par ( hbadness_code )
#define vbadness  int_par ( vbadness_code )
#define pausing  int_par ( pausing_code )
#define tracing_online  int_par ( tracing_online_code )
#define tracing_macros  int_par ( tracing_macros_code )
#define tracing_stats  int_par ( tracing_stats_code )
#define tracing_paragraphs  int_par ( tracing_paragraphs_code )
#define tracing_pages  int_par ( tracing_pages_code )
#define tracing_output  int_par ( tracing_output_code )
#define tracing_lost_chars  int_par ( tracing_lost_chars_code )
#define tracing_commands  int_par ( tracing_commands_code )
#define tracing_restores  int_par ( tracing_restores_code )
#define uc_hyph  int_par ( uc_hyph_code )
#define output_penalty  int_par ( output_penalty_code )
#define max_dead_cycles  int_par ( max_dead_cycles_code )
#define hang_after  int_par ( hang_after_code )
#define floating_penalty  int_par ( floating_penalty_code )
#define global_defs  int_par ( global_defs_code )
#define cur_fam  int_par ( cur_fam_code )
#define escape_char  int_par ( escape_char_code )
#define default_hyphen_char  int_par ( default_hyphen_char_code )
#define default_skew_char  int_par ( default_skew_char_code )
#define end_line_char  int_par ( end_line_char_code )
#define new_line_char  int_par ( new_line_char_code )
#define language  int_par ( language_code )
#define left_hyphen_min  int_par ( left_hyphen_min_code )
#define right_hyphen_min  int_par ( right_hyphen_min_code )
#define holding_inserts  int_par ( holding_inserts_code )
#define error_context_lines  int_par ( error_context_lines_code )
#define char_sub_def_min  int_par ( char_sub_def_min_code )
#define char_sub_def_max  int_par ( char_sub_def_max_code )
#define tracing_char_sub_def  int_par ( tracing_char_sub_def_code )
#define pdf_output  int_par ( pdf_output_code )
#define pdf_adjust_spacing  int_par ( pdf_adjust_spacing_code )
#define pdf_compress_level  int_par ( pdf_compress_level_code )
#define pdf_decimal_digits  int_par ( pdf_decimal_digits_code )
#define pdf_move_chars  int_par ( pdf_move_chars_code )
#define pdf_image_resolution  int_par ( pdf_image_resolution_code )
#define pdf_pk_resolution  int_par ( pdf_pk_resolution_code )
#define pdf_unique_resname  int_par ( pdf_unique_resname_code )
#define pdf_protrude_chars  int_par ( pdf_protrude_chars_code )
#define pdf_avoid_overfull  int_par ( pdf_avoid_overfull_code )
#define pdf_max_penalty  int_par ( pdf_max_penalty_code )
#define pdf_min_penalty  int_par ( pdf_min_penalty_code )
#define pdf_option_pdf_minor_version  int_par ( pdf_option_pdf_minor_version_code )
#define pdf_option_always_use_pdfpagebox  int_par ( pdf_option_always_use_pdfpagebox_code )
#define pdf_option_pdf_inclusion_errorlevel  int_par ( pdf_option_pdf_inclusion_errorlevel_code )
#define tracing_assigns  int_par ( tracing_assigns_code )
#define tracing_groups  int_par ( tracing_groups_code )
#define tracing_ifs  int_par ( tracing_ifs_code )
#define tracing_scan_tokens  int_par ( tracing_scan_tokens_code )
#define tracing_nesting  int_par ( tracing_nesting_code )
#define pre_display_direction  int_par ( pre_display_direction_code )
#define last_line_fit  int_par ( last_line_fit_code )
#define saving_vdiscards  int_par ( saving_vdiscards_code )
#define saving_hyph_codes  int_par ( saving_hyph_codes_code )

/* module 247 */
#define par_indent_code 0
#define math_surround_code 1
#define line_skip_limit_code 2
#define hsize_code 3
#define vsize_code 4
#define max_depth_code 5
#define split_max_depth_code 6
#define box_max_depth_code 7
#define hfuzz_code 8
#define vfuzz_code 9
#define delimiter_shortfall_code 10
#define null_delimiter_space_code 11
#define script_space_code 12
#define pre_display_size_code 13
#define display_width_code 14
#define display_indent_code 15
#define overfull_rule_code 16
#define hang_indent_code 17
#define h_offset_code 18
#define v_offset_code 19
#define emergency_stretch_code 20
#define pdf_h_origin_code 21
#define pdf_v_origin_code 22
#define pdf_page_width_code 23
#define pdf_page_height_code 24
#define pdf_link_margin_code 25
#define pdf_dest_margin_code 26
#define pdf_thread_margin_code 27
#define dimen_pars 28
#define scaled_base  (dimen_base  +  dimen_pars)
#define eqtb_size  (scaled_base  + 255)
#define dimen( arg )  eqtb [ scaled_base  +  arg ]. sc
#define dimen_par( arg )  eqtb [ dimen_base  +  arg ]. sc
#define par_indent  dimen_par ( par_indent_code )
#define math_surround  dimen_par ( math_surround_code )
#define line_skip_limit  dimen_par ( line_skip_limit_code )
#define hsize  dimen_par ( hsize_code )
#define vsize  dimen_par ( vsize_code )
#define max_depth  dimen_par ( max_depth_code )
#define split_max_depth  dimen_par ( split_max_depth_code )
#define box_max_depth  dimen_par ( box_max_depth_code )
#define hfuzz  dimen_par ( hfuzz_code )
#define vfuzz  dimen_par ( vfuzz_code )
#define delimiter_shortfall  dimen_par ( delimiter_shortfall_code )
#define null_delimiter_space  dimen_par ( null_delimiter_space_code )
#define script_space  dimen_par ( script_space_code )
#define pre_display_size  dimen_par ( pre_display_size_code )
#define display_width  dimen_par ( display_width_code )
#define display_indent  dimen_par ( display_indent_code )
#define overfull_rule  dimen_par ( overfull_rule_code )
#define hang_indent  dimen_par ( hang_indent_code )
#define h_offset  dimen_par ( h_offset_code )
#define v_offset  dimen_par ( v_offset_code )
#define emergency_stretch  dimen_par ( emergency_stretch_code )
#define pdf_h_origin  dimen_par ( pdf_h_origin_code )
#define pdf_v_origin  dimen_par ( pdf_v_origin_code )
#define pdf_page_width  dimen_par ( pdf_page_width_code )
#define pdf_page_height  dimen_par ( pdf_page_height_code )
#define pdf_link_margin  dimen_par ( pdf_link_margin_code )
#define pdf_dest_margin  dimen_par ( pdf_dest_margin_code )
#define pdf_thread_margin  dimen_par ( pdf_thread_margin_code )


EXTERN memory_word *eqtb;
EXTERN quarterword xeq_level[eqtb_size];


EXTERN void print_param (int n);
EXTERN integer zgetintpar (int p);

EXTERN void print_skip_param (int n);

EXTERN void print_length_param (int n);


EXTERN void show_eqtb (pointer n);

EXTERN void eqtb_initialize (void);

EXTERN void eqtb_initialize_init (void);
