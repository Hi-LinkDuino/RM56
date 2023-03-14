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
import CommonEvent from '@ohos.commonevent'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe('Actspublish_0100', function () {
    console.info("===========ActspublishAsUser start====================>");
   
     it('Actspublishr_0100', 0, async function (done) {
        function PublishCallBack(err) {
            if (err.code) {
                console.error("publish failed " + JSON.stringify(err));
            } else {

                console.info("publish success");
                expect(event).assertEqual("event_test");
            }
        }
        var event = "event_test"
        CommonEvent.publishAsUser(event, 100, PublishCallBack);
        done();
    })
})



