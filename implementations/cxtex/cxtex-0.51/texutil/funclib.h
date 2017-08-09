
/* Set this to 1 for printf statements regarding memory allocation */
#define DEBUGMEM 0

extern void *safe_malloc (size_t memsize);
extern void safe_free(void *a);
extern void *safe_strdup (void *orig);
extern void *safe_realloc (void *orig, size_t newsize);

#if DEBUGMEM
#define safe_fopen(a,b,c)  a = fopen(b,c); fprintf(stdout,"f %p open was %s (%s)\n",a,b,c)
#define safe_fclose(a)     fprintf(stdout,"f %p close\n",a); fclose(a)
#else
#define safe_fopen(a,b,c)  a = fopen(b,c)
#define safe_fclose(a)     fclose(a)
#endif


extern char *checked_path (char *path) ;
extern char *unix_path (char *path);
extern char *checked_file (char *path);

/* USE_KPSE:
 * If you want to use the kpathsea library to find input files 
 * (instead of the "kpsewhich" executable), you need this switch,
 * and you have to link against the library itself.
 * WIN32:
 * Untested, but switching to Win32 PathS should happen automatically
 */

#ifdef USE_KPSE
#define HAVE_PROTOTYPES 1
#include <kpathsea/progname.h>
#include <kpathsea/proginit.h>
#include <kpathsea/tex-file.h>
#else
typedef char *string;
extern const char *kpsewhich;
#endif


extern const char pathslash;
extern const char *pathslash_string;

extern const char pathslash_u;
extern const char *pathslash_u_string;

#ifdef WIN32
#define dosish 1
#else
#define dosish 0
#endif

#define GLOB_FILES 1
#define GLOB_DIRS 2

extern int globfiles (char *Input,string **Files,int glob_options);

#define STREQ(s1, s2) ((s1) && (strcmp (s1, s2) == 0))

extern void discover_init (char *exe_name);
extern char *discover_file (char *Filename, char *kpse_type_string);
extern char *discover_path (char *kpse_type_string);
