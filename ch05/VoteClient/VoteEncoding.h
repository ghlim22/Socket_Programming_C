#pragma once

#include "VoteProtocol.h"
#include <cstddef>
#include <cstdint>

bool Decode(uint8_t *inBuf, size_t mSize, VoteInfo *v);
size_t Encode(VoteInfo *v, uint8_t *outBuf, size_t bufSize);
