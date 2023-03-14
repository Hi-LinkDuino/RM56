/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

// AUTO GENERATED, PLEASE DO NOT EDIT DIRECTLY

#ifndef ACELITE_FRAMEWORK_RAW_BUFFER
#error THIS FILE CAN ONLY BE INCLUDED BY RAW BUFFER CPP
#endif

#ifndef OHOS_ACELITE_FRAMEWORK_MIN_JS_BUFFER
#define OHOS_ACELITE_FRAMEWORK_MIN_JS_BUFFER
const char * const g_frameworkJSBuffer =
    "!function(t,e){'object'==typeof exports&&'undefined'!=typeof module?module.exports=e():'fun"
    "ction'==typeof define&&define.amd?define(e):(t=t||self).ViewModel=e()}(this,(function(){'u"
    "se strict';var t={stack:[],push:function(t){this.stack.push(t)},pop:function(){return this"
    ".stack.pop()},top:function(){return this.stack[this.stack.length-1]}},e=function(t){return"
    "'object'==typeof t&&null!==t},n=function(t,e,n){Object.defineProperty(t,e,{enumerable:!1,v"
    "alue:n})};function i(t,e,n,i){this._ctx=t,this._getter=e,this._fn=n,this._meta=i,this._las"
    "tValue=this._get()}function o(e){this._hijacking=!0,n(e,'__ob__',this),Array.isArray(e)&&f"
    "unction(t){s.forEach((function(e){var i=t[e];n(t,e,(function(){var n,s=Array.prototype.sli"
    "ce.call(arguments);i.apply(this,s),r.PUSH===e||r.UNSHIFT===e?n=s:r.SPLICE===e&&(n=s.slice("
    "2)),n&&n.length&&n.forEach(o.of);var c=t.__ob__;c&&c.notifyParent()}))}))}(e),Object.keys("
    "e).forEach((function(n){return function(e,n,i){var r=e.__ob__;Object.defineProperty(e,n,{e"
    "numerable:!0,get:function(){var e=t.top();e&&e.subscribe(r,n);var s=o.of(i);return o.is(s)"
    "&&s.setParent(r,n),i},set:function(t){i=t,r.notify(n)}})}(e,n,e[n])}))}i.prototype._get=fu"
    "nction(){try{return t.push(this),this._getter.call(this._ctx)}finally{t.pop()}},i.prototyp"
    "e.update=function(){var t=this._lastValue,n=this._get(),i=this._ctx,o=this._meta;(n!==t||e"
    "(n))&&(this._fn.call(i,n,t,o),this._lastValue=n)},i.prototype.subscribe=function(t,e){var "
    "n=t.attach(e,this);'function'==typeof n&&(this._detaches||(this._detaches=[]),this._detach"
    "es.push(n))},i.prototype.unsubscribe=function(){var t=this._detaches;if(t)for(;t.length;)t"
    ".pop()()},o.of=function(t){return t&&e(t)?t.__ob__?t.__ob__:new o(t):t},o.is=function(t){r"
    "eturn t&&t._hijacking},o.prototype.attach=function(t,e){if(void 0!==t&&e){this._obsMap||(t"
    "his._obsMap={}),this._obsMap[t]||(this._obsMap[t]=[]);var n=this._obsMap[t];return n.index"
    "Of(e)<0?(n.push(e),function(){n.splice(n.indexOf(e),1)}):void 0}},o.prototype.notify=funct"
    "ion(t){void 0!==t&&this._obsMap&&this._obsMap[t]&&this._obsMap[t].forEach((function(t){ret"
    "urn t.update()}))},o.prototype.setParent=function(t,e){this._parent=t,this._key=e},o.proto"
    "type.notifyParent=function(){this._parent&&this._parent.notify(this._key)};var r={PUSH:'pu"
    "sh',POP:'pop',UNSHIFT:'unshift',SHIFT:'shift',SORT:'sort',SPLICE:'splice',REVERSE:'reverse"
    "'},s=Object.keys(r).map((function(t){return r[t]}));function c(t){if(!(this instanceof c))"
    "return new c(t);var e=this._vm=this;'[object Object]'===Object.prototype.toString.call(t)&"
    "&Object.keys(t).forEach((function(n){var i=t[n];'render'===n?e.$render=i:'data'===n?functi"
    "on(t,e){'function'==typeof e&&(e=e.call(t,t));Object.keys(e).forEach((function(n){return f"
    "unction(t,e,n){Object.defineProperty(t,n,{enumerable:!1,configurable:!0,get:function(){ret"
    "urn e[n]},set:function(t){e[n]=t}})}(t,e,n)})),o.of(e)}(e,i):'styleSheet'===n?initStyleShe"
    "et(i):'function'==typeof i&&(e[n]=i.bind(e))}))}return c.prototype.$watch=function(t,e,n){"
    "return new i(this,t,e,n)},c}));";
#endif
