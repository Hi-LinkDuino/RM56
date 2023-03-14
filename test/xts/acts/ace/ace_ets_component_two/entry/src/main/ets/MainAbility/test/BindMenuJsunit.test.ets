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
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from "deccjsunit/index.ets";
import router from '@system.router';
import events_emitter from '@ohos.events.emitter';
import Utils from './Utils';

export default function aboutToDisappearJsunit() {
  describe('bindMenuTest', function () {
    beforeEach(async function (done) {
      console.info("bindMenu beforeEach start");
      let options = {
        uri: 'pages/bindMenu',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get bindMenu state pages:" + JSON.stringify(pages));
        if (!("bindMenu" == pages.name)) {
          console.info("get bindMenu state pages.name:" + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push bindMenu page result:" + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push bindMenu page error:" + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("bindMenu after each called");
    });

    it('bindMenuTest_0100', 0, async function (done) {
      console.info('bindMenuTest_0100 START');
      console.info("bindMenuTest_0100 text click result is: " + JSON.stringify(sendEventByKey('text',10,"")));
      await Utils.sleep(1500);
      let callback = (indexEvent) => {
        console.info("bindMenuTest_0100 get state result is: " + JSON.stringify(indexEvent));
        expect(indexEvent.data.ACTION).assertEqual(true);
      }
      let indexEvent = {
        eventId: 64,
        priority: events_emitter.EventPriority.LOW
      }
      try {
        events_emitter.on(indexEvent, callback);
      } catch (err) {
        console.info("bindMenuTest_0100 on events_emitter err : " + JSON.stringify(err));
      }
      console.info("bindMenuTest_0100 button click result is: " + JSON.stringify(sendEventByKey('button',10,"")));
      await Utils.sleep(1000);
      console.info('bindMenuTest_0100 END');
      done();
    });
  })
}