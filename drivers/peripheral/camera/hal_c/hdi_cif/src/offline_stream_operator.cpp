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

#include "offline_stream_operator.h"

namespace OHOS::Camera {

OfflineStreamOperator::OfflineStreamOperator()
{
}

OfflineStreamOperator::~OfflineStreamOperator()
{
}

void OfflineStreamOperator::Init(OfflineStreamOperatorCIF* op)
{
    operator_ = op;
}

CamRetCode OfflineStreamOperator::CancelCapture(int captureId)
{
    if (operator_ == nullptr) {
        return INSUFFICIENT_RESOURCES;
    }
    int ret = operator_->CancelCapture(captureId);

    return static_cast<CamRetCode>(ret);
}

CamRetCode OfflineStreamOperator::ReleaseStreams(const std::vector<int>& streamIds)
{
    if (operator_ == nullptr) {
        return INSUFFICIENT_RESOURCES;
    }

    int count = streamIds.size();
    if (count <= 0) {
        return DEVICE_ERROR;
    }
    int* ids = new int[count];
    if (ids == nullptr) {
        return INSUFFICIENT_RESOURCES;
    }

    int ret = operator_->ReleaseStreams(ids, count);

    if (ids != nullptr) {
        delete [] ids;
    }

    return static_cast<CamRetCode>(ret);
}

CamRetCode OfflineStreamOperator::Release()
{
    if (operator_ == nullptr) {
        return INSUFFICIENT_RESOURCES;
    }

    int ret = operator_->Release();

    return static_cast<CamRetCode>(ret);
}

} // end namespace OHOS::Camera
