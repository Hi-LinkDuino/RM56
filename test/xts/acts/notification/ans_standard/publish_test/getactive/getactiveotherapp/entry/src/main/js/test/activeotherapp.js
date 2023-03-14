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

import notify from '@ohos.notification'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'
var time = 1000
describe('ActsAnsGetActiveOtherApp', function () {
    console.info("===========ActsAnsAllActiveTestOne start====================>");

    /*
    * @tc.number: Ans_GetAllActive_0100(publish)
    * @tc.name: getAllActiveNotifications(callback: AsyncCallback<Array<NotificationRequest>>): void;
    * @tc.desc: Send notification as other apps.
    */
    it('ActPublish_0100', 0, async function (done) {
        console.debug("===============ActPublish_0100 start==================>");
        await notify.removeAll();
        var notificationRequestOfOtherApp = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title_otherApp",
                    text: "test_text_otherApp",
                    additionalText: "test_additionalText_otherApp"
                },
            },
            id: 2,
            label: "otherApp",
        }
        await notify.publish(notificationRequestOfOtherApp);
        console.debug("===============ActPublish_0100 publish CurrentApp notify end==================>");
        done();
        setTimeout(async function(){
            console.debug("===============ActPublish_0100 done==================>");
        }, time);
    })

})

