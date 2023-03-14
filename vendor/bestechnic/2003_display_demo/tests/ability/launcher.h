/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
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
#ifndef LAUNCHER_H
#define LAUNCHER_H

#include "ability_info.h"
#include "ability_state.h"
#include "slite_ability.h"
#include "want.h"
#include "common/screen.h"
#include "components/root_view.h"
#include "components/ui_image_view.h"

namespace OHOS
{
class Launcher : public SliteAbility
{
public:
    static Launcher *GetInstance()
    {
        static Launcher instance;
        return &instance;
    }

    void OnStart(const Want &want) override;
    void OnInactive();
    void OnActive(const Want &want) override;
    void OnBackground() override;
    void OnStop() override;

private:
    Launcher() {}
    ~Launcher();
    void InitUI();
    void DeleteUI();

    RootView *rootView_ = nullptr;
    UIImageView *gifImageView_ = nullptr;
};
} // namespace OHOS

extern "C" void InstallLauncher();

#endif
