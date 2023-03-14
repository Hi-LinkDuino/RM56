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
    onShow(){
        // 屬性
        this.props1()
        this.props2()
        this.props3()
    },
    props1() {
        const el =this.$refs.canvas1;
        const ctx = el.getContext('2d');
        ctx.fillRect(0,0,150,100)
        var imageData = ctx.createImageData(2,1)
        prompt.showToast({
            message:JSON.stringify(imageData)+',width:' +imageData.width,
            duration:5000
        })
        console.log(imageData.width)
    },
    props2() {
        const el =this.$refs.canvas2;
        const ctx = el.getContext('2d');
        ctx.fillRect(0,0,100,150)
        var imageData = ctx.createImageData(1,2)
        prompt.showToast({
            message:JSON.stringify(imageData)+',width:' +imageData.height,
            duration:5000
        })
        console.log(imageData.height)
    },
    props3() {
        const el =this.$refs.canvas3;
        const ctx = el.getContext('2d');
        ctx.fillRect(0,0,100,100)
        var imageData = ctx.createImageData(1,1)
        prompt.showToast({
            message:JSON.stringify(imageData),
            duration:5000
        })
        console.log(imageData.data)
    },

}