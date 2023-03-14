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
import {routePage} from "../../../common/js/general"
export default {
    data: {
        index: 0,
        vert: true
    },
    showPage(e) {
        this.index = e.index;
    },
    updateVert() {
        this.vert = false;
        console.log(this.vert);
    },
    ...routePage("pages/swiper/SwiperA06/index", "pages/swiper/SwiperA16/index")
}