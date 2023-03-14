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

#ifndef FOUNDATION_ACE_ACE_ENGINE_ADAPTER_OHOS_CPP_ACE_FORM_ABILITY_H
#define FOUNDATION_ACE_ACE_ENGINE_ADAPTER_OHOS_CPP_ACE_FORM_ABILITY_H

#include <string>
#include <vector>

#include "ability.h"
#include "ability_loader.h"
#include "iremote_object.h"
#include "want.h"

namespace OHOS::Ace {

class AceFormAbility final : public OHOS::AppExecFwk::Ability {
public:
    AceFormAbility();
    virtual ~AceFormAbility() = default;
    void OnStart(const OHOS::AAFwk::Want& want) override;
    void OnStop() override;
    sptr<IRemoteObject> OnConnect(const OHOS::AAFwk::Want& want) override;
    void OnDisconnect(const OHOS::AAFwk::Want& want) override;

    /**
     * @brief Called to return a FormProviderInfo object.
     *
     * <p>You must override this method if your ability will serve as a form provider to provide a form for clients.
     * The default implementation returns nullptr. </p>
     *
     * @param want   Indicates the detailed information for creating a FormProviderInfo.
     *               The Want object must include the form ID, form name of the form,
     *               which can be obtained from Ability#PARAM_FORM_IDENTITY_KEY,
     *               Ability#PARAM_FORM_NAME_KEY, and Ability#PARAM_FORM_DIMENSION_KEY,
     *               respectively. Such form information must be managed as persistent data for further form
     *               acquisition, update, and deletion.
     *
     * @return Returns the created FormProviderInfo object.
     */
    OHOS::AppExecFwk::FormProviderInfo OnCreate(const OHOS::AAFwk::Want& want) override;

    /**
     * @brief Called to notify the form provider that a specified form has been deleted. Override this method if
     * you want your application, as the form provider, to be notified of form deletion.
     *
     * @param formId Indicates the ID of the deleted form.
     * @return None.
     */
    void OnDelete(const int64_t formId) override;

    /**
     * @brief Called to notify the form provider to update a specified form.
     *
     * @param formId Indicates the ID of the form to update.
     * @param message Form event message.
     */
    void OnTriggerEvent(const int64_t formId, const std::string& message) override;

    /**
     * @brief Called to notify the form provider to update a specified form.
     *
     * @param formId Indicates the ID of the form to update.
     * @return none.
     */
    void OnUpdate(const int64_t formId) override;

    /**
     * @brief Called when the form provider is notified that a temporary form is successfully converted to
     * a normal form.
     *
     * @param formId Indicates the ID of the form.
     * @return None.
     */
    void OnCastTemptoNormal(const int64_t formId) override;

    /**
     * @brief Called when the form provider receives form events from the fms.
     *
     * @param formEventsMap Indicates the form events occurred. The key in the Map object indicates the form ID,
     *                      and the value indicates the event type, which can be either FORM_VISIBLE
     *                      or FORM_INVISIBLE. FORM_VISIBLE means that the form becomes visible,
     *                      and FORM_INVISIBLE means that the form becomes invisible.
     * @return none.
     */
    void OnVisibilityChanged(const std::map<int64_t, int32_t>& formEventsMap) override;

private:
    void LoadFormEnv(const OHOS::AAFwk::Want& want);

    static int32_t instanceId_;
    static const std::string START_PARAMS_KEY;
    static const std::string URI;
};

} // namespace OHOS::Ace
#endif // FOUNDATION_ACE_ACE_ENGINE_ADAPTER_OHOS_CPP_ACE_FORM_ABILITY_H
