#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lzma_decode.h"
#include "hal_trace.h"
#include "hal_cmu.h"

#if defined(NUTTX_BUILD)
#define nts_free free
#define nts_malloc malloc
#endif
extern void nts_free(void *rmem);
extern void *nts_malloc(size_t size);

#if 0
void dsp_code_decom(unsigned char * inbuf, int in_size, unsigned char *outbuf)
{
    unsigned int length, processedSize;
    unsigned int compressedSize, outSize, outSizeProcessed, lzmaInternalSize;
    void *inStream, *outStream, *lzmaInternalData;
    unsigned char properties[5];
    unsigned char prop0;
    int ii;
    int lc, lp, pb;
    int res;
    int in_pos = 0;

    length = in_size;
    TRACE(0, "a7_dsp_code_decom start");

    memcpy(properties, &inbuf[in_pos], sizeof(properties));
    in_pos += sizeof(properties);

    outSize = 0;
    for (ii = 0; ii < 4; ii++)
    {
        unsigned char b;
        b = inbuf[in_pos++];
        outSize += (unsigned int)(b) << (ii * 8);
    }

    if (outSize == 0xFFFFFFFF)
    {
        TRACE(0, "stream version is not supported");
        return;
    }

    for (ii = 0; ii < 4; ii++)
    {
        unsigned char b;
        b = inbuf[in_pos++];
        if (b != 0) {
            TRACE(0, "too long file");
            return;
        }
    }

    compressedSize = length - 13;
    inStream = (void *)&inbuf[in_pos];
    prop0 = properties[0];
    if (prop0 >= (9*5*5)) {
        TRACE(0, "\n Properties error");
        return;
    }
    for (pb = 0; prop0 >= (9 * 5); 
        pb++, prop0 -= (9 * 5));
    for (lp = 0; prop0 >= 9; 
        lp++, prop0 -= 9);
    lc = prop0;
    lzmaInternalSize = (LZMA_BASE_SIZE + (LZMA_LIT_SIZE << (lc + lp)))* sizeof(CProb);
    TRACE(0, "malloc size = %d", lzmaInternalSize);

    outStream = outbuf;
    lzmaInternalData = nts_malloc(lzmaInternalSize);
    if (outStream == 0 || lzmaInternalData == 0) {
        TRACE(0, "can't allocate");
        return;
    }

    res = LzmaDecode((unsigned char *)lzmaInternalData, lzmaInternalSize,
    lc, lp, pb,
    (unsigned char *)inStream, compressedSize,
    (unsigned char *)outStream, outSize, &outSizeProcessed);
    outSize = outSizeProcessed;

    if (res != 0) {
        TRACE(0, "error = %d\n", res);
        nts_free(lzmaInternalData);
        return;
    }

    nts_free(lzmaInternalData);
    TRACE(0, "dsp_code_decom done");

    return;
}
#endif

#define LZMA_PROPERTIES_OFFSET 0
#define LZMA_SIZE_OFFSET       LZMA_PROPERTIES_SIZE
//#define LZMA_DATA_OFFSET       LZMA_SIZE_OFFSET + sizeof(uint64_t)
#define LZMA_DATA_OFFSET       LZMA_SIZE_OFFSET + 8

int dsp_code_decom (unsigned char *outStream, unsigned int *uncompressedSize,
			      unsigned char *inStream,  unsigned int  length)
{
	int res = LZMA_RESULT_DATA_ERROR;
	int i;
	unsigned int outSizeFull = 0xFFFFFFFF; /* 4GBytes limit */
	unsigned int inProcessed;
	unsigned int outProcessed;
	unsigned int outSize;
	unsigned int outSizeHigh;
	CLzmaDecoderState state;  /* it's about 24-80 bytes structure, if int is 32-bit */
	unsigned char properties[LZMA_PROPERTIES_SIZE];
	unsigned int compressedSize = (unsigned int)(length - LZMA_DATA_OFFSET);

#if 0
	TRACE ("LZMA: Image address............... 0x%lx", inStream);
	TRACE ("LZMA: Properties address.......... 0x%lx", inStream + LZMA_PROPERTIES_OFFSET);
	TRACE ("LZMA: Uncompressed size address... 0x%lx", inStream + LZMA_SIZE_OFFSET);
	TRACE ("LZMA: Compressed data address..... 0x%lx", inStream + LZMA_DATA_OFFSET);
	TRACE ("LZMA: Destination address......... 0x%lx", outStream);
#endif
	if (inStream == NULL)
		return LZMA_RESULT_DATA_ERROR;

	memcpy(properties, inStream + LZMA_PROPERTIES_OFFSET, LZMA_PROPERTIES_SIZE);
	memset(&state, 0, sizeof(state));
	res = bes_LzmaDecodeProperties(&state.Properties, properties, LZMA_PROPERTIES_SIZE);
	if (res != LZMA_RESULT_OK) {
		TRACE(0, "lzma decode properties fail");
		return res;
	}

	outSize = 0;
	outSizeHigh = 0;
	/* Read the uncompressed size */
	for (i = 0; i < 8; i++) {
		unsigned char b = inStream[LZMA_SIZE_OFFSET + i];
	    if (i < 4) {
		    outSize += (UInt32)(b) << (i * 8);
		} else {
			outSizeHigh += (UInt32)(b) << ((i - 4) * 8);
		}
	}

	outSizeFull = (unsigned int)outSize;
#if 0
	if (sizeof(unsigned int) >= 8) {
		/*unsigned int is a 64 bit uint => We can manage files larger than 4GB! */
		outSizeFull |= (((unsigned int)outSizeHigh << 16) << 16);
	} else if (outSizeHigh != 0 || (UInt32)(unsigned int)outSize != outSize) {
	    /* unsigned int is a 32 bit uint => We cannot manage files larger than 4GB! */
		TRACE ("LZMA: 64bit support not enabled.\n");
	     return LZMA_RESULT_DATA_ERROR;
	}
#endif

	//TRACE ("LZMA: Uncompresed size............ 0x%lx", outSizeFull);
	//TRACE ("LZMA: Compresed size.............. 0x%lx", compressedSize);
	TRACE (1, "LZMA: Dynamic memory needed....... 0x%x", bes_LzmaGetNumProbs(&state.Properties) * sizeof(CProb));

	state.Probs = (CProb *)nts_malloc(bes_LzmaGetNumProbs(&state.Properties) * sizeof(CProb));
#if 0
	hal_cmu_codec_clock_enable();
    hal_cmu_codec_reset_clear();
	state.Probs = (CProb *)0x40308000;
#endif
	if (state.Probs == 0
	    || (outStream == 0 && outSizeFull != 0)
	    || (inStream == 0 && compressedSize != 0)) {
        if (state.Probs != NULL)
        {
            nts_free(state.Probs);
        }

		TRACE (0, "a7 decom malloc fail");
		return LZMA_RESULT_DATA_ERROR;
	}

	TRACE (0, " allocated.");

	/* Decompress */
	res = bes_LzmaDecode(&state, inStream + LZMA_DATA_OFFSET, compressedSize,
		&inProcessed, outStream, outSizeFull,  &outProcessed);
	if (res != LZMA_RESULT_OK)  {
		TRACE(0, "lzma decompress fail");
		return res;
	}

	*uncompressedSize = outProcessed;
	nts_free(state.Probs);
	//TRACE ("LZMA: Uncompresed end");
	return res;
}
