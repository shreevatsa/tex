
#include "types.h"
#include "c-compat.h"


#include "globals.h"


#include "mainio.h"

#include <errno.h>

/* Full source file name. */
extern char *full_name_of_file;
/* The C version of the jobname, if given. */
extern const_string job_name;


/* module 26 */

/* Most of what we need to do with respect to input and output can be handled
 * by the I/O facilities that are standard in \PASCAL, i.e., the routines
 * called |get|, |put|, |eof|, and so on. But
 * standard \PASCAL\ does not allow file variables to be associated with file
 * names that are determined at run time, so it cannot be used to implement
 * \TeX; some sort of extension to \PASCAL's ordinary |reset| and |rewrite|
 * is crucial for our purposes. We shall assume that |name_of_file| is a variable
 * of an appropriate type such that the \PASCAL\ run-time system being used to
 * implement \TeX\ can open a file whose external name is specified by
 * |name_of_file|.
 */

text_char  *name_of_file;
unsigned int name_length; /* this many characters are actually relevant in 
							 |name_of_file| (the rest are blank) */
/* module 29 */

/* Binary input and output are done with \PASCAL's ordinary |get| and |put|
 * procedures, so we don't have to make any other special arrangements for
 * binary~I/O. Text output is also easy to do with standard \PASCAL\ routines.
 * The treatment of text input is more difficult, however, because
 * of the necessary translation to |ASCII_code| values.
 * \TeX's conventions should be efficient, and they should
 * blend nicely with the user's operating environment.
 */

/* module 30 */

/* Input from text files is read one line at a time, using a routine called
 * |input_ln|. This function is defined in terms of global variables called
 * |buffer|, |first|, and |last| that will be described in detail later; for
 * now, it suffices for us to know that |buffer| is an array of |ASCII_code|
 * values, and that |first| and |last| are indices into this array
 * representing the beginning and ending of a line of text.
 */

ASCII_code *buffer; /* lines of characters being read */ 
unsigned int first; /* the first unused position in |buffer| */ 
unsigned int last; /* end of the line just input to |buffer| */ 
integer max_buf_stack; /* largest index used in |buffer| */

void
realloc_buffer (void) {
  buffer = xrealloc_array(buffer,sizeof(ASCII_code)*2*buf_size);
  if (buffer == NULL) {
    overflow ("buffer size", buf_size);
  } else {
    buf_size = 2*buf_size;
  }
}

/* module 32 */

/* The user's terminal acts essentially like other files of text, except
 * that it is used both for input and for output. When the terminal is
 * considered an input file, the file variable is called |term_in|, and when it
 * is considered an output file the file variable is |term_out|.
 */

FILE *log_file; /* transcript of \TeX\ session */ 



/* module 31 */

/* The |input_ln| function brings the next line of input from the specified
 * file into available positions of the buffer array and returns the value
 * |true|, unless the file has already been entirely read, in which case it
 * returns |false| and sets |last:=first|. In general, the |ASCII_code|
 * numbers that represent the next line of the file are input into
 * |buffer[first]|, |buffer[first+1]|, \dots, |buffer[last-1]|; and the
 * global variable |last| is set equal to |first| plus the length of the
 * line. Trailing blanks are removed from the line; thus, either |last=first|
 * (in which case the line was entirely blank) or |buffer[last-1]<>" "|.
 * 
 * An overflow error is given, however, if the normal actions of |input_ln|
 * would make |last>=buf_size|; this is done so that other parts of \TeX\
 * can safely look at the contents of |buffer[last+1]| without overstepping
 * the bounds of the |buffer| array. Upon entry to |input_ln|, the condition
 * |first<buf_size| will always hold, so that there is always room for an
 * ``empty'' line.
 * 
 * The variable |max_buf_stack|, which is used to keep track of how large
 * the |buf_size| parameter must be to accommodate the present job, is
 * also kept up to date by |input_ln|.
 * 
 * If the |bypass_eoln| parameter is |true|, |input_ln| will do a |get|
 * before looking at the first character of the line; this skips over
 * an |eoln| that was in |f^|. The procedure does not do a |get| when it
 * reaches the end of the line; therefore it can be used to acquire input
 * from the user's terminal as well as from ordinary text files.
 * 
 * Standard \PASCAL\ says that a file should have |eoln| immediately
 * before |eof|, but \TeX\ needs only a weaker restriction: If |eof|
 * occurs in the middle of a line, the system function |eoln| should return
 * a |true| result (even though |f^| will be undefined).
 * 
 * Since the inner loop of |input_ln| is part of \TeX's ``inner loop''---each
 * character of input comes in at this place---it is wise to reduce system
 * overhead by making use of special routines that read in an entire array
 * of characters at once, if such routines are available. The following
 * code uses standard \PASCAL\ to illustrate what needs to be done, but
 * finer tuning is often possible at well-developed \PASCAL\ sites.
 * 
 */

/* Read a line of input as efficiently as possible while still looking
   like Pascal.  We set `last' to `first' and return `false' if we get
   to eof.  Otherwise, we return `true' and set last = first +
   length(line except trailing whitespace).  */

#define ffs_getc getc
#define ffs_ungetc ungetc
#define ffs_errno() errno

boolean
input_line (FILE * f) {
  int i;
  i = 0; /* TH -Wall */
  /* Recognize either LF or CR as a line terminator.  */
  last = first;
 REDO:
  while (last < (unsigned)buf_size && (i = ffs_getc (f)) != EOF && i != '\n' && i != '\r')
    buffer[last++] = i;
  if (i == EOF && ffs_errno() != EINTR && last == first)
    return false;
  /* We didn't get the whole line because our buffer was too small.  */
  if (i != EOF && i != '\n' && i != '\r') {
    buffer = xrealloc_array(buffer,sizeof(ASCII_code)*2*buf_size);
    if(buffer == NULL) {
      fprintf (stderr, "! Unable to read an entire line---bufsize=%u.\n",(unsigned) buf_size);
      fputs ("Please increase buf_size in texmf.cnf.\n", stderr);
      uexit (1);
    } else {
      buf_size = 2*buf_size;
      goto REDO;
    }
  }
  buffer[last] = ' ';
  if (last >= (unsigned)max_buf_stack)
    max_buf_stack = last;
  /* If next char is LF of a CRLF, read it.  */
  if (i == '\r') {
    while ((i = ffs_getc (f)) == EOF && ffs_errno() == EINTR)
      ;
    if (i != '\n')
      ffs_ungetc (i, f);
  }
  /* Trim trailing whitespace.  */
  while (last > first && ISBLANK (buffer[last - 1]))
    --last;
  for (i = first; i <= (int)last; i++)
     buffer[i] = Xord(buffer[i]);
  return true;
}


/* module 37 */

/* The following program does the required initialization.
 * Iff anything has been specified on the command line, then |t_open_in|
 * will return with |last > first|.
 */
boolean
init_terminal (void) {	/* gets the terminal input started */
  topenin();
  if (last > first) {
	loc = first;
	while (loc < (int)last && buffer[loc] == ' ')
	  incr (loc);
	if (loc < (int)last)
	  return true;
  };
  loop {
    wake_up_terminal;
    fprintf (term_out, "%s", "**");
    update_terminal;
    if (!input_ln (term_in, true))	{ /* this shouldn't happen */
	  fprintf (term_out,"\n%s", "! End of file on the terminal... why?");
	  return false;
	};
    loc = first;
    while ((loc < (int)last) && (buffer[loc] == ' '))
      incr (loc);
    if (loc < (int)last)
      return true;		/* return unless the line was all blank */
    fprintf (term_out, "%s\n","Please type the name of your input file.");
  };
};

/* module 71 */

/* Here is a procedure that asks the user to type a line of input,
 * assuming that the |selector| setting is either |term_only| or |term_and_log|.
 * The input is placed into locations |first| through |last-1| of the
 * |buffer| array, and echoed on the transcript file if appropriate.
 * 
 * This procedure is never called when |interaction<scroll_mode|.
 */
void
term_input (void) { /* gets a line from the terminal */
  unsigned int k;		/* index into |buffer| */
  update_terminal;  /* now the user sees the prompt for sure */
  if (!input_ln (term_in, true))
    fatal_error ("End of file on the terminal!");
  term_offset = 0; /* the user's line ended with \<\rm return> */
  decr (selector); /* prepare to echo the input */
  if (last != first) {
	for (k = first; k <= last - 1; k++)
	  zprint (buffer[k]);
  }
  print_ln();
  incr (selector); /* restore previous status */
};

/* module 304 */
str_number *source_filename_stack;
str_number *full_source_filename_stack;

/* module 328 */

/* The |begin_file_reading| procedure starts a new level of input for lines
 * of characters to be read from a file, or as an insertion from the
 * terminal. It does not take care of opening the file, nor does it set |loc|
 * or |limit| or |line|.
 */
void
begin_file_reading (void) {
  if (in_open == (unsigned)max_in_open)
    overflow ("text input levels", max_in_open);
  if (first == (unsigned)buf_size) {
    realloc_buffer();
  }
  incr (in_open);
  push_input;
  index = in_open;
  eof_seen[index] = false;
  grp_stack[index] = cur_boundary;
  if_stack[index] = cond_ptr;
  line_stack[index] = line;
  start = first;
  state = mid_line;
  name = 0; /* |terminal_input| is now |true| */ 
};


/* module 329 */

/* Conversely, the variables must be downdated when such a level of input
 * is finished:
 */
void
end_file_reading (void) {
  first = start;
  line = line_stack[index];
  if ((name == 18) || (name == 19)) {
	pseudo_close();
  } else {
	if (name > 17)
	  a_close (cur_file);
  }  /* forget it */
  pop_input;
  decr (in_open);
};

/* module 511 */

/* 
 * It's time now to fret about file names. Besides the fact that different
 * operating systems treat files in different ways, we must cope with the
 * fact that completely different naming conventions are used by different
 * groups of people. The following programs show what is required for one
 * particular operating system; similar routines for other systems are not
 * difficult to devise.
 * 
 * \TeX\ assumes that a file name has three parts: the name proper; its
 * ``extension''; and a ``file area'' where it is found in an external file
 * system. The extension of an input file or a write file is assumed to be
 * `\.{.tex}' unless otherwise specified; it is `\.{.log}' on the
 * transcript file that records each run of \TeX; it is `\.{.tfm}' on the font
 * metric files that describe characters in the fonts \TeX\ uses; it is
 * `\.{.dvi}' on the output files that specify typesetting information; and it
 * is `\.{.fmt}' on the format files written by \.{INITEX} to initialize \TeX.
 * The file area can be arbitrary on input files, but files are usually
 * output to the user's current area. If an input file cannot be
 * found on the specified area, \TeX\ will look for it on a special system
 * area; this special area is intended for commonly used input files like
 * \.{webmac.tex}.
 * 
 * Simple uses of \TeX\ refer only to file names that have no explicit
 * extension or area. For example, a person usually says `\.{\\input} \.{paper}'
 * or `\.{\\font\\tenrm} \.= \.{helvetica}' instead of `\.{\\input}
 * \.{paper.new}' or `\.{\\font\\tenrm} \.= \.{<csd.knuth>test}'. Simple file
 * names are best, because they make the \TeX\ source files portable;
 * whenever a file name consists entirely of letters and digits, it should be
 * treated in the same way by all implementations of \TeX. However, users
 * need the ability to refer to other files in their environment, especially
 * when responding to error messages concerning unopenable files; therefore
 * we want to let them use the syntax that appears in their favorite
 * operating system.
 * 
 * The following procedures don't allow spaces to be part of
 * file names; but some users seem to like names that are spaced-out.
 * System-dependent changes to allow such things should probably
 * be made with reluctance, and only when an entire file name that
 * includes spaces is ``quoted'' somehow.
 */

/* module 512 */

/* In order to isolate the system-dependent aspects of file names, the
 * 
 * system-independent parts of \TeX\ are expressed in terms
 * of three system-dependent
 * procedures called |begin_name|, |more_name|, and |end_name|. In
 * essence, if the user-specified characters of the file name are $c_1\ldots c_n$,
 * the system-independent driver program does the operations
 * $$|begin_name|;\,|more_name|(c_1);\,\ldots\,;|more_name|(c_n);
 * \,|end_name|.$$
 * These three procedures communicate with each other via global variables.
 * Afterwards the file name will appear in the string pool as three strings
 * called |cur_name|\penalty10000\hskip-.05em,
 * |cur_area|, and |cur_ext|; the latter two are null (i.e.,
 * |""|), unless they were explicitly specified by the user.
 * 
 * Actually the situation is slightly more complicated, because \TeX\ needs
 * to know when the file name ends. The |more_name| routine is a function
 * (with side effects) that returns |true| on the calls |more_name|$(c_1)$,
 * \dots, |more_name|$(c_{n-1})$. The final call |more_name|$(c_n)$
 * returns |false|; or, it returns |true| and the token following $c_n$ is
 * something like `\.{\\hbox}' (i.e., not a character). In other words,
 * |more_name| is supposed to return |true| unless it is sure that the
 * file name has been completely scanned; and |end_name| is supposed to be able
 * to finish the assembly of |cur_name|, |cur_area|, and |cur_ext| regardless of
 * whether $|more_name|(c_n)$ returned |true| or |false|.
 */
str_number cur_name; /* name of file just scanned */ 
str_number cur_area; /* file area just scanned, or \.{""} */ 
str_number cur_ext; /* file extension just scanned, or \.{""} */

/* module 513 */

/* The file names we shall deal with have the
 * following structure: If the name contains `\./' or `\.:'
 * (for Amiga only), the file area
 * consists of all characters up to and including the final such character;
 * otherwise the file area is null. If the remaining file name contains
 * `\..', the file extension consists of all such characters from the last
 * `\..' to the end, otherwise the file extension is null.
 * 
 * We can scan such file names easily by using two global variables that keep track
 * of the occurrences of area and extension delimiters:
 */

pool_pointer area_delimiter; /* the most recent `\./', if any */ 
pool_pointer ext_delimiter; /* the most recent `\..', if any */

/* module 520 */

/* A messier routine is also needed, since format file names must be scanned
 * before \TeX's string mechanism has been initialized. We shall use the
 * global variable |TEX_format_default| to supply the text for default system areas
 * and extensions related to format files.
 * 
 * Under {\mc UNIX} we don't give the area part, instead depending
 * on the path searching that will happen during file opening. Also, the
 * length will be set in the main program.
 */

integer format_default_length;
char *TEX_format_default;

/* module 521 */

/* We set the name of the default format file and the length of that name
 * in C, instead of Pascal, since we want them to depend on the name of the
 * program.
 */

/* module 527 */

/* The global variable |name_in_progress| is used to prevent recursive
 * use of |scan_file_name|, since the |begin_name| and other procedures
 * communicate via global variables. Recursion would arise only by
 * devious tricks like `\.{\\input\\input f}'; such attempts at sabotage
 * must be thwarted. Furthermore, |name_in_progress| prevents \.{\\input}
 * 
 * from being initiated when a font size specification is being scanned.
 * 
 * Another global variable, |jobname|, contains the file name that was first
 * \.{\\input} by the user. This name is extended by `\.{.log}' and `\.{.dvi}'
 * and `\.{.fmt}' in the names of \TeX's output files.
 */

boolean name_in_progress; /* is a file name being scanned? */ 
str_number jobname; /* principal file name */ 
boolean log_opened; /* has the transcript file been opened? */

/* module 514 */

/* Input files that can't be found in the user's area may appear in a standard
 * system area called |TEX_area|. Font metric files whose areas are not given
 * explicitly are assumed to appear in a standard system area called
 * |TEX_font_area|. These system area names will, of course, vary from place
 * to place.
 * 
 * In C, the default paths are specified separately.
 */

/* module 515 */

/* Here now is the first of the system-dependent routines for file name scanning.
 */
void
begin_name (void) {
  area_delimiter = 0;
  ext_delimiter = 0;
};


/* module 516 */

/* And here's the second. The string pool might change as the file name is
 * being scanned, since a new \.{\\csname} might be entered; therefore we keep
 * |area_delimiter| and |ext_delimiter| relative to the beginning of the current
 * string, instead of assigning an absolute address like |pool_ptr| to them.
 */
boolean
more_name (ASCII_code c) {
  /*  if ((stop_at_space && (c == ' ')) || c == end_line_char) {*/
  if (stop_at_space && (c == ' ')) {
    return false;
  } else {
	str_room (1);
	append_char (c);	/* contribute |c| to the current string */ 
	if (IS_DIR_SEP (c)) {
	  area_delimiter = cur_length;
	  ext_delimiter = 0;
	} else if (c == '.') {
	  ext_delimiter = cur_length;
	}
	return true;
  };
};

/* module 517 */

/* The third.
 * 
 * If a string is already in the string pool, the function
 * |slow_make_string| does not create a new string but returns this string
 * number, thus saving string space. Because of this new property of the
 * returned string number it is not possible to apply |flush_string| to
 * these strings.
 */
void
end_name (void) {
  /*str_number temp_str; *//* result of file name cache lookups */ 
  /*pool_pointer j; *//* running index */ 
  strings_room(3);
  if (area_delimiter == 0) {
    cur_area = null_string;
  } else {
	cur_area = str_ptr;
	str_start[str_ptr + 1] = str_start[str_ptr] + area_delimiter;
	incr (str_ptr);
	/* temp_str = search_string (cur_area);
	   if (temp_str > 0) {
	   cur_area = temp_str;
	   decr (str_ptr); */
	   /* no |flush_string|, |pool_ptr| will be wrong! */ 
	/* for (j =str_start[str_ptr+1]; j <= pool_ptr - 1; j++) {
	   str_pool[j - area_delimiter] = str_pool[j];
	   };
	   pool_ptr = pool_ptr - area_delimiter;
	   }; */
  };
  if (ext_delimiter == 0) {
	cur_ext = null_string;
	/*cur_name = slow_make_string();*/
	cur_name = make_string();
  } else {
	cur_name = str_ptr;
	str_start[str_ptr + 1] = str_start[str_ptr] + ext_delimiter - area_delimiter - 1;
	incr (str_ptr);
	cur_ext = make_string();
	decr (str_ptr); /* undo extension string to look at name part */ 
	/*temp_str = search_string (cur_name);
	if (temp_str > 0) {
	  cur_name = temp_str;
	  decr (str_ptr);
	  for (j =str_start[str_ptr+ 1];j<= pool_ptr - 1; j++) {
		str_pool[j - ext_delimiter + area_delimiter + 1] = str_pool[j];
	  };
	  pool_ptr = pool_ptr - ext_delimiter + area_delimiter + 1;
	};*/
	cur_ext = make_string();
	/* cur_ext = slow_make_string(); */ 
  };
};


/* module 519 */

/* Another system-dependent routine is needed to convert three internal
 * \TeX\ strings
 * into the |name_of_file| value that is used to open files. The present code
 * allows both lowercase and uppercase letters in the file name.
 */
void
pack_file_name (str_number n, str_number a, str_number e) {
  integer k; /* number of positions filled in |name_of_file| */ 
  ASCII_code c; /* character being packed */ 
  pool_pointer j; /* index into |str_pool| */ 
  k = 0;
  if (name_of_file)
    libc_free (name_of_file);
  name_of_file = xmalloc_array (ASCII_code, length (a) + length (n) + length (e) + 1);
  for (j = str_start[a]; j <= str_start[a + 1] - 1; j++)
    append_to_name (str_pool[j]);
  for (j = str_start[n]; j <= str_start[n + 1] - 1; j++)
    append_to_name (str_pool[j]);
  for (j = str_start[e]; j <= str_start[e + 1] - 1; j++)
    append_to_name (str_pool[j]);
  if (k < file_name_size) { /* it had beter be smaller, file_name_size = maxint */
    name_length = k;
  } else {
    name_length = file_name_size;
  }
  name_of_file[name_length + 1] = 0;
};


/* module 523 */

/* Here is the messy routine that was just mentioned. It sets |name_of_file|
 * from the first |n| characters of |TEX_format_default|, followed by
 * |buffer[a..b]|, followed by the last |format_ext_length| characters of
 * |TEX_format_default|.
 * 
 * We dare not give error messages here, since \TeX\ calls this routine before
 * the |error| routine is ready to roll. Instead, we simply drop excess characters,
 * since the error will be detected in another way when a strange file name
 * isn't found.
 */
void
pack_buffered_name (small_number n, int a, int b) {
  int k; /* number of positions filled in |name_of_file| */ 
  ASCII_code c; /* character being packed */ 
  int j; /* index into |buffer| or |TEX_format_default| */ 
  if (n + b - a + 1 + format_ext_length > file_name_size)
    b = a + file_name_size - n - 1 - format_ext_length;
  k = 0;
  if (name_of_file)
    libc_free (name_of_file);
  name_of_file = xmalloc_array (ASCII_code, n + (b - a + 1) + format_ext_length + 1);
  for (j = 1; j <= n; j++)
    append_to_name (Xord((integer)TEX_format_default[j]));
  for (j = a; j <= b; j++)
    append_to_name (buffer[j]);
  for (j = format_default_length - format_ext_length + 1; j <= format_default_length; j++)
    append_to_name (Xord((integer)TEX_format_default[j]));
  if (k < file_name_size) {
    name_length = k;
  } else {
    name_length = file_name_size;
  };
  name_of_file[name_length + 1] = 0;
};


/* module 525 */

/* Operating systems often make it possible to determine the exact name (and
 * possible version number) of a file that has been opened. The following routine,
 * which simply makes a \TeX\ string from the value of |name_of_file|, should
 * ideally be changed to deduce the full name of file~|f|, which is the file
 * most recently opened, if it is possible to do this in a \PASCAL\ program.
 * 
 * This routine might be called after string memory has overflowed, hence
 * we dare not use `|str_room|'.
 * 
 */
str_number
make_name_string (void) {
  unsigned int k; /* index into |name_of_file| */ 
  if ((pool_ptr + name_length > (unsigned)pool_size) || (str_ptr == max_strings) || (cur_length > 0)) {
	return '?';
  } else {
	for (k = 1; k <= name_length; k++)
	  append_char (Xord(name_of_file[k]));
	return make_string();
  };
}

str_number
a_make_name_string () {
  return make_name_string();
};

str_number
b_make_name_string () {
  return make_name_string();
};

str_number
w_make_name_string () {
  return make_name_string();
};


/* module 526 */

/* Now let's consider the ``driver''
 * routines by which \TeX\ deals with file names
 * in a system-independent manner. First comes a procedure that looks for a
 * file name in the input by calling |get_x_token| for the information.
 */
void
scan_file_name (void) {
  name_in_progress = true;
  begin_name();
  get_nblank_ncall;   /* Get the next non-blank non-call... */
  loop {
    if ((cur_cmd > other_char) || (cur_chr > 255)) {	/* not a character */
	  back_input();
	  goto DONE;
	};
    if (!more_name (cur_chr))
      goto DONE;
    get_x_token();
  };
 DONE:
  end_name();
  name_in_progress = false;
};


/* module 529 */

/* Here is a routine that manufactures the output file names, assuming that
 * |job_name<>0|. It ignores and changes the current settings of |cur_area|
 * and |cur_ext|.
 */
void
pack_job_name (char *s) { /* |s = ".log"|, |".dvi"|, or |format_extension| */
  cur_area = null_string;
  cur_ext = slow_make_tex_string(s);
  cur_name = jobname;
  pack_cur_name;
};

str_number
make_full_name_string() {
  return maketexstring(full_name_of_file);
}

str_number
getjobname() {
  str_number ret = cur_name;
  if (job_name != NULL)
	ret = maketexstring(job_name);
  return ret;
}


/* module 530 */

/* If some trouble arises when \TeX\ tries to open a file, the following
 * routine calls upon the user to supply another file name. Parameter~|s|
 * is used in the error message to identify the type of file; parameter~|e|
 * is the default extension if none is given. Upon exit from the routine,
 * variables |cur_name|, |cur_area|, |cur_ext|, and |name_of_file| are
 * ready for another attempt at file opening.
 */
void
prompt_file_name (char *ss,char *es) {
  str_number s,e;
  unsigned int k; /* index into |buffer| */ 
  s = slow_make_tex_string(ss);
  e = slow_make_tex_string(es);
  if (interaction == scroll_mode)
    wake_up_terminal;
  if (s == slow_make_tex_string("input file name")) {
    print_err ("I can't find file `");
  } else {
    print_err ("I can't write on file `");}
  print_file_name (cur_name, cur_area, cur_ext);
  zprint_string("'.");
  if ((e == slow_make_tex_string(".tex")) || (e == null_string))
    show_context();
  print_nl_string("Please type another ");
  zprint (s);
  if (interaction < scroll_mode)
    fatal_error ("*** (job aborted, file error in nonstop mode)");
  clear_terminal;
  prompt_input (": ");
  /* begin expansion of Scan file name in the buffer */
  /* module 531 */
  begin_name();
  k = first;
  while ((buffer[k] == ' ') && (k < last))
	incr (k);
  loop {
	if (k == last)
	  goto DONE;
	if (!more_name (buffer[k]))
	  goto DONE;
	incr (k);
  };
 DONE:
  end_name();
  /* end expansion of Scan file name in the buffer */
  if (cur_ext == null_string)
    cur_ext = e;
  pack_cur_name;
}

/* module 532 */

/* Here's an example of how these conventions are used. Whenever it is time to
 * ship out a box of stuff, we shall use the macro |ensure_dvi_open|.
 */

FILE *dvi_file; /* the device-independent output goes here */ 
str_number output_file_name; /* full name of the output file */ 
str_number log_name; /* full name of the log file */

/* module 534 */

/* The |open_log_file| routine is used to open the transcript file and to help
 * it catch up to what has previously been printed on the terminal.
 */
void
open_log_file (void) {
  unsigned char old_setting; /* previous |selector| setting */ 
  unsigned int k; /* index into |months| and |buffer| */ 
  unsigned int l; /* end of first input line */ 
  char *months;
  old_setting = selector;
  if (jobname == 0)
    jobname = slow_make_tex_string("texput");
  pack_job_name (".log");
  while (!a_open_out (log_file)) {
	/* begin expansion of Try to get a different log file name */
	/* module 535 */
	/* Sometimes |open_log_file| is called at awkward moments when \TeX\ is
	 * unable to zprint error messages or even to |show_context|.
	 * The |prompt_file_name| routine can result in a |fatal_error|, but the |error|
	 * routine will not be invoked because |log_opened| will be false.
	 * 
	 * The normal idea of |batch_mode| is that nothing at all should be written
	 * on the terminal. However, in the unusual case that
	 * no log file could be opened, we make an exception and allow
	 * an explanatory message to be seen.
	 * 
	 * Incidentally, the program always refers to the log file as a `\.{transcript
	 * file}', because some systems cannot use the extension `\.{.log}' for
	 * this file.
	 */
	selector = term_only;
	prompt_file_name ("transcript file name",".log");
	/* end expansion of Try to get a different log file name */
  }
  log_name = a_make_name_string (log_file);
  selector = log_only;
  log_opened = true;
  /* begin expansion of Print the banner line, including the date and time */
  /* module 536 */
  fprintf ( log_file , "%s%c%s%c%s",banner);
  wlog_string (version_string);
  slow_print (format_ident);
  zprint_string("  ");
  print_int (day);
  print_char (' ');
  months = " JANFEBMARAPRMAYJUNJULAUGSEPOCTNOVDEC";
  for (k = 3 * month - 2; k <=(unsigned)3 * month; k++)
	wlog_char (months[k]);
  print_char (' ');
  print_int (year);
  print_char (' ');
  print_two (tex_time / 60);
  print_char (':');
  print_two (tex_time % 60);
  /* end expansion of Print the banner line, including the date and time */
  if (mltex_enabled_p) {
	wlog_cr;
	wlog_string ("MLTeX v2.2 enabled");
  };
  input_stack[input_ptr] = cur_input;
  /* make sure bottom level is in memory */ 
  print_nl_string("**");
  l = input_stack[0].limit_field; /* last position of first line */ 
  if (buffer[l] == end_line_char)
    decr (l);
  for (k = 1; k <= l; k++)
    zprint (buffer[k]);
  print_ln();
  /* now the transcript file contains the first line of input */ 
  selector = old_setting + 2; /* |log_only| or |term_and_log| */ 
}

/* module 658 */

/* Checks that we have a name for the generated pdf file and that it's open.
 * |check_and_set_pdfoptionpdfminorversion| should be called directly
 * hereafter.
 */
void 
ensure_pdf_open (void) {
  if (output_file_name != 0)
	return;
  if (jobname == 0)
	open_log_file();
  pack_job_name (".pdf");
  while (!b_open_out (pdf_file))
	prompt_file_name ("file name for output",".pdf");
  output_file_name = b_make_name_string (pdf_file);
}


/* module 1420 */
void 
open_or_close_in (void) {
  unsigned char c; /* 1 for \.{\\openin}, 0 for \.{\\closein} */ 
  unsigned char n; /* stream number */ 
  int k;
  c = cur_chr;
  scan_four_bit_int();
  n = cur_val;
  if (read_open[n] != closed) {
    a_close (read_file[n]);
    read_open[n] = closed;
  };
  if (c != 0) {
    scan_optional_equals();
    scan_file_name();
    pack_cur_name;
    tex_input_type = 0; /* Tell |open_input| we are \.{\\openin}. */
    if (open_in_name_ok (stringcast(name_of_file+1)) 
	&& a_open_in (read_file[n], kpse_tex_format)) {
      k = 1;
      name_in_progress = true;
      begin_name();
      stop_at_space = false;
      while ((k <= (int)name_length) && (more_name (name_of_file[k])))
	incr (k);
      stop_at_space = true;
      end_name();
      name_in_progress = false;
      read_open[n] = just_open;
    };
  };
}

/* module 1478 */

/* Here we do whatever is needed to complete \TeX's job gracefully on the
 * local operating system. The code here might come into play after a fatal
 * error; it must therefore consist entirely of ``safe'' operations that
 * cannot produce error messages. For example, it would be a mistake to call
 * |str_room| or |make_string| at this time, because a call on |overflow|
 * might lead to an infinite loop.
 * 
 * Actually there's one way to get error messages, via |prepare_mag|;
 * but that can't cause infinite recursion.
 * 
 * This program doesn't bother to close the input files that may still be open.
 */
void 
close_files_and_terminate (void) {
  int k;
  /* begin expansion of Finish the extensions */
  /* module 1574 */
  for (k = 0; k <= 15; k++)
	if (write_open[k])
	  a_close (write_file[k]);
  /* end expansion of Finish the extensions */
  if (tracing_stats > 0)
	/* begin expansion of Output statistics about this job */
	/* module 1479 */	
	/* The present section goes directly to the log file instead of using
	 * |zprint| commands, because there's no need for these strings to take
	 * up |str_pool| memory when a non-{\bf stat} version of \TeX\ is being used.
	 */
	if (log_opened) {
	  fprintf ( log_file , "%c\n",  ' ' ) ;
	  fprintf ( log_file , "%s%s\n",  "Here is how much of TeX's memory" , " you used:" ) ;
	  fprintf ( log_file , "%c%ld%s",  ' ' , (long)str_ptr - init_str_ptr , " string" ) ;
	  if ( str_ptr != init_str_ptr + 1 )
		putc ( 's' ,  log_file );
	  fprintf ( log_file , "%s%ld\n",  " out of " , (long)max_strings - init_str_ptr ) ;
	  fprintf ( log_file , "%c%ld%s%ld\n",  ' ' , (long)pool_ptr - init_pool_ptr ,     " string characters out of " , 
				(long)pool_size - init_pool_ptr ) ;
	  fprintf ( log_file , "%c%ld%s%ld\n",  ' ' , (long)lo_mem_max - mem_min + mem_end - hi_mem_min + 2 ,
				" words of memory out of " , (long)mem_end + 1 - mem_min ) ;
	  fprintf ( log_file , "%c%ld%s%ld\n",  ' ' , (long)cs_count ,     " multiletter control sequences out of " , 
				(long)HASH_SIZE) ;
	  fprintf ( log_file , "%c%ld%s%ld%s",  ' ' , (long)fmem_ptr , 
				" words of font info for " , (long)font_ptr - 0     ," font" ) ;
	  if ( font_ptr != 1 )
		putc ( 's' ,  log_file );
	  fprintf ( log_file , "%s%ld%s%ld\n",  ", out of " , (long)font_mem_size , " for " , (long)font_max - 0 ) ;
	  fprintf ( log_file , "%c%ld%s",  ' ' , (long)hyph_count , " hyphenation exception" ) ;
	  if ( hyph_count != 1 )
		putc ( 's' ,  log_file );
	  fprintf ( log_file , "%s%ld\n",  " out of " , (long)hyph_size ) ;
	  fprintf ( log_file , "%c%ld%s%ld%s%ld%s%ld%s%ld%s%ld%s%ld%s%ld%s%ld%s%ld%c\n",  ' ' , (long)max_in_stack , "i," , 
				(long)max_nest_stack , "n," ,     (long)max_param_stack , "p," , (long)max_buf_stack + 1 , "b," , 
				(long)max_save_stack + 6 , "s stack positions out of " , (long)stack_size , "i," , 
				(long)nest_size , "n," ,     (long)param_size , "p," , 
				(long)buf_size , "b," , (long)save_size , 's' ) ;
	  fprintf ( log_file , "%c%ld%s%ld\n",  ' ' , (long)obj_ptr , " PDF objects out of " , (long)obj_tab_size ) ;
	  fprintf ( log_file , "%c%ld%s%ld\n",  ' ' , (long)pdf_dest_names_ptr , 
				" named destinations out of "     , (long)dest_names_size ) ;
	  fprintf ( log_file , "%c%ld%s%ld\n",  ' ' , (long)pdf_mem_ptr , 
				" words of extra memory for PDF output out of ", (long)pdf_mem_size ) ;
	};
  /* end expansion of Output statistics about this job */
  wake_up_terminal;
  if (fixed_output > 0) {
	if (history == fatal_error_stop) {
	  print_err    (" ==> Fatal error occurred, the output PDF file is not finished!");
	} else {
	  /* Finish the PDF file */
      finish_pdf_file();
	};
  } else {
	/* Finish the \.{DVI} file */
	finish_dvi_file();
  };
  if (log_opened) {
	wlog_cr;
	a_close (log_file);
	selector = selector - 2;
	if (selector == term_only) {
	  print_nl_string("Transcript written on ");
	  slow_print (log_name);
	  print_char ('.');
	};
  };
  print_ln();
};

/* NEW, called from out_what() */

void
open_out_what (pointer p, small_number j) {
  cur_name = open_name (p);
  cur_area = open_area (p);
  cur_ext = open_ext (p);
  if (cur_ext == null_string)
	cur_ext = slow_make_tex_string(".tex");
  pack_cur_name;
  while (!open_out_name_ok (stringcast (name_of_file + 1))
		 || !a_open_out (write_file[j]))
	prompt_file_name ("output file name",".tex");
}


void 
close_out_what (FILE *j) {
  a_close (j);
}

/* module 485 */
/* The first line of a file must be treated specially, since |input_ln|
 * must be told not to start with |get|.
 */
void
read_first_line (small_number m) {
  if (input_ln (read_file[m], false)) {
	read_open[m] = normal;
  } else {
	a_close (read_file[m]);
	read_open[m] = closed;
  }
}

/* module 486 */
/* An empty line is appended at the end of a |read_file|.
 */
void
read_next_line (small_number m) {
  if (!input_ln (read_file[m], true)) {
	a_close (read_file[m]);
	read_open[m] = closed;
	if (align_state != 1000000) {
	  runaway();
	  print_err ("File ended within ");
	  print_esc_string ("read");
	  help1 ("This \\read has unbalanced braces.");
	  align_state = 1000000;
	  error();
	};
  };
}

/* module 537 */

/* Let's turn now to the procedure that is used to initiate file reading
 * when an `\.{\\input}' command is being processed.
 */
void
start_input (void) { /* \TeX\ will \.{\\input} something */
  str_number temp_str;
  int k;
  scan_file_name(); /* set |cur_name| to desired file name */ 
  pack_cur_name;
  loop {
    begin_file_reading(); /* set up |cur_file| and new level of input */ 
	tex_input_type = 1; /* Tell |open_input| we are \.{\\input}. */
	/* Kpathsea tries all the various ways to get the file. */
	if (open_in_name_ok (stringcast (name_of_file + 1)) && a_open_in (cur_file, kpse_tex_format)) {
      /* At this point |name_of_file| contains the actual name found. 
		 We extract the |cur_area|, |cur_name|, and |cur_ext| from it. */
	  k = 1;
	  name_in_progress = true;
	  begin_name();
	  stop_at_space = false;
	  while ((k <= (int)name_length) && (more_name (name_of_file[k])))
		incr (k);
	  stop_at_space = true;
	  end_name();
	  name_in_progress = false;
	  goto DONE;
	};
    end_file_reading();
	/* remove the level that didn't work */
    prompt_file_name ("input file name","");
  };
 DONE:
  name = a_make_name_string (cur_file);
  source_filename_stack[in_open] = name;
  full_source_filename_stack[in_open] = make_full_name_string();
  if (name == str_ptr - 1)	{/* we can try to conserve string pool space now */
	temp_str = search_string (name);
	if (temp_str > 0) {
	  name = temp_str;
	  flush_string;
	};
  };
  if (jobname == 0) {
	jobname = getjobname();
	open_log_file();
  }; /* |open_log_file| doesn't |show_context|, so |limit| and |loc| needn't be set to meaningful values yet */
  if (term_offset + length (full_source_filename_stack[in_open]) > (unsigned)max_print_line - 2) {
    print_ln();
  } else if ((term_offset > 0) || (file_offset > 0)) {
    print_char (' ');
  }
  print_char ('(');
  incr (open_parens);
  slow_print (full_source_filename_stack[in_open]);
  update_terminal;
  state = new_line;
  /* begin expansion of Read the first line of the new file */
  /* module 538 */
  /* Here we have to remember to tell the |input_ln| routine not to
   * start with a |get|. If the file is empty, it is considered to
   * contain a single blank line.
   */
  line = 1;
  /* next line was guarded by if() */
  input_ln (cur_file, false);
  firm_up_the_line();
  if (end_line_char_inactive) {
	decr (limit);
  } else {
	buffer[limit] = end_line_char;
  }
  first = limit + 1;
  loc = start;
  /* end expansion of Read the first line of the new file */
}

boolean 
input_next_line (void) {
  return input_ln (cur_file, true);
}

int 
read_terminal (void) {
  int i;
  if(fscanf (term_in,"%d",&i)==0) {
	return -2;
  }
  return i;
}

/* module 1812 */
boolean stop_at_space; /* whether |more_name| returns false for space */
