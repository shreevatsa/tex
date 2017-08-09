
extern char *my_concat (char **d, const char *s, const char *t) ;

extern char *my_concat3 (char **d, const char *s, const char *t, const char *u);

extern char *strndup (char *in,int num);

extern char *strcasestr (char *haystack, char *needle);


/* This is the size of the line buffer for 'text' input files .
 * Input lines that are longer are silently truncated.
 */
#define BUFSIZE 10240


extern char *readline (FILE *file, char **thebuffer);

extern char *chomp (char *line);

extern char *trim (char *line);

extern char *chop (char *line);

extern char *lc (char *mixed);
extern char *uc (char *mixed);


extern int my_strcmp (const void *s1, const void *s2) ;
extern int my_strcasecmp (const void *s1, const void *s2) ;

extern char *Itoa (int in) ;

extern char *backtick(const char *progname, char *options);

extern unsigned cksum (char *line);


#define HASHSIZE 1003

struct hash_item {
  struct hash_item *next;
  char *name;
  void *value;
};

typedef struct hash_item *HashItem;
typedef HashItem *Hash;

extern Hash make_hash (void) ;

extern HashItem get_item (Hash hashtab, char *s);

extern void *do_get_value (Hash hashtab, char *s);

extern char *hash_value (Hash hashtab, char *s);

extern HashItem do_set_value (Hash hashtab, char *name, void *defn);

extern HashItem set_value (Hash hashtab, char *name, char *defn);

extern int keys (Hash hashtab,char * **thekeys);


extern unsigned file_size (char *filename);
extern int file_exists (char *filename);
extern int dir_exists (char *filename);


extern char *next_word (char **restofline, char *split);
extern char *words (char **restofline, char *split);

