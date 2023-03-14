/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef POWERMGR_SCREEN_SAVER_FEATURE_H
#define POWERMGR_SCREEN_SAVER_FEATURE_H

#include <feature.h>

#include "screen_saver_interface.h"
#include "screen_saver_mgr.h"

typedef struct {
    INHERIT_FEATURE;
    INHERIT_IUNKNOWNENTRY(ScreenSaverInterface);
    Identity identity;
} ScreenSaverFeature;

const char *GetScreenSaverFeatureName(Feature *feature);
void OnScreenSaverFeatureInitialize(Feature *feature, Service *parent, Identity identity);
void OnScreenSaverFeatureStop(Feature *feature, Identity identity);
BOOL OnScreenSaverFeatureMessage(Feature *feature, Request *request);
int32_t OnSetScreenSaverState(IUnknown *iUnknown, BOOL enable);

#define SCREEN_SAVER_FEATURE_INTERFACE_IMPL                         \
    .GetName = GetScreenSaverFeatureName,                                      \
    .OnInitialize = OnScreenSaverFeatureInitialize,                            \
    .OnStop = OnScreenSaverFeatureStop,                                        \
    .OnMessage = OnScreenSaverFeatureMessage

#define SCREEN_SAVER_INTERFACE_IMPL                                 \
    .SetScreenSaverStateFunc = OnSetScreenSaverState

ScreenSaverFeature *GetScreenSaverFeatureImpl(void);

#endif // POWERMGR_SCREEN_SAVER_FEATURE_H
