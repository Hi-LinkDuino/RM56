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
/******/ 	return __webpack_require__(__webpack_require__.s = 12);
/******/ })
/************************************************************************/
/******/ ([
/* 0 */,
/* 1 */,
/* 2 */,
/* 3 */,
/* 4 */,
/* 5 */,
/* 6 */,
/* 7 */
/***/ (function(module, exports) {

function _interopRequireDefault(obj) {
  return obj && obj.__esModule ? obj : {
    "default": obj
  };
}

module.exports = _interopRequireDefault;

/***/ }),
/* 8 */,
/* 9 */,
/* 10 */,
/* 11 */,
/* 12 */
/***/ (function(module, exports, __webpack_require__) {

var $app_template$ = __webpack_require__(13)
var $app_style$ = __webpack_require__(14)
var $app_script$ = __webpack_require__(15)
var options=$app_script$
 if ($app_script$.__esModule) {

      options = $app_script$.default;
 }
options.styleSheet=$app_style$
options.render=$app_template$;
module.exports=new ViewModel(options);

/***/ }),
/* 13 */
/***/ (function(module, exports) {

module.exports = function (vm) { var _vm = vm || this; return _c('list', {'staticStyle' : {'width' : 321,'height' : 321,'top' : 67,'left' : 67}, } , [_c('list-item', {'staticClass' : ["info-list-item"]} , [_c('div', {'staticStyle' : {'width' : 321,'height' : 80,'flexDirection' : "column",'alignItems' : "flex-start"}, } , [_c('text', {'attrs' : {'value' : function () {return _vm.historicalData}},'staticClass' : ["div-info-historical-data"]} )] )] ),_l((function () {return _vm.datasets}),function($item,$idx){return _c('list-item', {'staticStyle' : {'width' : 321,'height' : 160}, } , [_c('div', {'staticStyle' : {'width' : 321,'height' : 160,'flexDirection' : "column"}, } , [_c('div', {'staticStyle' : {'width' : 321,'height' : 2,'backgroundColor' : 16121850}, } ),_c('text', {'attrs' : {'value' : function () {return $item}},'staticClass' : ["gas-name"]} ),_c('div', {'staticStyle' : {'width' : 321,'height' : 100,'marginTop' : 4,'justifyContent' : "flex-start",'alignItems' : "flex-end"}, } , [_c('div', {'staticStyle' : {'width' : 21,'marginLeft' : 21,'height' : 10,'backgroundColor' : 65280}, } ),_c('div', {'staticStyle' : {'width' : 21,'marginLeft' : 21,'height' : 20,'backgroundColor' : 65280}, } ),_c('div', {'staticStyle' : {'width' : 21,'marginLeft' : 21,'height' : 90,'backgroundColor' : 16711680}, } ),_c('div', {'staticStyle' : {'width' : 21,'marginLeft' : 21,'height' : 80,'backgroundColor' : 16711680}, } ),_c('div', {'staticStyle' : {'width' : 21,'marginLeft' : 21,'height' : 60,'backgroundColor' : 10066329}, } ),_c('div', {'staticStyle' : {'width' : 21,'marginLeft' : 21,'height' : 50,'backgroundColor' : 10066329}, } ),_c('div', {'staticStyle' : {'width' : 21,'marginLeft' : 21,'height' : 100,'backgroundColor' : 16711680}, } )] ),_c('image', {'attrs' : {'src' : "common/week.png"},'staticStyle' : {'width' : 321,'height' : 20}, } )] )] )}),_c('list-item', {'staticClass' : ["info-list-item"]} , [_c('input', {'attrs' : {'type' : "button",'value' : "Back"},'staticStyle' : {'borderTopWidth' : 2,'borderRightWidth' : 2,'borderBottomWidth' : 2,'borderLeftWidth' : 2,'borderTopColor' : 9498256,'borderRightColor' : 9498256,'borderBottomColor' : 9498256,'borderLeftColor' : 9498256,'width' : 256,'height' : 60,'marginLeft' : 30,'marginTop' : 10}, 'on' : {'click' : _vm.backDetail}} )] )] ) }

/***/ }),
/* 14 */
/***/ (function(module, exports) {

module.exports = {"classSelectors":{"div-info-location":{"color":14474460,"width":321,"height":40},"div-info-historical-data":{"color":16121850,"width":321,"height":40},"gas-name":{"color":15794175,"textAlign":"right","width":321,"height":35},"info-list-item":{"width":321,"height":80}}}

/***/ }),
/* 15 */
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
    historicalData: "historicalData",
    datasets: ["CO", "PM2.5", "NO2", "NO", "PM10", "SO2"]
  },
  onInit: function onInit() {
    this.historicalData = this.$t(this.historicalData);
  },
  backDetail: function backDetail() {
    _system["default"].replace({
      uri: "pages/detail/detail"
    });
  }
};
exports["default"] = _default;

/***/ })
/******/ ]);
//# sourceMappingURL=history.js.map