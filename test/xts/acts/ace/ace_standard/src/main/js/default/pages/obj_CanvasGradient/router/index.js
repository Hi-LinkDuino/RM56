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
        left:0,
        top:0,
    },
    onShow() {
        this.functionTest()
    },
    functionTest() {
        const el =this.$refs.canvas;
        const ctx =el.getContext('2d');
        const gradient = ctx.createLinearGradient(0,0,200,0);
        gradient.addColorStop(0,'#00ffff');
        gradient.addColorStop(1,'#ffff00');
        ctx.fillStyle=gradient;
        ctx.fillRect(20,20,300,100);
    },

}