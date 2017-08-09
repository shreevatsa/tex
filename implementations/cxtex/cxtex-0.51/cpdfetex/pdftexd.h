#include "types.h"
#include "c-compat.h"
#include "globals.h"

#include "mainio.h"

#undef name
#undef skip
#undef sc
#undef HAVE_STRSTR
#undef state
#undef mode
#undef data
#undef index
#undef start
#undef year
#undef month
#undef day
#undef link

EXTERN integer vfpacketlength;

#define pdfTeX

#define pdfcharmarked(f, c) (boolean)(pdfcharused[f][c/8] & (1<<(c%8)))
#define pdfmarkchar(f, c) pdfcharused[f][c/8] |= (1<<(c%8))
