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
#ifndef PAGE_ABILITY_DEMO_H
#define PAGE_ABILITY_DEMO_H
#include <string>

#include "ability.h"
#include "ability_loader.h"
#include "app_log_wrapper.h"
#include "stpageabilityevent.h"
namespace OHOS {
namespace AppExecFwk {
class PageAbilityDemo : public Ability {
protected:
    virtual void OnStart(const Want &want) override;
    virtual void OnStop() override;
    virtual void OnActive() override;
    virtual void OnInactive() override;
    virtual void OnBackground() override;

    virtual void OnForeground(const Want &want) override;
    virtual void OnNewWant(const Want &want) override;

public:
    void CreateFile(const std::string &path) const;
    void CreateDir(const std::string &path) const;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  //PAGE_ABILITY_DEMO_H