#include <stdio.h>
#include <stdlib.h>
#include "Practical.h"

void die_with_user_message(const char *msg, const char *detail)
{
    fputs(msg, stderr);
    fputs(": ", stderr);
    fputs(detail, stderr);
    fputs("\n", stderr);
    exit(EXIT_FAILURE);
}

void die_with_system_message(const char *msg)
{
    perror(msg);
    exit(EXIT_SUCCESS);
}