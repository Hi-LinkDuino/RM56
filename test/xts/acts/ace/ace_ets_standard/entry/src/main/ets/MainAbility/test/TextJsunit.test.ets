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
      await sleep(1)
      console.info("Text after each called");
    });
    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_1200
     * @tc.name      textGetInspectorByKey010
     * @tc.desc      aceEtsTest
     */
    it('textGetInspectorByKey010', 0, async function (done) {
      console.info('testGetInspectorByKey START');
      await sleep(3)
      let strJson = getInspectorByKey('Text');
      let obj = JSON.parse(strJson);
      console.info("cd text component obj is: " + JSON.stringify(obj));
      console.log("cd text type" + JSON.stringify(obj.$type));
      console.log("cd text height" + JSON.stringify(obj.$attrs.height));
      console.log("cd text width" + JSON.stringify(obj.$attrs.width));
      console.log("cd text padding" + JSON.stringify(obj.$attrs.padding));
      console.log("cd text constraintSize" + JSON.stringify(obj.$attrs.constraintSize));
      console.log("cd text opacity" + JSON.stringify(obj.$attrs.opacity));
      console.log("cd text align" + JSON.stringify(obj.$attrs.align));
      console.log("cd text markAnchor type1" + typeof ({
        "x": "50.00vp", "y": "50.00vp"
      }));
      console.log("cd text markAnchor1" + JSON.stringify({
        "x": "50.00vp", "y": "50.00vp"
      }));
      console.log("cd text markAnchor type" + typeof (obj.$attrs.markAnchor));
      console.log("cd text markAnchor" + JSON.stringify(obj.$attrs.markAnchor));
      console.log("cd text offset" + JSON.stringify(obj.$attrs.offset));
      console.log("cd text position" + JSON.stringify(obj.$attrs.position));
      console.log("cd text flexGrow" + JSON.stringify(obj.$attrs.flexGrow));
      console.log("cd text flexShrink" + JSON.stringify(obj.$attrs.flexShrink));
      console.log("cd text alignSelf" + JSON.stringify(obj.$attrs.alignSelf));
      console.log("cd text borderStyle" + JSON.stringify(obj.$attrs.borderStyle));
      console.log("cd text borderRadius" + JSON.stringify(obj.$attrs.borderRadius));
      console.log("cd text borderWidth" + JSON.stringify(obj.$attrs.borderWidth));
      console.log("cd text borderColor" + JSON.stringify(obj.$attrs.borderColor));
      console.log("cd text backgroundColor" + JSON.stringify(obj.$attrs.backgroundColor));
      console.log("cd text enabled" + JSON.stringify(obj.$attrs.enabled));
      console.log("cd text zIndex" + JSON.stringify(obj.$attrs.zIndex));
      console.log("cd text fontColor" + JSON.stringify(obj.$attrs.fontColor));
      console.log("cd text visibility" + JSON.stringify(obj.$attrs.visibility));
      console.log("cd text fontSize" + JSON.stringify(obj.$attrs.fontSize));
      console.log("cd text minFontSize" + JSON.stringify(obj.$attrs.minFontSize));
      console.log("cd text letterSpacing" + JSON.stringify(obj.$attrs.letterSpacing));
      console.log("cd text maxFontSize" + JSON.stringify(obj.$attrs.maxFontSize));
      console.log("cd text fontWeight" + JSON.stringify(obj.$attrs.fontWeight));
      console.log("cd text fontStyle" + JSON.stringify(obj.$attrs.fontStyle));
      console.log("cd text fontFamily" + JSON.stringify(obj.$attrs.fontFamily));
      console.log("cd text textAlign" + JSON.stringify(obj.$attrs.textAlign));
      console.log("cd text textOverflow" + JSON.stringify(obj.$attrs.textOverflow));
      console.log("cd text lineHeight" + JSON.stringify(obj.$attrs.lineHeight));
      console.log("cd text baselineOffset" + JSON.stringify(obj.$attrs.baselineOffset));
      console.log("cd text maxLines" + JSON.stringify(obj.$attrs.maxLines));
      console.log("cd text textCase" + JSON.stringify(obj.$attrs.textCase));
      console.log("cd text decoration" + JSON.stringify(obj.$attrs.decoration));
      console.log("cd text decoration1" + JSON.stringify(JSON.stringify({
        type: "TextDecorationType.Underline", color: "#FFFF0000"
      })));
      console.log("cd text direction" + JSON.stringify(obj.$attrs.direction));
      console.log("cd text aspectRatio" + JSON.stringify(obj.$attrs.aspectRatio));
      console.log("cd text fontColor" + JSON.stringify(obj.$attrs.fontColor));
      expect(obj.$type).assertEqual('Text')
      expect(obj.$attrs.width).assertEqual('200.00vp')
      expect(obj.$attrs.padding).assertEqual('5.00vp')
      expect(obj.$attrs.margin).assertEqual('10.00px')
      expect(obj.$attrs.opacity).assertEqual(1)
      expect(obj.$attrs.align).assertEqual("Alignment.Center")
      expect(JSON.stringify(obj.$attrs.markAnchor)).assertEqual(JSON.stringify({
        "x": "50.00vp", "y": "50.00vp"
      }))
      expect(JSON.stringify(obj.$attrs.offset)).assertEqual(JSON.stringify({
        "x": "50.00vp", "y": "50.00vp"
      }))
      expect(obj.$attrs.flexGrow).assertEqual(5)
      expect(obj.$attrs.flexShrink).assertEqual(2)
      expect(obj.$attrs.alignSelf).assertEqual("ItemAlign.Auto")
      expect(obj.$attrs.borderStyle).assertEqual("BorderStyle.Dashed")
      expect(obj.$attrs.borderRadius).assertEqual("10.00vp")
      expect(obj.$attrs.borderWidth).assertEqual("4.00px")
      expect(obj.$attrs.borderColor).assertEqual("#FF0000FF")
      expect(obj.$attrs.backgroundColor).assertEqual("#FFFFC0CB")
      expect(obj.$attrs.enabled).assertEqual(false)
      expect(obj.$attrs.zIndex).assertEqual(1)
      expect(obj.$attrs.fontColor).assertEqual("#FF000000")
      expect(obj.$attrs.visibility).assertEqual("Visibility.Visible")
      expect(obj.$attrs.fontSize).assertEqual("15.00px")
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
     * @tc.number    SUB_ACE_BASIC_ETS_API_1210
     * @tc.name      textGetInspectorByKey020
     * @tc.desc      aceEtsTest
     */
    it('textGetInspectorByKey020', 0, async function (done) {
      let strJson = getInspectorByKey('Text1');
      let obj = JSON.parse(strJson);
      console.info("cd text1 component obj is: " + JSON.stringify(obj));
      console.log("cd text1 align" + JSON.stringify(obj.$attrs.align));
      console.log("cd text1 alignSelf" + JSON.stringify(obj.$attrs.alignSelf));
      console.log("cd text1 borderStyle" + JSON.stringify(obj.$attrs.borderStyle));
      console.log("cd text1 enabled" + JSON.stringify(obj.$attrs.enabled));
      console.log("cd text1 visibility" + JSON.stringify(obj.$attrs.visibility));
      console.log("cd text1 fontWeight" + JSON.stringify(obj.$attrs.fontWeight));
      console.log("cd text1 textAlign" + JSON.stringify(obj.$attrs.textAlign));
      console.log("cd text1 textOverflow" + JSON.stringify(obj.$attrs.textOverflow));
      console.log("cd text1 direction" + JSON.stringify(obj.$attrs.direction));
      console.log("cd text1 textCase" + JSON.stringify(obj.$attrs.textCase));
      console.log("cd text1 decoration" + JSON.stringify(obj.$attrs.decoration));
      console.log("cd text1 decoration1" + JSON.stringify(JSON.stringify({
        type: "TextDecorationType.LineThrough", color: "#FFFF0000"
      })));
      console.log("cd text1 fontColor" + JSON.stringify(obj.$attrs.fontColor));
      console.log("cd text padding" + JSON.stringify(obj.$attrs.padding));
      console.log("cd text margin" + JSON.stringify(obj.$attrs.margin))
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
     * @tc.number    SUB_ACE_BASIC_ETS_API_1220
     * @tc.name      textGetInspectorByKey030
     * @tc.desc      aceEtsTest
     */
    it('textGetInspectorByKey030', 0, async function (done) {
      let strJson = getInspectorByKey('Text2');
      let obj = JSON.parse(strJson);
      console.info("cd text2 component obj is: " + JSON.stringify(obj));
      console.log("cd text2 align" + JSON.stringify(obj.$attrs.align));
      console.log("cd text2 alignSelf" + JSON.stringify(obj.$attrs.alignSelf));
      console.log("cd text2 borderStyle" + JSON.stringify(obj.$attrs.borderStyle));
      console.log("cd text2 visibility" + JSON.stringify(obj.$attrs.visibility));
      console.log("cd text2 fontWeight" + JSON.stringify(obj.$attrs.fontWeight));
      console.log("cd text2 textAlign" + JSON.stringify(obj.$attrs.textAlign));
      console.log("cd text2 textOverflow" + JSON.stringify(obj.$attrs.textOverflow));
      console.log("cd text2 direction" + JSON.stringify(obj.$attrs.direction));
      console.log("cd text2 textCase" + JSON.stringify(obj.$attrs.textCase));
      console.log("cd text2 decoration" + JSON.stringify(obj.$attrs.decoration));
      console.log("cd text2 decoration1" + JSON.stringify(JSON.stringify({
        type: "TextDecorationType.Noneh", color: "#FFFF0000"
      })));
      console.log("cd text2 fontColor" + JSON.stringify(obj.$attrs.fontColor));
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
     * @tc.number    SUB_ACE_BASIC_ETS_API_1230
     * @tc.name      textGetInspectorByKey040
     * @tc.desc      aceEtsTest
     */
    it('textGetInspectorByKey040', 0, async function (done) {
      let strJson = getInspectorByKey('Text3');
      let obj = JSON.parse(strJson);
      console.info("cd text3 component obj is: " + JSON.stringify(obj));
      console.log("cd text3 align" + JSON.stringify(obj.$attrs.align));
      console.log("cd text3 alignSelf" + JSON.stringify(obj.$attrs.alignSelf));
      console.log("cd text3 fontWeight" + JSON.stringify(obj.$attrs.fontWeight));
      console.log("cd text3 decoration" + JSON.stringify(obj.$attrs.decoration));
      console.log("cd text3 fontColor" + JSON.stringify(obj.$attrs.fontColor));
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
     * @tc.number    SUB_ACE_BASIC_ETS_API_1240
     * @tc.name      textGetInspectorByKey050
     * @tc.desc      aceEtsTest
     */
    it('textGetInspectorByKey050', 0, async function (done) {
      let strJson = getInspectorByKey('Text4');
      let obj = JSON.parse(strJson);
      console.info("cd text4 component obj is: " + JSON.stringify(obj));
      console.log("cd text4 align" + JSON.stringify(obj.$attrs.align));
      console.log("cd text4 alignSelf" + JSON.stringify(obj.$attrs.alignSelf));
      console.log("cd text4 fontWeight" + JSON.stringify(obj.$attrs.fontWeight));
      console.log("cd text4 fontColor" + JSON.stringify(obj.$attrs.fontColor));
      expect(obj.$attrs.align).assertEqual("Alignment.TopStart")
      expect(obj.$attrs.alignSelf).assertEqual("ItemAlign.Baseline")
      expect(obj.$attrs.fontWeight).assertEqual("FontWeight.Medium")
      expect(obj.$attrs.fontColor).assertEqual("#FFA52A2A")
      console.info('cd text4InspectorByKey END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_1250
     * @tc.name      textGetInspectorByKey060
     * @tc.desc      aceEtsTest
     */
    it('textGetInspectorByKey060', 0, async function (done) {
      let strJson = getInspectorByKey('Text5');
      let obj = JSON.parse(strJson);
      console.info("cd text5 component obj is: " + JSON.stringify(obj));
      console.log("cd text5 align" + JSON.stringify(obj.$attrs.align));
      console.log("cd text5 alignSelf" + JSON.stringify(obj.$attrs.alignSelf));
      console.log("cd text5 fontWeight" + JSON.stringify(obj.$attrs.fontWeight));
      console.log("cd text5 fontColor" + JSON.stringify(obj.$attrs.fontColor));
      expect(obj.$attrs.align).assertEqual("Alignment.End")
      expect(obj.$attrs.alignSelf).assertEqual("ItemAlign.Stretch")
      expect(obj.$attrs.fontWeight).assertEqual("FontWeight.Regular")
      expect(obj.$attrs.fontColor).assertEqual("#FF808080")
      console.info('cd text5InspectorByKey END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_1260
     * @tc.name      textGetInspectorByKey070
     * @tc.desc      aceEtsTest
     */
    it('textGetInspectorByKey070', 0, async function (done) {
      let strJson = getInspectorByKey('Text6');
      let obj = JSON.parse(strJson);
      console.info("cd text6 component obj is: " + JSON.stringify(obj));
      console.log("cd text6 align" + JSON.stringify(obj.$attrs.align));
      console.log("cd text6 fontColor" + JSON.stringify(obj.$attrs.fontColor));
      expect(obj.$attrs.align).assertEqual("Alignment.BottomEnd")
      expect(obj.$attrs.fontColor).assertEqual("#FFFFA500")
      console.info('cd text6InspectorByKey END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_1270
     * @tc.name      textGetInspectorByKey080
     * @tc.desc      aceEtsTest
     */
    it('textGetInspectorByKey080', 0, async function (done) {
      let strJson = getInspectorByKey('Text7');
      let obj = JSON.parse(strJson);
      console.info("cd text7 component obj is: " + JSON.stringify(obj));
      console.log("cd text7 align" + JSON.stringify(obj.$attrs.align));
      console.log("cd text7 fontColor" + JSON.stringify(obj.$attrs.fontColor));
      expect(obj.$attrs.align).assertEqual("Alignment.Bottom")
      expect(obj.$attrs.fontColor).assertEqual("#FFFFFF00")
      console.info('cd text7InspectorByKey END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_1280
     * @tc.name      textGetInspectorByKey090
     * @tc.desc      aceEtsTest
     */
    it('textGetInspectorByKey090', 0, async function (done) {
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
