/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
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
/******/ (function(modules) { // webpackBootstrap
/******/ 	// The module cache
/******/ 	var installedModules = {};
/******/
/******/ 	// The require function
/******/ 	function __webpack_require__(moduleId) {
/******/
/******/ 		// Check if module is in cache
/******/ 		if(installedModules[moduleId]) {
/******/ 			return installedModules[moduleId].exports;
/******/ 		}
/******/ 		// Create a new module (and put it into the cache)
/******/ 		var module = installedModules[moduleId] = {
/******/ 			i: moduleId,
/******/ 			l: false,
/******/ 			exports: {}
/******/ 		};
/******/
/******/ 		// Execute the module function
/******/ 		modules[moduleId].call(module.exports, module, module.exports, __webpack_require__);
/******/
/******/ 		// Flag the module as loaded
/******/ 		module.l = true;
/******/
/******/ 		// Return the exports of the module
/******/ 		return module.exports;
/******/ 	}
/******/
/******/
/******/ 	// expose the modules object (__webpack_modules__)
/******/ 	__webpack_require__.m = modules;
/******/
/******/ 	// expose the module cache
/******/ 	__webpack_require__.c = installedModules;
/******/
/******/ 	// define getter function for harmony exports
/******/ 	__webpack_require__.d = function(exports, name, getter) {
/******/ 		if(!__webpack_require__.o(exports, name)) {
/******/ 			Object.defineProperty(exports, name, { enumerable: true, get: getter });
/******/ 		}
/******/ 	};
/******/
/******/ 	// define __esModule on exports
/******/ 	__webpack_require__.r = function(exports) {
/******/ 		if(typeof Symbol !== 'undefined' && Symbol.toStringTag) {
/******/ 			Object.defineProperty(exports, Symbol.toStringTag, { value: 'Module' });
/******/ 		}
/******/ 		Object.defineProperty(exports, '__esModule', { value: true });
/******/ 	};
/******/
/******/ 	// create a fake namespace object
/******/ 	// mode & 1: value is a module id, require it
/******/ 	// mode & 2: merge all properties of value into the ns
/******/ 	// mode & 4: return value when already ns object
/******/ 	// mode & 8|1: behave like require
/******/ 	__webpack_require__.t = function(value, mode) {
/******/ 		if(mode & 1) value = __webpack_require__(value);
/******/ 		if(mode & 8) return value;
/******/ 		if((mode & 4) && typeof value === 'object' && value && value.__esModule) return value;
/******/ 		var ns = Object.create(null);
/******/ 		__webpack_require__.r(ns);
/******/ 		Object.defineProperty(ns, 'default', { enumerable: true, value: value });
/******/ 		if(mode & 2 && typeof value != 'string') for(var key in value) __webpack_require__.d(ns, key, function(key) { return value[key]; }.bind(null, key));
/******/ 		return ns;
/******/ 	};
/******/
/******/ 	// getDefaultExport function for compatibility with non-harmony modules
/******/ 	__webpack_require__.n = function(module) {
/******/ 		var getter = module && module.__esModule ?
/******/ 			function getDefault() { return module['default']; } :
/******/ 			function getModuleExports() { return module; };
/******/ 		__webpack_require__.d(getter, 'a', getter);
/******/ 		return getter;
/******/ 	};
/******/
/******/ 	// Object.prototype.hasOwnProperty.call
/******/ 	__webpack_require__.o = function(object, property) { return Object.prototype.hasOwnProperty.call(object, property); };
/******/
/******/ 	// __webpack_public_path__
/******/ 	__webpack_require__.p = "";
/******/
/******/
/******/ 	// Load entry module and return exports
/******/ 	return __webpack_require__(__webpack_require__.s = 3);
/******/ })
/************************************************************************/
/******/ ([
/* 0 */,
/* 1 */,
/* 2 */,
/* 3 */
/***/ (function(module, exports, __webpack_require__) {

var $app_template$ = __webpack_require__(4)
var $app_style$ = __webpack_require__(5)
var $app_script$ = __webpack_require__(6)
var options=$app_script$
 if ($app_script$.__esModule) {

      options = $app_script$.default;
 }
options.styleSheet=$app_style$
options.render=$app_template$;
module.exports=new ViewModel(options);

/***/ }),
/* 4 */
/***/ (function(module, exports) {

module.exports = function (vm) { var _vm = vm || this; return _c('swiper', {'attrs' : {'index' : function () {return _vm.swiperPage},'duration' : 500},'staticClass' : ["container"],'on' : {'change' : _vm.swiperChange}} , [_c('stack', {'staticClass' : ["container"]} , [_c('text', {'attrs' : {'value' : function () {return _vm.airData[0].airQuality}},'staticClass' : ["airquality"],'dynamicStyle' :{'color' : function () {return _vm.textColor1}}} ),_c('text', {'attrs' : {'value' : function () {return _vm.airData[0].location}},'staticClass' : ["location-text"]} ),_c('progress', {'attrs' : {'type' : "arc",'percent' : function () {return _vm.percent1}},'staticClass' : ["circleProgress"],'dynamicStyle' :{'color' : function () {return _vm.textColor1},'background-Color' : function () {return _vm.bgColor1}},'on' : {'click' : _vm.openDetail}} ),_c('image', {'attrs' : {'src' : function () {return _vm.src1}},'staticClass' : ["image"]} ),_c('text', {'attrs' : {'value' : function () {return _vm.airData[0].detailData}},'staticClass' : ["pm25-value"]} ),_c('text', {'attrs' : {'value' : "AQI"},'staticClass' : ["pm25-name"]} )] ),_c('stack', {'staticClass' : ["container"]} , [_c('text', {'attrs' : {'value' : function () {return _vm.airData[1].airQuality}},'staticClass' : ["airquality"],'dynamicStyle' :{'color' : function () {return _vm.textColor2}}} ),_c('text', {'attrs' : {'value' : function () {return _vm.airData[1].location}},'staticClass' : ["location-text"]} ),_c('progress', {'attrs' : {'type' : "arc",'percent' : function () {return _vm.percent2}},'staticClass' : ["circleProgress"],'dynamicStyle' :{'color' : function () {return _vm.textColor2},'background-Color' : function () {return _vm.bgColor2}},'on' : {'click' : _vm.openDetail}} ),_c('image', {'attrs' : {'src' : function () {return _vm.src2}},'staticClass' : ["image"]} ),_c('text', {'attrs' : {'value' : function () {return _vm.airData[1].detailData}},'staticClass' : ["pm25-value"]} ),_c('text', {'attrs' : {'value' : "AQI"},'staticClass' : ["pm25-name"]} )] )] ) }

/***/ }),
/* 5 */
/***/ (function(module, exports) {

module.exports = {"classSelectors":{"pm25-value":{"textAlign":"center","fontSize":38,"color":15794175,"width":454,"height":50,"top":275},"pm25-name":{"textAlign":"center","color":10667170,"width":454,"height":50,"top":335},"location-text":{"textAlign":"center","color":15794175,"width":454,"height":50,"top":20},"container":{"height":454,"width":454},"circleProgress":{"centerX":227,"centerY":250,"radius":180,"startAngle":198,"totalAngle":320,"strokeWidth":45,"width":454,"height":454},"image":{"top":390,"left":217,"width":32,"height":32},"airquality":{"top":220,"textAlign":"center","width":454,"height":40}}}

/***/ }),
/* 6 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";


var _interopRequireDefault = __webpack_require__(7);

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports["default"] = void 0;

var _system = _interopRequireDefault(requireNative("system.router"));

var _default = {
  data: {
    textColor1: "#00ff00",
    textColor2: "#00ff00",
    bgColor1: "#669966",
    bgColor2: "#669966",
    swiperPage: 0,
    percent1: 0,
    percent2: 0,
    src1: "common/cloud_green.png",
    src2: "common/cloud_green.png",
    airData: [{
      location: "HangZhou",
      airQuality: "Good",
      detailData: 10
    }, {
      location: "ShangHai",
      airQuality: "Unhealth",
      detailData: 90
    }]
  },
  onInit: function onInit() {
    this.airData[0].location = this.$t(this.airData[0].location);
    this.airData[1].location = this.$t(this.airData[1].location);
    this.airData[0].airQuality = this.$t(this.airData[0].airQuality);
    this.airData[1].airQuality = this.$t(this.airData[1].airQuality);

    if (this.airData[0].detailData > 100) {
      this.src1 = "common/cloud_red.png";
      this.textColor1 = "#ff0000";
      this.bgColor1 = "#9d7462";
    } else if (50 < this.airData[0].detailData && this.airData[0].detailData <= 100) {
      this.src1 = "common/cloud_yellow.png";
      this.textColor1 = "#ecf19a";
      this.bgColor1 = "#9d9d62";
    }

    if (this.airData[1].detailData > 100) {
      this.src2 = "common/cloud_red.png";
      this.textColor2 = "#ff0000";
      this.bgColor2 = "#9d7462";
    } else if (50 < this.airData[1].detailData && this.airData[1].detailData <= 100) {
      this.src2 = "common/cloud_yellow.png";
      this.textColor2 = "#ecf19a";
      this.bgColor2 = "#9d9d62";
    }

    if (this.selectedCityIndex) {
      this.swiperPage = this.selectedCityIndex;
    }
  },
  onShow: function onShow() {
    var self = this;
    var time = 1000 / self.airData[self.swiperPage].detailData;

    if (time == 0) {
      time = 100;
    }

    var interval = setInterval(function () {
      if ((self.swiperPage == 0 ? self.percent1 : self.percent2) >= self.airData[self.swiperPage].detailData) {
        clearInterval(interval);
        return;
      }

      self.swiperPage == 0 ? self.percent1++ : self.percent2++;
    }, time);
  },
  openDetail: function openDetail() {
    _system["default"].replace({
      uri: "pages/detail/detail",
      params: {
        selectedCityIndex: this.swiperPage
      }
    });
  },
  swiperChange: function swiperChange(e) {
    this.swiperPage = e.index;
    var self = this;
    var time = 1000 / self.airData[self.swiperPage].detailData;

    if (time == 0) {
      time = 100;
    }

    var interval = setInterval(function () {
      if ((self.swiperPage == 0 ? self.percent1 : self.percent2) >= self.airData[self.swiperPage].detailData) {
        clearInterval(interval);
        return;
      }

      self.swiperPage == 0 ? self.percent1++ : self.percent2++;
    }, time);
  }
};
exports["default"] = _default;

/***/ }),
/* 7 */
/***/ (function(module, exports) {

function _interopRequireDefault(obj) {
  return obj && obj.__esModule ? obj : {
    "default": obj
  };
}

module.exports = _interopRequireDefault;

/***/ })
/******/ ]);
//# sourceMappingURL=index.js.map