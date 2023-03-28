#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lzma_decode.h"
#include "hal_trace.h"
#include "hal_cmu.h"

extern uint32_t __lzma_test_bin_start[];
extern uint32_t __lzma_test_bin_end[];

#define LZMA_PROPERTIES_OFFSET 0
#define LZMA_SIZE_OFFSET       LZMA_PROPERTIES_SIZE
//#define LZMA_DATA_OFFSET       LZMA_SIZE_OFFSET + sizeof(uint64_t)
#define LZMA_DATA_OFFSET       LZMA_SIZE_OFFSET + 8
#define LZMA_MEM_POOL_SIZE  (16*1024)
uint32_t lzma_mem_pool[LZMA_MEM_POOL_SIZE/sizeof(uint32_t)];

int lzma_decom(unsigned char *outStream, unsigned int *uncompressedSize,
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
	if (inStream == NULL) {
		return LZMA_RESULT_DATA_ERROR;
	}
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
    ASSERT((bes_LzmaGetNumProbs(&state.Properties) * sizeof(CProb) <= LZMA_MEM_POOL_SIZE), "LZMA mem pool not enough!");

	state.Probs = (CProb *)lzma_mem_pool;
	if (state.Probs == 0
	    || (outStream == 0 && outSizeFull != 0)
	    || (inStream == 0 && compressedSize != 0)) {
		TRACE (0, "lzma data error");
		return LZMA_RESULT_DATA_ERROR;
	}

	/* Decompress */
	res = bes_LzmaDecode(&state, inStream + LZMA_DATA_OFFSET, compressedSize,
		&inProcessed, outStream, outSizeFull,  &outProcessed);
	if (res != LZMA_RESULT_OK)  {
		TRACE(0, "lzma decompress fail");
		return res;
	}

	*uncompressedSize = outProcessed;
	//TRACE ("LZMA: Uncompresed end");
	return res;
}

/* the length of decom_out should be at least 0x10000 */
int decom_test(unsigned char * decom_out)
{
	unsigned int uncompressed_size = 0;
	unsigned int size;
	int ret = 0;

	size = (uint32_t)__lzma_test_bin_end - (uint32_t)__lzma_test_bin_start;
	TRACE(0, "decom_test size %d", size);
	ret = lzma_decom(decom_out, &uncompressed_size, (unsigned char *)__lzma_test_bin_start, size);
	TRACE(0, "decom_test ret %d", ret);
	return ret;
}
