/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#ifndef GRAPHIC_LITE_AUTO_TEST_APP_H
#define GRAPHIC_LITE_AUTO_TEST_APP_H

namespace OHOS {
class AutoTestApp {
public:
    static AutoTestApp* GetInstance();
    void Start();

private:
    AutoTestApp() {}
    virtual ~AutoTestApp();

    AutoTestApp(const AutoTestApp&) = delete;
    AutoTestApp& operator=(const AutoTestApp&) = delete;
    AutoTestApp(AutoTestApp&&) = delete;
    AutoTestApp& operator=(AutoTestApp&&) = delete;
};
} // namespace OHOS
#endif // GRAPHIC_LITE_AUTO_TEST_APP_H
