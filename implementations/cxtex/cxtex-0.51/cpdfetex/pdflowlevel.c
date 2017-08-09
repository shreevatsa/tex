
#include "types.h"
#include "c-compat.h"
#include "globals.h"

/* web2c's pdfetex.h :*/

/* writepdf() always writes by fwrite() */
#define       write_pdf(a, b) \
  (void) fwrite ((char *) &pdf_buf[a], sizeof (pdf_buf[a]), \
                 (int) ((b) - (a) + 1), pdf_file)


/* module 654 */

integer fixed_output; /* fixed output format */
FILE *pdf_file; /* the PDF output file */
eight_bits pdf_buf[pdf_buf_size]; /* the PDF buffer */
integer pdf_ptr; /* pointer to the first unused byte in the PDF buffer */
integer pdf_gone; /* number of bytes that were flushed to output */
integer pdf_save_offset; /* to save |pdf_offset| */
integer zip_write_state; /* which state of compression we are in */
integer fixed_pdf_minor_version; /* fixed minor part of the pdf version */
boolean  pdf_minor_version_has_been_written; /* flag if the pdf version has been written */

/* module 657 */

/* This checks that |pdfoptionpdfminorversion| can only be set before any
 * bytes have been written to the generated pdf file. It should be called
 * directly every after |ensure_pdf_open|.
 */
void 
check_and_set_pdfoptionpdfminorversion (void) {
  if (!pdf_minor_version_has_been_written) {
	pdf_minor_version_has_been_written = true;
	if ((pdf_option_pdf_minor_version < 0) || (pdf_option_pdf_minor_version > 9)) {
	  print_err ("pdfTeX error (illegal pdfoptionpdfminorversion)");
	  print_ln();
	  help2 ("The pdfoptionpdfminorversion must be between 0 and 9.",
			 "I changed this to 4.");
	  int_error (pdf_option_pdf_minor_version);
	  pdf_option_pdf_minor_version = 4;
	};
	fixed_pdf_minor_version = pdf_option_pdf_minor_version;
	pdf_buf[7] = chr (ord ('0') + fixed_pdf_minor_version);
  } else {
	if (fixed_pdf_minor_version != pdf_option_pdf_minor_version)
	  pdf_error_string("setup","\\pdfoptionpdfminorversion cannot be changed after data is written to the pdf file");
  };
};


/* module 659 */

/* The PDF buffer is flushed by calling |pdf_flush|, which checks the
 * variable |zip_write_state| and will compress the buffer before flushing if
 * neccesary. We call |pdf_begin_stream| to begin a stream and |pdf_end_stream|
 * to finish it. The stream contents will be compressed if compression is turn on.
 */
void 
pdf_flush (void) { /* flush out the |pdf_buf| */
  ensure_pdf_open();
  check_and_set_pdfoptionpdfminorversion();
  switch (zip_write_state) {
  case no_zip:
	if (pdf_ptr > 0) {
	  write_pdf (0, pdf_ptr - 1);
	  pdf_gone = pdf_gone + pdf_ptr;
	};
	break;
  case zip_writing:
	write_zip (false);
	break;
  case zip_finish:
	write_zip (true);
	zip_write_state = no_zip;
  };
  pdf_ptr = 0;
};

void 
pdf_begin_stream (void) { /* begin a stream */
  ensure_pdf_open();
  check_and_set_pdfoptionpdfminorversion();
  pdf_print_ln_string("/Length           ");
  pdf_stream_length_offset = pdf_offset - 11;
  pdf_stream_length = 0;
  if (pdf_compress_level > 0) {
	pdf_print_ln_string("/Filter /FlateDecode");
	pdf_print_ln_string(">>");
	pdf_print_ln_string("stream");
	pdf_flush();
	zip_write_state = zip_writing;
  } else {
	pdf_print_ln_string(">>");
	pdf_print_ln_string("stream");
	pdf_save_offset = pdf_offset;
  };
}

void 
pdf_end_stream (void) {	/* end a stream */
  if (pdf_compress_level > 0) {
	zip_write_state = zip_finish;
  } else {
	pdf_stream_length = pdf_offset - pdf_save_offset;
  }
  pdf_flush();
  write_stream_length (pdf_stream_length, pdf_stream_length_offset);
  pdf_print_ln_string("endstream");
  pdf_end_obj();
}

void
remove_last_space (void) {
  if ((pdf_ptr > 0) && (pdf_buf[pdf_ptr - 1] == 32))
    decr (pdf_ptr);
}



void
pdf_print_octal (integer n) { /* prints an integer in octal form to PDF buffer */
  unsigned char k; /* index to current digit; we assume that $|n|<10^{23}$ */ 
  k = 0;
  do {
	dig[k] = n % 8;
	n = n / 8;
	incr (k);
  } while (n != 0);
  if (k == 1) {
	pdf_out ('0');
	pdf_out ('0');
  };
  if (k == 2)
    pdf_out ('0');
  while (k > 0) {
	decr (k);
	pdf_out ('0' + dig[k]);
  };
}


void
pdf_print_char (internal_font_number f, integer c) {
  /* print out a character to PDF buffer; the character will be printed in octal 
     form in the following cases: chars <= 32, backslash (92), left parenthesis (40)
     and  right parenthesis (41)  */
  if (c <= 32)
    c = pdf_char_map[f][c];
  pdf_mark_char (f, c);
  if ((c <= 32) || (c == 92) || (c == 40) || (c == 41) || (c > 127)) {
	pdf_out (92);		/* output a backslash */
	pdf_print_octal (c);
  } else {
	pdf_out (c);
  }
}

void
pdf_print (str_number s) { /* print out a string to PDF buffer */
  pool_pointer j;		/* current character code position */
  int c;
  j = str_start[s];
  while (j < str_start[s + 1]) {
	c = str_pool[j];
	pdf_out (c);
	incr (j);
  };
}

void
pdf_print_string (char * s) { /* print out a string to PDF buffer */
  while (*s) {
    pdf_out (str_pool[(integer)*s]);
	s++;
  };
}




void
literal (str_number s, boolean reset_origin, boolean is_special, boolean warn) {
  pool_pointer j;		/* current character code position */
  j = str_start[s];
  if (is_special) {
	if (!(str_in_str (s,"PDF:", 0)|| str_in_str (s,"pdf:", 0))){
	  if (warn && !(str_in_str (s,"SRC:", 0)|| str_in_str (s,"src:", 0)))
	    print_nl_string("Non-PDF special ignored!");
	  return;
	};
	j = j + strlen("PDF:");
	if (str_in_str (s,"direct:", strlen("PDF:"))) {
	  j = j + strlen("direct:");
	  reset_origin = false;
	} else {
	  reset_origin = true;
	};
  };
  if (reset_origin) {
	pdf_end_text();
	pdf_set_origin();
  } else {
	pdf_end_string();
	pdf_print_nl;
  };
  while (j < str_start[s + 1]) {
	pdf_out (str_pool[j]);
	incr (j);
  };
  pdf_print_nl;
}

void
pdf_print_int (integer n) { /* print out a integer to PDF buffer */
  unsigned char k;		/* index to current digit; we assume that $|n|<10^{23}$ */
  integer m;			/* used to negate |n| in possibly dangerous cases */
  k = 0;
  if (n < 0) {
	pdf_out ('-');
	if (n > -100000000) {
	  negate (n);
	} else {
	  m = -1 - n;
	  n = m / 10;
	  m = (m % 10) + 1;
	  k = 1;
	  if (m < 10) {
		dig[0] = m;
	  } else {
		dig[0] = 0;
		incr (n);
	  };
	};
  };
  do {
	dig[k] = n % 10;
	n = n / 10;
	incr (k);
  } while (n != 0);
  pdf_room (k);
  while (k > 0) {
	decr (k);
	pdf_quick_out ('0' + dig[k]);
  };
}

void
pdf_print_two (integer n) {
  /* prints two least significant digits in
     decimal form to PDF buffer */
  n = abs (n) % 100;
  pdf_out ('0' + (n / 10));
  pdf_out ('0' + (n % 10));
}


/* module 661 */

/* To print |scaled| value to PDF output we need some subroutines to ensure
 * accurary.
 */

scaled one_bp; /* scaled value corresponds to 1bp */
scaled one_hundred_bp; /* scaled value corresponds to 100bp */
scaled one_hundred_inch; /* scaled value corresponds to 100in */
integer ten_pow[10]; /* $10^0..10^9$ */
integer scaled_out; /* amount of |scaled| that was taken out in |divide_scaled| */
boolean init_pdf_output;

void 
pdf_print_real (integer m, integer d) {	/* print $m/10^d$ as real */
  integer n;
  if (m < 0) {
	pdf_out ('-');
	m = -m;
  };
  n = ten_pow[d];
  pdf_print_int (m / n);
  m = m % n;
  if (m > 0) {
	pdf_out ('.');
	n = n / 10;
	while (m < n) {
	  pdf_out ('0');
	  n = n / 10;
	};
	while (m % 10 == 0)
	  m = m / 10;
	pdf_print_int (m);
  };
}
		
void 
pdf_print_bp (scaled s) { /* print scaled as |bp| */
  pdf_print_real (divide_scaled (s, one_hundred_bp, fixed_decimal_digits + 2), fixed_decimal_digits);
}

void 
pdf_print_mag_bp (scaled s) {/* take |mag| into account */
  if ((mag != 1000) && (mag != 0))
	s = round_xn_over_d (s, mag, 1000);
  pdf_print_bp (s);
}


/* module 655 */
void
pdflowlevel_initialize (void) {
  int i;
  pdf_buf[0] = '%';
  pdf_buf[1] = 'P';
  pdf_buf[2] = 'D';
  pdf_buf[3] = 'F';
  pdf_buf[4] = '-';
  pdf_buf[5] = '1';
  pdf_buf[6] = '.';
  pdf_buf[7] = '4';
  pdf_buf[8] = pdf_new_line_char;
  pdf_ptr = 9;
  pdf_gone = 0;
  zip_write_state = no_zip;
  pdf_minor_version_has_been_written = false;
  /* module 662 */
  one_bp = 65782;		/* 65781.76 */
  one_hundred_bp = 6578176;
  one_hundred_inch = 473628672;
  ten_pow[0] = 1;
  for (i = 1; i <= 9; i++)
    ten_pow[i] = 10 * ten_pow[i - 1];
  init_pdf_output = false;
}


