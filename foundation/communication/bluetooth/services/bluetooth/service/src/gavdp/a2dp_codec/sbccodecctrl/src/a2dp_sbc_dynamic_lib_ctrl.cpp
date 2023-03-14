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

#include "../include/a2dp_sbc_dynamic_lib_ctrl.h"

#include <dlfcn.h>

#include "log.h"

namespace OHOS {
namespace bluetooth {
#define CODEC_LIB_SBC "libbtsbc.z.so"

A2dpSBCDynamicLibCtrl::A2dpSBCDynamicLibCtrl(bool isEncoder)
{
    isEncoder_ = isEncoder;
}

A2dpSBCDynamicLibCtrl::~A2dpSBCDynamicLibCtrl()
{
    isEncoder_ = false;
}

CODECSbcLib *A2dpSBCDynamicLibCtrl::LoadCodecSbcLib() const
{
    CODECSbcLib *lib = new CODECSbcLib();
    if (lib != nullptr) {
        do {
            lib->lib = dlopen(CODEC_LIB_SBC, RTLD_LAZY | RTLD_NODELETE);
            if (lib->lib == nullptr) {
                LOG_ERROR("Load libbtsbc.z.so failed");
                break;
            }
            if (isEncoder_) {
                lib->sbcEncoder.createSbcEncode = (sbc::createSbcEncoder)dlsym(lib->lib, "CreateEncode");
                if (lib->sbcEncoder.createSbcEncode == nullptr) {
                    LOG_ERROR("Load symbol create Sbc Encoder failed");
                }
                lib->sbcEncoder.destroySbcEncode = (sbc::destroySbcEncoder)dlsym(lib->lib, "DestroyEncode");
                if (lib->sbcEncoder.destroySbcEncode == nullptr) {
                    LOG_ERROR("Load symbol destroy Sbc Encoder failed");
                }
            } else {
                lib->sbcDecoder.createSbcDecode = (sbc::createSbcDecoder)dlsym(lib->lib, "CreateDecode");

                if (lib->sbcDecoder.createSbcDecode == nullptr) {
                    LOG_ERROR("Load symbol create Sbc Decoder failed");
                }

                lib->sbcDecoder.destroySbcDecode = (sbc::destroySbcDecoder)dlsym(lib->lib, "DestroyDecode");
                if (lib->sbcDecoder.destroySbcDecode == nullptr) {
                    LOG_ERROR("Load symbol destroy Sbc Decoder failed");
                }
            }
        } while (0);

        if (isEncoder_) {
            if ((lib->lib == nullptr) ||
                (lib->sbcEncoder.createSbcEncode == nullptr) ||
                (lib->sbcEncoder.destroySbcEncode == nullptr)) {
                    delete lib;
                    lib = nullptr;
            }
        } else {
            if ((lib->lib == nullptr) ||
                (lib->sbcDecoder.createSbcDecode == nullptr) ||
                (lib->sbcDecoder.destroySbcDecode == nullptr)) {
                    delete lib;
                    lib = nullptr;
            }
        }
    }
    return lib;
}

void A2dpSBCDynamicLibCtrl::UnloadCodecSbcLib(CODECSbcLib *lib) const
{
    if (isEncoder_) {
        if (lib != nullptr) {
            lib->sbcEncoder.createSbcEncode = nullptr;
            lib->sbcEncoder.destroySbcEncode = nullptr;
            if (lib->lib != nullptr) {
                dlclose(lib->lib);
                lib->lib = nullptr;
            }
            delete lib;
        }
    } else {
        if (lib != nullptr) {
            lib->sbcDecoder.createSbcDecode = nullptr;
            lib->sbcDecoder.destroySbcDecode = nullptr;
            if (lib->lib != nullptr) {
                dlclose(lib->lib);
                lib->lib = nullptr;
            }
            delete lib;
        }
    }
}
}  // namespace bluetooth
}  // namespace OHOS