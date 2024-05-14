#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/_types/_size_t.h>
#include "Practical.h"
#include "Framer.h"
#include "VoteEncoding.h"
#include "VoteProtocol.h"

int main(int argc, char *argv[])
{
    /* Argument Processing */
    if (argc != 2) {
        ExitWithUserMsg("Parameter(s)", "<Port/Service>");
    }

    int sock = SetupServer(argv[1]);
    assert(sock > 0);

    

    return 0;
}