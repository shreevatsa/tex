# b1 = "bannerprogramLabels in the outer blockTypes in the outer blockprocedurebeginBasic printing proceduresend_of_TEXgubedinitInitialize table entries (done by \\.{INITEX} only)endcasesbuf_sizemax_print_linefont_maxnest_sizepool_sizetrie_op_sizepool_namefont_basehyph_sizeCheck the ``constant'' values for consistencythenrestartdonedone3done6found2incrloopdogototext_char"

# b2 = "mtypeTEXconstvarinitializeInitialize whatever \\TeX\\ might accessError handling proceduresfinal_endstattiniothercasesmem_maxerror_linestack_sizefont_mem_sizemax_stringssave_sizedvi_buf_sizemem_bothash_sizebadifmodreswitchdone1done4foundnot_founddecrwhiledo_nothingemptychar"

# bs = [0, 0, 0, 0, 6, 5, 19, 13, 8, 24, 38, 13, 52, 62, 16, 68, 71, 26, 102, 76, 64, 105, 101, 89, 117, 111, 98, 122, 116, 102, 126, 120, 106, 161, 170, 116, 167, 178, 123, 174, 186, 133, 189, 200, 143, 200, 208, 156, 210, 217, 167, 226, 226, 176, 235, 238, 188, 249, 247, 195, 256, 256, 204, 266, 265, 207, 273, 310, 209, 275, 314, 212, 279, 321, 220, 287, 325, 225, 292, 330, 230, 297, 335, 235, 303, 341, 244, 316, 345, 248, 322, 349, 253, 326, 351, 263, 332, 355, 268, 342]


raw = "Compiler directiveslabel"
bs = [0, len(raw) - 5]
bsp = []
w = 0

linked = (None,)
depth = 0
for i in range(len(raw)):
  linked = (raw[-i-1], linked)
  depth += 1
  if depth == len(raw) - bs[-w-1]:
    bsp = [linked] + bsp



# print len(bs)

# #define MAX 100
# char* bytestart_pointer[MAX];
# int main() {
#   for (int i = 0; i < MAX; ++i) {
#     if (i % 3 == 0) bytestart_pointer[i] = &bytemem0[bytestart[i]];
#     if (i % 3 == 1) bytestart_pointer[i] = &bytemem1[bytestart[i]];
#     if (i % 3 == 2) bytestart_pointer[i] = &bytemem2[bytestart[i]];
#   }
# }
