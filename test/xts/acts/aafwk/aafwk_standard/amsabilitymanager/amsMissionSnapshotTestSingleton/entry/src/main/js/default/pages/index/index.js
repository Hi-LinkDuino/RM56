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
export default {
    data: {
        title: ''
    },
    onInit() {
        this.title = this.$t('strings.world');
    },
    OnActive(){
        console.debug('ACTS_MissionSnapshot_OnActive');
        commonEvent.publish("ACTS_MissionSnapshot_OnActive", (err,data) =>{
            console.debug("=ACTS_MissionSnapshot_OnActive err,data=======>"
                    + ("json err【") + JSON.stringify(err) + (" 】")
                    + ("json data【") + JSON.stringify(data) + (" 】")
                    + " ,err=" + err + " ,data=" + data);
        });
    },
    onShow() {
        console.debug('ACTS_MissionSnapshot_onShow');
        setTimeout(()=>{
            commonEvent.publish("ACTS_MissionSnapshot_onShow", (err,data) =>{
                console.debug("=ACTS_MissionSnapshot_onShow err,data=======>"
                        + ("json err【") + JSON.stringify(err) + (" 】")
                        + ("json data【") + JSON.stringify(data) + (" 】")
                        + " ,err=" + err + " ,data=" + data);
            });
        }, 3000);
    },
}