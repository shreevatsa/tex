
#include "types.h"
#include "c-compat.h"


#include "globals.h"


/* module 669 */
integer obj_tab_size;
obj_entry *obj_tab;
integer head_tab[(head_tab_max+1)];
integer obj_ptr;/* objects counter */
integer pdf_last_pages;/* pointer to most recently generated pages object */
integer pdf_last_page;/* pointer to most recently generated page object */
integer pdf_last_stream;/* pointer to most recently generated stream */
integer pdf_stream_length;/* length of most recently generated stream */
integer pdf_stream_length_offset; /* file offset of the last stream length */
integer ff; /* for use with |set_ff| */

/* module 670 */
void
pdfxref_initialize (void) {
  integer k;
  obj_ptr = 0;
  for (k = 1; k <= head_tab_max; k++)
    head_tab[k] = 0;
}


/* module 671 */

/* Here we implement subroutines for work with objects and related things.
 * Some of them are used in former parts too, so we need to declare them
 * forward.
 */
void
append_dest_name (str_number s, integer n) {
  if (pdf_dest_names_ptr == dest_names_size)
    overflow ("number of destination names", dest_names_size);
  dest_names[pdf_dest_names_ptr].objname = s;
  dest_names[pdf_dest_names_ptr].objnum = n;
  incr (pdf_dest_names_ptr);
};


void
pdf_create_obj (integer t, integer i) {
  /* create an object with type |t| and identifier |i| */
  integer p, q;
  if (obj_ptr == obj_tab_size)
    overflow ("indirect objects table size", obj_tab_size);
  incr (obj_ptr);
  obj_info (obj_ptr) = i;
  obj_offset (obj_ptr) = 0;
  obj_aux (obj_ptr) = 0;
  if (t == obj_type_page) {
	p = head_tab[t];
	/* find the right poition to insert newly created object */
	q = p ; /*TH: please -O2 -Wall*/
	if ((p == 0) || (obj_info (p) < i)) {
	  obj_link (obj_ptr) = p;
	  head_tab[t] = obj_ptr;
	} else {
	  while (p != 0) {
		if (obj_info (p) < i)
		  goto DONE;
		q = p;
		p = obj_link (p);
	  };
	DONE:
	  obj_link (q) = obj_ptr;
	  obj_link (obj_ptr) = p;
	};
  } else {
	if (t != obj_type_others) {
	  obj_link (obj_ptr) = head_tab[t];
	  head_tab[t] = obj_ptr;
	  if ((t == obj_type_dest) && (i < 0))
	    append_dest_name (-obj_info (obj_ptr), obj_ptr);
	};
  };
};

integer
pdf_new_objnum (void) {
  /* create a new object and return its number */
  pdf_create_obj (obj_type_others, 0);
  return obj_ptr;
};

void
pdf_begin_obj (integer i) { /* begin a PDF object */
  ensure_pdf_open();
  check_and_set_pdfoptionpdfminorversion();
  obj_offset (i) = pdf_offset;
  pdf_print_int (i);
  pdf_print_ln_string(" 0 obj");
};

void
pdf_end_obj (void) {
  pdf_print_ln_string("endobj");	/* end a PDF object */
};

void
pdf_begin_dict (integer i) { /* begin a PDF dictionary object */
  obj_offset (i) = pdf_offset;
  pdf_print_int (i);
  pdf_print_ln_string(" 0 obj <<");
};

void
pdf_end_dict (void) {/* end a PDF object of type dictionary */
  pdf_print_ln_string(">> endobj");
};

void
pdf_new_obj (integer t, integer i) { /* begin to a new object */
  pdf_create_obj (t, i);
  pdf_begin_obj (obj_ptr);
};

void
pdf_new_dict (integer t, integer i) {	/* begin a new object with type dictionary */
  pdf_create_obj (t, i);
  pdf_begin_dict (obj_ptr);
};


pointer
append_ptr (pointer p, integer i) {
  /* appends a pointer with info |i| to the end of linked list with head |p| */
  pointer q;
  pointer retval;
  retval = p;
  fast_get_avail (q);
  info (q) = i;
  link (q) = null;
  if (p == null) {
    return q;
  };
  while (link (p) != null)
    p = link (p);
  link (p) = q;
  return retval;
};


pointer
pdf_lookup_list (pointer p, integer i) {  /* looks up for pointer with info |i| in list |p| */
  while (p != null) {
	if (info (p) == i) {
	  return p;
	};
	p = link (p);
  };
  return null;
};

/* ProcSet's handling. */
/* module 673 */
integer pdf_image_procset;/* collection of image types used in current  page/form */
boolean pdf_text_procset;/* mask of used ProcSet's in the current page/form */

/* module 674 */

/* Subroutines to print out various PDF objects
 */
void 
pdf_print_fw_int (integer n, integer w) {
  /* print out an integer with fixed width; used for outputting cross-reference table */
  unsigned char k;
  k = 0;
  do {
	dig[k] = n % 10;
	n = n / 10;
	incr (k);
  } while (k != w);
  pdf_room (k);
  while (k > 0) {
	decr (k);
	pdf_quick_out ('0' + dig[k]);
  };
};
	
void 
pdf_int_entry (char *s, integer v) {
  /* print out an entry in dictionary with integer value to PDF buffer */
  pdf_out ('/');
  pdf_print_string (s);
  pdf_out (' ');
  pdf_print_int (v);
};

void 
pdf_int_entry_ln (char *s, integer v) {
  pdf_int_entry (s, v);
  pdf_print_nl;
};

void 
pdf_indirect (str_number s, integer o) {
  /* print out an indirect entry in dictionary */
  pdf_out ('/');
  pdf_print (s);
  pdf_out (' ');
  pdf_print_int (o);
  pdf_print_string(" 0 R");
};

void 
pdf_indirect_ln (str_number s, integer o) {
  pdf_indirect (s, o);
  pdf_print_nl;
};

void 
pdf_indirect_string (char *s, integer o) {
  /* print out an indirect entry in dictionary */
  pdf_out ('/');
  pdf_print_string (s);
  pdf_out (' ');
  pdf_print_int (o);
  pdf_print_string(" 0 R");
};

void 
pdf_indirect_string_ln (char *s, integer o) {
  pdf_indirect_string (s, o);
  pdf_print_nl;
};

void 
pdf_print_str (str_number s) {
  /* print out |s| as string in PDF output */
  pdf_out ('(');
  pdf_print (s);
  pdf_out (')');
};

void 
pdf_print_str_string (char *s) { 
  /* print out |s| as string in PDF  output */
  pdf_out ('(');
  pdf_print_string (s);
  pdf_out (')');
};

void 
pdf_print_str_ln (str_number s) {
  /* print out |s| as string in PDF  output */
  pdf_print_str (s);
  pdf_print_nl;
};

void 
pdf_str_entry (char *s, str_number v) {			
  /* print out an entry in dictionary with string value to PDF buffer */
  if (v == 0)
	return;
  pdf_out ('/');
  pdf_print_string (s);
  pdf_out (' ');
  pdf_print_str (v);
};

void 
pdf_str_entry_ln (char  *s, char *v) {
  if (strlen(v) == 0)
	return;
  pdf_str_entry (s, slow_make_tex_string(v));
  pdf_print_nl;
};

