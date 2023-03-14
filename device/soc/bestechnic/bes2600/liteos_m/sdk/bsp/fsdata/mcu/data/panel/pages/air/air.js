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

module.exports = function (vm) { var _vm = vm || this; return _c('div', {'staticClass' : ["container"]} , [_c('div', {'staticClass' : ["header"],'on' : {'click' : _vm.back}} , [_c('text', {'attrs' : {'value' : function () {return _vm.title}},'staticClass' : ["title"]} )] ),_c('div', {'staticClass' : ["body"],'staticStyle' : {'flexDirection' : "column"}, } , [_c('div', {'staticClass' : ["row"],'staticStyle' : {'height' : "50%"}, } , [_c('progress', {'attrs' : {'type' : "arc",'percent' : function () {return _vm.temp_percent}},'staticClass' : ["circleProgress"]} )] ),_c('div', {'staticClass' : ["row"],'staticStyle' : {'height' : "10%"}, } , [_c('text', {'attrs' : {'value' : "16°C"},'staticClass' : ["col3"],'staticStyle' : {'width' : "30%"}, } ),_c('text', {'attrs' : {'value' : function () {return '当前' + (_vm.cur_temp) + '°C'}},'staticClass' : ["col3"],'staticStyle' : {'width' : "40%",'textAlign' : "center"}, } ),_c('text', {'attrs' : {'value' : "30°C"},'staticClass' : ["col3"],'staticStyle' : {'width' : "30%",'textAlign' : "right"}, } )] ),_c('div', {'staticClass' : ["row"],'staticStyle' : {'height' : "10%",'marginTop' : 10}, } , [_c('text', {'attrs' : {'value' : "设置温度:"},'staticClass' : ["col2"],'staticStyle' : {'height' : 32}, } ),_c('slider', {'attrs' : {'min' : 16,'max' : 30,'value' : function () {return _vm.cur_temp}},'staticClass' : ["col2","slider"],'on' : {'change' : _vm.changeTemp}} )] ),_c('div', {'staticClass' : ["row"],'staticStyle' : {'height' : "10%",'marginTop' : 10}, } , [_c('text', {'attrs' : {'value' : "工作模式:"},'staticClass' : ["col2"]} ),_c('picker-view', {'attrs' : {'type' : "text",'range' : function () {return _vm.textContent},'selected' : 1},'staticClass' : ["col2","picker-view"],'on' : {'change' : _vm.changeMode}} )] )] )] ) }

/***/ }),
/* 14 */
/***/ (function(module, exports) {

module.exports = {"classSelectors":{"container":{"flexDirection":"column","justifyContent":"center","alignItems":"center","left":0,"top":0,"width":454,"height":454,"marginTop":5,"marginRight":5,"marginBottom":5,"marginLeft":5},"header":{"flexDirection":"row","justifyContent":"center","alignItems":"center","width":"100%","height":"10%"},"body":{"width":"100%","height":"80%"},"footer":{"flexDirection":"row","justifyContent":"center","alignItems":"center","width":"100%","height":"10%"},"grid":{"flexDirection":"row","flexWrap":"wrap","width":"100%","height":"100%"},"row":{"width":"100%","flexDirection":"row"},"col":{"height":"100%","flexDirection":"column"},"row2":{"width":"100%","height":"50%"},"row3":{"width":"100%","height":"33.3%"},"row4":{"width":"100%","height":"25%"},"col2":{"width":"50%","height":"100%"},"col3":{"width":"33%","height":"100%"},"col4":{"width":"25%","height":"100%"},"slider":{"color":8421504,"selectedColor":255},"picker-view":{"marginLeft":-70,"fontSize":30,"selectedFontSize":30},"circleProgress":{"centerX":225,"centerY":225,"radius":200,"startAngle":300,"totalAngle":120,"strokeWidth":40,"width":"100%","height":"100%","color":65280,"background-Color":6723942}}}

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
    title: '< 空调控制',
    temp_percent: 36,
    cur_temp: '21',
    cur_mode: '制冷',
    textContent: ["制热", "制冷", "出风", "除霜"]
  },
  back: function back() {
    _system["default"].replace({
      uri: "pages/index/index"
    });
  },
  changeTemp: function changeTemp(e) {
    console.log("temp change to " + e.progress);
    this.cur_temp = e.progress;
    this.temp_percent = 100 * (this.cur_temp - 16) / (30 - 16);
  },
  changeMode: function changeMode(e) {
    console.log("mode change to " + e.newValue);
  }
};
exports["default"] = _default;

/***/ })
/******/ ]);