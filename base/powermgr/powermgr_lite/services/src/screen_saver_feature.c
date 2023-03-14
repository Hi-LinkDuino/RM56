/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "screen_saver_feature.h"

#include <feature.h>
#include <ohos_init.h>
#include <samgr_lite.h>

#include "hilog_wrapper.h"
#include "power_mgr.h"
#include "screen_saver_intf_define.h"
#include "screen_saver_mgr.h"

const char *GetScreenSaverFeatureName(Feature *feature)
{
    (void)feature;
    return POWER_SCREEN_SAVER_FEATURE;
}

void OnScreenSaverFeatureInitialize(Feature *feature, Service *parent, Identity identity)
{
    (void)parent;
    (void)identity;
    if (feature == NULL) {
        POWER_HILOGE("Invalid feature");
        return;
    }

    ScreenSaverFeature *f = (ScreenSaverFeature *)feature;
    f->identity = identity;
    ScreenSaverMgrInit();
    POWER_HILOGI("Init screen saver feature done");
}

void OnScreenSaverFeatureStop(Feature *feature, Identity identity)
{
    (void)feature;
    (void)identity;
    ScreenSaverFeature *f = GetScreenSaverFeatureImpl();
    if (f != NULL) {
        f->identity.queueId = NULL;
        f->identity.featureId = -1;
        f->identity.serviceId = -1;
    }
}

BOOL OnScreenSaverFeatureMessage(Feature *feature, Request *request)
{
    return ((feature != NULL) && (request != NULL)) ? TRUE : FALSE;
}

int32_t OnSetScreenSaverState(IUnknown *iUnknown, BOOL enable)
{
    (void)iUnknown;
    return ScreenSaverMgrSetState(enable);
}

static void Init(void)
{
    SamgrLite *sam = SAMGR_GetInstance();
    if (sam == NULL) {
        POWER_HILOGE("Failed to get samgr");
        return;
    }

    ScreenSaverFeature *feature = GetScreenSaverFeatureImpl();
    if (feature == NULL) {
        POWER_HILOGE("Failed to get screen saver feature");
        return;
    }
    BOOL result = sam->RegisterFeature(POWER_MANAGE_SERVICE, (Feature *)feature);
    if (result == FALSE) {
        POWER_HILOGE("Failed to register screen saver feature");
        return;
    }
    result = sam->RegisterFeatureApi(POWER_MANAGE_SERVICE, POWER_SCREEN_SAVER_FEATURE, GET_IUNKNOWN(*feature));
    if (result == FALSE) {
        POWER_HILOGE("Failed to register screen saver feature api");
        return;
    }
    POWER_HILOGI("Succeed to register screen saver feature");
}
SYS_FEATURE_INIT(Init);
