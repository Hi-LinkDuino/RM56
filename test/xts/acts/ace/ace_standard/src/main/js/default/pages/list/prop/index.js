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
         ifPropFalse : null,
         ifPropNone : null,
         showPropTrue : null,
         showPropFalse : null,
         showPropNone : null ,
         array:['#',1,2,3,4,5,6,7,8],
         scrollPageTrue:null,
         scrollPageFalse:null,
         scrollPageNone:null,
         cachedcount:null,
         scrollbarOff:null,
         scrollbarAuto:null,
         scrollbarOn:null,
         scrollbarNone:null,
         scrolleffectSpring:null,
         scrolleffectFade:null,
         scrolleffectNo:null,
         scrolleffectNone:null,
         indexerTrue:null,
         indexerFalse:null,
         indexerArray:null,
         indexerNone:null,
         indexercircleTrue:null,
         indexercircleFalse:null,
         indexercircleNone:null,
         indexermultiTrue:null,
         indexermultiFalse:null,
         indexermultiNone:null,
         indexerbubbleTrue:null,
         indexerbubbleFalse:null,
         indexerbubbleNone:null,
         dividerTrue:null,
         dividerFalse:null,
         dividerNone:null,
         shapemodeDefault:null,
         shapemodeRect:null,
         shapemodeRound:null,
         shapemodeNone:null,
         chainanimationTrue:null,
         chainanimationFalse:null,
         chainanimationNone:null,
         updateeffectTrue:null,
         updateeffectFalse:null,
         updateeffectNone:null,
         initialindex:null,
         initialoffset:null,
         selected:null,
         selectedNone:null
     },
 
     onShow(){
         this.getCommonPropValues();
         this.getSpecificPropValues()
         globalThis.value = {
             idProp : this.idProp, classProp : this.classProp,
             classPropNone : this.classPropNone, styleProp : this.styleProp,
             refProp : this.refProp, refPropNone : this.refPropNone,
             disabledPropTrue : this.disabledPropTrue, disabledPropFalse : this.disabledPropFalse,
             disabledPropNone : this.disabledPropNone, focusablePropTrue : this.focusablePropTrue,
             focusablePropFalse : this.focusablePropFalse, focusablePropNone : this.focusablePropNone,
             dataProp : this.dataProp, dataPropNone : this.dataPropNone,
             clickEffectPropSmall : this.clickEffectPropSmall, clickEffectPropMedium : this.clickEffectPropMedium,
             clickEffectPropLarge : this.clickEffectPropLarge, clickEffectPropNone : this.clickEffectPropNone,
             dirPropRtl : this.dirPropRtl, dirPropAuto : this.dirPropAuto,
             dirPropLtr : this.dirPropLtr, dirPropNone : this.dirPropNone,
             forPropNull : this.forPropNull, forPropOne : this.forPropOne,
             forPropThree : this.forPropThree, ifPropTrue : this.ifPropTrue,
             showPropTrue : this.showPropTrue, showPropFalse : this.showPropFalse,
             showPropNone : this.showPropNone, scrollPageTrue:this.scrollPageTrue,
             scrollPageFalse:this.scrollPageFalse, scrollPageNone:this.scrollPageNone,
             cachedcount:this.cachedcount, scrollbarOff:this.scrollbarOff,
             scrollbarAuto:this.scrollbarAuto, scrollbarOn:this.scrollbarOn,
             scrollbarNone:this.scrollbarNone, scrolleffectSpring:this.scrolleffectSpring,
             scrolleffectFade:this.scrolleffectFade,
             scrolleffectNo:this.scrolleffectNo, scrolleffectNone:this.scrolleffectNone,
             indexerTrue:this.indexerTrue, indexerFalse:this.indexerFalse,
             indexerArray:this.indexerArray, indexerNone:this.indexerNone,
             indexercircleTrue:this.indexercircleTrue, indexercircleFalse:this.indexercircleFalse,
             indexercircleNone:this.indexercircleNone, indexermultiTrue:this.indexermultiTrue,
             indexermultiFalse:this.indexermultiFalse, indexermultiNone:this.indexermultiNone,
             indexerbubbleTrue:this.indexerbubbleTrue, indexerbubbleFalse:this.indexerbubbleFalse,
             indexerbubbleNone:this.indexerbubbleNone, dividerTrue:this.dividerTrue,
             dividerFalse:this.dividerFalse, dividerNone:this.dividerNone,
             shapemodeDefault:this.shapemodeDefault, shapemodeRect:this.shapemodeRect,
             shapemodeRound:this.shapemodeRound, shapemodeNone:this.shapemodeNone,
             chainanimationTrue:this.chainanimationTrue,
             chainanimationFalse:this.chainanimationFalse,
             chainanimationNone:this.chainanimationNone,
             updateeffectTrue:this.updateeffectTrue,
             updateeffectFalse:this.updateeffectFalse,
             updateeffectNone:this.updateeffectNone,
             initialindex:this.initialindex, initialoffset:this.initialoffset,
             selected:this.selected, selectedNone:this.selectedNone
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
         this.scrollPageTrue = this.$element("scrollPageTrue").getInspector()
         this.scrollPageFalse = this.$element("scrollPageFalse").getInspector()
         this.scrollPageNone = this.$element("scrollPageNone").getInspector()
         this.cachedcount = this.$element("cachedcount").getInspector()
         this.scrollbarOff = this.$element("scrollbarOff").getInspector()
         this.scrollbarAuto = this.$element("scrollbarAuto").getInspector()
         this.scrollbarOn = this.$element("scrollbarOn").getInspector()
         this.scrollbarNone = this.$element("scrollbarNone").getInspector()
         this.scrolleffectSpring = this.$element("scrolleffectSpring").getInspector()
         this.scrolleffectFade = this.$element("scrolleffectFade").getInspector()
         this.scrolleffectNo = this.$element("scrolleffectNo").getInspector()
         this.scrolleffectNone = this.$element("scrolleffectNone").getInspector()
         this.indexerTrue = this.$element("indexerTrue").getInspector()
         this.indexerFalse = this.$element("indexerFalse").getInspector()
         this.indexerArray = this.$element("indexerArray").getInspector()
         this.indexerNone = this.$element("indexerNone").getInspector()
         this.indexercircleTrue = this.$element("indexercircleTrue").getInspector()
         this.indexercircleFalse = this.$element("indexercircleFalse").getInspector()
         this.indexercircleNone = this.$element("indexercircleNone").getInspector()
         this.indexermultiTrue = this.$element("indexermultiTrue").getInspector()
         this.indexermultiFalse = this.$element("indexermultiFalse").getInspector()
         this.indexermultiNone = this.$element("indexermultiNone").getInspector()
         this.indexerbubbleTrue = this.$element("indexerbubbleTrue").getInspector()
         this.indexerbubbleFalse = this.$element("indexerbubbleFalse").getInspector()
         this.indexerbubbleNone = this.$element("indexerbubbleNone").getInspector()
         this.dividerTrue = this.$element("dividerTrue").getInspector()
         this.dividerFalse = this.$element("dividerFalse").getInspector()
         this.dividerNone = this.$element("dividerNone").getInspector()
         this.shapemodeDefault = this.$element("shapemodeDefault").getInspector()
         this.shapemodeRect = this.$element("shapemodeRect").getInspector()
         this.shapemodeRound = this.$element("shapemodeRound").getInspector()
         this.shapemodeNone = this.$element("shapemodeNone").getInspector()
         this.chainanimationTrue = this.$element("chainanimationTrue").getInspector()
         this.chainanimationFalse = this.$element("chainanimationFalse").getInspector()
         this.chainanimationNone = this.$element("chainanimationNone").getInspector()
         this.updateeffectTrue = this.$element("updateeffectTrue").getInspector()
         this.updateeffectFalse = this.$element("updateeffectFalse").getInspector()
         this.updateeffectNone = this.$element("updateeffectNone").getInspector()
         this.initialindex = this.$element("initialindex").getInspector()
         this.initialoffset = this.$element("initialoffset").getInspector()
         this.selected = this.$element("selected").getInspector()
         this.selectedNone = this.$element("selectedNone").getInspector()
     }
 }
 