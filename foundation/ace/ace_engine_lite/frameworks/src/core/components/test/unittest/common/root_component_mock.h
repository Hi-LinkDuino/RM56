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

#ifndef ACELITE_JSFWK_TEST_CONTAINER_HOLDER_H
#define ACELITE_JSFWK_TEST_CONTAINER_HOLDER_H

#include "component.h"
#include "stylemgr/app_style_manager.h"

namespace OHOS {
namespace ACELite {
class RootComponentMock final {
public:
    RootComponentMock();
    ~RootComponentMock();

    /**
     * @brief PrepareRootContainer prepare the container
     */
    void PrepareRootContainer(int16_t width = DEFAULT_CONTAINER_WIDTH,
                              int16_t height = DEFAULT_CONTAINER_HEIGHT,
                              bool flex = true);
    /**
     * @brief RenderUnderTestComponent render the given component and build view tree to the container
     * @param component the under test component
     */
    void RenderComponent(Component &component) const;

private:
    static const int16_t DEFAULT_CONTAINER_WIDTH = 454;
    static const int16_t DEFAULT_CONTAINER_HEIGHT = 454;
    // used for build view tree flow, no specific meaning
    Component *rootContainer_;
    AppStyleManager *containerStyleMgr_;
    int16_t rootWidth_;
    int16_t rootHeight_;
};
} // namespace ACELite
} // namespace OHOS
#endif // ACELITE_JSFWK_TEST_CONTAINER_HOLDER_H
