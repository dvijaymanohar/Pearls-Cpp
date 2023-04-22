
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <poll.h>

#include "betting_game_common.h"


/** This function is used to make up the header of the 4 messages specified,
 *  as all messages share a common header
 */
void
make_header(
    unsigned protocol_ver,
    unsigned pkt_type,
    uint8_t pkt_size,
    uint16_t client_id,
    betting_game_common_hdr_t *send_hdr)
{
    send_hdr->version = protocol_ver;
    send_hdr->type = pkt_type;
    send_hdr->length = pkt_size;
    send_hdr->client_id = client_id;
}

int
gen_betting_number(
    int min,
    int max)
{
    struct timeval tv;
    struct timezone tz;

    gettimeofday(&tv, &tz);
    srand((unsigned)tv.tv_usec);

    return (rand() % (max - min + 1) + min);
}

/* error - wrapper for perror */
void
error(
    char *msg)
{
    perror(msg);
    exit(0);
}


