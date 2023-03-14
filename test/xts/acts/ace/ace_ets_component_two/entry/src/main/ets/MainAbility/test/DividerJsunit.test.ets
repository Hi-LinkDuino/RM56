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

export default function dividerJsunit() {
  describe('dividerTest', function () {
    beforeEach(async function (done) {
      let options = {
        uri: 'pages/divider',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get divider state success " + JSON.stringify(pages));
        if (!("divider" == pages.name)) {
          console.info("get divider state success " + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(1000);
          console.info("push divider page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push divider page error: " + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("divider after each called");
    });

    it('dividerTest_0100', 0, async function (done) {
      console.info('dividerTest_0100 START');
      let strJson = getInspectorByKey('Divider');
      console.info("dividerTest_0100 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("dividerTest_0100 component obj is: " + JSON.stringify(obj));
      expect(obj.$type).assertEqual('Divider');
      console.info('dividerTest_0200 END');
      done();
    });

    it('dividerTest_0200', 0, async function (done) {
      console.info('dividerTest_0200 START');
      let strJson = getInspectorByKey('Divider');
      console.info("dividerTest_0200 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("dividerTest_0200 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.color).assertEqual('#FFCCCCCC');
      console.info('dividerTest_0200 END');
      done();
    });

    it('dividerTest_0300', 0, async function (done) {
      console.info('dividerTest_0300 START');
      try {
        let eventData = {
          data: {
            "color": '#FF000990',
          }
        }
        let indexEvent = {
          eventId: 85,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("dividerTest_0300 start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("dividerTest_0300 change component data error: " + err.message);
      }
      await Utils.sleep(3000);
      let strJsonNew = getInspectorByKey('Divider');
      let objNew = JSON.parse(strJsonNew);
      console.info("dividerTest_0300 component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.color).assertEqual('#FF000990');
      console.info('dividerTest_0300 END');
      done();
    });
  })
}
