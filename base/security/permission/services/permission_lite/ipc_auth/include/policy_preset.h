/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef POLICY_PERSET_H
#define POLICY_PERSET_H

#include "policy_define.h"

FeaturePolicy pmsFeature[] = {
    {
        "PmsFeature",
        {
            {
                .type = RANGE,
                .uidMin = 0,
                .uidMax = __INT_MAX__,
            },
        },
    },
    {
        "PmsInnerFeature",
        {
            {
                .type = RANGE,
                .uidMin = 0,
                .uidMax = 999,
            },
        },
    },
};

FeaturePolicy amsFeature[] = {
    {
        "AmsFeature",
        {
            {
                .type = RANGE,
                .uidMin = 0,
                .uidMax = __INT_MAX__,
            },
        }
    },
    {
        "AmsInnerFeature",
        {
            {
                .type = FIXED,
                .fixedUid = {2}
            },
        }
    },
};

FeaturePolicy bmsFeature[] = {
    {
        "BmsFeature",
        {
            {
                .type = FIXED,
                .fixedUid = {2, 3, 8}
            },
            {
                .type = RANGE,
                .uidMin = 100,
                .uidMax = __INT_MAX__,
            },
        }
    },
    {
        "BmsInnerFeature",
        {
            {
                .type = FIXED,
                .fixedUid = {2, 3, 8}
            },
            {
                .type = RANGE,
                .uidMin = 100,
                .uidMax = 999,
            },
        }
    },
};

FeaturePolicy bdsFeature[] = {
    {
        NULL,
        {
            {
                .type = FIXED,
                .fixedUid = {7}
            },
        }
    },
};

FeaturePolicy dmsFeature[] = {
    {
        "dmslite",
        {
            {
                .type = RANGE,
                .uidMin = 0,
                .uidMax = __INT_MAX__,
            },
        }
    },
};

FeaturePolicy samgrFeature[] = {
    {
        NULL,
        {
            {
                .type = RANGE,
                .uidMin = 0,
                .uidMax = __INT_MAX__,
            },
        }
    },
};

FeaturePolicy appspawnFeature[] = {
    {
        NULL,
        {
            {
                .type = FIXED,
                .fixedUid = {7}
            },
        }
    },
};

FeaturePolicy imsFeature[] = {
    {
        NULL,
        {
            {
                .type = RANGE,
                .uidMin = 100,
                .uidMax = __INT_MAX__,
            },
            {
                .type = FIXED,
                .fixedUid = {2, 7}
            },
        }
    },
};

FeaturePolicy wmsFeature[] = {
    {
        NULL,
        {
            {
                .type = RANGE,
                .uidMin = 100,
                .uidMax = __INT_MAX__,
            },
            {
                .type = FIXED,
                .fixedUid = {2, 7}
            },
        }
    },
};

FeaturePolicy sensorFeature[] = {
    {
        NULL,
        {
            {
                .type = RANGE,
                .uidMin = 0,
                .uidMax = __INT_MAX__,
            },
        }
    },
};

FeaturePolicy aiFeature[] = {
    {
        NULL,
        {
            {
                .type = RANGE,
                .uidMin = 0,
                .uidMax = __INT_MAX__,
            },
        }
    },
};

FeaturePolicy powermgrFeature[] = {
    {
        "screensaver",
        {
            {
                .type = RANGE,
                .uidMin = 0,
                .uidMax = 1000,
            },
        },
    },
    {
        "powermanage",
        {
            {
                .type = RANGE,
                .uidMin = 0,
                .uidMax = __INT_MAX__,
            },
        },
    },
};

FeaturePolicy timertaskFeature[] = {
    {
        "timertask_fea",
        {
            {
                .type = RANGE,
                .uidMin = 0,
                .uidMax = 999,
            },
        },
    },
};

FeaturePolicy softbusFeature[] = {
    {
        NULL,
        {
            {
                .type = RANGE,
                .uidMin = 0,
                .uidMax = __INT_MAX__,
            },
        },
    },
};

FeaturePolicy playerFeature[] = {
    {
        NULL,
        {
            {
                .type = RANGE,
                .uidMin = 0,
                .uidMax = __INT_MAX__,
            },
        },
    },
};

FeaturePolicy cameraFeature[] = {
    {
        NULL,
        {
            {
                .type = RANGE,
                .uidMin = 0,
                .uidMax = __INT_MAX__,
            },
        },
    },
};

FeaturePolicy recorderFeature[] = {
    {
        NULL,
        {
            {
                .type = RANGE,
                .uidMin = 0,
                .uidMax = __INT_MAX__,
            },
        },
    },
};

FeaturePolicy audioCapturerFeature[] = {
    {
        NULL,
        {
            {
                .type = RANGE,
                .uidMin = 0,
                .uidMax = __INT_MAX__,
            },
        },
    },
};

FeaturePolicy devAuthFeature[] = {
    {
        NULL,
        {
            {
                .type = RANGE,
                .uidMin = 0,
                .uidMax = 999,
            },
        },
    },
};

FeaturePolicy batteryFeature[] = {
    {
        "battery_feature",
        {
            {
                .type = RANGE,
                .uidMin = 0,
                .uidMax = __INT_MAX__,
            },
        },
    },
};

FeaturePolicy deviceManagerFeature[] = {
    {
        NULL,
        {
            {
                .type = RANGE,
                .uidMin = 0,
                .uidMax = __INT_MAX__,
            },
        },
    },
};

static PolicySetting g_presetPolicies[] = {
    {"permissionms", pmsFeature, 2},
    {"abilityms", amsFeature, 2},
    {"bundlems", bmsFeature, 2},
    {"dtbschedsrv", dmsFeature, 1},
    {"samgr", samgrFeature, 1},
    {"appspawn", appspawnFeature, 1},
    {"IMS", imsFeature, 1},
    {"WMS", wmsFeature, 1},
    {"bundle_daemon", bdsFeature, 1},
    {"sensor_service", sensorFeature, 1},
    {"ai_service", aiFeature, 1},
    {"powermgr", powermgrFeature, 2},
    {"timertask_srv", timertaskFeature, 1},
    {"softbus_service", softbusFeature, 1},
    {"PlayerServer", playerFeature, 1},
    {"CameraServer", cameraFeature, 1},
    {"RecorderServer", recorderFeature, 1},
    {"AudioCapServer", audioCapturerFeature, 1},
    {"devauth_svc", devAuthFeature, 1},
    {"battery_service", batteryFeature, 1},
    {"dev_mgr_svc", deviceManagerFeature, 1}
};

static int g_presetPolicySize = sizeof(g_presetPolicies) / sizeof(PolicySetting);

#endif // POLICY_PERSET_H
