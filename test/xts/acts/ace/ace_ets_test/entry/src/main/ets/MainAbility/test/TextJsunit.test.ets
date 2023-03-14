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

export default function textJsunit() {

  function sleep(time) {
    return new Promise((resolve, reject) => {
      setTimeout(() => {
        resolve()
      }, time * 1000)
    }).then(() => {
      console.info(`sleep ${time} over...`)
    })
  }

  describe('textTest', function () {

    beforeEach(async function (done) {
      let options = {
        uri: 'pages/Text',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get Text state success " + JSON.stringify(pages));
        if (!("Text" == pages.name)) {
          console.info("get Text state success " + JSON.stringify(pages.name));
          let result = await router.push(options)
          console.info("push Text page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push Text page error " + JSON.stringify(result));
      }
      await sleep(2)
      done()
    });

    afterEach(async function () {
      console.info("Text after each called")
      await sleep(1)
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0540
     * @tc.name      textTest001
     * @tc.desc      aceEtsTest
     */
    it('textTest001', 0, async function (done) {
      console.info('testGetInspectorByKey START');
      await sleep(2)
      let strJson = getInspectorByKey('Text');
      let obj = JSON.parse(strJson);
      console.info("cd text component obj is: " + JSON.stringify(obj));
      expect(obj.$type).assertEqual('Text')
      expect(obj.$attrs.width).assertEqual('100.000000vp')
      expect(obj.$attrs.padding).assertEqual('5.000000vp')
      expect(obj.$attrs.opacity).assertEqual(1)
      expect(obj.$attrs.align).assertEqual("Alignment.Center")
      expect(JSON.stringify(obj.$attrs.markAnchor)).assertEqual(JSON.stringify({
        "x": "50.000000vp", "y": "50.000000vp"
      }))
      expect(JSON.stringify(obj.$attrs.offset)).assertEqual(JSON.stringify({
        "x": "50.000000vp", "y": "50.000000vp"
      }))
      expect(obj.$attrs.flexGrow).assertEqual(5)
      expect(obj.$attrs.flexShrink).assertEqual(2)
      expect(obj.$attrs.alignSelf).assertEqual("ItemAlign.Auto")
      expect(obj.$attrs.borderStyle).assertEqual("BorderStyle.Dashed")
      expect(obj.$attrs.borderRadius).assertEqual("10.000000vp")
      expect(obj.$attrs.borderWidth).assertEqual("4.000000px")
      expect(obj.$attrs.borderColor).assertEqual("#FF0000FF")
      expect(obj.$attrs.backgroundColor).assertEqual("#FFFFC0CB")
      expect(obj.$attrs.enabled).assertEqual(false)
      expect(obj.$attrs.zIndex).assertEqual(1)
      expect(obj.$attrs.fontColor).assertEqual("#FF000000")
      expect(obj.$attrs.visibility).assertEqual("Visibility.Visible")
      expect(obj.$attrs.fontSize).assertEqual("15")
      expect(obj.$attrs.fontWeight).assertEqual("FontWeight.Bold")
      expect(obj.$attrs.fontStyle).assertEqual("FontStyle.Italic")
      expect(obj.$attrs.fontFamily).assertEqual("sans-serif")
      expect(obj.$attrs.textAlign).assertEqual("TextAlign.End")
      expect(obj.$attrs.textOverflow).assertEqual("TextOverflow.Ellipsis")
      expect(obj.$attrs.lineHeight).assertEqual("15")
      expect(obj.$attrs.baselineOffset).assertEqual("0")
      expect(obj.$attrs.maxLines).assertEqual("2")
      expect(obj.$attrs.textCase).assertEqual("TextCase.Normal")
      expect(obj.$attrs.decoration).assertEqual(JSON.stringify({
        type: "TextDecorationType.Underline", color: "#FFFF0000"
      }))
      expect(obj.$attrs.direction).assertEqual("Direction.Auto")
      expect(obj.$attrs.aspectRatio).assertEqual(1)
      expect(obj.$attrs.fontColor).assertEqual("#FF000000")
      console.info('cd textInspectorByKey END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0550
     * @tc.name      textTest002
     * @tc.desc      aceEtsTest
     */
    it('textTest002', 0, async function (done) {
      await sleep(2)
      let strJson = getInspectorByKey('Text1');
      let obj = JSON.parse(strJson);
      console.info("cd text1 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.direction).assertEqual("Direction.Ltr")
      expect(obj.$attrs.align).assertEqual("Alignment.Start")
      expect(obj.$attrs.alignSelf).assertEqual("ItemAlign.Start")
      expect(obj.$attrs.borderStyle).assertEqual("BorderStyle.Dotted")
      expect(obj.$attrs.enabled).assertEqual(true)
      expect(obj.$attrs.visibility).assertEqual("Visibility.Hidden")
      expect(obj.$attrs.fontWeight).assertEqual("FontWeight.Normal")
      expect(obj.$attrs.textAlign).assertEqual("TextAlign.Start")
      expect(obj.$attrs.textOverflow).assertEqual("TextOverflow.Clip")
      expect(obj.$attrs.decoration).assertEqual(JSON.stringify({
        type: "TextDecorationType.LineThrough", color: "#FFFF0000"
      }))
      expect(obj.$attrs.fontColor).assertEqual("#FFFFFFFF")
      console.info('cd text1InspectorByKey END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0560
     * @tc.name      textTest003
     * @tc.desc      aceEtsTest
     */
    it('textTest003', 0, async function (done) {
      await sleep(2)
      let strJson = getInspectorByKey('Text2');
      let obj = JSON.parse(strJson);
      console.info("cd text2 component obj is: " + JSON.stringify(obj));

      expect(obj.$attrs.direction).assertEqual("Direction.Rtl")
      expect(obj.$attrs.align).assertEqual("Alignment.TopEnd")
      expect(obj.$attrs.alignSelf).assertEqual("ItemAlign.Center")
      expect(obj.$attrs.borderStyle).assertEqual("BorderStyle.Solid")
      expect(obj.$attrs.visibility).assertEqual("Visibility.None")
      expect(obj.$attrs.fontWeight).assertEqual("FontWeight.Bolder")
      expect(obj.$attrs.textAlign).assertEqual("TextAlign.Center")
      expect(obj.$attrs.textCase).assertEqual("TextCase.UpperCase")
      expect(obj.$attrs.decoration).assertEqual(JSON.stringify({
        type: "TextDecorationType.None", color: "#FFFF0000"
      }))
      expect(obj.$attrs.fontColor).assertEqual("#FF0000FF")
      console.info('cd text2InspectorByKey END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0570
     * @tc.name      textTest004
     * @tc.desc      aceEtsTest
     */
    it('textTest004', 0, async function (done) {
      await sleep(2)
      let strJson = getInspectorByKey('Text3');
      let obj = JSON.parse(strJson);
      console.info("cd text3 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.align).assertEqual("Alignment.Top")
      expect(obj.$attrs.alignSelf).assertEqual("ItemAlign.End")
      expect(obj.$attrs.fontWeight).assertEqual("FontWeight.Lighter")
      expect(obj.$attrs.decoration).assertEqual(JSON.stringify({
        type: "TextDecorationType.Overline", color: "#FFFF0000"
      }))
      expect(obj.$attrs.fontColor).assertEqual("#FF808080")
      console.info('cd text3InspectorByKey END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0580
     * @tc.name      textTest005
     * @tc.desc      aceEtsTest
     */
    it('textTest005', 0, async function (done) {
      await sleep(2)
      let strJson = getInspectorByKey('Text4');
      let obj = JSON.parse(strJson);
      console.info("cd text4 component obj is: " + JSON.stringify(obj));

      expect(obj.$attrs.align).assertEqual("Alignment.TopStart")
      expect(obj.$attrs.alignSelf).assertEqual("ItemAlign.Baseline")
      expect(obj.$attrs.fontWeight).assertEqual("FontWeight.Medium")
      expect(obj.$attrs.fontColor).assertEqual("#FFA52A2A")
      console.info('cd text4InspectorByKey END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0590
     * @tc.name      textTest006
     * @tc.desc      aceEtsTest
     */
    it('textTest006', 0, async function (done) {
      await sleep(2)
      let strJson = getInspectorByKey('Text5');
      let obj = JSON.parse(strJson);
      console.info("cd text5 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.align).assertEqual("Alignment.End")
      expect(obj.$attrs.alignSelf).assertEqual("ItemAlign.Stretch")
      expect(obj.$attrs.fontWeight).assertEqual("FontWeight.Regular")
      expect(obj.$attrs.fontColor).assertEqual("#FF808080")
      console.info('cd text5InspectorByKey END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0600
     * @tc.name      textTest007
     * @tc.desc      aceEtsTest
     */
    it('textTest007', 0, async function (done) {
      await sleep(2)
      let strJson = getInspectorByKey('Text6');
      let obj = JSON.parse(strJson);
      console.info("cd text6 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.align).assertEqual("Alignment.BottomEnd")
      expect(obj.$attrs.fontColor).assertEqual("#FFFFA500")
      console.info('cd text6InspectorByKey END')
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0610
     * @tc.name      textTest008
     * @tc.desc      aceEtsTest
     */
    it('textTest008', 0, async function (done) {
      await sleep(2)
      let strJson = getInspectorByKey('Text7');
      let obj = JSON.parse(strJson);
      console.info("cd text7 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.align).assertEqual("Alignment.Bottom")
      expect(obj.$attrs.fontColor).assertEqual("#FFFFFF00")
      console.info('cd text7InspectorByKey END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0620
     * @tc.name      textTest009
     * @tc.desc      aceEtsTest
     */
    it('textTest009', 0, async function (done) {
      await sleep(2)
      let strJson = getInspectorByKey('Text8');
      let obj = JSON.parse(strJson);
      console.info("cd text8 component obj is: " + JSON.stringify(obj));
      console.log("cd text8 align" + JSON.stringify(obj.$attrs.align));
      expect(obj.$attrs.align).assertEqual("Alignment.BottomStart")
      console.info('cd text8InspectorByKey END');
      done();
    });
  })
}
