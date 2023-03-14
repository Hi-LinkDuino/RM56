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

class StubTest6 extends rpc.RemoteObject {
    constructor(des) {
        super(des)
    }

    onRemoteRequest(code, data, reply, option) {
        console.info('ServiceAbility6 onRemoteRequest');
        if (code === 1) {
            let op1 = data.readInt();
            let op2 = data.readInt();
            reply.writeInt(op1 + op2);
            console.info('ServiceAbility6 op1:' + op1 + ' op2:' + op2);
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

export default {
    onStart() {
        console.info('ServiceAbility onStart');
    },
    onStop() {
        console.info('ServiceAbility onStop');
    },
    onCommand(want, startId) {
        console.info('ServiceAbility onCommand');

        let request = {
            "bundleName":"com.ohos.acecollaboration",
            "abilityName":"com.ohos.acecollaboration.xxx",
        }
        let options = {
            onConnect:async function (element:any, proxy:any) {
                console.log('particleAbility_connectAbility_test_0500 ConnectAbility onConnect element.deviceId : '
                + JSON.stringify(element.deviceId))
                console.log('particleAbility_connectAbility_test_0500 ConnectAbility onConnect element.bundleName : '
                + JSON.stringify(element.bundleName))
                console.log('particleAbility_connectAbility_test_0500 ConnectAbility onConnect element.abilityName : '
                + JSON.stringify(element.abilityName))
                console.log('particleAbility_connectAbility_test_0500 ConnectAbility onConnect element.uri : '
                + JSON.stringify(element.uri))
                console.log('particleAbility_connectAbility_test_0500 ConnectAbility onConnect element.shortName : '
                + JSON.stringify(element.shortName))
                console.log('particleAbility_connectAbility_test_0500 ConnectAbility onConnect proxy : '
                + JSON.stringify(proxy));
            },
            onDisconnect:function (element1:any) {
                console.log('particleAbility_connectAbility_test_0500 ConnectAbility onDisconnect element.deviceId : '
                + JSON.stringify(element1.deviceId));
                console.log('particleAbility_connectAbility_test_0500 ConnectAbility onDisconnect element.bundleName: '
                + JSON.stringify(element1.bundleName));
                console.log('particleAbility_connectAbility_test_0500 ConnectAbility onDisconnect element.abilityName:'
                + JSON.stringify(element1.abilityName));
                console.log('particleAbility_connectAbility_test_0500 ConnectAbility onDisconnect element.uri : '
                + JSON.stringify(element1.uri));
                console.log('particleAbility_connectAbility_test_0500 ConnectAbility onDisconnect element.shortName : '
                + JSON.stringify(element1.shortName));
            },
            onFailed:function (code:any) {
                console.log('particleAbility_connectAbility_test_0500 ConnectAbility  service onFailed errCode:' + code)
            },
        }

        console.info('particleAbility_connectAbility_test_0500 errorabilityName start');
        var timeOldStamp = getNowTime();
        var connection = particleAbility.connectAbility(request, options);
        var timeNewStamp = getNowTime();
        getDurationTime('particleAbility_connectAbility_test_0500 connectability', timeOldStamp, timeNewStamp);
        console.info('particleAbility_connectAbility_test_0500 errorabilityName request is:'
        + JSON.stringify(request));
        console.info('particleAbility_connectAbility_test_0500 errorabilityName options is:'
        + JSON.stringify(options));
        console.info('particleAbility_connectAbility_test_0500 errorabilityName connection=: ' + connection);
    },
    onConnect(want) {
        console.info('ServiceAbility6 onConnect');
        return new StubTest6("test");
    },
};