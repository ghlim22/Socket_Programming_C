#include "Framer.h"
#include "Practical.h"
#include <stdio.h>

static const char DELIM = '\n';

int GetNextMsg(FILE *in, uint8_t *buf, size_t bufSize)
{
    int cnt = 0;
    int nextChar;

    do {
        nextChar = getc(in);

        switch (nextChar) {
        case EOF:
            if (cnt == 0) {
                return -1;
            } else {
                ExitWithUserMsg("GetNextMsg", "Stream ended prematurely.");
            }
            break;
        case DELIM:
            return cnt;
        default:
            buf[cnt++] = nextChar;
            break;
        }
    } while (cnt < bufSize);

    return -cnt; /* out of space: cnt == bufSize */
}

int PutMsg(uint8_t *buf, size_t msgSize, FILE *out)
{
    for (int i = 0; i < msgSize; ++i) {
        if (buf[i] == DELIM) {
            return -1;
        }
    }
    if (fwrite(buf, msgSize, 1, out) != 1) {
        return -1;
    }
    fputc(DELIM, out);
    fflush(out);

    return msgSize;
}