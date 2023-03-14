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
import router from "@system.router";

export const backPage = function(superiorUri) {
  return {
    // back to main page
    backMain() {
      router.replace({ uri: "/pages/index/index" });
    },
    // back to superior page
    backSuperior() {
      router.replace({ uri: superiorUri });
    }
  };
};
export const imageFrames = [
  { src: "/common/A094_051.png" },
  { src: "/common/A094_052.png" },
  { src: "/common/A094_053.png" },
  { src: "/common/A094_054.png" },
  { src: "/common/A094_055.png" },
  { src: "/common/A094_056.png" },
  { src: "/common/A094_057.png" },
  { src: "/common/A094_058.png" },
  { src: "/common/A094_059.png" },
  { src: "/common/A094_060.png" },
  { src: "/common/A094_061.png" },
  { src: "/common/A094_062.png" },
  { src: "/common/A094_063.png" },
  { src: "/common/A094_064.png" },
  { src: "/common/A094_065.png" },
  { src: "/common/A094_066.png" },
  { src: "/common/A094_067.png" }
];
export const pageRoute = function(prevUri, nextUri) {
  return {
    changePrev() {
      router.replace({
        uri: prevUri
      });
    },
    backMain() {
      router.replace({
        uri: "pages/index/index"
      });
    },
    changeNext() {
      router.replace({
        uri: nextUri
      });
    }
  };
};
// route new page
export const routePage = function(uri) {
  return {
    changePage() {
      router.replace({ uri: uri });
    }
  };
};
// for back prev page
export const goPage = function(prevUri) {
  return {
    goHome() {
      router.replace({ uri: "pages/index/index" });
    },
    goPrevious() {
      router.replace({ uri: prevUri });
    }
  };
};
export const lifeCycleFunc = function() {
  return {
    onInit() {
      console.log("onInit called...");
    },
    onReady() {
      console.log("onReady called...");
    },
    onShow() {
      console.log("onShow called...");
    },
    onDestroy() {
      console.log("onDestroy called...");
    }
  };
};