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

#include "root_component_mock.h"

#include "div_component.h"
#include "stack_component.h"

namespace OHOS {
namespace ACELite {
RootComponentMock::RootComponentMock()
    : rootContainer_(nullptr),
      containerStyleMgr_(nullptr),
      rootWidth_(DEFAULT_CONTAINER_WIDTH),
      rootHeight_(DEFAULT_CONTAINER_HEIGHT)
{
}

RootComponentMock::~RootComponentMock()
{
    if (containerStyleMgr_ != nullptr) {
        delete containerStyleMgr_;
        containerStyleMgr_ = nullptr;
    }

    if (rootContainer_ != nullptr) {
        // we do not render this container, so no need to call release
        delete rootContainer_;
        rootContainer_ = nullptr;
    }
}

/**
 * @brief PrepareRootContainer prepare the container, must call this after engine initialized
 */
void RootComponentMock::PrepareRootContainer(int16_t width, int16_t height, bool flex)
{
    rootWidth_ = width;
    rootHeight_ = height;

    if (containerStyleMgr_ == nullptr) {
        containerStyleMgr_ = new AppStyleManager();
    }

    if (rootContainer_ == nullptr) {
        if (flex) {
            rootContainer_ = new DivComponent(UNDEFINED, UNDEFINED, containerStyleMgr_);
        } else {
            rootContainer_ = new StackComponent(UNDEFINED, UNDEFINED, containerStyleMgr_);
        }
    }
}

/**
 * @brief RenderUnderTestComponent render the given component and build view tree to the container
 * @param component the under test component
 */
void RootComponentMock::RenderComponent(Component &component) const
{
    component.Render();
    // trigger build tree flow to apply aligned dimension to low layer
    ConstrainedParameter parentParameter = {rootWidth_, rootHeight_};
    Component::BuildViewTree(&component, rootContainer_, parentParameter);
}
} // namespace ACELite
} // namespace OHOS
