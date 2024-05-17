/* Routines for Text encoding for vote messages.
 * Wire Format:
 * "Voting <v|i> [R] <candidate ID> <count>"
 */

#include "VoteEncoding.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *MAGIC = "Voting";
static const char *VOTE_STR = "v";
static const char *INQ_STR = "i";
static const char *RESPONSE_STR = "R";
static const char *DELIM_STR = " ";

enum {
    BASE = 10,
};

/* Extract message information from given buffer.
 * Note: modifies input buffer.
 */
bool Decode(uint8_t *inBuf, size_t mSize, VoteInfo *v)
{
    char *tok = strtok((char *)inBuf, DELIM_STR);
    if (tok == NULL || strcmp(tok, MAGIC) != 0) {
        return false;
    }

    tok = strtok(NULL, DELIM_STR);
    if (tok == NULL) {
        return false;
    }
    if (strcmp(tok, VOTE_STR) == 0) {
        v->isInquiry = false;
    } else if (strcmp(tok, INQ_STR) == 0) {
        v->isInquiry = true;
    } else {
        return false;
    }

    tok = strtok(NULL, DELIM_STR);
    if (tok == NULL) {
        return false;
    }
    if (strcmp(tok, RESPONSE_STR) == 0) {
        v->isResponse = true;
        tok = strtok(NULL, DELIM_STR);
        if (tok == NULL) {
            return false;
        }
    } else {
        v->isResponse = false;
    }

    v->candidate = atoi(tok);
    if (v->isResponse) {
        tok = strtok(NULL, DELIM_STR);
        if (tok == NULL) {
            return false;
        }
        v->count = strtoll(tok, NULL, BASE);
    } else {
        v->count = 0L;
    }
    return true;
}

/* Encode voting message info as a text string.
 * Warning: Message will be silently truncated if buffer is too samll.
 * Invariants not checked.
 */
size_t Encode(VoteInfo *v, uint8_t *outBuf, size_t bufSize)
{
    uint8_t *bufPtr = outBuf;
    long size = bufSize;
    int rv = snprintf((char *)outBuf, size, "%s %c %s %d", MAGIC,
                      (v->isInquiry ? "i" : "v"), (v->isResponse ? "R" : ""),
                      v->candidate);
    bufPtr += rv;
    size -= rv;
    if (v->isResponse) {
        rv = snprintf((char *)outBuf, size, " %llu", v->count);
        bufPtr += rv;
    }

    return (size_t)(bufPtr - outBuf);
}