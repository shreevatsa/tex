
/* module 12 */

/* Like the preceding parameters, the following quantities can be changed
 * at compile time to extend or reduce \TeX's capacity. But if they are changed,
 * it is necessary to rerun the initialization program \.{INITEX}
 * 
 * to generate new tables for the production \TeX\ program.
 * One can't simply make helter-skelter changes to the following constants,
 * since certain rather complex initialization
 * numbers are computed from them. They are defined here using
 * \.{WEB} macros, instead of being put into \PASCAL's |const| list, in order to
 * emphasize this distinction.
 */

#define HASH_SIZE (256*256*16)
EXTERN const int hash_prime;

/* module 256 */
#define next(arg)  hash[arg].lhfield
#define text(arg)  hash[arg].rh

EXTERN two_halves *hash; /* the hash table */ 

EXTERN boolean global_no_new_control_sequence; /* are new identifiers legal? */ 
#define is_no_new_control_sequence() global_no_new_control_sequence
#define set_no_new_control_sequence(a) global_no_new_control_sequence=a

EXTERN int cs_count; /* total number of known identifiers */

EXTERN pointer id_lookup (int j, int l);
EXTERN pointer id_lookup_string (str_number);

EXTERN void primitive (str_number s, quarterword c, halfword o);

EXTERN void hash_initialize (void);

EXTERN void hash_initialize_init (void);

EXTERN void dump_hash (void);

EXTERN void undump_hash (void);
