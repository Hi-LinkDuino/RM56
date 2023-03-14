/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdmi_ncts.h"

#define HDF_LOG_TAG hdmi_ncts_c
#define HDMI_NCTS_KHZ 1000
#define HDMI_NCTS_TMDS_PER_PIXEL 10

#define HDMI_NCTS_INVALID_VALUE 0xffffffff

/*
 * see hdmi1.4 table 7-1, 7-2, 7-3.
 * The exact relationship: 128 * fs = tmdsClock * N / CTS.
 */
static struct HdmiAudioNCts g_audioNctsMap[] = {
    { 32000, 25174, 4576, 28125 },
    { 32000, 25200, 4096, 25200 },
    { 32000, 27000, 4096, 27000 },
    { 32000, 27027, 4096, 27027 },
    { 32000, 54000, 4096, 54000 },
    { 32000, 54054, 4096, 54054 },
    { 32000, 74175, 11648, 210937},
    { 32000, 74250, 4096, 74250 },
    { 32000, 148351, 11648, 421875 },
    { 32000, 148500, 4096, 148500 },
    { 32000, 296703, 5824, 421875 },
    { 32000, 297000, 3072, 222750 },
    { 32000, 593406, 5824, 843750 },
    { 32000, 594000, 3072, 445500 },
    { 32000, 0,      4096, 0 },
    { 44100, 25174, 7007, 31250 },
    { 44100, 25200, 6272, 28000 },
    { 44100, 27000, 6272, 30000 },
    { 44100, 27027, 6272, 30030 },
    { 44100, 54000, 6272, 60000 },
    { 44100, 54054, 6272, 60060 },
    { 44100, 74175, 17836, 234375 },
    { 44100, 74250, 6272, 82500 },
    { 44100, 148351, 8918, 234375 },
    { 44100, 148500, 6272, 165000 },
    { 44100, 296703, 4459, 234375 },
    { 44100, 297000, 4704, 247500 },
    { 44100, 593406, 8918, 937500 },
    { 44100, 594000, 9408, 990000 },
    { 44100, 0,      6272, 0 },
    { 48000, 25174, 6864, 28125 },
    { 48000, 25200, 6144, 25200 },
    { 48000, 27000, 6144, 27000 },
    { 48000, 27027, 6144, 27027 },
    { 48000, 54000, 6144, 54000 },
    { 48000, 54054, 6144, 54054 },
    { 48000, 74175, 11648, 140625 },
    { 48000, 74250, 6144, 74250 },
    { 48000, 148351, 5824, 140625 },
    { 48000, 148500, 6144, 148500 },
    { 48000, 296703, 5824, 281250 },
    { 48000, 297000, 5120, 247500 },
    { 48000, 593406, 5824, 562500 },
    { 48000, 594000, 6144, 594000 },
    { 48000, 0,      6144, 0 }
    };

uint32_t HdmiGetN(uint32_t sampleRate, uint32_t tmdsClock)
{
    uint32_t i;

    for (i = 0; i < sizeof(g_audioNctsMap) / sizeof(g_audioNctsMap[0]); i++) {
        if (g_audioNctsMap[i].sampleRate == sampleRate &&
            g_audioNctsMap[i].tmdsClock == tmdsClock) {
            return g_audioNctsMap[i].n;
        }
        if (g_audioNctsMap[i].sampleRate == sampleRate &&
            g_audioNctsMap[i].tmdsClock >= (tmdsClock - HDMI_NCTS_TMDS_DEVIATION) &&
            g_audioNctsMap[i].tmdsClock <= (tmdsClock + HDMI_NCTS_TMDS_DEVIATION)) {
            return g_audioNctsMap[i].n;
        }
        if (g_audioNctsMap[i].sampleRate == sampleRate &&
            g_audioNctsMap[i].tmdsClock == 0) {
            return g_audioNctsMap[i].n;
        }
    }
    return HDMI_NCTS_N_DEFAULT;
}

uint32_t HdmiGetCts(uint32_t sampleRate, uint32_t tmdsClock)
{
    uint32_t i;
    uint32_t tmpN = HDMI_NCTS_INVALID_VALUE;
    uint32_t tmpCts = HDMI_NCTS_INVALID_VALUE;

    for (i = 0; i < sizeof(g_audioNctsMap) / sizeof(g_audioNctsMap[0]); i++) {
        if (g_audioNctsMap[i].sampleRate == sampleRate &&
            g_audioNctsMap[i].tmdsClock == tmdsClock) {
            tmpN = g_audioNctsMap[i].n;
            tmpCts = g_audioNctsMap[i].cts;
            break;
        }
        if (g_audioNctsMap[i].sampleRate == sampleRate &&
            g_audioNctsMap[i].tmdsClock >= (tmdsClock - HDMI_NCTS_TMDS_DEVIATION) &&
            g_audioNctsMap[i].tmdsClock <= (tmdsClock + HDMI_NCTS_TMDS_DEVIATION)) {
            tmpN = g_audioNctsMap[i].n;
            tmpCts = g_audioNctsMap[i].cts;
            break;
        }
        if (g_audioNctsMap[i].sampleRate == sampleRate &&
            g_audioNctsMap[i].tmdsClock == 0) {
            tmpN = g_audioNctsMap[i].n;
            tmpCts = g_audioNctsMap[i].cts;
            break;
        }
    }

    if (tmpCts == 0 && sampleRate >= HDMI_NCTS_KHZ) {
        tmpCts = (tmpN / HDMI_NCTS_FACTOR)  * tmdsClock * HDMI_NCTS_TMDS_PER_PIXEL / (sampleRate / HDMI_NCTS_KHZ);
    } else if (tmpCts == HDMI_NCTS_INVALID_VALUE && sampleRate >= HDMI_NCTS_KHZ) {
        tmpCts = (HDMI_NCTS_N_DEFAULT / HDMI_NCTS_FACTOR)  * tmdsClock * HDMI_NCTS_TMDS_PER_PIXEL /
            (sampleRate / HDMI_NCTS_KHZ);
    }
    return tmpCts;
}
