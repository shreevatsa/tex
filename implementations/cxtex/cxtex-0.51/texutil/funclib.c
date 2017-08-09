
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

#include "funclib.h"
#include "perlemu.h"

#ifdef WIN32
#include "glob/glob.h"
#else
#include <glob.h>
#endif

extern char *my_concat (char **d, const char *s, const char *t);

/* memory management functions */

void *safe_malloc (size_t memsize) {
  void *test = malloc(memsize);
  if (test == NULL)
    exit(1);
#if DEBUGMEM
    fprintf(stdout,"%p alloc (malloc) (%i bytes)\n",test,memsize);
#endif
  return test;
}

void safe_free(void *a) {
  if (a!=NULL) {
    free(a);
#if DEBUGMEM
    fprintf(stdout,"%p free\n",a);
#endif
  }
}

void *safe_strdup (void *orig) {
  void *test = strdup(orig);
  if (test == NULL)
    exit(1);
#if DEBUGMEM
    fprintf(stdout,"%p alloc (strdup) (%i bytes)\n",test,strlen(orig));
#endif
  return test;
}

void *safe_realloc (void *orig, size_t newsize) {
  void *test = realloc (orig, newsize);
  if (test == NULL) 
    exit (1);
#if DEBUGMEM
  fprintf(stdout,"%p free (realloc)\n",orig);
  fprintf(stdout,"%p alloc (realloc) (%i bytes)\n",test,newsize);
#endif
  return test;
}

#ifndef USE_KPSE
#ifdef WIN32
const char *kpsewhich = "kpsewhich.exe";
#else
const char *kpsewhich = "kpsewhich";
#endif
#endif

#ifdef WIN32
const char pathslash = '\\';
const char *pathslash_string = "\\";
#else
const char *pathslash_string = "/";
const char pathslash = '/';
#endif

const char *pathslash_u_string = "/";
const char pathslash_u = '/';


char *checked_path (char *path) {
  if (path ==NULL)
	return (char *)pathslash_string;
  char *ret = path;
  while (*ret) {
    if (*ret == '/' || *ret == '\\')
      *ret = pathslash;
    ret++;
  }
  ret--;
  if (*ret != pathslash)
    my_concat(&path,path,pathslash_string);
  return path; 
}


char *unix_path (char *path) {
  if (path ==NULL)
	return safe_strdup("");
  char *ret = path;
  while (*ret) {
    if (*ret == '/' || *ret == '\\')
      *ret = pathslash_u;
    ret++;
  }
  return path; 
}


char *checked_file (char *path) {
  if (path ==NULL)
	return "";
  char *ret = path;
  while (*ret) {
    if (*ret == '/' || *ret == '\\')
      *ret = pathslash;
    ret++;
  }
  return path; 
}

#define ERROR_CHECK(a)                   \
  if(a) {                                \
    safe_free(globpointer);           	 \
    globfree(&pglob);                    \
    fprintf(stdout,"glob failed\n");     \
    exit(1);				 \
  }

#define doglob(a,b,c) ERROR_CHECK(glob(a,(GLOB_MARK|GLOB_NOSORT|GLOB_NOCHECK|b),NULL,&c))

int globfiles (char *Input,string **Files, int options) {
  glob_t pglob;
  char *file;
  int NOfFiles = 0;
  string *MyFiles;
  char *globs = safe_strdup(Input);
  char *globpointer = globs;
  if (strchr(globs,' ') != NULL) {
    char *nextglob = next_word(&globs," ");
    doglob(nextglob,0,pglob);
    while ((nextglob = words(&globs," ")) != NULL) {
	  doglob(nextglob,GLOB_APPEND,pglob);
    }
  } else {
    doglob(globs,0,pglob);
  }
  /* now finish */
  if (pglob.gl_pathc) {
    MyFiles = safe_malloc((pglob.gl_pathc+1)*sizeof(char *));
    int k = 0;
    while (pglob.gl_pathv[k]) {
      file = pglob.gl_pathv[k];
	  if (options & GLOB_DIRS) {
		if(file_exists(file) || dir_exists(file)) {
		  MyFiles[NOfFiles] = safe_strdup(file);
		  NOfFiles++;
		}
	  } else {
		char *lastslash = strrchr(file,pathslash);
		if (lastslash && lastslash != file+strlen(file)-1)
		  lastslash = NULL;
		if (!lastslash) {
		  if(file_exists(file)) {
			MyFiles[NOfFiles] = safe_strdup(file);
			NOfFiles++;
		  }
		}
	  }
	  k++;
	}
  } else {
    MyFiles = safe_malloc(sizeof(char *));
  }
  MyFiles[NOfFiles] = NULL;
  *Files = MyFiles;
  safe_free(globpointer);
  globfree(&pglob);
  return NOfFiles;
}

void discover_init(char *exe_name) {
#ifdef USE_KPSE
  kpse_set_program_name(exe_name,"context");
  kpse_init_prog("CONTEXT",600,NULL,NULL);
#endif
}


char *discover_path (char *kpse_type_string) {
  char *ret;
#ifdef USE_KPSE
  int kpse_type_id=kpse_tex_format;
  if (strstr(kpse_type_string,"fmt")==0)
	kpse_type_id=kpse_fmt_format;
  //
  if (!kpse_format_info[kpse_type_id].type) /* needed if arg was numeric */
	kpse_init_format (kpse_type_id);
  ret = (char *)kpse_format_info[kpse_type_id].path;
  if (!ret)
	ret = safe_strdup("");
  return ret;
#else
  char *cmdline = safe_malloc((strlen(kpse_type_string)+14));
  sprintf(cmdline,"--show_path=\"%s\"",kpse_type_string);
  ret = backtick(kpsewhich,cmdline);
  safe_free(cmdline);
  return chomp(ret);
#endif
}  

char *discover_file (char *Filename, char *kpse_type_string) {
#ifdef USE_KPSE
  int kpse_type_id=kpse_tex_format;
  if (STREQ(kpse_type_string,"web2c files"))
	kpse_type_id=kpse_web2c_format;
  if (STREQ(kpse_type_string,"other text files"))
	kpse_type_id=kpse_program_text_format;
  char *ret = kpse_find_file(Filename,kpse_type_id,true);
  if (!ret)
	ret = safe_strdup("");
  return ret;
#else
  char *cmdline = safe_malloc((strlen(kpse_type_string)+strlen(Filename)+13+20));
  sprintf(cmdline,"--progname=context --format=\"%s\" %s",kpse_type_string,Filename);
  char *ret = backtick(kpsewhich,cmdline);
  safe_free(cmdline);
  return chomp(ret);
#endif
}
