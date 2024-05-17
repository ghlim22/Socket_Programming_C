#include <stdio.h>
#include <stdlib.h>
#include "Practical.h"

void ExitWithSystemMsg(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void ExitWithUserMsg(const char *msg, const char *detail)
{
    fputs(msg, stderr);
    fputs(": ", stderr);
    fputs(detail, stderr);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}
