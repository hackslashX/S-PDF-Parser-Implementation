#ifndef SPDF_COMMONDEFINITIONS_H
#define SPDF_COMMONDEFINITIONS_H

#include<map>

const char newline{ '\n' };

// Stream filters
#define SPDF_FILTER_ASCIIHEX      1
#define SPDF_FILTER_ASCII85       2
#define SPDF_FILTER_LZW           3
#define SPDF_FILTER_FLATE         4
#define SPDF_FILTER_RUNLENGTH     5
#define SPDF_FILTER_CCITTFAX      6
#define SPDF_FILTER_JBIG2         7
#define SPDF_FILTER_DCT           8
#define SPDF_FILTER_JPX           9
#define SPDF_FILTER_UNKNOWN     254
#define SPDF_FILTER_NONE        255      // No filter means stream is NOT decoded

// Chunk Data
#ifndef SPDF_ZLIB_CHUNK
#define SPDF_ZLIB_CHUNK		262144
#endif


#endif