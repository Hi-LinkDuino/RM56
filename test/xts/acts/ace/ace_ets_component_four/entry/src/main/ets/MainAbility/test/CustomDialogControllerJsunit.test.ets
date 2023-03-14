// @ts-nocheck
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
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from "deccjsunit/index.ets"
import router from '@system.router';
import Utils from './Utils';
import events_emitter from '@ohos.events.emitter';

export default function customDialogControllerJsunit() {
  describe('customDialogControllerTest', function () {
    beforeEach(async function (done) {
      let options = {
        uri: 'pages/customDialogController',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get customDialogController state success " + JSON.stringify(pages));
        if (!("customDialogController" == pages.name)) {
          console.info("get customDialogController state success " + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(4000);
          console.info("push customDialogController page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push customDialogController page error " + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("customDialogControllers after each called");
    });

    it('testCustomDialogControllers_100', 0, async function (done) {
      console.info('[testCustomDialogControllers_100] START');
      var callback = (eventData) => {
        console.info("[testClickEvent001] get event state result is: " + JSON.stringify(eventData));
        expect(eventData.data.ACTION).assertEqual(true)
      }
      var innerEvent = {
        eventId: 236,
        priority: events_emitter.EventPriority.LOW
      }
      try {
        events_emitter.on(innerEvent, callback)
      } catch (err) {
        console.info("[testClickEvent001] on events_emitter err : " + JSON.stringify(err));
      }
      console.info('[testClickEvent001] sendEventByKey ' + JSON.stringify(sendEventByKey('button', 10, "")));
      await Utils.sleep(1000)
      console.info('[testClickEvent001] testSendTouchEvent END');
      done();
    });

  })
}