/* texmf.c: Hand-coded routines for TeX or Metafont in C.  Originally
   written by Tim Morgan, drawing from other Unix ports of TeX.  This is
   a collection of miscellany, everything that's easier (or only
   possible) to do in C.
   
   This file is public domain.  */

#include "types.h"

#include "c-compat.h"
#include "mainio.h"

/*TODO next define is a hack!! */
#define eTeX_states 1

int junk_nothing; /* weird goto-is-unreached-warning evasion */

#include "globals.h"

#undef name
#undef y1
#undef y0
#undef index
#undef explicit

#include <kpathsea/getopt.h>
#include <kpathsea/proginit.h>
/*#include <kpathsea/debug.h>*/ /* gives a *lot* of redundant decls */
extern unsigned kpathsea_debug;



#define edit_var "TEXEDIT"
#define MAKE_TEX_TEX_BY_DEFAULT 1
#define MAKE_TEX_FMT_BY_DEFAULT 1
#define MAKE_TEX_TFM_BY_DEFAULT 1


typedef struct option getoptstruct;

/* lib defines these*/

extern void usage (const_string str);
extern void usagehelp (const_string *);
extern void printversionandexit (const_string, const_string, const_string);

#include <time.h> /* For `struct tm'.  */
#if defined (HAVE_SYS_TIME_H)
#include <sys/time.h>
#elif defined (HAVE_SYS_TIMEB_H)
#include <sys/timeb.h>
#endif

#include <signal.h> /* Catch interrupts.  */

const_string PDFETEXHELP[] = {
    "Usage: pdfetex [OPTION]... [TEXNAME[.tex]] [COMMANDS]",
    "   or: pdfetex [OPTION]... \\FIRST-LINE",
    "   or: pdfetex [OPTION]... &FMT ARGS",
    "  Run pdfeTeX on TEXNAME, usually creating TEXNAME.pdf.",
    "  Any remaining COMMANDS are processed as pdfeTeX input, after TEXNAME is read.",
    "  If the first line of TEXNAME is %&FMT, and FMT is an existing .fmt file,",
    "  use it.  Else use `NAME.efmt', where NAME is the program invocation name,",
    "  most commonly `pdfetex'.",
    "",
    "  Alternatively, if the first non-option argument begins with a backslash,",
    "  interpret all non-option arguments as a line of pdfeTeX input.",
    "",
    "  Alternatively, if the first non-option argument begins with a &, the",
    "  next word is taken as the FMT to read, overriding all else.  Any",
    "  remaining arguments are processed as above.",
    "",
    "  If no arguments or options are specified, prompt for input.",
    "",
    "-efm=FMTNAME             use FMTNAME instead of program name or a %& line",
    "-ini                     be pdfeinitex, for dumping formats; this is implicitly",
    "                          true if the program name is `pdfeinitex'",
    "-interaction=STRING      set interaction mode (STRING=batchmode/nonstopmode/",
    "                          scrollmode/errorstopmode)",
    "-jobname=STRING          set the job name to STRING",
    "-mltex                   enable MLTeX extensions such as \\charsubdef",
    "-progname=STRING         set program (and fmt) name to STRING",
    "-help                    display this help and exit",
    "-version                 output version information and exit",
    NULL
};


#define BANNER "This is pdfeTeX, Version 3.141592-1.11b-2.1"
#define COPYRIGHT_HOLDER "The NTS Team (eTeX)/Han The Thanh (pdfTeX)"
#define AUTHOR NULL
#define PROGRAM_HELP PDFETEXHELP
#define INPUT_FORMAT kpse_tex_format
#define INI_PROGRAM "pdfeinitex"
#define VIR_PROGRAM "pdfevirtex"



/* The main program, etc.  */

/* What we were invoked as and with.  */
char **argv;
int argc;

/* If the user overrides argv[0] with -progname.  */
static string user_progname;

/* The C version of the jobname, if given. */
const_string job_name;

/* The main body of the WEB is transformed into this procedure.  */
extern int main_body (void);

static void
parse_options(int, string *);

/* TeX globals we use in this file */

//extern unsigned char interaction_option;
extern integer ready_already;


/* The entry point: set up for reading the command line, which will
   happen in `topenin', then call the main body.  */

#define DUMP_OPTION "efm"

void
main_init (int ac, string * av)
{
  /* The `ini_version' variable is declared/used in the change files.  */
  /* TODO: make sure we can still make the fmt file: extra main.c */
  boolean virversion = false;
  char *compatname;
  /* Save to pass along to topenin.  */
  argc = ac;
  argv = av;

  /* Must be initialized before options are parsed.  */
  interaction_option = 4;

  /* If the user says --help or --version, we need to notice early.  And
     since we want the --ini option, have to do it before getting into
     the web (which would read the base file, etc.).  */
  parse_options (ac, av);
  
  /* Do this early so we can inspect program_invocation_name and
     kpse_program_name below, and because we have to do this before
     any path searching.  */
  if (FILESTRCASEEQ (argv[0], "cpdfetex")) {
	compatname = "cpdfetex";
  } else {
	compatname = argv[0];
  }
  kpse_set_program_name (compatname, user_progname); /* TODO: remove */
  
    if (FILESTRCASEEQ (kpse_program_name, INI_PROGRAM)) {
      ini_version = true;
    } else if (FILESTRCASEEQ (kpse_program_name, VIR_PROGRAM)) {
      virversion = true;
    } else if (FILESTRCASEEQ (kpse_program_name, "mltex")) {
      mltex_p = true;
    } else if (FILESTRCASEEQ (kpse_program_name, "initex")) {
      ini_version = true;
    } else if (FILESTRCASEEQ (kpse_program_name, "virtex")) {
      virversion = true;
    }

    if (!dump_name) {
      /* If called as *vir{mf,tex,mpost} use `plain'.  Otherwise, use the
         name we were invoked under.  */
      dump_name = (virversion ? "plain" : kpse_program_name);
    }

  /* If we've set up the fmt/base default in any of the various ways
     above, also set its length.  */
  if (dump_name) {
    /* adjust array for Pascal and provide extension */
    TEX_format_default = concat3 (" ", dump_name, DUMP_EXT);
    format_default_length = strlen (TEX_format_default + 1);
  } else {
    /* For dump_name to be NULL is a bug.  */
    abort();
  }
  /* Additional initializations.  No particular reason for doing them
     here instead of first thing in the change file; less symbols to
     propagate through Webc, that's all.  */
  /* kpse_set_program_enabled (kpse_tfm_format, MAKE_TEX_TFM_BY_DEFAULT,kpse_src_compile); */
  /* kpse_set_program_enabled (kpse_tex_format, MAKE_TEX_TEX_BY_DEFAULT,kpse_src_compile); */
  /* kpse_set_program_enabled (kpse_fmt_format, MAKE_TEX_FMT_BY_DEFAULT,kpse_src_compile); */

  shell_enabled_p = 1 ;
}

/* The entry point: set up for reading the command line, which will
   happen in `topenin', then call the main body.  */

int main (int ac,  string *av) {
  main_init(ac, av);
  return main_body ();
} 



/* This is supposed to ``open the terminal for input'', but what we
   really do is copy command line arguments into TeX's or Metafont's
   buffer, so they can handle them.  If nothing is available, or we've
   been called already (and hence, argc==0), we return with
   `last=first'.  */

void topenin (void) {
  int i;
  buffer[first] = 0; /* In case there are no arguments.  */
  if (optind < argc) { /* We have command line arguments.  */
    int k = first;
    for (i = optind; i < argc; i++) {
      char *ptr = &(argv[i][0]);
      /* Don't use strcat, since in Omega the buffer elements aren't single bytes.  */
      while (*ptr) {
        buffer[k++] = *(ptr++);
      }
      buffer[k++] = ' ';
    }
    argc = 0;	/* Don't do this again.  */
    buffer[k] = 0;
  }
  /* Find the end of the buffer.  */
  for (last = first; buffer[last]; ++last)
    ;

  /* Make `last' be one past the last non-blank character in `buffer'.  */
  /* ??? The test for '\r' should not be necessary.  */
  for (--last; last >= first
       && ISBLANK (buffer[last]) && buffer[last] != '\r'; --last) 
    ;
  last++;

  /* One more time, this time converting to TeX's internal character
     representation.  */
  for (i = first; i < (int)last; i++)
    buffer[i] = Xord(buffer[i]);
}

/* Reading the options.  */

/* Test whether getopt found an option ``A''.
   Assumes the option index is in the variable `option_index', and the
   option table in a variable `long_options'.  */
#define ARGUMENT_IS(a) STREQ (long_options[option_index].name, a)

/* SunOS cc can't initialize automatic structs, so make this static.  */
static struct option long_options[]
  = { { DUMP_OPTION,              1, 0, 0 },
      { "help",                   0, 0, 0 },
      { "ini",                    0, &ini_version, 1 },
      { "interaction",            1, 0, 0 },
      { "progname",               1, 0, 0 },
      { "version",                0, 0, 0 },
      { "mltex",                  0, &mltex_p, 1 },
      { "debug-format",           0, &debug_format_file, 1 },
      { "jobname",                1, 0, 0 },
      { 0, 0, 0, 0 } };

static void
parse_options(int argc,  string * argv)
{
  int g;   /* `getopt' return code.  */
  int option_index;

  for (;;) {
    g = getopt_long_only (argc, argv, "+", long_options, &option_index);

    if (g == -1) /* End of arguments, exit the loop.  */
      break;

    if (g == '?') { /* Unknown option.  */
      usage (argv[0]);
    }

    /*assert (g == 0); */ /* We have no short option names.  */ /*TH and no debug.h ;)*/

	if (ARGUMENT_IS ("progname")) {
      user_progname = optarg;

    } else if (ARGUMENT_IS ("jobname")) {
      job_name = optarg;
      
    } else if (ARGUMENT_IS (DUMP_OPTION)) {
      dump_name = optarg;
      if (!user_progname) user_progname = optarg;
      dump_option = true;

    } else if (ARGUMENT_IS ("interaction")) {
        /* These numbers match @d's in *.ch */
      if (STREQ (optarg, "batchmode")) {
        interaction_option = 0;
      } else if (STREQ (optarg, "nonstopmode")) {
        interaction_option = 1;
      } else if (STREQ (optarg, "scrollmode")) {
        interaction_option = 2;
      } else if (STREQ (optarg, "errorstopmode")) {
        interaction_option = 3;
      } else {
        WARNING1 ("Ignoring unknown argument `%s' to --interaction", optarg);
      }
      
    } else if (ARGUMENT_IS ("help")) {
       usagehelp (PROGRAM_HELP);

    } else if (ARGUMENT_IS ("version")) {
      printversionandexit (BANNER, COPYRIGHT_HOLDER, AUTHOR);

    } /* Else it was a flag; getopt has already done the assignment.  */
  }
}

/* Return true if FNAME is acceptable as a name for \openout, \openin, or
   \input.  */

static boolean
opennameok (const_string fname, const_string check_var,
               const_string default_choice)
{
  /* We distinguish three cases:
     'a' (any)        allows any file to be opened.
     'r' (restricted) means disallowing special file names.
     'p' (paranoid)   means being really paranoid: disallowing special file
                      names and restricting output files to be in or below
                      the working directory or $TEXMFOUTPUT, while input files
                      must be below the current directory, $TEXMFOUTPUT, or
                      (implicitly) in the system areas.
     We default to "paranoid".  The error messages from TeX will be somewhat
     puzzling...
     This function contains several return statements...  */

  const_string open_choice = kpse_var_value (check_var);

  if (!open_choice) open_choice = default_choice;

  if (*open_choice == 'a' || *open_choice == 'y' || *open_choice == '1')
    return true;

#if defined (unix) && !defined (MSDOS)
  {
    const_string base = xbasename (fname);
    /* Disallow .rhosts, .login, etc.  Allow .tex (for LaTeX).  */
    if (base[0] == 0 ||
        (base[0] == '.' && !IS_DIR_SEP(base[1]) && !STREQ (base, ".tex"))) {
      fprintf(stderr, "%s: Not writing to %s (%s = %s).\n",
              program_invocation_name, fname, check_var, open_choice);
      return false;
    }
  }
#else
  /* Other OSs don't have special names? */
#endif

  if (*open_choice == 'r' || *open_choice == 'n' || *open_choice == '0')
    return true;

  /* Paranoia supplied by Charles Karney...  */
  if (kpse_absolute_p (fname, false)) {
    const_string texmfoutput = kpse_var_value ("TEXMFOUTPUT");
    /* Absolute pathname is only OK if TEXMFOUTPUT is set, it's not empty,
       fname begins the TEXMFOUTPUT, and is followed by / */
    if (!texmfoutput || *texmfoutput == '\0'
        || fname != strstr (fname, texmfoutput)
        || !IS_DIR_SEP(fname[strlen(texmfoutput)])) {
      fprintf(stderr, "%s: Not writing to %s (%s = %s).\n",
              program_invocation_name, fname, check_var, open_choice);
      return false;
    }
  }
  /* For all pathnames, we disallow "../" at the beginning or "/../"
     anywhere.  */
  if (fname[0] == '.' && fname[1] == '.' && IS_DIR_SEP(fname[2])) {
    fprintf(stderr, "%s: Not writing to %s (%s = %s).\n",
            program_invocation_name, fname, check_var, open_choice);
    return false;
  } else {
    const_string dotpair = strstr (fname, "..");
    /* If dotpair[2] == DIR_SEP, then dotpair[-1] is well-defined. */
    if (dotpair && IS_DIR_SEP(dotpair[2]) && IS_DIR_SEP(dotpair[-1])) {
      fprintf(stderr, "%s: Not writing to %s (%s = %s).\n",
              program_invocation_name, fname, check_var, open_choice);
      return false;
    }
  }

  /* We passed all tests.  */
  return true;
}

boolean open_in_name_ok (const_string fname)
{
    /* For input default to all. */
    return opennameok (fname, "openin_any", "a");
}

boolean open_out_name_ok (const_string fname)
{
    /* For output, default to paranoid. */
    return opennameok (fname, "openout_any", "p");
}


#define RETSIGTYPE void
/* All our interrupt handler has to do is set TeX's or Metafont's global
   variable `interrupt'; then they will do everything needed.  */
#ifdef WIN32_REAL
/* Win32 doesn't set SIGINT ... */
BOOL WINAPI
catch_interrupt (DWORD arg)
{
  switch (arg) {
  case CTRL_C_EVENT:
  case CTRL_BREAK_EVENT:
    interrupt = 1;
    return TRUE;
  default:
    /* No need to set interrupt as we are exiting anyway */
    return FALSE;
  }
}
#else /* not WIN32 */
static RETSIGTYPE
catch_interrupt (int arg)
{
  int junkvar; 
  junkvar = arg; /* gcc -Wunused */
  interrupt = 1;
#ifdef OS2
  (void) signal (SIGINT, SIG_ACK);
#else
  (void) signal (SIGINT, catch_interrupt);
#endif /* not OS2 */
}
#endif /* not WIN32 */

/* Besides getting the date and time here, we also set up the interrupt
   handler, for no particularly good reason.  It's just that since the
   `fix_date_and_time' routine is called early on (section 1337 in TeX,
   ``Get the first line of input and prepare to start''), this is as
   good a place as any.  */

void
get_date_and_time (integer * zminutes,  integer * zday,
                      integer * zmonth,  integer * zyear)
{
  time_t clock = time ((time_t *) 0);
  struct tm *tmptr = localtime (&clock);

  *zminutes = tmptr->tm_hour * 60 + tmptr->tm_min;
  *zday = tmptr->tm_mday;
  *zmonth = tmptr->tm_mon + 1;
  *zyear = tmptr->tm_year + 1900;

  {
#ifdef SA_INTERRUPT
    /* Under SunOS 4.1.x, the default action after return from the
       signal handler is to restart the I/O if nothing has been
       transferred.  The effect on TeX is that interrupts are ignored if
       we are waiting for input.  The following tells the system to
       return EINTR from read() in this case.  From ken@cs.toronto.edu.  */

    struct sigaction a, oa;

    a.sa_handler = catch_interrupt;
    sigemptyset (&a.sa_mask);
    sigaddset (&a.sa_mask, SIGINT);
    a.sa_flags = SA_INTERRUPT;
    sigaction (SIGINT, &a, &oa);
    if (oa.sa_handler != SIG_DFL)
      sigaction (SIGINT, &oa, (struct sigaction *) 0);
#else /* no SA_INTERRUPT */
#ifdef WIN32_REAL
    SetConsoleCtrlHandler(catch_interrupt, TRUE);
#else /* not WIN32 */
    RETSIGTYPE (*old_handler) P1H(int);
    
    old_handler = signal (SIGINT, catch_interrupt);
    if (old_handler != SIG_DFL)
      signal (SIGINT, old_handler);
#endif /* not WIN32 */
#endif /* no SA_INTERRUPT */
  }
}

/*
  Generating a better seed numbers
  */
integer
getrandomseed()
{
#if defined (HAVE_GETTIMEOFDAY)
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (tv.tv_usec + 1000000 * tv.tv_usec);
#elif defined (HAVE_FTIME)
  struct timeb tb;
  ftime(&tb);
  return (tb.millitm + 1000 * tb.time);
#else
  time_t clock = time ((time_t*)NULL);
  struct tm *tmptr = localtime(&clock);
  return (tmptr->tm_sec + 60*(tmptr->tm_min + 60*tmptr->tm_hour));
#endif
}



/* Look up VAR_NAME in texmf.cnf; assign either the value found there or
   DFLT to *VAR.  */

void
setup_bound_variable (integer * var,  const_string var_name,  integer dflt)
{
  string expansion = kpse_var_value (var_name); /* const char * = ffs_ini_value(const char *) */
  *var = dflt;

  if (expansion) {
    integer conf_val = atoi (expansion);
    /* It's ok if the cnf file specifies 0 for extra_mem_{top,bot}, etc.
       But negative numbers are always wrong.  */
    if (conf_val < 0 || (conf_val == 0 && dflt > 0)) {
      fprintf (stderr,
               "%s: Bad value (%ld) in texmf.cnf for %s, keeping %ld.\n",
               program_invocation_name,
               (long) conf_val, var_name + 1, (long) dflt);
    } else {
      *var = conf_val; /* We'll make further checks later.  */
    }
    free (expansion);
  }
}

