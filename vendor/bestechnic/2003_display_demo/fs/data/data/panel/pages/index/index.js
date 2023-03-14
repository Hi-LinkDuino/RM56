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

module.exports = function (vm) { var _vm = vm || this; return _c('div', {'staticClass' : ["container"]} , [_c('div', {'staticClass' : ["header"]} , [_c('text', {'attrs' : {'value' : function () {return (_vm.time) + ' ' + (_vm.date) + ' ' + (_vm.week)}}} )] ),_c('div', {'staticClass' : ["body"]} , [_c('swiper', {'attrs' : {'index' : function () {return _vm.index}},'staticClass' : ["swiper"]} , [_c('div', {'staticClass' : ["swiper-item"]} , [_c('div', {'staticClass' : ["grid"]} , [_c('div', {'staticClass' : ["row2"]} , [_c('div', {'staticClass' : ["col","block-wrapper"],'on' : {'click' : _vm.OnScene0Clicked}} , [_c('image', {'attrs' : {'src' : function () {return _vm.image0}},'staticClass' : ["image"]} ),_c('text', {'attrs' : {'value' : "日常"}} )] ),_c('div', {'staticClass' : ["col","block-wrapper"],'on' : {'click' : _vm.OnScene1Clicked}} , [_c('image', {'attrs' : {'src' : function () {return _vm.image1}},'staticClass' : ["image"]} ),_c('text', {'attrs' : {'value' : "周末"}} )] )] ),_c('div', {'staticClass' : ["row2"]} , [_c('div', {'staticClass' : ["col","block-wrapper"],'on' : {'click' : _vm.OnScene2Clicked}} , [_c('image', {'attrs' : {'src' : function () {return _vm.image2}},'staticClass' : ["image"]} ),_c('text', {'attrs' : {'value' : "聚会"}} )] ),_c('div', {'staticClass' : ["col","block-wrapper"],'on' : {'click' : _vm.OnScene3Clicked}} , [_c('image', {'attrs' : {'src' : function () {return _vm.image3}},'staticClass' : ["image"]} ),_c('text', {'attrs' : {'value' : "烛光"}} )] )] )] )] ),_c('div', {'staticClass' : ["swiper-item"]} , [_c('div', {'staticClass' : ["grid"]} , [_c('div', {'staticClass' : ["row2"]} , [_c('div', {'staticClass' : ["col","block-wrapper"],'on' : {'click' : _vm.OnDev0Clicked}} , [_c('image', {'attrs' : {'src' : function () {return _vm.image4}},'staticClass' : ["image"]} ),_c('text', {'attrs' : {'value' : "空调"}} )] ),_c('div', {'staticClass' : ["col","block-wrapper"],'on' : {'click' : _vm.OnDev1Clicked}} , [_c('image', {'attrs' : {'src' : function () {return _vm.image5}},'staticClass' : ["image"]} ),_c('text', {'attrs' : {'value' : "冰箱"}} )] )] ),_c('div', {'staticClass' : ["row2"]} , [_c('div', {'staticClass' : ["col","block-wrapper"],'on' : {'click' : _vm.OnDev2Clicked}} , [_c('image', {'attrs' : {'src' : function () {return _vm.image6}},'staticClass' : ["image"]} ),_c('text', {'attrs' : {'value' : "窗帘"}} )] ),_c('div', {'staticClass' : ["col","block-wrapper"],'on' : {'click' : _vm.OnDev3Clicked}} , [_c('image', {'attrs' : {'src' : function () {return _vm.image7}},'staticClass' : ["image"]} ),_c('text', {'attrs' : {'value' : "灯光"}} )] )] )] )] )] )] ),_c('div', {'staticClass' : ["footer"]} , [_c('text', {'attrs' : {'value' : "。。"}} )] )] ) }

/***/ }),
/* 5 */
/***/ (function(module, exports) {

module.exports = {"classSelectors":{"container":{"flexDirection":"column","justifyContent":"center","alignItems":"center","left":0,"top":0,"width":454,"height":454,"marginTop":5,"marginRight":5,"marginBottom":5,"marginLeft":5},"header":{"flexDirection":"row","justifyContent":"center","alignItems":"center","width":"100%","height":"10%"},"body":{"width":"100%","height":"80%"},"footer":{"flexDirection":"row","justifyContent":"center","alignItems":"center","width":"100%","height":"10%"},"grid":{"flexDirection":"row","flexWrap":"wrap","width":"100%","height":"100%"},"row":{"width":"100%","flexDirection":"row"},"col":{"height":"100%","flexDirection":"column"},"row2":{"width":"100%","height":"50%"},"row3":{"width":"100%","height":"33.3%"},"row4":{"width":"100%","height":"25%"},"col2":{"width":"50%","height":"100%"},"col3":{"width":"33%","height":"100%"},"col4":{"width":"25%","height":"100%"},"swiper":{"width":"100%","height":"100%"},"swiper-item":{"width":"100%","height":"100%"},"block-wrapper":{"width":"50%","justifyContent":"center","alignItems":"center"},"image":{"width":64,"height":64}}}

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

var _system2 = _interopRequireDefault(requireNative("system.device"));

var _system3 = _interopRequireDefault(requireNative("system.storage"));

var _system4 = _interopRequireDefault(requireNative("system.file"));

var _default = {
  data: {
    index: 0,
    time: "",
    date: "",
    image0: "/common/icons8-workday-64.png",
    image1: "/common/icons8-sun-64.png",
    image2: "/common/icons8-myspace-64.png",
    image3: "/common/icons8-candle-64.png",
    image4: "/common/icons8-air-conditioner-64.png",
    image5: "/common/icons8-fridge-64.png",
    image6: "/common/icons8-curtains-64.png",
    image7: "/common/icons8-light-64.png"
  },
  onInit: function onInit() {
    var weeks = ["星期日", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六"];
    var date = new Date();
    this.time = date.getHours() + ":" + date.getMinutes() + ":" + date.getSeconds();
    this.date = date.toDateString();
    this.week = weeks[date.getDay()];
  },
  OnScene0Clicked: function OnScene0Clicked() {
    console.log("scene0 clicked");
    this.GetDeviceInfo();
  },
  OnScene1Clicked: function OnScene1Clicked() {
    console.log("scene1 clicked");
    this.SetKey();
  },
  OnScene2Clicked: function OnScene2Clicked() {
    console.log("scene2 clicked");
    this.GetKey();
  },
  OnScene3Clicked: function OnScene3Clicked() {
    console.log("scene3 clicked");
    this.GetFileList();
  },
  OnDev0Clicked: function OnDev0Clicked() {
    console.log("dev0 clicked");

    _system["default"].replace({
      uri: 'pages/air/air'
    });
  },
  OnDev1Clicked: function OnDev1Clicked() {
    console.log("dev1 clicked");
  },
  OnDev2Clicked: function OnDev2Clicked() {
    console.log("dev2 clicked");
  },
  OnDev3Clicked: function OnDev3Clicked() {
    console.log("dev3 clicked");

    _system["default"].replace({
      uri: 'pages/light/light'
    });
  },
  GetDeviceInfo: function GetDeviceInfo() {
    _system2["default"].getInfo({
      success: function success(data) {
        console.log('Device information obtained successfully.');
        console.log('brand:' + data.brand);
        console.log('manufacturer:' + data.manufacturer);
        console.log('model:' + data.model);
        console.log('product:' + data.product);
        console.log('language:' + data.language);
        console.log('region:' + data.region);
        console.log('windowWidth:' + data.windowWidth);
        console.log('windowHeight:' + data.windowHeight);
        console.log('screenDensity:' + data.screenDensity);
        console.log('screenShape:' + data.screenShape);
        console.log('apiVersion:' + data.apiVersion);
        console.log('deviceType:' + data.deviceType);
      },
      fail: function fail(data, code) {
        console.log('Failed to obtain device information. Error code:' + code + '; Error information: ' + data);
      }
    });
  },
  SetKey: function SetKey() {
    _system3["default"].set({
      key: 'storage_key',
      value: 'storage value',
      success: function success() {
        console.log('call storage.set success.');
      },
      fail: function fail(data, code) {
        console.log('call storage.set fail, code: ' + code + ', data: ' + data);
      }
    });
  },
  GetKey: function GetKey() {
    _system3["default"].get({
      key: 'storage_key',
      success: function success(data) {
        console.log('call storage.get success: ' + data);
      },
      fail: function fail(data, code) {
        console.log('call storage.get fail, code: ' + code + ', data: ' + data);
      },
      complete: function complete() {
        console.log('call complete');
      }
    });
  },
  GetFileList: function GetFileList() {
    _system4["default"].list({
      uri: 'internal://app/img',
      success: function success(data) {
        for (var i = 0; i < data.fileList.length; i++) {
          console.log(i + ": {");
          console.log("uri: " + data.fileList[i].uri);
          console.log("lastModifiedTime: " + data.fileList[i].lastModifiedTime);
          console.log("length: " + data.fileList[i].length);
          console.log("type: " + data.fileList[i].type);
          console.log("}");
        }
      },
      fail: function fail(data, code) {
        console.error('call fail callback fail, code: ' + code + ', data: ' + data);
      }
    });
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