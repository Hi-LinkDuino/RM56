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
        // 方法
        this.functionTouch1()
        this.functionTouch2()
        this.functionTouch3()
        this.functionTouch4()
        this.functionTouch5()
        this.functionTouch6()
        this.functionTouch7()
        this.functionTouch8()
        this.functionTouch9()
    },

    functionTouch1() {
        const el =this.$refs.function1;
        const ctx = el.getContext('2d');
        var path1 = ctx.createPath2D("M80 20 L40 100 L120 100 Z");
        var path2 = ctx.createPath2D();
        path2.addPath(path1);
        ctx.stroke(path2);
    },
    functionTouch2() {
        const el =this.$refs.function2;
        const ctx = el.getContext('2d');
        var path = ctx.createPath2D("M80 20 L40 100 L120 100 Z");
        path.setTransform(0.8, 0, 0, 0.4, 0, 0);
        ctx.stroke(path);

    },
    functionTouch3() {
        const el =this.$refs.function3;
        const ctx = el.getContext('2d');
        var path = ctx.createPath2D();
        path.moveTo(80, 40);
        path.lineTo(120, 40);
        path.lineTo(80, 80);
        path.closePath();
        ctx.stroke(path);
    },
    functionTouch4() {
        const el =this.$refs.function4;
        const ctx = el.getContext('2d');
        var path = ctx.createPath2D();
        path.moveTo(10, 10);
        path.bezierCurveTo(10, 50, 100, 50, 100, 10);
        ctx.stroke(path);
    },
    functionTouch5() {
        const el =this.$refs.function5;
        const ctx = el.getContext('2d');
        var path = ctx.createPath2D();
        path.moveTo(10, 10);
        path.quadraticCurveTo(50, 50, 100, 10);
        ctx.stroke(path);
    },
    functionTouch6() {
        const el =this.$refs.function6;
        const ctx = el.getContext('2d');
        var path = ctx.createPath2D();
        path.arc(80, 50, 40, 0, 6.28);
        ctx.stroke(path);
    },
    functionTouch7() {
        const el =this.$refs.function7;
        const ctx = el.getContext('2d');
        var path = ctx.createPath2D();
        path.arcTo(75, 10, 75, 35, 25);
        ctx.stroke(path);
    },
    functionTouch8() {
        const el =this.$refs.function8;
        const ctx =el.getContext('2d');
        var path = ctx.createPath2D();
        path.ellipse(50, 50, 25, 50, Math.PI * 0.25, Math.PI * 0.5, Math.PI, 1);
        ctx.stroke(path);
    },
    functionTouch9() {
        const el =this.$refs.function9;
        const ctx = el.getContext('2d');
        var path = ctx.createPath2D();
        path.rect(20, 20, 100, 100);
        ctx.stroke(path);
    },

}