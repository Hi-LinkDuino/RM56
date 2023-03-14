/**
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

import particleAbility from '@ohos.ability.particleAbility';
import rpc from '@ohos.rpc'

class StubTest5 extends rpc.RemoteObject {
    constructor(des) {
        super(des)
    }

    onRemoteRequest(code, data, reply, option) {
        console.info('ServiceAbility5 onRemoteRequest');
        if (code === 1) {
            let op1 = data.readInt();
            let op2 = data.readInt();
            reply.writeInt(op1 + op2);
            console.info('ServiceAbility5 op1:' + op1 + ' op2:' + op2);
        }
        return true;
    }
}

function getNowTime() {
    return new Date().getTime();
}

function getDurationTime(msg, startTime, endTime) {
    console.info(msg + 'Get Interface startTime: ' + startTime);
    console.info(msg + 'Get Interface endTime: ' + endTime);
    var duration = (endTime - startTime)
    console.info(msg + 'Get Interface Duration: ' + duration);
    return duration;
}

async function sleep(time: any) {
    var now = new Date();
    var exitTime = now.getTime() + time;
    while (true) {
        now = new Date();
        if (now.getTime() > exitTime)
        break;
    }
    console.info(`sleep ${time} over...`);
}

export default {
    onStart() {
        console.info('ServiceAbility5 onStart');
    },
    onStop() {
        console.info('ServiceAbility5 onStop');
    },
    onCommand(want, startId) {
        console.info('ServiceAbility5 onCommand');
        let request = {
            "bundleName": "",
            "abilityName": "",
        }
        let options = {
            onConnect: async function (element: any, proxy: any) {
                console.log('particleAbility_connectAbility_test_0400 ConnectAbility onConnect element.deviceId : '
                + JSON.stringify(element.deviceId))
                console.log('particleAbility_connectAbility_test_0400 ConnectAbility onConnect element.bundleName : '
                + JSON.stringify(element.bundleName))
                console.log('particleAbility_connectAbility_test_0400 ConnectAbility onConnect element.abilityName : '
                + JSON.stringify(element.abilityName))
                console.log('particleAbility_connectAbility_test_0400 ConnectAbility onConnect element.uri : '
                + JSON.stringify(element.uri))
                console.log('particleAbility_connectAbility_test_0400 ConnectAbility onConnect element.shortName : '
                + JSON.stringify(element.shortName))
                console.log('particleAbility_connectAbility_test_0400 ConnectAbility onConnect proxy : '
                + JSON.stringify(proxy));
            },
            onDisconnect: function (element1: any) {
                console.log('particleAbility_connectAbility_test_0400 ConnectAbility onDisconnect element.deviceId : '
                + JSON.stringify(element1.deviceId));
                console.log('particleAbility_connectAbility_test_0400 ConnectAbility onDisconnect element.bundleName: '
                + JSON.stringify(element1.bundleName));
                console.log('particleAbility_connectAbility_test_0400 ConnectAbility onDisconnect element.abilityName:'
                + JSON.stringify(element1.abilityName));
                console.log('particleAbility_connectAbility_test_0400 ConnectAbility onDisconnect element.uri : '
                + JSON.stringify(element1.uri));
                console.log('particleAbility_connectAbility_test_0400 ConnectAbility onDisconnect element.shortName : '
                + JSON.stringify(element1.shortName));
            },
            onFailed: function (code: any) {
                console.log('particleAbility_connectAbility_test_0400 ConnectAbility onFailed errCode:' + code);
            },
        }

        let connection_succeeded: any;
        console.info('particleAbility_connectAbility_test_0400  start ');
        var timeOldStamp = getNowTime();
        var connection = particleAbility.connectAbility(request, options);
        var timeNewStamp = getNowTime();
        getDurationTime('particleAbility_connectAbility_test_0400 connectability', timeOldStamp, timeNewStamp);
        connection_succeeded = connection;
        console.info('particleAbility_connectAbility_test_0400 service  request is:' + JSON.stringify(request));
        console.info('particleAbility_connectAbility_test_0400 options is:' + JSON.stringify(options));
        console.info('particleAbility_connectAbility_test_0400 connection=: ' + connection);
        sleep(5000);
        console.info('particleAbility_connectAbility_test_0400  disconnectability start ');
        var timeOldStamp = getNowTime();
        particleAbility.disconnectAbility(connection_succeeded).then((data: any) => {
            var timeNewStamp = getNowTime();
            getDurationTime('particleAbility_connectAbility_test_0400 disconnectability', timeOldStamp, timeNewStamp);
            console.info('particleAbility_connectAbility_test_0400 disconnectability succeeded: ' +
            JSON.stringify(data));
        }).catch((error: any) => {
            console.error('particleAbility_connectAbility_test_0400 disconnectability failed. Cause: ' +
            JSON.stringify(error));
        });
        sleep(5000);
        console.info('particleAbility_connectAbility_test_0400  disconnectability_fail start ');
        try {
            var timeOldStamp = getNowTime();
            particleAbility.disconnectAbility(-1).then((data: any) => {
                var timeNewStamp = getNowTime();
                getDurationTime('particleAbility_connectAbility_test_0400 disconnectability_fail',
                    timeOldStamp, timeNewStamp);
                console.info('particleAbility_connectAbility_test_0400 disconnectability_fail succeeded: ' +
                JSON.stringify(data));
            })
        }
        catch (error: any) {
            console.error('particleAbility_connectAbility_test_0400 disconnectability_fail failed. Cause: ' +
            JSON.stringify(error));
        }
    },
    onConnect(want) {
        console.info('ServiceAbility5 onConnect');
        return new StubTest5("test");
    },
};