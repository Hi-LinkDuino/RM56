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

#include "hdi_wrapper.h"

#include <dlfcn.h>

#include "log.h"
#include "platform/include/allocator.h"

#ifdef __x86_64__
#define HDI_LIB "libbluetooth_hdi_adapter.so"
#else
#define HDI_LIB "libbluetooth_hdi_adapter.z.so"
#endif

HDILib *LoadHdiLib()
{
    HDILib *lib = MEM_CALLOC.alloc(sizeof(HDILib));
    if (lib != NULL) {
        do {
            lib->lib = dlopen(HDI_LIB, RTLD_LAZY | RTLD_NODELETE);
            if (lib->lib == NULL) {
                LOG_ERROR("Load libbluetooth_hdi_adapter.so failed, %{public}s", dlerror());
                break;
            }

            lib->hdiInit = dlsym(lib->lib, "HdiInit");
            if (lib->hdiInit == NULL) {
                LOG_ERROR("Load symbol HdiInit failed");
            }

            lib->hdiSendHciPacket = dlsym(lib->lib, "HdiSendHciPacket");
            if (lib->hdiSendHciPacket == NULL) {
                LOG_ERROR("Load symbol HdiSendHciPacket failed");
            }

            lib->hdiClose = dlsym(lib->lib, "HdiClose");
            if (lib->hdiClose == NULL) {
                LOG_ERROR("Load symbol HdiClose failed");
            }
        } while (0);

        if ((lib->lib == NULL) ||
            (lib->hdiInit == NULL) ||
            (lib->hdiSendHciPacket == NULL) ||
            (lib->hdiClose == NULL)) {
            MEM_CALLOC.free(lib);
            lib = NULL;
        }
    }
    return lib;
}

void UnloadHdiLib(HDILib *lib)
{
    if (lib != NULL) {
        lib->hdiInit = NULL;
        lib->hdiSendHciPacket = NULL;
        lib->hdiClose = NULL;
        if (lib->lib != NULL) {
            dlclose(lib->lib);
            lib->lib = NULL;
        }
        MEM_CALLOC.free(lib);
    }
}