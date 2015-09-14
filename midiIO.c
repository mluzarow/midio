#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "midiIO.h"

#define TRUE 1
#define FALSE 0

/*int readMIDIdbytes(char input[2000], int length, int location) {
    int i = 0;
    char val[length];
    int ival = 0;

    for (i = 0; i < length; i++) {
        val[i] = input[i + location];
        ival += val[i];
    }

    return(ival);
}*/

/*void readMIDIheader(char input[2000]) {
    int i = 0;
    char val[5];
    int ival = 0;

    for (i = 0; i < 4; i++) {
        val[i] = input[i];
    }
    val[4] = '\0';

    //read <header_length>, 4 bytes
    ival = readMIDIdbytes(input, 4, 4);
    printf("Header chunk length: %d\n", ival);

    //read <format>, 2 bytes
    ival = readMIDIdbytes(input, 2, 8);
    printf("File format: %d\n", ival);

    //read <n>, 2 bytes
    ival = readMIDIdbytes(input, 2, 10);
    printf("Track chunks following header chunk: %d\n", ival);

    //read <division>, 2 bytes
    ival = readMIDIdbytes(input, 2, 12);
    printf("Delta timing: %d\n", ival);

    return;
}*/

/*long flipBytesLong(long input) {
    long * val = malloc(sizeof(val));
    long in;
    memcpy(val, &input, sizeof(input));

    *val = ((*val >> 24) & 0xff) | //move byte 3 to byte 0
            ((*val << 8) & 0xff0000) | //move byte 1 to byte 2
            ((*val >> 8) & 0xff00) | //move byte 2 to byte 1
            ((*val << 24) & 0xff000000); //move byte 0 to byte 3

    memcpy(&in, val, sizeof(*val));
    free(val);
    return(in);
}*/

uint16_t flipBytes_uint16_t(uint16_t val) { //reverse 2 bytes
    val = (val << 8) | (val >> 8);
    return(val);
}

uint32_t flipBytes_uint32_t(uint32_t val) { //reverse 4 bytes
    val = ((val >> 24) & 0xff) | //move byte 3 to byte 0
            ((val << 8) & 0xff0000) | //move byte 1 to byte 2
            ((val >> 8) & 0xff00) | //move byte 2 to byte 1
            ((val << 24) & 0xff000000); //move byte 0 to byte 3

    return(val);
}

MIDIheader MIDIheaderLoad(FILE * f) {
    MIDIheader header; //MIDI header
    size_t read;       //read data
    int error = FALSE; //error flag

    if (!error) { //Try to read MIDI header
        read = fread(&header, sizeof(header), 1, f);

        //Header has not been read correctly
        if (read != 1) { //Print header read failure
            printf("Failed to read header\n");
            error = TRUE;
        }
    }

    //Header has been read correctly
    if (!error) { //reverse bytes in case of big endianness
        header.headerLength = flipBytes_uint32_t(header.headerLength);
        header.format = flipBytes_uint16_t(header.format);
        header.n = flipBytes_uint16_t(header.n);
        header.division = flipBytes_uint16_t(header.division);
    }
//C:\users\Mark\Desktop\toot\test.mid
    //Header has been read correctly
    if (!error) { //Print MIDI header
        //Add '\0' to text tags
        char pMThd[5];
        int i = 0;

        for (i = 0; i < 4; i++) {
            pMThd[i] = header.MThd[i];
        }
        pMThd[4] = '\0';

        printf("MThd tag (should be MThd): %s\n", pMThd);
        printf("Header length: %d bytes\n", header.headerLength);

        switch((int)header.format) {
            case(0): printf("MIDI format: %x - Single track format\n", header.format);
                break;
            case(1): printf("MIDI format: %x - Multiple track format\n", header.format);
                break;
            case(2): printf("MIDI format: %x - Multiple song format\n", header.format);
        }

        printf("Number of truck chunks following the header chunk: %d track chunks\n", (int)header.n);
        printf("Delta timing: %d\n", (int)header.division);
    }

    return(header);
}

MIDItrack MIDItrackLoad(FILE * f) {
    MIDItrack track;   //MIDI track
    size_t read;       //read data
    int error = FALSE; //error flag

    if (!error) { //Try to read MIDI header
        //read header
        read = fread(&track.MTrk, sizeof(track.MTrk), 1, f);
        read = fread(&track.length, sizeof(track.length), 1, f);

        if (read != 1) { //Read failed
            printf("Failed to read track\n");
            error = TRUE;
        }
    }

    if (!error) { //reverse bytes in case of big endianness
        track.length = flipBytes_uint32_t(track.length);
    }

    track.track = malloc(sizeof(uint8_t) * 10);

    fread(&track.track, sizeof(track.track), 1, f);
//C:\users\Mark\Desktop\toot\test.mid
    if (!error) {
        printf("MTrk tag (should be MTrk): %s\n", track.MTrk);
        printf("Track length: %d bytes\n", (int)track.length);
    }

    printf("%x", track.track);
    free(track.track);
    return(track);
}

void MIDIload(const char filename[]) {
    int error = FALSE;
    FILE * f = NULL;
    uint8_t * buffer = NULL;

    if (!error) { //Try to open MIDI file stream
        //Open file with non-text read
        f = fopen(filename, "rb");

        if (!f) { //File is NULL
            printf("Failed to open file '%s'\n", filename);
            error = TRUE;
        }
    }

    if (!error) { //Read file into buffer
        //seek to the end, return size, and seek back to start
        fseek(f, 0, SEEK_END);
        int sz = ftell(f);
        fseek(f, 0, SEEK_SET);

        //size buffer to fit entire file
        buffer = malloc(sizeof(uint8_t) * sz);
    }


    //header -> midi type, track chunk

    MIDIheaderLoad(f);
    MIDItrackLoad(f);

    //close file stream
    fclose(f);

    return;
}

int main(void) {
    printf("MIDI file IO: Reading MIDI\n\n");

 //   int i = 0;
    char fileName[60];

    while(1) {
        printf("Enter the file name of the MIDI Ex: MIDI_File.mid\n");
        scanf("%s", &fileName);

        MIDIload(fileName);
        //header = MIDIheaderLoad(fileName);
        //track = MIDItrackLoad(fileName);
        //MIDItrack track[header.n];
/*
        if (strcmp(header.MThd, "MThd") == 0) {
            MIDItrack track[(int)header.n];

            for (i = 0; i < (int)header.n; i++) {
                track[i] = MIDItrackLoad(fileName);
            }
        }*/
    }

    return(0);
}
