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

#include "bluetooth_a2dp_src_observer.h"
#include <hdf_log.h>

void BluetoothA2dpSrcObserver::OnConnectionStateChanged(const bluetooth::RawAddress &device, int state)
{
    HDF_LOGI("BluetoothA2dpSrcObserver::OnConnectionStateChanged");
    if ((callbacks_ != nullptr) && (callbacks_->OnConnectionStateChanged)) {
        callbacks_->OnConnectionStateChanged(device, state);
    }
}

void BluetoothA2dpSrcObserver::OnPlayingStatusChanged(const bluetooth::RawAddress &device, int playingState, int error)
{
    HDF_LOGI("BluetoothA2dpSrcObserver::OnPlayingStatusChanged");
    if ((callbacks_ != nullptr) && (callbacks_->OnPlayingStatusChanged)) {
        callbacks_->OnPlayingStatusChanged(device, playingState, error);
    }
}

void BluetoothA2dpSrcObserver::OnConfigurationChanged
    (const bluetooth::RawAddress &device, const OHOS::Bluetooth::BluetoothA2dpCodecInfo &info, int error)
{
    HDF_LOGI("BluetoothA2dpSrcObserver::OnConfigurationChanged");
    if ((callbacks_ != nullptr) && (callbacks_->OnConfigurationChanged)) {
        callbacks_->OnConfigurationChanged(device, info, error);
    }
}