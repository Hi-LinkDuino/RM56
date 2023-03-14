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

export default function navigationJsunit() {
  describe('appInfoTest', function () {
    beforeEach(async function (done) {
      console.info("navigation beforeEach start");
      let options = {
        uri: 'pages/navigation',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get navigation state pages:" + JSON.stringify(pages));
        if (!("navigation" == pages.name)) {
          console.info("get navigation state pages.name:" + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push navigation page result:" + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push navigation page error:" + JSON.stringify(err));
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("navigation after each called");
    });

    it('testNavigation_100', 0, async function (done) {
      console.info('[testNavigation_100] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('Navigation');
      let obj = JSON.parse(strJson);
      console.info("[testNavigation_100] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.width).assertEqual("100.00%");
      console.info('[testNavigation_100] END');
      done();
    });

    it('testNavigation_200', 0, async function (done) {
      console.info('testNavigation_200 START');
      await Utils.sleep(1000);
      let callback = (indexEvent) => {
        console.info("testNavigation_200 get state result is: " + JSON.stringify(indexEvent));
        expect(indexEvent.data.ACTION).assertEqual(false);
      }
      let indexEvent = {
        eventId: 136,
        priority: events_emitter.EventPriority.LOW
      }
      try {
        events_emitter.on(indexEvent, callback);
      } catch (err) {
        console.info("testNavigation_200 on events_emitter err : " + JSON.stringify(err));
      }
      console.info("testNavigation_200 click result is: " + JSON.stringify(sendEventByKey('NavigationContent1',10,"")));
      await Utils.sleep(2000);
      console.info('testNavigation_200 END');
      done();
    });

    it('testNavigation_300', 0, async function (done) {
      console.info('[testNavigation_300] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('Navigation');
      let obj = JSON.parse(strJson);
      console.info("[testNavigation_300] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.height).assertEqual("300.00vp");
      console.info('[testNavigation_300] END');
      done();
    });

    it('testNavigation_400', 0, async function (done) {
      console.info('[testNavigation_400] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('Navigation');
      let obj = JSON.parse(strJson);
      console.info("[testNavigation_400] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.align).assertEqual("Alignment.Center");
      console.info('[testNavigation_400] END');
      done();
    });
  })
}