/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License"),
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

import formBindingData from './@ohos.application.formBindingData';
import FormExtensionContext from "./application/FormExtensionContext";
import Want from './@ohos.application.Want';
import { Configuration } from './@ohos.application.Configuration';

/**
 * class of form extension.
 *
 * @since 9
 * @syscap SystemCapability.Ability.Form
 * @StageModelOnly
 */
export default class FormExtension {
    /**
     * Indicates form extension context.
     *
     * @since 9
     * @syscap SystemCapability.Ability.Form
     * @StageModelOnly
     */
    context: FormExtensionContext;

    /**
     * Called to return a {@link formBindingData#FormBindingData} object.
     *
     * @since 9
     * @syscap SystemCapability.Ability.Form
     * @param want Indicates the detailed information for creating a {@link formBindingData#FormBindingData}.
     *               The {@code Want} object must include the form ID, form name, and grid style of the form.
     *               Such form information must be managed as persistent data for further form
     *               acquisition, update, and deletion.
     * @return Returns the created {@link formBindingData#FormBindingData} object.
     * @StageModelOnly
     */
    onCreate(want: Want): formBindingData.FormBindingData;

    /**
     * Called when the form provider is notified that a temporary form is successfully converted to a normal form.
     *
     * @since 9
     * @syscap SystemCapability.Ability.Form
     * @param formId Indicates the ID of the form.
     * @return -
     * @StageModelOnly
     */
    onCastToNormal(formId: string): void;

    /**
     * Called to notify the form provider to update a specified form.
     *
     * @since 9
     * @syscap SystemCapability.Ability.Form
     * @param formId Indicates the ID of the form to update.
     * @return -
     * @StageModelOnly
     */
    onUpdate(formId: string): void;

    /**
     * Called when the form provider receives form events from the system.
     *
     * @since 9
     * @syscap SystemCapability.Ability.Form
     * @param newStatus Indicates the form events occurred. The key in the {@code Map} object indicates the form ID,
     *    and the value indicates the event type, which can be either {@link formManager#VisibilityType#FORM_VISIBLE}
     *    or {@link formManager#VisibilityType#FORM_INVISIBLE}. {@link formManager#VisibilityType#FORM_VISIBLE}
     *    means that the form becomes visible, and {@link formManager#VisibilityType#FORM_INVISIBLE}
     *    means that the form becomes invisible.
     * @return -
     * @StageModelOnly
     */
    onVisibilityChange(newStatus: { [key: string]: number }): void;

    /**
     * Called when a specified message event defined by the form provider is triggered. This method is valid only for
     * JS forms.
     *
     * @since 9
     * @syscap SystemCapability.Ability.Form
     * @param formId Indicates the ID of the form on which the message event is triggered, which is provided by
     *               the client to the form provider.
     * @param message Indicates the value of the {@code params} field of the message event. This parameter is
     *                used to identify the specific component on which the event is triggered.
     * @return -
     * @StageModelOnly
     */
    onEvent(formId: string, message: string): void;

    /**
     * Called to notify the form provider that a specified form has been destroyed. Override this method if
     * you want your application, as the form provider, to be notified of form deletion.
     *
     * @since 9
     * @syscap SystemCapability.Ability.Form
     * @param formId Indicates the ID of the destroyed form.
     * @return -
     * @StageModelOnly
     */
    onDestroy(formId: string): void;

    /**
     * Called when the system configuration is updated.
     *
     * @since 9
     * @syscap SystemCapability.Ability.Form
     * @param configuration Indicates the system configuration, such as language and color mode.
     * @return -
     * @StageModelOnly
     */
    onConfigurationUpdated(config: Configuration): void;
}