#include "Framer.h"
#include "Practical.h"
#include <stdint.h>
#include <stdio.h>
#include <netinet/in.h>

int GetNextMsg(FILE *in, uint8_t *buf, size_t bufSize)
{
    uint16_t mSize = 0;
    uint16_t extra = 0;

    if (fread(&mSize, sizeof(uint16_t), 1, in) != 1) {
        return -1;
    }
    mSize = ntohs(mSize);
    if (mSize > bufSize) {
        extra = mSize - bufSize;
        mSize = bufSize;
    }
    if (fread(buf, sizeof(uint8_t), mSize, in) != mSize) {
        fprintf(stderr, "Framing error: expected %d, read less\n", mSize);
        return -1;
    }
    if (extra > 0) {
        uint8_t waste[BUF_SIZE];
        fread(waste, sizeof(uint8_t), extra, in);
        return -(mSize + extra);
    }
    return mSize;
}

int PutMsg(uint8_t *buf, size_t msgSize, FILE *out)
{
    if (msgSize > UINT16_MAX) {
        return -1;
    }
    uint16_t padloadSize = htons(msgSize);
    if (fwrite(&padloadSize, sizeof(uint16_t), 1, out) != 1) {
        return -1;
    }
    if (fwrite(buf, sizeof(uint8_t), msgSize, out) != msgSize) {
        return -1;
    }
    fflush(out);
    return msgSize;
}