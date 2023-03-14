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

#include "btm_sco_def.h"

#include <stddef.h>

#include "hci/hci_def.h"

#define CODEC_FRAME_SIZE 60

#define CODED_DATA_SIZE 16

#define ESCO_PARAMETERS_TABLE_SIZE 3

static BtmEscoParameters g_escoParametersTable[ESCO_PARAMETERS_TABLE_SIZE] = {
    // CVSD
    {
        .transmitCodingFormat =
            {
                .codingFormat = HCI_CODING_FORMAT_CVSD,
                .companyID = 0x0000,
                .vendorSpecificCodecID = 0x0000,
            },
        .receiveCodingFormat =
            {
                .codingFormat = HCI_CODING_FORMAT_CVSD,
                .companyID = 0x0000,
                .vendorSpecificCodecID = 0x0000,
            },
        .transmitCodecFrameSize = CODEC_FRAME_SIZE,
        .receiveCodecFrameSize = CODEC_FRAME_SIZE,
        .inputBandwidth = INPUT_OUTPUT_64K_RATE,
        .outputBandwidth = INPUT_OUTPUT_64K_RATE,
        .inputCodingFormat =
            {
                .codingFormat = HCI_CODING_FORMAT_LINEAR_PCM,
                .companyID = 0x0000,
                .vendorSpecificCodecID = 0x0000,
            },
        .outputCodingFormat =
            {
                .codingFormat = HCI_CODING_FORMAT_LINEAR_PCM,
                .companyID = 0x0000,
                .vendorSpecificCodecID = 0x0000,
            },
        .inputCodedDataSize = CODED_DATA_SIZE,
        .outputCodedDataSize = CODED_DATA_SIZE,
        .inputPCMDataFormat = HCI_PCM_DATA_FORMAT_2_COMPLEMENT,
        .outputPCMDataFormat = HCI_PCM_DATA_FORMAT_2_COMPLEMENT,
        .inputPCMSamplePayloadMSBPosition = 0,
        .outputPCMSamplePayloadMSBPosition = 0,
        .inputDataPath = ESCO_DATA_PATH_PCM,
        .outputDataPath = ESCO_DATA_PATH_PCM,
        .inputTransportUnitSize = 0x00,
        .outputTransportUnitSize = 0x00,
    },
    // MSBC T1
    {
        .transmitCodingFormat =
            {
                .codingFormat = HCI_CODING_FORMAT_MSBC,
                .companyID = 0x0000,
                .vendorSpecificCodecID = 0x0000,
            },
        .receiveCodingFormat =
            {
                .codingFormat = HCI_CODING_FORMAT_MSBC,
                .companyID = 0x0000,
                .vendorSpecificCodecID = 0x0000,
            },
        .transmitCodecFrameSize = CODEC_FRAME_SIZE,
        .receiveCodecFrameSize = CODEC_FRAME_SIZE,
        .inputBandwidth = INPUT_OUTPUT_128K_RATE,
        .outputBandwidth = INPUT_OUTPUT_128K_RATE,
        .inputCodingFormat =
            {
                .codingFormat = HCI_CODING_FORMAT_LINEAR_PCM,
                .companyID = 0x0000,
                .vendorSpecificCodecID = 0x0000,
            },
        .outputCodingFormat =
            {
                .codingFormat = HCI_CODING_FORMAT_LINEAR_PCM,
                .companyID = 0x0000,
                .vendorSpecificCodecID = 0x0000,
            },
        .inputCodedDataSize = CODED_DATA_SIZE,
        .outputCodedDataSize = CODED_DATA_SIZE,
        .inputPCMDataFormat = HCI_PCM_DATA_FORMAT_2_COMPLEMENT,
        .outputPCMDataFormat = HCI_PCM_DATA_FORMAT_2_COMPLEMENT,
        .inputPCMSamplePayloadMSBPosition = 0,
        .outputPCMSamplePayloadMSBPosition = 0,
        .inputDataPath = ESCO_DATA_PATH_PCM,
        .outputDataPath = ESCO_DATA_PATH_PCM,
        .inputTransportUnitSize = 0x00,
        .outputTransportUnitSize = 0x00,
    },
    // MSBC T2
    {
        .transmitCodingFormat =
            {
                .codingFormat = HCI_CODING_FORMAT_MSBC,
                .companyID = 0x0000,
                .vendorSpecificCodecID = 0x0000,
            },
        .receiveCodingFormat =
            {
                .codingFormat = HCI_CODING_FORMAT_MSBC,
                .companyID = 0x0000,
                .vendorSpecificCodecID = 0x0000,
            },
        .transmitCodecFrameSize = CODEC_FRAME_SIZE,
        .receiveCodecFrameSize = CODEC_FRAME_SIZE,
        .inputBandwidth = INPUT_OUTPUT_128K_RATE,
        .outputBandwidth = INPUT_OUTPUT_128K_RATE,
        .inputCodingFormat =
            {
                .codingFormat = HCI_CODING_FORMAT_LINEAR_PCM,
                .companyID = 0x0000,
                .vendorSpecificCodecID = 0x0000,
            },
        .outputCodingFormat =
            {
                .codingFormat = HCI_CODING_FORMAT_LINEAR_PCM,
                .companyID = 0x0000,
                .vendorSpecificCodecID = 0x0000,
            },
        .inputCodedDataSize = CODED_DATA_SIZE,
        .outputCodedDataSize = CODED_DATA_SIZE,
        .inputPCMDataFormat = HCI_PCM_DATA_FORMAT_2_COMPLEMENT,
        .outputPCMDataFormat = HCI_PCM_DATA_FORMAT_2_COMPLEMENT,
        .inputPCMSamplePayloadMSBPosition = 0,
        .outputPCMSamplePayloadMSBPosition = 0,
        .inputDataPath = ESCO_DATA_PATH_PCM,
        .outputDataPath = ESCO_DATA_PATH_PCM,
        .inputTransportUnitSize = 0x00,
        .outputTransportUnitSize = 0x00,
    },
};

const BtmEscoParameters *BtmGetEscoParameters(uint8_t codec)
{
    BtmEscoParameters *parameters = NULL;
    if (codec < ESCO_PARAMETERS_TABLE_SIZE) {
        parameters = g_escoParametersTable + codec;
    }
    return parameters;
}