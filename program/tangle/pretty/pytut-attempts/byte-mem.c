#include <stdio.h>
#include <string.h>

char bytemem0[] = "Compiler directiveslabelConstants in the outer blockGlobal variablesLocal variables for initializationendstart_of_TEXdebugtatsSet initial values of key variablesothersmem_minhalf_error_linemax_in_openparam_sizestring_vacanciestrie_sizefile_name_sizemem_tophash_primeintegerorexitcontinuedone2done5found1common_endingnegatetruereturnASCII_codefirst_text_chararraynull_codeforeight_bitsfilename_lengthrewrite_OKbooleanrewritew_open_ina_closew_closelastbypass_eolneofelse\" t_open_inbreakwake_up_terminalcur_inputoverflowinit_terminalsistr_numberstr_startinit_pool_ptrcur_lengthstr_room\"number of stringsresultMake the first 256 strings\"0";

char bytemem1[] = "bannerprogramLabels in the outer blockTypes in the outer blockprocedurebeginBasic printing proceduresend_of_TEXgubedinitInitialize table entries (done by \\.{INITEX} only)endcasesbuf_sizemax_print_linefont_maxnest_sizepool_sizetrie_op_sizepool_namefont_basehyph_sizeCheck the ``constant'' values for consistencythenrestartdonedone3done6found2incrloopdogototext_charlast_text_charofcarriage_returntoalpha_filebyte_filereset_OKfunctionresetb_open_inword_fileclosebuffermax_buf_stacklast_nonblankgeteolnterm_int_open_outclear_terminalformat_identloc_field\"buffer sizewritesopacked_ASCII_codepool_ptrinit_str_ptrappend_char\"pool sizeflush_stringstr_eq_strRead the other strings from the \\.{TEX.POOL} file and return |true|, or give an error message and return |false|\"a";

char bytemem2[] = "mtypeTEXconstvarinitializeInitialize whatever \\TeX\\ might accessError handling proceduresfinal_endstattiniothercasesmem_maxerror_linestack_sizefont_mem_sizemax_stringssave_sizedvi_buf_sizemem_bothash_sizebadifmodreswitchdone1done4foundnot_founddecrwhiledo_nothingemptycharxordxchrinvalid_codechrpackedname_of_fileerstata_open_ina_open_outb_open_outw_open_outb_closefirstinput_lnnotfalseReport overflow of the input buffer, and abortterm_outupdate_terminalbreak_inwrite_lnlimit_fieldlocandpool_pointerstr_poolstr_ptrlengthflush_charmake_stringstr_eq_bufget_strings_startedapp_lc_hex";
int bytestart[] = {0, 0, 0, 0, 6, 5, 19, 13, 8, 24, 38, 13, 52, 62, 16, 68, 71, 26, 102, 76, 64, 105, 101, 89, 117, 111, 98, 122, 116, 102, 126, 120, 106, 161, 170, 116, 167, 178, 123, 174, 186, 133, 189, 200, 143, 200, 208, 156, 210, 217, 167, 226, 226, 176, 235, 238, 188, 249, 247, 195, 256, 256, 204, 266, 265, 207, 273, 310, 209, 275, 314, 212, 279, 321, 220, 287, 325, 225, 292, 330, 230, 297, 335, 235, 303, 341, 244, 316, 345, 248, 322, 349, 253, 326, 351, 263, 332, 355, 268, 342, 364, 272, 357, 378, 276, 362, 380, 280, 371, 395, 292, 374, 397, 295, 384, 407, 301, 388, 416, 313, 399, 424, 319, 409, 432, 328, 416, 437, 338, 423, 446, 348, 432, 455, 358, 439, 460, 365, 446, 466, 370, 450, 479, 378, 461, 492, 381, 464, 495, 386, 468, 499, 432, 470, 506, 440, 479, 516, 455, 484, 530, 463, 500, 542, 471, 509, 551, 482, 517, 563, 485, 530, 568, 488, 532, 570, 500, 542, 587, 508, 551, 595, 515, 564, 607, 521, 574, 618, 531, 582, 628, 542, 600, 640, 552, 606, 650, 571, 632, 762, 581, 634, 764, 612, 636, 767, 614, 645, 775, 620, 740, 780, 624};

#define MAX 100
char* bytestart_pointer[MAX];
char tmp[2000];

int main() {
  printf("%ld\n", sizeof(bytestart) / sizeof(bytestart[0]));
  for (int i = 0; i < MAX; ++i) {
    if (i % 3 == 0) bytestart_pointer[i] = &bytemem0[bytestart[i]];
    if (i % 3 == 1) bytestart_pointer[i] = &bytemem1[bytestart[i]];
    if (i % 3 == 2) bytestart_pointer[i] = &bytemem2[bytestart[i]];
    int length = bytestart[i+3] - bytestart[i];
    strncpy(tmp, bytestart_pointer[i], length);
    tmp[length] = '\0';
    printf("%d %s\n", i%3, tmp);
  }
}
