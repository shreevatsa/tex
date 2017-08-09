
/* ctexmf.h: Main include file for TeX in C. */

/* because of problems with glue-ing the typical non-underscore
  web2c stuff that we intend to use unchanged, we need to setup
  this next list of defines */

#include "web2c.h"

#include <stdlib.h>
#include <ctype.h> /* isprint*/
#include <string.h> /* strlen */

#define HAVE_STRSTR
#include <kpathsea/lib.h> /* PERROR */
#include <kpathsea/progname.h> /* program_invocation_name */
#include <kpathsea/c-pathch.h>

#ifndef HAVE_PROTOTYPES
#define HAVE_PROTOTYPES
#endif
#include <kpathsea/tex-file.h>
#undef HAVE_PROTOTYPES

#include <kpathsea/tex-make.h> /* kpse_make_tex_discard_errors */
#include <kpathsea/readable.h>
#include <kpathsea/variable.h>
#include <kpathsea/absolute.h>


#define INIT 1

#define system_exec             system


/* pdftexdir/ptexlib.h */

extern void printID(str_number);
extern void dopdffont(integer, internal_font_number);
extern void writezip(boolean);
extern void writestreamlength(integer, integer);
extern scaled getpkcharwidth(internal_font_number, scaled);
extern void checkextfm(str_number, integer);
extern integer fmlookup(internal_font_number);
extern internal_font_number tfmoffm(integer);
extern integer newvfpacket(internal_font_number);
extern void startpacket(internal_font_number,integer);
extern void storepacket(integer, integer, integer);
extern void setcharmap(internal_font_number);
extern scaled extxnoverd(scaled, scaled, scaled);
extern boolean ispdfimage(integer);
extern integer imagewidth(integer);
extern integer imageheight(integer);
extern void deleteimage(integer);
extern void writeimage(integer);
extern integer epdforigx(integer);
extern integer epdforigy(integer);
extern integer cfgpar(integer);
extern boolean iscfgtruedimen(integer);
extern void setjobid(int, int, int, int, int, int);
extern void updateimageprocset(integer);
extern boolean checkimageb(integer);
extern boolean checkimagec(integer);
extern boolean checkimagei(integer);
extern integer imagepages(integer);
extern integer imagexres(integer);
extern integer imageyres(integer);
extern integer readimage(str_number, integer, str_number, integer, integer, integer);
extern void pdfmapfile(integer);
extern void libpdffinish  (void);

extern char *makecstring (integer);
extern str_number maketexstring (const char*);






#define maxint 2147483647
#define round(x)        zround ((double) (x))
#define intcast(x) ((integer) (x))
#define stringcast(x) ((string) (x))
#define z_fabs(x) ((x) >= 0.0 ? (x) : -(x))
#define libc_free(a) free(a)
#define address_of(x) (&(x))
#define xmalloc_array(type,size) ((type*)xmalloc((size+1)*sizeof(type)))

#define true 1
#define false 0

/* module 16 */

/* Here are some macros for common programming idioms.
 */
#define loop  while (  true  )
#define do_nothing 
#define empty 0

extern int junk_nothing;
#define do_something junk_nothing=0

/* version.c */
#define WEB2CVERSION " (Web2C 7.5.2, C port 0.5.1)"
#define version_string WEB2CVERSION

extern void readconfigfile(void);

extern eight_bits packetbyte(void);

extern void poppacketstate(void);
extern void pushpacketstate(void);
extern str_number getresnameprefix(void);

extern void printcreationdate(void); /*pdtexdir/utils.c */


#include "lib.h"

