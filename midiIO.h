#include <stdint.h>

#pragma pack(1)

//prototypes
uint16_t flipBytes_uint16_t(uint16_t);
uint32_t flipBytes_uint32_t(uint32_t);

typedef struct MIDIheader_st {
    //MThd, 4 bytes
    char MThd[4];
    //<header_length>, 4 bytes
    uint32_t headerLength;
    //<format>, 2 bytes
    uint16_t format;
    //<n>, 2 bytes
    uint16_t n;
    //<division> 2 bytes
    uint16_t division;
} MIDIheader;

typedef struct MIDItrack_st {
    //MTrk, 4 bytes
    char MTrk[4];
    //<length>, 4 bytes
    uint32_t length;
    //<track_event>, <length> bytes
    uint8_t * track;
} MIDItrack;

