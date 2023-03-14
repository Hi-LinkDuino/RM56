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

// @ts-nocheck
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from "deccjsunit/index.ets"
import router from '@system.router';

export default function badgeJsunit() {

  function sleep(time) {
    return new Promise((resolve, reject) => {
      setTimeout(() => {
        resolve()
      }, time * 1000)
    }).then(() => {
      console.info(`sleep ${time} over...`)
    })
  }

  describe('badgeTest', function () {
    beforeEach(async function (done) {
      let options = {
        uri: 'pages/Badge',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get Badge state success " + JSON.stringify(pages));
        if (!("Badge" == pages.name)) {
          console.info("get Badge state success " + JSON.stringify(pages.name));
          let result = await router.push(options)
          console.info("push Badge page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push Badge page error " + JSON.stringify(result));
      }
      await sleep(2)
      done()
    });

    afterEach(async function () {
      await sleep(1)
      console.info("Badge after each called");
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0600
     * @tc.name      testBadge010
     * @tc.desc      aceEtsTest
     */
    it('testBadge010', 0, async function (done) {
      console.info('new Badge0 START');
      let strJson = getInspectorByKey('badge');
      let obj = JSON.parse(strJson);
      let style = JSON.parse(obj.$attrs.style)
      expect(obj.$type).assertEqual('Badge');
      console.log(JSON.stringify(obj.$type))
      console.info("yr badge0 type: " + JSON.stringify(obj.$type));
      console.info("component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.count).assertEqual('33');
      console.log("yr badge0 count-'33'" + JSON.stringify(obj.$attrs.count))
      expect(obj.$attrs.position).assertEqual('BadgePosition.Right');
      console.log("yr badge0 position-BadgePosition.Right" + JSON.stringify(obj.$attrs.position))
      expect(obj.$attrs.maxCount).assertEqual('99');
      console.log("yr badge0 maxCount-'99'" + JSON.stringify(obj.$attrs.maxCount))
      console.log("yr badge0 style.color" + style.color);
      console.log("yr badge0 style.fontSize" + style.fontSize);
      console.log("yr badge0 style.badgeSize" + style.badgeSize);
      expect(style.color).assertEqual('#FFFFFFFF');
      expect(style.fontSize).assertEqual('16.00fp');
      expect(style.badgeSize).assertEqual('20.00fp');
      console.log("badge0 style : " + JSON.stringify(obj.$attrs.style))
      console.info('testGetInspectorByKey END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0610
     * @tc.name      testBadge020
     * @tc.desc      aceEtsTest
     */
    it('testBadge020', 0, async function (done) {
      console.info('new Badge1 START');
      let strJson = getInspectorByKey('badge1');
      let obj = JSON.parse(strJson);
      let style = JSON.parse(obj.$attrs.style)
      console.info("component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.position).assertEqual('BadgePosition.RightTop');
      console.log("yr badge1 position-'BadgePosition.RightTop'" + JSON.stringify(obj.$attrs.position))
      console.log("yr badge0 style.fontSize" + style.fontSize);
      console.log("yr badge0 style.badgeSize" + style.badgeSize);
      expect(style.fontSize).assertEqual('16.00fp');
      expect(style.badgeSize).assertEqual('20.00fp');
      console.info('new Badge1 END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0620
     * @tc.name      testBadge030
     * @tc.desc      aceEtsTest
     */
    it('testBadge030', 0, async function (done) {
      console.info('new Badge2 START');
      let strJson = getInspectorByKey('badge2');
      let obj = JSON.parse(strJson);
      console.info("component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.position).assertEqual('BadgePosition.Left');
      console.log("yr badge2 position-'BadgePosition.Left'" + JSON.stringify(obj.$attrs.position))
      console.info('testGetInspectorByKey END');
      done();
    });
  }

  )
}
