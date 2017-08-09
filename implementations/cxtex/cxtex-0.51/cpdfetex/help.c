
#include "types.h"
#include "c-compat.h"
#include "globals.h"


/* module 79 */

/* Individual lines of help are recorded in the array |help_line|, which
 * contains entries in positions |0..(help_ptr-1)|. They should be printed
 * in reverse order, i.e., with |help_line[0]| appearing last.
 */
char *help_line[6]; /* helps for the next |error| */ 
unsigned char help_ptr; /* the number of help lines present */ 
boolean use_err_help; /* should the |err_help| list be shown? */

/* module 1429 */

/* The |error| routine calls on |give_err_help| if help is requested from
 * the |err_help| parameter.
 */
void give_err_help (void) {
  token_show (err_help);
};

/* module 1483 */

/* 
 * Once \TeX\ is working, you should be able to diagnose most errors with
 * the \.{\\show} commands and other diagnostic features. But for the initial
 * stages of debugging, and for the revelation of really deep mysteries, you
 * can compile \TeX\ with a few more aids, including the \PASCAL\ runtime
 * checks and its debugger. An additional routine called |debug_help|
 * will also come into play when you type `\.D' after an error message;
 * |debug_help| also occurs just before a fatal error causes \TeX\ to succumb.
 * 
 * The interface to |debug_help| is primitive, but it is good enough when used
 * with a \PASCAL\ debugger that allows you to set breakpoints and to read
 * variables and change their values. After getting the prompt `\.{debug \#}', you
 * type either a negative number (this exits |debug_help|), or zero (this
 * goes to a location where you can set a breakpoint, thereby entering into
 * dialog with the \PASCAL\ debugger), or a positive number |m| followed by
 * an argument |n|. The meaning of |m| and |n| will be clear from the
 * program below. (If |m=13|, there is an additional argument, |l|.)
 */

#ifdef TEXMF_DEBUG
void 
debug_help (void) {	/* routine to display various things */
  int k, l, m, n;
  loop {
	wake_up_terminal;
	print_nl_string("debug # (-1 to exit):");
	update_terminal;
	m = read_terminal ();
	if (m == -1) {
	  return;
	} else if (m == 0) {
	  dump_core;	/* do something to cause a core dump */
	} else if (m > 0) {
	  n = read_terminal ();
	  switch (m) {
	  case 1:
		print_word (mem [n]);
		break;
	  case 2: /* display |mem[n]| in all forms */
		print_int (info (n));
		break;
	  case 3:
		print_int (link (n));
		break;
	  case 4:
		print_word (eqtb [n]);
		break;
	  case 5:
		print_scaled (font_info[n].sc);
		print_char (' ');
		print_int (font_info[n].qqqq.b0);
		print_char (':');
		print_int (font_info[n].qqqq.b1);
		print_char (':');
		print_int (font_info[n].qqqq.b2);
		print_char (':');
		print_int (font_info[n].qqqq.b3);
		break;
	  case 6:
		print_word (save_stack[n]);
		break;
	  case 7:
		show_box (n);
		break;
	  case 8: /* show a box, abbreviated by |show_box_depth| and |show_box_breadth| */ 
		breadth_max = 10000;
		depth_threshold = pool_size - pool_ptr - 10;
		show_node_list (n); /* show a box in its entirety */
		break;
	  case 9:
		show_token_list (n, null, 1000);
		break;
	  case 10:
		slow_print (n);
		break;
	  case 11:
		check_mem (n > 0);
		break;
	  case 12:  /* check wellformedness; zprint new busy locations if |n>0| */
		search_mem (n);
		break;
	  case 13:  /* look for pointers to |n| */ 
		l = read_terminal ();
		print_cmd_chr (n, l);
		break;
	  case 14:
		for (k = 0; k <= n; k++) {
		  zprint (buffer[k]);
		}
		do_something;
		break;
	  case 15:
		font_in_short_display = null_font;
		short_display (n);
		break;
	  case 16:
		panicking = !panicking;
		break;
	  default:
		zprint ('?');
	  };
	} else {
		zprint ('?');
	};
  };
};

#endif /* TEXMF_DEBUG */


void
help_initialize (void) {
  help_ptr = 0;
  use_err_help = false;
}

