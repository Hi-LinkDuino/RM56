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
import {backPage, routePage} from "../../../common/js/general";

export default {
    goHome:routePage("/").changePage,
    goNext:routePage("pages/imageView/fitOriginalSize/index").changePage,

    data: {
        title: 'World',
        fitobject: 'fill',
        imageWith : '200px',
        imageHeight : "400px",
        fitOriginal : true
    },
    changeToCover() {
        this.fitobject = "cover";
        this.imageWith = "200px";
        this.imageHeight = "400px";
    },
    changeToContain() {
        this.fitobject = "contain";
        this.imageWith = "200px";
        this.imageHeight = "400px";
    },
    changeToFill() {
        this.fitobject = "fill";
        this.imageWith = "200px";
        this.imageHeight = "400px";
    },
    changeToNone() {
        this.fitobject = "none";
        this.imageWith = "200px";
        this.imageHeight = "400px";
    },
    changeToScaleDown() {
        this.fitobject = "scale-down";
        this.imageWith = "200px";
        this.imageHeight = "400px";
    },
    changeToFitOriginal() {
        this.fitOriginal = "true";
        this.imageHeight = 0;
        this.imageWith = 0;
    },
}
