/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
import app from '@system.app'
import Context from '@ohos.napi_context'

import {
	describe,
	beforeAll,
	beforeEach,
	afterEach,
	afterAll,
	it,
	expect
} from 'deccjsunit/index'

describe('webgl1Test', function() {
	console.info('webgltest start');
	var gl;
	var gl2;

	var indices = new Uint16Array([0, 1, 2, 1, 3, 4]);
	var vertices = new Uint16Array([
		-0.5, 0.5, 0.0,
		0.0, 0.5, 0.0,
		-0.25, 0.25, 0.0,
		0.5, 0.5, 0.0,
		0.25, 0.25, 0.0,
	])

	//顶点着色器程序
	var VSHADER_SOURCE =
		"attribute vec4 a_Position;" +
		"void main() {" +
		//设置坐标
		"gl_Position = a_Position; " +
		//    "gl_PointSize = 10.0;" +
		"} ";

	//片元着色器
	var FSHADER_SOURCE =
		"void main() {" +
		//设置颜色
		"gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);" +
		"}";

	function globalFunction() {
		const vertexShader = gl.createShader(gl.VERTEX_SHADER);
		gl.shaderSource(vertexShader, VSHADER_SOURCE);
		gl.compileShader(vertexShader);
		const fragmentShader = gl.createShader(gl.FRAGMENT_SHADER);
		gl.shaderSource(fragmentShader, FSHADER_SOURCE);
		gl.compileShader(fragmentShader);
		const programObj = gl.createProgram();
		console.info("testUseProgram has failed for " + programObj)
		const useProgramError1 = gl.getError();
		console.info("useProgramError: " + useProgramError1);
		const renderBufferValue1 = gl.getParameter(gl.CURRENT_PROGRAM);
		console.info("testUseProgram has failed for " + renderBufferValue1)
		gl.attachShader(programObj, vertexShader);
		gl.attachShader(programObj, fragmentShader);
		gl.linkProgram(programObj);
		gl.useProgram(programObj);
		return programObj;
	}


	function createProgram(gl) {
		//顶点着色器程序
		var VSHADER_SOURCE =
			'attribute vec4 a_Position;\n' +
			'void main() {\n' +
			'  gl_Position = a_Position;\n' +
			'}\n';

		// 片元着色器程序
		var FSHADER_SOURCE =
			'precision mediump float;\n' +
			'uniform vec4 u_FragColor;\n' +
			'void main() {\n' +
			'  gl_FragColor = u_FragColor;\n' +
			'}\n';
		var vertexShader = gl.createShader(gl.VERTEX_SHADER);
		gl.shaderSource(vertexShader, VSHADER_SOURCE);
		gl.compileShader(vertexShader);
		var fragmentShader = gl.createShader(gl.FRAGMENT_SHADER);
		gl.shaderSource(fragmentShader, FSHADER_SOURCE);
		gl.compileShader(fragmentShader);
		const programObj = gl.createProgram();
		console.log("testUseProgram has failed for " + programObj)
		const useProgramError1 = gl.getError();
		console.info("useProgramError: " + useProgramError1);
		const renderBufferValue1 = gl.getParameter(gl.CURRENT_PROGRAM);
		console.log("testUseProgram has failed for " + renderBufferValue1)
		gl.attachShader(programObj, vertexShader);
		gl.attachShader(programObj, fragmentShader);
		gl.linkProgram(programObj);
		gl.useProgram(programObj);
		return programObj;
	}

	function initShaders(gl, vshader, fshader) {
		var program = createProgramExternal(gl, vshader, fshader);
		console.log("======createProgram program: " + JSON.stringify(program));

		if (!program) {
			console.log('Failed to create program');
			return false;
		}

		gl.useProgram(program);
		gl.program = program;

		return true;
	}

	function createProgramExternal(gl, vshader, fshader) {
		// Create shader object
		var vertexShader = loadShader(gl, gl.VERTEX_SHADER, vshader);
		console.log("======vertexShader: " + vertexShader);
		var fragmentShader = loadShader(gl, gl.FRAGMENT_SHADER, fshader);
		if (!vertexShader || !fragmentShader) {
			return null;
		}

		// Create a program object
		var program = gl.createProgram();
		console.log("======createProgram program: " + JSON.stringify(program));

		if (!program) {
			return null;
		}

		// Attach the shader objects
		gl.attachShader(program, vertexShader);
		gl.attachShader(program, fragmentShader);

		// Link the program object
		gl.linkProgram(program);

		// Check the result of linking
		var linked = gl.getProgramParameter(program, 0x8B82);
		console.log("======getProgramParameter linked: " + linked);

		const getUniformLocationValue = gl.getUniformLocation(program, "a_Position");
		console.log("======getUniformLocation: " + JSON.stringify(getUniformLocationValue));


		if (!linked) {
			var error = gl.getProgramInfoLog(program);
			console.log('Failed to link program: ' + error);
			gl.deleteProgram(program);
			gl.deleteShader(fragmentShader);
			gl.deleteShader(vertexShader);
			return null;
		}

		return program;
	}

	function loadShader(gl, type, source) {
		console.log("======into loadShader====");
		// Create shader object
		var shader = gl.createShader(type);
		if (shader == null) {
			console.log('unable to create shader');
			return null;
		}

		const isShaderValue = gl.isShader(shader);
		console.log('isShader: ' + isShaderValue);

		// Set the shader program
		gl.shaderSource(shader, source);

		// Compile the shader
		gl.compileShader(shader);

		// Check the result of compilation
		var compiled = gl.getShaderParameter(shader, gl.COMPILE_STATUS);
		if (!compiled) {
			var error = gl.getShaderInfoLog(shader);
			console.log('Failed to compile shader: ' + error);
			gl.deleteShader(shader);
			return null;
		}

		var vertex = gl.getShaderParameter(shader, gl.VERTEX_SHADER);
		console.log('getShaderParameter VERTEX_SHADER: ' + vertex);


		return shader;
	}

	function initVertexBuffers(gl) {
		var vertices = new Float32Array([
			0.0, -1.0, -0.5, 0, 0.5, 0
		]);

		var n = 3; // 点的个数

		// 创建缓冲区对象
		var vertexBuffer = gl.createBuffer();
		if (!vertexBuffer) {
			console.log('Failed to create the buffer object');
			return -1;
		}

		// 将缓冲区对象绑定到目标
		gl.bindBuffer(gl.ARRAY_BUFFER, vertexBuffer);
		// 向缓冲区对象写入数据
		gl.bufferData(gl.ARRAY_BUFFER, vertices.buffer, gl.STATIC_DRAW);

		var aPosition = gl.getAttribLocation(gl.program, 'a_Position');
		console.info("webgl# getAttribLocation getAttribLocation success:" + JSON.stringify(gl.program));
		console.info("webgl# getAttribLocation getAttribLocation success:" + aPosition);
		if (aPosition < 0) {
			console.log('Failed to get the storage location of a_Position');
			return -1;
		}
		// 将缓冲区对象分配给a_Position变量
		gl.vertexAttribPointer(aPosition, 2, gl.FLOAT, false, 0, 0);

		// 连接a_Position变量与分配给它的缓冲区对象
		gl.enableVertexAttribArray(aPosition);

		return n;
	}


	var float1 = new Float32Array([1.1, 1.2, 1.3, 1.4]);
	var int1 = new Int8Array([1, 1, 1, 1]);
	var uint1 = new Uint8Array([1, 1, 1, 1]);
	var float2 = [1.1, 1.2, 1.3, 1.4];
	var int2 = [1, 1, 1, 1];
	var uint2 = [1, 1, 1, 1];

	function initContext() {
		console.info('initContext start');
		// 获取canvas元素
		const el = global.el;
		const el2 = global.el2;
		// 获取webgl上下文
		gl = el.getContext('webgl');
		if (!gl) {
			console.log('webgltest Failed to get the rendering context for WebGL');
		}
		gl2 = el2.getContext('webgl2');
		if (!gl) {
			console.log('webgltest Failed to get the rendering context for WebGL2');
		}
		console.info('webgltest initContext finish');
	}

	function deleteContext() {
		if (gl != null) {
			gl = null;
			console.info("webgltest gl has null");
		}
		if (gl2 != null) {
			console.info("webgltest gl2 has null");
			gl2 = null;
		}
	}

	/**
	 * run before testClass
	 */
	beforeAll(async function(done) {
		console.info('webgltest beforeAll called');
		initContext();
		done();
	});

	/**
	 * run after testClass
	 */
	afterAll(async function(done) {
		console.info('webgltest afterEach called');
		deleteContext();
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0001
	 * @tc.name webgl_test_getContextAttributes
	 * @tc.desc Test getContextAttributes.
	 */
	it('webgl_test_getContextAttributes', 0, async function(done) {
		console.info("webgltest into getContextAttributes");
		const getContextAttributesValue = gl.getContextAttributes();
		console.info("webgltest getContextAttributes: " + getContextAttributesValue);
		expect(typeof(getContextAttributesValue)).assertEqual("object" || null);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0002
	 * @tc.name webgl_test_isContextLost
	 * @tc.desc Test isContextLost.
	 */
	it('webgl_test_isContextLost', 0, async function(done) {
		console.info("webgltest into isContextLost");
		const isContextLostValue = gl.isContextLost();
		console.info("webgltest isContextLost: " + isContextLostValue);
		expect(isContextLostValue).assertEqual(false);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0003
	 * @tc.name webgl_test_getSupportedExtensions
	 * @tc.desc Test getSupportedExtensions.
	 */
	it('webgl_test_getSupportedExtensions', 0, async function(done) {
		//initContext();
		console.info("webgltest into getSupportedExtensions");
		const getSupportedExtensionsValue = gl.getSupportedExtensions();
		console.info("webgltest getSupportedExtensions: " + getSupportedExtensionsValue);
		expect(typeof(getSupportedExtensionsValue)).assertEqual('object');
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0005
	 * @tc.name webgl_test_getExtension
	 * @tc.desc Test getExtension.
	 */
	it('webgl_test_getExtension', 0, async function(done) {
		//initContext();
		console.info("webgltest into getExtension");
		const getExtensionValue = gl.getExtension();
		console.info("webgltest getExtension: " + getExtensionValue);
		expect(getExtensionValue).assertEqual(undefined);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0006
	 * @tc.name webgl_test_activeTexture
	 * @tc.desc Test activeTexture.
	 */
	it('webgl_test_activeTexture', 0, async function(done) {
		//initContext();
		console.info("webgltest into activeTexture");
		gl.activeTexture(gl.TEXTURE0);
		const activeTextureParameter = gl.getParameter(gl.ACTIVE_TEXTURE);
		console.info("webgltest activeTexture --> getParameter: " + activeTextureParameter);
		expect(activeTextureParameter).assertEqual(33984);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0007
	 * @tc.name webgl_test_activeTexture_1
	 * @tc.desc Test activeTexture.
	 */
	it('webgl_test_activeTexture_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into activeTexture");
		gl.activeTexture(gl.TEXTURE1);
		const activeTextureParameter = gl.getParameter(gl.ACTIVE_TEXTURE);
		console.info("webgltest activeTexture --> getParameter: " + activeTextureParameter);
		expect(activeTextureParameter).assertEqual(33985);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0008
	 * @tc.name webgl_test_attachShader
	 * @tc.desc Test attachShader.
	 */
	it('webgl_test_attachShader', 0, async function(done) {
		//initContext();
		console.info("webgltest into attachShader");

		//顶点着色器程序
		var VSHADER_SOURCE =
			'attribute vec4 a_Position;\n' +
			'void main() {\n' +
			'  gl_Position = a_Position;\n' +
			'}\n';

		// 片元着色器程序
		var FSHADER_SOURCE =
			'precision mediump float;\n' +
			'uniform vec4 u_FragColor;\n' +
			'void main() {\n' +
			'  gl_FragColor = u_FragColor;\n' +
			'}\n';
		var vertexShader = gl.createShader(gl.VERTEX_SHADER);
		gl.shaderSource(vertexShader, VSHADER_SOURCE);
		gl.compileShader(vertexShader);
		var fragmentShader = gl.createShader(gl.FRAGMENT_SHADER);
		gl.shaderSource(fragmentShader, FSHADER_SOURCE);
		gl.compileShader(fragmentShader);
		const programObj = gl.createProgram();
		console.log("testUseProgram has failed for " + programObj)
		const useProgramError1 = gl.getError();
		console.info("useProgramError: " + useProgramError1);
		const renderBufferValue1 = gl.getParameter(gl.CURRENT_PROGRAM);
		console.log("testUseProgram has failed for " + renderBufferValue1)
		gl.attachShader(programObj, vertexShader);
		gl.attachShader(programObj, fragmentShader);
		gl.linkProgram(programObj);
		gl.useProgram(programObj);

		let errorCode = gl.getError();
		console.info("webgltest attachShader getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0009
	 * @tc.name webgl_test_bindAttribLocation
	 * @tc.desc Test bindAttribLocation.
	 */
	it('webgl_test_bindAttribLocation', 0, async function(done) {
		//initContext();
		console.info("webgltest into bindAttribLocation");

		//顶点着色器程序
		var VSHADER_SOURCE =
			'attribute vec4 a_Position;\n' +
			'void main() {\n' +
			'  gl_Position = a_Position;\n' +
			'}\n';

		// 片元着色器程序
		var FSHADER_SOURCE =
			'precision mediump float;\n' +
			'uniform vec4 u_FragColor;\n' +
			'void main() {\n' +
			'  gl_FragColor = u_FragColor;\n' +
			'}\n';
		var vertexShader = gl.createShader(gl.VERTEX_SHADER);
		gl.shaderSource(vertexShader, VSHADER_SOURCE);
		gl.compileShader(vertexShader);
		var fragmentShader = gl.createShader(gl.FRAGMENT_SHADER);
		gl.shaderSource(fragmentShader, FSHADER_SOURCE);
		gl.compileShader(fragmentShader);
		const programObj = gl.createProgram();
		console.log("testUseProgram has failed for " + programObj)
		const useProgramError1 = gl.getError();
		console.info("useProgramError: " + useProgramError1);
		const renderBufferValue1 = gl.getParameter(gl.CURRENT_PROGRAM);
		console.log("testUseProgram has failed for " + renderBufferValue1)
		gl.attachShader(programObj, vertexShader);
		gl.attachShader(programObj, fragmentShader);
		gl.linkProgram(programObj);
		gl.useProgram(programObj);

		gl.bindAttribLocation(programObj, 1, 'a_Position');

		let errorCode = gl.getError();
		console.info("webgltest bindAttribLocation getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0010
	 * @tc.name webgl_test_bindBuffer
	 * @tc.desc Test bindBuffer.
	 */
	it('webgl_test_bindBuffer', 0, async function(done) {
		//initContext();
		console.info("webgltest into bindBuffer");

		var buffer = gl.createBuffer();

		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);

		//bind后isBuffer为true
		const isBufferValue = gl.isBuffer(buffer);
		console.info("isBufferValue: " + isBufferValue);
		expect(isBufferValue).assertEqual(true);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0011
	 * @tc.name webgl_test_bindBuffer_1
	 * @tc.desc Test bindBuffer.
	 */
	it('webgl_test_bindBuffer_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into bindBuffer");

		var buffer = gl.createBuffer();

		// bind前isBuffer为false
		const isBufferValue0 = gl.isBuffer(buffer);
		console.info("webgltest isBufferValue0: " + isBufferValue0);
		expect(isBufferValue0).assertEqual(false);

		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);

		//bind后isBuffer为true
		const isBufferValue = gl.isBuffer(buffer);
		console.info("isBufferValue: " + isBufferValue);
		expect(isBufferValue).assertEqual(true);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0012
	 * @tc.name webgl_test_bindFramebuffer
	 * @tc.desc Test bindFramebuffer.
	 */
	it('webgl_test_bindFramebuffer', 0, async function(done) {
		//initContext();
		console.info("webgltest into bindFramebuffer");

		var framebuffer = gl.createFramebuffer();

		const isFramebuffer1 = gl.isFramebuffer(framebuffer);
		console.info("createFramebuffer --> isFramebuffer1: " + isFramebuffer1);
		expect(isFramebuffer1).assertEqual(false);

		// bind
		gl.bindFramebuffer(gl.FRAMEBUFFER, framebuffer);

		const isFrameBuffer2 = gl.isFramebuffer(framebuffer);
		console.info("createFramebuffer --> bindFramebuffer --> isFrameBuffer2: " +
			isFrameBuffer2);
		expect(isFrameBuffer2).assertEqual(true);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0013
	 * @tc.name webgl_test_bindRenderbuffer
	 * @tc.desc Test bindRenderbuffer.
	 */
	it('webgl_test_bindRenderbuffer', 0, async function(done) {
		//initContext();
		console.info("webgltest into bindRenderbuffer");

		var renderbuffer = gl.createRenderbuffer();

		//不 bindRenderbuffer, 那么下面会返回 false
		const isRenderBuffer0 = gl.isRenderbuffer(renderbuffer);
		console.info("createRenderbuffer --> isRenderbuffer0: " + isRenderBuffer0);
		expect(isRenderBuffer0).assertEqual(false);

		gl.bindRenderbuffer(gl.RENDERBUFFER, renderbuffer);

		// bindRenderbuffer后, isRenderbuffer返回true
		const isRenderBuffer1 = gl.isRenderbuffer(renderbuffer);
		console.info("createRenderbuffer --> bindRenderbuffer --> isRenderbuffer: " +
			isRenderBuffer1);
		expect(isRenderBuffer1).assertEqual(true);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0014
	 * @tc.name webgl_test_bindTexture
	 * @tc.desc Test bindTexture.
	 */
	it('webgl_test_bindTexture', 0, async function(done) {
		//initContext();
		console.info("webgltest into bindTexture");

		var texture = gl.createTexture();

		const isTexture0 = gl.isTexture(texture);
		console.info("createTexture --> isTexture: " + isTexture0);
		expect(isTexture0).assertEqual(false);

		gl.bindTexture(gl.TEXTURE_2D, texture);

		const isTexture1 = gl.isTexture(texture);
		console.info("createTexture --> bindTexture --> isTexture: " + isTexture1);
		expect(isTexture1).assertEqual(true);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0015
	 * @tc.name webgl_test_blendColor
	 * @tc.desc Test blendColor.
	 */
	it('webgl_test_blendColor', 0, async function(done) {
		//initContext();
		console.info("webgltest into blendColor");

		gl.blendColor(0, 0.5, 1, 1);
		const blendColorValue = gl.getParameter(gl.BLEND_COLOR);
		console.info("blendColor --> getParameter: " + blendColorValue);

		var blendColorArr = new Float32Array([0, 0.5, 1, 1]);

		// expect(blendColorValue).assertEqual(blendColorArr);

		// 判断数据类型是否正确
		expect(blendColorValue.constructor).assertEqual(blendColorArr.constructor);
		// 判断数据值是否正确
		expect(blendColorValue.toString()).assertEqual(blendColorArr.toString());

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0016
	 * @tc.name webgl_test_blendColor_1
	 * @tc.desc Test blendColor.
	 */
	it('webgl_test_blendColor_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into blendColor");

		gl.blendColor(1, 1, 1, 1);
		const blendColorValue = gl.getParameter(gl.BLEND_COLOR);
		console.info("blendColor --> getParameter: " + blendColorValue);

		var blendColorArr = new Float32Array([1, 1, 1, 1]);

		// expect(blendColorValue).assertEqual(blendColorArr);

		// 判断数据类型是否正确
		expect(blendColorValue.constructor).assertEqual(blendColorArr.constructor);
		// 判断数据值是否正确
		expect(blendColorValue.toString()).assertEqual(blendColorArr.toString());

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0017
	 * @tc.name webgl_test_blendEquation
	 * @tc.desc Test blendEquation.
	 */
	it('webgl_test_blendEquation', 0, async function(done) {
		//initContext();
		console.info("webgltest into blendEquation");

		gl.blendEquation(gl.FUNC_ADD);

		const blendEquationValue = gl.getParameter(gl.BLEND_EQUATION_RGB);
		console.info("blendEquation --> getParameter: " + blendEquationValue);
		expect(blendEquationValue).assertEqual(gl.FUNC_ADD);

		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0018
	 * @tc.name webgl_test_blendEquation_1
	 * @tc.desc Test blendEquation.
	 */
	it('webgl_test_blendEquation_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into blendEquation");

		gl.blendEquation(gl.FUNC_ADD);

		const blendEquationValue2 = gl.getParameter(gl.BLEND_EQUATION_ALPHA);
		console.info("blendEquation --> getParameter: " + blendEquationValue2);
		expect(blendEquationValue2).assertEqual(32774);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0019
	 * @tc.name webgl_test_blendEquationSeparate
	 * @tc.desc Test blendEquationSeparate.
	 */
	it('webgl_test_blendEquationSeparate', 0, async function(done) {
		//initContext();
		console.info("webgltest into blendEquationSeparate");
		gl.blendEquationSeparate(gl.FUNC_ADD, gl.FUNC_SUBTRACT);

		const blendEquationSeparateValue = gl.getParameter(gl.BLEND_EQUATION_RGB);
		console.info("blendEquation --> getParameter: " + blendEquationSeparateValue);
		expect(blendEquationSeparateValue).assertEqual(gl.FUNC_ADD);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0020
	 * @tc.name webgl_test_blendFunc
	 * @tc.desc Test blendFunc.
	 */
	it('webgl_test_blendFunc', 0, async function(done) {
		//initContext();
		console.info("webgltest into blendFunc");

		gl.enable(gl.BLEND);

		gl.blendFunc(gl.SRC_COLOR, gl.DST_COLOR);

		const blendFuncValue = gl.getParameter(gl.BLEND_SRC_RGB);
		console.info("blendFunc --> getParameter: " + blendFuncValue);
		expect(blendFuncValue).assertEqual(gl.SRC_COLOR);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0021
	 * @tc.name webgl_test_blendFuncSeparate
	 * @tc.desc Test blendFuncSeparate.
	 */
	it('webgl_test_blendFuncSeparate', 0, async function(done) {
		//initContext();
		console.info("webgltest into blendFuncSeparate");

		gl.enable(gl.BLEND);

		gl.blendFuncSeparate(gl.SRC_COLOR, gl.DST_COLOR, gl.ONE, gl.ZERO);

		const blendFuncSeparateParameter = gl.getParameter(gl.BLEND_SRC_RGB)
		console.info("blendFuncSeparate --> getParameter: " + blendFuncSeparateParameter);
		expect(blendFuncSeparateParameter).assertEqual(gl.SRC_COLOR);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0022
	 * @tc.name webgl_test_checkFramebufferStatus2
	 * @tc.desc Test checkFramebufferStatus.
	 */
	it('webgl_test_checkFramebufferStatus2', 0, async function(done) {
		//initContext();
		console.info("webgltest into checkFramebufferStatus");

		var framebuffer = gl.createFramebuffer();
		gl.bindFramebuffer(gl.FRAMEBUFFER, framebuffer);

		const checkFramebufferStatus = gl.checkFramebufferStatus(gl.FRAMEBUFFER);
		console.info("createFramebuffer --> bindFramebuffer --> checkFramebufferStatus: " +
			checkFramebufferStatus);
		expect(checkFramebufferStatus).assertEqual(gl.FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0023
	 * @tc.name webgl_test_checkFramebufferStatus_1
	 * @tc.desc Test checkFramebufferStatus.
	 */
	it('webgl_test_checkFramebufferStatus_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into checkFramebufferStatus");

		var framebuffer = gl.createFramebuffer();
		gl.bindFramebuffer(gl.FRAMEBUFFER, framebuffer);

		gl.deleteFramebuffer(framebuffer);

		const checkFrameBufferStatus2 = gl.checkFramebufferStatus(gl.FRAMEBUFFER);
		console.info(
			"createFramebuffer --> bindFramebuffer --> deleteFramebuffer --> checkFrameBufferStatus2: " +
			checkFrameBufferStatus2);
		expect(checkFrameBufferStatus2).assertEqual(gl.FRAMEBUFFER_COMPLETE);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0024
	 * @tc.name webgl_test_clear
	 * @tc.desc Test clear.
	 */
	it('webgl_test_clear', 0, async function(done) {
		//initContext();
		console.info("webgltest into clear");
		gl.clear(gl.COLOR_BUFFER_BIT);
		var float32Array = new Float32Array([0, 0, 0, 0]);
		const clearParameter = gl.getParameter(gl.COLOR_CLEAR_VALUE);
		console.info("webgltest clear getParameter: " + clearParameter);
		expect(typeof(clearParameter)).assertEqual(typeof(float32Array));
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0025
	 * @tc.name webgl_test_clearColor
	 * @tc.desc Test clearColor.
	 */
	it('webgl_test_clearColor', 0, async function(done) {
		//initContext();
		console.info("webgltest into clearColor");
		gl.clearColor(1, 0.5, 0.5, 3);

		const clearColorParameter = gl.getParameter(gl.COLOR_CLEAR_VALUE);

		var float32ArrayValue = new Float32Array([1, 0.5, 0.5, 3]);

		console.info("webgltest clearColor: " + clearColorParameter);
		expect(clearColorParameter.toString()).assertEqual(float32ArrayValue.toString());
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0026
	 * @tc.name webgl_test_clearDepth
	 * @tc.desc Test clearDepth.
	 */
	it('webgl_test_clearDepth', 0, async function(done) {
		//initContext();
		console.info("webgltest into clearDepth");

		//用于设置深度缓冲区的深度清除值。
		gl.clearDepth(0.5);
		//若要获取当前深度清除值，查询DEPTH_CLEAR_VALUE 常量。
		const clearDepthValue = gl.getParameter(gl.DEPTH_CLEAR_VALUE);
		// 0.5
		console.info("webgltest clearDepth --> getParameter: " + clearDepthValue);

		expect(clearDepthValue).assertEqual(0.5);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0027
	 * @tc.name webgl_test_clearDepth_2
	 * @tc.desc Test clearDepth.
	 */
	it('webgl_test_clearDepth_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into clearDepth");

		//用于设置深度缓冲区的深度清除值。
		gl.clearDepth(8);
		//若要获取当前深度清除值，查询DEPTH_CLEAR_VALUE 常量。
		const clearDepthValue = gl.getParameter(gl.DEPTH_CLEAR_VALUE);
		// 0.5
		console.info("webgltest clearDepth --> getParameter: " + clearDepthValue);

		expect(clearDepthValue).assertEqual(1);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0028
	 * @tc.name webgl_test_clearStencil
	 * @tc.desc Test clearStencil.
	 */
	it('webgl_test_clearStencil', 0, async function(done) {
		//initContext();
		console.info("webgltest into clearStencil");

		gl.clearStencil(1);
		//要获取当前模板清除值，请查询STENCIL_CLEAR_VALUE 常量。
		const clearStencilValue = gl.getParameter(gl.STENCIL_CLEAR_VALUE);
		console.info("webgltest clearStencil --> getParameter: " + clearStencilValue);
		expect(clearStencilValue).assertEqual(1);

		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0029
	 * @tc.name webgl_test_clearStencil_1
	 * @tc.desc Test clearStencil.
	 */
	it('webgl_test_clearStencil_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into clearStencil");

		gl.clearStencil(20);
		//要获取当前模板清除值，请查询STENCIL_CLEAR_VALUE 常量。
		const clearStencilValue = gl.getParameter(gl.STENCIL_CLEAR_VALUE);
		console.info("webgltest clearStencil --> getParameter: " + clearStencilValue);
		expect(clearStencilValue).assertEqual(20);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0030
	 * @tc.name webgl_test_colorMask
	 * @tc.desc Test colorMask.
	 */
	it('webgl_test_colorMask', 0, async function(done) {
		//initContext();
		console.info("webgltest into colorMask");

		gl.colorMask(true, true, true, false);
		// 要获取当前的颜色掩码，请查询COLOR_WRITEMASK返回Array.
		const colorMaskValue = gl.getParameter(gl.COLOR_WRITEMASK);
		// [true, true, true, false]
		console.info("webgltest colorMask --> getParameter: " + colorMaskValue);

		expect(colorMaskValue.toString()).assertEqual('true,true,true,false');
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0031
	 * @tc.name webgl_test_colorMask_1
	 * @tc.desc Test colorMask.
	 */
	it('webgl_test_colorMask_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into colorMask");

		gl.colorMask(false, false, false, false);
		// 要获取当前的颜色掩码，请查询COLOR_WRITEMASK返回Array.
		const colorMaskValue = gl.getParameter(gl.COLOR_WRITEMASK);
		// [true, true, true, false]
		console.info("webgltest colorMask --> getParameter: " + colorMaskValue);

		expect(colorMaskValue.toString()).assertEqual('false,false,false,false');
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0032
	 * @tc.name webgl_test_compileShader
	 * @tc.desc Test compileShader.
	 */
	it('webgl_test_compileShader', 0, async function(done) {
		//initContext();
		console.info("webgltest into compileShader");

		var shader = gl.createShader(gl.VERTEX_SHADER);
		gl.shaderSource(shader, VSHADER_SOURCE);
		gl.compileShader(shader);

		let errorCode = gl.getError();
		console.info("webgltest compileShader getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0033
	 * @tc.name webgl_test_copyTexImage2D
	 * @tc.desc Test copyTexImage2D.
	 */
	it('webgl_test_copyTexImage2D', 0, async function(done) {
		//initContext();
		console.info("webgltest into copyTexImage2D");
		gl.copyTexImage2D(gl.TEXTURE_2D, 0, gl.RGBA, 0, 0, 512, 512, 0);
		let errorCode = gl.getError();
		console.info("webgltest copyTexImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0034
	 * @tc.name webgl_test_copyTexImage2D
	 * @tc.desc Test copyTexImage2D.
	 */
	it('webgl_test_copyTexImage2D', 0, async function(done) {
		//initContext();
		console.info("webgltest into copyTexImage2D");
		gl.copyTexImage2D(gl.TEXTURE_2D, 0, gl.RGBA, 0, 0, '512', 512, 0);
		let errorCode = gl.getError();
		console.info("webgltest copyTexImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0035
	 * @tc.name webgl_test_copyTexSubImage2D
	 * @tc.desc Test copyTexSubImage2D.
	 */
	it('webgl_test_copyTexSubImage2D', 0, async function(done) {
		//initContext();
		console.info("webgltest into copyTexSubImage2D");

		gl.copyTexSubImage2D(gl.TEXTURE_2D, 0, 0, 0, 0, 0, 16, 16);
		let errorCode = gl.getError();
		console.info("webgltest copyTexSubImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);

		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0036
	 * @tc.name webgl_test_copyTexSubImage2D_1
	 * @tc.desc Test copyTexSubImage2D.
	 */
	it('webgl_test_copyTexSubImage2D_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into copyTexSubImage2D");

		gl.copyTexSubImage2D(gl.TEXTURE_2D, 0, 0, 0, 0, 0, false, 16);
		let errorCode = gl.getError();
		console.info("webgltest copyTexSubImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0037
	 * @tc.name webgl_test_createBuffer
	 * @tc.desc Test createBuffer.
	 */
	it('webgl_test_createBuffer', 0, async function(done) {
		//initContext();
		console.info("webgltest into createBuffer");

		const buffer = gl.createBuffer();

		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		const isBufferValue = gl.isBuffer(buffer);
		console.info("isBufferValue: " + isBufferValue);

		console.info("webgltest createBuffer bindBuffer isBuffer: " + isBufferValue);
		expect(isBufferValue).assertEqual(true);

		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0038
	 * @tc.name webgl_test_createBuffer_1
	 * @tc.desc Test createBuffer.
	 */
	it('webgl_test_createBuffer_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into createBuffer");

		const buffer = gl.createBuffer();

		const isBufferValue = gl.isBuffer(buffer);
		console.info("isBufferValue: " + isBufferValue);

		console.info("webgltest createBuffer bindBuffer isBuffer: " + isBufferValue);
		expect(isBufferValue).assertEqual(false);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0039
	 * @tc.name webgl_test_createFramebuffer
	 * @tc.desc Test createFramebuffer.
	 */
	it('webgl_test_createFramebuffer', 0, async function(done) {
		//initContext();
		console.info("webgltest into createFramebuffer");

		const framebuffer = gl.createFramebuffer();

		gl.bindFramebuffer(gl.FRAMEBUFFER, framebuffer);
		const isFramebuffer = gl.isFramebuffer(framebuffer);
		console.info("webgltest createFramebuffer --> bindFramebuffer --> isFramebuffer: " +
			isFramebuffer);
		expect(isFramebuffer).assertEqual(true);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0040
	 * @tc.name webgl_test_createFramebuffer_1
	 * @tc.desc Test createFramebuffer.
	 */
	it('webgl_test_createFramebuffer_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into createFramebuffer");

		const framebuffer = gl.createFramebuffer();

		const isFramebuffer = gl.isFramebuffer(framebuffer);
		console.info("webgltest createFramebuffer --> bindFramebuffer --> isFramebuffer: " +
			isFramebuffer);
		expect(isFramebuffer).assertEqual(false);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0041
	 * @tc.name webgl_test_createProgram
	 * @tc.desc Test createProgram.
	 */
	it('webgl_test_createProgram', 0, async function(done) {
		//initContext();
		console.info("webgltest into createProgram");
		const program = gl.createProgram();
		const isProgram = gl.isProgram(program);
		console.info("webgltest createProgram isProgram: " + isProgram);
		expect(isProgram).assertEqual(true);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0042
	 * @tc.name webgl_test_createRenderbuffer
	 * @tc.desc Test createRenderbuffer.
	 */
	it('webgl_test_createRenderbuffer', 0, async function(done) {
		//initContext();
		console.info("webgltest into createRenderbuffer");

		const renderbuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderbuffer);
		const isRenderBuffer = gl.isRenderbuffer(renderbuffer);
		console.info(
			"createRenderbuffer --> bindRenderbuffer --> getParameter --> isRenderbuffer: " +
			isRenderBuffer);
		expect(isRenderBuffer).assertEqual(true);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0043
	 * @tc.name webgl_test_createRenderbuffer_1
	 * @tc.desc Test createRenderbuffer.
	 */
	it('webgl_test_createRenderbuffer_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into createRenderbuffer");

		const renderbuffer = gl.createRenderbuffer();
		const isRenderBuffer = gl.isRenderbuffer(renderbuffer);
		console.info(
			"createRenderbuffer --> bindRenderbuffer --> getParameter --> isRenderbuffer: " +
			isRenderBuffer);
		expect(isRenderBuffer).assertEqual(false);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0044
	 * @tc.name webgl_test_createShader
	 * @tc.desc Test createShader.
	 */
	it('webgl_test_createShader', 0, async function(done) {
		//initContext();
		console.info("webgltest into createShader");

		const shader = gl.createShader(gl.VERTEX_SHADER);
		const isShader = gl.isShader(shader);
		console.info("webgltest createShader isShader: " + isShader);
		expect(isShader).assertEqual(true);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0045
	 * @tc.name webgl_test_createTexture
	 * @tc.desc Test createTexture.
	 */
	it('webgl_test_createTexture', 0, async function(done) {
		//initContext();
		console.info("webgltest into createTexture");

		const texture = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, texture);
		const isTexture = gl.isTexture(texture);
		console.info("webgltest createTexture isTexture: " + isTexture);
		expect(isTexture).assertEqual(true);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0046
	 * @tc.name webgl_test_createTexture_1
	 * @tc.desc Test createTexture.
	 */
	it('webgl_test_createTexture_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into createTexture");

		const texture = gl.createTexture();
		const isTexture = gl.isTexture(texture);
		console.info("webgltest createTexture isTexture: " + isTexture);
		expect(isTexture).assertEqual(false);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0047
	 * @tc.name webgl_test_cullFace
	 * @tc.desc Test cullFace.
	 */
	it('webgl_test_cullFace', 0, async function(done) {
		//initContext();
		console.info("webgltest into cullFace");

		gl.enable(gl.CULL_FACE);
		gl.cullFace(gl.FRONT_AND_BACK);
		// 需要 CULL_FACE_MODE 常量来检查当前多边形剔除模式。
		const cullFaceparameter = gl.getParameter(gl.CULL_FACE_MODE)
		console.info("cullFace --> getParameter: " + cullFaceparameter);
		expect(cullFaceparameter).assertEqual(gl.FRONT_AND_BACK);

		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0048
	 * @tc.name webgl_test_cullFace_2
	 * @tc.desc Test cullFace.
	 */
	it('webgl_test_cullFace_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into cullFace");

		gl.enable(gl.CULL_FACE);
		gl.cullFace(gl.FRONT);
		// 需要 CULL_FACE_MODE 常量来检查当前多边形剔除模式。
		const cullFaceparameter = gl.getParameter(gl.CULL_FACE_MODE)
		console.info("cullFace --> getParameter: " + cullFaceparameter);
		expect(cullFaceparameter).assertEqual(gl.FRONT);

		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0049
	 * @tc.name webgl_test_cullFace_3
	 * @tc.desc Test cullFace.
	 */
	it('webgl_test_cullFace_3', 0, async function(done) {
		//initContext();
		console.info("webgltest into cullFace");

		gl.enable(gl.CULL_FACE);
		gl.cullFace(gl.BACK);
		// 需要 CULL_FACE_MODE 常量来检查当前多边形剔除模式。
		const cullFaceparameter = gl.getParameter(gl.CULL_FACE_MODE)
		console.info("cullFace --> getParameter: " + cullFaceparameter);
		expect(cullFaceparameter).assertEqual(gl.BACK);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0050
	 * @tc.name webgl_test_deleteBuffer
	 * @tc.desc Test deleteBuffer.
	 */
	it('webgl_test_deleteBuffer', 0, async function(done) {
		//initContext();
		console.info("webgltest into deleteBuffer");

		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		const isBuffer1 = gl.isBuffer(buffer);
		console.info("webgltest createBuffer bindBuffer isBuffer1: " + isBuffer1);
		expect(isBuffer1).assertEqual(true);

		gl.deleteBuffer(buffer);
		const isBuffer2 = gl.isBuffer(buffer);
		console.info("webgltest createBuffer bindBuffer deleteBuffer isBuffer2: " + isBuffer2);
		expect(isBuffer2).assertEqual(false);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0051
	 * @tc.name webgl_test_deleteBuffer_2
	 * @tc.desc Test deleteBuffer.
	 */
	it('webgl_test_deleteBuffer_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into deleteBuffer");

		var buffer = gl.createBuffer();

		const isBuffer1 = gl.isBuffer(buffer);
		console.info("webgltest createBuffer bindBuffer isBuffer1: " + isBuffer1);
		expect(isBuffer1).assertEqual(false);

		gl.deleteBuffer(buffer);
		const isBuffer2 = gl.isBuffer(buffer);
		console.info("webgltest createBuffer bindBuffer deleteBuffer isBuffer2: " + isBuffer2);
		expect(isBuffer2).assertEqual(false);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0052
	 * @tc.name webgl_test_deleteFramebuffer
	 * @tc.desc Test deleteFramebuffer.
	 */
	it('webgl_test_deleteFramebuffer', 0, async function(done) {
		//initContext();
		console.info("webgltest into deleteFramebuffer");

		var framebuffer = gl.createFramebuffer();
		gl.bindFramebuffer(gl.FRAMEBUFFER, framebuffer);

		const isFramebuffer1 = gl.isFramebuffer(framebuffer);
		console.info("webgltest createFramebuffer bindFramebuffer isFramebuffer1: " +
			isFramebuffer1);
		expect(isFramebuffer1).assertEqual(true);

		gl.deleteFramebuffer(framebuffer);

		const isFramebuffer2 = gl.isFramebuffer(framebuffer);
		console.info("webgltest createFramebuffer bindFramebuffer isFramebuffer2: " +
			isFramebuffer2);
		expect(isFramebuffer2).assertEqual(false);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0053
	 * @tc.name webgl_test_deleteProgram
	 * @tc.desc Test deleteProgram.
	 */
	it('webgl_test_deleteProgram', 0, async function(done) {
		//initContext();
		console.info("webgltest into deleteProgram");

		const program = gl.createProgram();
		const isProgram1 = gl.isProgram(program);
		console.info("webgltest createProgram isProgram1: " + isProgram1);
		expect(isProgram1).assertEqual(true);

		gl.deleteProgram(program);
		const isProgram2 = gl.isProgram(program);
		console.info("webgltest getExtension: " + isProgram2);
		expect(isProgram2).assertEqual(false);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0054
	 * @tc.name webgl_test_deleteProgram_2
	 * @tc.desc Test deleteProgram.
	 */
	it('webgl_test_deleteProgram_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into deleteProgram");

		const program = gl.createProgram();

		gl.deleteProgram(program);
		const isProgram2 = gl.isProgram(program);
		console.info("webgltest getExtension: " + isProgram2);
		expect(isProgram2).assertEqual(false);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0055
	 * @tc.name webgl_test_deleteRenderbuffer
	 * @tc.desc Test deleteRenderbuffer.
	 */
	it('webgl_test_deleteRenderbuffer', 0, async function(done) {
		//initContext();
		console.info("webgltest into deleteRenderbuffer");

		const renderbuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderbuffer);
		const isRenderbuffer1 = gl.isRenderbuffer(renderbuffer);
		console.info("webgltest createRenderbuffer bindRenderbuffer isRenderbuffer1: " +
			isRenderbuffer1);
		expect(isRenderbuffer1).assertEqual(true);

		gl.deleteRenderbuffer(renderbuffer);

		const isRenderbuffer2 = gl.isRenderbuffer(renderbuffer);
		console.info(
			"webgltest createRenderbuffer bindRenderbuffer  deleteRenderbuffer isRenderbuffer2: " +
			isRenderbuffer2);
		expect(isRenderbuffer2).assertEqual(false);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0056
	 * @tc.name webgl_test_deleteShader
	 * @tc.desc Test deleteShader.
	 */
	it('webgl_test_deleteShader', 0, async function(done) {
		//initContext();
		console.info("webgltest into deleteShader");

		var shader = gl.createShader(gl.VERTEX_SHADER);
		const isShader1 = gl.isShader(shader);
		console.info("webgltest createShader isShader: " + isShader1);
		expect(isShader1).assertEqual(true);

		gl.deleteShader(shader);

		const isShader2 = gl.isShader(shader);
		console.info("webgltest createShader deleteShader isShader2: " + isShader2);
		expect(isShader2).assertEqual(false);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0057
	 * @tc.name webgl_test_deleteShader_2
	 * @tc.desc Test deleteShader.
	 */
	it('webgl_test_deleteShader_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into deleteShader");

		var shader = gl.createShader(gl.VERTEX_SHADER);

		gl.deleteShader(shader);

		const isShader2 = gl.isShader(shader);
		console.info("webgltest createShader deleteShader isShader2: " + isShader2);
		expect(isShader2).assertEqual(false);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0058
	 * @tc.name webgl_test_deleteTexture
	 * @tc.desc Test deleteTexture.
	 */
	it('webgl_test_deleteTexture', 0, async function(done) {
		//initContext();
		console.info("webgltest into deleteTexture");

		var texture = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, texture);
		const isTexture1 = gl.isTexture(texture);
		console.info("webgltest createTexture isTexture: " + isTexture1);
		expect(isTexture1).assertEqual(true);

		gl.deleteTexture(texture);

		const isTexture2 = gl.isTexture(texture);
		console.info("webgltest createTexture deleteTexture isTexture2: " + isTexture2);
		expect(isTexture2).assertEqual(false);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0059
	 * @tc.name webgl_test_deleteTexture_2
	 * @tc.desc Test deleteTexture.
	 */
	it('webgl_test_deleteTexture_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into deleteTexture");

		var texture = gl.createTexture();

		const isTexture1 = gl.isTexture(texture);
		console.info("webgltest createTexture isTexture: " + isTexture1);
		expect(isTexture1).assertEqual(false);

		gl.deleteTexture(texture);

		const isTexture2 = gl.isTexture(texture);
		console.info("webgltest createTexture deleteTexture isTexture2: " + isTexture2);
		expect(isTexture2).assertEqual(false);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0060
	 * @tc.name webgl_test_depthFunc
	 * @tc.desc Test depthFunc.
	 */
	it('webgl_test_depthFunc', 0, async function(done) {
		//initContext();
		console.info("webgltest into depthFunc");

		gl.enable(gl.DEPTH_TEST);
		gl.depthFunc(gl.NEVER);
		// 要检查当前深度函数，请查询DEPTH_FUNC常量。
		const depthParameter = gl.getParameter(gl.DEPTH_FUNC);
		console.info("depthFunc --> getParameter: " + depthParameter);
		expect(depthParameter).assertEqual(gl.NEVER);

		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0061
	 * @tc.name webgl_test_depthFunc_2
	 * @tc.desc Test depthFunc.
	 */
	it('webgl_test_depthFunc_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into depthFunc");

		gl.enable(gl.DEPTH_TEST);
		gl.depthFunc(gl.LESS);
		// 要检查当前深度函数，请查询DEPTH_FUNC常量。
		const depthParameter = gl.getParameter(gl.DEPTH_FUNC);
		console.info("depthFunc --> getParameter: " + depthParameter);
		expect(depthParameter).assertEqual(gl.LESS);

		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0062
	 * @tc.name webgl_test_depthFunc_3
	 * @tc.desc Test depthFunc.
	 */
	it('webgl_test_depthFunc_3', 0, async function(done) {
		//initContext();
		console.info("webgltest into depthFunc");

		gl.enable(gl.DEPTH_TEST);
		gl.depthFunc(gl.EQUAL);
		// 要检查当前深度函数，请查询DEPTH_FUNC常量。
		const depthParameter = gl.getParameter(gl.DEPTH_FUNC);
		console.info("depthFunc --> getParameter: " + depthParameter);
		expect(depthParameter).assertEqual(gl.EQUAL);

		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0063
	 * @tc.name webgl_test_depthFunc_4
	 * @tc.desc Test depthFunc.
	 */
	it('webgl_test_depthFunc_4', 0, async function(done) {
		//initContext();
		console.info("webgltest into depthFunc");

		gl.enable(gl.DEPTH_TEST);
		gl.depthFunc(gl.LEQUAL);
		// 要检查当前深度函数，请查询DEPTH_FUNC常量。
		const depthParameter = gl.getParameter(gl.DEPTH_FUNC);
		console.info("depthFunc --> getParameter: " + depthParameter);
		expect(depthParameter).assertEqual(gl.LEQUAL);

		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0064
	 * @tc.name webgl_test_depthFunc_5
	 * @tc.desc Test depthFunc.
	 */
	it('webgl_test_depthFunc_5', 0, async function(done) {
		//initContext();
		console.info("webgltest into depthFunc");

		gl.enable(gl.DEPTH_TEST);
		gl.depthFunc(gl.GREATER);
		// 要检查当前深度函数，请查询DEPTH_FUNC常量。
		const depthParameter = gl.getParameter(gl.DEPTH_FUNC);
		console.info("depthFunc --> getParameter: " + depthParameter);
		expect(depthParameter).assertEqual(gl.GREATER);

		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0065
	 * @tc.name webgl_test_depthFunc_6
	 * @tc.desc Test depthFunc.
	 */
	it('webgl_test_depthFunc_6', 0, async function(done) {
		//initContext();
		console.info("webgltest into depthFunc");

		gl.enable(gl.DEPTH_TEST);
		gl.depthFunc(gl.NOTEQUAL);
		// 要检查当前深度函数，请查询DEPTH_FUNC常量。
		const depthParameter = gl.getParameter(gl.DEPTH_FUNC);
		console.info("depthFunc --> getParameter: " + depthParameter);
		expect(depthParameter).assertEqual(gl.NOTEQUAL);

		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0066
	 * @tc.name webgl_test_depthFunc_7
	 * @tc.desc Test depthFunc.
	 */
	it('webgl_test_depthFunc_7', 0, async function(done) {
		//initContext();
		console.info("webgltest into depthFunc");

		gl.enable(gl.DEPTH_TEST);
		gl.depthFunc(gl.GEQUAL);
		// 要检查当前深度函数，请查询DEPTH_FUNC常量。
		const depthParameter = gl.getParameter(gl.DEPTH_FUNC);
		console.info("depthFunc --> getParameter: " + depthParameter);
		expect(depthParameter).assertEqual(gl.GEQUAL);

		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0067
	 * @tc.name webgl_test_depthFunc_8
	 * @tc.desc Test depthFunc.
	 */
	it('webgl_test_depthFunc_8', 0, async function(done) {
		//initContext();
		console.info("webgltest into depthFunc");

		gl.enable(gl.DEPTH_TEST);
		gl.depthFunc(gl.ALWAYS);
		// 要检查当前深度函数，请查询DEPTH_FUNC常量。
		const depthParameter = gl.getParameter(gl.DEPTH_FUNC);
		console.info("depthFunc --> getParameter: " + depthParameter);
		expect(depthParameter).assertEqual(gl.ALWAYS);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0068
	 * @tc.name webgl_test_depthMask
	 * @tc.desc Test depthMask.
	 */
	it('webgl_test_depthMask', 0, async function(done) {
		//initContext();
		console.info("webgltest into depthMask");

		gl.depthMask(false);
		// 要获得当前的深度遮罩值，传入 DEPTH_WRITEMASK 常量，返回  Boolean.
		const depthMaskValue = gl.getParameter(gl.DEPTH_WRITEMASK);
		// false
		console.info("webgltest depthMaskValue: " + depthMaskValue);
		expect(depthMaskValue).assertEqual(false);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0069
	 * @tc.name webgl_test_depthMask_2
	 * @tc.desc Test depthMask.
	 */
	it('webgl_test_depthMask_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into depthMask");

		gl.depthMask(true);
		// 要获得当前的深度遮罩值，传入 DEPTH_WRITEMASK 常量，返回  Boolean.
		const depthMaskValue = gl.getParameter(gl.DEPTH_WRITEMASK);
		// false
		console.info("webgltest depthMaskValue: " + depthMaskValue);
		expect(depthMaskValue).assertEqual(true);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0070
	 * @tc.name webgl_test_depthRange
	 * @tc.desc Test depthRange.
	 */
	it('webgl_test_depthRange', 0, async function(done) {
		//initContext();
		console.info("webgltest into depthRange");

		gl.depthRange(0.2, 0.6);
		// 要检查当前深度范围，请查询DEPTH_RANGE返回一个的常量Float32Array
		const depthRangeValue = gl.getParameter(gl.DEPTH_RANGE);
		// Float32Array[0.2, 0.6]
		const float32ArrayValue = new Float32Array([0.2, 0.6]);
		console.info("webgltest depthRange: " + depthRangeValue);
		expect(depthRangeValue.toString()).assertEqual(float32ArrayValue.toString());
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0071
	 * @tc.name webgl_test_depthRange_2
	 * @tc.desc Test depthRange.
	 */
	it('webgl_test_depthRange_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into depthRange");

		gl.depthRange(0.8, 1.0);
		// 要检查当前深度范围，请查询DEPTH_RANGE返回一个的常量Float32Array
		const depthRangeValue = gl.getParameter(gl.DEPTH_RANGE);
		// Float32Array[0.2, 0.6]
		const float32ArrayValue = new Float32Array([0.8, 1.0]);
		console.info("webgltest depthRange: " + depthRangeValue);
		expect(depthRangeValue.toString()).assertEqual(float32ArrayValue.toString());
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0072
	 * @tc.name webgl_test_detachShader
	 * @tc.desc Test detachShader.
	 */
	it('webgl_test_detachShader', 0, async function(done) {
		//initContext();
		console.info("webgltest into detachShader");

		//顶点着色器
		var vertexShader = gl.createShader(gl.VERTEX_SHADER);
		gl.shaderSource(vertexShader, VSHADER_SOURCE);
		gl.compileShader(vertexShader);
		//片元着色器
		var fragmentShader = gl.createShader(gl.FRAGMENT_SHADER); //创建 WebGLShader。
		gl.shaderSource(fragmentShader, FSHADER_SOURCE); //fragmentSrc设置一个 WebGLShader 的源码。
		gl.compileShader(fragmentShader);

		//WebGLProgram
		var program = gl.createProgram(); //创建 WebGLProgram
		gl.attachShader(program, vertexShader); //往 WebGLProgram 添加一个片段或者顶点着色器。
		gl.attachShader(program, fragmentShader);
		gl.linkProgram(program); //链接给入的 WebGLProgram 对象
		gl.detachShader(program, vertexShader); //从一个WebGLProgram中分离一个先前附加的片段或者顶点着色器;
		gl.detachShader(program, fragmentShader);
		gl.deleteShader(vertexShader);
		gl.deleteShader(fragmentShader);

		let errorCode = gl.getError();
		console.info("webgltest uniform3uiv getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0073
	 * @tc.name webgl_test_disable
	 * @tc.desc Test disable.
	 */
	it('webgl_test_disable', 0, async function(done) {
		//initContext();
		console.info("webgltest into disable");

		gl.disable(gl.DITHER);
		// 要检查功能是否被禁用，请使用以下WebGLRenderingContext.isEnabled()方法：
		const isEnabled = gl.isEnabled(gl.DITHER);
		// false
		console.info("webgltest disable isEnabled: " + isEnabled);
		expect(isEnabled).assertEqual(false);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0074
	 * @tc.name webgl_test_disable_2
	 * @tc.desc Test disable.
	 */
	it('webgl_test_disable_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into disable");
		gl.enable(gl.DITHER);
		const isEnabled2 = gl.isEnabled(gl.DITHER);
		// true
		console.info("webgltest disable isEnabled2: " + isEnabled2);
		expect(isEnabled2).assertEqual(true);

		gl.disable(gl.DITHER);
		// 要检查功能是否被禁用，请使用以下WebGLRenderingContext.isEnabled()方法：
		const isEnabled = gl.isEnabled(gl.DITHER);
		// false
		console.info("webgltest disable isEnabled: " + isEnabled);
		expect(isEnabled).assertEqual(false);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0075
	 * @tc.name webgl_test_disableVertexAttribArray
	 * @tc.desc Test disableVertexAttribArray.
	 */
	it('webgl_test_disableVertexAttribArray', 0, async function(done) {
		//initContext();
		console.info("webgltest into disableVertexAttribArray");
		gl.disableVertexAttribArray(0);
		let errorCode = gl.getError();
		console.info("webgltest disableVertexAttribArray getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0076
	 * @tc.name webgl_test_drawArrays
	 * @tc.desc Test drawArrays.
	 */
	it('webgl_test_drawArrays', 0, async function(done) {
		//initContext();
		console.info("webgltest into drawArrays");

		gl.drawArrays(gl.POINTS, 0, 8);
		let errorCode = gl.getError();
		console.info("webgltest drawArrays getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0077
	 * @tc.name webgl_test_drawArrays_2
	 * @tc.desc Test drawArrays.
	 */
	it('webgl_test_drawArrays_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into drawArrays");

		gl.drawArrays(gl.POINTS, 0, -8);
		let errorCode = gl.getError();
		console.info("webgltest drawArrays getError: " + errorCode);
		expect(errorCode).assertEqual(1281);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0078
	 * @tc.name webgl_test_drawElements
	 * @tc.desc Test drawElements.
	 */
	it('webgl_test_drawElements', 0, async function(done) {
		//initContext();
		console.info("webgltest into drawElements");


		// 初始化着色器
		if (!initShaders(gl, VSHADER_SOURCE, FSHADER_SOURCE)) {
			console.log('Failed to intialize shaders.');
			return;
		}

		// 设置顶点位置
		var n = initVertexBuffers(gl);
		if (n < 0) {
			console.log('Failed to set the positions of the vertices');
			return;
		}

		var viewPortParameter = gl.getParameter(gl.MAX_VIEWPORT_DIMS);
		console.log('viewPortParameter: ' + viewPortParameter);

		const isContextLostValue = gl.isContextLost();
		console.info("isContextLostValue: " + isContextLostValue);

		// 指定清空<canvas>的颜色
		gl.clearColor(0.0, 0.0, 0.0, 1.0);

		// 清空<canvas>
		// COLOR_BUFFER_BIT	0x00004000
		gl.clear(gl.COLOR_BUFFER_BIT);

		console.info("indices.length: " + indices.length)
		gl.drawElements(gl.TRIANGLES, indices.length, gl.UNSIGNED_SHORT, 0);

		let errorCode = gl.getError();
		console.info("webgltest drawArrays getError: " + errorCode);
		expect(errorCode).assertEqual(1280);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0079
	 * @tc.name webgl_test_drawElements_2
	 * @tc.desc Test drawElements.
	 */
	it('webgl_test_drawElements_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into drawElements");

		gl.drawElements(1, 2, 3, 4);

		let errorCode = gl.getError();
		console.info("webgltest drawElements getError: " + errorCode);
		expect(errorCode).assertEqual(0x0500);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0080
	 * @tc.name webgl_test_enable
	 * @tc.desc Test enable.
	 */
	it('webgl_test_enable', 0, async function(done) {
		//initContext();
		console.info("webgltest into enable");

		gl.enable(gl.DITHER);

		const isEnabled = gl.isEnabled(gl.DITHER);
		// true
		console.info("webgltest enable isEnabled: " + isEnabled);
		expect(isEnabled).assertEqual(true);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0081
	 * @tc.name webgl_test_enable_2
	 * @tc.desc Test enable.
	 */
	it('webgl_test_enable_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into enable");

		gl.enable(gl.DITHER);

		const isEnabled = gl.isEnabled(gl.DITHER);
		// true
		console.info("webgltest enable isEnabled: " + isEnabled);
		expect(isEnabled).assertEqual(true);

		gl.disable(gl.DITHER);
		// 要检查功能是否被禁用，请使用以下WebGLRenderingContext.isEnabled()方法：
		const isEnabled2 = gl.isEnabled(gl.DITHER);
		// false
		console.info("webgltest disable isEnabled2: " + isEnabled2);
		expect(isEnabled2).assertEqual(false);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0082
	 * @tc.name webgl_test_enableVertexAttribArray
	 * @tc.desc Test enableVertexAttribArray.
	 */
	it('webgl_test_enableVertexAttribArray', 0, async function(done) {
		//initContext();
		console.info("webgltest into enableVertexAttribArray");
		//顶点着色器程序
		var VSHADER_SOURCE =
			'attribute vec4 a_Position;\n' +
			'void main() {\n' +
			'  gl_Position = a_Position;\n' +
			'}\n';

		// 片元着色器程序
		var FSHADER_SOURCE =
			'precision mediump float;\n' +
			'uniform vec4 u_FragColor;\n' +
			'void main() {\n' +
			'  gl_FragColor = u_FragColor;\n' +
			'}\n';
		var vertexShader = gl.createShader(gl.VERTEX_SHADER);
		gl.shaderSource(vertexShader, VSHADER_SOURCE);
		gl.compileShader(vertexShader);
		var fragmentShader = gl.createShader(gl.FRAGMENT_SHADER);
		gl.shaderSource(fragmentShader, FSHADER_SOURCE);
		gl.compileShader(fragmentShader);
		const programObj = gl.createProgram();
		console.log("testUseProgram has failed for " + programObj)
		const useProgramError1 = gl.getError();
		console.info("useProgramError: " + useProgramError1);
		const renderBufferValue1 = gl.getParameter(gl.CURRENT_PROGRAM);
		console.log("testUseProgram has failed for " + renderBufferValue1)
		gl.attachShader(programObj, vertexShader);
		gl.attachShader(programObj, fragmentShader);
		gl.linkProgram(programObj);
		gl.useProgram(programObj);

		const aVertexPosition = gl.getAttribLocation(programObj, "a_Position");

		gl.enableVertexAttribArray(aVertexPosition);
		gl.vertexAttribPointer(aVertexPosition, 4, gl.FLOAT, false, 0, 0);
		gl.drawArrays(gl.TRIANGLES, 0, 3);

		let errorCode = gl.getError();
		console.info("webgltest enableVertexAttribArray getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0083
	 * @tc.name webgl_test_finish
	 * @tc.desc Test finish.
	 */
	it('webgl_test_finish', 0, async function(done) {
		//initContext();
		console.info("webgltest into finish");
		gl.finish();
		let errorCode = gl.getError();
		console.info("webgltest finish getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0084
	 * @tc.name webgl_test_flush
	 * @tc.desc Test flush.
	 */
	it('webgl_test_flush', 0, async function(done) {
		//initContext();
		console.info("webgltest into flush");
		gl.flush();
		let errorCode = gl.getError();
		console.info("webgltest flush getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0085
	 * @tc.name webgl_test_framebufferRenderbuffer
	 * @tc.desc Test framebufferRenderbuffer.
	 */
	it('webgl_test_framebufferRenderbuffer', 0, async function(done) {
		//initContext();
		console.info("webgltest into framebufferRenderbuffer");

		const renderbuffer = gl.createRenderbuffer();
		gl.framebufferRenderbuffer(gl.DRAW_FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.RENDERBUFFER,
			renderbuffer);

		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0086
	 * @tc.name webgl_test_framebufferRenderbuffer_2
	 * @tc.desc Test framebufferRenderbuffer.
	 */
	it('webgl_test_framebufferRenderbuffer_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into framebufferRenderbuffer");

		const renderbuffer = gl.createRenderbuffer();
		gl.framebufferRenderbuffer(gl.DRAW_FRAMEBUFFER, "11", gl.RENDERBUFFER, renderbuffer);

		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0087
	 * @tc.name webgl_test_framebufferTexture2D
	 * @tc.desc Test framebufferTexture2D.
	 */
	it('webgl_test_framebufferTexture2D', 0, async function(done) {
		//initContext();
		console.info("webgltest into framebufferTexture2D");
		const texture = gl.createTexture();
		gl.framebufferTexture2D(gl.DRAW_FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, texture,
			0);

		let errorCode = gl.getError();
		console.info("webgltest framebufferTexture2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0088
	 * @tc.name webgl_test_framebufferTexture2D_1
	 * @tc.desc Test framebufferTexture2D.
	 */
	it('webgl_test_framebufferTexture2D_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into framebufferTexture2D");
		const texture = gl.createTexture();
		gl.framebufferTexture2D(gl.DRAW_FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, texture,
			"123");

		let errorCode = gl.getError();
		console.info("webgltest framebufferTexture2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);

		//deleteContext();
		done();
	});


	//********************************************* webgl2 ******************************************************

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0089
	 * @tc.name webgl_test_drawBuffers
	 * @tc.desc Test drawBuffers.
	 */
	it('webgl_test_drawBuffers', 0, async function(done) {
		//initContext();
		console.info("webgltest into drawBuffers");

		gl2.drawBuffers([gl2.NONE, gl2.COLOR_ATTACHMENT1]);

		let errorCode = gl.getError();
		console.info("webgltest drawBuffers getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0090
	 * @tc.name webgl_test_drawBuffers_2
	 * @tc.desc Test drawBuffers.
	 */
	it('webgl_test_drawBuffers_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into drawBuffers");
		gl2.drawBuffers(["gl2.NONE", "gl2.COLOR_ATTACHMENT1"]);
		let errorCode = gl.getError();
		console.info("webgltest drawBuffers getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_091
	 * @tc.name webgl_test_clearBufferfv
	 * @tc.desc Test clearBufferfv.
	 */
	it('webgl_test_clearBufferfv', 0, async function(done) {
		//initContext();
		console.info("webgltest into clearBufferfv");

		gl2.clearBufferfv(gl.COLOR, 0, [0.0, 0.0, 0.0, 0.0]);

		let errorCode = gl.getError();
		console.info("webgltest webgl_test_clearBufferfv getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0092
	 * @tc.name webgl_test_clearBufferfv_2
	 * @tc.desc Test clearBufferfv.
	 */
	it('webgl_test_clearBufferfv_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into clearBufferfv");

		gl2.clearBufferfv(gl.COLOR, "0", [0.0, 0.0, 0.0, 0.0]);

		let errorCode = gl.getError();
		console.info("webgltest webgl_test_clearBufferfv getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0093
	 * @tc.name webgl_test_clearBufferiv
	 * @tc.desc Test clearBufferiv.
	 */
	it('webgl_test_clearBufferiv', 0, async function(done) {
		//initContext();
		console.info("webgltest into clearBufferiv");

		gl2.clearBufferiv(gl.COLOR, 0, new Int32Array([0, 1, 0, 0.5]));

		let errorCode = gl.getError();
		console.info("webgltest clearBufferiv getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0094
	 * @tc.name webgl_test_clearBufferiv_2
	 * @tc.desc Test clearBufferiv.
	 */
	it('webgl_test_clearBufferiv_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into clearBufferiv");

		gl2.clearBufferiv(gl.COLOR, "0", new Int32Array([0, 1, 0, 0.5]));

		let errorCode = gl.getError();
		console.info("webgltest clearBufferiv getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0095
	 * @tc.name webgl_test_clearBufferuiv
	 * @tc.desc Test clearBufferuiv.
	 */
	it('webgl_test_clearBufferuiv', 0, async function(done) {
		//initContext();
		console.info("webgltest into clearBufferuiv");

		gl2.clearBufferuiv(gl.COLOR, 0, new Uint32Array([0, 1, 0, 0]));

		let errorCode = gl.getError();
		console.info("webgltest clearBufferuiv getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0096
	 * @tc.name webgl_test_clearBufferuiv_2
	 * @tc.desc Test clearBufferuiv.
	 */
	it('webgl_test_clearBufferuiv_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into clearBufferuiv");

		gl2.clearBufferuiv(gl.COLOR, "0", new Uint32Array([0, 1, 0, 0]));

		let errorCode = gl.getError();
		console.info("webgltest clearBufferuiv getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0097
	 * @tc.name webgl_test_clearBufferfi
	 * @tc.desc Test clearBufferfi.
	 */
	it('webgl_test_clearBufferfi', 0, async function(done) {
		//initContext();
		console.info("webgltest into clearBufferfi");

		gl2.clearBufferfi(gl.DEPTH_STENCIL, 0, 1.0, 0);

		let errorCode = gl.getError();
		console.info("webgltest clearBufferfi getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0098
	 * @tc.name webgl_test_clearBufferfi_2
	 * @tc.desc Test clearBufferfi.
	 */
	it('webgl_test_clearBufferfi_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into clearBufferfi");

		gl2.clearBufferfi(gl.DEPTH_STENCIL, "0", 1.0, 0);

		let errorCode = gl.getError();
		console.info("webgltest clearBufferfi getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0099
	 * @tc.name webgl_test_createQuery
	 * @tc.desc Test createQuery.
	 */
	it('webgl_test_createQuery', 0, async function(done) {
		//initContext();
		console.info("webgltest into createQuery");

		const query = gl2.createQuery();
		gl2.beginQuery(0x8C2F, query);

		const currentQuery = gl2.getQuery(gl2.ANY_SAMPLES_PASSED, 0x8865);
		const isQuery = gl2.isQuery(currentQuery);

		console.info("webgltest createQuery isQuery: " + isQuery);
		expect(isQuery).assertEqual(true);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0100
	 * @tc.name webgl_test_beginQuery
	 * @tc.desc Test beginQuery.
	 */
	it('webgl_test_beginQuery', 0, async function(done) {
		//initContext();
		console.info("webgltest into beginQuery");

		var query = gl2.createQuery();
		gl2.beginQuery(0x8C2F, query);

		const currentQuery = gl2.getQuery(0x8C2F, 0x8865);
		const isQuery1 = gl2.isQuery(currentQuery);
		console.info("webgltest createQuery isQuery1: " + isQuery1);
		expect(isQuery1).assertEqual(true);
		//deleteContext();
		done();
	});
})
