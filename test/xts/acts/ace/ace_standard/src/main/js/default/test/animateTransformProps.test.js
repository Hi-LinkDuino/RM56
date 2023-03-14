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

    var name = 'animateTransform', nameA = 'AnimateTransform', labelName = 'animateTransform';

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
     * @tc.name      testAnimateTransformIdProp
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
     * @tc.name      testAnimateTransformAttributeNameProp
     * @tc.desc      ACE
     */
    it('test'+nameA+'AttributeNameProp', 0, async function (done) {
        console.info('test'+nameA+'AttributeNameProp START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.attributeNameProp);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('attributeNameProp')
        expect(obj.$attrs.attributename).assertEqual('x')
        console.info("["+name+"Props] get attributename value is: " + JSON.stringify(obj.$attrs.attributename));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testAnimateTransformAttributeNamePropNone
     * @tc.desc      ACE
     */
    it('test'+nameA+'AttributeNamePropNone', 0, async function (done) {
        console.info('test'+nameA+'AttributeNamePropNone START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.attributeNamePropNone);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('attributeNamePropNone')
        expect(obj.$attrs.attributename).assertEqual(undefined)
        console.info("["+name+"Props] get attributename value is: " + JSON.stringify(obj.$attrs.attributename));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testAnimateTransformBeginProp
     * @tc.desc      ACE
     */
    it('test'+nameA+'BeginProp', 0, async function (done) {
        console.info('test'+nameA+'BeginProp START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.beginProp);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('beginProp')
        expect(obj.$attrs.begin).assertEqual('1000')
        console.info("["+name+"Props] get begin value is: " + JSON.stringify(obj.$attrs.begin));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testAnimateTransformBeginPropNone
     * @tc.desc      ACE
     */
    it('test'+nameA+'BeginPropNone', 0, async function (done) {
        console.info('test'+nameA+'BeginPropNone START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.beginPropNone);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('beginPropNone')
        expect(obj.$attrs.begin).assertEqual(undefined)
        console.info("["+name+"Props] get begin value is: " + JSON.stringify(obj.$attrs.begin));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testAnimateTransformDurProp
     * @tc.desc      ACE
     */
    it('test'+nameA+'DurProp', 0, async function (done) {
        console.info('test'+nameA+'DurProp START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.durProp);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('durProp')
        expect(obj.$attrs.dur).assertEqual('1000')
        console.info("["+name+"Props] get dur value is: " + JSON.stringify(obj.$attrs.dur));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testAnimateTransformDurPropNone
     * @tc.desc      ACE
     */
    it('test'+nameA+'DurPropNone', 0, async function (done) {
        console.info('test'+nameA+'DurPropNone START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.durPropNone);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('durPropNone')
        expect(obj.$attrs.dur).assertEqual(undefined)
        console.info("["+name+"Props] get dur value is: " + JSON.stringify(obj.$attrs.dur));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testAnimateTransformEndProp
     * @tc.desc      ACE
     */
    it('test'+nameA+'EndProp', 0, async function (done) {
        console.info('test'+nameA+'EndProp START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.endProp);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('endProp')
        expect(obj.$attrs.end).assertEqual('1000')
        console.info("["+name+"Props] get end value is: " + JSON.stringify(obj.$attrs.end));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testAnimateTransformEndPropNone
     * @tc.desc      ACE
     */
    it('test'+nameA+'EndPropNone', 0, async function (done) {
        console.info('test'+nameA+'EndPropNone START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.endPropNone);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('endPropNone')
        expect(obj.$attrs.end).assertEqual(undefined)
        console.info("["+name+"Props] get end value is: " + JSON.stringify(obj.$attrs.end));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testAnimateTransformRepeatCountProp
     * @tc.desc      ACE
     */
    it('test'+nameA+'RepeatCountProp', 0, async function (done) {
        console.info('test'+nameA+'RepeatCountProp START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.repeatCountProp);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('repeatCountProp')
        expect(obj.$attrs.repeatcount).assertEqual('20')
        console.info("["+name+"Props] get repeatcount value is: " + JSON.stringify(obj.$attrs.repeatcount));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testAnimateTransformRepeatCountPropNone
     * @tc.desc      ACE
     */
    it('test'+nameA+'RepeatCountPropNone', 0, async function (done) {
        console.info('test'+nameA+'RepeatCountPropNone START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.repeatCountPropNone);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('repeatCountPropNone')
        expect(obj.$attrs.repeatcount).assertEqual(undefined)
        console.info("["+name+"Props] get repeatcount value is: " + JSON.stringify(obj.$attrs.repeatcount));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testAnimateTransformFillPropFreeze
     * @tc.desc      ACE
     */
    it('test'+nameA+'FillPropFreeze', 0, async function (done) {
        console.info('test'+nameA+'FillPropFreeze START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.fillPropFreeze);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('fillPropFreeze')
        expect(obj.$attrs.fill).assertEqual('freeze')
        console.info("["+name+"Props] get fill value is: " + JSON.stringify(obj.$attrs.fill));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testAnimateTransformFillPropRemove
     * @tc.desc      ACE
     */
    it('test'+nameA+'FillPropRemove', 0, async function (done) {
        console.info('test'+nameA+'FillPropRemove START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.fillPropRemove);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('fillPropRemove')
        expect(obj.$attrs.fill).assertEqual('remove')
        console.info("["+name+"Props] get fill value is: " + JSON.stringify(obj.$attrs.fill));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testAnimateTransformCalcModePropDiscrete
     * @tc.desc      ACE
     */
    it('test'+nameA+'CalcModePropDiscrete', 0, async function (done) {
        console.info('test'+nameA+'CalcModePropDiscrete START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.calcModePropDiscrete);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('calcModePropDiscrete')
        expect(obj.$attrs.calcmode).assertEqual('discrete')
        console.info("["+name+"Props] get calcmode value is: " + JSON.stringify(obj.$attrs.calcmode));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testAnimateTransformCalcModePropLinear
     * @tc.desc      ACE
     */
    it('test'+nameA+'CalcModePropLinear', 0, async function (done) {
        console.info('test'+nameA+'CalcModePropLinear START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.calcModePropLinear);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('calcModePropLinear')
        expect(obj.$attrs.calcmode).assertEqual('linear')
        console.info("["+name+"Props] get calcmode value is: " + JSON.stringify(obj.$attrs.calcmode));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testAnimateTransformCalcModePropPaced
     * @tc.desc      ACE
     */
    it('test'+nameA+'CalcModePropPaced', 0, async function (done) {
        console.info('test'+nameA+'CalcModePropPaced START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.calcModePropPaced);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('calcModePropPaced')
        expect(obj.$attrs.calcmode).assertEqual('Paced')
        console.info("["+name+"Props] get calcmode value is: " + JSON.stringify(obj.$attrs.calcmode));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testAnimateTransformCalcModePropSpline
     * @tc.desc      ACE
     */
    it('test'+nameA+'CalcModePropSpline', 0, async function (done) {
        console.info('test'+nameA+'CalcModePropSpline START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.calcModePropSpline);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('calcModePropSpline')
        expect(obj.$attrs.calcmode).assertEqual('spline')
        console.info("["+name+"Props] get calcmode value is: " + JSON.stringify(obj.$attrs.calcmode));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testAnimateTransformKeyTimesProp
     * @tc.desc      ACE
     */
    it('test'+nameA+'KeyTimesProp', 0, async function (done) {
        console.info('test'+nameA+'KeyTimesProp START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.keyTimesProp);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('keyTimesProp')
        expect(obj.$attrs.keytimes).assertEqual('0.3;0.6;0,9')
        console.info("["+name+"Props] get keytimes value is: " + JSON.stringify(obj.$attrs.keytimes));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testAnimateTransformKeyTimesPropNone
     * @tc.desc      ACE
     */
    it('test'+nameA+'KeyTimesPropNone', 0, async function (done) {
        console.info('test'+nameA+'KeyTimesPropNone START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.keyTimesPropNone);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('keyTimesPropNone')
        expect(obj.$attrs.keytimes).assertEqual(undefined)
        console.info("["+name+"Props] get keytimes value is: " + JSON.stringify(obj.$attrs.keytimes));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testAnimateTransformKeySplinesProp
     * @tc.desc      ACE
     */
    it('test'+nameA+'KeySplinesProp', 0, async function (done) {
        console.info('test'+nameA+'KeySplinesProp START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.keySplinesProp);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('keySplinesProp')
        expect(obj.$attrs.keysplines).assertEqual('0.5 0 0.5 1')
        console.info("["+name+"Props] get keysplines value is: " + JSON.stringify(obj.$attrs.keysplines));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testAnimateTransformKeySplinesPropNone
     * @tc.desc      ACE
     */
    it('test'+nameA+'KeySplinesPropNone', 0, async function (done) {
        console.info('test'+nameA+'KeySplinesPropNone START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.keySplinesPropNone);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('keySplinesPropNone')
        expect(obj.$attrs.keysplines).assertEqual(undefined)
        console.info("["+name+"Props] get keysplines value is: " + JSON.stringify(obj.$attrs.keysplines));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testAnimateTransformByProp
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
     * @tc.name      testAnimateTransformByPropNone
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
     * @tc.name      testAnimateTransformFromProp
     * @tc.desc      ACE
     */
    it('test'+nameA+'FromProp', 0, async function (done) {
        console.info('test'+nameA+'FromProp START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.fromProp);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('fromProp')
        expect(obj.$attrs.from).assertEqual('10')
        console.info("["+name+"Props] get from value is: " + JSON.stringify(obj.$attrs.from));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testAnimateTransformFromPropNone
     * @tc.desc      ACE
     */
    it('test'+nameA+'FromPropNone', 0, async function (done) {
        console.info('test'+nameA+'FromPropNone START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.fromPropNone);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('fromPropNone')
        expect(obj.$attrs.from).assertEqual(undefined)
        console.info("["+name+"Props] get from value is: " + JSON.stringify(obj.$attrs.from));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testAnimateTransformToProp
     * @tc.desc      ACE
     */
    it('test'+nameA+'ToProp', 0, async function (done) {
        console.info('test'+nameA+'ToProp START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.toProp);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('toProp')
        expect(obj.$attrs.to).assertEqual('10')
        console.info("["+name+"Props] get to value is: " + JSON.stringify(obj.$attrs.to));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testAnimateTransformToPropNone
     * @tc.desc      ACE
     */
    it('test'+nameA+'ToPropNone', 0, async function (done) {
        console.info('test'+nameA+'ToPropNone START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.toPropNone);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('toPropNone')
        expect(obj.$attrs.to).assertEqual(undefined)
        console.info("["+name+"Props] get to value is: " + JSON.stringify(obj.$attrs.to));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testAnimateTransformValuesProp
     * @tc.desc      ACE
     */
    it('test'+nameA+'ValuesProp', 0, async function (done) {
        console.info('test'+nameA+'ValuesProp START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.valuesProp);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('valuesProp')
        expect(obj.$attrs.values).assertEqual('10;30')
        console.info("["+name+"Props] get values value is: " + JSON.stringify(obj.$attrs.values));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testAnimateTransformValuesPropNone
     * @tc.desc      ACE
     */
    it('test'+nameA+'ValuesPropNone', 0, async function (done) {
        console.info('test'+nameA+'ValuesPropNone START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.valuesPropNone);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('valuesPropNone')
        expect(obj.$attrs.values).assertEqual(undefined)
        console.info("["+name+"Props] get values value is: " + JSON.stringify(obj.$attrs.values));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testAnimateTransformTypePropTranslate
     * @tc.desc      ACE
     */
    it('test'+nameA+'TypePropTranslate', 0, async function (done) {
        console.info('test'+nameA+'TypePropTranslate START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typePropTranslate);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('typePropTranslate')
        expect(obj.$attrs.type).assertEqual('translate')
        console.info("["+name+"Props] get values type is: " + JSON.stringify(obj.$attrs.type));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testAnimateTransformTypePropScale
     * @tc.desc      ACE
     */
    it('test'+nameA+'TypePropScale', 0, async function (done) {
        console.info('test'+nameA+'TypePropScale START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typePropScale);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('typePropScale')
        expect(obj.$attrs.type).assertEqual('scale')
        console.info("["+name+"Props] get values type is: " + JSON.stringify(obj.$attrs.type));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testAnimateTransformTypePropRotate
     * @tc.desc      ACE
     */
    it('test'+nameA+'TypePropRotate', 0, async function (done) {
        console.info('test'+nameA+'TypePropRotate START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typePropRotate);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('typePropRotate')
        expect(obj.$attrs.type).assertEqual('rotate')
        console.info("["+name+"Props] get values type is: " + JSON.stringify(obj.$attrs.type));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testAnimateTransformTypePropSkewX
     * @tc.desc      ACE
     */
    it('test'+nameA+'TypePropSkewX', 0, async function (done) {
        console.info('test'+nameA+'TypePropSkewX START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typePropSkewX);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('typePropSkewX')
        expect(obj.$attrs.type).assertEqual('skewX')
        console.info("["+name+"Props] get values type is: " + JSON.stringify(obj.$attrs.type));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testAnimateTransformTypePropSkewY
     * @tc.desc      ACE
     */
    it('test'+nameA+'TypePropSkewY', 0, async function (done) {
        console.info('test'+nameA+'TypePropSkewY START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typePropSkewY);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('typePropSkewY')
        expect(obj.$attrs.type).assertEqual('skewY')
        console.info("["+name+"Props] get values type is: " + JSON.stringify(obj.$attrs.type));
        done();
    });
});
