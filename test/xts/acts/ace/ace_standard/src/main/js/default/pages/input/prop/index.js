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
        array: [
            {id: 1, name: 'jack', age: 18},
            {id: 2, name: 'tony', age: 18},
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

        typeText : null,
        typeEmail : null,
        typeDate : null,
        typeTime : null,
        typeNumber : null,
        typePassword : null,
        typeButton : null,
        typeCheckbox : null,
        typeRadio : null,
        typeNone : null,

        checkboxFalse : null,
        checkboxTrue : null,
        checkboxNone : null,
        radioFalse : null,
        radioTrue : null,
        radioNone : null,

        inputName : null,
        inputNameNone : null,
        inputValue : null,
        inputValueNone : null,
        textPlaceholder : null,
        textPlaceholderNone : null,
        emailPlaceholder : null,
        emailPlaceholderNone : null,
        datePlaceholder : null,
        datePlaceholderNone : null,
        timePlaceholder : null,
        timePlaceholderNone : null,
        numberPlaceholder : null,
        numberPlaceholderNone : null,
        passwordPlaceholder : null,
        passwordPlaceholderNone : null,
        maxlength : null,
        enterkeytypeDefault : null,
        enterkeytypeNext : null,
        enterkeytypeGo : null,
        enterkeytypeDone : null,
        enterkeytypeSend : null,
        enterkeytypeSearch : null,
        headericon : null,
        headericonNone : null,
        showcounterTrue : null,
        showcounterFalse : null,
        showcounterNone : null,
        menuoptions : null,
        menuoptionsNone : null,
        autofocusTrue : null,
        autofocusFalse : null,
        autofocusNone : null,
        selectedstart : null,
        selectedstartNone : null,
        selectedendTrue : null,
        selectedendNone : null,
        softkeyboardenabledTrue : null,
        softkeyboardenabledFalse : null,
        softkeyboardenabledNone : null,
        showpasswordiconTrue : null,
        showpasswordiconFalse : null,
        showpasswordiconNone : null
    },

    onShow(){
        this.getCommonPropValues();
        this.getSpecificPropValuesOne();
        this.getSpecificPropValuesTwo();
        globalThis.value = {
            idProp : this.idProp, classProp : this.classProp, classPropNone : this.classPropNone,
            styleProp : this.styleProp, refProp : this.refProp, refPropNone : this.refPropNone,
            disabledPropTrue : this.disabledPropTrue, disabledPropFalse : this.disabledPropFalse,
            disabledPropNone : this.disabledPropNone, focusablePropTrue : this.focusablePropTrue,
            focusablePropFalse : this.focusablePropFalse, focusablePropNone : this.focusablePropNone,
            dataProp : this.dataProp, dataPropNone : this.dataPropNone,
            clickEffectPropSmall : this.clickEffectPropSmall, clickEffectPropMedium : this.clickEffectPropMedium,
            clickEffectPropLarge : this.clickEffectPropLarge, clickEffectPropNone : this.clickEffectPropNone,
            dirPropRtl : this.dirPropRtl, dirPropAuto : this.dirPropAuto, dirPropLtr : this.dirPropLtr,
            dirPropNone : this.dirPropNone, forPropNull : this.forPropNull, forPropOne : this.forPropOne,
            forPropThree : this.forPropThree, ifPropTrue : this.ifPropTrue, showPropTrue : this.showPropTrue,
            showPropFalse : this.showPropFalse, showPropNone : this.showPropNone,
            
            typeText : this.typeText, typeEmail : this.typeEmail, typeDate : this.typeDate,
            typeTime : this.typeTime, typeNumber : this.typeNumber, typePassword : this.typePassword,
            typeButton : this.typeButton, typeCheckbox : this.typeCheckbox,
            typeRadio : this.typeRadio, typeNone : this.typeNone,
            checkboxFalse : this.checkboxFalse, checkboxTrue : this.checkboxTrue,
            checkboxNone : this.checkboxNone, radioFalse : this.radioFalse,
            radioTrue : this.radioTrue, radioNone : this.radioNone, inputName : this.inputName,
            inputNameNone : this.inputNameNone, inputValue : this.inputValue,
            inputValueNone : this.inputValueNone, textPlaceholder : this.textPlaceholder,
            textPlaceholderNone : this.textPlaceholderNone, emailPlaceholder : this.emailPlaceholder,
            emailPlaceholderNone : this.emailPlaceholderNone, datePlaceholder : this.datePlaceholder,
            datePlaceholderNone : this.datePlaceholderNone, timePlaceholder : this.timePlaceholder,
            timePlaceholderNone : this.timePlaceholderNone, numberPlaceholder : this.numberPlaceholder,
            numberPlaceholderNone : this.numberPlaceholderNone, passwordPlaceholder : this.passwordPlaceholder,
            passwordPlaceholderNone : this.passwordPlaceholderNone, maxlength : this.maxlength,
            enterkeytypeDefault : this.enterkeytypeDefault, enterkeytypeNext : this.enterkeytypeNext,
            enterkeytypeGo : this.enterkeytypeGo, enterkeytypeDone : this.enterkeytypeDone,
            enterkeytypeSend : this.enterkeytypeSend, enterkeytypeSearch : this.enterkeytypeSearch,
            headericon : this.headericon, headericonNone : this.headericonNone,
            showcounterTrue : this.showcounterTrue, showcounterFalse : this.showcounterFalse,
            showcounterNone : this.showcounterNone, menuoptions : this.menuoptions,
            menuoptionsNone : this.menuoptionsNone, autofocusTrue : this.autofocusTrue,
            autofocusFalse : this.autofocusFalse, autofocusNone : this.autofocusNone,
            selectedstart : this.selectedstart, selectedstartNone : this.selectedstartNone,
            selectedendTrue : this.selectedendTrue, selectedendNone : this.selectedendNone,
            softkeyboardenabledTrue : this.softkeyboardenabledTrue,
            softkeyboardenabledFalse : this.softkeyboardenabledFalse,
            softkeyboardenabledNone : this.softkeyboardenabledNone, showpasswordiconTrue : this.showpasswordiconTrue,
            showpasswordiconFalse : this.showpasswordiconFalse, showpasswordiconNone : this.showpasswordiconNone
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

    getSpecificPropValuesOne(){
        this.typeText = this.$element("typeText").getInspector()
        this.typeEmail = this.$element("typeEmail").getInspector()
        this.typeDate = this.$element("typeDate").getInspector()
        this.typeTime = this.$element("typeTime").getInspector()
        this.typeNumber = this.$element("typeNumber").getInspector()
        this.typePassword = this.$element("typePassword").getInspector()
        this.typeButton = this.$element("typeButton").getInspector()
        this.typeCheckbox = this.$element("typeCheckbox").getInspector()
        this.typeRadio = this.$element("typeRadio").getInspector()
        this.typeNone = this.$element("typeNone").getInspector()
        this.checkboxFalse = this.$element("checkboxFalse").getInspector()
        this.checkboxTrue = this.$element("checkboxTrue").getInspector()
        this.checkboxNone = this.$element("checkboxNone").getInspector()
        this.radioFalse = this.$element("radioFalse").getInspector()
        this.radioTrue = this.$element("radioTrue").getInspector()
        this.radioNone = this.$element("radioNone").getInspector()
        this.inputName = this.$element("inputName").getInspector()
        this.inputNameNone = this.$element("inputNameNone").getInspector()
        this.inputValue = this.$element("inputValue").getInspector()
        this.inputValueNone = this.$element("inputValueNone").getInspector()
        this.textPlaceholder = this.$element("textPlaceholder").getInspector()
        this.textPlaceholderNone = this.$element("textPlaceholderNone").getInspector()
        this.emailPlaceholder = this.$element("emailPlaceholder").getInspector()
        this.emailPlaceholderNone = this.$element("emailPlaceholderNone").getInspector()
        this.datePlaceholder = this.$element("datePlaceholder").getInspector()
        this.datePlaceholderNone = this.$element("datePlaceholderNone").getInspector()
        this.timePlaceholder = this.$element("timePlaceholder").getInspector()
        this.timePlaceholderNone = this.$element("timePlaceholderNone").getInspector()
        this.numberPlaceholder = this.$element("numberPlaceholder").getInspector()
        this.numberPlaceholderNone = this.$element("numberPlaceholderNone").getInspector()
        this.passwordPlaceholder = this.$element("passwordPlaceholder").getInspector()
        this.passwordPlaceholderNone = this.$element("passwordPlaceholderNone").getInspector()
        this.maxlength = this.$element("maxlength").getInspector()

    },
    getSpecificPropValuesTwo(){
        this.enterkeytypeDefault = this.$element("enterkeytypeDefault").getInspector()
        this.enterkeytypeNext = this.$element("enterkeytypeNext").getInspector()
        this.enterkeytypeGo = this.$element("enterkeytypeGo").getInspector()
        this.enterkeytypeDone = this.$element("enterkeytypeDone").getInspector()
        this.enterkeytypeSend = this.$element("enterkeytypeSend").getInspector()
        this.enterkeytypeSearch = this.$element("enterkeytypeSearch").getInspector()
        this.headericon = this.$element("headericon").getInspector()
        this.headericonNone = this.$element("headericonNone").getInspector()
        this.showcounterTrue = this.$element("showcounterTrue").getInspector()
        this.showcounterFalse = this.$element("showcounterFalse").getInspector()
        this.showcounterNone = this.$element("showcounterNone").getInspector()
        this.menuoptions = this.$element("menuoptions").getInspector()
        this.menuoptionsNone = this.$element("menuoptionsNone").getInspector()
        this.autofocusTrue = this.$element("autofocusTrue").getInspector()
        this.autofocusFalse = this.$element("autofocusFalse").getInspector()
        this.autofocusNone = this.$element("autofocusNone").getInspector()
        this.selectedstart = this.$element("selectedstart").getInspector()
        this.selectedstartNone = this.$element("selectedstartNone").getInspector()
        this.selectedendTrue = this.$element("selectedendTrue").getInspector()
        this.selectedendNone = this.$element("selectedendNone").getInspector()
        this.softkeyboardenabledTrue = this.$element("softkeyboardenabledTrue").getInspector()
        this.softkeyboardenabledFalse = this.$element("softkeyboardenabledFalse").getInspector()
        this.softkeyboardenabledNone = this.$element("softkeyboardenabledNone").getInspector()
        this.showpasswordiconTrue = this.$element("showpasswordiconTrue").getInspector()
        this.showpasswordiconFalse = this.$element("showpasswordiconFalse").getInspector()
        this.showpasswordiconNone = this.$element("showpasswordiconNone").getInspector()
    }
}
