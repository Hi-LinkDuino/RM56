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

#ifndef HDI_STREAM_POST_VIEW_H
#define HDI_STREAM_POST_VIEW_H

#include "stream_base.h"

namespace OHOS::Camera {
class StreamPostView : public StreamBase {
public:
    StreamPostView(const int32_t id,
                       const StreamIntent type,
                       std::shared_ptr<IPipelineCore>& p,
                       std::shared_ptr<CaptureMessageOperator>& m);
    virtual ~StreamPostView();
};
} // end namespace OHOS::Camera
#endif // HDI_STREAM_POST_VIEW_H
