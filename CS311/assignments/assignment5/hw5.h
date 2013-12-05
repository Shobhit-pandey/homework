#ifndef _HW_5
#define _HW_5

#include <stdlib.h>

void errExit(const char *errmsg) {
    perror(errmsg);
    exit(EXIT_FAILURE);
}

#endif
