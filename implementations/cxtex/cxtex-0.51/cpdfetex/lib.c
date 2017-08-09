
/* this file is a mini-version of web2c's lib.a */

#include "types.h"
#include "c-compat.h"
#include "globals.h"
#include "mainio.h"

/* zround.c: round R to the nearest whole number.  This is supposed to
   implement the predefined Pascal round function.  */

integer 
zround (double r) {
  integer i;

  /* R can be outside the range of an integer if glue is stretching or
     shrinking a lot.  We can't do any better than returning the largest
     or smallest integer possible in that case.  It doesn't seem to make
     any practical difference.  Here is a sample input file which
     demonstrates the problem, from phil@cs.arizona.edu:
     	\documentstyle{article}
	\begin{document}
	\begin{flushleft}
	$\hbox{} $\hfill 
	\filbreak
	\eject
    
     djb@silverton.berkeley.edu points out we should testing against
     TeX's largest or smallest integer (32 bits), not the machine's.  So
     we might as well use a floating-point constant, and avoid potential
     compiler bugs (also noted by djb, on BSDI).  */
  if (r > 2147483647.0)
    i = 2147483647;
  /* should be ...8, but atof bugs are too common */
  else if (r < -2147483647.0)
    i = -2147483647;
  /* Admittedly some compilers don't follow the ANSI rules of casting
     meaning truncating toward zero; but it doesn't matter enough to do
     anything more complicated here.  */
  else if (r >= 0.0)
    i = (integer)(r + 0.5);
  else
    i = (integer)(r - 0.5);

  return i;
}

/* uexit.c: define uexit to do an exit with the right status.  We can't
   just call `exit' from the web files, since the webs use `exit' as a
   loop label.  */

void
uexit (int unix_code) {
  int final_code;
  
  if (unix_code == 0)
    final_code = EXIT_SUCCESS;
  else if (unix_code == 1)
    final_code = EXIT_FAILURE;
  else
    final_code = unix_code;
  
  exit (final_code);
}

/* Define some variables. */
string full_name_of_file = NULL;

/* For TeX and MetaPost.  See below.  Always defined so we don't have to
   #ifdef, and thus this file can be compiled once and go in lib.a.  */
int tfmtemp;
int ocptemp;
int tex_input_type;


/* Open an input file F, using the kpathsea format FILEFMT and passing
   FOPEN_MODE to fopen.  The filename is in `name_of_file+1'.  We return
   whether or not the open succeeded.  If it did, `name_of_file' is set to
   the full filename opened, and `name_length' to its length.  */

boolean
open_input (FILE ** f_ptr,  int filefmt,  const_string fopen_mode) {
  /* A negative FILEFMT means don't use a path.  */
  if (filefmt < 0)
    { /* no_file_path, for BibTeX .aux files and MetaPost things.  */
      *f_ptr = fopen (name_of_file + 1, fopen_mode);
    }
  else 
    {
      /* The only exception to `must_exist' being true is \openin, for
         which we set `tex_input_type' to 0 in the change file.  */
      /* According to the pdfTeX people, pounding the disk for .vf files
         is overkill as well.  A more general solution would be nice. */
      boolean must_exist = (filefmt != kpse_tex_format || tex_input_type)
              && (filefmt != kpse_vf_format);
      text_char *fname = kpse_find_file (name_of_file + 1,
                                     (kpse_file_format_type)filefmt,
                                     must_exist);

      if (full_name_of_file) {
          free(full_name_of_file);
          full_name_of_file = NULL;
      }
      
      if (fname)
        {
          full_name_of_file = xstrdup(fname);
          /* If we found the file in the current directory, don't leave
             the `./' at the beginning of `name_of_file', since it looks
             dumb when `tex foo' says `(./foo.tex ... )'.  On the other
             hand, if the user said `tex ./foo', and that's what we
             opened, then keep it -- the user specified it, so we
             shouldn't remove it.  */
          if (fname[0] == '.' && IS_DIR_SEP (fname[1])
              && (name_of_file[1] != '.' || !IS_DIR_SEP (name_of_file[2])))
            {
              unsigned i = 0;
              while (fname[i + 2] != 0)
                {
                  fname[i] = fname[i + 2];
                  i++;
                }
              fname[i] = 0;
            }
          
          if (name_of_file + 1 != fname)
            {
              free (name_of_file);
              name_length = strlen (fname);
              name_of_file = (string)xmalloc (name_length + 2);
              strcpy (name_of_file + 1, fname);
              free (fname);
            }
          
          *f_ptr = xfopen (name_of_file + 1, fopen_mode);
          
          /* If we just opened a TFM file, we have to read the first byte,
             to pretend we're Pascal.  See tex.ch and mp.ch.  Ditto for
             the ocp/ofm Omega file formats.  */
          if (filefmt == kpse_tfm_format) {
            tfmtemp = getc (*f_ptr);
            /* We intentionally do not check for EOF here, i.e., an
               empty TFM file.  TeX will see the 255 byte and complain
               about a bad TFM file, which is what we want.  */
          } else if (filefmt == kpse_ocp_format) {
            ocptemp = getc (*f_ptr);
          } else if (filefmt == kpse_ofm_format) {
            tfmtemp = getc (*f_ptr);
          }
        }
      else
        *f_ptr = NULL;
    } /* search path case */

  return *f_ptr != NULL;
}

/* Open an output file F either in the current directory or in
   $TEXMFOUTPUT/F, if the environment variable `TEXMFOUTPUT' exists.
   (Actually, this also applies to the BibTeX and MetaPost output files,
   but `TEXMFMPBIBOUTPUT' was just too long.)  The filename is in the
   global `name_of_file' + 1.  We return whether or not the open
   succeeded.  If it did, `name_of_file' is reset to the name opened if
   necessary, and `name_length' to its length.  */

boolean
open_output P2C(FILE **, f_ptr,  const_string, fopen_mode)
{
  /* Is the filename openable as given?  */
  *f_ptr = fopen (name_of_file + 1, fopen_mode);

  if (! *f_ptr)
    { /* Can't open as given.  Try the envvar.  */
      string texmfoutput = kpse_var_value ("TEXMFOUTPUT");

      if (texmfoutput && *texmfoutput && !kpse_absolute_p(name_of_file+1, false))
        {
          string fname = concat3 (texmfoutput, DIR_SEP_STRING, name_of_file + 1);
          *f_ptr = fopen (fname, fopen_mode);
          
          /* If this succeeded, change name_of_file accordingly.  */
          if (*f_ptr)
            {
              free (name_of_file);
              name_length = strlen (fname);
              name_of_file = (string)xmalloc (name_length + 2);
              strcpy (name_of_file + 1, fname);
            }

          free (fname);
        }
    }
        
  return *f_ptr != NULL;
}

/* Close F.  */

void
aclose P1C(FILE *, f)
{
  /* If F is null, just return.  bad_pool might close a file that has
     never been opened.  */
  if (!f)
    return;
    
  if (fclose (f) == EOF) {
    /* It's not always name_of_file, we might have opened something else
       in the meantime.  And it's not easy to extract the filenames out
       of the pool array.  So just punt on the filename.  Sigh.  This
       probably doesn't need to be a fatal error.  */
    perror ("fclose");
  }
}

/* This string is appended to all the banners and used in --version.  */

char *versionstring = WEB2CVERSION;


/* usage.c: Output a help message (from help.h).

   Modified in 2001 by O. Weber.
   Written in 1995 by K. Berry.  Public domain.  */


/* Call usage if the program exits with an "usage error".  STR is supposed
   to be the program name. */

void
usage (const_string str) {
  fprintf (stderr, "Try `%s --help' for more information.\n", str);
  uexit (1);
}

/* Call usage if the program exits by printing the help message.
   MESSAGE is an NULL-terminated array or strings which make up the
   help message.  Each string is printed on a separate line.
   We use arrays instead of a single string to work around compiler
   limitations (sigh).
*/
void
usagehelp (const_string*Message) {
    extern KPSEDLL char *kpse_bug_address;

    while (*Message) {
        fputs(*Message, stdout);
        putchar('\n');
        ++Message;
    }
    putchar('\n');
    fputs(kpse_bug_address, stdout);
    uexit(0);
}

/* printversion.c: Output for the standard GNU option --version.

   Written in 1996 by K. Berry.  Public domain.  */

/* We're passed in the original WEB banner string, which has the form
This is PROGRAM, Version VERSION-NUMBER
   We parse the PROGRAM and VERSION-NUMBER out of this.
   
   If COPYRIGHT_HOLDER is specified and AUTHOR isn't, then use the
   former for the latter.  If AUTHOR is specified and COPYRIGHT_HOLDER
   isn't, it means the original program is public domain.
   
   Maybe I should have just done it all inline in each individual
   program, but tangle doesn't allow multiline string constants ...  */

void
printversionandexit (const_string Banner,
					 const_string copyright_holder,  
					 const_string author) {
  /*  extern string versionstring; */          /* from web2c/lib/version.c */
  extern KPSEDLL string kpathsea_version_string;/* from kpathsea/version.c */
  string prog_name;
  unsigned len;
  const_string prog_name_end = strchr (Banner, ',');
  const_string prog_version = strrchr (Banner, ' ');
  /*assert (prog_name_end && prog_version);*/
  prog_version++;
  
  len = prog_name_end - Banner - sizeof ("This is");
  prog_name = (string)xmalloc (len + 1);
  strncpy (prog_name, Banner + sizeof ("This is"), len);
  prog_name[len] = 0;

  /* The Web2c version string starts with a space.  */
  printf ("%s%s %s\n", prog_name, versionstring, prog_version);
  puts (kpathsea_version_string);

  if (copyright_holder) {
    printf ("Copyright (C) 1997-2003 %s.\n", copyright_holder);
    if (!author)
      author = copyright_holder;
  }

  puts ("Kpathsea is copyright (C) 1997-2003 Free Software Foundation, Inc.");

  puts ("There is NO warranty.  Redistribution of this software is");
  fputs ("covered by the terms of ", stdout);
  /* DVIcopy is GPL'd, so no additional words needed. */
  if (/*copyright_holder && */!STREQ (prog_name, "DVIcopy")) {
    printf ("both the %s copyright and\n", prog_name);
  }
  puts ("the GNU General Public License.");
  puts ("For more information about these matters, see the files");
  printf ("named COPYING and the %s source.\n", prog_name);
  printf ("Primary author of %s: %s.\n", prog_name, author);
  puts ("Kpathsea written by Karl Berry and others.\n");

  uexit (0);
}

void *
xrealloc_array (void *initialv, unsigned newsize) {
  void *newpointer = realloc(initialv,newsize);
  if (newpointer != NULL) {
	return newpointer;
  } else {
	return NULL;
  } 
}


