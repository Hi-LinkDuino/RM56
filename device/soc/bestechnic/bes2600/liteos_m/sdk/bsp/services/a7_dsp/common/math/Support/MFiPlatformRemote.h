#ifndef _MFiServerPlatformRemote_H_
#define _MFiServerPlatformRemote_H_

#include <stdint.h>
#include <arpa/inet.h>
#include "stream_sock.h"

typedef uint32_t MFiAuthFlags;
#ifndef OSStatus
typedef int32_t OSStatus;
#endif


OSStatus MFiPlatform_Initialize(void);
void MFiPlatform_Finalize(void);
OSStatus MFiPlatform_CreateSignatureEx(MFiAuthFlags inFlags, const void* inDataPtr, size_t inDataLen, uint8_t** outSignaturePtr, size_t* outSignatureLen);
OSStatus MFiPlatform_CopyCertificate(uint8_t** outCertificatePtr, size_t* outCertificateLen);

#endif //_MFiServerPlatformRemote_H_