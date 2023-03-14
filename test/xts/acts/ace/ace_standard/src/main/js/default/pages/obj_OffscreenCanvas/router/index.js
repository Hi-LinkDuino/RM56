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
        // 属性
        this.props1()
        // 方法
        this.functionTouch1()
    },
    props1() {
        const el =this.$refs.canvas1
        const ctx = el.getContext('2d');
        var offscreen = new OffscreenCanvas(200,100);
        var offscreenCanvasCtx = offscreen.getContext("2d");
        var img = new Image();
        img.src = 'common/images/image.png';
        offscreenCanvasCtx.drawImage(img, 0, 0, 100, 100);
        prompt.showToast({
            message: 'OffscreenCanvas width='+offscreen.width+',OffscreenCanvas height='+offscreen.height
        });
        console.log('OffscreenCanvas width='+offscreen.width+',OffscreenCanvas height='+offscreen.height)
        var bitmap = offscreen.transferToImageBitmap();
        ctx.transferFromImageBitmap(bitmap);
    },

    functionTouch1() {
        const el =this.$refs.function1
        const ctx = el.getContext('2d');
        var offscreen = new OffscreenCanvas(500,500);
        var offscreenCanvasCtx = offscreen.getContext("2d");
        var img = new Image();
        img.src = 'common/images/image.png';
        offscreenCanvasCtx.drawImage(img, 0, 0, 100, 100);
        var dataURL = offscreen.toDataURL();
        var bitmap = offscreen.transferToImageBitmap();
        ctx.transferFromImageBitmap(bitmap);
        prompt.showToast({
            message: 'offscreenCanvasCtx='+offscreenCanvasCtx+',dataURL='+dataURL+',bitmap='+JSON.stringify(bitmap)
        });
        console.log('offscreenCanvasCtx='+offscreenCanvasCtx+',dataURL='+dataURL+',bitmap='+JSON.stringify(bitmap));

    },

}