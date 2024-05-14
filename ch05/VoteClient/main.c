#include <cstdint>
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
    if (argc < 4 || argc > 5) {
        ExitWithUserMsg("Parameter(s)",
                        "<Server> <Port/Service> <Candidate> [I]");
    }

    const char *server = argv[1];
    const char *service = argv[2];
    int candidate = atoi(argv[3]);
    if (candidate < 0 || candidate > MAX_CANDIDATE) {
        ExitWithUserMsg("Candidate # not valid", argv[3]);
    }
    bool inq = (argc > 4) && (strcmp(argv[4], "I") == 0);

	/* Get connected socket */

    // Create a connected TCP socket
    int sock = SetupClient(server, service);
    if (sock < 0) {
        ExitWithUserMsg("SetupClient() failed", "unable to connect");
    }

    FILE *str = fdopen(sock, "r+"); // Wrap for stream I/O
    if (str == NULL) {
        ExitWithSystemMsg("fdopen() failed");
    }

    // Set up info for request
    VoteInfo vi;
    memset(&vi, 0, sizeof(vi));
    vi.isInquiry = inq;
    vi.candidate = candidate;

    // Encode for transmission
    uint8_t outbuf[MAX_WIRE_SIZE];
    size_t reqSize = Encode(&vi, outbuf, sizeof(outbuf));

    // Print info
    printf("Sending %lu-byte %s for candidate %d...\n", reqSize,
           (inq ? "inquiry" : "vote"), candidate);

    // Frame and send
    if (PutMsg(outbuf, reqSize, str) < 0) {
        ExitWithSystemMsg("PutMsg() failed");
    }

    // Receive and print response
    uint8_t inbuf[MAX_WIRE_SIZE];
    size_t respSize = GetNextMsg(str, inbuf, sizeof(inbuf));
    if (Decode(inbuf, respSize, &vi)) {
        printf("Received:\n");
        if (vi.isResponse) {
            printf(" Response to ");
        }
        if (vi.isInquiry) {
            printf("inquiry ");
        } else {
            printf("vote ");
        }
        printf("for candidate %d\n", vi.candidate);
        if (vi.isResponse) {
            printf(" count = %llu\n", vi.count);
        }
    }

    fclose(str);

    return 0;
}