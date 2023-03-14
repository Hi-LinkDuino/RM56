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
        this.props4()
        this.props5()
        this.props6()
        this.props7()
        this.props8()
        this.props9()
        this.props10()
        this.props11()
        this.props12()
        this.props13()
        this.props14()
        this.props15()
        this.props16()
        this.props17()

    },
    props1() {
        const el = this.$refs.canvas1;
        const ctx = el.getContext('2d');
        ctx.fillStyle = '#0000ff';
        ctx.fillRect(0, 0, 100, 30);
    },
    props2() {
        const el = this.$refs.canvas2;
        const ctx = el.getContext('2d');
        ctx.lineWidth = 10;
        ctx.lineCap = 'round';
        ctx.strokeRect(0, 0, 100, 30);
    },
    props3() {
        const el = this.$refs.canvas3;
        const ctx = el.getContext('2d');
        ctx.lineWidth = 8;
        ctx.beginPath();
        ctx.lineCap = 'round';
        ctx.moveTo(10, 10);
        ctx.lineTo(100, 10);
        ctx.stroke();
    },
    props4() {
        const el = this.$refs.canvas4;
        const ctx = el.getContext('2d');
        ctx.beginPath();
        ctx.lineWidth = 5;
        ctx.lineJoin = 'miter';
        ctx.moveTo(10, 10);
        ctx.lineTo(80, 30);
        ctx.lineTo(10, 50);
        ctx.stroke();
    },
    props5() {
        const el =this.$refs.canvas5;
        const ctx = el.getContext('2d');
        ctx.lineWidth = 5;
        ctx.lineJoin = 'miter';
        ctx.miterLimit = 3;
        ctx.moveTo(10, 10);
        ctx.lineTo(60, 15);
        ctx.lineTo(10, 40);
        ctx.stroke();
    },
    props6() {
        const el =this.$refs.canvas6;
        const ctx = el.getContext('2d');
        ctx.font = '20px sans-serif';
        ctx.fillText("Hello World", 10, 20);
    },
    props7() {
        const el =this.$refs.canvas7;
        const ctx = el.getContext('2d');
        ctx.fillStyle = 'rgb(255,0,0)';
        ctx.fillRect(0, 0, 40, 40);
        ctx.globalAlpha = 0.4;
        ctx.fillStyle = 'rgb(0,0,255)';
        ctx.fillRect(40, 40, 40, 40);

    },
    props8() {
        const el =this.$refs.canvas8;
        const ctx = el.getContext('2d');
        ctx.arc(50, 40, 30, 0, 6.28);
        ctx.setLineDash([10,20]);
        ctx.lineDashOffset = 10.0;
        ctx.stroke();
    },
    props9() {
        const el =this.$refs.canvas9;
        const ctx = el.getContext('2d');
        ctx.fillStyle = 'rgb(255,0,0)';
        ctx.fillRect(10, 10, 30, 30);
        ctx.globalCompositeOperation = 'source-over';
        ctx.fillStyle = 'rgb(0,0,255)';
        ctx.fillRect(25, 25, 30, 30);
        // Start drawing second example
        ctx.fillStyle = 'rgb(255,0,0)';
        ctx.fillRect(70, 10, 30, 30);
        ctx.globalCompositeOperation = 'destination-over';
        ctx.fillStyle = 'rgb(0,0,255)';
        ctx.fillRect(85, 25, 30, 30);
    },
    props10() {
        const el =this.$refs.canvas10;
        const ctx = el.getContext('2d');
        ctx.shadowBlur = 20;
        ctx.shadowColor = 'rgb(0,0,0)';
        ctx.fillStyle = 'rgb(255,0,0)';
        ctx.fillRect(10, 10, 70, 40);
    },
    props11() {
        const el =this.$refs.canvas11;
        const ctx = el.getContext('2d');
        ctx.shadowBlur = 20;
        ctx.shadowColor = 'rgb(0,0,255)';
        ctx.fillStyle = 'rgb(255,0,0)';
        ctx.fillRect(10, 10, 70, 40);
    },
    props12() {
        const el =this.$refs.canvas12;
        const ctx = el.getContext('2d');
        ctx.shadowBlur = 10;
        ctx.shadowOffsetX = 20;
        ctx.shadowColor = 'rgb(0,0,0)';
        ctx.fillStyle = 'rgb(255,0,0)';
        ctx.fillRect(10, 0, 70, 40);
    },
    props13() {
        const el =this.$refs.canvas13;
        const ctx = el.getContext('2d');
        ctx.shadowBlur = 10;
        ctx.shadowOffsetY = 20;
        ctx.shadowColor = 'rgb(0,0,0)';
        ctx.fillStyle = 'rgb(255,0,0)';
        ctx.fillRect(10, 0, 70, 40);
    },
    props14() {
        const el =this.$refs.canvas14;
        const ctx = el.getContext('2d');
        var img = new Image();
        img.src = 'common/images/image.png';
        img.onload = function() {
            ctx.imageSmoothingEnabled = false;
            ctx.drawImage(img, 10, 0, 100, 60);
        };
    },
    props15() {
        const el =this.$refs.canvas15;
        const ctx = el.getContext('2d');
        ctx.lineWidth = 10;
        ctx.strokeStyle = '#0000ff';
        ctx.strokeRect(0, 0, 100, 30);
    },
    props16() {
        const el =this.$refs.canvas16;
        const ctx = el.getContext('2d');
        ctx.strokeStyle = '#0000ff';
        ctx.moveTo(80, 0);
        ctx.lineTo(80, 160);
        ctx.stroke();

        ctx.font = '12px sans-serif';

        // Show the different textAlign values
        ctx.textAlign = 'start';
        ctx.fillText('textAlign=start', 80, 20);
        ctx.textAlign = 'end';
        ctx.fillText('textAlign=end', 80, 30);
        ctx.textAlign = 'left';
        ctx.fillText('textAlign=left', 80, 40);
        ctx.textAlign = 'center';
        ctx.fillText('textAlign=center',80, 50);
        ctx.textAlign = 'right';
        ctx.fillText('textAlign=right',80, 60);
    },
    props17() {
        const el =this.$refs.canvas17;
        const ctx = el.getContext('2d');
        ctx.strokeStyle = '#0000ff';
        ctx.moveTo(0, 30);
        ctx.lineTo(200, 30);
        ctx.stroke();

        ctx.font = '10px sans-serif';

        ctx.textBaseline = 'top';
        ctx.fillText('Top', 0, 30);
        ctx.textBaseline = 'bottom';
        ctx.fillText('Bottom', 20, 30);
        ctx.textBaseline = 'middle';
        ctx.fillText('Middle', 55, 30);
        ctx.textBaseline = 'alphabetic';
        ctx.fillText('Alphabetic', 90, 30);
        ctx.textBaseline = 'hanging';
        ctx.fillText('Hanging', 140, 30);
    }

}