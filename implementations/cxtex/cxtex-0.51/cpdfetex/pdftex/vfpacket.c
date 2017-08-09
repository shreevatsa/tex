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

$Id: vfpacket.c,v 1.2 2004/05/11 14:30:32 taco Exp $
*/

#include "ptexlib.h"

typedef struct {
    internal_font_number font;
    char *dataptr;
    int  len;
} packet_entry;

static packet_entry *packet_ptr, *packet_tab = 0;
static int packet_max;

typedef struct {
    char **data;
    int *len;
    internal_font_number font;
}  vf_entry;

static vf_entry *vf_ptr, *vf_tab = 0;
static int vf_max;

static char *packet_data_ptr;

integer newvfpacket(internal_font_number f)
{
    int i,
        n = font_ec[f] - font_bc[f] + 1;
    entry_room(vf, 1, 256);
    vf_ptr->len = xtalloc(n, int);
    vf_ptr->data = xtalloc(n, char *);
    for (i = 0; i < n; i++) {
        vf_ptr->data[i] = 0;
        vf_ptr->len[i] = 0;
    }
    vf_ptr->font = f;
    return vf_ptr++ - vf_tab;
}

void storepacket(integer f, integer c, integer s)
{
    int l = str_start[s + 1] - str_start[s];
    vf_tab[vfpacketbase[f]].len[c - font_bc[f]] = l;
    vf_tab[vfpacketbase[f]].data[c - font_bc[f]] = xtalloc(l, char);
    memcpy((void *)vf_tab[vfpacketbase[f]].data[c - font_bc[f]], 
           (void *)(str_pool + str_start[s]), (unsigned)l);
}

void pushpacketstate()
{
    entry_room(packet, 1, 256);
    packet_ptr->font = f;
    packet_ptr->dataptr = packet_data_ptr;
    packet_ptr->len = vfpacketlength;
    packet_ptr++;
}

void poppacketstate()
{
    if (packet_ptr == packet_tab)
        pdftex_fail("packet stack empty, impossible to pop");
    packet_ptr--;
    f = packet_ptr->font;
    packet_data_ptr = packet_ptr->dataptr;
    vfpacketlength = packet_ptr->len;
}

void startpacket(internal_font_number f, integer c)
{
    packet_data_ptr = vf_tab[vfpacketbase[f]].data[c - font_bc[f]];
    vfpacketlength = vf_tab[vfpacketbase[f]].len[c - font_bc[f]];
}

eight_bits packetbyte()
{
    vfpacketlength--;
    return *packet_data_ptr++;
}

void vf_free(void)
{
    vf_entry *v;
    int n;
    char **p;
    if (vf_tab != 0) {
        for (v = vf_tab; v < vf_ptr; v++) {
            xfree(v->len);
            n = font_ec[v->font] - font_ec[v->font] + 1;
            for (p = v->data; p - v->data < n ; p++)
                xfree(*p);
            xfree(v->data);
        }
        xfree(vf_tab);
    }
    xfree(packet_tab);
}
