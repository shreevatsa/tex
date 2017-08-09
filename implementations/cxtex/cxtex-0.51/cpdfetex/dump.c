

#include "types.h"
#include "c-compat.h"
#include "globals.h"
#include "mainio.h"

boolean dump_option; /* was the dump name option used? */ 
boolean dump_line; /* was a \.{\%\AM format} line seen? */ 
const_string dump_name; /* The C version of what might wind up in DUMP_VAR.  */

/* module 1816 */
/* Are we printing extra info as we read the format file? */

boolean debug_format_file;

/* module 1444 */
/* 
 * After \.{INITEX} has seen a collection of fonts and macros, it
 * can write all the necessary information on an auxiliary file so
 * that production versions of \TeX\ are able to initialize their
 * memory at high speed. The present section of the program takes
 * care of such output and input. We shall consider simultaneously
 * the processes of storing and restoring,
 * so that the inverse relation between them is clear.
 * 
 * The global variable |format_ident| is a string that is printed right
 * after the |banner| line when \TeX\ is ready to start. For \.{INITEX} this
 * string says simply `\.{(INITEX)}'; for other versions of \TeX\ it says,
 * for example, `\.{(preloaded format=plain 82.11.19)}', showing the year,
 * month, and day that the format file was created. We have |format_ident=0|
 * before \TeX's tables are loaded.
 */
str_number format_ident;

/* module 1450 */

/* Format files consist of |memory_word| items, and we use the following
 * macros to dump words of different types:
 */

FILE *fmt_file; /* for input or output of format information */

/* module 1448 */
#define too_small( arg ) { wake_up_terminal ;\
           fprintf(term_out,"%s%s\n","---! Must increase the ",arg ); goto  BAD_FMT ;}


/* module 1447 */

void 
store_fmt_file (void) {
  int j,k, l; /* all-purpose indices */ 
  pointer p, q; /* all-purpose pointers */ 
  int x; /* something to dump */
  /* begin expansion of If dumping is not allowed, abort */
  /* module 1449 */
  /* The user is not allowed to dump a format file unless |save_ptr=0|.
   * This condition implies that |cur_level=level_one|, hence
   * the |xeq_level| array is constant and it need not be dumped.
   */
  if (save_ptr != 0) {
	print_err ("You can't dump inside a group");
	help1 ("`{...\\dump}' is a no-no.");
	succumb;
  };
  /* end expansion of If dumping is not allowed, abort */
  /* begin expansion of Create the |format_ident|, open the format file, and inform the user that dumping has begun */
  /* module 1473 */
  selector = new_string;
  zprint_string(" (format=");
  zprint (jobname);
  print_char (' ');
  print_int (year);
  print_char ('.');
  print_int (month);
  print_char ('.');
  print_int (day);
  print_char (')');
  if (interaction == batch_mode) {
	selector = log_only;
  } else {
	selector = term_and_log;
  }
  str_room (1);
  format_ident = make_string();
  pack_job_name (format_extension);
  while (!w_open_out (fmt_file))
	prompt_file_name ("format file name",format_extension);
  print_nl_string("Beginning to dump on file ");
  slow_print (w_make_name_string (fmt_file));
  flush_string;
  print_nl_string("");
  slow_print (format_ident);
  /* end expansion of Create the |format_ident|, open the format file, and inform the user that dumping has begun */
  /* begin expansion of Dump constants for consistency check */
  /* module 1452 */
  /* The next few sections of the program should make it clear how we use the
   * dump/undump macros.
   */
  dump_int (max_halfword);
  /* Dump the \eTeX\ state */
  dump_etex_stuff();
  dump_int (mem_bot);
  dump_int (mem_top);
  dump_int (eqtb_size);
  dump_int (hash_prime);
  dump_int (hyph_prime);
  /* end expansion of Dump constants for consistency check */
  dump_int (1296847960); /* ML\TeX's magic constant: "MLTX" */ 
  if (mltex_p) {
	dump_int (1);
  } else {
	dump_int (0);
  }
  /* begin expansion of Dump the string pool */
  /* module 1454 */
  dump_int (pool_ptr);
  dump_int (str_ptr);
  dump_things (str_start[0], str_ptr + 1);
  dump_things (str_pool[0], pool_ptr);
  print_ln();
  print_int (str_ptr);
  zprint_string(" strings of total length ");
  print_int (pool_ptr);
  /* end expansion of Dump the string pool */
  /* begin expansion of Dump the dynamic memory */
  /* module 1456 */
  /* By sorting the list of available spaces in the variable-size portion of
   * |mem|, we are usually able to get by without having to dump very much
   * of the dynamic memory.
   * 
   * We recompute |var_used| and |dyn_used|, so that \.{INITEX} dumps valid
   * information even when it has not been gathering statistics.
   */
  sort_avail();
  var_used = 0;
  dump_int (lo_mem_max);
  dump_int (rover);
  if (eTeX_ex) {
	for (k = int_val; k <= tok_val; k++) {
	  dump_int (sa_root[k]);
	}
  }
  p = mem_bot;
  q = rover;
  x = 0;
  do {
	dump_things (mem[p], q + 2 - p);
	x = x + q + 2 - p;
	var_used = var_used + q - p;
	p = q + node_size (q);
	q = rlink (q);
  } while (!(q == rover));
  var_used = var_used + lo_mem_max - p;
  dyn_used = mem_end + 1 - hi_mem_min;
  dump_things (mem[p], lo_mem_max + 1 - p);
  x = x + lo_mem_max + 1 - p;
  dump_int (hi_mem_min);
  dump_int (avail);
  dump_things (mem[hi_mem_min], mem_end + 1 - hi_mem_min);
  x = x + mem_end + 1 - hi_mem_min;
  p = avail;
  while (p != null) {
	decr (dyn_used);
	p = link (p);
  };
  dump_int (var_used);
  dump_int (dyn_used);
  print_ln();
  print_int (x);
  zprint_string   (" memory locations dumped; current usage is ");
  print_int (var_used);
  print_char ('&');
  print_int (dyn_used);
  /* end expansion of Dump the dynamic memory */
  /* begin expansion of Dump the table of equivalents */
  /* TH: The C version dumps eqtb[] in it's entirity. There was
   * an error in the converted pascal code that handled
   * to manually compressed information, and I don't feel like 
   * debugging that bit of code. eqtb isn't THAT big anyway.
   * TH oops, wrong. with 1M csnames, eqtb's size is suddenly substantial
   *
   */
  /* l = eqtb_size+1;*/
  /* dump_things (eqtb[0], l);*/
  /* region 1-4: */
  k=active_base;
  do {
	j=k;
	while (j<int_base-1) {
	  if ((equiv(j)==equiv(j+1))&&
		  (eq_type(j)==eq_type(j+1))&&
		  (eq_level(j)==eq_level(j+1))) { 
		goto FOUND1; 
	  }
	  incr(j);
	}
	l=int_base; 
	goto DONE1; /*|j=int_base-1|*/
  FOUND1: 
	incr(j); 
	l=j;
	while (j<int_base-1) {
	  if ((equiv(j)!=equiv(j+1))||
		  (eq_type(j)!=eq_type(j+1))||
		  (eq_level(j)!=eq_level(j+1))) {
		goto DONE1;
	  }
	  incr(j);
	}
  DONE1:
	dump_int(l-k);
	dump_things(eqtb[k], l-k);
	k=j+1; 
	dump_int(k-l);
  } while (k!=int_base);
  /* regions 5&6 */
  do {
	j=k;
	while (j<eqtb_size) {
	  if (eqtb[j].cint==eqtb[j+1].cint) {
		goto FOUND2;
	  }
	  incr(j);
	};
	l=eqtb_size+1; 
	goto DONE2; /*|j=eqtb_size|*/
  FOUND2: 
	incr(j); 
	l=j;
	while (j<eqtb_size) {
	  if (eqtb[j].cint!=eqtb[j+1].cint) {
		goto DONE2;
	  }
	  incr(j);
	};
  DONE2:
	dump_int(l-k);
	dump_things(eqtb[k], l-k);
	k=j+1; 
	dump_int(k-l);
  } while (k<=eqtb_size);

  dump_int (par_loc);
  dump_int (write_loc);

  /* Dump the hash table */
  dump_hash();
  /* end expansion of Dump the hash table */
  /* end expansion of Dump the table of equivalents */
  /* begin expansion of Dump the font information */
  /* module 1465 */
  dump_int (fmem_ptr);
  dump_things (font_info[0], fmem_ptr);
  dump_int (font_ptr);
  /* begin expansion of Dump the array info for internal font number |k| */
  /* module 1467 */
  dump_things (font_check[null_font], font_ptr + 1 - null_font);
  dump_things (font_size[null_font], font_ptr + 1 - null_font);
  dump_things (font_dsize[null_font], font_ptr + 1 - null_font);
  dump_things (font_params[null_font], font_ptr + 1 - null_font);
  dump_things (hyphen_char[null_font], font_ptr + 1 - null_font);
  dump_things (skew_char[null_font], font_ptr + 1 - null_font);
  dump_things (font_name[null_font], font_ptr + 1 - null_font);
  dump_things (font_area[null_font], font_ptr + 1 - null_font);
  dump_things (font_bc[null_font], font_ptr + 1 - null_font);
  dump_things (font_ec[null_font], font_ptr + 1 - null_font);
  dump_things (char_base[null_font], font_ptr + 1 - null_font);
  dump_things (width_base[null_font], font_ptr + 1 - null_font);
  dump_things (height_base[null_font], font_ptr + 1 - null_font);
  dump_things (depth_base[null_font], font_ptr + 1 - null_font);
  dump_things (italic_base[null_font], font_ptr + 1 - null_font);
  dump_things (lig_kern_base[null_font], font_ptr + 1 - null_font);
  dump_things (kern_base[null_font], font_ptr + 1 - null_font);
  dump_things (exten_base[null_font], font_ptr + 1 - null_font);
  dump_things (param_base[null_font], font_ptr + 1 - null_font);
  dump_things (font_glue[null_font], font_ptr + 1 - null_font);
  dump_things (bchar_label[null_font], font_ptr + 1 - null_font);
  dump_things (font_bchar[null_font], font_ptr + 1 - null_font);
  dump_things (font_false_bchar[null_font], font_ptr + 1 - null_font);
  for (k = null_font; k <= font_ptr; k++) {
	print_nl_string("\\font");
	print_esc (font_id_text (k));
	print_char ('=');
	print_file_name (font_name[k], font_area[k],null_string);
	if (font_size[k] != font_dsize[k]) {
	  zprint_string(" at ");
	  print_scaled (font_size[k]);
	  zprint_string("pt");
	};
  };
  /* end expansion of Dump the array info for internal font number |k| */
  print_ln();
  print_int (fmem_ptr - 7);
  zprint_string(" words of font info for ");
  print_int (font_ptr - font_base);
  if (font_ptr != font_base + 1) {
	zprint_string(" preloaded fonts");
  } else {
	zprint_string (" preloaded font");
  }
  /* end expansion of Dump the font information */
  /* Dump the hyphenation tables */
  dump_trie();
  /* begin expansion of Dump a couple more things and the closing check word */
  /* module 1471 */
  /* We have already printed a lot of statistics, so we set |tracing_stats:=0|
   * to prevent them from appearing again.
   */
  dump_int (interaction);
  dump_int (format_ident);
  dump_int (69069);
  tracing_stats = 0;
  /* end expansion of Dump a couple more things and the closing check word */
  /* begin expansion of Close the format file */
  /* module 1474 */
  w_close (fmt_file);
  /* end expansion of Close the format file */
};

/* Here we write NITEMS items, each item being ITEM_SIZE bytes long.
   The pointer to the stuff to write is P, and we write to the file
   OUT_FILE.  */

void
do_dump (char * p,  int item_size,  int nitems,  FILE * out_file) {
  if (fwrite (p, item_size, nitems, out_file) != (unsigned)nitems) {
	fprintf (stderr, "! Could not write %d %d-byte item(s).\n", nitems, item_size);
	uexit (1);
  }
}


/* Here is the dual of the writing routine.  */

void
do_undump (char * p,  int item_size,  int nitems,  FILE * in_file) {
  if (fread (p, item_size, nitems, in_file) != (unsigned)nitems)
    FATAL2 ("Could not undump %d %d-byte item(s)", nitems, item_size);
}

#define undump(var,varx,vary)  { undump_int(x); if ((x<var) ||\
                                  (x>varx)) {goto BAD_FMT; } else  vary=x;}

#define undump_size(var,varw,varx,vary)  {\
                                    undump_int(x); \
                                    if (x<var) goto BAD_FMT; if (x>varw) {\
                                    too_small(varx) } else {\
                                    FORMAT_DEBUG (varx,x); vary = x; }}


/* module 1448 */

/* Corresponding to the procedure that dumps a format file, we have a function
 * that reads one in. The function returns |false| if the dumped format is
 * incompatible with the present \TeX\ table sizes, etc.
 */

/* module 524 */

/* Here is the only place we use |pack_buffered_name|. This part of the program
 * becomes active when a ``virgin'' \TeX\ is trying to get going, just after
 * the preliminary initialization, or when the user is substituting another
 * format file by typing `\.\&' after the initial `\.{**}' prompt. The buffer
 * contains the first line of input in |buffer[loc..(last-1)]|, where
 * |loc<last| and |buffer[loc]<>" "|.
 */

boolean 
open_fmt_file (void) {
  unsigned int j; /* the first space after the format file name */ 
  j = loc;
  if (buffer[loc] == '&') {
	incr (loc);
	j = loc;
	buffer[last] = ' ';
	while (buffer[j] != ' ')
	  incr (j);
	pack_buffered_name (0, loc, j - 1); /* Kpathsea does everything */
	if (w_open_in (fmt_file)) {
	  loc = j;
	  return true;
	}
	wake_up_terminal;
	wterm_string ("Sorry, I can't find the format `");
	fputs (stringcast (name_of_file + 1), stdout);
	wterm_string ("'; will try `");
	fputs (TEX_format_default + 1, stdout);
	wterm_string ("'.\n");
	update_terminal;
  };
  /* now pull out all the stops: try for the system \.{plain} file */
  pack_buffered_name (format_default_length - format_ext_length, 1, 0);
  if (!w_open_in (fmt_file)) {
	wake_up_terminal;
	wterm_string ("I can't find the format file `");
	fputs (TEX_format_default + 1, stdout);
	wterm_string("'!\n");
	return false;
  };
  loc = j;
  return true;
};

/* module 1451 */

/* The inverse macros are slightly more complicated, since we need to check
 * the range of the values we are reading in. We say `|undump(a, b, x)|' to
 * read an integer value |x| that is supposed to be in the range |a<=x<=b|.
 */

boolean load_fmt_file (void) {
  int j,k; /* all-purpose indices */ 
  pointer p, q;  /* all-purpose pointers */ 
  int x;  /* something undumped */
  /* begin expansion of Undump constants for consistency check */
  if (ini_version) {
	libc_free (font_info);
	libc_free (str_pool);
	libc_free (str_start);
	libc_free (eqtb);
	libc_free (yzmem);
  }
  undump_int (x);
  if (x != max_halfword)
    goto BAD_FMT; /* check |max_halfword| */ 
  eqtb = xmalloc_array (memory_word, eqtb_size + 1);
  eq_type (undefined_control_sequence) = undefined_cs;
  equiv (undefined_control_sequence) = null;
  eq_level (undefined_control_sequence) = level_zero;
  /* begin expansion of Undump the \eTeX\ state */
  /* module 1601 */
  undump (0, 1, eTeX_mode);
  if (eTeX_ex) {
    /* begin expansion of Initialize variables for \eTeX\ extended mode */
    /* module 1758 */
    max_reg_num = 32767;
    max_reg_help_line = "A register number must be between 0 and 32767.";
    /* end expansion of Initialize variables for \eTeX\ extended mode */
  }  else {
    /* begin expansion of Initialize variables for \eTeX\ compatibility mode */
    /* module 1757 */
    max_reg_num = 255;
    max_reg_help_line = "A register number must be between 0 and 255.";
    /* end expansion of Initialize variables for \eTeX\ compatibility mode */
  };
  /* end expansion of Undump the \eTeX\ state */
  undump_int (x);
  FORMAT_DEBUG ("mem_bot",x);
  if (x != mem_bot)
    goto BAD_FMT;
  undump_int (mem_top);
  FORMAT_DEBUG ("mem_top", mem_top);
  if (mem_bot + 1100 > mem_top)
    goto BAD_FMT;
  head = contrib_head;
  tail = contrib_head;
  page_tail = page_head; /* page initialization */ 
  mem_min =  mem_bot - extra_mem_bot;
  mem_max = mem_top + extra_mem_top;
  yzmem = xmalloc_array (memory_word, mem_max - mem_min);
  mem = yzmem - mem_min; /* this pointer arithmetic fails with some compilers */
  undump_int (x);
  if (x != eqtb_size)
    goto BAD_FMT;
  undump_int (x);
  if (x != hash_prime)
    goto BAD_FMT;
  undump_int (x);
  if (x != hyph_prime)
    goto BAD_FMT;
  /* end expansion of Undump constants for consistency check */
  /* check magic constant of ML\TeX */ 
  undump_int (x);
  if (x != 1296847960)
    goto BAD_FMT;
  /* undump |mltex_p| flag into |mltex_enabled_p| */
  undump_int (x);
  if (x == 1) {
    mltex_enabled_p = true;
  } else if (x != 0)
	goto BAD_FMT;
  /* begin expansion of Undump the string pool */
  /* module 1455 */
  undump_size (0, sup_pool_size - pool_free, "string pool size", pool_ptr);
  if (pool_size < pool_ptr + pool_free)
    pool_size = pool_ptr + pool_free;
  undump_size (0, sup_max_strings - strings_free, "sup strings", str_ptr);
  if (max_strings < str_ptr + strings_free)
    max_strings = str_ptr + strings_free;
  str_start = xmalloc_array (pool_pointer, max_strings);
  undump_checked_things (0, pool_ptr, str_start[0],str_ptr+1);
  str_pool = xmalloc_array (packed_ASCII_code, pool_size);
  undump_things (str_pool[0], pool_ptr);
  init_str_ptr = str_ptr;
  init_pool_ptr = pool_ptr;
  /* end expansion of Undump the string pool */
  /* begin expansion of Undump the dynamic memory */
  /* module 1457 */
  undump (lo_mem_stat_max + 1000,hi_mem_stat_min -  1,lo_mem_max);
  undump (lo_mem_stat_max + 1,lo_mem_max,rover);
  if (eTeX_ex) {
    for (k = int_val; k <= tok_val; k++) {
      undump (null,lo_mem_max,sa_root[k]);
	}
  }
  p = mem_bot;
  q = rover;
  do {
    undump_things (mem[p], q + 2 - p);
    p = q + node_size (q);
    if ((p > lo_mem_max)|| ((q >= rlink (q)) && (rlink (q) != rover))) {
	  do_something;
      goto BAD_FMT;
	}
    q = rlink (q);
  } while (q != rover);
  undump_things (mem[p], lo_mem_max + 1 - p);
  if (mem_min < mem_bot - 2) {/* make more low memory available */
	p = llink (rover);
	q = mem_min + 1;
	link (mem_min) = null;
	info (mem_min) = null; /* we don't use the bottom word */ 
	rlink (p) = q;
	llink (rover) = q;
	rlink (q) = rover;
	llink (q) = p;
	link (q) = empty_flag;
	node_size (q) = mem_bot - q;
  };
  undump (lo_mem_max + 1,hi_mem_stat_min,hi_mem_min);
  undump (null,mem_top,avail);
  mem_end = mem_top;
  undump_things (mem[hi_mem_min], mem_end + 1 - hi_mem_min);
  undump_int (var_used);
  undump_int (dyn_used);
  FORMAT_DEBUG ("var_used", var_used);
  FORMAT_DEBUG ("dyn_used", dyn_used);
  /* end expansion of Undump the dynamic memory */
  /* begin expansion of Undump the table of equivalents */
  /* begin expansion of Undump regions 1 to 6 of |eqtb| */
  /*undump_things (eqtb[0], (eqtb_size+1));*/
  k=active_base;
  do {
	undump_int(x);
	if ((x<1)||((k+x)>eqtb_size+1)) { 
	  goto BAD_FMT;
	}
	undump_things(eqtb[k], x);
	k=k+x;
	undump_int(x);
	if ((x<0)||((k+x)>eqtb_size+1)) { 
	  goto BAD_FMT;
	}
	j=k;
	while (j<=k+x-1) { 
	  eqtb[j]=eqtb[k-1]; 
	  j++;
	};
	k=k+x;
  } while (k<=eqtb_size);

  /* end expansion of Undump regions 1 to 6 of |eqtb| */
  /* a direct call to |undump_int| removes the need for some hash globals,
   * just for the next two lines. Since this is rather static data, that
   * should be ok. (the |hash| cannot be expanded)
  */
  undump_int (par_loc);
  par_token = cs_token_flag + par_loc;
  undump_int (write_loc);

  /* begin expansion of Undump the hash table */
  undump_hash();
  /* end expansion of Undump the hash table */
  /* end expansion of Undump the table of equivalents */
  /* begin expansion of Undump the font information */
  /* module 1466 */
  undump_size (7,sup_font_mem_size,"font mem size",fmem_ptr);
  if (fmem_ptr > font_mem_size)
    font_mem_size = fmem_ptr;
  font_info = xmalloc_array (fmemory_word, font_mem_size);
  undump_things (font_info[0], fmem_ptr);
  undump_size (font_base,font_base +max_font_max,"font max",font_ptr);
  /* This undumps all of the font info, despite the name. */
  /* begin expansion of Undump the array info for internal font number |k| */
  /* module 1468 */
  /* This module should now be named `Undump all the font arrays'.
   */
  /* Allocate the font arrays */
  font_xmalloc(font_max);
  pdffont_xmalloc(font_max);
  vf_xmalloc(font_max);
  pdffont_initialize_init(font_max);
  undump_things (font_check[null_font], font_ptr + 1 - null_font);
  undump_things (font_size[null_font],  font_ptr + 1 - null_font);
  undump_things (font_dsize[null_font], font_ptr + 1 - null_font);
  undump_checked_things (min_halfword, max_halfword, font_params[null_font],  font_ptr + 1 - null_font);
  undump_things (hyphen_char[null_font],  font_ptr + 1 - null_font);
  undump_things (skew_char[null_font], font_ptr + 1 - null_font);
  undump_upper_check_things (str_ptr, font_name[null_font],(unsigned)(font_ptr + 1 - null_font));
  undump_upper_check_things (str_ptr, font_area[null_font],(unsigned)(font_ptr + 1 - null_font));
  /* There's no point in checking these values against the range $[0,255]$, since the data type is 
	 |unsigned char|, and all values of that type are in that range by definition. */
  undump_things (font_bc  [null_font], font_ptr + 1 - null_font);
  undump_things (font_ec[null_font],  font_ptr + 1 - null_font);
  undump_things (char_base[null_font], font_ptr + 1 - null_font);
  undump_things (width_base[null_font], font_ptr + 1 - null_font);
  undump_things (height_base[null_font], font_ptr + 1 - null_font);
  undump_things (depth_base[null_font], font_ptr + 1 - null_font);
  undump_things (italic_base[null_font], font_ptr + 1 - null_font);
  undump_things (lig_kern_base[null_font], font_ptr + 1 - null_font);
  undump_things (kern_base[null_font], font_ptr + 1 - null_font);
  undump_things (exten_base[null_font], font_ptr + 1 - null_font);
  undump_things (param_base[null_font], font_ptr + 1 - null_font);
  undump_checked_things (min_halfword, lo_mem_max,font_glue[null_font], font_ptr + 1 - null_font);
  undump_checked_things (0, fmem_ptr - 1, bchar_label[null_font],font_ptr + 1 - null_font);
  undump_checked_things (min_quarterword, non_char,font_bchar[null_font], font_ptr + 1 - null_font);
  undump_checked_things (min_quarterword, non_char,font_false_bchar[null_font],font_ptr + 1 - null_font);

  /* end expansion of Undump the array info for internal font number |k| */
  /* end expansion of Undump the font information */

  /* begin expansion of Undump the hyphenation tables */
  if(!undump_trie())
	goto BAD_FMT;

  /* begin expansion of Undump a couple more things and the closing check word */
  /* module 1472 */
  undump (batch_mode,error_stop_mode,interaction);
  if (interaction_option != unspecified_mode)
    interaction = interaction_option;
  undump (0,str_ptr,format_ident);
  undump_int (x);
  if ((x != 69069) || feof (fmt_file)) {
	do_something;
    goto BAD_FMT;
  }
  /* end expansion of Undump a couple more things and the closing check word */
  return true; /* it worked! */ 
 BAD_FMT:
  wake_up_terminal;
  wterm_string("(Fatal format file error(); I'm stymied)\n");
  return false;
};

/* module 1445 */

void
dump_initialize (void) {
  format_ident = 0;
}
