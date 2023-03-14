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

import commonEvent from '@ohos.commonEvent'
import featureAbility from '@ohos.ability.featureAbility'

const injectRef = Object.getPrototypeOf(global) || global
injectRef.regeneratorRuntime = require('@babel/runtime/regenerator')

function PublishCallBack() {
    console.debug("====>Publish CallBack ACTS_FinishWithResult_0100_CommonEvent====>");
}
export default {
    data: {
        title: ""
    },
    onInit() {
        this.title = this.$t('strings.world');
    },
    async onShow() {
        commonEvent.publish("ACTS_FinishWithResult_0100_CommonEvent", PublishCallBack);
        await featureAbility.terminateSelfWithResult(
            {
                resultCode: 1,
                want:
                {
                    action: "action.system.home",
                    entities: ["entity.system.home"],
                    type: "MIMETYPE",
                    options:{
                    // indicates the grant to perform read operations on the URI
                        authReadUriPermission: true,
                    // indicates the grant to perform write operations on the URI
                        authWriteUriPermission: true,
                    // support forward intent result to origin ability
                        abilityForwardResult: true,
                    // used for marking the ability start-up is triggered by continuation
                        abilityContinuation: true,
                    // specifies whether a component does not belong to ohos
                        notOhosComponent: true,
                    // specifies whether an ability is started
                        abilityFormEnabled: true,
                    // indicates the grant for possible persisting on the URI.
                        authPersistableUriPermission: true,
                    // indicates the grant for possible persisting on the URI.
                        authPrefixUriPermission: true,
                    // support distributed scheduling system start up multiple devices
                        abilitySliceMultiDevice: true,
                    // indicates that an ability using the service template is started regardless of whether the
                    // host application has been started.
                        startForegroundAbility: true,
                    // install the specified ability if it's not installed.
                        installOnDemand: true,
                    // return result to origin ability slice
                        abilitySliceForwardResult: true,
                    // install the specified ability with background mode if it's not installed.
                        installWithBackgroundMode: true
                    },
                    deviceId: 'deviceId',
                    bundleName: 'com.example.finishwithresultpromiseparameterstest',
                    abilityName: 'com.example.finishwithresultpromiseparameterstest.MainAbility',
                    uri: '',
                },
            }
        )
        featureAbility.terminateSelf(
            (err, data)=>{
            console.debug('==========Terminate Ability Success==========');
        });
    },
    onReady() {
        console.info('onReady');
    },
}
