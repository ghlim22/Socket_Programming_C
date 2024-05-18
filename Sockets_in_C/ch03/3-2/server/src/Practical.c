#include <stdio.h>
#include <stdlib.h>
#include "Practical.h"

void exit_with_system_message(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void exit_with_user_message(const char *msg, const char *detail)
{
    fputs(msg, stderr);
    fputs(": ", stderr);
    fputs(detail, stderr);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}
