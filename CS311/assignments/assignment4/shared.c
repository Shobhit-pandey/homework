/*
 * Processes and Shared Memory
 */

#include <stdio.h>

#include "main.h"
#include "shared.h"

int shared_main(long max, long procs, FILE* output)
{
    printf("Hello, Shared World! %ld, %ld\n", max, procs);

    return 0;
}
