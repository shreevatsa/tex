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

$Id: config.c,v 1.2 2004/05/11 14:30:32 taco Exp $
*/

#include "ptexlib.h"

static const char perforce_id[] = 
    "$Id: config.c,v 1.2 2004/05/11 14:30:32 taco Exp $";

static FILE *cfg_file;
static char config_name[] = "pdftex.cfg";
boolean true_dimen;

#define cfg_open()       \
    open_input(&cfg_file, kpse_tex_format, FOPEN_RBIN_MODE)
#define cfg_close()      xfclose(cfg_file, cur_file_name)
#define cfg_getchar()    xgetc(cfg_file)
#define cfg_eof()        feof(cfg_file)

typedef struct {
    int code;
    const char *name;
    integer value;
    boolean is_true_dimen;
} cfg_entry;

#define CFG_BUF_SIZE     1024
#define CFG_FONTMAP_CODE 0

cfg_entry cfg_tab[] = {
    {CFG_FONTMAP_CODE,       "map",                  0, false},
    {cfg_output_code,          "output_format",        0, false},
    {cfg_adjust_spacing_code,   "adjust_spacing"      , 0, false},
    {cfg_compress_level_code,   "compress_level",       0, false},
    {cfg_decimal_digits_code,   "decimal_digits",       4, false},
    {cfg_move_chars_code,       "move_chars",           0, false},
    {cfg_image_resolution_code, "image_resolution",     0, false},
    {cfg_pk_resolution_code,    "pk_resolution",        0, false},
    {cfg_unique_resname_code,   "unique_resname",       0, false},
    {cfg_protrude_chars_code,   "protrude_chars",       0, false},
    {cfg_h_origin_code,         "horigin",              0, false},
    {cfg_v_origin_code,         "vorigin",              0, false},
    {cfg_page_height_code,      "page_height",          0, false},
    {cfg_page_width_code,       "page_width",           0, false},
    {cfg_link_margin_code,      "link_margin",          0, false},
    {cfg_dest_margin_code,      "dest_margin",          0, false},
    {cfg_thread_margin_code,    "thread_margin",        0, false},
    {cfg_pdf12_compliant_code,  "pdf12_compliant",      0, false},
    {cfg_pdf13_compliant_code,  "pdf13_compliant",      0, false},
    {cfg_pdf_minor_version_code, "pdf_minorversion",    -1, false},
    {cfg_always_use_pdf_pagebox_code,    "always_use_pdfpagebox",  0, false},
    {cfg_pdf_option_pdf_inclusion_errorlevel_code, "pdf_inclusion_errorlevel",  1, false},
    {0,                      0,                      0, false}
};

#define is_cfg_comment(c) (c == '*' || c == '#' || c == ';' || c == '%')

static char *add_map_file(char *s)
{
    char *p = s, *q = s;
    for (; *p != 0 && *p != ' ' && *p != 10; p++);
    if (*q != '+') {
        xfree(mapfiles);
        mapfiles = 0;
    }
    else
        q++;
    if (mapfiles == 0) {
        mapfiles = xtalloc(p - q + 2, char);
        *mapfiles = 0;
    }
    else
        mapfiles = 
            xretalloc(mapfiles, strlen(mapfiles) + p - q + 2, char);
    strncat(mapfiles, q, (unsigned)(p - q));
    strcat(mapfiles, "\n");
    return p;
}

void pdfmapfile(integer t)
{
    add_map_file(makecstring(tokens_to_string(t)));
}

void readconfigfile()
{
    int c, res, sign;
    cfg_entry *ce;
    char cfg_line[CFG_BUF_SIZE], *p;
    set_cur_file_name(config_name);
    if (!cfg_open())
        pdftex_fail("cannot open config file");
    cur_file_name = (char*)name_of_file + 1;
    tex_printf("{%s", cur_file_name);
    for (;;) {
        if (cfg_eof()) {
            cfg_close();
            tex_printf("}");
            cur_file_name = 0;
            break;
        }
        p = cfg_line;
        do {
            c = cfg_getchar();
            append_char_to_buf(c, p, cfg_line, CFG_BUF_SIZE);
        } while (c != 10);
        append_eol(p, cfg_line, CFG_BUF_SIZE);
        c = *cfg_line;
        if (p - cfg_line == 1 || is_cfg_comment(c))
            continue;
        p = cfg_line;
        for (ce = cfg_tab; ce->name != 0; ce++)
            if (!strncmp(cfg_line, ce->name, strlen(ce->name))) {
                break;
                }
        if (ce->name == 0) {
            remove_eol(p, cfg_line);
            pdftex_warn("invalid parameter name in config file: `%s'", cfg_line);
            continue;
        }
        p = cfg_line + strlen(ce->name);
        skip(p, ' ');
        skip(p, '=');
        skip(p, ' ');
        switch (ce->code) {
        case CFG_FONTMAP_CODE:
            p = add_map_file(p);
            break;
        case cfg_output_code:
        case cfg_adjust_spacing_code:
        case cfg_compress_level_code:
        case cfg_decimal_digits_code:
        case cfg_move_chars_code:
        case cfg_image_resolution_code:
        case cfg_pk_resolution_code:
        case cfg_unique_resname_code:
        case cfg_protrude_chars_code:
        case cfg_pdf12_compliant_code:
        case cfg_pdf13_compliant_code:
        case cfg_pdf_minor_version_code:
        case cfg_always_use_pdf_pagebox_code:
        case cfg_pdf_option_pdf_inclusion_errorlevel_code:
            if (*p == '-') {
                p++;
                sign = -1;
            }
            else
                sign = 1;
            ce->value = myatol(&p);
            if (ce->value == -1) {
                remove_eol(p, cfg_line);
                pdftex_warn("invalid parameter value in config filecode: `%s'", cfg_line);
                ce->value = 0;
            }
            else
                ce->value *= sign;
            break;
        case cfg_h_origin_code:
        case cfg_v_origin_code:
        case cfg_page_height_code:
        case cfg_page_width_code:
        case cfg_link_margin_code:
        case cfg_thread_margin_code:
            ce->value = myatodim(&p);
            ce->is_true_dimen = true_dimen;
            break;
        }
        skip(p, ' ');
        if (*p != 10 && !is_cfg_comment(*p)) {
            remove_eol(p, cfg_line);
            pdftex_warn("invalid line in config file: `%s'", cfg_line);
        }
    }
    res = cfgpar(cfg_pk_resolution_code);
    if (res == 0)
        res = 600;
    kpse_init_prog("pdfTeX", (unsigned)res, NULL, NULL);
    if (mapfiles == 0)
        mapfiles = xstrdup("psfonts.map\n");
}

boolean iscfgtruedimen(integer code)
{
    cfg_entry *ce;
    for (ce = cfg_tab; ce->name != 0; ce++)
        if (ce->code == code)
            return ce->is_true_dimen;
    return 0;
}

integer cfgpar(integer code)
{
    cfg_entry *ce;
    for (ce = cfg_tab; ce->name != 0; ce++)
        if (ce->code == code)
           return ce->value;
    return 0;
}
