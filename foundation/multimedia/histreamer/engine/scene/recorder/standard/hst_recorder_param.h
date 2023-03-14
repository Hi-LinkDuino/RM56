/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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

#ifndef HISTREAMER_SCENE_RECORDER_STD_HST_REC_PARAM_H
#define HISTREAMER_SCENE_RECORDER_STD_HST_REC_PARAM_H
#include "recorder_param.h"
#include "plugin/common/plugin_tags.h"
namespace OHOS {
namespace Media {
namespace Record {
struct HstRecParam {
    int32_t srcId {-1};
    RecorderPublicParamType stdParamType {};
    Plugin::ValueType val;
    template<typename T>
    inline const T* GetValPtr() const
    {
        if (!val.SameTypeWith(typeid(T))) {
            return nullptr;
        }
        return Plugin::AnyCast<T>(&val);
    }
};
} // Record
} // Media
} // OHOS
#endif // HISTREAMER_SCENE_RECORDER_STD_HST_REC_PARAM_H
