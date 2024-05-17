/* Routines for Text encoding for vote messages.
 * Wire Format:
 * "Voting <v|i> [R] <candidate ID> <count>"
 */

#include "VoteEncoding.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_types/_size_t.h>

static const char *MAGIC = "Voting";
static const char *VOTE_STR = "v";
static const char *INQ_STR = "i";
static const char *RESPONSE_STR = "R";
static const char *DELIM_STR = " ";

enum {
    BASE = 10,
};

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
    if (strcmp(tok, RESPONSE_STR) == 0) { /* Response contains voting count */
        v->isResponse = true;
        tok = strtok(NULL, DELIM_STR);
        if (tok == NULL) {
            return false;
        }
    }

    tok = strtok(NULL, DELIM_STR);
    if (tok == NULL) {
        return false;
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

size_t Encode(VoteInfo *v, uint8_t *outBuf, size_t bufSize)
{
    uint8_t *bufPtr = outBuf;
    size_t size = bufSize;

    int rv =
        snprintf(bufPtr, size, "%s %c %s %d", MAGIC, (v->isInquiry ? 'i' : 'v'),
                 (v->isResponse ? RESPONSE_STR : ""), v->candidate);
    bufPtr += rv;
    size -= rv;
    if (v->isResponse) {
        rv = snprintf(bufPtr, size, " %llu", v->count);
        bufPtr += rv;
    }
    return (size_t)(bufPtr - outBuf);
}
