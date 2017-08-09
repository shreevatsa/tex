
#include "types.h"
#include "c-compat.h"
#include "globals.h"

/* module 151 */

/* Here is a function that returns a pointer to a copy of a glue spec.
 * The reference count in the copy is |null|, because there is assumed
 * to be exactly one reference to the new specification.
 */
pointer
new_spec (pointer p) { /* duplicates a glue specification */
  pointer q;			/* the new spec */
  q = get_node (glue_spec_size);
  mem[q] = mem[p];
  glue_ref_count (q) = null;
  width (q) = width (p);
  stretch (q) = stretch (p);
  shrink (q) = shrink (p);
  return q;
};


/* module 152 */

/* And here's a function that creates a glue node for a given parameter
 * identified by its code number; for example,
 * |new_param_glue(line_skip_code)| returns a pointer to a glue node for the
 * current \.{\\lineskip}.
 */
pointer
new_param_glue (small_number n) {
  pointer p;			/* the new node */
  pointer q;			/* the glue specification */
  p = get_node (small_node_size);
  type (p) = glue_node;
  subtype (p) = n + 1;
  leader_ptr (p) = null;
  q = glue_par(n);
  glue_ptr (p) = q;
  incr (glue_ref_count (q));
  return p;
};


/* module 153 */

/* Glue nodes that are more or less anonymous are created by |new_glue|,
 * whose argument points to a glue specification.
 */
pointer
new_glue (pointer q) {
  pointer p;			/* the new node */
  p = get_node (small_node_size);
  type (p) = glue_node;
  subtype (p) = normal;
  leader_ptr (p) = null;
  glue_ptr (p) = q;
  incr (glue_ref_count (q));
  return p;
};



/* module 154 */

/* Still another subroutine is needed: this one is sort of a combination
 * of |new_param_glue| and |new_glue|. It creates a glue node for one of
 * the current glue parameters, but it makes a fresh copy of the glue
 * specification, since that specification will probably be subject to change,
 * while the parameter will stay put. The global variable |temp_ptr| is
 * set to the address of the new spec.
 */
pointer
new_skip_param (small_number n) {
  pointer p;			/* the new node */
  /* Current |mem| equivalent of glue parameter... */
  temp_ptr = new_spec (glue_par(n));
  p = new_glue (temp_ptr);
  glue_ref_count (temp_ptr) = null;
  subtype (p) = n + 1;
  return p;
};




/* module 177 */

/* Then there is a subroutine that prints glue stretch and shrink, possibly
 * followed by the name of finite units:
 */
void
print_glue (scaled d, int order, char *s) { /* prints a glue component */
  print_scaled (d);
  if ((order < normal) || (order > filll)) {
	zprint_string ("foul");
  } else if (order > normal) {
	zprint_string ("fil");
	while (order > fil) {
	  print_char ('l');
	  decr (order);
	};
  } else { /* if (s != 0)*/
	zprint_string (s);
  };
};


/* module 178 */

/* The next subroutine prints a whole glue specification.
 */
void
print_spec (int p, char *s) { /* prints a glue specification */
  if ((p < mem_min) || (p >= lo_mem_max)) {
	print_char ('*');
  } else {
	print_scaled (width (p));	/*   if (s != 0)*/
	zprint_string (s);
	if (stretch (p) != 0) {
	  zprint_string (" plus ");
	  print_glue (stretch (p), stretch_order (p), s);
	};
	if (shrink (p) != 0) {
	  zprint_string (" minus ");
	  print_glue (shrink (p), shrink_order (p), s);
	};
  };
};


/* module 625 */
#define billion  FLOAT_CONSTANT (1000000000 )
#define vet_glue( arg )  glue_temp   =  arg ; \
 if (  glue_temp  >  billion  )   {           \
   glue_temp   =  billion;                    \
 } else if (  glue_temp  <  -  billion  )  {  \
   glue_temp   =  -  billion;                 \
 }



/* module 1188 */
void 
app_space (void) { /* handle spaces when |space_factor<>1000| */
  pointer q; /* glue node */ 
  if ((space_factor >= 2000) && (xspace_skip != zero_glue)) {
	q = new_param_glue (xspace_skip_code);
  } else {
	if (space_skip != zero_glue) {
	  main_p = space_skip;
	} else { 
	  /* Find the glue specification... */
	  find_glue_spec;
	};
	main_p = new_spec (main_p);
	/* begin expansion of Modify the glue specification in |main_p| according to the space factor */
	/* module 1189 */
	if (space_factor >= 2000)
	  width (main_p) = width (main_p) + extra_space (cur_font);
	stretch (main_p) =  xn_over_d (stretch (main_p), space_factor, 1000);
	shrink (main_p) =  xn_over_d (shrink (main_p), 1000, space_factor);
	/* end expansion of Modify the glue specification in |main_p| according to the space factor */
	q = new_glue (main_p);
	glue_ref_count (main_p) = null;
  };
  link (tail) = q;
  tail = q;
};

/* module 1205 */

/* All the work relating to glue creation has been relegated to the
 * following subroutine. It does not call |build_page|, because it is
 * used in at least one place where that would be a mistake.
 */
void append_glue (void) {
  small_number s; /* modifier of skip command */ 
  s = cur_chr;
  switch (s) {
  case fil_code:
	cur_val = fil_glue;
	break;
  case fill_code:
	cur_val = fill_glue;
	break;
  case ss_code:
	cur_val = ss_glue;
	break;
  case fil_neg_code:
	cur_val = fil_neg_glue;
	break;
  case skip_code:
	scan_glue (glue_val);
	break;
  case mskip_code:
	scan_glue (mu_val);
	;
  }; /* now |cur_val| points to the glue specification */
  tail_append (new_glue (cur_val));
  if (s >= skip_code) {
	decr (glue_ref_count (cur_val));
	if (s > skip_code)
	  subtype (tail) = mu_glue;
  };
};

/* module 1645 */

/* In constructions such as
 * $$\vbox{\halign{\.{#}\hfil\cr
 * {}\\vbox to \\vsize\{\cr
 * \hskip 25pt \\vskip 0pt plus 0.0001fil\cr
 * \hskip 25pt ...\cr
 * \hskip 25pt \\vfil\\penalty-200\\vfilneg\cr
 * \hskip 25pt ...\}\cr}}$$
 * the stretch components of \.{\\vfil} and \.{\\vfilneg} compensate;
 * in standard \TeX\ they may get modified in order to prevent arithmetic
 * overflow during |ship_out| when each of them is multiplied by a large
 * |glue_set| value.
 * 
 * In \eTeX\ the conversion from stretch or shrink components of glue to
 * \.{DVI} units is performed by the |do_glue| function defined below.
 * 
 * In extended mode the |do_glue| function adds up the relevant stretch (or
 * shrink) components of consecutive glue nodes and converts the glue nodes
 * into equivalent kern nodes; during this process glue specifications may
 * be recycled. The accumulated stretch or shrink is then multiplied by
 * |glue_set(this_box)| and returned as result. Since one and the same box
 * may be used several times inside leaders the result is also added to the
 * width of the first or only kern node; the subtype of the glue node(s)
 * remains unchanged. The consecutive glue nodes may be separated by
 * insert, mark, adjust, kern, and penalty nodes.
 */

#define add_stretch_shrink              \
   if (g_sign == stretching) {          \
     if (stretch_order (g) == g_order)  \
       s = s + stretch (g);             \
   } else {                             \
     if (shrink_order (g) == g_order)   \
       s = s - shrink (g);              \
   }

scaled
do_glue (pointer this_box, pointer p) {
  pointer q; /* list traverser */ 
  glue_ord g_order; /* applicable order of infinity for glue */ 
  unsigned char g_sign; /* selects type of glue */ 
  scaled s; /* accumulated stretch or shrink */ 
  real glue_temp; /* glue value before rounding */ 
  g_order = glue_order (this_box);
  g_sign = glue_sign (this_box);
  s = 0;
  add_stretch_shrink;
  if ((!eTeX_ex) || (subtype (p) >= a_leaders)) {
	do_something;
    goto DONE;
  }
  q = p;
CONTINUE:
  type (q) = kern_node;
  width (q) = width (g);
  fast_delete_glue_ref (g);
 NEXTP:
  q = link (q);
  if ((q != null) && !is_char_node (q)) {
    switch (type (q)) {
	case ins_node:
	case mark_node:
	case adjust_node:
	case kern_node:
	case penalty_node:
	  do_something;
	  goto NEXTP;
	case glue_node:
	  if (subtype (q) < a_leaders) {
		g = glue_ptr (q);
		add_stretch_shrink;
		goto CONTINUE;
	  };
	  do_something;
	  break;
	default:
	  do_nothing;
	};
  }
 DONE:
  if (s != 0) {
	vet_glue (zfloat (glue_set (this_box)) * s);
	s = round (glue_temp);
	if (type (p) == kern_node)
	  width (p) = width (p) + s;
  };
  return s;
};

