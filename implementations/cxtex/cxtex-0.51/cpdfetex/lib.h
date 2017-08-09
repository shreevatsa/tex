/* zround.c*/
extern integer zround (double);

/* uexit.c */
extern void uexit (int);

extern void *xrealloc_array (void *initialv, unsigned newsize) ;


#define dump_core uexit(1)

