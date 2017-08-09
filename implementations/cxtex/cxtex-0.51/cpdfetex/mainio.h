
/* File open/close stuff */

#include <stdio.h>

#define FOPEN_RBIN_MODE "r"
#define FOPEN_R_MODE "r"
#define FOPEN_W_MODE "w"

#define a_open_in(f,t)  open_input (&(f), t, FOPEN_RBIN_MODE)
#define tfm_b_open_in(f)    open_input (&(f), kpse_tfm_format, FOPEN_RBIN_MODE)

#define w_open_in(f)    open_input (&(f), kpse_fmt_format, FOPEN_RBIN_MODE)
#define tex_b_openin(f) open_input (&(f), kpse_tex_format, FOPEN_RBIN_MODE)
#define vf_b_open_in(f) open_input (&(f), kpse_vf_format, FOPEN_RBIN_MODE)


#define a_open_out(f)  open_output (&(f), FOPEN_W_MODE)
#define b_open_out a_open_out
#define w_open_out a_open_out

#define a_close(a) fclose(a)
#define b_close a_close
#define w_close a_close

/* main.c */
extern void         call_edit (packed_ASCII_code *,pool_pointer,integer,integer);
extern void         topenin (void);
extern boolean      open_in_name_ok (const_string fname);
extern boolean      open_out_name_ok (const_string fname);
extern char *output_comment;


/* lib/openclose.c */
extern boolean open_input (FILE **,  int,  const_string);
extern boolean open_output (FILE **, const_string);
extern int tex_input_type;
extern char *       read_line (FILE *f);

