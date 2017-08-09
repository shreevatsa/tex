
#define trie_op_size ( 35111 ) /* space for ``opcodes'' in the hyphenation patterns; best if relatively 
								  prime to 313, 361, and 1009. */
#define min_trie_op ( 0 ) /* first possible trie op code for any language */
#define max_trie_op ( ssup_trie_opcode ) /* largest possible trie opcode for any language */
#define inf_trie_size ( 8000 )
#define sup_trie_size ( ssup_trie_size )

#define hyph_prime 607

EXTERN integer trie_size; 
EXTERN trie_pointer *trie_trl;
EXTERN trie_pointer *trie_tro;
EXTERN quarterword *trie_trc;
EXTERN small_number hyf_distance[trie_op_size];
EXTERN small_number hyf_num[trie_op_size];
EXTERN trie_opcode hyf_next[trie_op_size];
EXTERN unsigned int op_start[257];
EXTERN str_number *hyph_word;
EXTERN halfword *hyph_list;
EXTERN hyph_pointer *hyph_link;
EXTERN int hyph_count;
EXTERN int hyph_next;
EXTERN boolean trie_not_ready;

EXTERN void new_patterns (void);
EXTERN void init_trie (void);
EXTERN void trie_initialize (void);
EXTERN void trie_initialize_init (void);
EXTERN void trie_xmalloc (integer size);

/* module 1803 */
#define set_hyph_index   if (trie_char (hyph_start + cur_lang) !=  qi (cur_lang)) { hyph_index = 0; }\
                         else  { hyph_index   =  trie_link ( hyph_start  +  cur_lang ); }

#define set_lc_code(arg)                                   \
    if (hyph_index == 0)   {                               \
      hc [0 ]  =  lc_code ( arg );                         \
    } else if (trie_char (hyph_index + arg) != qi (arg)) { \
       hc [0 ]  = 0;                                       \
    } else  {                                              \
	  hc [0 ]  =  qo ( trie_op ( hyph_index  +  arg ));    \
	}

/* module 1803 */
EXTERN trie_pointer hyph_start; /* root of the packed trie for |hyph_codes| */
EXTERN trie_pointer hyph_index; /* pointer to hyphenation codes for |cur_lang| */

EXTERN void dump_trie (void);
EXTERN boolean undump_trie (void);
