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
/******/ 	return __webpack_require__(__webpack_require__.s = 8);
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
/* 8 */
/***/ (function(module, exports, __webpack_require__) {

var $app_template$ = __webpack_require__(9)
var $app_style$ = __webpack_require__(10)
var $app_script$ = __webpack_require__(11)
var options=$app_script$
 if ($app_script$.__esModule) {

      options = $app_script$.default;
 }
options.styleSheet=$app_style$
options.render=$app_template$;
module.exports=new ViewModel(options);

/***/ }),
/* 9 */
/***/ (function(module, exports) {

module.exports = function (vm) { var _vm = vm || this; return _c('list', {'staticStyle' : {'top' : 67,'left' : 67,'width' : 321,'height' : 350}, } , [_c('list-item', {'staticStyle' : {'width' : 321,'height' : 300}, } , [_c('div', {'staticStyle' : {'width' : 321,'height' : 300,'backgroundColor' : 0,'flexDirection' : "column",'alignItems' : "flex-start"}, } , [_c('text', {'attrs' : {'value' : function () {return _vm.location}},'staticStyle' : {'width' : 321,'height' : 40,'color' : 14474460}, } ),_c('div', {'staticClass' : ["line-div"]} ),_c('div', {'staticClass' : ["info-div-width-height"],'staticStyle' : {'marginTop' : 10}, } , [_c('text', {'attrs' : {'value' : "CO"},'staticClass' : ["gas-name"]} ),_c('text', {'attrs' : {'value' : function () {return _vm.airData[0]}},'staticClass' : ["gas-value"]} )] ),_c('div', {'staticClass' : ["info-div-width-height"],'staticStyle' : {'marginTop' : 10}, } , [_c('text', {'attrs' : {'value' : "NO2"},'staticClass' : ["gas-name"]} ),_c('text', {'attrs' : {'value' : function () {return _vm.airData[1]}},'staticClass' : ["gas-value"]} )] ),_c('div', {'staticClass' : ["info-div-width-height"],'staticStyle' : {'marginTop' : 10}, } , [_c('text', {'attrs' : {'value' : "PM10"},'staticClass' : ["gas-name"]} ),_c('text', {'attrs' : {'value' : function () {return _vm.airData[2]}},'staticClass' : ["gas-value"]} )] ),_c('div', {'staticClass' : ["info-div-width-height"],'staticStyle' : {'marginTop' : 10}, } , [_c('text', {'attrs' : {'value' : "PM2.5"},'staticClass' : ["gas-name"]} ),_c('text', {'attrs' : {'value' : function () {return _vm.airData[3]}},'staticClass' : ["gas-value"]} )] ),_c('div', {'staticClass' : ["info-div-width-height"],'staticStyle' : {'marginTop' : 10}, } , [_c('text', {'attrs' : {'value' : "SO2"},'staticClass' : ["gas-name"]} ),_c('text', {'attrs' : {'value' : function () {return _vm.airData[4]}},'staticClass' : ["gas-value"]} )] )] )] ),_c('list-item', {'staticStyle' : {'width' : 321,'height' : 220}, } , [_c('div', {'staticStyle' : {'width' : 321,'height' : 220,'backgroundColor' : 0,'flexDirection' : "column",'alignItems' : "flex-start"}, } , [_c('text', {'attrs' : {'value' : function () {return (_vm.updated) + ':' + (_vm.updateStatus)}},'staticClass' : ["config-info"]} ),_c('text', {'attrs' : {'value' : function () {return (_vm.dataSourceName) + ':' + (_vm.dataSource)}},'staticClass' : ["config-info"]} ),_c('div', {'staticClass' : ["line-div"]} ),_c('div', {'staticStyle' : {'width' : 321,'height' : 55,'alignItems' : "center",'marginTop' : 20}, } , [_c('input', {'attrs' : {'type' : "button",'value' : "History"},'staticStyle' : {'borderTopWidth' : 3,'borderRightWidth' : 3,'borderBottomWidth' : 3,'borderLeftWidth' : 3,'marginLeft' : 10,'borderTopColor' : 9498256,'borderRightColor' : 9498256,'borderBottomColor' : 9498256,'borderLeftColor' : 9498256,'width' : 146,'height' : 50}, 'on' : {'click' : _vm.openHistory}} ),_c('input', {'attrs' : {'type' : "button",'value' : "Close"},'staticStyle' : {'borderTopWidth' : 3,'borderRightWidth' : 3,'borderBottomWidth' : 3,'borderLeftWidth' : 3,'marginLeft' : 5,'borderTopColor' : 16711680,'borderRightColor' : 16711680,'borderBottomColor' : 16711680,'borderLeftColor' : 16711680,'width' : 146,'height' : 50}, 'on' : {'click' : _vm.backMain}} )] )] )] )] ) }

/***/ }),
/* 10 */
/***/ (function(module, exports) {

module.exports = {"classSelectors":{"line-div":{"backgroundColor":16121850,"height":2,"width":454},"info-div-width-height":{"width":321,"height":35,"marginTop":20},"gas-name":{"color":16121850,"width":160,"height":35},"gas-value":{"textAlign":"right","color":64154,"width":160,"height":35},"config-info":{"height":40,"width":321,"color":16121850}}}

/***/ }),
/* 11 */
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
    location: "HangZhou",
    updateStatus: "15:13",
    dataSource: "tianqi.com",
    updated: "Updated",
    dataSourceName: "Data Source",
    selectedIndex: 0,
    airData: ["100", "90", "120", "50", "150", "40", "25"]
  },
  onInit: function onInit() {
    this.location = this.$t(this.location);
    this.updated = this.$t("updated");
    this.dataSourceName = this.$t("dataSourceName");
    this.sampledOn = this.$t("sampledOn");
    this.monitoringStation = this.$t("monitoringStation");
    this.selectedIndex = this.selectedCityIndex;
  },
  openHistory: function openHistory() {
    _system["default"].replace({
      uri: "pages/history/history"
    });
  },
  backMain: function backMain() {
    _system["default"].replace({
      uri: "pages/index/index",
      params: {
        selectedCityIndex: this.selectedIndex
      }
    });
  }
};
exports["default"] = _default;

/***/ })
/******/ ]);
//# sourceMappingURL=detail.js.map