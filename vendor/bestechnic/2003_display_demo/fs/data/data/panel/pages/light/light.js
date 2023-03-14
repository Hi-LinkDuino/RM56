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

module.exports = function (vm) { var _vm = vm || this; return _c('div', {'staticClass' : ["container"]} , [_c('div', {'staticClass' : ["header"],'on' : {'click' : _vm.back}} , [_c('text', {'attrs' : {'value' : function () {return _vm.title}},'staticClass' : ["title"]} )] ),_c('div', {'staticClass' : ["body"]} , [_c('div', {'staticClass' : ["grid"]} , [_c('div', {'staticClass' : ["row2"]} , [_c('div', {'attrs' : {'value' : ">"},'staticClass' : ["col","light-wrapper"],'on' : {'click' : _vm.Onlight0Changed}} , [_i((function () {return _vm.light0_on}),function(){return _c('image', {'attrs' : {'src' : function () {return _vm.light_image}},'staticClass' : ["light-on-image"]} )}),_i((function () {return !_vm.light0_on}),function(){return _c('image', {'attrs' : {'src' : function () {return _vm.light_image}},'staticClass' : ["light-off-image"]} )}),_c('text', {'attrs' : {'value' : "客厅灯"}} )] ),_c('div', {'staticClass' : ["col","light-wrapper"],'on' : {'click' : _vm.Onlight1Changed}} , [_i((function () {return _vm.light1_on}),function(){return _c('image', {'attrs' : {'src' : function () {return _vm.light_image}},'staticClass' : ["light-on-image"]} )}),_i((function () {return !_vm.light1_on}),function(){return _c('image', {'attrs' : {'src' : function () {return _vm.light_image}},'staticClass' : ["light-off-image"]} )}),_c('text', {'attrs' : {'value' : "厨房灯"}} )] )] ),_c('div', {'staticClass' : ["row2"]} , [_c('div', {'staticClass' : ["col","light-wrapper"],'on' : {'click' : _vm.Onlight2Changed}} , [_i((function () {return _vm.light2_on}),function(){return _c('image', {'attrs' : {'src' : function () {return _vm.light_image}},'staticClass' : ["light-on-image"]} )}),_i((function () {return !_vm.light2_on}),function(){return _c('image', {'attrs' : {'src' : function () {return _vm.light_image}},'staticClass' : ["light-off-image"]} )}),_c('text', {'attrs' : {'value' : "主卧灯"}} )] ),_c('div', {'staticClass' : ["col","light-wrapper"],'on' : {'click' : _vm.Onlight3Changed}} , [_i((function () {return _vm.light3_on}),function(){return _c('image', {'attrs' : {'src' : function () {return _vm.light_image}},'staticClass' : ["light-on-image"]} )}),_i((function () {return !_vm.light3_on}),function(){return _c('image', {'attrs' : {'src' : function () {return _vm.light_image}},'staticClass' : ["light-off-image"]} )}),_c('text', {'attrs' : {'value' : "次卧灯"}} )] )] )] )] )] ) }

/***/ }),
/* 10 */
/***/ (function(module, exports) {

module.exports = {"classSelectors":{"container":{"flexDirection":"column","justifyContent":"center","alignItems":"center","left":0,"top":0,"width":454,"height":454,"marginTop":5,"marginRight":5,"marginBottom":5,"marginLeft":5},"header":{"flexDirection":"row","justifyContent":"center","alignItems":"center","width":"100%","height":"10%"},"body":{"width":"100%","height":"80%"},"footer":{"flexDirection":"row","justifyContent":"center","alignItems":"center","width":"100%","height":"10%"},"grid":{"flexDirection":"row","flexWrap":"wrap","width":"100%","height":"100%"},"row":{"width":"100%","flexDirection":"row"},"col":{"height":"100%","flexDirection":"column"},"row2":{"width":"100%","height":"50%"},"row3":{"width":"100%","height":"33.3%"},"row4":{"width":"100%","height":"25%"},"col2":{"width":"50%","height":"100%"},"col3":{"width":"33%","height":"100%"},"col4":{"width":"25%","height":"100%"},"light-wrapper":{"width":"50%","justifyContent":"center","alignItems":"center"},"light-on-image":{"width":64,"height":64,"backgroundColor":16766720},"light-off-image":{"width":64,"height":64}}}

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
    title: '< 灯光控制',
    light_image: "/common/icons8-light-64.png",
    light0_on: false,
    light1_on: false,
    light2_on: false,
    light3_on: false
  },
  back: function back() {
    _system["default"].replace({
      uri: "pages/index/index"
    });
  },
  Onlight0Changed: function Onlight0Changed() {
    this.light0_on = !this.light0_on;
  },
  Onlight1Changed: function Onlight1Changed() {
    this.light1_on = !this.light1_on;
  },
  Onlight2Changed: function Onlight2Changed() {
    this.light2_on = !this.light2_on;
  },
  Onlight3Changed: function Onlight3Changed() {
    this.light3_on = !this.light3_on;
  }
};
exports["default"] = _default;

/***/ })
/******/ ]);