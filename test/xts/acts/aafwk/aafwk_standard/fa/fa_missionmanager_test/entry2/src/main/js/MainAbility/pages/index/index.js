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
import router from '@system.router'
import {Core} from 'deccjsunit/index'
import commonEvent from '@ohos.commonEvent';

export default {
    data: {
        title: "",
        text: ""
    },
    onInit() {
        this.title = this.$t('strings.world22222');
        this.text = "This is entry MainAbility"
    },

    onShow() {
        console.info("singletonEntry2Ability1_onShow");
        commonEvent.publish("singletonEntry2Ability1_onShow", () =>{
            console.log("singletonEntry2Ability1 publish callback singletonEntry2Ability1_onShow")
        });
    },

    onHide(){
        console.info("singletonEntry2Ability1_onHide");
        commonEvent.publish("singletonEntry2Ability1_onHide", () =>{
            console.log("singletonEntry2Ability1 publish callback singletonEntry2Ability1_onHide")
        });
    },

    onActive(){
        console.info("singletonEntry2Ability1_onActive");
        commonEvent.publish("singletonEntry2Ability1_onActive", () =>{
            console.log("singletonEntry2Ability1 publish callback singletonEntry2Ability1_onActive")
        });

    },

    onInactive(){
        console.info("singletonEntry2Ability1_onInactive");
        commonEvent.publish("singletonEntry2Ability1_onInactive", () =>{
            console.log("singletonEntry2Ability1 publish callback singletonEntry2Ability1_onInactive")
        });
    },

    onclick: function () {
        router.replace({
            uri: "pages/second/second"
        })
    }

}



