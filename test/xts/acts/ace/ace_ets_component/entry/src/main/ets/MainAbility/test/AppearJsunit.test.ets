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
import events_emitter from '@ohos.events.emitter'
import Utils from './Utils.ets';

export default function appearJsunit() {
  describe('appearTest', function () {
    beforeEach(async function (done) {
      console.info("appear beforeEach start");
      let options = {
        uri: 'pages/appear',
      }
      let result;
      try {
        router.clear();
        let pages = router.getState();
        console.info("get appear state pages: " + JSON.stringify(pages));
        if (!("appear" == pages.name)) {
          console.info("get appear state pages.name: " + JSON.stringify(pages.name));
          result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push appear page result: " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push appear page error: " + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("appear after each called");
    })

    it('testAppear01', 0, async function (done) {
      console.info('[testAppear01] START');
      await Utils.sleep(1000);
      try {
        let callBackAppear = (backData) => {
          console.info("testAppear01 get appearEvent result is: " + JSON.stringify(appearEvent));
          expect(backData.data.ACTION).assertEqual(true);
        }
        var appearEvent = {
          eventId: 58,
          priority: events_emitter.EventPriority.LOW
        }
        events_emitter.on(appearEvent, callBackAppear);
      } catch (err) {
        console.info("testAppear01 on appearEvent err : " + JSON.stringify(err));
      }
      console.info("testAppear01 click result is: " + JSON.stringify(sendEventByKey('appear', 10, "")));
      console.info('[testAppear01] END');
      done();
    });

    it('testAppear02', 0, async function (done) {
      console.info('[testAppear02] START');
      await Utils.sleep(1000);
      try {
        let callBackAppear = (backData) => {
          console.info("testAppear02 get appearEvent result is: " + JSON.stringify(appearEvent));
          expect(backData.data.ACTION).assertEqual(true);
        }
        var appearEvent = {
          eventId: 59,
          priority: events_emitter.EventPriority.LOW
        }
        events_emitter.on(appearEvent, callBackAppear);
      } catch (err) {
        console.info("testAppear02 on appearEvent err : " + JSON.stringify(err));
      }
      console.info("testAppear02 appearEvent click result is: " + JSON.stringify(sendEventByKey('appear', 10, "")));

      try {
        let callBackDisAppear = (backData) => {
          console.info("testAppear02 get disAppearEvent result is: " + JSON.stringify(disAppearEvent));
          expect(backData.data.ACTION).assertEqual(true);
        }
        var disAppearEvent = {
          eventId: 60,
          priority: events_emitter.EventPriority.LOW
        }
        events_emitter.on(disAppearEvent, callBackDisAppear);
      } catch (err) {
        console.info("testAppear02 on disAppearEvent err : " + JSON.stringify(err));
      }
      console.info("testAppear02 disAppearEvent click result is: " + JSON.stringify(sendEventByKey('appear', 10, "")));
      console.info('[testAppear02] END');
      done();
    });

  })
}
