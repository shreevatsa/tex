
EXTERN boolean src_specials_p; /* Whether |src_specials| are enabled at all */ 

EXTERN char *last_source_name;
EXTERN int last_lineno;

EXTERN boolean insertsrcspecialauto;
EXTERN boolean insertsrcspecialeverypar;
EXTERN boolean insertsrcspecialeveryparend;
EXTERN boolean insertsrcspecialeverycr;
EXTERN boolean insertsrcspecialeverymath;
EXTERN boolean insertsrcspecialeveryhbox;
EXTERN boolean insertsrcspecialeveryvbox;
EXTERN boolean insertsrcspecialeverydisplay;

EXTERN void insert_src_special  (void);

EXTERN void append_src_special (void);
