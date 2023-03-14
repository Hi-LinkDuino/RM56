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
        content: `
        <div class="flex-direction: column; background-color: #ffffff;
        padding: 30px; margin-bottom: 30px;"  style="background-color: #FFFFFF">
          <style>h1{color: yellow;}</style>
          <p class="item-title">h1</p>
          <h1>文本测试(h1测试)</h1>
          <p class="item-title">h2</p>
          <h2>文本测试(h2测试)</h2>
        </div>
        `,
        listOne:[{}],
        listThree:[{},{},{}],
        idProp : null,
        classProp : null,
        classPropNone : null,
        styleProp : null
    },

    onShow(){
        this.getCommonPropValues();
        this.getSpecificPropValues();
        globalThis.value = {
            idProp : this.idProp,
            classProp : this.classProp,
            classPropNone : this.classPropNone,
            styleProp : this.styleProp
        }
    },

    getCommonPropValues(){
        this.idProp = this.$element("idProp").getInspector()
        this.classProp = this.$element("classProp").getInspector()
        this.classPropNone = this.$element("classPropNone").getInspector()
        this.styleProp = this.$element("styleProp").getInspector()
    },

    getSpecificPropValues(){

    }
}
