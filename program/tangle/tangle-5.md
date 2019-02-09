---
title: Tangle -- Data structures
---

<link rel="stylesheet" href="common.css">

If you were trying to read the program, this is probably where you got overwhelmed.

In this part DEK introduces a dizzying array of... well, arrays, with complicated purposes and little motivation. So let's try providing some.

Recall that Pascal did not have good support for strings. Recall also that after the first phase, we want to have read everything from the input file(s) into memory, so we should hold in memory everything that we need in order to output the tangled program. This includes many “strings” the output is supposed to contain, which therefore need to be stored in memory as characters in some consecutive positions of memory somewhere.

## What do we need to store: names (strings)

Specifically, we need the following kinds of strings (lists of characters):

- The names of modules, e.g. module 11 may have name `Types in the outer block`.

- The names of macros, e.g. if we have a macro definition like `define print(#) ≡ write(term_out, #)`, then we need to store the string `print` somewhere (at least during the first phase, so that we can recognize it when we see it again).

  - This includes zero-parameter macros like `define return ≡ goto exit`, where we need to store `return` in memory somewhere.

  - This also includes numeric macros like `define spotless = 0`, where we need to store `spotless`.

- The double-quoted strings that TANGLE is supposed to translate into (string pool) numbers, like `"A"` into `65`. (There aren't any of length > 1 in the TANGLE program itself, but there are many in TeX of course.)

- The identifiers in the program, like `limit` (the name of a variable) or `error` (the name of a procedure), or `begin` (a Pascal keyword).

All of these "names" (strings, names of identifiers/macros, names of modules) are stored in a huge array called `byte_mem[][]`: for every integer `i` up to the number of names stored, the name `i` occupies positions `byte_mem[i%ww][byte_start[i]]` to `byte_mem[i%ww][byte_start[i+ww] - 1]`.

Such integers `i` from `0` to the number of names stored (= `name_ptr` ≤ `max_names`) are said to be of type `name_pointer`.

For example, after reading about 2250 lines of `tex.web`, these are the contents of the `byte_mem` and `byte_start` arrays:

```
(gdb) p line
$15 = 2254
(gdb) p bytemem[0]
$16 = "Compiler directiveslabelConstants in the outer blockGlobal variablesLocal variables for initializationendstart_of_TEXdebugtatsSet initial values of key variablesothersmem_minhalf_error_linemax_in_openparam_sizestring_vacanciestrie_sizefile_name_sizemem_tophash_primeintegerorexitcontinuedone2done5found1common_endingnegatetruereturnASCII_codefirst_text_chararraynull_codeforeight_bitsfilename_lengthrewrite_OKbooleanrewritew_open_ina_closew_closelastbypass_eolneofelse\" t_open_inbreakwake_up_terminalcur_inputoverflowinit_terminalsistr_numberstr_startinit_pool_ptrcur_lengthstr_room\"number of stringsresultMake the first 256 strings\"0\"^pool_fileRead one string, but return |false| if the string memory space is getting too tight for comfortCheck the pool check sumno_printterm_and_logmax_selectordigfile_offsetfirst_countwterm_lnwlog_lncaseCharacter |s| is the current new-line character\"???print_nlSet variable |c| to the current escape characterprint_intabsprint_ASCII\"m2d5c2l5x2v5iprompt_input\"End of file on the terminal!scroll_modeinteractionspotlessfatal_error_stophistoryforwardbegin_file_readingclear_for_error_prompthlp1hlp3hlp6help1help4use_err_helps1s4\"(That makes 100 errors; please try again.)Interpret code |c| and |return| if done\"4\"7\"D\"You want to edit file \" at line Print the help information and |goto continue|\"QChange the interaction level and |return|\"Type <return> to proceed, S to scroll future error messages,\"E to edit your file,\"OK, entering \"scrollmodecur_tokalign_state\"You can now delete more, or insert, or whatever.\"Sorry, I already gave what help I could...int_errorlog_opened\"Emergency stop\"]confusion\"I can't go on meeting you like thischeck_interrupt\"Interruption\"unless you just want to quit by typing `X'.tworound_decimalsarith_errormult_and_add", '\000' <repeats 63752 times>
(gdb) p bytemem[1]
$17 = "bannerprogramLabels in the outer blockTypes in the outer blockprocedurebeginBasic printing proceduresend_of_TEXgubedinitInitialize table entries (done by \\.{INITEX} only)endcasesbuf_sizemax_print_linefont_maxnest_sizepool_sizetrie_op_sizepool_namefont_basehyph_sizeCheck the ``constant'' values for consistencythenrestartdonedone3done6found2incrloopdogototext_charlast_text_charofcarriage_returntoalpha_filebyte_filereset_OKfunctionresetb_open_inword_fileclosebuffermax_buf_stacklast_nonblankgeteolnterm_int_open_outclear_terminalformat_identloc_field\"buffer sizewritesopacked_ASCII_codepool_ptrinit_str_ptrappend_char\"pool sizeflush_stringstr_eq_strRead the other strings from the \\.{TEX.POOL} file and return |true|, or give an error message and return |false|\"adivbad_pooluntil\"9term_onlypseudolog_filetallytrick_bufInitialize the output routineswterm_crwlog_crwrite_fileprintnew_line_charoddprint_the_digs\"-print_hexprint_roman_int\"2term_inputbatch_modeerror_stop_mode\"! warning_issueddeletions_allowederror_countget_tokenopen_log_filegive_err_helphelp_linehlp4help0help2help5jump_outs2\".Put help message on the transcript file\"1\"5\"8\"Einput_stackline\"I\"R\"X\"R to run without stopping, Q to run quietly,\"1 or ... or 9 to ignore the next 1 to 9 tokens of input,\"batchmode\"...cur_cmdOK_to_interrupt\"Sorry, I don't know how to help in this situation.\"An error might have occurred before I noticed any problems.\" (job_name\"TeX capacity exceeded, sorry [\"If you really absolutely need more capacity,\"This can't happen (\"One of your faux pas seems to have wounded me deeply...interrupt\"You rang?halfscaledprint_scaledremainder", '\000' <repeats 63913 times>
(gdb) p bytemem[2]
$18 = "mtypeTEXconstvarinitializeInitialize whatever \\TeX\\ might accessError handling proceduresfinal_endstattiniothercasesmem_maxerror_linestack_sizefont_mem_sizemax_stringssave_sizedvi_buf_sizemem_bothash_sizebadifmodreswitchdone1done4foundnot_founddecrwhiledo_nothingemptycharxordxchrinvalid_codechrpackedname_of_fileerstata_open_ina_open_outb_open_outw_open_outb_closefirstinput_lnnotfalseReport overflow of the input buffer, and abortterm_outupdate_terminalbreak_inwrite_lnlimit_fieldlocandpool_pointerstr_poolstr_ptrlengthflush_charmake_stringstr_eq_bufget_strings_startedapp_lc_hexCharacter |k| cannot be printed\"~repeatreadread_lnlog_onlynew_stringselectorterm_offsettrick_countwtermwlogprint_lnprint_charnlslow_printprint_esc\"Aprint_two\"\"\"nonnegative_integerprint_current_stringfatal_errornonstop_modeprint_errInitialize the print |selector| based on |interaction|error_message_issuedset_box_allowednormalize_selectorshow_contextclose_files_and_terminatedebug_helphlp2hlp5help_ptrhelp3help6errors3Get user's advice and |return|\"? \"3\"6Delete \\(c)|c-\"0\"| tokens and |goto continue|base_ptrname_field\"HIntroduce new material from the terminal and |return|\"SPrint the menu of available options\"I to insert something, \"H for help, X to quit.\"nonstopmode\"insert>cur_chr\"I have just deleted some text, as you asked.\"Maybe you should try asking a human?\"``If all else fails, read the instructions.''\")succumb\"=\"you can ask a wizard to enlarge me.\"I'm broken. Please show this to someone who can fix can fix\"in fact, I'm barely conscious. Please fix it and try again.pause_for_instructions\"Try to insert some instructions for me (e.g.,`I\\showlists'),unitysmall_numberdeltanx_plus_y", '\000' <repeats 63861 times>
(gdb) p bytestart
$20 = {0, 0, 0, 0, 6, 5, 19, 13, 8, 24, 38, 13, 52, 62, 16, 68, 71, 26, 102, 76, 64, 105, 101, 89, 117, 111, 98, 122, 116, 102, 126, 120, 106, 161, 170, 116, 167, 178, 123, 174, 186, 133, 189, 200, 143, 200, 208, 156, 210, 217, 167, 226, 226, 176, 235, 238, 188, 249, 247, 195, 256, 256, 204, 266, 265, 207, 273, 310, 209, 275, 314, 212, 279, 321, 220, 287, 325, 225, 292, 330, 230, 297, 335, 235, 303, 341, 244, 316, 345, 248, 322, 349, 253, 326, 351, 263, 332, 355, 268, 342, 364, 272, 357, 378, 276, 362, 380, 280, 371, 395, 292, 374, 397, 295, 384, 407, 301, 388, 416, 313, 399, 424, 319, 409, 432, 328, 416, 437, 338, 423, 446, 348, 432, 455, 358, 439, 460, 365, 446, 466, 370, 450, 479, 378, 461, 492, 381, 464, 495, 386, 468, 499, 432, 470, 506, 440, 479, 516, 455, 484, 530, 463, 500, 542, 471, 509, 551, 482, 517, 563, 485, 530, 568, 488, 532, 570, 500, 542, 587, 508, 551, 595, 515, 564, 607, 521, 574, 618, 531, 582, 628, 542, 600, 640, 552, 606, 650, 571, 632, 762, 581, 634, 764, 612, 636, 767, 614, 645, 775, 620, 740, 780, 624, 764, 782, 631, 772, 791, 639, 784, 797, 649, 796, 805, 657, 799, 810, 668, 810, 819, 679, 821, 849, 684, 829, 857, 688, 836, 864, 696, 840, 874, 706, 887, 879, 708, 891, 892, 718, 899, 895, 727, 947, 909, 729, 956, 911, 738, 959, 920, 741, 970, 935, 760, 984, 937, 780, 996, 947, 791, 1025, 957, 803, 1036, 972, 812, 1047, 975, 866, 1055, 989, 886, 1071, 1006, 901, 1078, 1017, 919, 1085, 1026, 931, 1103, 1039, 956, 1125, 1052, 966, 1129, 1061, 970, 1133, 1065, 974, 1137, 1070, 982, 1142, 1075, 987, 1147, 1080, 992, 1159, 1088, 997, 1161, 1090, 999, 1163, 1092, 1029, 1206, 1131, 1032, 1245, 1133, 1034, 1247, 1135, 1036, 1249, 1137, 1081, 1251, 1139, 1089, 1274, 1150, 1099, 1284, 1154, 1101, 1330, 1156, 1154, 1332, 1158, 1156, 1373, 1160, 1191, 1434, 1205, 1215, 1455, 1262, 1238, 1469, 1272, 1250, 1480, 1276, 1258, 1487, 1283, 1265, 1498, 1298, 1310, 1547, 1349, 1347, 1590, 1409, 1393, 1599, 1412, 1395, 1609, 1420, 1402, 1624, 1451, 1404, 1626, 1496, 1440, 1635, 1516, 1500, 1671, 1572, 1560, 1686, 1581, 1582, 1699, 1591, 1643, 1743, 1595, 1648, 1746, 1601, 1660, 1760, 1613, 1665, 1771, 1622, 1674, 1783, 0 <repeats 9589 times>}
```

Here is all the byte memory after phase one of `POOLtype`.  Hover over an index in the `byte_start` array (last row) to see the string it represents in the `byte_mem` arrays (first three rows).

<div id="whereToAddDivs"></div>

And here are just the strings (name `i`, for each `i`), in a more readable form.

<div id="whereToAddDivsBetter"></div>
<script src="tangle-mem.js"></script>
<script src="pretty-5.js"></script>

## Getting to the names and vice-versa

With the above `byte_mem` arrays, these names are stored in memory, and with the `byte_start` array, we can get from "name `i`" to the actual name.

We also need a way of getting from the actual name (sequence of characters) to its number, i.e. a way to look up names in the above arrays. This is explained in the next two sections. Of the different kinds of names mentioned so far, a binary search tree is used for module names (as they need to be recognized given a prefix), and a hash table is used for everything else.

## What we need to store: Pascal texts (replacements / equivalents)

We also need to store “tokens”, but we'll discuss those later in Part 8. For now a brief discussion, as DEK mentions them in this section.

Above in `byte_mem` we stored the names of modules, the names of macros, double-quoted strings, and Pascal identifiers. For all the kinds of identifiers other than Pascal identifiers, we also need to store **equivalents**:

  - For modules, we need to store the Pascal code corresponding to them (the replacement text for that module).

  - For macros too, we need to store their replacement texts (definitions).

  - For double-quoted strings, we need to store the integers they should be treated as. (The order in which they were added to the string pool.)

The actual replacement texts (Pascal code, but potentially containing references to modules/macros/strings, which need to be replaced) are stored as consecutive tokens in `tok_mem` arrays, similar to the way names are stored as consecutive characters in `byte_mem` arrays. So when we say "equivalents" above, we need to store not the actual entire text of the replacement but some index saying "the replacement text for the name numbered `i` is the text numbered `j`".

## More info for each name

Above in `byte_mem` we stored the names of modules, the names of macros, double-quoted strings, and Pascal identifiers. As briefly mentioned above, we need to some more info for each name:

- For module names, we need some sort of binary search tree, so we need to store for each name `i`, a "left" and "right".

- For other names (strings, module names, macro names, Pascal identifiers), we need to store hash table "next" links (i.e. if name `i` is something that has hash `h`, then we need to store at name `i` where to look next for something else that has hash `h`).

- For these other names, we also need to store their type, whether they are (0) Pascal identifiers, (1) strings or numeric macros, (2) simple (string) macros, (3) parametric macros.

- For Pascal identifiers, given the constraints of Pascal compilers of the time, we also need something to check that we don't have two different identifiers x and y such that `x[:7].upper()` and `y[:7].upper()` are equal.

So let's keep track of the total info we need to keep for each name:

| Type of name                    | Text equivalent | Numeric equivalent | Tree “left” link | Tree “right” link | Hash table “next” link | Secondary hash table “next” link | Type code |
| :------------------------------ | :-------------: | :----------------: | :--------------: | :---------------: | :--------------------: | :------------------------------: | :-------: |
| Module name                     |        ✓        |                    |        ✓         |         ✓         |                        |                                  |           |
| (Double-quoted) String          |                 |         ✓          |                  |                   |           ✓            |                                  |     1     |
| Numeric macro name              |                 |         ✓          |                  |                   |           ✓            |                                  |     1     |
| Simple (text) macro name        |        ✓        |                    |                  |                   |           ✓            |                                  |     2     |
| Parametric macro name           |        ✓        |                    |                  |                   |           ✓            |                                  |     3     |
| Pascal keywords and identifiers |                 |                    |                  |                   |           ✓            |                ✓                 |     0     |

As we can see, we need to store three things for every type name. DEK's idea is that we can store them in just three arrays, as follows:

| Type of name                    | Text equivalent | Numeric equivalent |   Tree “left” link   |  Tree “right” link  | Hash table “next” link | Secondary hash table “next” link | Type code |
| :------------------------------ | :-------------: | :----------------: | :------------------: | :-----------------: | :--------------------: | :------------------------------: | :-------: |
| Module name                     |     `equiv`     |                    | `link` (aka `llink`) | `ilk` (aka `rlink`) |                        |                                  |           |
| (Double-quoted) String          |                 |      `equiv`       |                      |                     |         `link`         |                                  |   `ilk`   |
| Numeric macro name              |                 |      `equiv`       |                      |                     |         `link`         |                                  |   `ilk`   |
| Simple (text) macro name        |     `equiv`     |                    |                      |                     |         `link`         |                                  |   `ilk`   |
| Parametric macro name           |     `equiv`     |                    |                      |                     |         `link`         |                                  |   `ilk`   |
| Pascal keywords and identifiers |                 |                    |                      |                     |         `link`         |             `equiv`              |   `ilk`   |

As we can see, DEK only needs three arrays `equiv`, `link` and `ilk` (the last of these could be 2 bits in most cases but is unconditionally 16 bits) to store all the additional info for each name.

<object type="image/svg+xml" data="tangle-037.svg"></object>


<object type="image/svg+xml" data="tangle-038.svg"></object>


<object type="image/svg+xml" data="tangle-039.svg"></object>


<object type="image/svg+xml" data="tangle-040.svg"></object>


<object type="image/svg+xml" data="tangle-041.svg"></object>


<object type="image/svg+xml" data="tangle-042.svg"></object>


<object type="image/svg+xml" data="tangle-043.svg"></object>


<object type="image/svg+xml" data="tangle-044.svg"></object>


<object type="image/svg+xml" data="tangle-045.svg"></object>


<object type="image/svg+xml" data="tangle-046.svg"></object>


<object type="image/svg+xml" data="tangle-047.svg"></object>


<object type="image/svg+xml" data="tangle-048.svg"></object>


<object type="image/svg+xml" data="tangle-049.svg"></object>



