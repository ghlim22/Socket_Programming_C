#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/_types/_size_t.h>
#include <unistd.h>
#include "Practical.h"
#include "Framer.h"
#include "VoteEncoding.h"
#include "VoteProtocol.h"

static uint64_t counts[MAX_CANDIDATE + 1];

int main(int argc, char *argv[])
{
    if (argc != 2) {
        ExitWithUserMsg("Parameter(s)", "<Service/Port>");
    }

    int listeningSock = SetupServer(argv[1]);

    while (true) {
        int clientSock = AcceptConnection(listeningSock);
        FILE *channel = fdopen(clientSock, "r+");
        if (channel == NULL) {
            ExitWithSystemMsg("fdopen() failed");
        }

        VoteInfo v;
        int mSize;
        uint8_t inBuf[MAX_WIRE_SIZE];

        while ((mSize = GetNextMsg(channel, inBuf, sizeof(inBuf))) > 0) {
            memset(&v, 0, sizeof(v));
            if (!Decode(inBuf, mSize, &v)) {
                fputs("Parse error, closing connection.\n", stderr);
                break;
            }
            if (v.isResponse) {
                continue;
            }
            v.isResponse = true;
            if (v.candidate >= 0 && v.candidate <= MAX_CANDIDATE) {
                if (!v.isInquiry) {
                    counts[v.candidate]++;
                }
                v.count = counts[v.candidate];
            }

            uint8_t outBuf[MAX_WIRE_SIZE];
            mSize = Encode(&v, outBuf, sizeof(outBuf));
            if (PutMsg(outBuf, mSize, channel) < 0) {
                fputs("Error framing/putting message\n", stderr);
                break;
            }

            printf("Processed %s for candidate %d; current count is %llu.\n",
                   (v.isInquiry ? "inquiry" : "vote"), v.candidate, v.count);
            fflush(channel);
        }

        puts("Client finished");
        fclose(channel);
    }
}

void response(VoteInfo *v, FILE *channel)
{
    v->isResponse = true;
    if (!(v->candidate >= 0 && v->candidate <= MAX_CANDIDATE)) {
        return;
    }
    if (!v->isInquiry) {
        counts[v->candidate]++;
    }
    v->count = counts[v->candidate];

    uint8_t outBuf[MAX_WIRE_SIZE];
    int mSize = Encode(v, outBuf, sizeof(outBuf));
    if (PutMsg(outBuf, mSize, channel) < 0) {
        fputs("Error framing/outputting message\n", stderr);
    }
}

// int main(int argc, char *argv[])
// {
//     /* Argument Processing */
//     if (argc != 2) {
//         ExitWithUserMsg("Parameter(s)", "<Port/Service>");
//     }

//     int listeningSock = SetupServer(argv[1]);
//     assert(listeningSock > 0);

//     while (true) {
//         int clientSock = AcceptConnection(listeningSock);
//         FILE *channel = fdopen(clientSock, "r+");
//         if (channel == NULL) {
//             ExitWithSystemMsg("fdopen() failed");
//         }

//         int mSize;
//         uint8_t inBuf[MAX_WIRE_SIZE];
//         VoteInfo v;
//         while ((mSize = GetNextMsg(channel, inBuf, sizeof(inBuf))) > 0) {
//             memset(&v, 0, sizeof(v));
//             printf("Received message (%d bytes)\n", mSize);
//             if (Decode(inBuf, mSize, &v)) {
//                 if (!v.isResponse) {
//                     v.isResponse = true;
//                     if (v.candidate >= 0 && v.candidate <= MAX_CANDIDATE) {
//                         if (!v.isInquiry) {
//                             counts[v.candidate]++;
//                         }
//                         v.count = counts[v.candidate];
//                     }
//                 }
//                 uint8_t outBuf[MAX_WIRE_SIZE];
//                 mSize = Encode(&v, outBuf, sizeof(outBuf));
//                 if (PutMsg(outBuf, mSize, channel) < 0) {
//                     fputs("Error framing/outputting message\n", stderr);
//                     break;
//                 } else {
//                     printf("Processed %s for candidate %d; current count is "
//                            "%llu.\n",
//                            (v.isInquiry ? "inquity" : "vote"), v.candidate,
//                            v.count);
//                 }
//                 fflush(channel);
//             } else {
//                 fputs("Parse error, closing connection.\n", stderr);
//                 break;
//             }
//         }
//         puts("Client finished");
//         fclose(channel);
//     }

//     return 0;
// }