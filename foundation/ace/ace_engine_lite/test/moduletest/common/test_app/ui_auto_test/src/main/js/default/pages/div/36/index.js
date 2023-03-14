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
import {routePage} from "../../../common/js/general";
export default {
    data:{
        divWidth: 410,
        divHeight: 190,
        divPadding: 10,
        divBorder: 1,
        buttonWidth: 300,
        buttonHeight: 90,
        buttonPadding: 5,
        buttonBorder: 5,
        focusView: "div"
    },
    changeFocusToDiv() {
        this.focusView = "div";
    },
    changeFocusToButton() {
        this.focusView = "button";
    },
    increaseWidth() {
        if (this.focusView === "div") {
            this.divWidth = this.divWidth + 50;
        } else if (this.focusView === "button") {
            this.buttonWidth = this.buttonWidth + 10;
        }
    },
    reduceWidth() {
        if (this.focusView === "div") {
            this.divWidth = this.divWidth - 50;
        } else if (this.focusView === "button") {
            this.buttonWidth = this.buttonWidth - 10;
        }
    },
    increaseHeight() {
        if (this.focusView === "div") {
            this.divHeight = this.divHeight + 50;
        } else if (this.focusView === "button") {
            this.buttonHeight = this.buttonHeight + 10;
        }
    },
    reduceHight() {
        if (this.focusView === "div") {
            this.divHeight = this.divHeight - 50;
        } else if (this.focusView === "button") {
            this.buttonHeight = this.buttonHeight - 10;
        }
    },
    increasePadding() {
        if (this.focusView === "div") {
            this.divPadding = this.divPadding + 10;
        } else if (this.focusView === "button") {
            this.buttonPadding = this.buttonPadding + 10;
        }
    },
    reducePadding() {
        if (this.focusView === "div") {
            this.divPadding = this.divPadding - 10;
        } else if (this.focusView === "button") {
            this.buttonPadding = this.buttonPadding - 10;
        }
    },
    increaseBorder() {
        if (this.focusView === "div") {
            this.divBorder = this.divBorder + 10;
        } else if (this.focusView === "button") {
            this.buttonBorder = this.buttonBorder + 10;
        }
    },
    reduceBorder() {
        if (this.focusView === "div") {
            this.divBorder = this.divBorder - 10;
        } else if (this.focusView === "button") {
            this.buttonBorder = this.buttonBorder - 10;
        }
    },
    ...routePage('pages/div/35/index')
}