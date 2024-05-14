#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdio>

int GetNextMsg(FILE *in, uint8_t *buf, size_t bufSize);
int PutMsg(uint8_t buf[], size_t msgSize, FILE *out);