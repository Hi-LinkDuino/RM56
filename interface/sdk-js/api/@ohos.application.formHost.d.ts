/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

import { AsyncCallback } from "./basic";
import { Callback } from "./basic";
import Want from './@ohos.application.Want';
import formInfo from './@ohos.application.formInfo'

/**
 * interface of formHost.
 *
 * @name formHost
 * @since 8
 * @syscap SystemCapability.Ability.Form
 * @systemapi hide for inner use.
 */
declare namespace formHost {
    /**
     * Deletes an obtained form by its ID.
     *
     * <p>After this method is called, the form won't be available for use by the application and the Form Manager
     * Service no longer keeps the cache information about the form.</p>
     *
     * @since 8
     * @syscap SystemCapability.Ability.Form
     * @param formId Indicates the form ID
     * @return -
     * @permission ohos.permission.REQUIRE_FORM
     */
    function deleteForm(formId: string, callback: AsyncCallback<void>): void;
    function deleteForm(formId: string): Promise<void>;

    /**
     * Releases an obtained form by its ID.
     *
     * <p>After this method is called, the form won't be available for use by the application, but the Form Manager
     * Service still keeps the cache information about the form, so that the application can quickly obtain it based on
     * the {@code formId}.</p>
     *
     * @since 8
     * @syscap SystemCapability.Ability.Form
     * @param formId Indicates the form ID
     * @param isReleaseCache Indicates whether or not release cache
     * @return -
     * @permission ohos.permission.REQUIRE_FORM
     */
    function releaseForm(formId: string, callback: AsyncCallback<void>): void;
    function releaseForm(formId: string, isReleaseCache: boolean, callback: AsyncCallback<void>): void;
    function releaseForm(formId: string, isReleaseCache?: boolean): Promise<void>;

    /**
     * Requests for form update.
     *
     * <p>This method must be called when the application has detected that a system setting item (such as the language,
     * resolution, or screen orientation) being listened for has changed. Upon receiving the update request, the form
     * provider automatically updates the form data (if there is any update) through the form framework, with the update
     * process being unperceivable by the application.</p>
     *
     * @since 8
     * @syscap SystemCapability.Ability.Form
     * @param formId Indicates the ID of the form to update.
     * @return -
     * @permission ohos.permission.REQUIRE_FORM
     */
    function requestForm(formId: string, callback: AsyncCallback<void>): void;
    function requestForm(formId: string): Promise<void>;

    /**
     * Converts a specified temporary form that has been obtained by the application into a normal form.
     *
     * @since 8
     * @syscap SystemCapability.Ability.Form
     * @param formId Indicates the ID of the temporary form to convert.
     * @return -
     * @permission ohos.permission.REQUIRE_FORM
     */
    function castTempForm(formId: string, callback: AsyncCallback<void>): void;
    function castTempForm(formId: string): Promise<void>;

    /**
     * Sends a notification to the form framework to make the specified forms visible.
     *
     * <p>After this method is successfully called, onVisibilityChange will be called to notify the form provider of
     * the form visibility change event.</p>
     *
     * @since 8
     * @syscap SystemCapability.Ability.Form
     * @param formIds Indicates the IDs of the forms to be made visible.
     * @return -
     * @permission ohos.permission.REQUIRE_FORM
     */
    function notifyVisibleForms(formIds: Array<string>, callback: AsyncCallback<void>): void;
    function notifyVisibleForms(formIds: Array<string>): Promise<void>;

    /**
     * Sends a notification to the form framework to make the specified forms invisible.
     *
     * <p>After this method is successfully called, onVisibilityChange will be called to notify the form provider of
     * the form visibility change event.</p>
     *
     * @since 8
     * @syscap SystemCapability.Ability.Form
     * @param formIds Indicates the IDs of the forms to be made invisible.
     * @return -
     * @permission ohos.permission.REQUIRE_FORM
     */
    function notifyInvisibleForms(formIds: Array<string>, callback: AsyncCallback<void>): void;
    function notifyInvisibleForms(formIds: Array<string>): Promise<void>;

    /**
     * Notify the form framework to make the specified forms updatable.
     *
     * <p>You can use this method to set form refresh state to true, the form can receive new
     * update from service.</p>
     *
     * @since 8
     * @syscap SystemCapability.Ability.Form
     * @param formIds Indicates the IDs of the forms to be made invisible.
     * @return -
     * @permission ohos.permission.REQUIRE_FORM
     */
    function enableFormsUpdate(formIds: Array<string>, callback: AsyncCallback<void>): void;
    function enableFormsUpdate(formIds: Array<string>): Promise<void>;

    /**
     * Notifys the form framework to make the specified forms non updatable.
     *
     * <p>You can use this method to set form refresh state to false, the form do not receive
     * new update from service.</p>
     *
     * @since 8
     * @syscap SystemCapability.Ability.Form
     * @param formIds Indicates the IDs of the forms to be made invisible.
     * @return -
     * @permission ohos.permission.REQUIRE_FORM
     */
    function disableFormsUpdate(formIds: Array<string>, callback: AsyncCallback<void>): void;
    function disableFormsUpdate(formIds: Array<string>): Promise<void>;

    /**
     * Checks whether the system is ready.
     *
     * <p>You can use this method to obtain the system is ready.</p>
     *
     * @since 8
     * @syscap SystemCapability.Ability.Form
     * @return -
     */
    function isSystemReady(callback: AsyncCallback<void>): void;
    function isSystemReady(): Promise<void>;

    /**
     * Obtains the FormInfo objects provided by all applications on the device.
     *
     * @since 8
     * @syscap SystemCapability.Ability.Form
     * @return Returns the {@link FormInfo}
     * @permission ohos.permission.GET_BUNDLE_INFO_PRIVILEGED
     */
    function getAllFormsInfo(callback: AsyncCallback<Array<formInfo.FormInfo>>): void;
    function getAllFormsInfo(): Promise<Array<formInfo.FormInfo>>;

    /**
     * Obtains the FormInfo objects provided by a specified application on the device.
     *
     * @since 8
     * @syscap SystemCapability.Ability.Form
     * @param bundleName Indicates the bundle name of the application.
     * @param moduleName Indicates the module name of the application.
     * @return Returns the {@link FormInfo}
     * @permission ohos.permission.GET_BUNDLE_INFO_PRIVILEGED
     */
    function getFormsInfo(bundleName: string, callback: AsyncCallback<Array<formInfo.FormInfo>>): void;
    function getFormsInfo(bundleName: string, moduleName: string, callback: AsyncCallback<Array<formInfo.FormInfo>>): void;
    function getFormsInfo(bundleName: string, moduleName?: string): Promise<Array<formInfo.FormInfo>>;

    /**
     * Deletes invalid forms of the application in the Form Manager Service based on the list of.
     *
     * <p>You can use this method to delete invalid forms of the application.</p>
     *
     * @since 8
     * @syscap SystemCapability.Ability.Form
     * @param formIds Indicates the specify form id.
     * @return Returns the number of invalid forms deleted by the Form Manager Service
     * @permission ohos.permission.REQUIRE_FORM.
     */
    function deleteInvalidForms(formIds: Array<string>, callback: AsyncCallback<number>): void;
    function deleteInvalidForms(formIds: Array<string>): Promise<number>;

    /**
     * Obtains the Form state.
     *
     * <p>You can use this method to obtains the form state.</p>
     *
     * @since 8
     * @syscap SystemCapability.Ability.Form
     * @param want Indicates want of the form.
     * @return Returns form state {@link FormStateInfo}
     * @permission ohos.permission.GET_BUNDLE_INFO and ohos.permission.GET_BUNDLE_INFO_PRIVILEGED.
     */
    function acquireFormState(want: Want, callback: AsyncCallback<formInfo.FormStateInfo>): void;
    function acquireFormState(want: Want): Promise<formInfo.FormStateInfo>;

    /**
     * Listens to the event of uninstall form.
     *
     * <p>You can use this method to listen to the event of uninstall form.</p>
     *
     * @since 8
     * @syscap SystemCapability.Ability.Form
     * @param type Indicates event type.
     * @return -
     */
    function on(type: "formUninstall", callback: Callback<string>): void;

    /**
     * Cancels listening to the event of uninstall form.
     *
     * <p>You can use this method to cancel listening to the event of uninstall form.</p>
     *
     * @since 8
     * @syscap SystemCapability.Ability.Form
     * @param type Indicates event type.
     * @return -
     */
    function off(type: "formUninstall", callback?: Callback<string>): void;

    /**
     * notify form is Visible
     *
     * <p>You can use this method to notify form visible state.</p>
     *
     * @since 8
     * @syscap SystemCapability.Ability.Form
     * @param formIds Indicates the specify form id.
     * @param isVisible Indicates whether visible.
     * @return -
     * @permission ohos.permission.REQUIRE_FORM.
     */
    function notifyFormsVisible(formIds: Array<string>, isVisible: boolean, callback: AsyncCallback<void>): void;
    function notifyFormsVisible(formIds: Array<string>, isVisible: boolean): Promise<void>;

    /**
     * notify form enable update state.
     *
     * <p>You can use this method to notify form enable update state.</p>
     *
     * @since 8
     * @syscap SystemCapability.Ability.Form
     * @param formIds Indicates the specify form id.
     * @param isEnableUpdate Indicates whether enable update.
     * @return -
     * @permission ohos.permission.REQUIRE_FORM.
     */
    function notifyFormsEnableUpdate(formIds: Array<string>, isEnableUpdate: boolean, callback: AsyncCallback<void>): void;
    function notifyFormsEnableUpdate(formIds: Array<string>, isEnableUpdate: boolean): Promise<void>;
}
export default formHost;