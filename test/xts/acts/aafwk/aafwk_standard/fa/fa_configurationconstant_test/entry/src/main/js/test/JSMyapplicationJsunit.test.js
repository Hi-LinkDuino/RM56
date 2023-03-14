// @ts-nocheck
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
import {describe, expect, it} from 'deccjsunit/index'
import ConfigurationConstant from '@ohos.application.ConfigurationConstant'

describe("ConfigurationTest", function () {

    /*
     * @tc.number  SUB_AA_OpenHarmony_ConfigurationConstant_colorMode_0100
     * @tc.name    colorMode is "ConfigurationConstant.ColorMode.COLOR_MODE_LIGHT"
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_ConfigurationConstant_colorMode_0100', 0, async function (done) {
        console.log("------------start SUB_AA_OpenHarmony_ConfigurationConstant_colorMode_0100-------------");
        expect(ConfigurationConstant.ColorMode.COLOR_MODE_LIGHT).assertEqual(1);
        done();
        console.log("------------end SUB_AA_OpenHarmony_ConfigurationConstant_colorMode_0100-------------");
    });

    /*
     * @tc.number  SUB_AA_OpenHarmony_ConfigurationConstant_colorMode_0200
     * @tc.name    colorMode is "ConfigurationConstant.ColorMode.COLOR_MODE_DARK"
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_ConfigurationConstant_colorMode_0200', 0, async function (done) {
        console.log("------------start SUB_AA_OpenHarmony_ConfigurationConstant_colorMode_0200-------------");
        expect(ConfigurationConstant.ColorMode.COLOR_MODE_DARK).assertEqual(0);
        done();
        console.log("------------end SUB_AA_OpenHarmony_ConfigurationConstant_colorMode_0200-------------");
    });

    /*
     * @tc.number  SUB_AA_OpenHarmony_ConfigurationConstant_colorMode_0300
     * @tc.name    colorMode is "ConfigurationConstant.ColorMode.COLOR_MODE_NOT_SET"
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_ConfigurationConstant_colorMode_0300', 0, async function (done) {
        console.log("------------start SUB_AA_OpenHarmony_ConfigurationConstant_colorMode_0300-------------");
        expect(ConfigurationConstant.ColorMode.COLOR_MODE_NOT_SET).assertEqual(-1);
        done();
        console.log("------------end SUB_AA_OpenHarmony_ConfigurationConstant_colorMode_0300-------------");
    });

    /*
     * @tc.number  SUB_AA_OpenHarmony_ConfigurationConstant_Direction_0100
     * @tc.name    Direction is "ConfigurationConstant.Direction.DIRECTION_VERTICAL"
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_ConfigurationConstant_Direction_0100', 0, async function (done) {
        console.log("------------start SUB_AA_OpenHarmony_ConfigurationConstant_Direction_0100-------------");
        expect(ConfigurationConstant.Direction.DIRECTION_VERTICAL).assertEqual(0);
        done();
        console.log("------------end SUB_AA_OpenHarmony_ConfigurationConstant_Direction_0100-------------");
    });

    /*
     * @tc.number  SUB_AA_OpenHarmony_ConfigurationConstant_Direction_0200
     * @tc.name    Direction is "ConfigurationConstant.Direction.DIRECTION_HORIZONTAL"
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_ConfigurationConstant_Direction_0200', 0, async function (done) {
        console.log("------------start SUB_AA_OpenHarmony_ConfigurationConstant_Direction_0200-------------");
        expect(ConfigurationConstant.Direction.DIRECTION_HORIZONTAL).assertEqual(1);
        done();
        console.log("------------end SUB_AA_OpenHarmony_ConfigurationConstant_Direction_0200-------------");
    });

    /*
     * @tc.number  SUB_AA_OpenHarmony_ConfigurationConstant_Direction_0300
     * @tc.name    Direction is "ConfigurationConstant.Direction.DIRECTION_NOT_SET"
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_ConfigurationConstant_Direction_0300', 0, async function (done) {
        console.log("------------start SUB_AA_OpenHarmony_ConfigurationConstant_Direction_0300-------------");
        expect(ConfigurationConstant.Direction.DIRECTION_NOT_SET).assertEqual(-1);
        done();
        console.log("------------end SUB_AA_OpenHarmony_ConfigurationConstant_Direction_0300-------------");
    });

    /*
     * @tc.number  SUB_AA_OpenHarmony_ConfigurationConstant_ScreenDensity_0100
     * @tc.name    ScreenDensity is "ConfigurationConstant.ScreenDensity.SCREEN_DENSITY_SDPI"
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_ConfigurationConstant_ScreenDensity_0100', 0, async function (done) {
        console.log("------------start SUB_AA_OpenHarmony_ConfigurationConstant_ScreenDensity_0100-------------");
        expect(ConfigurationConstant.ScreenDensity.SCREEN_DENSITY_SDPI).assertEqual(120);
        done();
        console.log("------------end SUB_AA_OpenHarmony_ConfigurationConstant_ScreenDensity_0100-------------");
    });

    /*
     * @tc.number  SUB_AA_OpenHarmony_ConfigurationConstant_ScreenDensity_0200
     * @tc.name    ScreenDensity is "ConfigurationConstant.ScreenDensity.SCREEN_DENSITY_MDPI"
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_ConfigurationConstant_ScreenDensity_0200', 0, async function (done) {
        console.log("------------start SUB_AA_OpenHarmony_ConfigurationConstant_ScreenDensity_0200-------------");
        expect(ConfigurationConstant.ScreenDensity.SCREEN_DENSITY_MDPI).assertEqual(160);
        done();
        console.log("------------end SUB_AA_OpenHarmony_ConfigurationConstant_ScreenDensity_0200-------------");
    });

    /*
     * @tc.number  SUB_AA_OpenHarmony_ConfigurationConstant_ScreenDensity_0300
     * @tc.name    ScreenDensity is "ConfigurationConstant.ScreenDensity.SCREEN_DENSITY_LDPI"
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_ConfigurationConstant_ScreenDensity_0300', 0, async function (done) {
        console.log("------------start SUB_AA_OpenHarmony_ConfigurationConstant_ScreenDensity_0300-------------");
        expect(ConfigurationConstant.ScreenDensity.SCREEN_DENSITY_LDPI).assertEqual(240);
        done();
        console.log("------------end SUB_AA_OpenHarmony_ConfigurationConstant_ScreenDensity_0300-------------");
    });

    /*
     * @tc.number  SUB_AA_OpenHarmony_ConfigurationConstant_ScreenDensity_0400
     * @tc.name    ScreenDensity is "ConfigurationConstant.ScreenDensity.SCREEN_DENSITY_XLDPI"
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_ConfigurationConstant_ScreenDensity_0400', 0, async function (done) {
        console.log("------------start SUB_AA_OpenHarmony_ConfigurationConstant_ScreenDensity_0400-------------");
        expect(ConfigurationConstant.ScreenDensity.SCREEN_DENSITY_XLDPI).assertEqual(320);
        done();
        console.log("------------end SUB_AA_OpenHarmony_ConfigurationConstant_ScreenDensity_0400-------------");
    });

    /*
     * @tc.number  SUB_AA_OpenHarmony_ConfigurationConstant_ScreenDensity_0500
     * @tc.name    ScreenDensity is "ConfigurationConstant.ScreenDensity.SCREEN_DENSITY_XXLDPI"
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_ConfigurationConstant_ScreenDensity_0500', 0, async function (done) {
        console.log("------------start SUB_AA_OpenHarmony_ConfigurationConstant_ScreenDensity_0500-------------");
        expect(ConfigurationConstant.ScreenDensity.SCREEN_DENSITY_XXLDPI).assertEqual(480);
        done();
        console.log("------------end SUB_AA_OpenHarmony_ConfigurationConstant_ScreenDensity_0500-------------");
    });

    /*
     * @tc.number  SUB_AA_OpenHarmony_ConfigurationConstant_ScreenDensity_0600
     * @tc.name    ScreenDensity is "ConfigurationConstant.ScreenDensity.SCREEN_DENSITY_XXXLDPI"
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_ConfigurationConstant_ScreenDensity_0600', 0, async function (done) {
        console.log("------------start SUB_AA_OpenHarmony_ConfigurationConstant_ScreenDensity_0600-------------");
        expect(ConfigurationConstant.ScreenDensity.SCREEN_DENSITY_XXXLDPI).assertEqual(640);
        done();
        console.log("------------end SUB_AA_OpenHarmony_ConfigurationConstant_ScreenDensity_0600-------------");
    });

    /*
     * @tc.number  SUB_AA_OpenHarmony_ConfigurationConstant_ScreenDensity_0700
     * @tc.name    ScreenDensity is "ConfigurationConstant.ScreenDensity.SCREEN_DENSITY_NOT_SET"
     * @tc.desc    Function test
     * @tc.level   0
     */
    it('SUB_AA_OpenHarmony_ConfigurationConstant_ScreenDensity_0700', 0, async function (done) {
        console.log("------------start SUB_AA_OpenHarmony_ConfigurationConstant_ScreenDensity_0700-------------");
        expect(ConfigurationConstant.ScreenDensity.SCREEN_DENSITY_NOT_SET).assertEqual(0);
        done();
        console.log("------------end SUB_AA_OpenHarmony_ConfigurationConstant_ScreenDensity_0700-------------");
    });
    
    
})