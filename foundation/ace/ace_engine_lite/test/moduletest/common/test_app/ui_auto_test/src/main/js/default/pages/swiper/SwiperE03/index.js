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
        text2: "longpress",
        text3: "click"
    },
    textChange2() {
        this.text2 = "triggered";
    },
    textChange3() {
        this.text3 = "triggered";
    },
    showPage(e) {
        this.index = e.index;
    },
    ...routePage("pages/swiper/SwiperE02/index", "pages/swiper/SwiperS01/index")
}