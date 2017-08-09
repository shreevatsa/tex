
#include "types.h"
#include "c-compat.h"
#include "globals.h"
#include "mainio.h"
#include "dviops.h"

#define vf_max_packet_length 10000
#define long_char 242
#define vf_id 202

#define vf_byte  getc(vf_file)
#define bad_vf(arg) { vf_err_str = arg ; goto  VF_ERROR ;}
#define tmp_b0  tmp_w . qqqq . b0
#define tmp_b1  tmp_w . qqqq . b1
#define tmp_b2  tmp_w . qqqq . b2
#define tmp_b3  tmp_w . qqqq . b3
#define tmp_int  tmp_w . cint
#define scaled3u ((((( tmp_b3  *  vf_z )  / 256 ) + ( tmp_b2  *  vf_z ))\
                  / 256 ) + ( tmp_b1  *  vf_z ))  /  vf_beta
#define scaled4(arg)  arg   =  scaled3u ;  if (  tmp_b0  > 0  ) {\
    if (  tmp_b0  == 255  ) {  arg   =  arg  -  vf_alpha; } }
#define scaled3(arg)  arg   =  scaled3u ;  if (  tmp_b1  > 127  ) {\
       arg   =  arg  -  vf_alpha; }
#define scaled2   if (  tmp_b2  > 127  )  { tmp_b1   = 255;}\
                  else   tmp_b1   = 0 ; scaled3
#define scaled1   if (  tmp_b3  > 127  )  { tmp_b1   = 255;}\
                  else   tmp_b1   = 0 ; tmp_b2   =  tmp_b1 ; scaled3


#define FOUR_CASES(arg)            arg: case arg  + 1: case arg  + 2: case arg  + 3


/* module 692 */
#define vf_max_recursion ( 10 ) /* \.{VF} files shouldn't recurse beyond this level */
#define vf_stack_size ( 100 ) /* \.{DVI} files shouldn't |push| beyond this depth */

/* module 682 */
integer *vfpacketbase;          /* base addresses of character packets  from virtual fonts */
internal_font_number *vf_default_font; /* default font in a \.{VF} file */
internal_font_number *vf_local_font_num; /* number of local fonts in a  \.{VF} file */
integer vfpacketlength;             /* length of the current packet */
FILE *vf_file;
internal_font_number vf_nf;      /* the local fonts counter */
integer *vf_e_fnts;              /* external font numbers */
internal_font_number *vf_i_fnts; /* corresponding internal font numbers */
memory_word tmp_w;               /* accumulator */
integer vf_z;                        /* multiplier */
integer vf_alpha;                    /* correction for negative values */
unsigned char vf_beta;           /* divisor */

/* module 694 */
unsigned int vf_cur_s; /* current recursion level */
vf_stack_record vf_stack[102];
vf_stack_index vf_stack_ptr; /* pointer into |vf_stack| */

/* module 684 */

/* The |do_vf| procedure attempts to read the \.{VF} file for a font, and sets
 * |pdf_font_type| to |real_font_type| if the \.{VF} file could not be found
 * or loaded, otherwise sets |pdf_font_type| to |virtual_font_type|. At this
 * time, |tmp_f| is the internal font number of the current \.{TFM} font. To
 * process font definitions in virtual font we call |vf_def_font|.
 */
void 
vf_replace_z (void)	{
  vf_alpha = 16;
  while (vf_z >= 8388608) {
	vf_z = vf_z / 2;
	vf_alpha = vf_alpha + vf_alpha;
  };
  vf_beta = 256 / vf_alpha;
  vf_alpha = vf_alpha * vf_z;
};

integer 
vf_read (int k)	{	/* read |k| bytes as an integer from \.{VF} file */
  int i;
  i = 0;
  while (k > 0) {
	i = i * 256 + vf_byte;
	decr (k);
  };
  return i;
};

void 
vf_local_font_warning (internal_font_number f, internal_font_number k, char *s)	{
  /* print a warning message if an error ocurrs during processing local fonts in \.{VF} file */
  print_nl_string (s);
  zprint_string(" in local font ");
  zprint (font_name[k]);
  zprint_string(" in virtual font ");
  zprint (font_name[f]);
  zprint_string(".vf ignored.");
};


internal_font_number 
vf_def_font (internal_font_number f)	{
  /* process a local font in \.{VF} file */
  internal_font_number k;
  str_number s;
  scaled ds, fs;
  four_quarters cs;
  cs.b0 = vf_byte;
  cs.b1 = vf_byte;
  cs.b2 = vf_byte;
  cs.b3 = vf_byte;
  tmp_b0 = vf_byte;
  tmp_b1 = vf_byte;
  tmp_b2 = vf_byte;
  tmp_b3 = vf_byte;
  scaled4 (fs);
  ds = vf_read (4) / 16;
  tmp_b0 = vf_byte;
  tmp_b1 = vf_byte;
  while (tmp_b0 > 0) {
	decr (tmp_b0);
	/*if (vf_byte > 0)
	  do_nothing; *//* skip the font path */ 
  };
  str_room (tmp_b1);
  while (tmp_b1 > 0) {
	decr (tmp_b1);
	append_char (vf_byte);
  };
  s = make_string();
  for (k = font_base + 1; k <= font_ptr; k++)
	if (str_eq_str (font_name[k], s)) {
	  flush_fontname_k (s);
	  if ((font_dsize[k] == ds) && (font_size[k] == fs)
		  && (pdf_font_expand_ratio[k] =
			  pdf_font_expand_ratio[f])
		    && (pdf_font_expand_factor[k] =
				pdf_font_expand_factor[f]))
		goto FOUND;
	};
  k = read_font_info (null_cs, s,null_string, fs);
 FOUND:
  if (k != null_font) {
	if (((cs.b0 != 0) || (cs.b1 != 0) || (cs.b2 != 0) || (cs.b3 != 0)) 
		&& ((font_check[k].b0 != 0) || (font_check[k].b1 != 0)
			|| (font_check[k].b2 != 0) || (font_check[k].b3 != 0))
		&& ((cs.b0 != font_check[k].b0) || (cs.b1 != font_check[k].b1)
			|| (cs.b2 != font_check[k].b2) || (cs.b3 != font_check[k].b3)))
	  vf_local_font_warning (f, k, "checksum mismatch");
	if (ds != font_dsize[k])
	  vf_local_font_warning (f, k, "design size mismatch");
  };
  if (pdf_font_expand_ratio[f] != 0)
	set_expand_param (k, f, pdf_font_expand_ratio[f]);
  return k;
};

void 
do_vf (void) {
  /* process \.{VF} file with font internal number |f| */
  int cmd, k, n;
  int cc, cmd_length;
  scaled tfm_width;
  str_number s;
  char * vf_err_str;
  vf_stack_index stack_level;
  internal_font_number save_vf_nf;
  internal_font_number f;
  f = tmp_f;
  stack_level = 0;
  pdf_font_type[f] = real_font_type;
  /* begin expansion of Open |vf_file|, return if not found */
  /* module 685 */
  pack_file_name (font_name[f],null_string,slow_make_tex_string(".vf"));
  if (!vf_b_open_in (vf_file))
	return;
  /* end expansion of Open |vf_file|, return if not found */
  /* begin expansion of Process the preamble */
  /* module 686 */
  if (vf_byte != pre)
	bad_vf ("PRE command expected");
  if (vf_byte != vf_id)
	bad_vf ("wrong id byte");
  cmd_length = vf_byte;
  for (k = 1; k <= cmd_length; k++)
	tmp_int = vf_byte;
  tmp_b0 = vf_byte;
  tmp_b1 = vf_byte;
  tmp_b2 = vf_byte;
  tmp_b3 = vf_byte;
  if (((tmp_b0 != 0) || (tmp_b1 != 0) || (tmp_b2 != 0)|| (tmp_b3 != 0)) 
	  && ((font_check[f].b0 != 0) || (font_check[f].b1 != 0)
		  || (font_check[f].b2 != 0) || (font_check[f].b3 != 0))
	  && ((tmp_b0 != font_check[f].b0) || (tmp_b1 != font_check[f].b1)
		  || (tmp_b2 != font_check[f].b2) || (tmp_b3 != font_check[f].b3))) {
	print_nl_string("checksum mismatch in font ");
	zprint (font_name[f]);
	zprint_string(".vf ignored");
  };
  if (vf_read (4) / 16 != font_dsize[f]) {
	print_nl_string("design size mismatch in font ");
	zprint (font_name[f]);
	zprint_string(".vf ignored");
  };
  update_terminal;
  vf_z = font_size[f];
  vf_replace_z();
  /* end expansion of Process the preamble */
  /* begin expansion of Process the font definitions */
  /* module 687 */
  cmd = vf_byte;
  save_vf_nf = vf_nf;
  while ((cmd >= fnt_def1) && (cmd <= fnt_def1 + 3)) {
	vf_e_fnts[vf_nf] = vf_read (cmd - fnt_def1 + 1);
	vf_i_fnts[vf_nf] = vf_def_font (f);
	incr (vf_nf);
	cmd = vf_byte;
  };
  vf_default_font[f] = save_vf_nf;
  vf_local_font_num[f] = vf_nf - save_vf_nf;
  /* end expansion of Process the font definitions */
  /* begin expansion of Allocate memory for the new virtual font */
  /* module 688 */
  vfpacketbase[f] = new_vf_packet (f);
  /* end expansion of Allocate memory for the new virtual font */
  while (cmd <= long_char) {
	/* begin expansion of Build a character packet */
	/* module 689 */
	if (cmd == long_char) {
	  vfpacketlength = vf_read (4);
	  cc = vf_read (4);
	  if (!is_valid_char (cc))
		bad_vf ("invalid character code");
	  tmp_b0 = vf_byte;
	  tmp_b1 = vf_byte;
	  tmp_b2 = vf_byte;
	  tmp_b3 = vf_byte;
	  scaled4 (tfm_width);
	}  else {
	  vfpacketlength = cmd;
	  cc = vf_byte;
	  if (!is_valid_char (cc))
		bad_vf ("invalid character code");
	  tmp_b1 = vf_byte;
	  tmp_b2 = vf_byte;
	  tmp_b3 = vf_byte;
	  scaled3 (tfm_width);
	};
	if (vfpacketlength < 0)
	  bad_vf ("negative packet length");
	if (vfpacketlength > vf_max_packet_length)
	  bad_vf ("packet length too long");
	if ((tfm_width != char_width (f, char_info (f, cc)))) {
	  print_nl_string("character width mismatch in font ");
	  zprint (font_name[f]);
	  zprint_string(".vf ignored");
	};
	str_room (vfpacketlength);
	while (vfpacketlength > 0) {
	  cmd = vf_byte;
	  decr (vfpacketlength);
	  /* begin expansion of Cases of \.{DVI} commands that can appear in character packet */
	  /* module 691 */
	  if ((cmd >= set_char_0) && (cmd <= set_char_0 + 127)) {
		cmd_length = 0;
	  } else if (((fnt_num_0 <= cmd) && (cmd <= fnt_num_0 + 63))
				 || ((fnt1 <= cmd) && (cmd <= fnt1 + 3))) {
		if (cmd >= fnt1) {
		  k = vf_read (cmd - fnt1 + 1);
		  vfpacketlength = vfpacketlength - (cmd - fnt1 + 1);
		} else {
		  k = cmd - fnt_num_0;
		}
		if (k >= 256)
		  bad_vf ("too many local fonts");
		n = 0;
		while ((n < vf_local_font_num[f]) && (vf_e_fnts[vf_default_font[f] + n] != k))
		  incr (n);
		if (n == vf_local_font_num[f])
		  bad_vf ("undefined local font");
		if (k <= 63) {
		  append_char (fnt_num_0 + k);
		} else {
		  append_char (fnt1);
		  append_char (k);
		};
		cmd_length = 0;
		cmd = nop;
	  } else {
		switch (cmd) {
		case set_rule:
		case put_rule:
		  cmd_length = 8;
		  break;
		case FOUR_CASES (set1):
		  cmd_length = cmd - set1 + 1;
		  break;
		case FOUR_CASES (put1):
		  cmd_length = cmd - put1 + 1;
		  break;
		case FOUR_CASES (right1):
		  cmd_length = cmd - right1 + 1;
		  break;
		case FOUR_CASES (w1):
		  cmd_length = cmd - w1 + 1;
		  break;
		case FOUR_CASES (x1):
		  cmd_length = cmd - x1 + 1;
			break;
		case FOUR_CASES (down1):
		  cmd_length = cmd - down1 + 1;
		  break;
		case FOUR_CASES (y1):
		  cmd_length = cmd - y1 + 1;
		  break;
		case FOUR_CASES (z1):
		  cmd_length = cmd - z1 + 1;
		  break;
		case FOUR_CASES (xxx1):
		  cmd_length = vf_read (cmd - xxx1 + 1);
		  vfpacketlength = vfpacketlength - (cmd - xxx1 + 1);
		  if (cmd_length > vf_max_packet_length)
			bad_vf ("packet length too long");
		  if (cmd_length < 0)
			bad_vf ("string of negative length");
		  append_char (xxx1);
		  append_char (cmd_length);
		  cmd = nop;	/* |cmd| has been already stored above as |xxx1| */
		  break;
		case w0:
		case x0:
		case y0:
		case z0:
		case nop:
		  cmd_length = 0;
		  break;
		case push:
		case pop:
		  cmd_length = 0;
		  if (cmd == push) {
			if (stack_level == vf_stack_size) {
			  overflow ("virtual font stack size",vf_stack_size);
			} else {
			  incr (stack_level);
			}
		  } else  if (stack_level == 0) {
			bad_vf ("more POPs than PUSHs in character");
		  } else {
			decr (stack_level);
		  };
		  break;
		default:
		  bad_vf ("improver DVI command");
		  ;
		};
	  };
	  /* end expansion of Cases of \.{DVI} commands that can appear in character packet */
	  if (cmd != nop)
		append_char (cmd);
	  vfpacketlength = vfpacketlength - cmd_length;
	  while (cmd_length > 0) {
		decr (cmd_length);
		append_char (vf_byte);
	  };
	};
	if (stack_level != 0)
	  bad_vf ("more PUSHs than POPs in character packet");
	if (vfpacketlength != 0)
	  bad_vf ("invalid packet length or DVI command in packet");
	 /* begin expansion of Store the packet being built */
	/* module 690 */
	s = make_string();
	storepacket (f, cc, s);
	flush_str (s);
	/* end expansion of Store the packet being built */
	cmd = vf_byte;
	/* end expansion of Build a character packet */
  };
  if (cmd != post)
	bad_vf ("POST command expected");
  b_close (vf_file);
  pdf_font_type[f] = virtual_font_type;
  return;
 VF_ERROR:
  print_nl_string("Error in processing VF font (");
  zprint (font_name[f]);
  zprint_string(".vf): ");
  zprint_string (vf_err_str);
  zprint_string(", virtual font will be ignored");
  print_ln();
  b_close (vf_file);
  update_terminal;
};


/* module 696 */

/* Some functions for processing character packets.
 */
integer 
packet_read (int k) {
  /* read |k| bytes as an integer from character packet */
  int i;
  i = 0;
  while (k > 0) {
	i = i * 256 + packet_byte();
	decr (k);
  };
  return i;
};

integer 
packet_scaled (int k) {
  /* get |k| bytes from packet as a scaled */
  scaled s;
  s=0; /*TH-Wall*/
  switch (k) {
  case 1:
	tmp_b3 = packet_byte();
	scaled1 (s);
	break;
  case 2:
	tmp_b2 = packet_byte();
	tmp_b3 = packet_byte();
	scaled2 (s);
	break;
  case 3:
	tmp_b1 = packet_byte();
	tmp_b2 = packet_byte();
	tmp_b3 = packet_byte();
	scaled3 (s);
	break;
  case 4:
	tmp_b0 = packet_byte();
	tmp_b1 = packet_byte();
	tmp_b2 = packet_byte();
	tmp_b3 = packet_byte();
	scaled4 (s);
	break;
  default:
	pdf_error_string("vf","invalid number size");
	;
  };
  return s;
}

/* module 692 */

/* The |do_vf_packet| procedure is called in order to interpret the
 * character packet for a virtual character. Such a packet may contain the
 * instruction to typeset a character from the same or an other virtual
 * font; in such cases |do_vf_packet| calls itself recursively. The
 * recursion level, i.e., the number of times this has happened, is kept
 * in the global variable |vf_cur_s| and should not exceed |vf_max_recursion|.
 */

void 
do_vf_packet  (internal_font_number f, eight_bits c) {
  /* typeset the \.{DVI} commands in the character packet 
	 for character |c| in current font |f| */
  internal_font_number save_vf, k, n;
  scaled save_h, save_v; /*base_line*/
  int cmd;
  boolean char_move;
  scaled w, x, y, z;
  str_number s;
  incr (vf_cur_s);
  if (vf_cur_s > vf_max_recursion)
	overflow ("max level recursion", vf_max_recursion);
  push_packet_state();
  start_packet (f, c);
  vf_z = font_size[f];
  vf_replace_z();
  save_vf = f;
  f = vf_i_fnts[vf_default_font[save_vf]];
  save_v = cur_v;
  save_h = cur_h;
  w = 0;
  x = 0;
  y = 0;
  z = 0;
  while (vfpacketlength > 0) {
	cmd = packet_byte();
	/* begin expansion of Do typesetting the \.{DVI} commands in virtual character packet */
	/* module 697 */
	/* The following code typesets a character to PDF output.
	 */
	if ((cmd >= set_char_0) && (cmd <= set_char_0 + 127)) {
	  if (!is_valid_char (cmd)) {
		char_warning (f, cmd);
		goto CONTINUE;
	  };
	  c = cmd;
	  char_move = true;
	  goto DO_CHAR;
	} else if (((fnt_num_0 <= cmd) && (cmd <= fnt_num_0 + 63)) || (cmd == fnt1)) {
	  if (cmd == fnt1) {
		k = packet_byte();
	  } else {
		k = cmd - fnt_num_0;
	  }
	  n = 0;
	  while ((n < vf_local_font_num[save_vf]) && (vf_e_fnts[vf_default_font[save_vf] + n] != k))
		incr (n);
	  if ((n == vf_local_font_num[save_vf])) {
		f = null_font;
	  } else {
		f = vf_i_fnts[vf_default_font[save_vf] + n];
	  }
	} else {
	  switch (cmd) {
	  case push:
		vf_stack[vf_stack_ptr].stack_h = cur_h;
		vf_stack[vf_stack_ptr].stack_v = cur_v;
		vf_stack[vf_stack_ptr].stack_w = w;
		vf_stack[vf_stack_ptr].stack_x = x;
		vf_stack[vf_stack_ptr].stack_y = y;
		vf_stack[vf_stack_ptr].stack_z = z;
		incr (vf_stack_ptr);
		break;
	  case pop:
		decr (vf_stack_ptr);
		cur_h = vf_stack[vf_stack_ptr].stack_h;
		cur_v = vf_stack[vf_stack_ptr].stack_v;
		w = vf_stack[vf_stack_ptr].stack_w;
		x = vf_stack[vf_stack_ptr].stack_x;
		y = vf_stack[vf_stack_ptr].stack_y;
		z = vf_stack[vf_stack_ptr].stack_z;
		break;
	  case FOUR_CASES (set1):
	  case FOUR_CASES (put1):
		if ((set1 <= cmd) && (cmd <= set1 + 3)) {
		  tmp_int = packet_read (cmd - set1 + 1);
		  char_move = true;
		} else {
		  tmp_int = packet_read (cmd - put1 + 1);
		  char_move = false;
		};
		if (!is_valid_char (tmp_int)) {
		  char_warning (f, tmp_int);
		  goto CONTINUE;
		};
		c = tmp_int;		  
		goto DO_CHAR;
	  case set_rule:
	  case put_rule:
		rule_ht = packet_scaled (4);
		rule_wd = packet_scaled (4);
		if ((rule_wd > 0) && (rule_ht > 0)) {
		  pdf_set_rule (cur_h, cur_v, rule_wd, rule_ht);
		  if (cmd == set_rule)
			cur_h = cur_h + rule_wd;
		};
		break;
	  case FOUR_CASES (right1):
		cur_h = cur_h + packet_scaled (cmd - right1 + 1);
		break;
	  case w0:
	  case FOUR_CASES (w1):
		if (cmd > w0)
		  w = packet_scaled (cmd - w0);
		cur_h = cur_h + w;
		break;
	  case x0:
	  case FOUR_CASES (x1):
		if (cmd > x0)
		  x = packet_scaled (cmd - x0);
		cur_h = cur_h + x;
		break;
	  case FOUR_CASES (down1):
		cur_v = cur_v + packet_scaled (cmd - down1 + 1);
		break;
	  case y0:
	  case FOUR_CASES (y1):
		if (cmd > y0)
		  y = packet_scaled (cmd - y0);
		cur_v = cur_v + y;
		break;
	  case z0:
	  case FOUR_CASES (z1):
		if (cmd > z0)
		  z = packet_scaled (cmd - z0);
		cur_v = cur_v + z;
		break;
	  case FOUR_CASES (xxx1):
		tmp_int = packet_read (cmd - xxx1 + 1);
		str_room (tmp_int);
		while (tmp_int > 0) {
		  decr (tmp_int);
		  append_char (packet_byte());
		};
		s = make_string();
		literal (s, true, true, false);
		flush_str (s);
		break;
	  default:
		pdf_error_string("vf","invalid DVI command");
	  };
	};
	goto CONTINUE;
  DO_CHAR:
	if (is_valid_char (c)) {
	  output_one_char (c);
	} else {
	  char_warning (f, c);
	}
	if (char_move)
	  cur_h = cur_h + char_width (f, char_info (f, c));
	/* end expansion of Do typesetting the \.{DVI} commands in virtual character packet */
  CONTINUE: 
	do_nothing; 
  };
  cur_h = save_h;
  cur_v = save_v;
  pop_packet_state();
  vf_z = font_size[f];
  vf_replace_z();
  decr (vf_cur_s);
};


void
vf_initialize (void) {
  vf_nf = 0;
  vf_cur_s = 0;
  vf_stack_ptr = 0;
}


void
vf_xmalloc (integer font_max) {
  vfpacketbase = xmalloc_array (integer, font_max);
  vf_default_font = xmalloc_array (internal_font_number, font_max);
  vf_local_font_num = xmalloc_array (internal_font_number, font_max);
  vf_e_fnts = xmalloc_array (integer, font_max);
  vf_i_fnts = xmalloc_array (internal_font_number, font_max);
}

