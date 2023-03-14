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

import device from '@system.device';
import {backPage} from "../../common/js/general";

export default {
    data: {
        brand:'',
        manufacturer:'',
        model:'',
        product:'',
        language:'',
        region:'',
        windowWidth:'',
        windowHeight:'',
        screenDensity:'',
        screenShape:'',
        apiVersion:'',
        deviceType:''
    },
    backMain:backPage().backMain,
    clickAction:function(){
        let that = this;
        device.getInfo({
            success:function(data){
                that.brand = 'band:' + data.brand;
                that.manufacturer = 'manufacturer:' + data.manufacturer;
                that.model = 'model:' + data.model;
                that.product = 'product:' + data.product;
                that.language = 'language:' + data.language;
                that.region = 'region:' + data.region;
                that.windowHeight = 'windowHeight:' + data.windowHeight;
                that.windowWidth = 'windowWidth:' + data.windowWidth;
                that.screenDensity = 'screenDensity:' + data.screenDensity;
                that.screenShape = 'screenShape:' + data.screenShape;
                that.apiVersion = 'apiVersion:' + data.apiVersion;
                that.deviceType = 'deviceType:' + data.deviceType;
                console.log("get device info success");
            },
            fail:function(data,code){
                that.brand = "get device info failed, errorcode:"+code;
                console.log("get device info failed");
            },
            complete:function(){
                console.log("get device info completed");
            }
        });
    }
}