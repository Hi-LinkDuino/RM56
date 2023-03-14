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

describe('appInfoTest', function () {
    console.log("*************Battery commonEvent Test Begin*************");

    /**
     * @tc.number BatteryCommonEventTest_001
     * @tc.name subscribe battery changed common event
     * @tc.desc battery acquisition kit
     */
    it('BatteryCommonEventTest_001', 0, function (done) {
        createBatteryChangedSubscriber();
        done();
    })

    /**
     * @tc.number BatteryCommonEventTest_002
     * @tc.name subscribe battery okay common event
     * @tc.desc Battery acquisition kit
     */
    it('BatteryCommonEventTest_002', 0, function (done) {
        createBatteryOkaySubscriber();
        done();
    })

    /**
     * @tc.number BatteryCommonEventTest_003
     * @tc.name subscribe battery low common event
     * @tc.desc Battery acquisition kit
     */
    it('BatteryCommonEventTest_003', 0, function (done) {
        createBatteryLowSubscriber();
        done();
    })

    /**
     * @tc.number BatteryCommonEventTest_004
     * @tc.name subscribe power connected common event
     * @tc.desc Battery acquisition kit
     */
    it('BatteryCommonEventTest_004', 0, function (done) {
        createBatteryPowerConnectedSubscriber();
        done();
    })

    /**
     * @tc.number BatteryCommonEventTest_005
     * @tc.name subscribe power disconnected common event
     * @tc.desc Battery acquisition kit
     */
    it('BatteryCommonEventTest_005', 0, function (done) {
        createBatteryPowerDisconnectedSubscriber();
        done();
    })

    /**
     * @tc.number BatteryCommonEventTest_006
     * @tc.name subscribe battery charging common event
     * @tc.desc Battery acquisition kit
     */
    it('BatteryCommonEventTest_006', 0, function (done) {
        createBatteryChargingSubscriber();
        done();
    })

    /**
     * @tc.number BatteryCommonEventTest_007
     * @tc.name subscribe battery discharging common event
     * @tc.desc Battery acquisition kit
     */
    it('BatteryCommonEventTest_007', 0, function (done) {
        createBatteryDischargingSubscriber();
        done();
    })

    console.log("*************Battery commonEvent Test End*************");
})

function createBatteryChangedSubscriber() {
     var commonEventSubscribeInfo = {
         events: [commonEvent.Support.COMMON_EVENT_BATTERY_CHANGED],
     };
    commonEvent.createSubscriber(commonEventSubscribeInfo)
        .then(subscriber => {
            console.info('createBatteryChangedSubscriber success');
            var mySubscriber = subscriber;
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
            console.info('subscribe battery_changed begin ');

            commonEvent.subscribe(mySubscriber, (error, commonEventData) => {
                console.error('err code: ' + JSON.stringify(error));
                console.info('subscribe callback: ' + JSON.stringify(commonEventData));
                console.info("commonEventData event: " + commonEventData.event);
                console.info("commonEventData bundleName: " + commonEventData.bundleName);
                console.info("commonEventData data: " + commonEventData.data);
                console.info("commonEventData parameter: " + commonEventData.parameters[0]);
                var capacity = commonEventData.parameters['0'];
                console.info("capacity is:" + capacity);
                expect(capacity >= 0 && capacity <= 100).assertTrue();
            });
        }).catch((error) => {
            console.error('Operation failed. Cause: ' + JSON.stringify(error));
    });
}

function createBatteryOkaySubscriber() {
     var commonEventSubscribeInfo = {
         events: [commonEvent.Support.COMMON_EVENT_BATTERY_OKAY],
     };
    commonEvent.createSubscriber(commonEventSubscribeInfo)
        .then(subscriber => {
            console.info('createBatteryOkaySubscriber success');
            var mySubscriber = subscriber;
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
            console.info('subscribe battery_okay begin ');

            commonEvent.subscribe(mySubscriber, (error, commonEventData) => {
                console.error('err code: ' + JSON.stringify(error));
                console.info('subscribe callback: ' + JSON.stringify(commonEventData));
                console.info("commonEventData event: " + commonEventData.event);
                console.info("commonEventData bundleName: " + commonEventData.bundleName);
                console.info("commonEventData data: " + commonEventData.data);
                console.info("commonEventData parameter: " + commonEventData.parameters[0]);
                var capacity = commonEventData.parameters['0'];
                console.info("capacity is:" + capacity);
                expect(capacity >= 0 && capacity <= 100).assertTrue();
            });
        }).catch((error) => {
            console.error('Operation failed. Cause: ' + JSON.stringify(error));
    });
}

function createBatteryLowSubscriber() {
     var commonEventSubscribeInfo = {
         events: [commonEvent.Support.COMMON_EVENT_BATTERY_LOW],
     };
    commonEvent.createSubscriber(commonEventSubscribeInfo)
        .then(subscriber => {
            console.info('createBatteryLowSubscriber success');
            var mySubscriber = subscriber;
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
            console.info('subscribe battery_low begin ');

            commonEvent.subscribe(mySubscriber, (error, commonEventData) => {
                console.error('err code: ' + JSON.stringify(error));
                console.info('subscribe callback: ' + JSON.stringify(commonEventData));
                console.info("commonEventData event: " + commonEventData.event);
                console.info("commonEventData bundleName: " + commonEventData.bundleName);
                console.info("commonEventData data: " + commonEventData.data);
                console.info("commonEventData parameter: " + commonEventData.parameters[0]);
                var capacity = commonEventData.parameters['0'];
                console.info("capacity is:" + capacity);
                expect(capacity >= 0 && capacity <= 100).assertTrue();
            });
        }).catch((error) => {
            console.error('Operation failed. Cause: ' + JSON.stringify(error));
    });
}

function createBatteryPowerConnectedSubscriber() {
     var commonEventSubscribeInfo = {
         events: [commonEvent.Support.COMMON_EVENT_POWER_CONNECTED],
     };
    commonEvent.createSubscriber(commonEventSubscribeInfo)
        .then(subscriber => {
            console.info('createBatteryPowerConnectedSubscriber success');
            var mySubscriber = subscriber;
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
            console.info('subscribe power_connected begin ');

            commonEvent.subscribe(mySubscriber, (error, commonEventData) => {
                console.error('err code: ' + JSON.stringify(error));
                console.info('subscribe callback: ' + JSON.stringify(commonEventData));
                console.info("commonEventData event: " + commonEventData.event);
                console.info("commonEventData bundleName: " + commonEventData.bundleName);
                console.info("commonEventData data: " + commonEventData.data);
                console.info("commonEventData parameter: " + commonEventData.parameters[0]);
                var pluggedType = commonEventData.parameters['4'];
                console.info("pluggedType is:" + pluggedType);
                expect(pluggedType >= 0 && pluggedType <= 4).assertTrue();
            });
        }).catch((error) => {
            console.error('Operation failed. Cause: ' + JSON.stringify(error));
    });
}

function createBatteryPowerDisconnectedSubscriber() {
     var commonEventSubscribeInfo = {
         events: [commonEvent.Support.COMMON_EVENT_POWER_DISCONNECTED],
     };
    commonEvent.createSubscriber(commonEventSubscribeInfo)
        .then(subscriber => {
            console.info('createBatteryPowerDisconnectedSubscriber success');
            var mySubscriber = subscriber;
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
            console.info('subscribe power_disconnected begin ');

            commonEvent.subscribe(mySubscriber, (error, commonEventData) => {
                console.error('err code: ' + JSON.stringify(error));
                console.info('subscribe callback: ' + JSON.stringify(commonEventData));
                console.info("commonEventData event: " + commonEventData.event);
                console.info("commonEventData bundleName: " + commonEventData.bundleName);
                console.info("commonEventData data: " + commonEventData.data);
                console.info("commonEventData parameter: " + commonEventData.parameters[0]);
                var pluggedType = commonEventData.parameters['4'];
                console.info("pluggedType is:" + pluggedType);
                expect(pluggedType >= 0 && pluggedType <= 4).assertTrue();
            });
        }).catch((error) => {
            console.error('Operation failed. Cause: ' + JSON.stringify(error));
    });
}

function createBatteryChargingSubscriber() {
     var commonEventSubscribeInfo = {
         events: [commonEvent.Support.COMMON_EVENT_CHARGING],
     };
    commonEvent.createSubscriber(commonEventSubscribeInfo)
        .then(subscriber => {
            console.info('createBatteryChargingSubscriber success');
            var mySubscriber = subscriber;
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
            console.info('subscribe charging begin ');

            commonEvent.subscribe(mySubscriber, (error, commonEventData) => {
                console.error('err code: ' + JSON.stringify(error));
                console.info('subscribe callback: ' + JSON.stringify(commonEventData));
                console.info("commonEventData event: " + commonEventData.event);
                console.info("commonEventData bundleName: " + commonEventData.bundleName);
                console.info("commonEventData data: " + commonEventData.data);
                console.info("commonEventData parameter: " + commonEventData.parameters[0]);
                var chargeState = commonEventData.parameters['7'];
                console.info("chargeState is:" + chargeState);
                expect(chargeState >= 0 && chargeState <= 10).assertTrue();
            });
        }).catch((error) => {
            console.error('Operation failed. Cause: ' + JSON.stringify(error));
    });
}

function createBatteryDischargingSubscriber() {
     var commonEventSubscribeInfo = {
         events: [commonEvent.Support.COMMON_EVENT_DISCHARGING],
     };
    commonEvent.createSubscriber(commonEventSubscribeInfo)
        .then(subscriber => {
            console.info('createBatteryDischargingSubscriber success');
            var mySubscriber = subscriber;
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
            console.info('subscribe discharging begin ');

            commonEvent.subscribe(mySubscriber, (error, commonEventData) => {
                console.error('err code: ' + JSON.stringify(error));
                console.info('subscribe callback: ' + JSON.stringify(commonEventData));
                console.info("commonEventData event: " + commonEventData.event);
                console.info("commonEventData bundleName: " + commonEventData.bundleName);
                console.info("commonEventData data: " + commonEventData.data);
                console.info("commonEventData parameter: " + commonEventData.parameters[0]);
                var chargeState = commonEventData.parameters['7'];
                console.info("chargeState is:" + chargeState);
                expect(chargeState >= 0 && chargeState <= 10).assertTrue();
            });
        }).catch((error) => {
            console.error('Operation failed. Cause: ' + JSON.stringify(error));
    });
}
