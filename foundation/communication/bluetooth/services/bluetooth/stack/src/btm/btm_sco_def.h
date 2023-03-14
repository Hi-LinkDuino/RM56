/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef BTM_SCO_DEF_H
#define BTM_SCO_DEF_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BTM_MAX_SCO 7

#define INPUT_OUTPUT_64K_RATE 16000
#define INPUT_OUTPUT_128K_RATE 32000

#define ESCO_DATA_PATH_PCM 1

typedef struct {
    uint8_t codingFormat;
    uint16_t companyID;
    uint16_t vendorSpecificCodecID;
} BtmEscoCodingFormat;

typedef struct {
    BtmEscoCodingFormat transmitCodingFormat;
    BtmEscoCodingFormat receiveCodingFormat;
    uint16_t transmitCodecFrameSize;
    uint16_t receiveCodecFrameSize;
    uint32_t inputBandwidth;
    uint32_t outputBandwidth;
    BtmEscoCodingFormat inputCodingFormat;
    BtmEscoCodingFormat outputCodingFormat;
    uint16_t inputCodedDataSize;
    uint16_t outputCodedDataSize;
    uint8_t inputPCMDataFormat;
    uint8_t outputPCMDataFormat;
    uint8_t inputPCMSamplePayloadMSBPosition;
    uint8_t outputPCMSamplePayloadMSBPosition;
    uint8_t inputDataPath;
    uint8_t outputDataPath;
    uint8_t inputTransportUnitSize;
    uint8_t outputTransportUnitSize;
} BtmEscoParameters;

const BtmEscoParameters *BtmGetEscoParameters(uint8_t codec);

#ifdef __cplusplus
}
#endif

#endif