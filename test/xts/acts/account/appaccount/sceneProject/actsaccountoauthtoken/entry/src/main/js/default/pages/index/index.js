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
import appAccount from '@ohos.account.appAccount';
import featureAbility from '@ohos.ability.featureAbility';
import rpc from '@ohos.rpc';


class MyAuthenticator extends appAccount.Authenticator
{
    authenticate(name, authType, callerBundleName, options, callback) {
        var randNumber = Math.ceil(Math.random()*10000)
        var newTokenData = "service/authenticate/tokenInfo" + randNumber;
        console.log("Service authenticate name: " + name + ", authType: " + authType + ", options: "+ JSON.stringify(options));
        callback.onResult(0, {"name":name,"authType":authType,"token":newTokenData});
    }

    addAccountImplicitly(authType, callerBundleName, options, callback) {
        console.log("Service addAccountImplicitly authType: " + authType + " callerBundleName: " + callerBundleName  + "options: "+ JSON.stringify(options));
        callback.onRequestRedirected({
            bundleName: "com.huawei.openharmonyappdemo.service",
            abilityName: "com.huawei.openharmonyappdemo.service.settingAbility",
        });
    }
}


export default {
    onStart(want) {
        console.info('ServiceAbility onStart');
    },
    onStop() {
        console.info('ServiceAbility onStop');
    },
    onConnect(want) {
        console.info('ServiceAbility onConnect start');
        try{
            console.info("ServiceAbility want: " + typeof(want));
            console.info("ServiceAbility want: " + JSON.stringify(want));
            console.info("ServiceAbility want name: " + want.bundleName);
        }catch(err){
            console.info("ServiceAbility error: " + err);
        }
        console.info('ServiceAbility onConnect start');
        return new MyAuthenticator();
    },
    onReconnect(want) {
        console.info('ServiceAbility onReconnect');
    },
    onDisconnect(want) {
        console.info('ServiceAbility onDisconnect');
        console.info("ServiceAbility want: " + JSON.stringify(want));
    },
    onCommand(want, startId) {
        console.info('ServiceAbility onCommand');
        console.info("ServiceAbility want: " + JSON.stringify(want));
        console.info("ServiceAbility startId : " + startId);
    }
};