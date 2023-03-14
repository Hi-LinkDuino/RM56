/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
import commonEvent from '@ohos.commonEvent';
import thermal from "@ohos.thermal"

describe('appInfoTest', function () {
    console.log("*************Thermal commonEvent Test Begin*************");

    /**
     * @tc.number ThermalCommonEventTest_001
     * @tc.name subscribe thermal level changed common event
     * @tc.desc Thermal acquisition kit
     */
    it('ThermalCommonEventTest_001', 0, function (done) {
        createSubscriber();
        done();
    })
    console.log("*************Thermal commonEvent Test End*************");
})

function createSubscriber() {
    let commonEventSubscribeInfo = {
        events: [commonEvent.Support.COMMON_EVENT_THERMAL_LEVEL_CHANGED],
    };
    commonEvent.createSubscriber(commonEventSubscribeInfo)
        .then(subscriber => {
            console.info('createSubscriber success');
            let mySubscriber = subscriber;
            console.log(subscriber);

            if (subscriber == "" || subscriber == undefined || subscriber == null) {
                console.info("createSubscriber failed");
            }
            mySubscriber.getCode()
                .then((data) => {
                    console.info('Subscriber getCode success : ' + JSON.stringify(data));
                }).catch((error) => {
                    console.error('Subscriber getCode error because: ' + JSON.stringify(error));
                })
            mySubscriber.getData()
                .then((data) => {
                    console.info('Subscriber getData success : ' + JSON.stringify(data));
                }).catch((error) => {
                    console.error('Subscriber getData error because: ' + JSON.stringify(error));
                })
            console.info('subscribe begin ');

            commonEvent.subscribe(mySubscriber, (error, commonEventData) => {
                console.error('err code: ' + JSON.stringify(error));
                console.info('subscribe callback: ' + JSON.stringify(commonEventData));
                console.info("commonEventData event: " + commonEventData.event);
                console.info("commonEventData bundleName: " + commonEventData.bundleName);
                console.info("commonEventData data: " + commonEventData.data);
                console.info("commonEventData parameter: " + commonEventData.parameters[0]);
                let level = -1;
                expect(level >= thermal.ThermalLevel.COOL && warm <= level.ThermalLevel.EMERGENCY).assertTrue();
            });
        }).catch((error) => {
            console.error('Operation failed. Cause: ' + JSON.stringify(error));
        });
}