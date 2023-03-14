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

#include "frameworks/bridge/common/dom/dom_camera.h"

#include "frameworks/bridge/common/dom/dom_type.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {
namespace {

const char CALLBACK_SUCCESS[] = "success";
const char CALLBACK_FAIL[] = "fail";
const char CALLBACK_COMPLETE[] = "complete";
const char QUALITY[] = "quality";
const char DEVICE_POSITION_FRONT[] = "front";
const char FLUSH_ON[] = "on";
const char FLUSH_OFF[] = "off";
const char FLUSH_TORCH[] = "torch";
const char FLUSH_AUTO[] = "auto";
const char START_STR[] = "[\"";

} // namespace

DOMCamera::DOMCamera(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName)
{
    cameraComponent_ = AceType::MakeRefPtr<CameraComponent>();
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    if (IsRightToLeft()) {
        cameraComponent_->SetTextDirection(TextDirection::RTL);
    }
#endif
}

bool DOMCamera::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    // Operator map for attr
    static const std::unordered_map<std::string, void (*)(const RefPtr<CameraComponent>&, const std::string&)>
        attrOperators = {
            { DOM_CAMERA_FLASH,
                [](const RefPtr<CameraComponent>& cameraComponent, const std::string& val) {
                    cameraComponent->SetFlash(GetFlashType(val));
                } },
            { DOM_CAMERA_DEVICE_POSITION,
                [](const RefPtr<CameraComponent>& cameraComponent, const std::string& val) {
                    cameraComponent->SetDevicePosition(GetDevicePosition(val));
                } },
        };
    auto operatorIter = attrOperators.find(attr.first);
    if (operatorIter != attrOperators.end()) {
        operatorIter->second(cameraComponent_, attr.second);
        return true;
    }
    return false;
}

bool DOMCamera::AddSpecializedEvent(int32_t pageId, const std::string& event)
{
    // Operator map for event
    static const std::unordered_map<std::string, void (*)(const RefPtr<CameraComponent>&, const EventMarker&)>
        eventOperators = {
            { DOM_ERROR,
                [](const RefPtr<CameraComponent>& cameraComponent, const EventMarker& event) {
                    cameraComponent->SetErrorEventId(event);
                } },
        };
    auto operatorIter = eventOperators.find(event);
    if (operatorIter != eventOperators.end()) {
        operatorIter->second(cameraComponent_, EventMarker(GetNodeIdForEvent(), event, pageId));
        return true;
    }
    return false;
}

void DOMCamera::CallSpecializedMethod(const std::string& method, const std::string& args)
{
    LOGI("DOMCamera: method: %{public}s args: %{public}s", method.c_str(), args.c_str());
    if (method == DOM_TAKE_PHOTO) {
        auto controller = cameraComponent_->GetCameraController();
        if (controller) {
            controller->TakePhoto(GetParamFromJson(args));
        }
    }

    if (method == DOM_CAMERA_START_RECORD) {
        auto controller = cameraComponent_->GetCameraController();
        if (controller) {
            controller->StartRecord();
        }
    }

    if (method == DOM_CAMERA_CLOSE_RECORDER) {
        auto controller = cameraComponent_->GetCameraController();
        if (controller) {
            controller->CloseRecorder(GetRecorderParam(args));
        }
    }
}

TakePhotoParams DOMCamera::GetParamFromJson(const std::string& args) const
{
    std::unique_ptr<JsonValue> argsValue = JsonUtil::ParseJsonString(args);

    TakePhotoParams takePhotoParams;
    if (!argsValue || !argsValue->IsArray() || argsValue->GetArraySize() == 0) {
        LOGE("parse args error");
        return takePhotoParams;
    }
    std::unique_ptr<JsonValue> firstItem = argsValue->GetArrayItem(0);
    std::string quality = firstItem->GetString(QUALITY);
    if (!quality.empty()) {
        takePhotoParams.quality = quality;
    }

    std::string successId = firstItem->GetString(CALLBACK_SUCCESS);
    if (!successId.empty()) {
        takePhotoParams.success = successId;
    }

    std::string failId = firstItem->GetString(CALLBACK_FAIL);
    if (!failId.empty()) {
        takePhotoParams.fail = failId;
    }

    std::string completeId = firstItem->GetString(CALLBACK_COMPLETE);
    if (!completeId.empty()) {
        takePhotoParams.complete = completeId;
    }

    return takePhotoParams;
}

std::string DOMCamera::GetRecorderParam(const std::string& args) const
{
    size_t len = args.length();
    size_t pos = args.find(START_STR);
    size_t strLen = sizeof(START_STR) - 1;
    int32_t result = 0;

    if (pos == std::string::npos || (pos + strLen) >= len) {
        return std::string();
    }
    std::stringstream ss;
    ss << args.substr(pos + strLen);
    ss >> result;

    return std::to_string(result);
}

FlashType DOMCamera::GetFlashType(const std::string& val)
{
    static const std::unordered_map<std::string, FlashType> flashTypes = {
        { FLUSH_ON, FlashType::ON },
        { FLUSH_OFF, FlashType::OFF },
        { FLUSH_TORCH, FlashType::TORCH },
        { FLUSH_AUTO, FlashType::AUTO },
    };
    auto operatorIter = flashTypes.find(val);
    if (operatorIter != flashTypes.end()) {
        return operatorIter->second;
    }
    return FlashType::AUTO;
}

DevicePosition DOMCamera::GetDevicePosition(const std::string& val)
{
    if (val == DEVICE_POSITION_FRONT) {
        return DevicePosition::CAMERA_FACING_FRONT;
    } else {
        return DevicePosition::CAMERA_FACING_BACK;
    }
}

} // namespace OHOS::Ace::Framework