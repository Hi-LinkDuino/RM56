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

export default function swiperJsunit() {

  function sleep(time) {
    return new Promise((resolve, reject) => {
      setTimeout(() => {
        resolve()
      }, time * 1000)
    }).then(() => {
      console.info(`sleep ${time} over...`)
    })
  }
  //返回首页
  async function backToIndex() {
    let backToIndexPromise = new Promise((resolve, reject) => {
      console.info('afterEach backToIndexPromise ...');
      setTimeout(() => {
        router.back({
          uri: 'pages/index'
        });
        resolve();
      }, 500);
    });

    let clearPromise = new Promise((resolve, reject) => {
      console.info('afterEach clearPromise ...');
      setTimeout(() => {
        router.clear();
        resolve();
      }, 500);
    });
    await backToIndexPromise.then(() => {
      return clearPromise;
    });
  }

  describe('swiperTest', function () {

    beforeEach(async function (done) {
      let options = {
        uri: 'pages/Swiper',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get Swiper state success " + JSON.stringify(pages));
        if (!("Swiper" == pages.name)) {
          console.info("get Swiper state success " + JSON.stringify(pages.name));
          let result = await router.push(options)
          console.info("push Swiper page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push Swiper page error " + JSON.stringify(result));
      }
      await sleep(2)
      done()
    });

    afterEach(async function () {
      await sleep(1)
      console.info("Swiper after each called");
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0500
     * @tc.name      swiperTest001
     * @tc.desc      aceEtsTest
     */
    it('swiperTest001', 0, async function (done) {
      console.info('++++++++++++++++ Swiper TestCase 1 START ++++++++++++++++');
      await sleep(1)
      let strJson = getInspectorByKey('swiper');
      let obj = JSON.parse(strJson);
      console.info("get inspector value is: " + JSON.stringify(obj));
      console.log(JSON.stringify(obj.$type))
      expect(obj.$type).assertEqual('Swiper')
      console.log('Swiper‘s index is ' + JSON.stringify(obj.$attrs.index))
      console.log('Swiper‘s autoPlay is ' + JSON.stringify(obj.$attrs.autoPlay))
      console.log('Swiper‘s interval is ' + JSON.stringify(obj.$attrs.interval))
      console.log('Swiper‘s indicator is ' + JSON.stringify(obj.$attrs.indicator))
      console.log('Swiper‘s loop is ' + JSON.stringify(obj.$attrs.loop))
      console.log('Swiper‘s duration is ' + JSON.stringify(obj.$attrs.duration))
      console.log('Swiper‘s vertical is ' + JSON.stringify(obj.$attrs.vertical))
      console.log('Swiper‘s itemSpace is ' + JSON.stringify(obj.$attrs.itemSpace))
      console.log('Swiper‘s disableSwipe is ' + JSON.stringify(obj.$attrs.disableSwipe))
      expect(obj.$attrs.index).assertEqual('1')
      expect(obj.$attrs.autoPlay).assertEqual('true')
      expect(obj.$attrs.interval).assertEqual('4000')
      expect(obj.$attrs.indicator).assertEqual('true')
      expect(obj.$attrs.loop).assertEqual('false')
      expect(obj.$attrs.vertical).assertEqual('false')
      expect(obj.$attrs.duration).assertEqual(1000.000000.toFixed(6))
      console.info('++++++++++++++++ Swiper TestCase 1 End +++++++++++++++++');
      done();
    });
  })
}
