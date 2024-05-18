#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct VoteInfo
{
	uint64_t count; // invariant: !isResponse -> count == 0
	int candidate; // invariant: 0 <= candidate <= MAX_CANDIDATE
	bool isInquiry;
	bool isResponse;
} VoteInfo;

enum {
	MAX_CANDIDATE = 1000,
	MAX_WIRE_SIZE = 500
};
