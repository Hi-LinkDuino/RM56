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


describe('panelPropsJsTest', function () {

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
        console.info('[panelPropsJsTest] before each called')

        let result;
        let options = {
            uri: 'pages/panel/prop/index'
        }
        try {
            result = router.push(options)
            console.info("push panelProps page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push panelProps page error " + JSON.stringify(result));
        }
        await sleep(4000)
        done()
    });

    /**
    * run after testcase
    */
    afterAll(async function () {
        console.info('[panelPropsJsTest] after each called')
        await backToIndex()
        await sleep(1000)
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPanelIdProp
     * @tc.desc      ACE
     */
    it('testPanelIdProp', 0, async function (done) {
        console.info('testPanelIdProp START');
        console.info("[panelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.idProp);
        console.info("[panelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[panelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('panel')
        expect(obj.$attrs.id).assertEqual('idProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPanelClassProp
     * @tc.desc      ACE
     */
    it('testPanelClassProp', 0, async function (done) {
        console.info('testPanelClassProp START');
        console.info("[panelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classProp);
        console.info("[panelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[panelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('panel')
        expect(obj.$attrs.id).assertEqual('classProp')
        expect(obj.$attrs.className).assertEqual('classProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPanelClassPropNone
     * @tc.desc      ACE
     */
    it('testPanelClassPropNone', 0, async function (done) {
        console.info('testPanelClassPropNone START');
        console.info("[panelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classPropNone);
        console.info("[panelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[panelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('panel')
        expect(obj.$attrs.id).assertEqual('classPropNone')
        expect(obj.$attrs.className).assertEqual(undefined)
        console.info("[panelProps] get className value is: " + JSON.stringify(obj.$attrs.className));
        done();
    });


    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPanelPropsRefProp
     * @tc.desc      ACE
     */
    it('testPanelPropsRefProp', 0, async function (done) {
        console.info('testPanelPropsRefProp START');
        console.info("[panelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refProp);
        console.info("[panelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[panelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('panel')
        expect(obj.$attrs.id).assertEqual('refProp')
        expect(obj.$attrs.ref).assertEqual('refProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPanelRefPropNone
     * @tc.desc      ACE
     */
    it('testPanelRefPropNone', 0, async function (done) {
        console.info('testPanelRefPropNone START');
        console.info("[panelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refPropNone);
        console.info("[panelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[panelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('panel')
        expect(obj.$attrs.id).assertEqual('refPropNone')
        expect(obj.$attrs.ref).assertEqual(undefined)
        console.info("[panelProps] get ref value is: " + JSON.stringify(obj.$attrs.ref));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPanelDataProp
     * @tc.desc      ACE
     */
    it('testPanelDataProp', 0, async function (done) {
        console.info('testPanelDataProp START');
        console.info("[panelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataProp);
        console.info("[panelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[panelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('panel')
        expect(obj.$attrs.id).assertEqual('dataProp')
        expect(obj.$attrs.dataPanel).assertEqual(undefined);
        console.info("[panelProps] get dataPanel value is: " + JSON.stringify(obj.$attrs.dataPanel));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPanelDataPropNone
     * @tc.desc      ACE
     */
    it('testPanelDataPropNone', 0, async function (done) {
        console.info('testPanelDataPropNone START');
        console.info("[panelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataPropNone);
        console.info("[panelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[panelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('panel')
        expect(obj.$attrs.id).assertEqual('dataPropNone')
        expect(obj.$attrs.dataPanel).assertEqual(undefined)
        console.info("[panelProps] get dataPanel value is: " + JSON.stringify(obj.$attrs.dataPanel));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPanelClickEffectPropSmall
     * @tc.desc      ACE
     */
    it('testPanelClickEffectPropSmall', 0, async function (done) {
        console.info('testPanelClickEffectPropSmall START');
        console.info("[panelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropSmall);
        console.info("[panelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[panelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('panel')
        expect(obj.$attrs.id).assertEqual('clickEffectPropSmall')
        expect(obj.$attrs.clickEffect).assertEqual('spring-small')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPanelClickEffectPropMedium
     * @tc.desc      ACE
     */
    it('testPanelClickEffectPropMedium', 0, async function (done) {
        console.info('testPanelClickEffectPropMedium START');
        console.info("[panelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropMedium);
        console.info("[panelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[panelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('panel')
        expect(obj.$attrs.id).assertEqual('clickEffectPropMedium')
        expect(obj.$attrs.clickEffect).assertEqual('spring-medium')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPanelClickEffectPropLarge
     * @tc.desc      ACE
     */
    it('testPanelClickEffectPropLarge', 0, async function (done) {
        console.info('testPanelClickEffectPropLarge START');
        console.info("[panelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropLarge);
        console.info("[panelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[panelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('panel')
        expect(obj.$attrs.id).assertEqual('clickEffectPropLarge')
        expect(obj.$attrs.clickEffect).assertEqual('spring-large')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPanelClickEffectPropNone
     * @tc.desc      ACE
     */
    it('testPanelClickEffectPropNone', 0, async function (done) {
        console.info('testPanelClickEffectPropNone START');
        console.info("[panelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropNone);
        console.info("[panelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[panelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('panel')
        expect(obj.$attrs.id).assertEqual('clickEffectPropNone')
        expect(obj.$attrs.clickEffect).assertEqual(undefined)
        console.info("[panelProps] get clickEffect value is: " + JSON.stringify(obj.$attrs.clickEffect));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPanelDirPropRtl
     * @tc.desc      ACE
     */
    it('testPanelDirPropRtl', 0, async function (done) {
        console.info('testPanelDirPropRtl START');
        console.info("[panelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropRtl);
        console.info("[panelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[panelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('panel')
        expect(obj.$attrs.id).assertEqual('dirPropRtl')
        expect(obj.$attrs.dir).assertEqual('rtl')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPanelDirPropLtr
     * @tc.desc      ACE
     */
    it('testPanelDirPropLtr', 0, async function (done) {
        console.info('testPanelDirPropLtr START');
        console.info("[panelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropLtr);
        console.info("[panelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[panelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('panel')
        expect(obj.$attrs.id).assertEqual('dirPropLtr')
        expect(obj.$attrs.dir).assertEqual('ltr')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPanelDirPropAuto
     * @tc.desc      ACE
     */
    it('testPanelDirPropAuto', 0, async function (done) {
        console.info('testPanelDirPropAuto START');
        console.info("[panelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropAuto);
        console.info("[panelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[panelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('panel')
        expect(obj.$attrs.id).assertEqual('dirPropAuto')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPanelDirPropNone
     * @tc.desc      ACE
     */
    it('testPanelDirPropNone', 0, async function (done) {
        console.info('testPanelDirPropNone START');
        console.info("[panelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropNone);
        console.info("[panelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[panelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('panel')
        expect(obj.$attrs.id).assertEqual('dirPropNone')
        expect(obj.$attrs.dir).assertEqual(undefined)
        done();
    });


    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTypePropFoldAble
     * @tc.desc      ACE
     */
    it('testTypePropFoldAble', 0, async function (done) {
        console.info('testTypePropFoldAble START');
        console.info("[panelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typePropFoldAble); //ID
        console.info("[panelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[panelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('panel')
        expect(obj.$attrs.id).assertEqual('typePropFoldAble') //ID
        expect(obj.$attrs.type).assertEqual('foldable') //prop and value
        console.info("[panelProps] get type value is: " + JSON.stringify(obj.$attrs.type)); //prop
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTypePropMinibar
     * @tc.desc      ACE
     */
    it('testTypePropMinibar', 0, async function (done) {
        console.info('testTypePropMinibar START');
        console.info("[panelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typePropMinibar); //ID
        console.info("[panelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[panelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('panel')
        expect(obj.$attrs.id).assertEqual('typePropMinibar') //ID
        expect(obj.$attrs.type).assertEqual('minibar') //prop and value
        console.info("[panelProps] get type value is: " + JSON.stringify(obj.$attrs.type)); //prop
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTypePropTemporary
     * @tc.desc      ACE
     */
    it('testTypePropTemporary', 0, async function (done) {
        console.info('testTypePropTemporary START');
        console.info("[panelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typePropTemporary); //ID
        console.info("[panelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[panelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('panel')
        expect(obj.$attrs.id).assertEqual('typePropTemporary') //ID
        expect(obj.$attrs.type).assertEqual('temporary') //prop and value
        console.info("[panelProps] get type value is: " + JSON.stringify(obj.$attrs.type)); //prop
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testModePropMini
     * @tc.desc      ACE
     */
    it('testModePropMini', 0, async function (done) {
        console.info('testModePropMini START');
        console.info("[panelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.modePropMini); //ID
        console.info("[panelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[panelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('panel')
        expect(obj.$attrs.id).assertEqual('modePropMini') //ID
        expect(obj.$attrs.mode).assertEqual('mini') //prop and value
        console.info("[panelProps] get mode value is: " + JSON.stringify(obj.$attrs.mode)); //prop
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testModePropHalf
     * @tc.desc      ACE
     */
    it('testModePropHalf', 0, async function (done) {
        console.info('testModePropHalf START');
        console.info("[panelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.modePropHalf); //ID
        console.info("[panelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[panelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('panel')
        expect(obj.$attrs.id).assertEqual('modePropHalf') //ID
        expect(obj.$attrs.mode).assertEqual('half') //prop and value
        console.info("[panelProps] get mode value is: " + JSON.stringify(obj.$attrs.mode)); //prop
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testModePropFull
     * @tc.desc      ACE
     */
    it('testModePropFull', 0, async function (done) {
        console.info('testModePropFull START');
        console.info("[panelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.modePropFull); //ID
        console.info("[panelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[panelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('panel')
        expect(obj.$attrs.id).assertEqual('modePropFull') //ID
        expect(obj.$attrs.mode).assertEqual('full') //prop and value
        console.info("[panelProps] get mode value is: " + JSON.stringify(obj.$attrs.mode)); //prop
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDragBarPropTrue
     * @tc.desc      ACE
     */
    it('testDragBarPropTrue', 0, async function (done) {
        console.info('testDragBarPropTrue START');
        console.info("[panelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dragBarPropTrue); //ID
        console.info("[panelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[panelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('panel')
        expect(obj.$attrs.id).assertEqual('dragBarPropTrue') //ID
        expect(obj.$attrs.dragbar).assertEqual('true') //prop and value
        console.info("[panelProps] get dragbar value is: " + JSON.stringify(obj.$attrs.dragbar)); //prop
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDragBarPropFalse
     * @tc.desc      ACE
     */
    it('testDragBarPropFalse', 0, async function (done) {
        console.info('testDragBarPropFalse START');
        console.info("[panelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dragBarPropFalse); //ID
        console.info("[panelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[panelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('panel')
        expect(obj.$attrs.id).assertEqual('dragBarPropFalse') //ID
        expect(obj.$attrs.dragbar).assertEqual('false') //prop and value
        console.info("[panelProps] get dragbar value is: " + JSON.stringify(obj.$attrs.dragbar)); //prop
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testFullHeightPro
     * @tc.desc      ACE
     */
    it('testFullHeightPro', 0, async function (done) {
        console.info('testFullHeightPro START');
        console.info("[panelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.fullHeightPro); //ID
        console.info("[panelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[panelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('panel')
        expect(obj.$attrs.id).assertEqual('fullHeightPro') //ID
        expect(obj.$attrs.fullheight).assertEqual('500px') //prop and value
        console.info("[panelProps] get fullheight value is: " + JSON.stringify(obj.$attrs.fullheight)); //prop
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testHalfHeightPro
     * @tc.desc      ACE
     */
    it('testHalfHeightPro', 0, async function (done) {
        console.info('testHalfHeightPro START');
        console.info("[panelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.halfHeightPro); //ID
        console.info("[panelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[panelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('panel')
        expect(obj.$attrs.id).assertEqual('halfHeightPro') //ID
        expect(obj.$attrs.halfheight).assertEqual('400px') //prop and value
        console.info("[panelProps] get halfheight value is: " + JSON.stringify(obj.$attrs.halfheight)); //prop
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testMiniHeightPro
     * @tc.desc      ACE
     */
    it('testMiniHeightPro', 0, async function (done) {
        console.info('testMiniHeightPro START');
        console.info("[panelProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.miniHeightPro); //ID
        console.info("[panelProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[panelProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('panel')
        expect(obj.$attrs.id).assertEqual('miniHeightPro') //ID
        expect(obj.$attrs.miniheight).assertEqual('50px') //prop and value
        console.info("[panelProps] get miniheight value is: " + JSON.stringify(obj.$attrs.miniheight)); //prop
        done();
    });
});
