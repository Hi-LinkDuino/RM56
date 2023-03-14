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
import events_emitter from '@ohos.events.emitter';
import Utils from './Utils';

export default function searchJsunit() {
  describe('appInfoTest', function () {
    beforeEach(async function (done) {
      let options = {
        uri: 'pages/search',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get search state success " + JSON.stringify(pages));
        if (!("span" == pages.name)) {
          console.info("get search success " + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push search page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push search page error " + JSON.stringify(err));
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(2000);
      console.info("search after each called");
    });

    it('testSearch001', 0, async function (done) {
      console.info('[testSearch001] START');
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('search');
      console.info("[testSearch001] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      expect(obj.$attrs.searchButton).assertEqual('10');
      done();
    });

    it('testSearch002', 0, async function (done) {
      console.info('[testSearch002] START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "searchButton": "5"
          }
        }
        var innerEvent = {
          eventId: 190,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testSearch002] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testSearch002] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJsonNew = getInspectorByKey('search');
      let objNew = JSON.parse(strJsonNew);
      console.info("[testSearch002] component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.searchButton).assertEqual('5');
      done();
    });
  })
}
