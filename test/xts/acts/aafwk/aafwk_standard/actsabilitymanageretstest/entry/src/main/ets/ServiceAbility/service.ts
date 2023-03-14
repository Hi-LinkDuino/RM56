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

import rpc from '@ohos.rpc';

class StubTest extends rpc.RemoteObject {
    constructor(des) {
        super(des)
    }

    onRemoteRequest(code, data, reply, option) {
        console.info('ServiceAbility onRemoteRequest');
        if (code === 1) {
            let op1 = data.readInt();
            let op2 = data.readInt();
            reply.writeInt(op1 + op2);
            console.info('ServiceAbility op1:' + op1 + ' op2:' + op2);
        }
        return true;
    }
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
    },
    onConnect(want) {
        console.info('ServiceAbility onConnect');
        return new StubTest("test");
    },
    onDisConnect(want) {
        console.info('ServiceAbility onDisConnect');
    },
};
