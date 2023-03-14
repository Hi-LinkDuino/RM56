/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef AUDIO_COMMON_TEST_H
#define AUDIO_COMMON_TEST_H

#include "hdf_types.h"

const int32_t g_testAudioType = 701; // 701 is TEST_AUDIO_TYPE

enum {
    TEST_AUDIOHOSTCREATEANDBIND = 1,        // audio ADM audio_host
    TEST_GETCARDINSTANCE = 2,               // audio ADM audio_host

    TEST_AUDIOFILLCONFIGDATA = 3,           // audio ADM audio_parse
    TEST_CODECGETREGCONFIG = 4,             // audio ADM audio_parse

    TEST_AUDIODEVICEREADREG = 5,            // audio ADM audio_core
    TEST_AUDIODEVICEWRITEREG,               // audio ADM audio_core
    TEST_AUDIOSOCREGISTERPLATFORM,          // audio ADM audio_core
    TEST_AUDIOSOCREGISTERDAI,               // audio ADM audio_core
    TEST_AUDIOREGISTERDSP,                  // audio ADM audio_core
    TEST_AUDIOREGISTERCODEC,                // audio ADM audio_core
    TEST_AUDIOREGISTERACCESSORY,            // audio ADM audio_core
    TEST_AUDIOBINDDAILINK,                  // audio ADM audio_core
    TEST_AUDIOUPDATECODECREGBITS,           // audio ADM audio_core
    TEST_AUDIOUPDATEACCESSORYREGBITS,       // audio ADM audio_core
    TEST_AUDIOUPDATEDAIREGBITS,             // audio ADM audio_core
    TEST_AUDIOKCONTROLGETCPUDAI,            // audio ADM audio_core
    TEST_AUDIOKCONTROLGETCODEC,             // audio ADM audio_core
    TEST_AUDIOKCONTROLGETACCESSORY,         // audio ADM audio_core
    TEST_AUDIOADDCONTROLS,                  // audio ADM audio_core
    TEST_AUDIOADDCONTROL,                   // audio ADM audio_core
    TEST_AUDIOGETCTRLOPSRREG,               // audio ADM audio_core
    TEST_AUDIOGETCTRLOPSREG,                // audio ADM audio_core
    TEST_AUDIOSETCTRLOPSREG,                // audio ADM audio_core
    TEST_AUDIOSETCTRLOPSRREG,               // audio ADM audio_core
    TEST_AUDIODAIREADREG,                   // audio ADM audio_core
    TEST_AUDIODAIWRITEREG,                  // audio ADM audio_core
    TEST_AUDIOCODECREADREG,                 // audio ADM audio_core
    TEST_AUDIOCODECWRITEREG,                // audio ADM audio_core
    TEST_AUDIOACCESSORYREADREG,             // audio ADM audio_core
    TEST_AUDIOACCESSORYWRITEREG,            // audio ADM audio_core
    TEST_AUDIOINFOCTRLOPS,                  // audio ADM audio_core
    TEST_AUDIOCODECGETCTRLOPS,              // audio ADM audio_core
    TEST_AUDIOCODECSETCTRLOPS,              // audio ADM audio_core
    TEST_AUDIOACCESSORYGETCTRLOPS,          // audio ADM audio_core
    TEST_AUDIOACCESSORYSETCTRLOPS,          // audio ADM audio_core
    TEST_AUDIOCPUDAISETCTRLOPS,             // audio ADM audio_core
    TEST_AUDIOCPUDAIGETCTRLOPS = 37,        // audio ADM audio_core

    TEST_AUDIOSAPMNEWCOMPONENTS = 38,       // audio ADM audio_sapm
    TEST_AUDIOSAPMADDROUTES,                // audio ADM audio_sapm
    TEST_AUDIOSAPMNEWCONTROLS,              // audio ADM audio_sapm
    TEST_AUDIOSAPMSLEEP,                    // audio ADM audio_sapm
    TEST_AUDIOSAMPPOWERUP,                  // audio ADM audio_sapm
    TEST_AUDIOSAMPSETPOWERMONITOR,          // audio ADM audio_sapm
    TEST_AUDIOCODECSAPMSETCTRLOPS,          // audio ADM audio_sapm
    TEST_AUDIOCODECSAPMGETCTRLOPS,          // audio ADM audio_sapm
    TEST_AUDIOACCESSORYSAPMSETCTRLOPS,      // audio ADM audio_sapm
    TEST_AUDIOACCESSORYSAPMGETCTRLOPS = 47, // audio ADM audio_sapm
};

#endif /* AUDIO_COMMON_TEST_H */
