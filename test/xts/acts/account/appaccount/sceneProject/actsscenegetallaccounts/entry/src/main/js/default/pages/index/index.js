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
import account from '@ohos.account.appAccount'

const injectRef = Object.getPrototypeOf(global) || global
injectRef.regeneratorRuntime = require('@babel/runtime/regenerator')

export default {
    data: {
    },
    onInit() {
        this.title = "scene getAllAccounts";
    },
    onShow() {
        console.debug('====>scene getAllAccounts start====');
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat scene manager finish====");
        var enableBundle = "com.example.actsgetallaaccounts";
        console.debug("====>add account scene start====");
        appAccountManager.addAccount("account_name_scene_single", (err)=>{
            console.debug("====>add account scene err:" + JSON.stringify(err));
            appAccountManager.enableAppAccess("account_name_scene_single", enableBundle, (err)=>{
                console.debug("====>enableAppAccess scene err:" + JSON.stringify(err));
            });
        });
    },
    onReady() {
    },
}