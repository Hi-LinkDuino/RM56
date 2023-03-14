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
import Utils from './Utils';

export default function patternLockJsunit() {
  describe('appInfoTest', function () {
    beforeEach(async function (done) {
      console.info("patternLock beforeEach start");
      let options = {
        uri: 'pages/patternLock',
      }
      let result;
      try {
        router.clear();
        let pages = router.getState();
        console.info("get patternLock state pages: " + JSON.stringify(pages));
        if (!("button" == pages.name)) {
          console.info("get patternLock state pages.name: " + JSON.stringify(pages.name));
          result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push patternLock page result: " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push patternLock page error: " + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("patternLock after each called");
    })

    it('testPatternLock_01', 0, async function (done) {
      console.info('testPatternLock_01 START');
      await Utils.sleep(1500);
      let callback = (indexEvent) => {
        console.info("testPatternLock_01 get state result is: " + JSON.stringify(indexEvent));
        expect(indexEvent.data.ACTION).assertEqual(true);
      }
      let indexEvent = {
        eventId: 153,
        priority: events_emitter.EventPriority.LOW
      }
      try {
        events_emitter.on(indexEvent, callback);
      } catch (err) {
        console.info("testPatternLock_01 on events_emitter err : " + JSON.stringify(err));
      }
      console.info("testPatternLock_01 click result is: " + JSON.stringify(sendEventByKey('button1',10,"")));
      await Utils.sleep(1000);
      console.info('testPatternLock_01 END');
      done();
    });
  })
}
