/*
 * Copyright (c) 2021 Rockchip Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "config.h"
static const struct RouteCfgInfo default_speaker_normal_controls[] = {
    {
        .controlName = "Playback Path",
        .stringVal = "SPK",
    },
};

static const struct RouteCfgInfo default_headphone_normal_controls[] = {
    {
        .controlName = "Playback Path",
        .stringVal = "HP_NO_MIC",
    },
};

static const struct RouteCfgInfo default_speaker_headphone_normal_controls[] = {
    {
        .controlName = "Playback Path",
        .stringVal = "SPK_HP",
    },
};

static const struct RouteCfgInfo default_main_mic_capture_controls[] = {
    {
        .controlName = "Capture MIC Path",
        .stringVal = "Main Mic",
    },
};

static const struct RouteCfgInfo default_hands_free_mic_capture_controls[] = {
    {
        .controlName = "Capture MIC Path",
        .stringVal = "Hands Free Mic",
    },
};

static const struct RouteCfgInfo default_playback_off_controls[] = {
    {
        .controlName = "Playback Path",
        .stringVal = "OFF",
    },
};

static const struct RouteCfgInfo default_capture_off_controls[] = {
    {
        .controlName = "Capture MIC Path",
        .stringVal = "MIC OFF",
    },
};

static struct PathRouteCfgTable default_config_table = {
    // speaker
    .speakNormal = {
        .sndCard = 1,
        .devices = DEVICES_0,
        .controls = default_speaker_normal_controls,
        .ctlsNums = sizeof(default_speaker_normal_controls) / sizeof(struct RouteCfgInfo),
    },

    // headphone
    .headphoneNormal = {
        .sndCard = 1,
        .devices = DEVICES_0,
        .controls = default_headphone_normal_controls,
        .ctlsNums = sizeof(default_headphone_normal_controls) / sizeof(struct RouteCfgInfo),
    },

    .speakerHeadphoneNormal = {
        .sndCard = 1,
        .devices = DEVICES_0,
        .controls = default_speaker_headphone_normal_controls,
        .ctlsNums = sizeof(default_speaker_headphone_normal_controls) / sizeof(struct RouteCfgInfo),
    },

    // capture
    .mainMicCapture = {
        .sndCard = 1,
        .devices = DEVICES_0,
        .controls = default_main_mic_capture_controls,
        .ctlsNums = sizeof(default_main_mic_capture_controls) / sizeof(struct RouteCfgInfo),
    },
    .handsFreeMicCapture = {
        .sndCard = 1,
        .devices = DEVICES_0,
        .controls = default_hands_free_mic_capture_controls,
        .ctlsNums = sizeof(default_hands_free_mic_capture_controls) / sizeof(struct RouteCfgInfo),
    },

    // off
    .playbackOff = {
        .controls = default_playback_off_controls,
        .ctlsNums = sizeof(default_playback_off_controls) / sizeof(struct RouteCfgInfo),
    },
    .captureOff = {
        .controls = default_capture_off_controls,
        .ctlsNums = sizeof(default_capture_off_controls) / sizeof(struct RouteCfgInfo),
    },

#ifdef BOX_HAL
    // hdmi
    .HdmiNormal = {
        .sndCard = 0,
        .devices = DEVICES_0,
        .ctlsNums = 0,
    },
#else
    // hdmi
    .HdmiNormal = {
        .sndCard = 0,
        .devices = DEVICES_0,
        .ctlsNums = 0,
    },
#endif
};

static const struct RouteCfgInfo hdmi_analog_speaker_normal_controls[] = {
    {
        .controlName = "DAC Playback Volume",
        .intVal = {192, 192},
    },
};

static const struct RouteCfgInfo hdmi_analog_headphone_normal_controls[] = {
    {
        .controlName = "DAC Playback Volume",
        .intVal = {192, 192},
    },
};

static const struct RouteCfgInfo hdmi_analog_hdmi_normal_controls[] = {
    {
        .controlName = "DAC Playback Volume",
        .intVal = {0, 0},
    },
};

static const struct RouteCfgInfo hdmi_analog_speaker_headphone_normal_controls[] = {
    {
        .controlName = "DAC Playback Volume",
        .intVal = {192, 192},
    },
};

static const struct RouteCfgInfo hdmi_analog_main_mic_capture_controls[] = {
    {
        .controlName = "Differential Mux",
        .intVal = {0},
    },
};

static const struct RouteCfgInfo hdmi_analog_hands_free_mic_capture_controls[] = {
    {
        .controlName = "Differential Mux",
        .intVal = {1},
    },
};

static const struct RouteCfgInfo hdmi_analog_playback_off_controls[] = {
    {
        .controlName = "Playback Path",
        .stringVal = "OFF",
    },
};

static const struct RouteCfgInfo hdmi_analog_capture_off_controls[] = {
    {
        .controlName = "Capture MIC Path",
        .stringVal = "MIC OFF",
    },
};

static struct PathRouteCfgTable hdmi_analog_config_table = {
    // speaker
    .speakNormal = {
        .sndCard = 0,
        .devices = DEVICES_0,
        .controls = hdmi_analog_speaker_normal_controls,
        .ctlsNums = sizeof(hdmi_analog_speaker_normal_controls) / sizeof(struct RouteCfgInfo),
    },

    // headphone
    .headphoneNormal = {
        .sndCard = 0,
        .devices = DEVICES_0,
        .controls = hdmi_analog_headphone_normal_controls,
        .ctlsNums = sizeof(hdmi_analog_headphone_normal_controls) / sizeof(struct RouteCfgInfo),
    },

    .speakerHeadphoneNormal = {
        .sndCard = 0,
        .devices = DEVICES_0,
        .controls = hdmi_analog_speaker_headphone_normal_controls,
        .ctlsNums = sizeof(hdmi_analog_speaker_headphone_normal_controls) / sizeof(struct RouteCfgInfo),
    },

    // capture
    .mainMicCapture = {
        .sndCard = 0,
        .devices = DEVICES_0,
        .controls = hdmi_analog_main_mic_capture_controls,
        .ctlsNums = sizeof(hdmi_analog_main_mic_capture_controls) / sizeof(struct RouteCfgInfo),
    },
    .handsFreeMicCapture = {
        .sndCard = 0,
        .devices = DEVICES_0,
        .controls = hdmi_analog_hands_free_mic_capture_controls,
        .ctlsNums = sizeof(hdmi_analog_hands_free_mic_capture_controls) / sizeof(struct RouteCfgInfo),
    },

    // off
    .playbackOff = {
        .controls = hdmi_analog_playback_off_controls,
        .ctlsNums = sizeof(hdmi_analog_playback_off_controls) / sizeof(struct RouteCfgInfo),
    },
    .captureOff = {
        .controls = hdmi_analog_capture_off_controls,
        .ctlsNums = sizeof(hdmi_analog_capture_off_controls) / sizeof(struct RouteCfgInfo),
    },
#ifdef BOX_HAL
    // hdmi
    .HdmiNormal = {
        .sndCard = 0,
        .devices = DEVICES_0,
        .ctlsNums = 0,
    },
#else
    // hdmi
    .HdmiNormal = {
        .sndCard = 0,
        .devices = DEVICES_0,
        .controls = hdmi_analog_hdmi_normal_controls,
        .ctlsNums = sizeof(hdmi_analog_hdmi_normal_controls) / sizeof(struct RouteCfgInfo),
    },
#endif
};

struct PathRouteCfgTable* GetHdmiConfigTable(void)
{
    return &hdmi_analog_config_table;
}

struct PathRouteCfgTable* GetDefaultConfigTable(void)
{
    return &default_config_table;
}
