/*
Copyright (c) 1996-2002 Han The Thanh, <thanh@pdftex.org>

This file is part of pdfTeX.

pdfTeX is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

pdfTeX is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with pdfTeX; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

$Id: epdf.h,v 1.2 2004/04/19 07:21:28 taco Exp taco $
*/

extern "C" {

/* the following code is extremly ugly but needed for including web2c/config.h */

/* #include "../kpathsea/c-proto.h" */   /* define P?H macros */

  /* typedef const char *const_string; *//* including kpathsea/types.h doesn't work on some systems */

#define KPATHSEA_CONFIG_H         /* avoid including other kpathsea header files */
                                  /* from web2c/config.h */

#ifdef CONFIG_H                   /* CONFIG_H has been defined by some xpdf */
#undef CONFIG_H                   /* header file */
#endif

#include "../pdftexd.h"
#include "ptexmac.h"

#define pdf_bufsize      pdfbufmax

extern float epdf_width;
extern float epdf_height;
extern float epdf_Orig_x;
extern float epdf_Orig_y;
extern integer epdf_selected_page;
extern integer epdf_num_pages;
extern integer epdf_page_box;
extern integer epdf_always_use_pdf_pagebox;
extern void *epdf_doc;
extern void *epdf_xref;

extern integer pdfstreamlength;
extern integer pdfptr;
typedef unsigned char eightbits  ;
extern eightbits pdfbuf[];
extern integer pdfbufmax;

extern char notdef[];

extern int is_subsetable(int);
extern int is_type1(int);
extern int lookup_fontmap(char *);
extern integer get_fontfile(int);
extern integer get_fontname(int);
extern integer pdfnewobjnum(void);
extern integer read_pdf_info(char*, char*, integer, integer, integer, integer);
extern void embed_whole_font(int);
extern void epdf_check_mem(void);
extern void epdf_delete(void);
extern void epdf_free(void);
extern void mark_glyphs(int, char *);
extern void pdf_printf(const char *fmt,...);
extern void pdf_puts(const char *);
extern void pdfbeginstream(void);
extern void pdfendobj(void);
extern void pdfendstream(void);
extern void pdfflush(void);
extern void pdftex_fail(const char *fmt,...);
extern void pdftex_warn(const char *fmt,...);
extern void tex_printf(const char *, ...);
extern void write_enc(char **, integer);
extern void write_epdf(void);
extern void zpdfbegindict(integer);
extern void zpdfbeginobj(integer);
extern void zpdfcreateobj(integer, integer);
extern void zpdfnewdict(integer, integer);

/* utils.c */
extern void convertStringToPDFString(char *in, char *out);

/* config.c */
extern integer cfgpar(integer);
}
