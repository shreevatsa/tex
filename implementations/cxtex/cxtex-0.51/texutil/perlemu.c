
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "perlemu.h"

#include "funclib.h"

/* string my_concatenation $d = $s . $t$ ; */
char *my_concat (char **d, const char *s, const char *t) {
  unsigned slen = strlen(s);
  unsigned tlen = strlen(t);
  char *r = (char *)safe_malloc (slen+tlen+1);
  strcpy(r,s);
  strcat(r+slen,t);
  if (*d)
    safe_free(*d);
  *d = r;
  return *d;
}

/* string my_concatenation $d = $s . $t . $u ;  */
char *my_concat3 (char **d, const char *s, const char *t, const char *u) {
  int rover = 0;
  char *r = (char *)safe_malloc ((strlen(s)+strlen(t)+strlen(u)+1));
  if (!r)
    exit (1);
  while (*s) r[rover++] = *s++;
  while (*t) r[rover++] = *t++;
  while (*u) r[rover++] = *u++;
  r[rover] = 0;
  if (*d)
    safe_free(*d);
  *d = r;
  return *d;
}


char *strndup (char *in,int num) {
  char *out = safe_malloc(num*sizeof(char *)+1);
  int k =0;
  for (k=0;(k<num && in[k]);k++) {
    out[k] = in[k];
  }
  out[k] = 0;
  return out;
}


/* strstr, but case-insensitive */
char *strcasestr (char *haystack, char *needle) {
  char *a = strdup(haystack);
  char *b = strdup(needle);
  char *r;
  r = a;
  while (*r) {
	*r = tolower(*r);
	r++;
  }
  r = b;
  while (*r) {
	*r = tolower(*r);
	r++;
  }
  char *res = strstr(a,b);
  if (res == NULL)
	return res;
  int dif = res-a;
  return (haystack+dif);
}

/* reads a line from a text file */
char *readline (FILE *file, char **thebuffer) {
  char *buffer = *thebuffer;
  if (buffer==NULL) 
    exit (1);
  if (fgets(buffer,BUFSIZE,file))
    return buffer;
  return 0;
}

/* remove trailing crlf/newline */
char *chomp (char *line) {
  if (!strlen(line)) 
    return line;
  char *k= line+(strlen(line)-1);
  if (*k == '\n' || *k == '\r') *k-- = 0;
  if (strlen(line) && *k == '\n') *k = 0; /* msdos style input */
  return line;
}

/* implements s/\s*$//; */
char *trim (char *line) {
  if (!strlen(line)) 
    return line;
  char *k = line+(strlen(line)-1);
  while (isspace(*k)) *k-- = 0;
  return line;
}

char *chop (char *line) {
  if (!strlen(line)) 
    return line;
  char *k = line+(strlen(line)-1);
  *k = 0;
  return line;
}


char *lc (char *mixed) {
  char *lower = strdup(mixed);
  if (lower == NULL || strlen(lower)==0) 
    return lower;
  int k=0;
  while (lower[k]) {
    lower[k] = tolower(lower[k]);
    k++;
  }
  return lower;
}

char *uc (char *mixed) {
  char *upper = strdup(mixed);
  if (upper == NULL || strlen(upper)==0) 
    return upper;
  int k=0;
  while (upper[k]) {
    upper[k] = toupper(upper[k]);
    k++;
  }
  return upper;
}

/* for qsort() */
int my_strcmp (const void *s1, const void *s2) {
  return strcmp(*((const char **)s1), *((const char **)s2));
}

int my_strcasecmp (const void *s1, const void *s2) {
  return strcasecmp(*((const char **)s1), *((const char **)s2));
}

char *Itoa (int in) {
  char ret[7];
  snprintf(ret,6,"%d",in);
  return safe_strdup(ret);
}

char *backtick(const char *progname, char *options) {
  FILE *PIPE;
  char ret[BUFSIZE];
  char *val = ret;
  char *cmdline = NULL;
  my_concat3(&cmdline,(char *)progname," ",options);
  if((PIPE=popen(cmdline,"r")) && readline(PIPE,&val)) {
    pclose(PIPE);
    char *retval = safe_strdup(ret);
    return chomp(retval);
  }
  return safe_strdup("");
}



/* SysV chekcsum logic */
unsigned cksum (char *line) {
  unsigned int s=0; 
  int r;
  while (*line) {
    s += *line;
  }
  r = (s & 0xffff) + ((s & 0xffffffff) >> 16);
  return (r & 0xffff) + (r >> 16);
}



Hash make_hash (void) {
  Hash memblock = safe_malloc(sizeof(HashItem)*HASHSIZE);
  int k;
  for (k=0;k<HASHSIZE;k++) {
    memblock[k] = NULL;
  }
  return memblock;
}

unsigned id_hash (char *s) {
  unsigned val;
  for (val = 0; *s != 0; s++)
    val = *s + 31 * val;
  val =  val % HASHSIZE;
  return val;
}

HashItem get_item (Hash hashtab, char *s) {
  HashItem np;
  for (np = hashtab[id_hash(s)]; np != NULL; np = np->next) 
    if (strcmp(np->name,s) == 0)
      return np;
  return NULL;
}

void *do_get_value (Hash hashtab, char *s) {
  HashItem np;
  for (np = hashtab[id_hash(s)]; np != NULL; np = np->next) 
    if (strcmp(np->name,s) == 0)
      return (void *)np->value;
  return NULL;
}

char *hash_value (Hash hashtab, char *s) {
  char *ret = (char *)do_get_value(hashtab,s);
  if (ret == NULL)
    return safe_strdup("");
  return ret ;
}


int keys (Hash hashtab,char * **thekeys) {
  char * *mykeys = safe_malloc(sizeof(char *)*HASHSIZE);
  int k;
  int keycount = 0;
  for (k=0;k<HASHSIZE;k++) {
    if (hashtab[k] && hashtab[k]->name != NULL)
      mykeys[keycount++] = safe_strdup(hashtab[k]->name);
  }
  *thekeys=mykeys;
  return keycount;
}


HashItem do_set_value (Hash hashtab, char *name, void *defn) {
  HashItem np;
  unsigned hashval;
  if ((np = get_item(hashtab,name)) == NULL) {
    np = (HashItem) safe_malloc(sizeof(*np));
    np->name = safe_strdup(name);
    hashval = id_hash(name);
    np->next = hashtab[hashval];
    hashtab[hashval] = np;
  } else {
    safe_free(np->value);
  }
  np->value = defn;
  return np;
}

HashItem set_value (Hash hashtab, char *name, char *defn) {
  return do_set_value(hashtab, name, (void *)safe_strdup(defn));
}


/* if (-f filename) */
unsigned file_size (char *filename) {
  struct stat buf;
  if(!stat(filename, &buf)) 
    return (unsigned) buf.st_size;
  return 0;
}

int file_exists (char *filename) {
  struct stat buf;
  if(!stat(filename, &buf)) 
    return S_ISREG(buf.st_mode);
  return 0;
}

/* if (-d filename) */
int dir_exists (char *filename) {
  struct stat buf;
  if(!stat(filename, &buf))
	return S_ISDIR(buf.st_mode);
  return 0;
}

/* Return the stuff that is before 'split', or NULL
 * on success, moves 'restofline' to the part after 'split'
 */
char *next_word (char **restofline, char *split){
  char *retval = NULL;
  char *loc;
  if ((loc = strstr(*restofline, split)) != NULL) {
    retval = *restofline;
    *loc = 0;
    *restofline = loc + strlen(split);
  }
  return retval;
}

char *words (char **restofline, char *split){
  char *retval = NULL;
  char *loc;
  if (*restofline == NULL || strlen(*restofline)==0)
    return NULL;
  if ((loc = strstr(*restofline, split)) != NULL) {
    retval = *restofline;
    *loc = 0;
    *restofline = loc + strlen(split);
  } else {
    retval = *restofline;
    *restofline = loc + strlen(retval);
  }
  return retval;
}

