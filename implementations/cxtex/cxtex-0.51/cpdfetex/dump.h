
#define FORMAT_DEBUG(var,vary)        if (debug_format_file) \
                                 { fprintf (stderr, "%s%s", "fmtdebug:", var);  \
                                 fprintf (stderr, "%s%ld\n", " = ", (long)vary);  };


#define DUMP_FORMAT kpse_fmt_format

/* module 520 */
#define format_area_length 0


#define format_ext_length 4
#define format_extension  ".efm"

#define DUMP_EXT format_extension

/* module 1454 */
#define dump_four_ASCII  w.b0   =  str_pool [k] ;    \
                         w.b1   =  str_pool [k + 1]; \
                         w.b2   =  str_pool [k + 2]; \
                         w.b3   =  str_pool [k + 3]; \
                         dump_qqqq (w)
/* module 1455 */
#define undump_four_ASCII  undump_qqqq (w);          \
                           str_pool [k]      =  w.b0;\
                           str_pool [k + 1]  =  w.b1;\
                           str_pool [k + 2]  =  w.b2;\
                           str_pool [k + 3]  =  w.b3 


/* (Un)dumping.  These are called from the change file.  */
#define dump_things(base, len) \
  do_dump ((char *) &(base), sizeof (base), (int) (len), fmt_file)
#define undump_things(base, len) \
  do_undump ((char *) &(base), sizeof (base), (int) (len), fmt_file)

/* Like do_undump, but check each value against LOW and HIGH.  The
   slowdown isn't significant, and this improves the chances of
   detecting incompatible format files.  In fact, Knuth himself noted
   this problem with Web2c some years ago, so it seems worth fixing.  We
   can't make this a subroutine because then we lose the type of BASE.  */
#define undump_checked_things(low, high, base, len)                       \
  do {                                                                  \
    unsigned i;                                                         \
    undump_things (base, len);                                           \
    for (i = 0; i < (unsigned)(len); i++) {                                       \
      if ((int)(&(base))[i] < (low) || (&(base))[i] > (high)) {              \
        FATAL5 ("Item %u (=%ld) of .fmt array at %lx <%ld or >%ld",     \
                i, (integer) (&(base))[i], (unsigned long) &(base),     \
                (integer) low, (integer) high);                         \
      }                                                                 \
    }                                                                   \
  } while (0)

/* Like undump_checked_things, but only check the upper value. We use
   this when the base type is unsigned, and thus all the values will be
   greater than zero by definition.  */
#define undump_upper_check_things(high, base, len)                         \
  do {                                                                  \
    unsigned i;                                                         \
    undump_things (base, len);                                           \
    for (i = 0; i < (len); i++) {                                       \
      if ((&(base))[i] > (high)) {                                      \
        FATAL4 ("Item %u (=%ld) of .fmt array at %lx >%ld",             \
                i, (integer) (&(base))[i], (unsigned long) &(base),     \
                (integer) high);                                        \
      }                                                                 \
    }                                                                   \
  } while (0)


/* Use the above for all the other dumping and undumping.  */
#define generic_dump(x) dump_things (x, 1)
#define generic_undump(x) undump_things (x, 1)

#define dump_wd   generic_dump
#define dump_hh   generic_dump
#define dump_qqqq generic_dump
#define undump_wd   generic_undump
#define undump_hh   generic_undump
#define undump_qqqq generic_undump

/* `dump_int' is called with constant integers, so we put them into a
   variable first.  */
#define dump_int(x)                                                     \
  do {                                                                  \
	integer x_val = (x);												\
	generic_dump (x_val);												\
  } while (0)

#define undump_int generic_undump


EXTERN const_string dump_name;

EXTERN boolean dump_option;
EXTERN boolean dump_line;
EXTERN FILE *fmt_file;

/* module 1444 */
EXTERN str_number format_ident;


EXTERN void         do_dump (char * p,  int item_size,  int nitems,  FILE * out_file);
EXTERN void         do_undump (char * p,  int item_size,  int nitems,  FILE * in_file);
EXTERN boolean open_fmt_file (void);
EXTERN boolean load_fmt_file (void);

EXTERN void store_fmt_file (void);

/* module 1816 */
EXTERN boolean debug_format_file;

EXTERN void dump_initialize (void);
