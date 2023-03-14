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

import prompt from '@system.prompt';

export default {
    data:{
        lineData: [
            {
                strokeColor: '#0081ff',
                fillColor: '#cce5ff',
                data: [763, 550, 551, 554, 731, 654, 525, 696,
                595, 628, 791, 505, 613, 575, 475, 553, 491, 680, 657, 716],
                gradient: true,
            }
        ],
        lineOps: {
            xAxis: {
                min: 0,
                max: 20,
                display: false,
            },
            yAxis: {
                min: 0,
                max: 1000,
                display: false,
            },
            series: {
                lineStyle: {
                    width: "5px",
                    smooth: true,
                },
                headPoint: {
                    shape: "circle",
                    size: 20,
                    strokeWidth: 5,
                    fillColor: '#ffffff',
                    strokeColor: '#007aff',
                    display: true,
                },
                loop: {
                    margin: 2,
                    gradient: true,
                }
            }
        },
        lineSeg:{
            startColor:'#f7ce00',
            endColor:'#d9542b',
            value:50,
            name:'蔬菜'
        },
        lineRain:[
            {
                startColor:'#f7ce00',
                endColor:'#d9542b',
                value:50,
                name:'蔬菜'
            },
            {
                startColor:'#f7ce00',
                endColor:'#d9542b',
                value:50,
                name:'水果'
            }
        ],
        listOne:[{}],
        listThree:[{},{},{}],
        idProp : null,
        classProp : null,
        classPropNone : null,
        styleProp : null,
        refProp : null,
        refPropNone : null,
        disabledPropTrue : null,
        disabledPropFalse : null,
        disabledPropNone : null,
        focusablePropTrue : null,
        focusablePropFalse : null,
        focusablePropNone : null,
        dataProp : null,
        dataPropNone : null,
        clickEffectPropSmall : null,
        clickEffectPropMedium : null,
        clickEffectPropLarge : null,
        clickEffectPropNone : null,
        dirPropRtl : null,
        dirPropAuto : null,
        dirPropLtr : null,
        dirPropNone : null,
        forPropNull : null,
        forPropOne : null,
        forPropThree : null,
        ifPropTrue : null,
        showPropTrue : null,
        showPropFalse : null,
        showPropNone : null,

        typeBar : null,
        typeLine : null,
        typeGauge : null,
        typeProgress : null,
        typeLoading : null,
        typeRainbow : null,
        typeNone : null,
        options : null,
        optionsNone : null,
        datasets : null,
        datasetsNone : null,
        segmentsSeg : null,
        segmentsRain : null,
        segmentsNone : null,
        effectsTrue : null,
        effectsFalse : null,
        effectsNone : null,
        animationduration : null,
        animationdurationNone : null,
    },
    onShow(){
        this.getCommonPropValues();
        this.getSpecificPropValues();
        globalThis.value = {
            idProp : this.idProp, classProp : this.classProp, classPropNone : this.classPropNone,
            styleProp : this.styleProp, refProp : this.refProp, refPropNone : this.refPropNone,
            disabledPropTrue : this.disabledPropTrue, disabledPropFalse : this.disabledPropFalse,
            disabledPropNone : this.disabledPropNone, focusablePropTrue : this.focusablePropTrue,
            focusablePropFalse : this.focusablePropFalse, focusablePropNone : this.focusablePropNone,
            dataProp : this.dataProp, dataPropNone : this.dataPropNone,
            clickEffectPropSmall : this.clickEffectPropSmall,
            clickEffectPropMedium : this.clickEffectPropMedium,
            clickEffectPropLarge : this.clickEffectPropLarge,
            clickEffectPropNone : this.clickEffectPropNone, dirPropRtl : this.dirPropRtl,
            dirPropAuto : this.dirPropAuto,
            dirPropLtr : this.dirPropLtr, dirPropNone : this.dirPropNone, forPropNull : this.forPropNull,
            forPropOne : this.forPropOne, forPropThree : this.forPropThree, ifPropTrue : this.ifPropTrue,
            showPropTrue : this.showPropTrue, showPropFalse : this.showPropFalse, showPropNone : this.showPropNone,

            typeBar : this.typeBar, typeLine : this.typeLine, typeGauge : this.typeGauge,
            typeProgress : this.typeProgress, typeLoading : this.typeLoading, typeRainbow : this.typeRainbow,
            typeNone : this.typeNone, options : this.options,
            optionsNone : this.optionsNone, datasets : this.datasets, datasetsNone : this.datasetsNone,
            segmentsSeg : this.segmentsSeg, segmentsRain : this.segmentsRain, segmentsNone : this.segmentsNone,
            effectsTrue : this.effectsTrue, effectsFalse : this.effectsFalse, effectsNone : this.effectsNone,
            animationduration : this.animationduration, animationdurationNone : this.animationdurationNone
        }
    },
    getCommonPropValues(){
        this.idProp = this.$element("idProp").getInspector()
        this.classProp = this.$element("classProp").getInspector()
        this.classPropNone = this.$element("classPropNone").getInspector()
        this.styleProp = this.$element("styleProp").getInspector()
        this.refProp = this.$element("refProp").getInspector()
        this.refPropNone = this.$element("refPropNone").getInspector()
        this.disabledPropTrue = this.$element("disabledPropTrue").getInspector()
        this.disabledPropFalse = this.$element("disabledPropFalse").getInspector()
        this.disabledPropNone = this.$element("disabledPropNone").getInspector()
        this.focusablePropTrue = this.$element("focusablePropTrue").getInspector()
        this.focusablePropFalse = this.$element("focusablePropFalse").getInspector()
        this.focusablePropNone = this.$element("focusablePropNone").getInspector()
        this.dataProp = this.$element("dataProp").getInspector()
        this.dataPropNone = this.$element("dataPropNone").getInspector()
        this.clickEffectPropSmall = this.$element("clickEffectPropSmall").getInspector()
        this.clickEffectPropMedium = this.$element("clickEffectPropMedium").getInspector()
        this.clickEffectPropLarge = this.$element("clickEffectPropLarge").getInspector()
        this.clickEffectPropNone = this.$element("clickEffectPropNone").getInspector()
        this.dirPropRtl = this.$element("dirPropRtl").getInspector()
        this.dirPropAuto = this.$element("dirPropAuto").getInspector()
        this.dirPropLtr = this.$element("dirPropLtr").getInspector()
        this.dirPropNone = this.$element("dirPropNone").getInspector()
        this.forPropNull = this.$element("forPropNull").getInspector()
        this.forPropOne = this.$element("forPropOne").getInspector()
        this.forPropThree = this.$element("forPropThree").getInspector()
        this.ifPropTrue = this.$element("ifPropTrue").getInspector()
        this.showPropTrue = this.$element("showPropTrue").getInspector()
        this.showPropFalse = this.$element("showPropFalse").getInspector()
        this.showPropNone = this.$element("showPropNone").getInspector()
    },
    getSpecificPropValues(){
        this.typeBar = this.$element("typeBar").getInspector()
        this.typeLine = this.$element("typeLine").getInspector()
        this.typeGauge = this.$element("typeGauge").getInspector()
        this.typeProgress = this.$element("typeProgress").getInspector()
        this.typeLoading = this.$element("typeLoading").getInspector()
        this.typeRainbow = this.$element("typeRainbow").getInspector()
        this.typeNone = this.$element("typeNone").getInspector()
        this.options = this.$element("options").getInspector()
        this.optionsNone = this.$element("optionsNone").getInspector()
        this.datasets = this.$element("datasets").getInspector()
        this.datasetsNone = this.$element("datasetsNone").getInspector()
        this.segmentsSeg = this.$element("segmentsSeg").getInspector()
        this.segmentsRain = this.$element("segmentsRain").getInspector()
        this.segmentsNone = this.$element("segmentsNone").getInspector()
        this.effectsTrue = this.$element("effectsTrue").getInspector()
        this.effectsFalse = this.$element("effectsFalse").getInspector()
        this.effectsNone = this.$element("effectsNone").getInspector()
        this.animationduration = this.$element("animationduration").getInspector()
        this.animationdurationNone = this.$element("animationdurationNone").getInspector()
    }
}
