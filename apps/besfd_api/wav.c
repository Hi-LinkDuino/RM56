#include <string.h>
#include "besfd_api.h"

#include "hal_trace.h"

#if !defined(WAVE_FORMAT_PCM)
#define WAVE_FORMAT_PCM		(0x0001)
#endif
#if !defined(WAVE_FORMAT_ADPCM)
#define WAVE_FORMAT_ADPCM	(0x0011)
#endif



/* little endian*/
typedef struct { // 44 bytes
	char tag1[4]; // "RIFF"
	uint32_t flen; // the first 8 bytes not included: 00 03 B3 BE = 242622, +8=242630 is the file size
	char tag2[4]; // "WAVE"
	
	char tag3[4]; // "fmt "
	uint32_t chunkSize; // always be: 00 00 00 10
	uint16_t fmtTag; // 0x01
	uint16_t channel; // 00 01
	uint32_t samprate; // 00 00 56 22
	uint32_t avgBytesPerSec; // 00 00 AC 44
	uint16_t blockAlign; // 00 02
	uint16_t bitsPerSample; // 00 10

	char tag4[4]; // "data"
	uint32_t pcmDataLen; // 00 03 B3 8C = 242572, the following pcm data length
}WavFileHeader;


FILE *wav_file_new(char *fname, uint32_t sample_rate, uint8_t chan)
{
    TRACE(2, "%s(%s) start", __func__, fname);

    WavFileHeader header = {
        .tag1 = {'R', 'I', 'F', 'F'},
        .flen = 0,
        .tag2 = {'W', 'A', 'V', 'E'},
        .tag3 = {'f', 'm', 't', ' '},
        .chunkSize = 0x10,
        .fmtTag = 0x01,
        .channel = chan,
        .samprate = sample_rate,
        .avgBytesPerSec = sample_rate * chan * 2,
        .blockAlign = chan * 2,
        .bitsPerSample = 16,
        .tag4 = {'d', 'a', 't', 'a'},
        .pcmDataLen = 0,
    };

    FILE *fp = fopen(fname, "w+b");
    if(fp){
        int ret = fwrite(&header, 1, sizeof(header), fp);
        TRACE(0, "%s(%s) write header: %d", __func__, fname, ret);
    }else{
        TRACE(0, "%s(%s) failed", __func__, fname);
    }
    return fp;
}
int wav_file_write(FILE *fp, uint8_t *buf, uint32_t size){
    return fp?fwrite(buf, 1, size, fp):-1;
}
void wav_file_done(FILE *fp, uint32_t pcm_size){
    fseek(fp, sizeof(WavFileHeader)-4, SEEK_SET);
    fwrite(&pcm_size, 1, sizeof(pcm_size), fp);
    pcm_size += sizeof(WavFileHeader) - 8;
    fseek(fp, 4, SEEK_SET);
    fwrite(&pcm_size, 1, sizeof(pcm_size), fp);
#if 1
    //dump header
    WavFileHeader header;
    fseek(fp, 0, SEEK_SET);
    int ret = fread(&header, 1, sizeof(WavFileHeader), fp);
    TRACE(0, "%s(0x%02X) read header returns %d", __func__, pcm_size, ret);
    for(int i = 0; i < sizeof(header); i += 16){
        DUMP8("%02X ", (uint8_t *)&header+i, 16);
    }
#endif
    fclose(fp);
}



int wav_buffer_new(uint8_t *buf, uint32_t sample_rate, uint8_t chan){
    TRACE(2, "%s(%p) start", __func__, buf);

    WavFileHeader header = {
        .tag1 = {'R', 'I', 'F', 'F'},
        .flen = 0,
        .tag2 = {'W', 'A', 'V', 'E'},
        .tag3 = {'f', 'm', 't', ' '},
        .chunkSize = 0x10,
        .fmtTag = 0x01,
        .channel = chan,
        .samprate = sample_rate,
        .avgBytesPerSec = sample_rate * chan * 2,
        .blockAlign = chan * 2,
        .bitsPerSample = 16,
        .tag4 = {'d', 'a', 't', 'a'},
        .pcmDataLen = 0,
    };
    memcpy(buf, &header, sizeof(header));
    return sizeof(header);
}
void wav_buffer_done(uint8_t *buf, uint32_t pcm_size){
    int header = sizeof(WavFileHeader);
    pcm_size -= header;
    memcpy(&buf[header-4], &pcm_size, sizeof(pcm_size));  //pcm_size

    pcm_size += header - 8;
    memcpy(&buf[4], &pcm_size, sizeof(pcm_size)); // file size - 8
}
