#include <stdio.h>

/* typedef unsigned short  hword; */
typedef unsigned long   hword;
typedef long            scal;
typedef long            val;


#define INF_BAD 10000

hword
badness (t, s)
    scal    t;
    scal    s;
{
    val     r;

    if (t == 0)
        return 0;
    else if (s <= 0)
        return INF_BAD;
    else {
        if (t <= 7230584)
            r = (t * 297) / s;
        else if (s >= 1663497)
            r = t / (s / 297);
        else r = t;
        if (r > 1290)
            return INF_BAD;
        else return ((r * r * r + 0400000) / 01000000);
    }
}

int main() {
  scal s = 10000;
  /* 2.144333 */
  scal t = 21485;
  printf("%lu\n", badness(t, s));
}
