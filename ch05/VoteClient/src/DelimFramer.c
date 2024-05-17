#include "Framer.h"
#include "Practical.h"
#include <stdint.h>
#include <stdio.h>
#include <sys/_types/_size_t.h>

static const char DELIM = '\n';

/* Read up to bufSize or until delimiter, copying into the given buffer as we
 * go.
 * Encountering EOF after some data but before delimiter results in failure.
 * (That is: EOF is not a valid delimiter.)
 * If buffer fills without encountering delimiter, negative count is returned.
 * If stream ends before first byte, -1 is returned.
 * Precondition: buf has room for at leat bufSize bytes.
 */
int GetNextMsg(FILE *in, uint8_t *buf, size_t bufSize)
{
    size_t cnt = 0;
    int nextChar;

    do {
        nextChar = getc(in);

        switch (nextChar) {
        case EOF:
            if (cnt > 0) {
                ExitWithUserMsg("GetNextMsg()", "Stream ended prematurely");
            } else {
                return -1;
            }
            break;
        case DELIM:
            return cnt;
        default:
            buf[cnt++] = (uint8_t)nextChar;
            break;
        }
    } while (cnt < bufSize);

    // buffer is filled without encountering delimiter
    // out of space: cnt == bufSize
    return -cnt;
}

/* Write the given message to the output stream, followed by the delimiter.
 * Return number of bytes written, or -1 on faliure.
 */
int PutMsg(uint8_t buf[], size_t msgSize, FILE *out) 
{
    for (size_t i = 0; i < msgSize; ++i) {
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