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

export default function imageJsunit() {

  function sleep(time) {
    return new Promise((resolve, reject) => {
      setTimeout(() => {
        resolve()
      }, time * 1000)
    }).then(() => {
      console.info(`sleep ${time} over...`)
    })
  }

  describe('imageTest', function () {
    beforeEach(async function (done) {
      let options = {
        uri: 'pages/Image',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get Image state success " + JSON.stringify(pages));
        if (!("Image" == pages.name)) {
          console.info("get Image state success " + JSON.stringify(pages.name));
          let result = await router.push(options)
          console.info("push Image page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push image page error " + JSON.stringify(result));
      }
      await sleep(1)
      done()
    });

    afterEach(async function () {
      await sleep(1)
      console.info("Image after each called");
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0880
     * @tc.name      testGetImageAttr010
     * @tc.desc      aceEtsTest
     */
    it('testGetImageAttr010', 0, async function (done) {
      console.info('testGetImageAttr1 START');
      let strJson = getInspectorByKey('image1');
      let obj = JSON.parse(strJson);
      console.info("component obj is: " + JSON.stringify(obj));
      console.log("image1 matchTextDirection" + JSON.stringify(obj.$attrs.matchTextDirection));
      console.log("image1 objectFit" + JSON.stringify(obj.$attrs.objectFit));
      console.log("image1 fitOriginalSize" + JSON.stringify(obj.$attrs.fitOriginalSize));
      console.log("image1 fillColor" + JSON.stringify(obj.$attrs.fillColor));
      console.log("image1 autoResize" + JSON.stringify(obj.$attrs.autoResize));
      console.log("image1 syncLoad" + JSON.stringify(obj.$attrs.syncLoad));
      console.log("image1 objectRepeat" + JSON.stringify(obj.$attrs.objectRepeat));
      console.log("image1 interpolation" + JSON.stringify(obj.$attrs.interpolation));
      console.log("image1 renderMode" + JSON.stringify(obj.$attrs.renderMode));
      console.log("image1 sourceSize" + JSON.stringify(obj.$attrs.sourceSize));
      console.log("image1 alt" + JSON.stringify(obj.$attrs.alt));
      expect(obj.$attrs.objectFit).assertEqual('ImageFit.Cover')
      expect(obj.$attrs.syncLoad).assertEqual('false')
      expect(obj.$attrs.objectRepeat).assertEqual('ImageRepeat.NoRepeat')
      expect(obj.$attrs.interpolation).assertEqual('ImageInterpolation.None')
      expect(obj.$attrs.renderMode).assertEqual('ImageRenderMode.Original')
      expect(obj.$attrs.sourceSize).assertEqual("[200.00 x 100.00]")
      expect(obj.$attrs.alt).assertEqual('/resources/rawfile/person.png')
      console.info('testGetImageAttr1 END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0890
     * @tc.name      testGetImageAttr020
     * @tc.desc      aceEtsTest
     */
    it('testGetImageAttr020', 0, async function (done) {
      console.info('testGetImageAttr2 START');
      let strJson = getInspectorByKey('image2');
      let obj = JSON.parse(strJson);
      console.info("component obj is: " + JSON.stringify(obj));
      console.log("image2 objectFit" + JSON.stringify(obj.$attrs.objectFit));
      console.log("image2 objectRepeat" + JSON.stringify(obj.$attrs.objectRepeat));
      console.log("image2 interpolation" + JSON.stringify(obj.$attrs.interpolation));
      console.log("image2 renderMode" + JSON.stringify(obj.$attrs.renderMode));
      expect(obj.$attrs.objectFit).assertEqual('ImageFit.Contain')
      expect(obj.$attrs.interpolation).assertEqual('ImageInterpolation.High')
      expect(obj.$attrs.renderMode).assertEqual('ImageRenderMode.Template')
      console.info('testGetImageAttr2 END');
      done();
    });
    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0900
     * @tc.name      testGetImageAttr030
     * @tc.desc      aceEtsTest
     */
    it('testGetImageAttr030', 0, async function (done) {
      console.info('testGetImageAttr3 START');
      let strJson = getInspectorByKey('image3');
      let obj = JSON.parse(strJson);
      console.info("component obj is: " + JSON.stringify(obj));
      console.log("image3 objectFit" + JSON.stringify(obj.$attrs.objectFit));
      console.log("image3 objectRepeat" + JSON.stringify(obj.$attrs.objectRepeat));
      console.log("image3 interpolation" + JSON.stringify(obj.$attrs.interpolation));
      expect(obj.$attrs.objectRepeat).assertEqual('ImageRepeat.Y')
      expect(obj.$attrs.interpolation).assertEqual('ImageInterpolation.Low')
      console.info('testGetImageAttr3 END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0910
     * @tc.name      testGetImageAttr040
     * @tc.desc      aceEtsTest
     */
    it('testGetImageAttr040', 0, async function (done) {
      console.info('testGetImageAttr4 START');
      let strJson = getInspectorByKey('image4');
      let obj = JSON.parse(strJson);
      console.info("component obj is: " + JSON.stringify(obj));
      console.log("image4 objectFit" + JSON.stringify(obj.$attrs.objectFit));
      console.log("image4 objectRepeat" + JSON.stringify(obj.$attrs.objectRepeat));
      console.log("image4 interpolation" + JSON.stringify(obj.$attrs.interpolation));
      expect(obj.$attrs.objectFit).assertEqual('ImageFit.None')
      expect(obj.$attrs.interpolation).assertEqual('ImageInterpolation.Medium')
      console.info('testGetImageAttr4 END');
      done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_ETS_API_0920
     * @tc.name      testGetImageAttr050
     * @tc.desc      aceEtsTest
     */
    it('testGetImageAttr050', 0, async function (done) {
      console.info('testGetImageAttr5 START');
      let strJson = getInspectorByKey('image5');
      let obj = JSON.parse(strJson);
      console.info("component obj is: " + JSON.stringify(obj));
      console.log("image5 objectFit" + JSON.stringify(obj.$attrs.objectFit));
      expect(obj.$attrs.objectFit).assertEqual('ImageFit.ScaleDown')
      console.info('testGetImageAttr5 END');
      done();
    });
  })
}
