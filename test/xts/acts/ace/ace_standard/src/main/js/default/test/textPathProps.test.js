/**
 * Copyright (c) 2022 Shenzhen Kaihong Digital Industry Development Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import router from '@system.router';
import {describe, beforeAll,afterAll, it, expect} from 'deccjsunit/index';


describe('toolbarPropsJsTest', function () {

    var name = 'textPath', nameA = 'TextPath', labelName = 'textPath';

    async function sleep(time) {
        return new Promise((resolve, reject) => {
            setTimeout(() => {
                resolve()
            }, time)
        }).then(() => {
            console.info(`sleep ${time} over...`)
        })
    }

    async function backToIndex() {
        let backToIndexPromise = new Promise((resolve, reject) => {
            setTimeout(() => {
                router.back({
                    uri: 'pages/index/index'
                });
                resolve();
            }, 500);
        });
        let clearPromise = new Promise((resolve, reject) => {
            setTimeout(() => {
                router.clear();
                resolve();
            }, 500);
        });
        await backToIndexPromise.then(() => {
            return clearPromise;
        });
    }

    /**
    * run before testcase
    */
    beforeAll(async function (done) {
        console.info('['+name+'PropsJsTest] before each called')

        let result;
        let options = {
            uri: 'pages/'+name+'/prop/index'
        }
        try {
            result = router.push(options)
            console.info("push "+name+"Props page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push "+name+"Props page error " + JSON.stringify(result));
        }
        await sleep(4000)
        done()
    });

    /**
    * run after testcase
    */
    afterAll(async function () {
        console.info('['+name+'PropsJsTest] after each called')
        await backToIndex()
        await sleep(1000)
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTspanIdProp
     * @tc.desc      ACE
     */
    it('test'+nameA+'IdProp', 0, async function (done) {
        console.info('test'+nameA+'IdProp START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.idProp);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('idProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextPathPathProp
     * @tc.desc      ACE
     */
    it('test'+nameA+'PathProp', 0, async function (done) {
        console.info('test'+nameA+'PathProp START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.pathProp);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('pathProp')
        expect(obj.$attrs.path).assertEqual('M0,20 Q50,20 100,20')
        console.info("["+name+"Props] get path value is: " + JSON.stringify(obj.$attrs.path));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextPathPathPropNone
     * @tc.desc      ACE
     */
    it('test'+nameA+'PathPropNone', 0, async function (done) {
        console.info('test'+nameA+'PathPropNone START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.pathPropNone);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('pathPropNone')
        expect(obj.$attrs.path).assertEqual(undefined)
        console.info("["+name+"Props] get path value is: " + JSON.stringify(obj.$attrs.path));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextPathStartOffsetPropLength
     * @tc.desc      ACE
     */
    it('test'+nameA+'StartOffsetPropLength', 0, async function (done) {
        console.info('test'+nameA+'StartOffsetPropLength START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.startOffsetPropLength);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('startOffsetPropLength')
        expect(obj.$attrs.startoffset).assertEqual('30px')
        console.info("["+name+"Props] get x startOffset is: " + JSON.stringify(obj.$attrs.startoffset));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextPathStartOffsetPropPercentage
     * @tc.desc      ACE
     */
    it('test'+nameA+'StartOffsetPropPercentage', 0, async function (done) {
        console.info('test'+nameA+'StartOffsetPropPercentage START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.startOffsetPropPercentage);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('startOffsetPropPercentage')
        expect(obj.$attrs.x).startoffset('60%')
        console.info("["+name+"Props] get startOffset value is: " + JSON.stringify(obj.$attrs.startoffset));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextPathStartOffsetPropNone
     * @tc.desc      ACE
     */
    it('test'+nameA+'StartOffsetPropNone', 0, async function (done) {
        console.info('test'+nameA+'StartOffsetPropNone START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.startOffsetPropNone);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('startOffsetPropNone')
        expect(obj.$attrs.startoffset).assertEqual(undefined)
        console.info("["+name+"Props] get startOffset value is: " + JSON.stringify(obj.$attrs.startoffset));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextPathByProp
     * @tc.desc      ACE
     */
    it('test'+nameA+'ByProp', 0, async function (done) {
        console.info('test'+nameA+'ByProp START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.byProp);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('byProp')
        expect(obj.$attrs.by).assertEqual('1')
        console.info("["+name+"Props] get by value is: " + JSON.stringify(obj.$attrs.by));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextByPropNone
     * @tc.desc      ACE
     */
    it('test'+nameA+'ByPropNone', 0, async function (done) {
        console.info('test'+nameA+'ByPropNone START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.byPropNone);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('byPropNone')
        expect(obj.$attrs.by).assertEqual(undefined)
        console.info("["+name+"Props] get by value is: " + JSON.stringify(obj.$attrs.by));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextPathFontSizeProp
     * @tc.desc      ACE
     */
    it('test'+nameA+'FontSizeProp', 0, async function (done) {
        console.info('test'+nameA+'FontSizeProp START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.fontSizeProp);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('fontSizeProp')
        expect(obj.$attrs.fontSize).assertEqual('20')
        console.info("["+name+"Props] get fontSize value is: " + JSON.stringify(obj.$attrs.fontSize));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextPathFontSizePropNone
     * @tc.desc      ACE
     */
    it('test'+nameA+'FontSizePropNone', 0, async function (done) {
        console.info('test'+nameA+'FontSizePropNone START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.fontSizePropNone);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('fontSizePropNone')
        expect(obj.$attrs.fontSize).assertEqual(undefined)
        console.info("["+name+"Props] get fontSize value is: " + JSON.stringify(obj.$attrs.fontSize));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextPathFillProp
     * @tc.desc      ACE
     */
    it('test'+nameA+'FillProp', 0, async function (done) {
        console.info('test'+nameA+'FillProp START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.fillProp);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('fillProp')
        expect(obj.$attrs.fill).assertEqual('red')
        console.info("["+name+"Props] get fill value is: " + JSON.stringify(obj.$attrs.fill));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextPathFillPropNone
     * @tc.desc      ACE
     */
    it('test'+nameA+'FillPropNone', 0, async function (done) {
        console.info('test'+nameA+'FillPropNone START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.fillPropNone);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('fillPropNone')
        expect(obj.$attrs.fill).assertEqual(undefined)
        console.info("["+name+"Props] get fill value is: " + JSON.stringify(obj.$attrs.fill));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextPathFillOpacityProp
     * @tc.desc      ACE
     */
    it('test'+nameA+'FillOpacityProp', 0, async function (done) {
        console.info('test'+nameA+'FillOpacityProp START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.fillOpacityProp);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('fillOpacityProp')
        expect(obj.$attrs.fillOpacity).assertEqual('0.5')
        console.info("["+name+"Props] get fillOpacity value is: " + JSON.stringify(obj.$attrs.fillOpacity));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextPathFillOpacityPropNone
     * @tc.desc      ACE
     */
    it('test'+nameA+'FillOpacityPropNone', 0, async function (done) {
        console.info('test'+nameA+'FillOpacityPropNone START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.fillOpacityPropNone);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('fillOpacityPropNone')
        expect(obj.$attrs.fillOpacity).assertEqual(undefined)
        console.info("["+name+"Props] get fillOpacity value is: " + JSON.stringify(obj.$attrs.fillOpacity));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextPathOpacityProp
     * @tc.desc      ACE
     */
    it('test'+nameA+'OpacityProp', 0, async function (done) {
        console.info('test'+nameA+'OpacityProp START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.opacityProp);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('opacityProp')
        expect(obj.$attrs.opacity).assertEqual('0.5')
        console.info("["+name+"Props] get opacity value is: " + JSON.stringify(obj.$attrs.opacity));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextPathOpacityPropNone
     * @tc.desc      ACE
     */
    it('test'+nameA+'OpacityPropNone', 0, async function (done) {
        console.info('test'+nameA+'OpacityPropNone START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.opacityPropNone);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('opacityPropNone')
        expect(obj.$attrs.opacity).assertEqual(undefined)
        console.info("["+name+"Props] get opacity value is: " + JSON.stringify(obj.$attrs.opacity));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextPathStrokeProp
     * @tc.desc      ACE
     */
    it('test'+nameA+'StrokeProp', 0, async function (done) {
        console.info('test'+nameA+'StrokeProp START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.strokeProp);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('strokeProp')
        expect(obj.$attrs.stroke).assertEqual('#ff00ff')
        console.info("["+name+"Props] get stroke value is: " + JSON.stringify(obj.$attrs.stroke));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextPathStrokePropNone
     * @tc.desc      ACE
     */
    it('test'+nameA+'StrokePropNone', 0, async function (done) {
        console.info('test'+nameA+'StrokePropNone START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.strokePropNone);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('strokePropNone')
        expect(obj.$attrs.stroke).assertEqual(undefined)
        console.info("["+name+"Props] get stroke value is: " + JSON.stringify(obj.$attrs.stroke));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextPathStrokeWidthProp
     * @tc.desc      ACE
     */
    it('test'+nameA+'StrokeWidthProp', 0, async function (done) {
        console.info('test'+nameA+'StrokeWidthProp START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.strokeWidthProp);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('strokeWidthProp')
        expect(obj.$attrs.strokeWidth).assertEqual('2')
        console.info("["+name+"Props] get strokeWidth value is: " + JSON.stringify(obj.$attrs.strokeWidth));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextPathStrokeWidthPropNone
     * @tc.desc      ACE
     */
    it('test'+nameA+'StrokeWidthPropNone', 0, async function (done) {
        console.info('test'+nameA+'StrokeWidthPropNone START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.strokeWidthPropNone);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('strokeWidthPropNone')
        expect(obj.$attrs.strokeWidth).assertEqual(undefined)
        console.info("["+name+"Props] get strokeWidth value is: " + JSON.stringify(obj.$attrs.strokeWidth));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextPathStrokeOpacityProp
     * @tc.desc      ACE
     */
    it('test'+nameA+'StrokeOpacityProp', 0, async function (done) {
        console.info('test'+nameA+'StrokeOpacityProp START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.strokeOpacityProp);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('strokeOpacityProp')
        expect(obj.$attrs.strokeOpacity).assertEqual('0.4')
        console.info("["+name+"Props] get strokeOpacity value is: " + JSON.stringify(obj.$attrs.strokeOpacity));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextPathStrokeOpacityPropNone
     * @tc.desc      ACE
     */
    it('test'+nameA+'StrokeOpacityPropNone', 0, async function (done) {
        console.info('test'+nameA+'StrokeOpacityPropNone START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.strokeOpacityPropNone);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('strokeOpacityPropNone')
        expect(obj.$attrs.strokeOpacity).assertEqual(undefined)
        console.info("["+name+"Props] get strokeOpacity value is: " + JSON.stringify(obj.$attrs.strokeOpacity));
        done();
    });
});
