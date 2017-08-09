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

$Id: writeenc.c,v 1.2 2004/05/11 14:30:32 taco Exp $
*/

#include "ptexlib.h"

static const char perforce_id[] = 
    "$Id: writeenc.c,v 1.2 2004/05/11 14:30:32 taco Exp $";

enc_entry *enc_ptr, *enc_tab = 0;
int enc_max;

void read_enc(integer encoding)
{
    enc_entry *e = enc_tab + encoding;
    if (e->loaded)
        return;
    load_enc(e->name, e->glyph_names);
    e->loaded = true;
}

/* write_enc is used to write either external encoding (given in map file) or
 * internal encoding (read from the font file); when glyph_names is non null
 * the second argument will be treated as the number of the Encoding object;
 * otherwise the second argument will be treated as index of encoding in
 * enc_tab
 */

void write_enc(char **glyph_names, integer n)
{
    boolean is_notdef;
    int i;
    enc_entry *e;
    char **g;
    if (glyph_names == 0) {
        e = enc_tab + n;
        if (e->objnum != 0) /* the encoding has been written already */
            return;
        pdf_new_dict(0, 0);
        e->objnum = obj_ptr;
        g = e->glyph_names;
    }
    else {
        pdf_begin_dict(n);
        g = glyph_names;
    }
    pdf_printf("/Type /Encoding\n/Differences [ 0 /%s", g[0]);
    is_notdef = (g[0] == notdef);
    for (i = 1; i <= MAX_CHAR_CODE; i++) {
        if (g[i] == notdef) {
            if (!is_notdef) {
                pdf_printf(" %i/%s", i, notdef);
                is_notdef = true;
            }
        }
        else {
            if (is_notdef) {
                pdf_printf(" %i", i);
                is_notdef = false;
            }
            pdf_printf("/%s", g[i]);
        }
    }
    pdf_puts("]\n");
    pdfenddict();
}

integer add_enc(char *s) /* built-in encodings have s = 0 */
{
    int i;
    enc_entry *e;
    if (enc_tab != 0 && s != 0) {
        for (e = enc_tab; e < enc_ptr; e++)
            if (e->name != 0) /* don't check for built-in encodings */
                if  (strcmp(s, e->name) == 0)
                    return e - enc_tab;
    }
    entry_room(enc, 1, 256);
    if (s != 0)
        enc_ptr->name = xstrdup(s);
    else
        enc_ptr->name = 0;
    enc_ptr->loaded = false;
    enc_ptr->updated = false;
    enc_ptr->firstfont = get_null_font();
    enc_ptr->objnum = 0;
    enc_ptr->glyph_names = xtalloc(MAX_CHAR_CODE + 1, char *);
    for (i = 0; i <= MAX_CHAR_CODE; i++)
        enc_ptr->glyph_names[i] = xstrdup(notdef);
    return enc_ptr++ - enc_tab;
}

/* get encoding for map entry fm. When encoding vector is not given, try to
 * get it from T1 font file, in this case t1_read_enc sets the font being
 * reencoded, so next calls for the same entry doesn't cause reading the font
 * again
 */
boolean get_enc(fm_entry *fm)
{
    int i;
    char **glyph_names;
    if (is_reencoded(fm)) { /* external encoding vector available */
        read_enc(fm->encoding);
        return true;
    }
    if (!is_t1fontfile(fm)) /* get built-in encoding for T1 fonts only */
        return false;
    if (t1_read_enc(fm)) { /* encoding read into t1_builtin_glyph_names */
        fm->encoding = add_enc(0);
        glyph_names = enc_tab[fm->encoding].glyph_names;
        for (i = 0; i <= MAX_CHAR_CODE; i++)
            glyph_names[i] = t1_builtin_glyph_names[i];
        enc_tab[fm->encoding].loaded = true;
        return true;
    }
    return false;
}

/* check whether an encoding contains indexed glyph in form "/index123" */
/* boolean indexed_enc(fm_entry *fm) */
/* { */
/*     char **s = enc_tab[fm->encoding].glyph_names; */
/*     int i, n; */
/*     for (i = 0; i <= MAX_CHAR_CODE; i++, s++) */
/*         if (*s != 0 && *s != notdef &&  */
/*             sscanf(*s,  INDEXED_GLYPH_PREFIX "%i", &n) == 1) */
/*                 return true; */
/*     return false; */
/* } */

void setcharmap(internal_font_number f)
{
    fm_entry *fm;
    enc_entry *e;
    char **glyph_names;
    int i, k;
    if (pdfmovechars == 0 || font_bc[f] > 32 || pdffontmap[f] < 0)
        return;
    if (font_ec[f] < 128) {
        for (i = font_bc[f]; i <= 32; i++)
            pdfcharmap[f][i] = i + MOVE_CHARS_OFFSET;
        return;
    }
    fm = fm_tab + pdffontmap[f];
    if (pdfmovechars == 1 || !get_enc(fm))
        return;
    e = enc_tab + fm->encoding;
    if (e->firstfont != get_null_font()) {
        for (i = font_bc[f]; i <= 32; i++)
            pdfcharmap[f][i] = pdfcharmap[e->firstfont][i];
        return;
    }
    e->firstfont = f;
    glyph_names = e->glyph_names;
    for (i = 32, k = MAX_CHAR_CODE; i >= font_bc[f] && k > 127; i--) {
        if (glyph_names[i] == notdef)
            continue;
        while (glyph_names[k] != notdef && k > 127)
            k--;
        if (k < 128)
            return;
        glyph_names[k] = glyph_names[i];
        glyph_names[i] = xstrdup(notdef);
        pdfcharmap[f][i] = k;
    }
}

void enc_free()
{
    enc_entry *e;
    int k;
    for (e = enc_tab; e < enc_ptr; e++) {
        xfree(e->name);
        if (e->loaded != 0) { /* encoding has been loaded */
            for (k = 0; k <= MAX_CHAR_CODE; k++)
                if (e->glyph_names[k] != notdef)
                    xfree(e->glyph_names[k]);
        }
        xfree(e->glyph_names);
    }
    xfree(enc_tab);
}
