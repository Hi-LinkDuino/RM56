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

#ifndef POWERMGR_POWER_MANAGE_FEATURE_H
#define POWERMGR_POWER_MANAGE_FEATURE_H

#include <feature.h>

#include "power_manage_interface.h"
#include "running_lock_mgr.h"

typedef struct {
    INHERIT_FEATURE;
    INHERIT_IUNKNOWNENTRY(PowerManageInterface);
    Identity identity;
} PowerManageFeature;

const char *GetPowerManageFeatureName(Feature *feature);
void OnPowerManageFeatureInitialize(Feature *feature, Service *parent, Identity identity);
void OnPowerManageFeatureStop(Feature *feature, Identity identity);
BOOL OnPowerManageFeatureMessage(Feature *feature, Request *request);
int32_t OnAcquireRunningLockEntry(IUnknown *iUnknown, RunningLockEntry *entry, int32_t timeoutMs);
int32_t OnReleaseRunningLockEntry(IUnknown *iUnknown, RunningLockEntry *entry);
BOOL OnIsAnyRunningLockHolding(IUnknown *iUnknown);
void OnSuspendDevice(IUnknown *iUnknown, SuspendDeviceType reason, BOOL suspendImmed);
void OnWakeupDevice(IUnknown *iUnknown, WakeupDeviceType reason, const char* details);

#define POWER_MANAGE_FEATURE_INTERFACE_IMPL             \
    .GetName = GetPowerManageFeatureName,               \
    .OnInitialize = OnPowerManageFeatureInitialize,     \
    .OnStop = OnPowerManageFeatureStop,                 \
    .OnMessage = OnPowerManageFeatureMessage

#define POWER_MANAGE_INTERFACE_IMPL                                 \
    .AcquireRunningLockEntryFunc = OnAcquireRunningLockEntry,       \
    .ReleaseRunningLockEntryFunc = OnReleaseRunningLockEntry,       \
    .IsAnyRunningLockHoldingFunc = OnIsAnyRunningLockHolding,       \
    .SuspendDeviceFunc = OnSuspendDevice,                           \
    .WakeupDeviceFunc = OnWakeupDevice

PowerManageFeature *GetPowerManageFeatureImpl(void);

#endif // POWERMGR_POWER_MANAGE_FEATURE_H
