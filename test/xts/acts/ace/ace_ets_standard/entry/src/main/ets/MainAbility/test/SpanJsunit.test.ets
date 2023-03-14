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

export default function spanJsunit() {

  function sleep(time) {
    return new Promise((resolve, reject) => {
      setTimeout(() => {
        resolve()
      }, time * 1000)
    }).then(() => {
      console.info(`sleep ${time} over...`)
    })
  }

  describe('SpanTest', function () {
    beforeEach(async function (done) {
      let options = {
        uri: 'pages/Span',
      }
      try {
        router.clear();
	await sleep(1);
        let pages = router.getState();
        console.info("get Span state success " + JSON.stringify(pages));
        if (!("Span" == pages.name)) {
          console.info("get Span state success " + JSON.stringify(pages.name));
          let result = await router.push(options)
          console.info("push Span page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push Span page error " + JSON.stringify(result));
      }
      await sleep(2)
      done()
    });

    afterEach(async function () {
      await sleep(1)
      console.info("Span after each called");
    });
    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_1080
     * @tc.name      testGetSpanAttr010
     * @tc.desc      aceEtsTest
     */
    it('testGetSpanAttr010', 0, async function (done) {
      console.info('testGetSpanAttr1 START');
      let strJson = getInspectorByKey('span1');
      let obj = JSON.parse(strJson);
      console.info("component obj is: " + JSON.stringify(obj));
      
      console.info("span1 decoration" + JSON.stringify(obj.$attrs.decoration))
      console.info("span1 textCase" + JSON.stringify(obj.$attrs.textCase))
      console.info("span1 content" + JSON.stringify(obj.$attrs.content))
      console.info("span1 fontSize" + JSON.stringify(obj.$attrs.fontSize))
      console.info("span1 letterSpacing" + JSON.stringify(obj.$attrs.letterSpacing))
      expect(obj.$attrs.decoration).assertEqual('{"type":"TextDecorationType.LineThrough","color":"#FFFF0000"}')
      expect(obj.$attrs.textCase).assertEqual('TextCase.UpperCase')
      expect(obj.$attrs.content).assertEqual('Test Span demo')
      console.info('testGetSpanAttr1 END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_1090
     * @tc.name      testGetSpanAttr020
     * @tc.desc      aceEtsTest
     */
    it('testGetSpanAttr020', 0, async function (done) {
      console.info('testGetSpanAttr2 START');
      let strJson = getInspectorByKey('span2');
      let obj = JSON.parse(strJson);
      console.info("component obj is: " + JSON.stringify(obj));
      console.info("span2 textCase" + JSON.stringify(obj.$attrs.textCase))
      console.info("span2 content" + JSON.stringify(obj.$attrs.content))
      expect(obj.$attrs.textCase).assertEqual('TextCase.LowerCase')
      expect(obj.$attrs.content).assertEqual('The second test Span demo')
      console.info('testGetSpanAttr2 END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_1100
     * @tc.name      testGetSpanAttr030
     * @tc.desc      aceEtsTest
     */
    it('testGetSpanAttr030', 0, async function (done) {
      console.info('testGetSpanAttr3 START');
      let strJson = getInspectorByKey('span3');
      let obj = JSON.parse(strJson);
      console.info("component obj is: " + JSON.stringify(obj));
      console.info("span3 textCase" + JSON.stringify(obj.$attrs.textCase))
      console.info("span3 content" + JSON.stringify(obj.$attrs.content))
      expect(obj.$attrs.textCase).assertEqual('TextCase.Normal')
      expect(obj.$attrs.content).assertEqual('The third test Span demo')
      console.info('testGetSpanAttr3 END');
      done();
    });
  })
}
