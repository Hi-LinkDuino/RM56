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


describe('popupPropsJsTest', function () {

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
        console.info('[popupPropsJsTest] before each called')

        let result;
        let options = {
            uri: 'pages/popup/prop/index'
        }
        try {
            result = router.push(options)
            console.info("push popupProps page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push popupProps page error " + JSON.stringify(result));
        }
        await sleep(4000)
        done()
    });

    /**
    * run after testcase
    */
    afterAll(async function () {
        console.info('[popupPropsJsTest] after each called')
        await backToIndex()
        await sleep(1000)
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPopupIdProp
     * @tc.desc      ACE
     */
    it('testPopupIdProp', 0, async function (done) {
        console.info('testPopupIdProp START');
        console.info("[popupProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.idProp);
        console.info("[popupProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[popupProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('popup')
        expect(obj.$attrs.id).assertEqual('idProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPopupClassProp
     * @tc.desc      ACE
     */
    it('testPopupClassProp', 0, async function (done) {
        console.info('testPopupClassProp START');
        console.info("[popupProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classProp);
        console.info("[popupProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[popupProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('popup')
        expect(obj.$attrs.id).assertEqual('classProp')
        expect(obj.$attrs.className).assertEqual('classProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPopupClassPropNone
     * @tc.desc      ACE
     */
    it('testPopupClassPropNone', 0, async function (done) {
        console.info('testPopupClassPropNone START');
        console.info("[popupProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classPropNone);
        console.info("[popupProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[popupProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('popup')
        expect(obj.$attrs.id).assertEqual('classPropNone')
        expect(obj.$attrs.className).assertEqual(undefined)
        console.info("[popupProps] get className value is: " + JSON.stringify(obj.$attrs.className));
        done();
    });


    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPopupRefProp
     * @tc.desc      ACE
     */
    it('testPopupRefProp', 0, async function (done) {
        console.info('testPopupRefProp START');
        console.info("[popupProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refProp);
        console.info("[popupProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[popupProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('popup')
        expect(obj.$attrs.id).assertEqual('refProp')
        expect(obj.$attrs.ref).assertEqual('refProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPopupRefPropNone
     * @tc.desc      ACE
     */
    it('testPopupRefPropNone', 0, async function (done) {
        console.info('testPopupRefPropNone START');
        console.info("[popupProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refPropNone);
        console.info("[popupProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[popupProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('popup')
        expect(obj.$attrs.id).assertEqual('refPropNone')
        expect(obj.$attrs.ref).assertEqual(undefined)
        console.info("[popupProps] get ref value is: " + JSON.stringify(obj.$attrs.ref));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPopupDisabledPropTrue
     * @tc.desc      ACE
     */
    it('testPopupDisabledPropTrue', 0, async function (done) {
        console.info('testPopupDisabledPropTrue START');
        console.info("[popupProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropTrue);
        console.info("[popupProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[popupProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('popup')
        expect(obj.$attrs.id).assertEqual('disabledPropTrue')
        expect(obj.$attrs.disabled).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPopupDisabledPropFalse
     * @tc.desc      ACE
     */
    it('testPopupDisabledPropFalse', 0, async function (done) {
        console.info('testPopupDisabledPropFalse START');
        console.info("[popupProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropFalse);
        console.info("[popupProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[popupProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('popup')
        expect(obj.$attrs.id).assertEqual('disabledPropFalse')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPopupDisabledPropNone
     * @tc.desc      ACE
     */
    it('testPopupDisabledPropNone', 0, async function (done) {
        console.info('testPopupDisabledPropNone START');
        console.info("[popupProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropNone);
        console.info("[popupProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[popupProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('popup')
        expect(obj.$attrs.id).assertEqual('disabledPropNone')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPopupDataProp
     * @tc.desc      ACE
     */
    it('testPopupDataProp', 0, async function (done) {
        console.info('testPopupDataProp START');
        console.info("[popupProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataProp);
        console.info("[popupProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[popupProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('popup')
        expect(obj.$attrs.id).assertEqual('dataProp')
        expect(obj.$attrs.dataPopup).assertEqual(undefined);
        console.info("[popupProps] get dataPopup value is: " + JSON.stringify(obj.$attrs.dataPopup));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPopupDataPropNone
     * @tc.desc      ACE
     */
    it('testPopupDataPropNone', 0, async function (done) {
        console.info('testPopupDataPropNone START');
        console.info("[popupProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataPropNone);
        console.info("[popupProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[popupProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('popup')
        expect(obj.$attrs.id).assertEqual('dataPropNone')
        expect(obj.$attrs.dataPopup).assertEqual(undefined)
        console.info("[popupProps] get dataPopup value is: " + JSON.stringify(obj.$attrs.dataPopup));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPopupClickEffectPropSmall
     * @tc.desc      ACE
     */
    it('testPopupClickEffectPropSmall', 0, async function (done) {
        console.info('testPopupClickEffectPropSmall START');
        console.info("[popupProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropSmall);
        console.info("[popupProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[popupProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('popup')
        expect(obj.$attrs.id).assertEqual('clickEffectPropSmall')
        expect(obj.$attrs.clickEffect).assertEqual('spring-small')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPopupClickEffectPropMedium
     * @tc.desc      ACE
     */
    it('testPopupClickEffectPropMedium', 0, async function (done) {
        console.info('testPopupClickEffectPropMedium START');
        console.info("[popupProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropMedium);
        console.info("[popupProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[popupProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('popup')
        expect(obj.$attrs.id).assertEqual('clickEffectPropMedium')
        expect(obj.$attrs.clickEffect).assertEqual('spring-medium')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPopupClickEffectPropLarge
     * @tc.desc      ACE
     */
    it('testPopupClickEffectPropLarge', 0, async function (done) {
        console.info('testPopupClickEffectPropLarge START');
        console.info("[popupProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropLarge);
        console.info("[popupProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[popupProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('popup')
        expect(obj.$attrs.id).assertEqual('clickEffectPropLarge')
        expect(obj.$attrs.clickEffect).assertEqual('spring-large')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPopupClickEffectPropNone
     * @tc.desc      ACE
     */
    it('testPopupClickEffectPropNone', 0, async function (done) {
        console.info('testPopupClickEffectPropNone START');
        console.info("[popupProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropNone);
        console.info("[popupProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[popupProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('popup')
        expect(obj.$attrs.id).assertEqual('clickEffectPropNone')
        expect(obj.$attrs.clickEffect).assertEqual(undefined)
        console.info("[popupProps] get clickEffect value is: " + JSON.stringify(obj.$attrs.clickEffect));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPopupDirPropRtl
     * @tc.desc      ACE
     */
    it('testPopupDirPropRtl', 0, async function (done) {
        console.info('testPopupDirPropRtl START');
        console.info("[popupProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropRtl);
        console.info("[popupProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[popupProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('popup')
        expect(obj.$attrs.id).assertEqual('dirPropRtl')
        expect(obj.$attrs.dir).assertEqual('rtl')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPopupDirPropLtr
     * @tc.desc      ACE
     */
    it('testPopupDirPropLtr', 0, async function (done) {
        console.info('testPopupDirPropLtr START');
        console.info("[popupProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropLtr);
        console.info("[popupProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[popupProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('popup')
        expect(obj.$attrs.id).assertEqual('dirPropLtr')
        expect(obj.$attrs.dir).assertEqual('ltr')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPopupDirPropAuto
     * @tc.desc      ACE
     */
    it('testPopupDirPropAuto', 0, async function (done) {
        console.info('testPopupDirPropAuto START');
        console.info("[popupProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropAuto);
        console.info("[popupProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[popupProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('popup')
        expect(obj.$attrs.id).assertEqual('dirPropAuto')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPopupDirPropNone
     * @tc.desc      ACE
     */
    it('testPopupDirPropNone', 0, async function (done) {
        console.info('testPopupDirPropNone START');
        console.info("[popupProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropNone);
        console.info("[popupProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[popupProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('popup')
        expect(obj.$attrs.id).assertEqual('dirPropNone')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPopupForPropNull
     * @tc.desc      ACE
     */
    it('testPopupForPropNull', 0, async function (done) {
        console.info('testPopupForPropNull START');
        console.info("[popupProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropNull);
        console.info("[popupProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[popupProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('popup')
        expect(obj.$attrs.id).assertEqual('forPropNull')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[popupProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPopupForPropOne
     * @tc.desc      ACE
     */
    it('testPopupForPropOne', 0, async function (done) {
        console.info('testPopupForPropOne START');
        console.info("[popupProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropOne);
        console.info("[popupProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[popupProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('popup')
        expect(obj.$attrs.id).assertEqual('forPropOne')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[popupProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPopupForPropThree
     * @tc.desc      ACE
     */
    it('testPopupForPropThree', 0, async function (done) {
        console.info('testPopupForPropThree START');
        console.info("[popupProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropThree);
        console.info("[popupProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[popupProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('popup')
        expect(obj.$attrs.id).assertEqual('forPropThree')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[popupProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPopupIfPropTrue
     * @tc.desc      ACE
     */
    it('testPopupIfPropTrue', 0, async function (done) {
        console.info('testPopupIfPropTrue START');
        console.info("[popupProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.ifPropTrue);
        console.info("[popupProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[popupProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('popup')
        expect(obj.$attrs.id).assertEqual('ifPropTrue')
        expect(obj.$attrs.if).assertEqual(undefined)
        console.info("[popupProps] get for value is: " + JSON.stringify(obj.$attrs.if));
        done();
    });


    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPopupShowPropTrue
     * @tc.desc      ACE
     */
    it('testPopupShowPropTrue', 0, async function (done) {
        console.info('testPopupShowPropTrue START');
        console.info("[popupProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropTrue);
        console.info("[popupProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[popupProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('popup')
        expect(obj.$attrs.id).assertEqual('showPropTrue')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[popupProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPopupShowPropFalse
     * @tc.desc      ACE
     */
    it('testPopupShowPropFalse', 0, async function (done) {
        console.info('testPopupShowPropFalse START');
        console.info("[popupProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropFalse);
        console.info("[popupProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[popupProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('popup')
        expect(obj.$attrs.id).assertEqual('showPropFalse')
        expect(obj.$attrs.show).assertEqual('false')
        console.info("[popupProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPopupShowPropNone
     * @tc.desc      ACE
     */
    it('testPopupShowPropNone', 0, async function (done) {
        console.info('testPopupShowPropNone START');
        console.info("[popupProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropNone);
        console.info("[popupProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[popupProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('popup')
        expect(obj.$attrs.id).assertEqual('showPropNone')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[popupProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPopupTargetProp
     * @tc.desc      ACE
     */
    it('testPopupTargetProp', 0, async function (done) {
        console.info('testPopupTargetProp START');
        console.info("[popupProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.targetProp); //ID
        console.info("[popupProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[popupProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('popup')
        expect(obj.$attrs.id).assertEqual('targetProp') //ID
        expect(obj.$attrs.target).assertEqual('target1') //PROP AND VALUE
        console.info("[popupProps] get target value is: " + JSON.stringify(obj.$attrs.target)); //PROP
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPlacementPropLeft
     * @tc.desc      ACE
     */
    it('testPlacementPropLeft', 0, async function (done) {
        console.info('testPlacementPropLeft START');
        console.info("[popupProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.placementPropLeft); //ID
        console.info("[popupProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[popupProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('popup')
        expect(obj.$attrs.id).assertEqual('placementPropLeft') //ID
        expect(obj.$attrs.placement).assertEqual('left') //PROP AND VALUE
        console.info("[popupProps] get placement value is: " + JSON.stringify(obj.$attrs.placement)); //PROP
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPlacementPropRight
     * @tc.desc      ACE
     */
    it('testPlacementPropRight', 0, async function (done) {
        console.info('testPlacementPropRight START');
        console.info("[popupProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.placementPropRight); //ID
        console.info("[popupProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[popupProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('popup')
        expect(obj.$attrs.id).assertEqual('placementPropRight') //ID
        expect(obj.$attrs.placement).assertEqual('right') //PROP AND VALUE
        console.info("[popupProps] get placement value is: " + JSON.stringify(obj.$attrs.placement)); //PROP
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPlacementPropTop
     * @tc.desc      ACE
     */
    it('testPlacementPropTop', 0, async function (done) {
        console.info('testPlacementPropTop START');
        console.info("[popupProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.placementPropTop); //ID
        console.info("[popupProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[popupProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('popup')
        expect(obj.$attrs.id).assertEqual('placementPropTop') //ID
        expect(obj.$attrs.placement).assertEqual('top') //PROP AND VALUE
        console.info("[popupProps] get placement value is: " + JSON.stringify(obj.$attrs.placement)); //PROP
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testKeepAlivePropTrue
     * @tc.desc      ACE
     */
    it('testKeepAlivePropTrue', 0, async function (done) {
        console.info('testKeepAlivePropTrue START');
        console.info("[popupProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.keepAlivePropTrue); //ID
        console.info("[popupProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[popupProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('popup')
        expect(obj.$attrs.id).assertEqual('keepAlivePropTrue') //ID
        expect(obj.$attrs.keepalive).assertEqual('true') //PROP AND VALUE
        console.info("[popupProps] get keepalive value is: " + JSON.stringify(obj.$attrs.keepalive)); //PROP
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testKeepAlivePropFalse
     * @tc.desc      ACE
     */
    it('testKeepAlivePropFalse', 0, async function (done) {
        console.info('testKeepAlivePropFalse START');
        console.info("[popupProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.keepAlivePropFalse); //ID
        console.info("[popupProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[popupProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('popup')
        expect(obj.$attrs.id).assertEqual('keepAlivePropFalse') //ID
        expect(obj.$attrs.keepalive).assertEqual('false') //PROP AND VALUE
        console.info("[popupProps] get keepalive value is: " + JSON.stringify(obj.$attrs.keepalive)); //PROP
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testClickablePropTrue
     * @tc.desc      ACE
     */
    it('testClickablePropTrue', 0, async function (done) {
        console.info('testClickablePropTrue START');
        console.info("[popupProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickablePropTrue); //ID
        console.info("[popupProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[popupProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('popup')
        expect(obj.$attrs.id).assertEqual('clickablePropTrue') //ID
        expect(obj.$attrs.clickable).assertEqual('true') //PROP AND VALUE
        console.info("[popupProps] get clickable value is: " + JSON.stringify(obj.$attrs.clickable)); //PROP
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testClickablePropFalse
     * @tc.desc      ACE
     */
    it('testClickablePropFalse', 0, async function (done) {
        console.info('testClickablePropFalse START');
        console.info("[popupProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickablePropFalse); //ID
        console.info("[popupProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[popupProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('popup')
        expect(obj.$attrs.id).assertEqual('clickablePropFalse') //ID
        expect(obj.$attrs.clickable).assertEqual('false') //PROP AND VALUE
        console.info("[popupProps] get clickable value is: " + JSON.stringify(obj.$attrs.clickable)); //PROP
        done();
    });


    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testArrowOffsetProp
     * @tc.desc      ACE
     */
    it('testArrowOffsetProp', 0, async function (done) {
        console.info('testArrowOffsetProp START');
        console.info("[popupProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.arrowOffsetProp); //ID
        console.info("[popupProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[popupProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('popup')
        expect(obj.$attrs.id).assertEqual('arrowOffsetProp') //ID
        expect(obj.$attrs.arrowoffset).assertEqual('10px') //PROP AND VALUE
        console.info("[popupProps] get arrowoffset value is: " + JSON.stringify(obj.$attrs.arrowoffset)); //PROP
        done();
    });
});
