
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "texexec.h"

int RunTeX (char *a) { 
  return SystemTeX(a); 
}

int main(int ac,  string *av) {
  return texexec_main(ac,av);
}
