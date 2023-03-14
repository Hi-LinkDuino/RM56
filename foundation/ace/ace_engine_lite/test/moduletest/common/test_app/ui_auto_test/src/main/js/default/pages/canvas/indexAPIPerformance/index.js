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
    ...routePage("pages/canvas/indexAPI/index"),
    performance001Func: function () {
        var myDate = new Date();
        var myTime=myDate.toLocaleTimeString();
        var canvasRef = this.$refs.canvas1;
        var ctx = canvasRef.getContext("2d");
        console.log("stroke 1 time contains 100 times moveTo+lineTo begin =========" + myTime);
        // 画101条线
        var i;
        var x = 0;
        // 红色
        ctx.strokeStyle = "#FF0000";
        ctx.lineWidth = 5;
        for(i = 0; i< 100; i++) {
            x = x + 10;
            ctx.moveTo(10,x);
            ctx.lineTo(30,x);
            if(i === 99) {
                ctx.stroke();
                console.log("stroke 1 time contains 100 times moveTo+lineTo end =========" + myTime);
                ctx.beginPath();
                // 蓝色
                ctx.strokeStyle = "#0000FF";
                ctx.lineWidth = 3;
                ctx.moveTo(10,x+10);
                ctx.lineTo(30,x+10);
                ctx.stroke();
            }
        }

        // 画101个矩形
        console.log("stroke 1 time contains 100 times rect begin =========" + myTime);
        ctx.beginPath();
        var y = 0;
        for(i = 0; i< 100; i++) {
            y = y + 10;
            ctx.rect(40,y, 20,4);
            if(i === 99) {
                ctx.stroke();
                console.log("stroke 1 time contains 100 times rect end =========" + myTime);
                ctx.beginPath();
                // 红色
                ctx.strokeStyle = "#FF0000";
                ctx.lineWidth = 2;
                ctx.rect(40,y+10, 20,4);
                ctx.stroke();
            }
        }

        // 画101个圆
        console.log("stroke 1 time contains 100 times arc begin =========" + myTime);
        ctx.beginPath();
        var z = 0;
        for(i = 0; i< 100; i++) {
            z = z + 10;
            ctx.arc(80,z,5,Math.PI*2*(0/360),Math.PI*2*(90/360),false);
            if(i === 99) {
                ctx.stroke();
                console.log("stroke 1 time contains 100 times arc end =========" + myTime);
                ctx.beginPath();
                // 蓝色
                ctx.strokeStyle = "#0000FF";
                ctx.lineWidth = 2;
                ctx.arc(80,z+10,5,Math.PI*2*(0/360),Math.PI*2*(90/360),false);
                ctx.stroke();
            }
        }

        // 画101条线
        x = 0;
        // 红色
        ctx.strokeStyle = "#FF0000";
        ctx.lineWidth = 1;
        console.log("stroke 100 times contains 100 times moveTo+lineTo begin =========" + myTime);
        for(i = 0; i< 100; i++) {
            x = x + 10;
            ctx.beginPath();
            ctx.moveTo(100,x);
            ctx.lineTo(130,x);
            ctx.stroke();
        }
        console.log("stroke 100 times contains 100 times moveTo+lineTo end =========" + myTime);

        // 画100个矩形
        y = 0;
        // 红色
        ctx.strokeStyle = "#FF0000";
        console.log("stroke 100 times contains 100 times rect begin =========" + myTime);
        for(i = 0; i< 100; i++) {
            y = y + 10;
            ctx.beginPath();
            ctx.rect(140,y, 20,4);
            ctx.stroke();
        }
        console.log("stroke 100 time contains 100 times rect end =========" + myTime);

        // 画100个弧
        z = 0;
        // 红色
        ctx.strokeStyle = "#FF0000";
        ctx.lineWidth = 1;
        console.log("stroke 100 times contains 100 times arc begin =========" + myTime);
        for(i = 0; i< 100; i++) {
            z = z + 10;
            ctx.beginPath();
            ctx.arc(170,z,5,Math.PI*2*(0/360),Math.PI*2*(140/360),false);
            ctx.stroke();
        }
        console.log("stroke 100 times contains 100 times arc end =========" + myTime);
    },
    performance002Func: function() {
        var myDate = new Date();
        var myTime=myDate.toLocaleTimeString();
        var canvasRef = this.$refs.canvas2;
        var ctx = canvasRef.getContext("2d");
        // 画10条线
        var i = 0;
        // 红色
        ctx.strokeStyle = "#FF0000";
        ctx.lineWidth = 1;
        console.log("stroke 10 times contains 10 times moveTo+lineTo begin =========" + myTime);
        for(i = 0; i< 10; i++) {
            ctx.beginPath();
            ctx.moveTo(10,30);
            ctx.lineTo(60,30);
            ctx.stroke();
        }
        console.log("stroke 10 times contains 10 times moveTo+lineTo end =========" + myTime);

        // 画10个矩形
        // 红色
        ctx.strokeStyle = "#FF0000";
        console.log("stroke 10 times contains 10 times rect begin =========" + myTime);
        for(i = 0; i< 10; i++) {
            ctx.beginPath();
            ctx.rect(80,30,50,10);
            ctx.stroke();
        }
        console.log("stroke 10 time contains 10 times rect end =========" + myTime);

        // 画10个弧
        // 红色
        ctx.strokeStyle = "#FF0000";
        ctx.lineWidth = 1;
        console.log("stroke 10 times contains 10 times arc begin =========" + myTime);
        for(i = 0; i< 10; i++) {
            ctx.beginPath();
            ctx.arc(160,30,10,Math.PI*2*(0/360),Math.PI*2*(360/360),false);
            ctx.stroke();
        }
        console.log("stroke 10 times contains 10 times arc end =========" + myTime);
    }
}