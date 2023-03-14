/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

import Subscriber from '@ohos.commonEvent'

export default {
    constructor(obj) {
        console.log('formSystemTestServiceA: constructor ok, time stamp:'+ Date.parse(new Date()));

        Subscriber.publish(
            "fms_serviceA_constructor",
            (err,data) => {
                console.log("formSystemTestServiceA: constructor publish callback return! ")
            }
        );
    },

    updateData(obj) {
        console.log('formSystemTestServiceA: updateData ok, time stamp:'+ Date.parse(new Date()));

        Subscriber.publish(
            "fms_serviceA_updateData",
            (err,data) => {
                console.log("formSystemTestServiceA: updateData publish callback return! ")
            }
        );
    },

    getDataString() {
        console.log('formSystemTestServiceA: getDataString ok, time stamp:'+ Date.parse(new Date()));

        Subscriber.publish(
            "fms_serviceA_getDataString",
            (err,data) => {
                console.log("formSystemTestServiceA: getDataString publish callback return! ")
            }
        );
        return "dataString";
    },

    addImageData(name, data){
        console.log('formSystemTestServiceA: addImageData ok, time stamp:'+ Date.parse(new Date()));

        Subscriber.publish(
            "fms_serviceA_addImageData",
            (err,data) => {
                console.log("formSystemTestServiceA: addImageData publish callback return! ")
            }
        );
    },

    removeImageData(name){
        console.log('formSystemTestServiceA: removeImageData ok, time stamp:'+ Date.parse(new Date()));

        Subscriber.publish(
            "fms_serviceA_removeImageData",
            (err,data) => {
                console.log("formSystemTestServiceA: removeImageData publish callback return! ")
            }
        );
    },

    onCreate(para) {
        console.log('formSystemTestServiceA: onCreate ok, para：'+ para + ', time stamp:'+ Date.parse(new Date()));

        Subscriber.publish(
            "fms_serviceA_onCreate",
            (err,data) => {
              console.log("formSystemTestServiceA: onCreate publish callback return! ")
            }
        );
        return {"city":"beijingA"};
    },

    onDestroy(para) {
        console.log('formSystemTestServiceA: onDestroy para：'+ para + ', time stamp:'+ Date.parse(new Date()));

        Subscriber.publish(
            "fms_serviceA_onDestroy",
            (err,data) => {
                console.log("formSystemTestServiceA: onDestroy publish callback return! ")
            }
        );
    },

    onDelete(formId) {
        console.log('formSystemTestServiceA:  onDelete ok, formId:' + formId +
            ', time stamp:'+ Date.parse(new Date()));

        Subscriber.publish(
            "fms_serviceA_onDelete",
            (err,data) => {
                console.log("formSystemTestServiceA: onDelete publish callback return! ")
            }
        );
    },

    onTriggerEvent(formId, message) {
        console.log('formSystemTestServiceA:  onTriggerEvent ok, formId:' + formId
            + ', message:' + message + ', time stamp:'+ Date.parse(new Date()));

        Subscriber.publish(
            "fms_serviceA_onTriggerEvent",
            (err,data) => {
                console.log("formSystemTestServiceA: onTriggerEvent publish callback return! ")
            }
        );
    },

    onUpdate(formId) {
        console.log('formSystemTestServiceA:  onUpdate ok, formId:' + formId +
            ', time stamp:'+ Date.parse(new Date()));

        Subscriber.publish(
            "fms_serviceA_onUpdate",
            (err,data) => {
                console.log("formSystemTestServiceA: onUpdate publish callback return! ")
            }
        );
    },

    onCastTemp(formId) {
        console.log('formSystemTestServiceA:  onCastTemp ok, formId:' + formId +
            ', time stamp:'+ Date.parse(new Date()));

        Subscriber.publish(
            "fms_serviceA_onCastTemp",
            (err,data) => {
                console.log("formSystemTestServiceA: onCastTemp publish callback return! ")
            }
        );
    },

    onVisibilityChanged(formEventsMap) {
        console.log('formSystemTestServiceA:  OnVisibilityChanged ok, map:'+ formEventsMap +
            ', time stamp:'+ Date.parse(new Date()));

        Subscriber.publish(
            "fms_serviceA_onVisibilityChanged",
            (err,data) => {
                console.log("formSystemTestServiceA: onVisibilityChanged publish callback return! ")
            }
        );
    },

    onAcquireState(want) {
        console.log('formSystemTestServiceA:  OnAcquireState ok, '+ 'want.abilityName:' + want.abilityName +
            ', want.bundleName:' + want.bundleName + ', time stamp:'+ Date.parse(new Date()));

        Subscriber.publish(
            "fms_serviceA_onAcquireState",
            (err,data) => {
                console.log("formSystemTestServiceA: onAcquireState publish callback return! ")
            }
        );
    }
};
