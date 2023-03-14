/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
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

#define HST_LOG_TAG "AACAudioConfigParser"

#include "aac_audio_config_parser.h"
#include "bit_reader.h"
#include "foundation/log.h"

namespace OHOS {
namespace Media {
namespace Plugin {
namespace Ffmpeg {
AACAudioConfigParser::AACAudioConfigParser(const uint8_t* audioConfig, size_t len)
    : bitReader_(audioConfig, len), isConfigValid_(false)
{
}

bool AACAudioConfigParser::ParseConfigs()
{
    isConfigValid_ = ParseObjectTypeFull();
    isConfigValid_ = isConfigValid_ && ParseLevel();
    isConfigValid_ = isConfigValid_ && ParseProfile();
    if (!isConfigValid_) {
        Reset();
        MEDIA_LOG_E("ParseAudioObjectTypeFull failed");
    }
    return isConfigValid_;
}

uint32_t AACAudioConfigParser::GetLevel() const
{
    return audioConfig_.level;
}

AudioAacProfile AACAudioConfigParser::GetProfile() const
{
    return audioConfig_.profile;
}

void AACAudioConfigParser::Reset()
{
    audioConfig_.audioObjectType = 0xFF; // 0xFF
    audioConfig_.channelConfig = 0xFF;   // 0xFF
    audioConfig_.sampleRate = 0;
    audioConfig_.level = 0;
    audioConfig_.profile = AudioAacProfile::NONE;
}

bool AACAudioConfigParser::ParseObjectType()
{
    uint8_t audioObjectType = 0;
    if (!bitReader_.ReadBits(5, audioObjectType)) { // 5
        return false;
    }
    if (audioObjectType == 31) {                        // 31
        if (!bitReader_.ReadBits(6, audioObjectType)) { // 6
            return false;
        }
        audioObjectType += 32; // 32
    }
    audioConfig_.audioObjectType = audioObjectType;
    return true;
}

bool AACAudioConfigParser::ParseSampleRate()
{
    uint8_t sampleFreqIndex = 0;
    uint32_t sampleRate = 0;
    if (!bitReader_.ReadBits(4, sampleFreqIndex)) { // 4
        return false;
    }
    if (sampleFreqIndex == 0xf) {
        if (!bitReader_.ReadBits(24, sampleRate)) { // 24
            return false;
        }
    } else {
        const uint32_t sampleRates[] = {96000, 88200, 64000, 48000, 44100, 32000, 24000,
                                        22050, 16000, 12000, 11025, 8000,  7350};
        if (sampleFreqIndex < sizeof(sampleRates) / sizeof(uint32_t)) {
            sampleRate = sampleRates[sampleFreqIndex];
        } else {
            return false;
        }
    }
    audioConfig_.sampleRate = sampleRate;
    return true;
}

bool AACAudioConfigParser::ParseChannel()
{
    return bitReader_.ReadBits(4, audioConfig_.channelConfig); // 4
}

/**
 * object type, sample rate, channel need to be parsed in the following order.
 * @return TRUE if parse successfully, False otherwise.
 */
bool AACAudioConfigParser::ParseObjectTypeFull()
{
    if (!ParseObjectType() || !ParseSampleRate() || !ParseChannel()) {
        return false;
    }
    // 5 indicates SBR extension (i.e. HE-AAC), 29 indicates PS extension
    if (audioConfig_.audioObjectType == 5 || audioConfig_.audioObjectType == 29) { // 5 29
        return ParseSampleRate() && ParseObjectType();
    }
    return true;
}

/**
 * Extract Number of single channel elements, channel pair elements, low frequency elements,
 * independently switched coupling channel elements, and dependently switched coupling channel
 * elements.
 * Note: The 2 CCE types are ignored for now as they require us to actually
 * parse the first frame, and they are rarely found in actual streams.
 * @param sceCnt number of single channel elements
 * @param cpeCnt number of channel pair elements
 * @param lfeCnt number of low frequency elements
 * @param indepCce number of independently switched coupling channel elements
 * @param depCce number of dependently switched coupling channel elements
 */
bool AACAudioConfigParser::ExtractChannelElements(int& sceCnt, int& cpeCnt, int& lfeCnt, int& indepCce, int& depCce)
{
    indepCce = 0;
    depCce = 0;
    switch (audioConfig_.channelConfig) {
        case 0:
            MEDIA_LOG_W("Found a stream with channel configuration in the "
                        "AudioSpecificConfig. Please file a bug with a link to the media if "
                        "possible.");
            return false;
        case 1: /* front center */
            sceCnt = 1;
            break;
        case 2: // 2 front left and right
            cpeCnt = 1;
            break;
        case 3: // 3 front left, right, and center
            sceCnt = 1;
            cpeCnt = 1;
            break;
        case 4:         // 4 front left, right, and center; rear surround
            sceCnt = 2; // 2
            cpeCnt = 1;
            break;
        case 5: // 5 front left, right, and center; rear left and right surround
            sceCnt = 1;
            cpeCnt = 2; // 2
            break;
        case 6: // 6 front left, right, center and LFE; rear left and right surround
            sceCnt = 1;
            cpeCnt = 2; // 2
            break;
        case 7:  // 7
        case 12: // 12
        case 14: // 14
            /* front left, right, center and LFE; outside front left and right;
             * rear left and right surround */
            sceCnt = 1;
            cpeCnt = 3; // 3
            lfeCnt = 1;
            break;
        case 11:        // 11
            sceCnt = 2; // 2
            cpeCnt = 2; // 2
            lfeCnt = 1;
            break;
        default:
            MEDIA_LOG_W("Unknown channel config in header: " PUBLIC_LOG_D32, audioConfig_.channelConfig);
            return false;
    }
    return true;
}

/**
 * Extract Processor and RAM Complexity Units (calculated and "reference" for single channel)
 * @param refPcu
 * @param refRcu
 * @return
 */
bool AACAudioConfigParser::ExtractReferencePRCU(int& refPcu, int& refRcu)
{
    switch (audioConfig_.audioObjectType) {
        case 0: /* NULL */
            MEDIA_LOG_W("profile 0 is not a valid profile");
            return false;
        case 2:         // 2, LC
            refPcu = 3; // 3
            refRcu = 3; // 3
            break;
        case 3:         // 3, SSR
            refPcu = 4; // 4
            refRcu = 3; // 3
            break;
        case 4:         // 4, LTP
            refPcu = 4; // 4
            refRcu = 4; // 4
            break;
        case 1: /* Main */
        default:
            /* Other than a couple of ER profiles, Main is the worst-case */
            refPcu = 5; // 5
            refRcu = 5; // 5
            break;
    }
    return true;
}

/**
 * calculate profile from channelcnt, pcu, and rcu.
 * @param channelCnt number of channels
 * @param pcu processor complexity unit
 * @param rcu ram complexity unit
 * @return profile calculated result, true or false.
 */
bool AACAudioConfigParser::CalculateProfile(int channelCnt, int pcu, int rcu)
{
#define AAC_PROFILE_VALUE_RANGE(maxChannelCnt, maxSampleRate, maxPcu, maxRcu)                                          \
    channelCnt <= (maxChannelCnt) && audioConfig_.sampleRate <= (maxSampleRate) && pcu <= (maxPcu) && rcu <= (maxRcu)

    int ret = -1;                            // -1
    if (audioConfig_.audioObjectType == 2) { // 2
        /* AAC LC => return the level as per the 'AAC Profile' */
        if (AAC_PROFILE_VALUE_RANGE(2, 24000, 3, 5)) {          // 2 24000 3 5
            ret = 1;                                            // 1
        } else if (AAC_PROFILE_VALUE_RANGE(2, 48000, 6, 5)) {   // 2 48000 6 5
            ret = 2;                                            // 2
        } else if (AAC_PROFILE_VALUE_RANGE(5, 48000, 19, 15)) { // 5 48000 19 15
            ret = 4;                                            // 4
        } else if (AAC_PROFILE_VALUE_RANGE(5, 96000, 38, 15)) { // 5 96000 38 15
            ret = 5;                                            // 5
        } else if (AAC_PROFILE_VALUE_RANGE(7, 48000, 25, 19)) { // 7 48000 25 19
            ret = 6;                                            // 6
        } else if (AAC_PROFILE_VALUE_RANGE(7, 96000, 50, 19)) { // 7 96000 50 19
            ret = 7;                                            // 7
        }
    } else {
        /* Return the level as per the 'Main Profile' */
        if (pcu < 40 && rcu < 20) { // 40 20
            ret = 1;
        } else if (pcu < 80 && rcu < 64) {   // 80 64
            ret = 2;                         // 2
        } else if (pcu < 160 && rcu < 128) { // 160 128
            ret = 3;                         // 3
        } else if (pcu < 320 && rcu < 256) { // 320 256
            ret = 4;                         // 4
        }
    }
    MEDIA_LOG_W("determined level: profile=" PUBLIC_LOG_U8 ", sampleRate=" PUBLIC_LOG_U32 ", channel_config="
                PUBLIC_LOG_U8 ", pcu=" PUBLIC_LOG_D32 ",rcu=" PUBLIC_LOG_D32,
                audioConfig_.audioObjectType, audioConfig_.sampleRate, audioConfig_.channelConfig, pcu, rcu);
    audioConfig_.level = static_cast<uint32_t>(ret);
    return ret != -1;
}

/**
 * Determines the level of a stream as defined in ISO/IEC 14496-3. For AAC LC
 * streams, the constraints from the AAC audio profile are applied. For AAC
 * Main, LTP, SSR and others, the Main profile is used.
 *
 * The @audioConfig parameter follows the following format, starting from the
 * most significant bit of the first byte:
 *
 *   * Bit 0:4 contains the AudioObjectType (if this is 0x5, then the
 *     real AudioObjectType is carried after the rate and channel data)
 *   * Bit 5:8 contains the sample frequency index (if this is 0xf, then the
 *     next 24 bits define the actual sample frequency, and subsequent
 *     fields are appropriately shifted).
 *   * Bit 9:12 contains the channel configuration
 *
 * Returns: TRUE if level can be set, FALSE otherwise.
 */
bool AACAudioConfigParser::ParseLevel()
{
    int sceCnt = 0;
    int cpeCnt = 0;
    int lfeCnt = 0;
    int indepCce = 0;
    int depCce = 0;
    if (!ExtractChannelElements(sceCnt, cpeCnt, lfeCnt, indepCce, depCce)) {
        return false;
    }
    int refPcu = 0;
    int refRcu = 0;
    if (!ExtractReferencePRCU(refPcu, refRcu)) {
        return false;
    }
    int tmp = (static_cast<float>(audioConfig_.sampleRate) / 48000) * refPcu;     // 48000
    int pcu = tmp * ((2 * cpeCnt) + sceCnt + lfeCnt + indepCce + (0.3 * depCce)); // 48000 2 0.3
    int rcu = (static_cast<float>(refRcu)) * (sceCnt + (0.5 * lfeCnt) + (0.5 * indepCce) + (0.4 * depCce)); // 0.5 0.4
    if (cpeCnt < 2) {                                                                                       // 2
        rcu += (refRcu + (refRcu - 1)) * cpeCnt;
    } else {
        rcu += (refRcu + (refRcu - 1) * ((2 * cpeCnt) - 1)); // 2
    }
    int channelCnt = sceCnt + (2 * cpeCnt); // 2
    return CalculateProfile(channelCnt, pcu, rcu);
}

bool AACAudioConfigParser::ParseProfile()
{
    bool ret = true;
    switch (audioConfig_.audioObjectType) {
        case 1:
            audioConfig_.profile = AudioAacProfile::MAIN;
            break;
        case 2: // 2
            audioConfig_.profile = AudioAacProfile::LC;
            break;
        case 3: // 3
            audioConfig_.profile = AudioAacProfile::SSR;
            break;
        case 4: // 4
            audioConfig_.profile = AudioAacProfile::LTP;
            break;
        default:
            audioConfig_.profile = AudioAacProfile::NONE;
            MEDIA_LOG_W("ParseProfile failed due to invalid profile index: " PUBLIC_LOG_U8,
                        audioConfig_.audioObjectType);
            break;
    }
    return ret;
}
} // namespace Ffmpeg
} // namespace Plugin
} // namespace Media
} // namespace OHOS
