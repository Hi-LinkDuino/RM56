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


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0101
	 * @tc.name webgl_test_getQuery
	 * @tc.desc Test getQuery.
	 */
	it('webgl_test_getQuery', 0, async function(done) {
		//initContext();
		console.info("webgltest into getQuery");

		var query = gl2.createQuery();
		gl2.beginQuery(0x8C2F, query);

		var currentQuery = gl2.getQuery(0x8C2F, 0x8865);
		const isQuery1 = gl2.isQuery(currentQuery);
		console.info("webgltest createQuery isQuery1: " + isQuery1);
		expect(isQuery1).assertEqual(true);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0102
	 * @tc.name webgl_test_isQuery
	 * @tc.desc Test isQuery.
	 */
	it('webgl_test_isQuery', 0, async function(done) {
		//initContext();
		console.info("webgltest into isQuery");
		const query = gl2.createQuery();
		gl2.beginQuery(0x8C2F, query);

		const currentQuery = gl2.getQuery(0x8C2F, 0x8865);
		const isQuery1 = gl2.isQuery(currentQuery);
		console.info("webgltest createQuery isQuery1: " + isQuery1);
		expect(isQuery1).assertEqual(true);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0103
	 * @tc.name webgl_test_isQuery_2
	 * @tc.desc Test isQuery.
	 */
	it('webgl_test_isQuery_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into isQuery");
		const query = gl2.createQuery();

		const isQuery1 = gl2.isQuery(query);
		console.info("webgltest createQuery isQuery1: " + isQuery1);
		expect(isQuery1).assertEqual(false);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0104
	 * @tc.name webgl_test_endQuery
	 * @tc.desc Test endQuery.
	 */
	it('webgl_test_endQuery', 0, async function(done) {
		//initContext();
		console.info("webgltest into endQuery");

		var query = gl2.createQuery();
		gl2.beginQuery(0x8C2F, query);

		const currentQuery = gl2.getQuery(0x8C2F, 0x8865);
		const isQuery1 = gl2.isQuery(currentQuery);
		console.info("webgltest createQuery isQuery1: " + isQuery1);
		expect(isQuery1).assertEqual(true);

		gl2.endQuery(0x8C2F);

		const isQuery2 = gl2.isQuery(currentQuery);
		console.info("webgltest createQuery isQuery2: " + isQuery2);
		expect(isQuery2).assertEqual(true);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0105
	 * @tc.name webgl_test_getQueryParameter
	 * @tc.desc Test getQueryParameter.
	 */
	it('webgl_test_getQueryParameter', 0, async function(done) {
		//initContext();
		console.info("webgltest into getQueryParameter");
		var query = gl2.createQuery();
		gl2.beginQuery(0x8C2F, query);

		var currentQuery = gl2.getQuery(0x8C2F, 0x8865);
		const isQuery1 = gl2.isQuery(currentQuery);
		console.info("webgltest createQuery isQuery1: " + isQuery1);
		expect(isQuery1).assertEqual(true);

		// 返回一个GLboolean 指示查询结果是否可用。
		var result = gl2.getQueryParameter(currentQuery, gl2.QUERY_RESULT_AVAILABLE);

		console.info("webgltest createQuery beginQuery getQueryParameter: " + result);
		expect(typeof(result)).assertEqual("boolean");
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0106
	 * @tc.name webgl_test_getQueryParameter2
	 * @tc.desc Test getQueryParameter.
	 */
	it('webgl_test_getQueryParameter2', 0, async function(done) {
		//initContext();
		console.info("webgltest into getQueryParameter");
		var query = gl2.createQuery();
		gl2.beginQuery(0x8C2F, query);

		var currentQuery = gl2.getQuery(0x8C2F, gl2.CURRENT_QUERY);
		const isQuery1 = gl2.isQuery(currentQuery);
		console.info("webgltest createQuery isQuery1: " + isQuery1);
		expect(isQuery1).assertEqual(true);

		// 返回一个GLboolean 指示查询结果是否可用。
		var result = gl2.getQueryParameter(currentQuery, gl2.QUERY_RESULT);

		console.info("webgltest createQuery beginQuery getQueryParameter: " + result);
		expect(typeof(result)).assertEqual("number");
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0107
	 * @tc.name webgl_test_deleteQuery
	 * @tc.desc Test deleteQuery.
	 */
	it('webgl_test_deleteQuery', 0, async function(done) {
		//initContext();
		console.info("webgltest into deleteQuery");

		var query = gl2.createQuery();
		gl2.beginQuery(0x8C2F, query);

		const currentQuery = gl2.getQuery(0x8C2F, 0x8865);
		const isQuery1 = gl2.isQuery(currentQuery);
		console.info("webgltest createQuery isQuery1: " + isQuery1);
		expect(isQuery1).assertEqual(true);

		gl2.deleteQuery(currentQuery);

		const isQuery2 = gl2.isQuery(currentQuery);
		console.info("webgltest createQuery deleteQuery isQuery2: " + isQuery2);

		expect(isQuery2).assertEqual(false);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0108
	 * @tc.name webgl_test_createSampler
	 * @tc.desc Test createSampler.
	 */
	it('webgl_test_createSampler', 0, async function(done) {
		//initContext();
		console.info("webgltest into createSampler");
		const sampler = gl2.createSampler();
		const isSampler1 = gl2.isSampler(sampler);
		console.info("webgltest createSampler isSampler1: " + isSampler1);
		expect(isSampler1).assertEqual(true);

		gl2.bindSampler(0, sampler);
		const isSampler2 = gl2.isSampler(sampler);
		console.info("webgltest createSampler bindSampler isSampler2: " + isSampler2);
		expect(isSampler2).assertEqual(true);

		gl2.deleteSampler(sampler);
		const isSampler3 = gl2.isSampler(sampler);
		console.info("webgltest createSampler bindSampler deleteSampler isSampler3: " + isSampler3);
		expect(isSampler3).assertEqual(false);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0109
	 * @tc.name webgl_test_samplerParameterf
	 * @tc.desc Test samplerParameterf.
	 */
	it('webgl_test_samplerParameterf', 0, async function(done) {
		//initContext();
		console.info("webgltest into samplerParameterf");

		let sampler = gl2.createSampler();
		//        gl2.bindSampler(0, sampler);
		gl2.samplerParameterf(sampler, gl.TEXTURE_MAG_FILTER, gl.NEAREST);

		let errorCode = gl.getError();
		console.info("webgltest samplerParameterf getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		gl2.deleteSampler(sampler);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0110
	 * @tc.name webgl_test_samplerParameteri
	 * @tc.desc Test samplerParameteri.
	 */
	it('webgl_test_samplerParameteri', 0, async function(done) {
		//initContext();
		console.info("webgltest into samplerParameteri");
		let sampler = gl2.createSampler();
		//        gl2.bindSampler(0, sampler);
		//        gl2.samplerParameteri(sampler, 0x884D, 0x2600);
		gl2.samplerParameteri(sampler, gl.TEXTURE_MAG_FILTER, gl.NEAREST);

		let errorCode = gl.getError();
		console.info("webgltest samplerParameteri getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		gl2.deleteSampler(sampler);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0111
	 * @tc.name webgl_test_deleteSampler
	 * @tc.desc Test deleteSampler.
	 */
	it('webgl_test_deleteSampler', 0, async function(done) {
		//initContext();
		console.info("webgltest into deleteSampler");
		const sampler = gl2.createSampler();
		const isSampler1 = gl2.isSampler(sampler);
		console.info("webgltest createSampler isSampler1: " + isSampler1);
		expect(isSampler1).assertEqual(true);

		gl2.bindSampler(0, sampler);
		const isSampler2 = gl2.isSampler(sampler);
		console.info("webgltest createSampler bindSampler isSampler2: " + isSampler2);
		expect(isSampler2).assertEqual(true);

		gl2.deleteSampler(sampler);
		const isSampler3 = gl2.isSampler(sampler);
		console.info("webgltest createSampler bindSampler deleteSampler isSampler3: " + isSampler3);
		expect(isSampler3).assertEqual(false);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0112
	 * @tc.name webgl_test_isSampler
	 * @tc.desc Test isSampler.
	 */
	it('webgl_test_isSampler', 0, async function(done) {
		//initContext();
		console.info("webgltest into isSampler");
		const sampler = gl2.createSampler();
		const isSampler1 = gl2.isSampler(sampler);
		console.info("webgltest createSampler isSampler1: " + isSampler1);
		expect(isSampler1).assertEqual(true);

		gl2.bindSampler(0, sampler);
		const isSampler2 = gl2.isSampler(sampler);
		console.info("webgltest createSampler bindSampler isSampler2: " + isSampler2);
		expect(isSampler2).assertEqual(true);

		gl2.deleteSampler(sampler);
		const isSampler3 = gl2.isSampler(sampler);
		console.info("webgltest createSampler bindSampler deleteSampler isSampler3: " + isSampler3);
		expect(isSampler3).assertEqual(false);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0113
	 * @tc.name webgl_test_isSampler_2
	 * @tc.desc Test isSampler.
	 */
	it('webgl_test_isSampler_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into isSampler");
		const sampler = gl2.createSampler();

		gl2.bindSampler(0, sampler);
		const isSampler2 = gl2.isSampler(sampler);
		console.info("webgltest createSampler bindSampler isSampler2: " + isSampler2);
		expect(isSampler2).assertEqual(true);

		gl2.deleteSampler(sampler);
		const isSampler3 = gl2.isSampler(sampler);
		console.info("webgltest createSampler bindSampler deleteSampler isSampler3: " + isSampler3);
		expect(isSampler3).assertEqual(false);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0114
	 * @tc.name webgl_test_bindSampler
	 * @tc.desc Test bindSampler.
	 */
	it('webgl_test_bindSampler', 0, async function(done) {
		//initContext();
		console.info("webgltest into bindSampler");
		const sampler = gl2.createSampler();
		const isSampler1 = gl2.isSampler(sampler);
		console.info("webgltest createSampler isSampler1: " + isSampler1);
		expect(isSampler1).assertEqual(true);

		gl2.bindSampler(0, sampler);
		const isSampler2 = gl2.isSampler(sampler);
		console.info("webgltest createSampler bindSampler isSampler2: " + isSampler2);
		expect(isSampler2).assertEqual(true);

		gl2.deleteSampler(sampler);
		const isSampler3 = gl2.isSampler(sampler);
		console.info("webgltest createSampler bindSampler deleteSampler isSampler3: " + isSampler3);
		expect(isSampler3).assertEqual(false);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0115
	 * @tc.name webgl_test_getSamplerParameter_1
	 * @tc.desc Test getSamplerParameter.
	 */
	it('webgl_test_getSamplerParameter_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into getSamplerParameter");

		let sampler = gl2.createSampler();
		//        const samplerParameter = gl2.getSamplerParameter(sampler, gl.TEXTURE_COMPARE_FUNC);

		const samplerParameter = gl2.getSamplerParameter(sampler, 0x884D);

		console.info("webgltest createSampler getSamplerParameter_1: " + samplerParameter);
		expect(typeof(samplerParameter)).assertEqual("number");
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0116
	 * @tc.name webgl_test_getSamplerParameter_2
	 * @tc.desc Test getSamplerParameter.
	 */
	it('webgl_test_getSamplerParameter_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into getSamplerParameter");

		let sampler = gl2.createSampler();
		const samplerParameter = gl2.getSamplerParameter(sampler, 0x884C);

		console.info("webgltest createSampler getSamplerParameter: " + samplerParameter);
		expect(typeof(samplerParameter)).assertEqual("number");
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0117
	 * @tc.name webgl_test_getSamplerParameter_3
	 * @tc.desc Test getSamplerParameter.
	 */
	it('webgl_test_getSamplerParameter_3', 0, async function(done) {
		//initContext();
		console.info("webgltest into getSamplerParameter");

		let sampler = gl2.createSampler();
		const samplerParameter = gl2.getSamplerParameter(sampler, gl.TEXTURE_MAG_FILTER);

		console.info("webgltest createSampler getSamplerParameter: " + samplerParameter);
		expect(typeof(samplerParameter)).assertEqual("number");
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0118
	 * @tc.name webgl_test_getSamplerParameter_4
	 * @tc.desc Test getSamplerParameter.
	 */
	it('webgl_test_getSamplerParameter_4', 0, async function(done) {
		//initContext();
		console.info("webgltest into getSamplerParameter");

		let sampler = gl2.createSampler();
		const samplerParameter = gl2.getSamplerParameter(sampler, 0x813B);

		console.info("webgltest createSampler getSamplerParameter: " + samplerParameter);
		expect(typeof(samplerParameter)).assertEqual("number");
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0119
	 * @tc.name webgl_test_getSamplerParameter_5
	 * @tc.desc Test getSamplerParameter.
	 */
	it('webgl_test_getSamplerParameter_5', 0, async function(done) {
		//initContext();
		console.info("webgltest into getSamplerParameter");

		let sampler = gl2.createSampler();
		const samplerParameter = gl2.getSamplerParameter(sampler, gl.TEXTURE_MIN_FILTER);

		console.info("webgltest createSampler getSamplerParameter: " + samplerParameter);
		expect(typeof(samplerParameter)).assertEqual("number");
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0120
	 * @tc.name webgl_test_getSamplerParameter_6
	 * @tc.desc Test getSamplerParameter.
	 */
	it('webgl_test_getSamplerParameter_6', 0, async function(done) {
		//initContext();
		console.info("webgltest into getSamplerParameter");

		let sampler = gl2.createSampler();
		const samplerParameter = gl2.getSamplerParameter(sampler, 0x813A);

		console.info("webgltest createSampler getSamplerParameter: " + samplerParameter);
		expect(typeof(samplerParameter)).assertEqual("number");
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0121
	 * @tc.name webgl_test_getSamplerParameter_7
	 * @tc.desc Test getSamplerParameter.
	 */
	it('webgl_test_getSamplerParameter_7', 0, async function(done) {
		//initContext();
		console.info("webgltest into getSamplerParameter");

		let sampler = gl2.createSampler();
		const samplerParameter = gl2.getSamplerParameter(sampler, 0x8072);

		console.info("webgltest createSampler getSamplerParameter: " + samplerParameter);
		expect(typeof(samplerParameter)).assertEqual("number");
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0122
	 * @tc.name webgl_test_getSamplerParameter_8
	 * @tc.desc Test getSamplerParameter.
	 */
	it('webgl_test_getSamplerParameter_8', 0, async function(done) {
		//initContext();
		console.info("webgltest into getSamplerParameter");

		let sampler = gl2.createSampler();
		const samplerParameter = gl2.getSamplerParameter(sampler, gl.TEXTURE_WRAP_S);

		console.info("webgltest createSampler getSamplerParameter: " + samplerParameter);
		expect(typeof(samplerParameter)).assertEqual("number");
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0123
	 * @tc.name webgl_test_fenceSync
	 * @tc.desc Test fenceSync.
	 */
	it('webgl_test_fenceSync', 0, async function(done) {
		//initContext();
		console.info("webgltest into fenceSync");

		let sync = gl2.fenceSync(0x9117, 0);
		let status = gl2.clientWaitSync(sync, 0, 0);
		gl2.waitSync(sync, 0, -1);
		const isSync1 = gl2.isSync(sync);
		console.info("webgltest fenceSync isSync1: " + isSync1);
		expect(isSync1).assertEqual(true);

		gl2.deleteSync(sync);

		const isSync2 = gl2.isSync(sync);
		console.info("webgltest fenceSync deleteSync isSync2: " + isSync2);
		expect(isSync2).assertEqual(false);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0124
	 * @tc.name webgl_test_clientWaitSync
	 * @tc.desc Test clientWaitSync.
	 */
	it('webgl_test_clientWaitSync', 0, async function(done) {
		//initContext();
		console.info("webgltest into clientWaitSync");
		let sync = gl2.fenceSync(0x9117, 0);
		let status = gl2.clientWaitSync(sync, 0, 0);
		gl2.waitSync(sync, 0, -1);
		const isSync1 = gl2.isSync(sync);
		console.info("webgltest fenceSync isSync1: " + isSync1);
		expect(isSync1).assertEqual(true);

		console.info("webgltest fenceSync clientWaitSync: " + status);

		expect(status).assertEqual(gl2.ALREADY_SIGNALED || gl2.TIMEOUT_EXPIRED || gl
			.CONDITION_SATISFIED ||
			gl.WAIT_FAILED);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0125
	 * @tc.name webgl_test_waitSync
	 * @tc.desc Test waitSync.
	 */
	it('webgl_test_waitSync', 0, async function(done) {
		//initContext();
		console.info("webgltest into waitSync");

		let sync = gl2.fenceSync(gl.SYNC_GPU_COMMANDS_COMPLETE, 0);
		gl2.waitSync(sync, 0, gl.TIMEOUT_IGNORED);

		let errorCode = gl.getError();
		console.info("webgltest framebufferTexture2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0126
	 * @tc.name webgl_test_isSync
	 * @tc.desc Test isSync.
	 */
	it('webgl_test_isSync', 0, async function(done) {
		//initContext();
		console.info("webgltest into isSync");
		let sync = gl2.fenceSync(0x9117, 0);
		let status = gl2.clientWaitSync(sync, 0, 0);
		gl2.waitSync(sync, 0, -1);
		const isSync1 = gl2.isSync(sync);
		console.info("webgltest fenceSync isSync1: " + isSync1);
		expect(isSync1).assertEqual(true);

		gl2.deleteSync(sync);

		const isSync2 = gl2.isSync(sync);
		console.info("webgltest fenceSync deleteSync isSync2: " + isSync2);
		expect(isSync2).assertEqual(false);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0127
	 * @tc.name webgl_test_getSyncParameter
	 * @tc.desc Test getSyncParameter.
	 */
	it('webgl_test_getSyncParameter', 0, async function(done) {
		//initContext();
		console.info("webgltest into getSyncParameter");

		console.info("webgltest into isSync");
		let sync = gl2.fenceSync(0x9117, 0);
		let status = gl2.clientWaitSync(sync, 0, 0);
		gl2.waitSync(sync, 0, -1);
		const isSync1 = gl2.isSync(sync);
		console.info("webgltest fenceSync isSync1: " + isSync1);
		expect(isSync1).assertEqual(true);
		const syncParameter = gl2.getSyncParameter(sync, 0x9112);

		console.info("webgltest fenceSync getSyncParameter: " + syncParameter);
		expect(syncParameter).assertEqual(0x9116);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0128
	 * @tc.name webgl_test_getSyncParameter2
	 * @tc.desc Test getSyncParameter.
	 */
	it('webgl_test_getSyncParameter2', 0, async function(done) {
		//initContext();
		console.info("webgltest into getSyncParameter");

		console.info("webgltest into isSync");
		let sync = gl2.fenceSync(0x9117, 0);
		let status = gl2.clientWaitSync(sync, 0, 0);
		gl2.waitSync(sync, 0, -1);
		const isSync1 = gl2.isSync(sync);
		console.info("webgltest fenceSync isSync1: " + isSync1);
		expect(isSync1).assertEqual(true);

		const syncParameter = gl2.getSyncParameter(sync, 0x9114);

		console.info("webgltest fenceSync getSyncParameter: " + syncParameter);
		let syncParameterFlag = false;

		if (syncParameter == gl2.SIGNALED || syncParameter == gl2.UNSIGNALED) {
			syncParameterFlag = true;
		}
		expect(syncParameterFlag).assertEqual(true);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0129
	 * @tc.name webgl_test_getSyncParameter3
	 * @tc.desc Test getSyncParameter.
	 */
	it('webgl_test_getSyncParameter3', 0, async function(done) {
		//initContext();
		console.info("webgltest into getSyncParameter");

		let sync = gl2.fenceSync(0x9117, 0);
		let status = gl2.clientWaitSync(sync, 0, 0);
		gl2.waitSync(sync, 0, -1);
		const isSync1 = gl2.isSync(sync);
		console.info("webgltest fenceSync isSync1: " + isSync1);
		expect(isSync1).assertEqual(true);
		const syncParameter = gl2.getSyncParameter(sync, 0x9113);

		console.info("webgltest fenceSync getSyncParameter: " + syncParameter);
		expect(syncParameter).assertEqual(0x9117);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0130
	 * @tc.name webgl_test_getSyncParameter4
	 * @tc.desc Test getSyncParameter.
	 */
	it('webgl_test_getSyncParameter4', 0, async function(done) {
		//initContext();
		console.info("webgltest into getSyncParameter");

		let sync = gl2.fenceSync(0x9117, 0);
		let status = gl2.clientWaitSync(sync, 0, 0);
		gl2.waitSync(sync, 0, -1);
		const isSync1 = gl2.isSync(sync);
		console.info("webgltest fenceSync isSync1: " + isSync1);
		expect(isSync1).assertEqual(true);
		const syncParameter = gl2.getSyncParameter(sync, 0x9115);

		console.info("webgltest fenceSync getSyncParameter: " + syncParameter);
		expect(syncParameter).assertEqual(0);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0131
	 * @tc.name webgl_test_deleteSync
	 * @tc.desc Test deleteSync.
	 */
	it('webgl_test_deleteSync', 0, async function(done) {
		//initContext();
		console.info("webgltest into deleteSync");
		let sync = gl2.fenceSync(0x9117, 0);
		let status = gl2.clientWaitSync(sync, 0, 0);
		gl2.waitSync(sync, 0, -1);
		const isSync1 = gl2.isSync(sync);
		console.info("webgltest fenceSync isSync1: " + isSync1);
		expect(isSync1).assertEqual(true);

		gl2.deleteSync(sync);

		const isSync2 = gl2.isSync(sync);
		console.info("webgltest fenceSync deleteSync isSync2: " + isSync2);
		expect(isSync2).assertEqual(false);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0132
	 * @tc.name webgl_test_createTransformFeedback
	 * @tc.desc Test createTransformFeedback.
	 */
	it('webgl_test_createTransformFeedback', 0, async function(done) {
		//initContext();
		console.info("webgltest into createTransformFeedback");

		const transformFeedback = gl2.createTransformFeedback();


		console.info("webgltest into createTransformFeedback: " + transformFeedback)

		gl2.bindTransformFeedback(gl2.TRANSFORM_FEEDBACK, transformFeedback);

		gl2.beginTransformFeedback(gl2.TRIANGLES);

		const isTransformFeedback2 = gl2.isTransformFeedback(transformFeedback);

		console.info("BindTransformFeedback  : " + isTransformFeedback2);
		expect(isTransformFeedback2).assertEqual(true);

		gl2.deleteTransformFeedback(transformFeedback);
		const isTransformFeedback3 = gl2.isTransformFeedback(transformFeedback);
		console.info("webgltest createTransformFeedback bindTransformFeedback " +
			"deleteTransformFeedback isTransformFeedback3: " + isTransformFeedback3);
		expect(isTransformFeedback3).assertEqual(false);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0133
	 * @tc.name webgl_test_isTransformFeedback
	 * @tc.desc Test isTransformFeedback.
	 */
	it('webgl_test_isTransformFeedback', 0, async function(done) {
		//initContext();
		console.info("webgltest into isTransformFeedback");
		let transformFeedback = gl2.createTransformFeedback();
		const isTransformFeedback1 = gl2.isTransformFeedback(transformFeedback);
		console.info("webgltest createTransformFeedback isTransformFeedback1: " +
			isTransformFeedback1);
		expect(isTransformFeedback1).assertEqual(false);

		gl2.bindTransformFeedback(gl2.TRANSFORM_FEEDBACK, transformFeedback);

		const isTransformFeedback2 = gl2.isTransformFeedback(transformFeedback);
		console.info(
			"webgltest createTransformFeedback bindTransformFeedback isTransformFeedback2: " +
			isTransformFeedback2);
		expect(isTransformFeedback2).assertEqual(true);

		gl2.deleteTransformFeedback(transformFeedback);
		const isTransformFeedback3 = gl2.isTransformFeedback(transformFeedback);
		console.info("webgltest createTransformFeedback bindTransformFeedback " +
			"deleteTransformFeedback isTransformFeedback3: " + isTransformFeedback3);
		expect(isTransformFeedback3).assertEqual(false);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0134
	 * @tc.name webgl_test_bindTransformFeedback
	 * @tc.desc Test bindTransformFeedback.
	 */
	it('webgl_test_bindTransformFeedback', 0, async function(done) {
		//initContext();
		console.info("webgltest into bindTransformFeedback");
		let transformFeedback = gl2.createTransformFeedback();
		const isTransformFeedback1 = gl2.isTransformFeedback(transformFeedback);
		console.info("webgltest createTransformFeedback isTransformFeedback1: " +
			isTransformFeedback1);
		expect(isTransformFeedback1).assertEqual(false);

		gl2.bindTransformFeedback(gl2.TRANSFORM_FEEDBACK, transformFeedback);

		const isTransformFeedback2 = gl2.isTransformFeedback(transformFeedback);
		console.info(
			"webgltest createTransformFeedback bindTransformFeedback isTransformFeedback2: " +
			isTransformFeedback2);
		expect(isTransformFeedback2).assertEqual(true);

		gl2.deleteTransformFeedback(transformFeedback);
		const isTransformFeedback3 = gl2.isTransformFeedback(transformFeedback);
		console.info(
			"webgltest createTransformFeedback bindTransformFeedback deleteTransformFeedback " +
			"isTransformFeedback3: " + isTransformFeedback3);
		expect(isTransformFeedback3).assertEqual(false);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0135
	 * @tc.name webgl_test_beginTransformFeedback
	 * @tc.desc Test beginTransformFeedback.
	 */
	it('webgl_test_beginTransformFeedback', 0, async function(done) {
		//initContext();
		console.info("webgltest into beginTransformFeedback");

		let transformFeedback = gl2.createTransformFeedback();
		//        gl2.bindTransformFeedback(gl2.TRANSFORM_FEEDBACK, transformFeedback);
		gl2.beginTransformFeedback(gl2.TRIANGLES);


		let errorCode = gl.getError();
		console.info("webgltest framebufferTexture2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0136
	 * @tc.name webgl_test_endTransformFeedback
	 * @tc.desc Test endTransformFeedback.
	 */
	it('webgl_test_endTransformFeedback', 0, async function(done) {
		//initContext();
		console.info("webgltest into endTransformFeedback");
		let transformFeedback = gl2.createTransformFeedback();
		gl2.bindTransformFeedback(gl2.TRANSFORM_FEEDBACK, transformFeedback);
		gl2.beginTransformFeedback(gl.TRIANGLES);
		gl.drawArrays(gl.TRIANGLES, 0, 3);
		gl2.endTransformFeedback();
		let errorCode = gl.getError();
		console.info("webgltest framebufferTexture2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0137
	 * @tc.name webgl_test_deleteTransformFeedback
	 * @tc.desc Test deleteTransformFeedback.
	 */
	it('webgl_test_deleteTransformFeedback', 0, async function(done) {
		//initContext();
		console.info("webgltest into deleteTransformFeedback");
		const transformFeedback = gl2.createTransformFeedback();

		gl2.bindTransformFeedback(gl2.TRANSFORM_FEEDBACK, transformFeedback);

		const isTransformFeedback2 = gl2.isTransformFeedback(transformFeedback);
		console.info(
			"webgltest createTransformFeedback bindTransformFeedback isTransformFeedback2: " +
			isTransformFeedback2);
		expect(isTransformFeedback2).assertEqual(true);

		gl2.deleteTransformFeedback(transformFeedback);
		const isTransformFeedback3 = gl2.isTransformFeedback(transformFeedback);
		console.info(
			"webgltest createTransformFeedback bindTransformFeedback deleteTransformFeedback " +
			"isTransformFeedback3: " + isTransformFeedback3);
		expect(isTransformFeedback3).assertEqual(false);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0138
	 * @tc.name webgl_test_getFragDataLocation
	 * @tc.desc Test getFragDataLocation.
	 */
	it('webgl_test_getFragDataLocation', 0, async function(done) {
		//initContext();
		console.info("webgltest into getFragDataLocation");

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

		gl2.getFragDataLocation(programObj, 'a_Position');

		let errorCode = gl.getError();
		console.info("webgltest getFragDataLocation getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0139
	 * @tc.name webgl_test_uniform1ui
	 * @tc.desc Test uniform1ui.
	 */
	it('webgl_test_uniform1ui', 0, async function(done) {
		//initContext();
		console.info("webgltest into uniform1ui");

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

		const location = gl.getUniformLocation(programObj, "a_Position");
		gl2.uniform1ui(location, 1.8);

		let errorCode = gl.getError();
		console.info("webgltest uniform1ui getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0140
	 * @tc.name webgl_test_uniform1ui_2
	 * @tc.desc Test uniform1ui.
	 */
	it('webgl_test_uniform1ui_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into uniform1ui");

		const programObj = gl.createProgram();
		console.log("testUseProgram has failed for " + programObj)

		const location = gl.getUniformLocation(programObj, "v_color");
		gl2.uniform1ui(location, 1.8);

		let errorCode = gl.getError();
		console.info("webgltest uniform1ui getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0141
	 * @tc.name webgl_test_uniform2ui
	 * @tc.desc Test uniform2ui.
	 */
	it('webgl_test_uniform2ui', 0, async function(done) {
		//initContext();
		console.info("webgltest into uniform2ui");

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

		const location = gl.getUniformLocation(programObj, "a_Position");
		const a = gl2.uniform2ui(location, 2.8, 2.8);

		let errorCode = gl.getError();
		console.info("webgltest uniform2ui getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0142
	 * @tc.name webgl_test_uniform2ui_2
	 * @tc.desc Test uniform2ui.
	 */
	it('webgl_test_uniform2ui_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into uniform2ui");

		const programObj = gl.createProgram();
		console.log("testUseProgram has failed for " + programObj)

		const location = gl.getUniformLocation(programObj, "v_color");
		const a = gl2.uniform2ui(location, 2.8, 2.8);

		let errorCode = gl.getError();
		console.info("webgltest uniform2ui getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0143
	 * @tc.name webgl_test_uniform3ui
	 * @tc.desc Test uniform3ui.
	 */
	it('webgl_test_uniform3ui', 0, async function(done) {
		//initContext();
		console.info("webgltest into uniform3ui");

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

		const location = gl.getUniformLocation(programObj, "a_Position");
		const a = gl2.uniform3ui(location, 3.8, 3.8, 3.8);

		let errorCode = gl.getError();
		console.info("webgltest uniform3ui getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0144
	 * @tc.name webgl_test_uniform3ui_2
	 * @tc.desc Test uniform3ui.
	 */
	it('webgl_test_uniform3ui_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into uniform3ui");

		const programObj = gl.createProgram();
		console.log("testUseProgram has failed for " + programObj)

		const location = gl.getUniformLocation(programObj, "v_color");
		const a = gl2.uniform3ui(location, 3.8, 3.8, 3.8);

		let errorCode = gl.getError();
		console.info("webgltest uniform3ui getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0145
	 * @tc.name webgl_test_uniform4ui
	 * @tc.desc Test uniform4ui.
	 */
	it('webgl_test_uniform4ui', 0, async function(done) {
		//initContext();
		console.info("webgltest into uniform4ui");

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

		const location = gl.getUniformLocation(programObj, "a_Position");
		const a = gl2.uniform4ui(location, 4.8, 4.8, 4.8, 4.8);

		let errorCode = gl.getError();
		console.info("webgltest uniform4ui getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0146
	 * @tc.name webgl_test_uniform4ui_2
	 * @tc.desc Test uniform4ui.
	 */
	it('webgl_test_uniform4ui_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into uniform4ui");

		const programObj = gl.createProgram();
		console.log("testUseProgram has failed for " + programObj)

		const location = gl.getUniformLocation(programObj, "v_color");
		const a = gl2.uniform4ui(location, 4.8, 4.8, 4.8, 4.8);

		let errorCode = gl.getError();
		console.info("webgltest uniform4ui getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0147
	 * @tc.name webgl_test_uniform1uiv
	 * @tc.desc Test uniform1uiv.
	 */
	it('webgl_test_uniform1uiv', 0, async function(done) {
		//initContext();
		console.info("webgltest into uniform1uiv");

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
		const location = gl.getUniformLocation(programObj, "a_Position");
		const a = gl2.uniform1uiv(location, float1, 1, 255);

		let errorCode = gl.getError();
		console.info("webgltest uniform1uiv getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0148
	 * @tc.name webgl_test_uniform1uiv_2
	 * @tc.desc Test uniform1uiv.
	 */
	it('webgl_test_uniform1uiv_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into uniform1uiv");

		const programObj = gl.createProgram();

		const location = gl.getUniformLocation(programObj, "v_Color");
		const a = gl2.uniform1uiv(location, float1, 1, 255);

		let errorCode = gl.getError();
		console.info("webgltest uniform1uiv getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0149
	 * @tc.name webgl_test_uniform2uiv
	 * @tc.desc Test uniform2uiv.
	 */
	it('webgl_test_uniform2uiv', 0, async function(done) {
		//initContext();
		console.info("webgltest into uniform2uiv");

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

		const location = gl.getUniformLocation(programObj, "a_Position");
		const a = gl2.uniform2uiv(location, float1, 2, 255);

		let errorCode = gl.getError();
		console.info("webgltest uniform2uiv getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0150
	 * @tc.name webgl_test_uniform2uiv_2
	 * @tc.desc Test uniform2uiv.
	 */
	it('webgl_test_uniform2uiv_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into uniform2uiv");


		const programObj = gl.createProgram();


		const location = gl.getUniformLocation(programObj, "v_Color");
		const a = gl2.uniform2uiv(location, float2, 0, 255);

		let errorCode = gl.getError();
		console.info("webgltest uniform2uiv getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0151
	 * @tc.name webgl_test_uniform3uiv
	 * @tc.desc Test uniform3uiv.
	 */
	it('webgl_test_uniform3uiv', 0, async function(done) {
		//initContext();
		console.info("webgltest into uniform3uiv");

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

		const location = gl.getUniformLocation(programObj, "a_Position");
		const a = gl2.uniform3uiv(location, float1, 3, 255);

		let errorCode = gl.getError();
		console.info("webgltest uniform3uiv getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0152
	 * @tc.name webgl_test_uniform3uiv_3
	 * @tc.desc Test uniform3uiv.
	 */
	it('webgl_test_uniform3uiv_3', 0, async function(done) {
		//initContext();
		console.info("webgltest into uniform3uiv");


		const programObj = gl.createProgram();


		const location = gl.getUniformLocation(programObj, "v_Color");
		const a = gl2.uniform3uiv(location, float2, 0, 255);

		let errorCode = gl.getError();
		console.info("webgltest uniform3uiv getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0153
	 * @tc.name webgl_test_uniform4uiv
	 * @tc.desc Test uniform4uiv.
	 */
	it('webgl_test_uniform4uiv', 0, async function(done) {
		//initContext();
		console.info("webgltest into uniform4uiv");

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

		const location = gl.getUniformLocation(programObj, "a_Position");
		const a = gl2.uniform4uiv(location, float1, 0, 255);

		let errorCode = gl.getError();
		console.info("webgltest uniform4uiv getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0154
	 * @tc.name webgl_test_uniform4uiv_2
	 * @tc.desc Test uniform4uiv.
	 */
	it('webgl_test_uniform4uiv_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into uniform4uiv");


		const programObj = gl.createProgram();

		const location = gl.getUniformLocation(programObj, "v_Color");
		const a = gl2.uniform4uiv(location, float2, 0, 255);

		let errorCode = gl.getError();
		console.info("webgltest uniform4uiv getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0155
	 * @tc.name webgl_test_uniformMatrix3x2fv
	 * @tc.desc Test uniformMatrix3x2fv.
	 */
	it('webgl_test_uniformMatrix3x2fv', 0, async function(done) {
		//initContext();
		console.info("webgltest into uniformMatrix3x2fv");

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

		const location = gl.getUniformLocation(programObj, "a_Position");
		const a = gl2.uniformMatrix3x2fv(location, false, float1, 0, 255);

		let errorCode = gl.getError();
		console.info("webgltest uniformMatrix3x2fv getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0156
	 * @tc.name webgl_test_uniformMatrix3x2fv_2
	 * @tc.desc Test uniformMatrix3x2fv.
	 */
	it('webgl_test_uniformMatrix3x2fv_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into uniformMatrix3x2fv");


		const programObj = gl.createProgram();


		const location = gl.getUniformLocation(programObj, "v_Color");
		const a = gl2.uniformMatrix3x2fv(location, false, float2, 0, 255);

		let errorCode = gl.getError();
		console.info("webgltest uniformMatrix3x2fv getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0157
	 * @tc.name webgl_test_uniformMatrix4x2fv
	 * @tc.desc Test uniformMatrix4x2fv.
	 */
	it('webgl_test_uniformMatrix4x2fv', 0, async function(done) {
		//initContext();
		console.info("webgltest into uniformMatrix4x2fv");

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
		const location = gl.getUniformLocation(programObj, "a_Position");
		const a = gl2.uniformMatrix4x2fv(location, false, float1, 0, 255);

		let errorCode = gl.getError();
		console.info("webgltest uniformMatrix4x2fv getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0158
	 * @tc.name webgl_test_uniformMatrix4x2fv_2
	 * @tc.desc Test uniformMatrix4x2fv.
	 */
	it('webgl_test_uniformMatrix4x2fv_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into uniformMatrix4x2fv");


		const programObj = gl.createProgram();

		const location = gl.getUniformLocation(programObj, "v_Color");
		const a = gl2.uniformMatrix4x2fv(location, false, float2, 0, 255);

		let errorCode = gl.getError();
		console.info("webgltest uniformMatrix4x2fv getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0159
	 * @tc.name webgl_test_uniformMatrix2x3fv
	 * @tc.desc Test uniformMatrix2x3fv.
	 */
	it('webgl_test_uniformMatrix2x3fv', 0, async function(done) {
		//initContext();
		console.info("webgltest into uniformMatrix2x3fv");

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
		const location = gl.getUniformLocation(programObj, "a_Position");
		const a = gl2.uniformMatrix2x3fv(location, false, float1, 0, 255);

		let errorCode = gl.getError();
		console.info("webgltest uniformMatrix2x3fv getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0160
	 * @tc.name webgl_test_uniformMatrix2x3fv_2
	 * @tc.desc Test uniformMatrix2x3fv.
	 */
	it('webgl_test_uniformMatrix2x3fv_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into uniformMatrix2x3fv");

		const programObj = gl.createProgram();

		const location = gl.getUniformLocation(programObj, "v_Color");
		const a = gl2.uniformMatrix2x3fv(location, false, float2, 0, 255);

		let errorCode = gl.getError();
		console.info("webgltest uniformMatrix2x3fv getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0161
	 * @tc.name webgl_test_uniformMatrix4x3fv
	 * @tc.desc Test uniformMatrix4x3fv.
	 */
	it('webgl_test_uniformMatrix4x3fv', 0, async function(done) {
		//initContext();
		console.info("webgltest into uniformMatrix4x3fv");

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
		const location = gl.getUniformLocation(programObj, "a_Position");
		const a = gl2.uniformMatrix4x3fv(location, false, float1, 0, 255);

		let errorCode = gl.getError();
		console.info("webgltest uniformMatrix4x3fv getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0162
	 * @tc.name webgl_test_uniformMatrix4x3fv_2
	 * @tc.desc Test uniformMatrix4x3fv.
	 */
	it('webgl_test_uniformMatrix4x3fv_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into uniformMatrix4x3fv");


		const programObj = gl.createProgram();

		const location = gl.getUniformLocation(programObj, "v_Color");
		const a = gl2.uniformMatrix4x3fv(location, false, float2, 0, 255);

		let errorCode = gl.getError();
		console.info("webgltest uniformMatrix4x3fv getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0163
	 * @tc.name webgl_test_uniformMatrix2x4fv
	 * @tc.desc Test uniformMatrix2x4fv.
	 */
	it('webgl_test_uniformMatrix2x4fv', 0, async function(done) {
		//initContext();
		console.info("webgltest into uniformMatrix2x4fv");

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
		const location = gl.getUniformLocation(programObj, "a_Position");
		const a = gl2.uniformMatrix2x4fv(location, false, float1, 0, 255);

		let errorCode = gl.getError();
		console.info("webgltest uniformMatrix2x4fv getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0164
	 * @tc.name webgl_test_uniformMatrix2x4fv_2
	 * @tc.desc Test uniformMatrix2x4fv.
	 */
	it('webgl_test_uniformMatrix2x4fv_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into uniformMatrix2x4fv");


		const programObj = gl.createProgram();

		const location = gl.getUniformLocation(programObj, "v_Color");
		const a = gl2.uniformMatrix2x4fv(location, false, float2, 0, 255);

		let errorCode = gl.getError();
		console.info("webgltest uniformMatrix2x4fv getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0165
	 * @tc.name webgl_test_uniformMatrix3x4fv
	 * @tc.desc Test uniformMatrix3x4fv.
	 */
	it('webgl_test_uniformMatrix3x4fv', 0, async function(done) {
		//initContext();
		console.info("webgltest into uniformMatrix3x4fv");

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
		const location = gl.getUniformLocation(programObj, "a_Position");
		const a = gl2.uniformMatrix3x4fv(location, false, float1, 0, 255);

		let errorCode = gl.getError();
		console.info("webgltest uniformMatrix3x4fv getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0166
	 * @tc.name webgl_test_uniformMatrix3x4fv_2
	 * @tc.desc Test uniformMatrix3x4fv.
	 */
	it('webgl_test_uniformMatrix3x4fv_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into uniformMatrix3x4fv");


		const programObj = gl.createProgram();

		const location = gl.getUniformLocation(programObj, "v_Color");
		const a = gl2.uniformMatrix3x4fv(location, false, float2, 0, 255);

		let errorCode = gl.getError();
		console.info("webgltest uniformMatrix3x4fv getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0167
	 * @tc.name webgl_test_vertexAttribI4i
	 * @tc.desc Test vertexAttribI4i.
	 */
	it('webgl_test_vertexAttribI4i', 0, async function(done) {
		//initContext();
		console.info("webgltest into vertexAttribI4i");

		const a = gl2.vertexAttribI4i(1, 10, 11, 12, 13);

		let errorCode = gl.getError();
		console.info("webgltest vertexAttribI4i getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0168
	 * @tc.name webgl_test_vertexAttribI4iv
	 * @tc.desc Test vertexAttribI4iv.
	 */
	it('webgl_test_vertexAttribI4iv', 0, async function(done) {
		//initContext();
		console.info("webgltest into vertexAttribI4iv");

		const a = gl2.vertexAttribI4iv(1, int1);

		let errorCode = gl.getError();
		console.info("webgltest vertexAttribI4iv getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0169
	 * @tc.name webgl_test_vertexAttribI4iv_2
	 * @tc.desc Test vertexAttribI4iv.
	 */
	it('webgl_test_vertexAttribI4iv_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into vertexAttribI4iv");

		const a = gl2.vertexAttribI4iv(1, int2);

		let errorCode = gl.getError();
		console.info("webgltest vertexAttribI4iv getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0170
	 * @tc.name webgl_test_vertexAttribI4ui
	 * @tc.desc Test vertexAttribI4ui.
	 */
	it('webgl_test_vertexAttribI4ui', 0, async function(done) {
		//initContext();
		console.info("webgltest into vertexAttribI4ui");

		const a = gl2.vertexAttribI4ui(1, 10, 11, 12, 13);

		let errorCode = gl.getError();
		console.info("webgltest vertexAttribI4ui getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0171
	 * @tc.name webgl_test_vertexAttribI4uiv
	 * @tc.desc Test vertexAttribI4uiv.
	 */
	it('webgl_test_vertexAttribI4uiv', 0, async function(done) {
		//initContext();
		console.info("webgltest into vertexAttribI4uiv");

		const a = gl2.vertexAttribI4uiv(1, uint1);

		let errorCode = gl.getError();
		console.info("webgltest vertexAttribI4uiv getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0172
	 * @tc.name webgl_test_vertexAttribI4uiv_2
	 * @tc.desc Test vertexAttribI4uiv.
	 */
	it('webgl_test_vertexAttribI4uiv_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into vertexAttribI4uiv");

		const a = gl2.vertexAttribI4uiv(1, uint2);

		let errorCode = gl.getError();
		console.info("webgltest vertexAttribI4uiv getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0173
	 * @tc.name webgl_test_vertexAttribIPointer
	 * @tc.desc Test vertexAttribIPointer.
	 */
	it('webgl_test_vertexAttribIPointer', 0, async function(done) {
		//initContext();
		console.info("webgltest into vertexAttribIPointer");

		const a = gl2.vertexAttribIPointer(2, 4, gl.UNSIGNED_BYTE, 20, 16);

		let errorCode = gl.getError();
		console.info("webgltest vertexAttribIPointer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0174
	 * @tc.name webgl_test_vertexAttribDivisor
	 * @tc.desc Test vertexAttribDivisor.
	 */
	it('webgl_test_vertexAttribDivisor', 0, async function(done) {
		//initContext();
		console.info("webgltest into vertexAttribDivisor");

		const a = gl2.vertexAttribDivisor(0, 2);

		let errorCode = gl.getError();
		console.info("webgltest vertexAttribDivisor getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0175
	 * @tc.name webgl_test_vertexAttribDivisor_2
	 * @tc.desc Test vertexAttribDivisor.
	 */
	it('webgl_test_vertexAttribDivisor_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into vertexAttribDivisor");

		const a = gl2.vertexAttribDivisor(0, "2");

		let errorCode = gl.getError();
		console.info("webgltest vertexAttribDivisor getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0176
	 * @tc.name webgl_test_drawArraysInstanced
	 * @tc.desc Test drawArraysInstanced.
	 */
	it('webgl_test_drawArraysInstanced', 0, async function(done) {
		//initContext();
		console.info("webgltest into drawArraysInstanced");

		const a = gl2.drawArraysInstanced(gl.POINTS, 0, 8, 4);

		let errorCode = gl.getError();
		console.info("webgltest drawArraysInstanced getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0177
	 * @tc.name webgl_test_drawElementsInstanced
	 * @tc.desc Test drawElementsInstanced.
	 */
	it('webgl_test_drawElementsInstanced', 0, async function(done) {
		//initContext();
		console.info("webgltest into drawElementsInstanced");

		const a = gl2.drawElementsInstanced(gl.POINTS, 2, gl.UNSIGNED_SHORT, 0, 4);

		let errorCode = gl.getError();
		console.info("webgltest drawElementsInstanced getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_VALUE);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0178
	 * @tc.name webgl_test_drawRangeElements
	 * @tc.desc Test drawRangeElements.
	 */
	it('webgl_test_drawRangeElements', 0, async function(done) {
		//initContext();
		console.info("webgltest into drawRangeElements");

		const a = gl2.drawRangeElements(gl.POINTS, 0, 7, 8, gl.UNSIGNED_BYTE, 0);

		let errorCode = gl.getError();
		console.info("webgltest drawRangeElements getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_VALUE);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0179
	 * @tc.name testFrontFaceFirst
	 * @tc.desc Test frontFace.
	 */
	it('testFrontFaceFirst', 0, async function(done) {
		//initContext();
		console.info('jsWebGL frontFace test start ...' + JSON.stringify(gl));
		gl.enable(gl.CULL_FACE);
		gl.cullFace(gl.FRONT_AND_BACK);
		gl.frontFace(gl.CW);
		const frontFaceParameter = gl.getParameter(gl.CULL_FACE_MODE);
		console.info('frontFace -> frontFaceParameter' + frontFaceParameter);
		gl.disable(gl.CULL_FACE);
		expect(frontFaceParameter).assertEqual(gl.FRONT_AND_BACK);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0180
	 * @tc.name testFrontFace_Third
	 * @tc.desc Test frontFace.
	 */
	it('testFrontFace_Third', 0, async function(done) {
		//initContext();
		console.info('jsWebGL frontFace test start ...' + JSON.stringify(gl));
		gl.enable(gl.CULL_FACE);
		gl.cullFace(gl.BACK);
		gl.frontFace(gl.CW);
		const frontFaceParameter = gl.getParameter(gl.CULL_FACE_MODE);
		console.info('frontFace -> frontFaceParameter' + frontFaceParameter);
		gl.disable(gl.CULL_FACE);
		expect(frontFaceParameter).assertEqual(gl.BACK);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0181
	 * @tc.name testFrontFaceError
	 * @tc.desc Test frontFace.
	 */
	it('testFrontFaceError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL frontFace test start ...' + JSON.stringify(gl));
		gl.cullFace(gl.BACK);
		gl.frontFace(gl.CW);
		const frontFaceParameter = gl.getParameter(gl.CULL_FACE_MODE);
		console.info('frontFace -> frontFaceParameter' + frontFaceParameter);
		gl.disable(gl.CULL_FACE);
		expect(frontFaceParameter).assertEqual(gl.BACK);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0182
	 * @tc.name testGenerateMipmap
	 * @tc.desc Test generateMipmap.
	 */
	it('testGenerateMipmap', 0, async function(done) {
		//initContext();
		console.info('jsWebGL generateMipmap test start ...' + JSON.stringify(gl));
		gl.generateMipmap(gl2.TEXTURE_2D);
		const error = gl.getError();
		console.info('generateMipmap error ' + error);
		expect(error).assertEqual(0);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0183
	 * @tc.name testGenerateMipmapError
	 * @tc.desc Test generateMipmap.
	 */
	it('testGenerateMipmapError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL generateMipmap test start ...' + JSON.stringify(gl));
		gl.generateMipmap(gl.TEXTURE_2D);
		const error = gl.getError();
		console.info('generateMipmap error ' + error);
		expect(error).assertEqual(1282);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0184
	 * @tc.name testGetActiveAttrib
	 * @tc.desc Test getActiveAttrib.
	 */
	it('testGetActiveAttrib', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getActiveAttrib test start ...' + JSON.stringify(gl));
		const programObj = globalFunction();
		gl.bindAttribLocation(programObj, 0, 'a_Position');
		//        const numAttribs = gl.getProgramParameter(program, gl.ACTIVE_ATTRIBUTES);
		const info = gl.getActiveAttrib(programObj, 0);
		const error = gl.getError();
		expect(error).assertEqual(0);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0185
	 * @tc.name testGetActiveUniform
	 * @tc.desc Test getActiveUniform.
	 */
	it('testGetActiveUniform', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getActiveUniform test start ...' + JSON.stringify(gl));
		const programObj = globalFunction();
		const info = gl.getActiveUniform(programObj, 0);
		const error = gl.getError();
		expect(error).assertEqual(1281);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0186
	 * @tc.name testGetAttachedShaders
	 * @tc.desc Test getAttachedShaders.
	 */
	it('testGetAttachedShaders', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getAttachedShaders test start ...' + JSON.stringify(gl));
		const programObj = globalFunction();
		const shaderArray = gl.getAttachedShaders(programObj);
		const type = ((typeof shaderArray) === "object")
		console.info('getAttachedShaders shaderArray' + shaderArray);
		expect(type).assertEqual(true);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0187
	 * @tc.name testGetAttribLocationError
	 * @tc.desc Test getAttribLocation.
	 */
	it('testGetAttribLocationError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getAttribLocation test start ...' + JSON.stringify(gl));
		const program = gl.createProgram();
		const index = gl.getAttribLocation(program, 'webgl_Color');
		console.info('getAttribLocation  index' + index);
		expect(index).assertEqual(-1);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0188
	 * @tc.name testGetAttribLocationFirst
	 * @tc.desc Test getAttribLocation.
	 */
	it('testGetAttribLocationFirst', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getAttribLocation test start ...' + JSON.stringify(gl));
		var vertexShader = gl.createShader(gl.VERTEX_SHADER);
		gl.shaderSource(vertexShader, VSHADER_SOURCE);
		gl.compileShader(vertexShader);
		var fragmentShader = gl.createShader(gl.FRAGMENT_SHADER);
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

		const aVertexPosition = gl.getAttribLocation(programObj, "a_Position");
		expect(aVertexPosition).assertEqual(0);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0189
	 * @tc.name testGetBufferParameterFirst
	 * @tc.desc Test getBufferParameter.
	 */
	it('testGetBufferParameterFirst', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getBufferParameter test start ...' + JSON.stringify(gl));
		const buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ARRAY_BUFFER, 1024, gl.STATIC_DRAW);
		const bufferSize = gl.getBufferParameter(gl.ARRAY_BUFFER, gl.BUFFER_SIZE);
		console.info('bufferSize' + bufferSize);
		expect(bufferSize).assertEqual(1024);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0190
	 * @tc.name testGetBufferParameterSecond
	 * @tc.desc Test getBufferParameter.
	 */
	it('testGetBufferParameterSecond', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getBufferParameter test start ...' + JSON.stringify(gl));
		const buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ARRAY_BUFFER, 1024, gl.STATIC_DRAW);
		const bufferUsage = gl.getBufferParameter(gl.ARRAY_BUFFER, gl.BUFFER_USAGE);
		console.info('bufferUsage' + bufferUsage);
		expect(bufferUsage).assertEqual(gl.STATIC_DRAW);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0191
	 * @tc.name testGetErrorFirst
	 * @tc.desc Test getError.
	 */
	it('testGetErrorFirst', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getError test start ...' + JSON.stringify(gl));
		//    turn on scissor test
		gl.enable(gl.SCISSOR_TEST);
		// set the scissor rectangle
		gl.scissor(0, 0, -200, 200);
		// 如果宽度或高度为负值， gl.INVALID_VALUE(DEC:1281)则会引发错误
		const scissorErrorCode = gl.getError();
		console.info("scissorErrorCode: " + scissorErrorCode);
		expect(scissorErrorCode).assertEqual(1281);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0192
	 * @tc.name testGetErrorSecond
	 * @tc.desc Test getError.
	 */
	it('testGetErrorSecond', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getError test start ...' + JSON.stringify(gl));
		gl.getError(); // 0 (0)
		gl.enable(gl.DITHER);
		const errorCode = gl.getError(); // gl.INVALID_ENUM;
		console.info('getError 2 errorCode' + errorCode);
		expect(errorCode).assertEqual(0);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0193
	 * @tc.name testGetFramebufferAttachmentParameter
	 * @tc.desc Test getFramebufferAttachmentParameter.
	 */
	it('testGetFramebufferAttachmentParameter', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getFramebufferAttachmentParameter test start ...' + JSON.stringify(
			gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.RENDERBUFFER,
			renderBuffer);
		const framebuffer = gl.getFramebufferAttachmentParameter(gl.FRAMEBUFFER, gl
			.COLOR_ATTACHMENT0,
			gl.FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE);
		console.info('Framebuffer' + framebuffer);
		const errorCode = gl.getError();
		console.info("jsWebGL getFramebufferAttachmentParameter errorCode: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0194
	 * @tc.name testGetProgramParameterFirst
	 * @tc.desc Test getProgramParameter.
	 */
	it('testGetProgramParameterFirst', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getProgramParameter test start ...' + JSON.stringify(gl));
		const program = gl.createProgram();
		gl.deleteProgram(program);
		const deleteStatus = gl.getProgramParameter(program, gl.DELETE_STATUS);
		console.info('deleteStatus' + deleteStatus);
		expect(deleteStatus).assertEqual(true);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0195
	 * @tc.name testGetProgramParameterSecond
	 * @tc.desc Test getProgramParameter.
	 */
	it('testGetProgramParameterSecond', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getProgramParameter test start ...' + JSON.stringify(gl));
		const program = gl.createProgram();
		gl.deleteProgram(program);
		gl.linkProgram(program);
		const linkStatus = gl.getProgramParameter(program, gl.LINK_STATUS);
		console.info('linkStatus' + linkStatus);
		expect(linkStatus).assertEqual(true);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0196
	 * @tc.name testGetProgramParameterThird
	 * @tc.desc Test getProgramParameter.
	 */
	it('testGetProgramParameterThird', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getProgramParameter test start ...' + JSON.stringify(gl));
		const program = gl.createProgram();
		gl.deleteProgram(program);
		gl.linkProgram(program);
		gl.validateProgram(program);
		const validateStatus = gl.getProgramParameter(program, gl.VALIDATE_STATUS);
		console.info('validateStatus' + validateStatus);
		expect(validateStatus).assertEqual(true);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0197
	 * @tc.name testGetProgramParameterFourth
	 * @tc.desc Test getProgramParameter.
	 */
	it('testGetProgramParameterFourth', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getProgramParameter test start ...' + JSON.stringify(gl));
		const program = globalFunction();
		const attachedShaders = gl.getProgramParameter(program, gl.ATTACHED_SHADERS);
		console.info('attachedShaders' + attachedShaders);
		expect(attachedShaders).assertEqual(2);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0198
	 * @tc.name testGetProgramParameterError
	 * @tc.desc Test getProgramParameter.
	 */
	it('testGetProgramParameterError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getProgramParameter test start ...' + JSON.stringify(gl));
		const vertexShader = gl.createShader(gl.VERTEX_SHADER);
		gl.shaderSource(vertexShader, VSHADER_SOURCE);
		gl.compileShader(vertexShader);

		const programObj = gl.createProgram();
		//        gl.attachShader(programObj, vertexShader);
		const attachedShaders = gl.getProgramParameter(programObj, gl.ATTACHED_SHADERS);
		console.info('attachedShaders' + attachedShaders);
		const type = (attachedShaders === 1);
		expect(type).assertEqual(false);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0199
	 * @tc.name testGetProgramInfoLog
	 * @tc.desc Test getProgramInfoLog.
	 */
	it('testGetProgramInfoLog', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getProgramInfoLog test start ...' + JSON.stringify(gl));
		const program = gl.createProgram();
		// Attach pre-existing shaders
		gl.attachShader(program, VSHADER_SOURCE);
		gl.attachShader(program, FSHADER_SOURCE);
		gl.linkProgram(program);
		gl.validateProgram(program);
		const info = gl.getProgramInfoLog(program);
		gl.useProgram(program);
		expect(info).assertEqual('The program object is incomplete.');
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0200
	 * @tc.name testGetRenderbufferParameterFirst
	 * @tc.desc Test getRenderbufferParameter.
	 */
	it('testGetRenderbufferParameterFirst', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getRenderbufferParameter test start ...' + JSON.stringify(gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.renderbufferStorage(gl.RENDERBUFFER, gl.RGBA4, 256, 256);

		const width = gl.getRenderbufferParameter(gl.RENDERBUFFER, gl.RENDERBUFFER_WIDTH);
		expect(width).assertEqual(256);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0201
	 * @tc.name testGetRenderbufferParameterSecond
	 * @tc.desc Test getRenderbufferParameter.
	 */
	it('testGetRenderbufferParameterSecond', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getRenderbufferParameter test start ...' + JSON.stringify(gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.renderbufferStorage(gl.RENDERBUFFER, gl.RGBA4, 256, 256);

		const width = gl.getRenderbufferParameter(gl.RENDERBUFFER, gl.RENDERBUFFER_INTERNAL_FORMAT);
		expect(width).assertEqual(gl.RGBA4);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0202
	 * @tc.name testGetRenderbufferParameterThird
	 * @tc.desc Test getRenderbufferParameter.
	 */
	it('testGetRenderbufferParameterThird', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getRenderbufferParameter test start ...' + JSON.stringify(gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.renderbufferStorage(gl.RENDERBUFFER, gl.RGBA4, 256, 256);
		const internalFormat = gl.getRenderbufferParameter(gl.RENDERBUFFER, gl
			.RENDERBUFFER_INTERNAL_FORMAT);
		expect(internalFormat).assertEqual(gl.RGBA4);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0203
	 * @tc.name testGetRenderbufferParameterFourth
	 * @tc.desc Test getRenderbufferParameter.
	 */
	it('testGetRenderbufferParameterFourth', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getRenderbufferParameter test start ...' + JSON.stringify(gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.renderbufferStorage(gl.RENDERBUFFER, gl.RGBA4, 256, 256);
		const greenSize = gl.getRenderbufferParameter(gl.RENDERBUFFER, gl.RENDERBUFFER_GREEN_SIZE);
		expect(greenSize).assertEqual(4);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0204
	 * @tc.name testGetRenderbufferParameterError
	 * @tc.desc Test getRenderbufferParameter.
	 */
	it('testGetRenderbufferParameterError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getRenderbufferParameter test start ...' + JSON.stringify(gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		//        gl.renderbufferStorage(gl.RENDERBUFFER, gl.RGBA4, 256, 256);
		const greenSize = gl.getRenderbufferParameter(gl.RENDERBUFFER, gl.RENDERBUFFER_GREEN_SIZE);
		expect(greenSize).assertEqual(0);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0205
	 * @tc.name testGetShaderParameterFirst
	 * @tc.desc Test getShaderParameter.
	 */
	it('testGetShaderParameterFirst', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getShaderParameter test start ...' + JSON.stringify(gl));
		const vertexShader = gl.createShader(gl.VERTEX_SHADER);
		//        const fragmentShader = gl.createShader(gl.FRAGMENT_SHADER);
		gl.deleteShader(vertexShader);
		const deleteStatus = gl.getShaderParameter(vertexShader, gl.DELETE_STATUS);
		console.info('deleteStatus' + deleteStatus);
		expect(deleteStatus).assertEqual(true);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0206
	 * @tc.name testGetShaderParameterSecond
	 * @tc.desc Test getShaderParameter.
	 */
	it('testGetShaderParameterSecond', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getShaderParameter test start ...' + JSON.stringify(gl));
		const vertexShader = gl.createShader(gl.VERTEX_SHADER);
		//        const fragmentShader = gl.createShader(gl.FRAGMENT_SHADER);
		//        gl.deleteShader(vertexShader);
		const deleteStatus = gl.getShaderParameter(vertexShader, gl.DELETE_STATUS);
		console.info('deleteStatus' + deleteStatus);
		expect(deleteStatus).assertEqual(false);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0207
	 * @tc.name testGetShaderParameterThird
	 * @tc.desc Test getShaderParameter.
	 */
	it('testGetShaderParameterThird', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getShaderParameter test start ...' + JSON.stringify(gl));
		const vertexShader = gl.createShader(gl.VERTEX_SHADER);
		gl.shaderSource(vertexShader, VSHADER_SOURCE);
		gl.compileShader(vertexShader);
		const compileStatus = gl.getShaderParameter(vertexShader, gl.COMPILE_STATUS);
		console.info('compileStatus' + compileStatus);
		expect(compileStatus).assertEqual(true);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0208
	 * @tc.name testGetShaderParameterFourth
	 * @tc.desc Test getShaderParameter.
	 */
	it('testGetShaderParameterFourth', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getShaderParameter test start ...' + JSON.stringify(gl));
		var fragmentShader = gl.createShader(gl.FRAGMENT_SHADER);
		gl.shaderSource(fragmentShader, FSHADER_SOURCE);
		gl.compileShader(fragmentShader);
		const compileStatus = gl.getShaderParameter(fragmentShader, gl.COMPILE_STATUS);
		console.info('compileStatus' + compileStatus);
		expect(compileStatus).assertEqual(true);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0209
	 * @tc.name testGetShaderParameterError
	 * @tc.desc Test getShaderParameter.
	 */
	it('testGetShaderParameterError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getShaderParameter test start ...' + JSON.stringify(gl));
		var fragmentShader = gl.createShader(gl.FRAGMENT_SHADER);
		//        gl.shaderSource(fragmentShader, FSHADER_SOURCE);
		//        gl.compileShader(fragmentShader);
		const compileStatus = gl.getShaderParameter(fragmentShader, gl.COMPILE_STATUS);
		console.info('compileStatus' + compileStatus);
		const type = (compileStatus === true)
		expect(type).assertEqual(false);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0210
	 * @tc.name testGetShaderPrecisionFormat
	 * @tc.desc Test getShaderPrecisionFormat.
	 */
	it('testGetShaderPrecisionFormat', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getShaderPrecisionFormat test start ...' + JSON.stringify(gl));
		const shaderPrecisionFormat = gl.getShaderPrecisionFormat(gl.VERTEX_SHADER, gl
			.MEDIUM_FLOAT);
		console.info('shaderPrecisionFormat' + JSON.stringify(shaderPrecisionFormat));
		const type = ((typeof shaderPrecisionFormat) === "object")
		expect(type).assertEqual(true);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0211
	 * @tc.name testGetTexParameterError
	 * @tc.desc Test getTexParameter.
	 */
	it('testGetTexParameterError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getTexParameter test start ...' + JSON.stringify(gl));
		const texParameterValue = gl.getTexParameter(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER);
		console.info('texParameterfValue' + texParameterValue);
		const type = (texParameterValue === gl.LINEAR_MIPMAP_NEAREST);
		expect(type).assertEqual(false);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0212
	 * @tc.name testGetTexParameterFirst
	 * @tc.desc Test getTexParameter.
	 */
	it('testGetTexParameterFirst', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getTexParameter test start ...' + JSON.stringify(gl));
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR_MIPMAP_NEAREST);
		const texParameterValue = gl.getTexParameter(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER);
		console.info('texParameterfValue' + texParameterValue);
		expect(texParameterValue).assertEqual(gl.LINEAR_MIPMAP_NEAREST);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0213
	 * @tc.name testGetTexParameterSecond
	 * @tc.desc Test getTexParameter.
	 */
	it('testGetTexParameterSecond', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getTexParameter test start ...' + JSON.stringify(gl));
		gl.texParameterf(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
		const texParameterValue = gl.getTexParameter(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER);
		console.info('texParameterfValue' + texParameterValue);
		expect(texParameterValue).assertEqual(gl.LINEAR);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0214
	 * @tc.name testGetUniform
	 * @tc.desc Test getUniform.
	 */
	it('testGetUniform', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getUniform test start ...' + JSON.stringify(gl));
		//        const programObj = globalFunction();
		//
		//        const uniformLocation = gl.getUniformLocation(programObj, "a_Position");
		//        const uniform = gl.getUniform(programObj, uniformLocation);
		//        console.info('getUniformLocation uniform' + uniform);
		//        expect(uniform.toString()).assertEqual([0,0,0,0].toString());
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0215
	 * @tc.name testGetUniformError
	 * @tc.desc Test getUniform.
	 */
	it('testGetUniformError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getUniform test start ...' + JSON.stringify(gl));
		const programObj = gl.createProgram();


		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");
		const uniform = gl.getUniform(programObj, uniformLocation);
		console.info('getUniformLocation uniform' + uniform);
		expect(uniform).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0216
	 * @tc.name testGetUniformLocation
	 * @tc.desc Test getUniformLocation.
	 */
	it('testGetUniformLocation', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getUniformLocation test start ...' + JSON.stringify(gl));
		//        let programObj = globalFunction();
		//        const uniformLocation = gl.getUniformLocation(programObj, "a_Position");
		//        const uniform = gl.getUniform(programObj, uniformLocation);
		//        console.info('getUniformLocation uniform' + uniform);
		//        expect(uniform.toString()).assertEqual([0,0,0,0].toString());
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0217
	 * @tc.name testGetUniformLocationError
	 * @tc.desc Test getUniformLocation.
	 */
	it('testGetUniformLocationError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getUniformLocation test start ...' + JSON.stringify(gl));
		const programObj = gl.createProgram();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");
		const uniform = gl.getUniform(programObj, uniformLocation);
		console.info('getUniformLocation uniform' + uniform);
		expect(uniform).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0218
	 * @tc.name testGetParameter1
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.ACTIVE_TEXTURE);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		expect(textureParameter).assertEqual(33984);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0219
	 * @tc.name testGetParameter2
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));

		//设置线宽：
		gl.lineWidth(5);
		// 获取线宽：
		const lineWidthValue = gl.getParameter(gl.LINE_WIDTH);
		console.info("lineWidthValue: " + lineWidthValue);
		// 获取可用宽度的范围。返回一个Float32Array.
		const lineWidthArray = gl.getParameter(gl.ALIASED_LINE_WIDTH_RANGE);
		console.info("lineWidthArray: " + lineWidthArray);
		expect(lineWidthValue).assertEqual(5);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0220
	 * @tc.name testGetParameter3
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		const buffer = gl.getParameter(gl.ARRAY_BUFFER_BINDING);
		console.info("buffer: " + buffer);
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		const isBuffer = gl.isBuffer(buffer);
		console.info("isBuffer: " + isBuffer);
		expect(isBuffer).assertEqual(true);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0221
	 * @tc.name testGetParameter4
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		var framebuffer = gl.createFramebuffer();
		gl.bindFramebuffer(gl.FRAMEBUFFER, framebuffer);

		const framebufferParameter = gl.getParameter(gl.FRAMEBUFFER_BINDING);
		console.info("framebufferParameter: " + framebufferParameter);
		const isFramebuffer = gl.isFramebuffer(framebufferParameter);
		console.info("isFramebuffer: " + isFramebuffer);
		expect(isFramebuffer).assertEqual(true);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0222
	 * @tc.name testGetParameter5
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter5', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		const renderbuffer = gl.getParameter(gl.RENDERBUFFER_BINDING);
		console.info("renderbuffer: " + renderbuffer);
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderbuffer);
		const isRenderbuffer = gl.isRenderbuffer(renderbuffer);
		console.info("isRenderbuffer: " + isRenderbuffer);
		expect(isRenderbuffer).assertEqual(true);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0223
	 * @tc.name testGetParameter6
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter6', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		var texture = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, texture);
		const textureParameter = gl.getParameter(gl.TEXTURE_BINDING_2D);
		console.info("texture: " + texture);

		const isTexture = gl.isTexture(textureParameter);
		console.info("isTexture: " + isTexture);
		expect(isTexture).assertEqual(true);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0224
	 * @tc.name testGetParameter7
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter7', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.blendColor(0, 0.5, 1, 1);
		const blendColorValue = gl.getParameter(gl.BLEND_COLOR);
		// Float32Array[0, 0.5, 1, 1]
		console.info("blendColor --> getParameter: " + blendColorValue);
		var blendColorArr = new Float32Array([0, 0.5, 1, 1]);

		const equalsblendColorType = (blendColorValue.constructor === blendColorArr.constructor);
		console.info("blendColorequalsType: " + equalsblendColorType);

		const equalsblendColorValue = (blendColorValue.toString() === blendColorArr.toString());
		console.info("equalsblendColorValue: " + equalsblendColorValue);
		expect(equalsblendColorType).assertEqual(true);
		expect(equalsblendColorValue).assertEqual(true);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0225
	 * @tc.name testGetParameter8
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter8', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.depthMask(false);
		// 要获得当前的深度遮罩值，传入 DEPTH_WRITEMASK 常量，返回  Boolean.
		const depthMaskValue = gl.getParameter(gl.DEPTH_WRITEMASK);
		// false
		console.info("depthMaskValue: " + depthMaskValue);
		expect(depthMaskValue).assertEqual(false);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0226
	 * @tc.name testGetParameter9
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter9', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.colorMask(true, true, true, false);
		// 要获取当前的颜色掩码，请查询COLOR_WRITEMASK返回Array.
		const colorMaskValue = gl.getParameter(gl.COLOR_WRITEMASK);
		// [true, true, true, false]
		console.info("colorMask --> getParameter: " + colorMaskValue);
		expect(colorMaskValue.toString()).assertEqual([true, true, true, false].toString());
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0227
	 * @tc.name testGetParameter10
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter10', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		//用于设置深度缓冲区的深度清除值。
		gl.clearDepth(0.5);
		//若要获取当前深度清除值，查询DEPTH_CLEAR_VALUE 常量。
		const clearDepthValue = gl.getParameter(gl.DEPTH_CLEAR_VALUE);
		// 0.5
		console.info("clearDepth --> getParameter: " + clearDepthValue);
		expect(clearDepthValue).assertEqual(0.5);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0228
	 * @tc.name testGetParameter11
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter11', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		const tex = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, tex);
		gl.pixelStorei(gl.PACK_ALIGNMENT, 2);
		gl.pixelStorei(gl.UNPACK_ALIGNMENT, 8);
		//要检查像素数据的打包和解包值，可以使用 查询相同的像素存储参数WebGLRenderingContext.getParameter()。
		const packValue = gl.getParameter(gl.PACK_ALIGNMENT);
		const unpackValue = gl.getParameter(gl.UNPACK_ALIGNMENT);
		console.info("packValue: " + packValue);
		console.info("unpackValue: " + unpackValue);
		expect(packValue).assertEqual(2);
		expect(unpackValue).assertEqual(8);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0229
	 * @tc.name testGetParameter12
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter12', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		const str = gl.getParameter(gl.SHADING_LANGUAGE_VERSION);
		console.info("getParameter str: " + str);
		//        const strType = ((typeof str) === "object");
		const str1 = str.join(" ");
		console.info("strType: " + str1);
		expect(str1).assertEqual('OpenGL ES GLSL ES 3.20');
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0230
	 * @tc.name testGetParameter13
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter13', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.enable(gl.SAMPLE_COVERAGE);
		gl.sampleCoverage(0.5, false);
		// To check the sample coverage values, query the SAMPLE_COVERAGE_VALUE and SAMPLE_COVERAGE_INVERT constants.
		const sampleCoverageValue = gl.getParameter(gl.SAMPLE_COVERAGE_VALUE); // 0.5
		const sampleCoverageInvert = gl.getParameter(gl.SAMPLE_COVERAGE_INVERT); // false
		console.info("sampleCoverageValue: " + sampleCoverageValue);
		console.info("sampleCoverageInvert: " + sampleCoverageInvert);
		expect(sampleCoverageValue).assertEqual(0.5);
		expect(sampleCoverageInvert).assertEqual(false);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0231
	 * @tc.name testGetVertexAttribFirst
	 * @tc.desc Test getVertexAttrib.
	 */
	it('testGetVertexAttribFirst', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getVertexAttrib test start ...' + JSON.stringify(gl));
		const vertexBuffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, vertexBuffer);
		const shaderProgram = globalFunction();
		const aVertexPosition = gl.getAttribLocation(shaderProgram, "a_position");

		gl.enableVertexAttribArray(aVertexPosition);
		gl.vertexAttribPointer(aVertexPosition, 3, gl.FLOAT, false, 20, 0);

		gl.drawArrays(gl.TRIANGLES, 0, 8);
		gl.vertexAttrib1f(0, 2.8);
		const type = gl.getVertexAttrib(0, gl.VERTEX_ATTRIB_ARRAY_ENABLED);
		console.info("getVertexAttrib: type" + type);
		expect(type).assertEqual(true);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0232
	 * @tc.name testGetVertexAttrib
	 * @tc.desc Test getVertexAttrib.
	 */
	it('testGetVertexAttrib', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getVertexAttrib test start ...' + JSON.stringify(gl));
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		const bufferVertexAttrib = gl.getVertexAttrib(0, gl.VERTEX_ATTRIB_ARRAY_BUFFER_BINDING);
		const isBuffer = gl.isBuffer(bufferVertexAttrib);
		console.info("getVertexAttrib: bufferVertexAttrib" + bufferVertexAttrib);
		expect(isBuffer).assertEqual(false);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0233
	 * @tc.name testGetVertexAttribOffset
	 * @tc.desc Test getVertexAttribOffset.
	 */
	it('testGetVertexAttribOffset', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getVertexAttribOffset test start ...' + JSON.stringify(gl));
		gl.vertexAttrib1f(0, 2.8);
		const index = gl.getVertexAttribOffset(0, gl.VERTEX_ATTRIB_ARRAY_POINTER);
		console.info("getVertexAttribOffset: index" + index);

		const errorCode = gl.getError();
		console.info("jsWebGL getFramebufferAttachmentParameter errorCode: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_VALUE);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0234
	 * @tc.name testUniform1f
	 * @tc.desc Test uniform1f.
	 */
	it('testUniform1f', 0, async function(done) {
		//initContext();
		console.info('jsWebGL uniform1f test start ...' + JSON.stringify(gl));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl.uniform1f(uniformLocation, 0.8);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(0);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0235
	 * @tc.name testUniform1fError
	 * @tc.desc Test uniform1f.
	 */
	it('testUniform1fError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL uniform1f test start ...' + JSON.stringify(gl));
		const program = gl.createProgram();
		const uniformLocation = gl.getUniformLocation(program, "a_Position");

		gl.uniform1f(uniformLocation, 0.8, 0.8);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(1282);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0236
	 * @tc.name testUniform2f
	 * @tc.desc Test uniform2f.
	 */
	it('testUniform2f', 0, async function(done) {
		//initContext();
		console.info('jsWebGL uniform2f test start ...' + JSON.stringify(gl));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl.uniform2f(uniformLocation, 0.8, 0.8);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(0);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0237
	 * @tc.name testUniform2fError
	 * @tc.desc Test uniform2f.
	 */
	it('testUniform2fError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL uniform2f test start ...' + JSON.stringify(gl));
		const program = gl.createProgram();
		const uniformLocation = gl.getUniformLocation(program, "a_Position");

		gl.uniform2f(uniformLocation, 0.8);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(1282);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0238
	 * @tc.name testUniform3f
	 * @tc.desc Test uniform3f.
	 */
	it('testUniform3f', 0, async function(done) {
		//initContext();
		console.info('jsWebGL uniform3f test start ...' + JSON.stringify(gl));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl.uniform3f(uniformLocation, 0.8, 0.8, 0.8);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(0);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0239
	 * @tc.name testUniform3fError
	 * @tc.desc Test frontFace.
	 */
	it('testUniform3fError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL uniform3f test start ...' + JSON.stringify(gl));
		const program = gl.createProgram();
		const uniformLocation = gl.getUniformLocation(program, "a_Position");

		gl.uniform3f(uniformLocation, 0.8, 0.8);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(1282);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0240
	 * @tc.name testUniform4f
	 * @tc.desc Test uniform4f.
	 */
	it('testUniform4f', 0, async function(done) {
		//initContext();
		console.info('jsWebGL uniform4f test start ...' + JSON.stringify(gl));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl.uniform4f(uniformLocation, 0.8, 0.8, 0.8, 0.8);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(0);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0241
	 * @tc.name testUniform4fError
	 * @tc.desc Test uniform4f.
	 */
	it('testUniform4fError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL uniform4f test start ...' + JSON.stringify(gl));
		const program = gl.createProgram();
		const uniformLocation = gl.getUniformLocation(program, "a_Position");

		gl.uniform4f(uniformLocation, 0.8, 0.8);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(1282);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0242
	 * @tc.name testUniform1i
	 * @tc.desc Test uniform1i.
	 */
	it('testUniform1i', 0, async function(done) {
		//initContext();
		console.info('jsWebGL uniform1i test start ...' + JSON.stringify(gl));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl.uniform1i(uniformLocation, 2);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(0);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0243
	 * @tc.name testUniform1iError
	 * @tc.desc Test uniform1i.
	 */
	it('testUniform1iError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL uniform1i test start ...' + JSON.stringify(gl));
		const program = gl.createProgram();
		const uniformLocation = gl.getUniformLocation(program, "a_Position");

		gl.uniform1i(uniformLocation, 0.8, 0.8);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(1282);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0244
	 * @tc.name testUniform2i
	 * @tc.desc Test uniform2i.
	 */
	it('testUniform2i', 0, async function(done) {
		//initContext();
		console.info('jsWebGL uniform2i test start ...' + JSON.stringify(gl));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl.uniform2i(uniformLocation, 2, 2);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(0);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0245
	 * @tc.name testUniform2iError
	 * @tc.desc Test uniform2i.
	 */
	it('testUniform2iError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL uniform2i test start ...' + JSON.stringify(gl));
		const program = gl.createProgram();
		const uniformLocation = gl.getUniformLocation(program, "a_Position");

		gl.uniform2i(uniformLocation, 0.8, 0.8);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(1282);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0246
	 * @tc.name testUniform3i
	 * @tc.desc Test uniform3i.
	 */
	it('testUniform3i', 0, async function(done) {
		//initContext();
		console.info('jsWebGL uniform3i test start ...' + JSON.stringify(gl));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl.uniform3i(uniformLocation, 2, 2, 2);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(0);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0247
	 * @tc.name testUniform3iError
	 * @tc.desc Test uniform3i.
	 */
	it('testUniform3iError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL uniform3i test start ...' + JSON.stringify(gl));
		const program = gl.createProgram();
		const uniformLocation = gl.getUniformLocation(program, "a_Position");

		gl.uniform3i(uniformLocation, 0.8, 0.8);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(1282);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0248
	 * @tc.name testUniform4i
	 * @tc.desc Test uniform4i.
	 */
	it('testUniform4i', 0, async function(done) {
		//initContext();
		console.info('jsWebGL uniform4i test start ...' + JSON.stringify(gl));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl.uniform4i(uniformLocation, 2, 2, 2, 2);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(0);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0249
	 * @tc.name testUniform4iError
	 * @tc.desc Test uniform4i.
	 */
	it('testUniform4iError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL uniform4i test start ...' + JSON.stringify(gl));
		const program = gl.createProgram();
		const uniformLocation = gl.getUniformLocation(program, "a_Position");

		gl.uniform4i(uniformLocation, 0.8, 0.8);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(1282);
		done();
	});



	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0250
	 * @tc.name testTexParameterf
	 * @tc.desc Test texParameterf.
	 */
	it('testTexParameterf', 0, async function(done) {
		//initContext();
		console.info('jsWebGL texParameterf test start ...' + JSON.stringify(gl));
		gl.texParameterf(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
		const texParameterValue = gl.getTexParameter(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER);
		console.info('texParameterfValue' + texParameterValue);
		expect(texParameterValue).assertEqual(gl.LINEAR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0251
	 * @tc.name testTexParameteri
	 * @tc.desc Test texParameteri.
	 */
	it('testTexParameteri', 0, async function(done) {
		//initContext();
		console.info('jsWebGL texParameteri test start ...' + JSON.stringify(gl));
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR_MIPMAP_NEAREST);
		const texParameterValue = gl.getTexParameter(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER);
		console.info('texParameterfValue' + texParameterValue);
		expect(texParameterValue).assertEqual(gl.LINEAR_MIPMAP_NEAREST);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0252
	 * @tc.name testHint
	 * @tc.desc Test hint.
	 */
	it('testHint', 0, async function(done) {
		//initContext();
		console.info('jsWebGL hint test start ...' + JSON.stringify(gl));
		gl.hint(gl.GENERATE_MIPMAP_HINT, gl.FASTEST);
		gl.generateMipmap(gl2.TEXTURE_2D);
		const error = gl.getError();
		console.info('error' + error);
		expect(error).assertEqual(0);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0253
	 * @tc.name testHintError
	 * @tc.desc Test hint.
	 */
	it('testHintError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL hint test start ...' + JSON.stringify(gl));
		gl.hint(gl.GENERATE_MIPMAP_HINT, gl.FASTEST);
		gl.generateMipmap(gl.TEXTURE_2D);
		const error = gl.getError();
		console.info('hintError' + error);
		expect(error).assertEqual(1282);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0254
	 * @tc.name testIsBuffer
	 * @tc.desc Test isBuffer.
	 */
	it('testIsBuffer', 0, async function(done) {
		//initContext();
		console.info('jsWebGL isBuffer test start ...' + JSON.stringify(gl));
		const buffer = gl.createBuffer();

		const isBuffer1 = gl.isBuffer(buffer);
		console.info('isBuffer1' + isBuffer1);
		expect(isBuffer1).assertEqual(false);

		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);

		const isBuffer = gl.isBuffer(buffer);
		console.info('isBuffer' + isBuffer);
		expect(isBuffer).assertEqual(true);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0255
	 * @tc.name testIsEnabled
	 * @tc.desc Test isEnabled.
	 */
	it('testIsEnabled', 0, async function(done) {
		//initContext();
		console.info('jsWebGL isEnabled test start ...' + JSON.stringify(gl));

		gl.enable(gl.STENCIL_TEST);
		const isEnabledValue1 = gl.isEnabled(gl.STENCIL_TEST);
		console.info("isEnabledValue1: " + isEnabledValue1);
		expect(isEnabledValue1).assertEqual(true);

		// false
		// 启用或说明给定的能力项，使用WebGLRenderingContext.enable()方法和WebGLRenderingContext.disable()方法：
		gl.disable(gl.STENCIL_TEST);
		const isEnabledValue2 = gl.isEnabled(gl.STENCIL_TEST);
		console.info("isEnabledValue2: " + isEnabledValue2);
		expect(isEnabledValue2).assertEqual(false);

		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0256
	 * @tc.name testSampleCoverageFirst
	 * @tc.desc Test sampleCoverage.
	 */
	it('testSampleCoverageFirst', 0, async function(done) {
		//initContext();
		console.info('jsWebGL sampleCoverage test start ...' + JSON.stringify(gl));
		gl.enable(gl.SAMPLE_COVERAGE);
		gl.sampleCoverage(0.5, false);
		// To check the sample coverage values, query the SAMPLE_COVERAGE_VALUE and SAMPLE_COVERAGE_INVERT constants.
		const sampleCoverageInvert = gl.getParameter(gl.SAMPLE_COVERAGE_INVERT); // false
		console.info("sampleCoverageInvert: " + sampleCoverageInvert);

		expect(sampleCoverageInvert).assertEqual(false);

		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0257
	 * @tc.name testSampleCoverageSecond
	 * @tc.desc Test sampleCoverage.
	 */
	it('testSampleCoverageSecond', 0, async function(done) {
		//initContext();
		console.info('jsWebGL sampleCoverage test start ...' + JSON.stringify(gl));
		gl.enable(gl.SAMPLE_COVERAGE);
		gl.sampleCoverage(0.5, false);
		// To check the sample coverage values, query the SAMPLE_COVERAGE_VALUE and SAMPLE_COVERAGE_INVERT constants.
		const sampleCoverageValue = gl.getParameter(gl.SAMPLE_COVERAGE_VALUE); // 0.5
		console.info("sampleCoverageValue: " + sampleCoverageValue);
		expect(sampleCoverageValue).assertEqual(0.5);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0258
	 * @tc.name testScissor
	 * @tc.desc Test scissor.
	 */
	it('testScissor', 0, async function(done) {
		//initContext();
		console.info('jsWebGL scissor test start ...' + JSON.stringify(gl));

		// turn on scissor test
		gl.enable(gl.SCISSOR_TEST);
		// set the scissor rectangle
		gl.scissor(0, 0, -200, 200);
		// 如果宽度或高度为负值， gl.INVALID_VALUE(DEC:1281)则会引发错误
		const scissorErrorCode = gl.getError();
		console.info("scissorErrorCode: " + scissorErrorCode);
		expect(scissorErrorCode).assertEqual(1281);

		gl.scissor(0, 0, 200, 200);
		const scissorValue = gl.getParameter(gl.SCISSOR_BOX);
		console.info("scissorValue: " + scissorValue);
		// Int32Array[0, 0, 200, 200]
		var testInt32Array = new Int32Array([0, 0, 200, 200]);

		// 判断数据类型
		const equalsScissorType = (scissorValue.constructor === testInt32Array.constructor);
		// 判断数据值
		const equalsScissorValue = (scissorValue.toString() === testInt32Array.toString());

		expect(equalsScissorType).assertEqual(true);
		expect(equalsScissorValue).assertEqual(true);

		// execute drawing commands in the scissor box (e.g. clear)
		// turn off scissor test again
		gl.disable(gl.SCISSOR_TEST);

		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0259
	 * @tc.name testRenderbufferStorage
	 * @tc.desc Test renderbufferStorage.
	 */
	it('testRenderbufferStorage', 0, async function(done) {
		//initContext();
		console.info('jsWebGL renderbufferStorage test start ...' + JSON.stringify(gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.renderbufferStorage(gl.RENDERBUFFER, gl.RGBA4, 256, 256);
		const width = gl.getRenderbufferParameter(gl.RENDERBUFFER, gl.RENDERBUFFER_WIDTH);
		console.info("getRenderbufferParameter width: " + width);
		expect(width).assertEqual(256);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0260
	 * @tc.name testStencilFunc
	 * @tc.desc Test stencilFunc.
	 */
	it('testStencilFunc', 0, async function(done) {
		//initContext();
		console.info('jsWebGL stencilFunc test start ...' + JSON.stringify(gl));
		gl.enable(gl.STENCIL_TEST);
		gl.stencilFunc(gl.LESS, 0, 0b1110011);
		const stencilFuncParameter = gl.getParameter(gl.STENCIL_FUNC);
		console.info("stencilFunc stencilFuncParameter: " + stencilFuncParameter);
		expect(stencilFuncParameter).assertEqual(gl.LESS);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0261
	 * @tc.name testStencilFuncSeparate
	 * @tc.desc Test stencilFuncSeparate.
	 */
	it('testStencilFuncSeparate', 0, async function(done) {
		//initContext();
		console.info('jsWebGL stencilFuncSeparate test start ...' + JSON.stringify(gl));
		gl.enable(gl.STENCIL_TEST);
		gl.stencilFuncSeparate(gl.FRONT, gl.LESS, 0.2, 1110011);
		const stencilFuncSeparateParameter = gl.getParameter(gl.STENCIL_FUNC);
		console.info("stencilFuncSeparate stencilFuncSeparateParameter: " +
			stencilFuncSeparateParameter);
		expect(stencilFuncSeparateParameter).assertEqual(gl.LESS);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0262
	 * @tc.name testStencilMask
	 * @tc.desc Test stencilMask.
	 */
	it('testStencilMask', 0, async function(done) {
		//initContext();
		console.info('jsWebGL stencilMask test start ...' + JSON.stringify(gl));
		gl.stencilMask(110101);
		const stencilMaskParameter = gl.getParameter(gl.STENCIL_WRITEMASK);
		console.info("stencilMask stencilMaskParameter: " + stencilMaskParameter);
		expect(stencilMaskParameter).assertEqual(110101);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0263
	 * @tc.name testStencilMaskSeparate
	 * @tc.desc Test stencilMaskSeparate.
	 */
	it('testStencilMaskSeparate', 0, async function(done) {
		//initContext();
		console.info('jsWebGL stencilMaskSeparate test start ...' + JSON.stringify(gl));
		gl.stencilMaskSeparate(gl.FRONT, 110101);
		const stencilMaskSeparateParameter = gl.getParameter(gl.STENCIL_WRITEMASK);
		console.info("stencilMaskSeparate stencilMaskSeparateParameter: " +
			stencilMaskSeparateParameter);
		expect(stencilMaskSeparateParameter).assertEqual(110101);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0264
	 * @tc.name testStencilOp
	 * @tc.desc Test stencilOp.
	 */
	it('testStencilOp', 0, async function(done) {
		//initContext();
		console.info('jsWebGL stencilOp test start ...' + JSON.stringify(gl));
		gl.enable(gl.STENCIL_TEST);
		gl.stencilOp(gl.INCR, gl.DECR, gl.INVERT);
		const stencilOpParameter = gl.getParameter(gl.STENCIL_FAIL);
		console.info("stencilOp stencilOpParameter: " + stencilOpParameter);
		expect(stencilOpParameter).assertEqual(gl.INCR);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0265
	 * @tc.name testStencilOpSeparate
	 * @tc.desc Test stencilOpSeparate.
	 */
	it('testStencilOpSeparate', 0, async function(done) {
		//initContext();
		console.info('jsWebGL stencilOpSeparate test start ...' + JSON.stringify(gl));
		gl.enable(gl.STENCIL_TEST);
		gl.stencilOpSeparate(gl.FRONT, gl.INCR, gl.DECR, gl.INVERT);
		const stencilOpSeparateParameter = gl.getParameter(gl.STENCIL_FAIL);
		console.info("stencilOpSeparate stencilOpSeparateParameter: " + stencilOpSeparateParameter);
		expect(stencilOpSeparateParameter).assertEqual(gl.INCR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0266
	 * @tc.name testShaderSource
	 * @tc.desc Test shaderSource.
	 */
	it('testShaderSource', 0, async function(done) {
		//initContext();
		console.info('jsWebGL shaderSource test start ...' + JSON.stringify(gl));
		var shader = gl.createShader(gl.VERTEX_SHADER);
		gl.shaderSource(shader, 'originalSource');

		var source = gl.getShaderSource(shader);
		console.info("shaderSource source: " + source);
		expect(source).assertEqual('originalSource');
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0267
	 * @tc.name testGetShaderSource
	 * @tc.desc Test getShaderSource.
	 */
	it('testGetShaderSource', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getShaderSource test start ...' + JSON.stringify(gl));
		var shader = gl.createShader(gl.VERTEX_SHADER);
		gl.shaderSource(shader, 'originalSource');

		var source = gl.getShaderSource(shader);
		console.info("getShaderSource source: " + source);
		expect(source).assertEqual('originalSource');
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0268
	 * @tc.name testGetShaderSourceError
	 * @tc.desc Test getShaderSource.
	 */
	it('testGetShaderSourceError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getShaderSource test start ...' + JSON.stringify(gl));
		var shader = gl.createShader(gl.VERTEX_SHADER);

		var source = gl.getShaderSource(shader);
		console.info("getShaderSource source: " + source);
		expect(source).assertEqual();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0269
	 * @tc.name testGetShaderInfoLog
	 * @tc.desc Test getShaderInfoLog.
	 */
	it('testGetShaderInfoLog', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getShaderInfoLog test start ...' + JSON.stringify(gl));
		var shader = gl.createShader(gl.VERTEX_SHADER);
		gl.shaderSource(shader, "shaderCode");

		/* compile shader source code. */
		gl.compileShader(shader);
		const message = gl.getShaderInfoLog(shader);
		console.info("getShaderInfoLog message: " + message);
		expect(message).assertContain("0:1: L0001: Typename expected, found 'shaderCode'");
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0270
	 * @tc.name testFrontFaceSecond
	 * @tc.desc Test frontFace.
	 */
	it('testFrontFaceSecond', 0, async function(done) {
		//initContext();
		console.info('jsWebGL frontFace test start ...' + JSON.stringify(gl));
		gl.enable(gl.CULL_FACE);
		gl.cullFace(gl.FRONT);
		gl.frontFace(gl.CW);
		const frontFaceParameter = gl.getParameter(gl.CULL_FACE_MODE);
		console.info('frontFace -> frontFaceParameter' + frontFaceParameter);
		gl.disable(gl.CULL_FACE);
		expect(frontFaceParameter).assertEqual(gl.FRONT);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0271
	 * @tc.name testGetUniformBlockIndex
	 * @tc.desc Test getUniformBlockIndex.
	 */
	it('testGetUniformBlockIndex', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 getUniformBlockIndex test start ...' + JSON.stringify(gl2));
		const program = gl.createProgram();
		const blockIndex = gl2.getUniformBlockIndex(program, 'a_Position');
		gl2.uniformBlockBinding(program, blockIndex, 1);
		console.info("uniformBlockBinding blockIndex: " + blockIndex);
		expect(blockIndex).assertEqual(4294967295);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0272
	 * @tc.name testGetActiveUniformBlockParameter
	 * @tc.desc Test getActiveUniformBlockParameter.
	 */
	it('testGetActiveUniformBlockParameter', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 getActiveUniformBlockParameter test start ...' + JSON.stringify(
			gl2));
		const program = globalFunction();
		const uniformLocation = gl.getUniformLocation(program, "a_Position");
		const int32Array = new Int32Array([1, 2]);
		gl2.uniform1iv(uniformLocation, int32Array);
		const blockIndex = gl2.getUniformBlockIndex(program, 'a_Position');
		gl2.uniformBlockBinding(program, blockIndex, 1);
		const blockSize = gl2.getActiveUniformBlockParameter(program, blockIndex, gl
			.UNIFORM_BLOCK_DATA_SIZE);
		console.info("getActiveUniformBlockParameter blockSize" + blockSize);
		const errorCode = gl.getError();
		console.info("jsWebGL getFramebufferAttachmentParameter errorCode: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_VALUE);
		done();
	});

	/**
	 * createProgram
	 * getUniformBlockIndex
	 * getActiveUniformBlockName
	 *
	 */

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0273
	 * @tc.name testGetActiveUniformBlockName
	 * @tc.desc Test getActiveUniformBlockName.
	 */
	it('testGetActiveUniformBlockName', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 getActiveUniformBlockName test start ...' + JSON.stringify(gl2));
		const program = gl.createProgram();
		const blockIndex = gl2.getUniformBlockIndex(program, 'UBOData');
		console.info("getActiveUniformBlockName blockIndex" + blockIndex);

		gl2.uniformBlockBinding(program, blockIndex, 1);
		const blockName = gl2.getActiveUniformBlockName(program, blockIndex);
		console.info("blockName" + blockName);
		expect(blockName).assertEqual('UBOData');
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0274
	 * @tc.name testUniformBlockBinding
	 * @tc.desc Test uniformBlockBinding.
	 */
	it('testUniformBlockBinding', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniformBlockBinding test start ...' + JSON.stringify(gl2));
		const program = gl.createProgram();
		const blockIndex = gl2.getUniformBlockIndex(program, 'UBOData');
		gl2.uniformBlockBinding(program, blockIndex, 1);
		const blockName = gl2.getActiveUniformBlockName(program, blockIndex);
		console.info("blockName" + blockName);

		expect(blockName).assertEqual('UBOData');
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0275
	 * @tc.name testCreateVertexArray
	 * @tc.desc Test createVertexArray.
	 */
	it('testCreateVertexArray', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 createVertexArray test start ...' + JSON.stringify(gl2));
		const vao = gl2.createVertexArray();
		const isVertexArray1 = gl2.isVertexArray(vao);
		console.info("isVertexArray1" + isVertexArray1);
		gl2.bindVertexArray(vao);
		const isVertexArray = gl2.isVertexArray(vao);
		console.info("isVertexArray" + isVertexArray);
		expect(isVertexArray).assertEqual(true);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0276
	 * @tc.name testBindVertexArray
	 * @tc.desc Test bindVertexArray.
	 */
	it('testBindVertexArray', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 bindVertexArray test start ...' + JSON.stringify(gl2));
		const vao = gl2.createVertexArray();
		const isVertexArray1 = gl2.isVertexArray(vao);
		console.info("isVertexArray1" + isVertexArray1);
		gl2.bindVertexArray(vao);
		const isVertexArray = gl2.isVertexArray(vao);
		console.info("isVertexArray" + isVertexArray);
		expect(isVertexArray).assertEqual(true);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0277
	 * @tc.name testDeleteVertexArray
	 * @tc.desc Test deleteVertexArray.
	 */
	it('testDeleteVertexArray', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 deleteVertexArray test start ...' + JSON.stringify(gl2));
		const vao = gl2.createVertexArray();
		gl2.bindVertexArray(vao);
		const isVertexArray1 = gl2.isVertexArray(vao);
		console.info("isVertexArray1" + isVertexArray1);
		gl2.deleteVertexArray(vao);
		const isVertexArray = gl2.isVertexArray(vao);
		console.info("isVertexArray" + isVertexArray);
		expect(isVertexArray).assertEqual(false);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0278
	 * @tc.name testUniform1fvFirst
	 * @tc.desc Test uniform1fv.
	 */
	it('testUniform1fvFirst', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform1fv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		const flaot32list = new Float32Array([1.2, 2.2]);
		gl2.uniform1fv(uniformLocation, flaot32list);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0279
	 * @tc.name testUniform1fvSecond
	 * @tc.desc Test uniform1fv.
	 */
	it('testUniform1fvSecond', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform1fv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		const flaot32list = new Float32Array([1.2, 2.2]);
		gl2.uniform1fv(uniformLocation, flaot32list, 0, 0);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0280
	 * @tc.name testUniform1fvError
	 * @tc.desc Test uniform1fv.
	 */
	it('testUniform1fvError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform1fv test start ...' + JSON.stringify(gl2));
		const programObj = gl.createProgram();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		const flaot32list = new Float32Array([1.2, 2.2]);
		gl2.uniform1fv(uniformLocation, flaot32list);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(1282);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0281
	 * @tc.name testUniform1fvThird
	 * @tc.desc Test uniform1fv.
	 */
	it('testUniform1fvThird', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform1fv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl2.uniform1fv(uniformLocation, [1.2, 2.2]);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0282
	 * @tc.name testUniform1fv4
	 * @tc.desc Test uniform1fv.
	 */
	it('testUniform1fv4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform1fv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl2.uniform1fv(uniformLocation, [1.2, 2.2], 0, 0);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0283
	 * @tc.name testUniform2fvFirst
	 * @tc.desc Test uniform2fv.
	 */
	it('testUniform2fvFirst', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform2fv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		const flaot32list = new Float32Array([1.2, 2.2]);
		gl2.uniform2fv(uniformLocation, flaot32list);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0284
	 * @tc.name testUniform2fvSecond
	 * @tc.desc Test uniform2fv.
	 */
	it('testUniform2fvSecond', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform2fv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		const flaot32list = new Float32Array([1.2, 2.2], 0, 0);
		gl2.uniform2fv(uniformLocation, flaot32list);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0285
	 * @tc.name testUniform2fvError
	 * @tc.desc Test uniform2fv.
	 */
	it('testUniform2fvError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform2fv test start ...' + JSON.stringify(gl2));
		const programObj = gl.createProgram();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		const flaot32list = new Float32Array([1.2, 2.2]);
		gl2.uniform2fv(uniformLocation, flaot32list);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(1282);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0286
	 * @tc.name testUniform2fv3
	 * @tc.desc Test uniform2fv.
	 */
	it('testUniform2fv3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform2fv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl2.uniform2fv(uniformLocation, [1.2, 2.2]);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0287
	 * @tc.name testUniform2fv4
	 * @tc.desc Test uniform2fv.
	 */
	it('testUniform2fv4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform2fv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl2.uniform2fv(uniformLocation, [1.2, 2.2], 0, 0);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0288
	 * @tc.name testUniform3fvFirst
	 * @tc.desc Test frontFace.
	 */
	it('testUniform3fvFirst', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform3fv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		const flaot32list = new Float32Array([1.2, 2.2]);
		gl2.uniform3fv(uniformLocation, flaot32list);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0289
	 * @tc.name testUniform3fvSecond
	 * @tc.desc Test uniform3fv.
	 */
	it('testUniform3fvSecond', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform3fv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		const flaot32list = new Float32Array([1.2, 2.2]);
		gl2.uniform3fv(uniformLocation, flaot32list, 0, 0);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0290
	 * @tc.name testUniform3fvError
	 * @tc.desc Test uniform3fv.
	 */
	it('testUniform3fvError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform3fv test start ...' + JSON.stringify(gl2));
		const programObj = gl.createProgram();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		const flaot32list = new Float32Array([1.2, 2.2]);
		gl2.uniform3fv(uniformLocation, flaot32list);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(1282);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0291
	 * @tc.name testUniform3fv3
	 * @tc.desc Test uniform3fv.
	 */
	it('testUniform3fv3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform3fv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl2.uniform3fv(uniformLocation, [1.2, 2.2]);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0292
	 * @tc.name testUniform3fv4
	 * @tc.desc Test uniform3fv.
	 */
	it('testUniform3fv4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform3fv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl2.uniform3fv(uniformLocation, [1.2, 2.2], 0, 0);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0293
	 * @tc.name testUniform4fvFirst
	 * @tc.desc Test uniform4fv.
	 */
	it('testUniform4fvFirst', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform4fv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		const flaot32list = new Float32Array([1.2, 2.2]);
		gl2.uniform4fv(uniformLocation, flaot32list);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0294
	 * @tc.name testUniform4fvSecond
	 * @tc.desc Test uniform4fv.
	 */
	it('testUniform4fvSecond', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform4fv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		const flaot32list = new Float32Array([1.2, 2.2]);
		gl2.uniform4fv(uniformLocation, flaot32list, 0, 0);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0295
	 * @tc.name testUniform4fvError
	 * @tc.desc Test uniform4fv.
	 */
	it('testUniform4fvError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform4fv test start ...' + JSON.stringify(gl2));
		const programObj = gl.createProgram();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		const flaot32list = new Float32Array([1.2, 2.2]);
		gl2.uniform4fv(uniformLocation, flaot32list);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(1282);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0296
	 * @tc.name testUniform4fv3
	 * @tc.desc Test uniform4fv.
	 */
	it('testUniform4fv3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform4fv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl2.uniform4fv(uniformLocation, [1.2, 2.2]);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0297
	 * @tc.name testUniform4fvThird
	 * @tc.desc Test uniform4fv.
	 */
	it('testUniform4fvThird', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform4fv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl2.uniform4fv(uniformLocation, [1.2, 2.2], 0, 0);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0298
	 * @tc.name testUniform1ivFirst
	 * @tc.desc Test uniform1iv.
	 */
	it('testUniform1ivFirst', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform1iv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		const int32Array = new Int32Array([1, 2]);
		gl2.uniform1iv(uniformLocation, int32Array);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0299
	 * @tc.name testUniform1ivSecond
	 * @tc.desc Test uniform1iv.
	 */
	it('testUniform1ivSecond', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform1iv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		const int32Array = new Int32Array([1, 2]);
		gl2.uniform1iv(uniformLocation, int32Array, 0, 0);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0300
	 * @tc.name testUniform1ivError
	 * @tc.desc Test uniform1iv.
	 */
	it('testUniform1ivError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform1iv test start ...' + JSON.stringify(gl2));
		const programObj = gl.createProgram();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		const int32Array = new Int32Array([1, 2]);
		gl2.uniform1iv(uniformLocation, int32Array);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(1282);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0301
	 * @tc.name testUniform1iv3
	 * @tc.desc Test uniform1iv.
	 */
	it('testUniform1iv3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform1iv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl2.uniform1iv(uniformLocation, [1, 2]);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0302
	 * @tc.name testUniform1iv4
	 * @tc.desc Test uniform1iv.
	 */
	it('testUniform1iv4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform1iv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl2.uniform1iv(uniformLocation, [1, 2], 0, 0);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0303
	 * @tc.name testUniform2ivFirst
	 * @tc.desc Test uniform2iv.
	 */
	it('testUniform2ivFirst', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform2iv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		const int32Array = new Int32Array([1, 2]);
		gl2.uniform2iv(uniformLocation, int32Array);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0304
	 * @tc.name testUniform2ivSecond
	 * @tc.desc Test uniform2iv.
	 */
	it('testUniform2ivSecond', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform2iv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		const int32Array = new Int32Array([1, 2]);
		gl2.uniform2iv(uniformLocation, int32Array, 0, 0);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0305
	 * @tc.name testUniform2ivError
	 * @tc.desc Test uniform2iv.
	 */
	it('testUniform2ivError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform2iv test start ...' + JSON.stringify(gl2));
		const programObj = gl.createProgram();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		const int32Array = new Int32Array([1, 2]);
		gl2.uniform2iv(uniformLocation, int32Array);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(1282);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0306
	 * @tc.name testUniform2iv3
	 * @tc.desc Test uniform2iv.
	 */
	it('testUniform2iv3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform2iv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl2.uniform2iv(uniformLocation, [1, 2]);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0307
	 * @tc.name testUniform2iv4
	 * @tc.desc Test uniform2iv.
	 */
	it('testUniform2iv4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform2iv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl2.uniform2iv(uniformLocation, [1, 2], 0, 0);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0308
	 * @tc.name testUniform3ivFirst
	 * @tc.desc Test uniform3iv.
	 */
	it('testUniform3ivFirst', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform3iv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		const int32Array = new Int32Array([1, 2]);
		gl2.uniform3iv(uniformLocation, int32Array);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0309
	 * @tc.name testUniform3ivSecond
	 * @tc.desc Test uniform3iv.
	 */
	it('testUniform3ivSecond', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform3iv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		const int32Array = new Int32Array([1, 2]);
		gl2.uniform3iv(uniformLocation, int32Array, 0, 0);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0310
	 * @tc.name testUniform3ivError
	 * @tc.desc Test uniform3iv.
	 */
	it('testUniform3ivError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform3iv test start ...' + JSON.stringify(gl2));
		const programObj = gl.createProgram();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		const int32Array = new Int32Array([1, 2]);
		gl2.uniform3iv(uniformLocation, int32Array);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(1282);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0311
	 * @tc.name testUniform3iv3
	 * @tc.desc Test uniform3iv.
	 */
	it('testUniform3iv3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform3iv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl2.uniform3iv(uniformLocation, [1, 2]);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0312
	 * @tc.name testUniform3iv4
	 * @tc.desc Test uniform3iv.
	 */
	it('testUniform3iv4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform3iv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl2.uniform3iv(uniformLocation, [1, 2], 0, 0);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0313
	 * @tc.name testUniform4ivFirst
	 * @tc.desc Test uniform4iv.
	 */
	it('testUniform4ivFirst', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform4iv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		const int32Array = new Int32Array([1, 2]);
		gl2.uniform4iv(uniformLocation, int32Array);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0314
	 * @tc.name testUniform4ivSecond
	 * @tc.desc Test uniform4iv.
	 */
	it('testUniform4ivSecond', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform4iv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		const int32Array = new Int32Array([1, 2]);
		gl2.uniform4iv(uniformLocation, int32Array, 0, 0);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0315
	 * @tc.name testUniform4ivError
	 * @tc.desc Test uniform4iv.
	 */
	it('testUniform4ivError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform4iv test start ...' + JSON.stringify(gl2));
		const programObj = gl.createProgram();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		const int32Array = new Int32Array([1, 2]);
		gl2.uniform4iv(uniformLocation, int32Array);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(1282);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0316
	 * @tc.name testUniform4iv3
	 * @tc.desc Test uniform4iv.
	 */
	it('testUniform4iv3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform4iv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl2.uniform4iv(uniformLocation, [1, 2]);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0317
	 * @tc.name testUniform4iv4
	 * @tc.desc Test uniform4iv.
	 */
	it('testUniform4iv4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform4iv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl2.uniform4iv(uniformLocation, [1, 2], 0, 0);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0318
	 * @tc.name testUniformMatrix4fvFirst
	 * @tc.desc Test uniformMatrix4fv.
	 */
	it('testUniformMatrix4fvFirst', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniformMatrix4fv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		var flaot32list = new Float32Array([1, 2]);
		gl2.uniformMatrix4fv(uniformLocation, true, flaot32list);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0319
	 * @tc.name testUniformMatrix4fv2
	 * @tc.desc Test uniformMatrix4fv.
	 */
	it('testUniformMatrix4fv2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniformMatrix4fv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		var flaot32list = new Float32Array([1, 2]);
		gl2.uniformMatrix4fv(uniformLocation, true, flaot32list, 0, 0);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0320
	 * @tc.name testUniformMatrix4fvError
	 * @tc.desc Test uniformMatrix4fv.
	 */
	it('testUniformMatrix4fvError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniformMatrix4fv test start ...' + JSON.stringify(gl2));
		const programObj = gl.createProgram();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		var flaot32list = new Float32Array([1, 2]);
		gl2.uniformMatrix4fv(uniformLocation, true, flaot32list);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(1282);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0321
	 * @tc.name testUniformMatrix4fv3
	 * @tc.desc Test uniformMatrix4fv.
	 */
	it('testUniformMatrix4fv3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniformMatrix4fv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl2.uniformMatrix4fv(uniformLocation, true, [1, 2]);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0322
	 * @tc.name testUniformMatrix4fv4
	 * @tc.desc Test uniformMatrix4fv.
	 */
	it('testUniformMatrix4fv4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniformMatrix4fv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl2.uniformMatrix4fv(uniformLocation, true, [1, 2], 0, 0);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0323
	 * @tc.name testUniformMatrix3fvFirst
	 * @tc.desc Test uniformMatrix3fv.
	 */
	it('testUniformMatrix3fvFirst', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniformMatrix3fv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		var flaot32list = new Float32Array([1, 2]);
		gl2.uniformMatrix3fv(uniformLocation, true, flaot32list);

		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0324
	 * @tc.name testUniformMatrix3fv2
	 * @tc.desc Test uniformMatrix3fv.
	 */
	it('testUniformMatrix3fv2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniformMatrix3fv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		var flaot32list = new Float32Array([1, 2]);
		gl2.uniformMatrix3fv(uniformLocation, true, flaot32list, 0, 0);

		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0325
	 * @tc.name testUniformMatrix3fvError
	 * @tc.desc Test uniformMatrix3fv.
	 */
	it('testUniformMatrix3fvError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniformMatrix3fv test start ...' + JSON.stringify(gl2));
		const programObj = gl.createProgram();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		var flaot32list = new Float32Array([1, 2]);
		gl2.uniformMatrix3fv(uniformLocation, true, flaot32list);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(1282);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0326
	 * @tc.name testUniformMatrix3fv3
	 * @tc.desc Test uniformMatrix3fv.
	 */
	it('testUniformMatrix3fv3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniformMatrix3fv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl2.uniformMatrix3fv(uniformLocation, true, [1, 2]);

		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0327
	 * @tc.name testUniformMatrix3fv4
	 * @tc.desc Test uniformMatrix3fv.
	 */
	it('testUniformMatrix3fv4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniformMatrix3fv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl2.uniformMatrix3fv(uniformLocation, true, [1, 2], 0, 0);

		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0328
	 * @tc.name testUniformMatrix2fv1
	 * @tc.desc Test uniformMatrix2fv.
	 */
	it('testUniformMatrix2fv1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniformMatrix2fv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		var flaot32list = new Float32Array([1, 2]);
		gl2.uniformMatrix2fv(uniformLocation, true, flaot32list);

		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0329
	 * @tc.name testUniformMatrix2fv2
	 * @tc.desc Test uniformMatrix2fv.
	 */
	it('testUniformMatrix2fv2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniformMatrix2fv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		var flaot32list = new Float32Array([1, 2]);
		gl2.uniformMatrix2fv(uniformLocation, true, flaot32list, 0, 0);

		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0330
	 * @tc.name testUniformMatrix2fvError
	 * @tc.desc Test uniformMatrix2fv.
	 */
	it('testUniformMatrix2fvError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniformMatrix2fv test start ...' + JSON.stringify(gl2));
		const programObj = gl.createProgram();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		var flaot32list = new Float32Array([1, 2]);
		gl2.uniformMatrix2fv(uniformLocation, true, flaot32list);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(1282);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0331
	 * @tc.name testUniformMatrix2fv3
	 * @tc.desc Test uniformMatrix2fv.
	 */
	it('testUniformMatrix2fv3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniformMatrix2fv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl2.uniformMatrix2fv(uniformLocation, true, [1, 2]);

		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0332
	 * @tc.name testUniformMatrix2fv4
	 * @tc.desc Test uniformMatrix2fv.
	 */
	it('testUniformMatrix2fv4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniformMatrix2fv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl2.uniformMatrix2fv(uniformLocation, true, [1, 2], 0, 0);

		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0333
	 * @tc.name testBufferData1
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 bufferData test start ...' + JSON.stringify(gl2));
		const buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl2.bufferData(gl.ARRAY_BUFFER, 1024, gl.STATIC_DRAW);
		const bufferSize = gl.getBufferParameter(gl.ARRAY_BUFFER, gl.BUFFER_SIZE);
		console.info('bufferSize' + bufferSize);
		expect(bufferSize).assertEqual(1024);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0334
	 * @tc.name testBufferData2
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 bufferData test start ...' + JSON.stringify(gl2));
		const buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl2.bufferData(gl.ARRAY_BUFFER, 1024, gl.STATIC_DRAW);
		const bufferUsage = gl.getBufferParameter(gl.ARRAY_BUFFER, gl.BUFFER_USAGE);
		console.info('bufferUsage' + bufferUsage);
		expect(bufferUsage).assertEqual(gl.STATIC_DRAW);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0335
	 * @tc.name testBufferData3
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 bufferData test start ...' + JSON.stringify(gl2));
		gl2.bufferData(gl.ELEMENT_ARRAY_BUFFER, new ArrayBuffer(8), gl.DYNAMIC_READ);
		const errorCode = gl.getError();
		console.info("webgltest bufferData getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0336
	 * @tc.name testBufferData4
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 bufferData test start ...' + JSON.stringify(gl2));
		gl2.bufferData(gl.ELEMENT_ARRAY_BUFFER, new ArrayBuffer(8), gl.DYNAMIC_READ, 0, 0);
		const errorCode = gl.getError();
		console.info("webgl2test bufferData getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0337
	 * @tc.name testBufferDataError
	 * @tc.desc Test bufferData.
	 */
	it('testBufferDataError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 bufferData test start ...' + JSON.stringify(gl2));
		gl2.bufferData(1024, gl.STATIC_DRAW);
		const bufferSize = gl.getBufferParameter(gl.ARRAY_BUFFER, gl.BUFFER_SIZE);
		console.info('bufferSize' + bufferSize);
		const type = (bufferSize === 1024)
		expect(type).assertEqual(true);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0338
	 * @tc.name testBufferSubData
	 * @tc.desc Test bufferData.
	 */
	it('testBufferSubData', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 bufferData test start ...' + JSON.stringify(gl2));
		gl2.bufferSubData(gl.COPY_READ_BUFFER, 512, new ArrayBuffer(8));
		const errorCode = gl.getError();
		console.info("webgl2test bufferSubData getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0339
	 * @tc.name testBufferSubData2
	 * @tc.desc Test bufferData.
	 */
	it('testBufferSubData2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 bufferData test start ...' + JSON.stringify(gl2));
		gl2.bufferSubData(gl.COPY_READ_BUFFER, 512, new ArrayBuffer(8), 0, 0);
		const errorCode = gl.getError();
		console.info("webgl2test bufferSubData getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0340
	 * @tc.name testTexImage2D
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texImage2D(gl2.TEXTURE_2D, 0, 32, 512, 512, 0, 32, 32, view);
		const errorCode = gl.getError();
		console.info("webgl2test texImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0341
	 * @tc.name testTexImage2D2
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texImage2D test start ...' + JSON.stringify(gl2));
		gl2.texImage2D(gl2.TEXTURE_2D, 0, gl.RGB8, 512, 512, 0, gl.RGB, gl.UNSIGNED_BYTE, 0);
		const errorCode = gl.getError();
		console.info("webgl2test texImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0342
	 * @tc.name testTexImage2D3
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texImage2D(gl2.TEXTURE_2D, 0, gl.R16F, 512, 512, 0, gl.RED, gl.FLOAT, view, 0);
		const errorCode = gl.getError();
		console.info("webgl2test texImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0343
	 * @tc.name testTexImage2D4
	 * @tc.desc Test frontFace.
	 */
	it('testTexImage2D4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texImage2D(gl2.TEXTURE_2D, 0, gl.R16F, 512, 512, 0, gl.RED, gl.FLOAT, view, 0);
		const errorCode = gl.getError();
		console.info("webgl2test texImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0344
	 * @tc.name testTexSubImage2D5
	 * @tc.desc Test texSubImage2D.
	 */
	it('testTexSubImage2D5', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texSubImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texSubImage2D(gl2.TEXTURE_2D, 0, 0, 0, 512, 512, gl.RED, gl.FLOAT, view, 0);
		const errorCode = gl.getError();
		console.info("webgl2test texSubImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0345
	 * @tc.name testTexSubImage2D6
	 * @tc.desc Test texSubImage2D.
	 */
	it('testTexSubImage2D6', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texSubImage2D test start ...' + JSON.stringify(gl2));
		gl2.texSubImage2D(gl2.TEXTURE_2D, 0, 0, 0, 512, 512, gl.RED, gl.FLOAT, 0);
		const errorCode = gl.getError();
		console.info("webgl2test texSubImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0346
	 * @tc.name testTexSubImage2D7
	 * @tc.desc Test texSubImage2D.
	 */
	it('testTexSubImage2D7', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texSubImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texSubImage2D(gl2.TEXTURE_2D, 0, 0, 0, 512, 512, gl.RGBA, gl.UNSIGNED_BYTE, view);
		const errorCode = gl.getError();
		console.info("webgl2test texSubImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0347
	 * @tc.name testTexSubImage2D8
	 * @tc.desc Test texSubImage2D.
	 */
	it('testTexSubImage2D8', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texSubImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texSubImage2D(gl2.TEXTURE_2D, 0, 0, 0, 512, 512, gl.RGBA, gl.UNSIGNED_BYTE, view);
		const errorCode = gl.getError();
		console.info("webgl2test texSubImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0348
	 * @tc.name testCompressedTexImage2D9
	 * @tc.desc Test compressedTexImage2D.
	 */
	it('testCompressedTexImage2D9', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 compressedTexImage2D test start ...' + JSON.stringify(gl2));
		gl2.compressedTexImage2D(gl2.TEXTURE_2D, 0, 0x83F3, 512, 512, 0, gl.PIXEL_UNPACK_BUFFER, 0);
		const errorCode = gl.getError();
		console.info("webgl2test compressedTexImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0349
	 * @tc.name testCompressedTexImage2D11
	 * @tc.desc Test compressedTexImage2D.
	 */
	it('testCompressedTexImage2D11', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 compressedTexImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.compressedTexImage2D(gl2.TEXTURE_2D, 0, 0x83F3, 512, 512, 0, view, 0, 0);
		const errorCode = gl.getError();
		console.info("webgl2test compressedTexImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0350
	 * @tc.name testCompressedTexSubImage2D12
	 * @tc.desc Test compressedTexSubImage2D.
	 */
	it('testCompressedTexSubImage2D12', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 compressedTexSubImage2D test start ...' + JSON.stringify(gl2));
		gl2.compressedTexSubImage2D(gl2.TEXTURE_2D, 0, 256, 256, 512, 512, 0x83F3, gl
			.PIXEL_UNPACK_BUFFER, 0);
		const errorCode = gl.getError();
		console.info("webgl2test compressedTexSubImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0351
	 * @tc.name testCompressedTexSubImage2D13
	 * @tc.desc Test compressedTexSubImage2D.
	 */
	it('testCompressedTexSubImage2D13', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 compressedTexSubImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.compressedTexSubImage2D(gl2.TEXTURE_2D, 0, 256, 256, 512, 512, 0x83F3, view, 0, 0);
		const errorCode = gl.getError();
		console.info("webgl2test compressedTexSubImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0352
	 * @tc.name testReadPixelsFirst
	 * @tc.desc Test readPixels.
	 */
	it('testReadPixelsFirst', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 readPixels test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.readPixels(0, 0, 512, 512, gl2.RGBA, gl2.UNSIGNED_BYTE, view);
		const errorCode = gl.getError();
		console.info("webgl2test readPixels getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0353
	 * @tc.name testReadPixelsSecond
	 * @tc.desc Test readPixels.
	 */
	it('testReadPixelsSecond', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 readPixels test start ...' + JSON.stringify(gl2));
		gl2.readPixels(0, 0, 512, 512, gl2.RGBA, gl2.UNSIGNED_BYTE, 0);
		const errorCode = gl.getError();
		console.info("webgl2test readPixels getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0354
	 * @tc.name testReadPixelsThird
	 * @tc.desc Test readPixels.
	 */
	it('testReadPixelsThird', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 readPixels test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.readPixels(0, 0, 512, 512, gl2.RGBA, gl2.UNSIGNED_BYTE, view, 0);
		const errorCode = gl.getError();
		console.info("webgl2test readPixels getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0355
	 * @tc.name testReadPixelsError
	 * @tc.desc Test readPixels.
	 */
	it('testReadPixelsError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 readPixels test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		const returnVale = gl2.readPixels(0, 0, 512, 512, gl2.RGBA, gl2.UNSIGNED_BYTE, view, 0);
		const errorCode = gl.getError();
		console.info("webgl2test readPixels getError: " + errorCode);
		const type =
			expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});



	// ***************************************** GT ************************************************


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0356
	 * @tc.name testIsFramebuffer
	 * @tc.desc Test isFramebuffer.
	 */
	it('testIsFramebuffer', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testIsFramebuffer test start ...66');
			var framebuffer = gl.createFramebuffer();
			const isFramebuffer = gl.isFramebuffer(framebuffer);
			console.info("createFramebuffer --> isFramebuffer: " + isFramebuffer);
			expect(isFramebuffer).assertEqual(false);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testIsFramebuffer has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0357
	 * @tc.name testIsFramebuffer_01
	 * @tc.desc Test isFramebuffer.
	 */
	it('testIsFramebuffer_01', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testIsFramebuffer_01 test start ...66');
			var framebuffer = gl.createFramebuffer();
			gl.bindFramebuffer(gl.FRAMEBUFFER, framebuffer);
			const isFramebuffer = gl.isFramebuffer(framebuffer);
			console.info("createFramebuffer --> bindFramebuffer --> isFramebuffer: " +
				isFramebuffer);
			expect(isFramebuffer).assertEqual(true);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testIsFramebuffer_01 has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0358
	 * @tc.name testIsFramebuffer_02
	 * @tc.desc Test isFramebuffer.
	 */
	it('testIsFramebuffer_02', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testIsFramebuffer_02 test start ...66');
			var programobject = gl.createProgram();
			const isFramebuffer = gl.isFramebuffer(programobject);
			console.info("createFramebuffer --> isFramebuffer: " + isFramebuffer);
			expect(isFramebuffer).assertEqual(false);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testIsFramebuffer_02 has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0359
	 * @tc.name testIsFramebuffer_03
	 * @tc.desc Test isFramebuffer.
	 */
	it('testIsFramebuffer_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testIsFramebuffer_03 test start ...66');
		var shader = gl.createShader(gl.VERTEX_SHADER)
		const isFramebuffer = gl.isFramebuffer(shader);
		console.info("createFramebuffer --> isFramebuffer: " + isFramebuffer);
		expect(isFramebuffer).assertEqual(false);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0360
	 * @tc.name testIsFramebuffer_04
	 * @tc.desc Test isFramebuffer.
	 */
	it('testIsFramebuffer_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testIsFramebuffer_04 test start ...66');
		var renderbufferObject = gl.createRenderbuffer();
		const isFramebuffer = gl.isFramebuffer(renderbufferObject);
		console.info("createFramebuffer --> isFramebuffer: " + isFramebuffer);
		expect(isFramebuffer).assertEqual(false);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0361
	 * @tc.name testIsProgram
	 * @tc.desc Test isProgram.
	 */
	it('testIsProgram', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testIsProgram test start ...66');
		var program = gl.createProgram();
		const isProgram = gl.isProgram(program);
		console.info("createProgram --> isProgram: " + isProgram);
		expect(isProgram).assertEqual(true);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0362
	 * @tc.name testIsProgram_01
	 * @tc.desc Test isProgram.
	 */
	it('testIsProgram_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testIsProgram_01 test start ...66');
		var framebuffer = gl.createFramebuffer();
		const programError = gl.getError();
		console.info("createProgram --> programError: " + programError);
		const isProgram = gl.isProgram(framebuffer);
		console.info("createProgram --> isProgram: " + isProgram);
		expect(isProgram).assertEqual(true);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0363
	 * @tc.name testIsProgram_02
	 * @tc.desc Test isProgram.
	 */
	it('testIsProgram_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testIsProgram_02 test start ...66');
		var renderbuffer = gl.createRenderbuffer();
		const programError = gl.getError();
		console.info("createProgram --> programError: " + programError);
		const isProgram = gl.isProgram(renderbuffer);
		console.info("createProgram --> isProgram: " + isProgram);
		expect(isProgram).assertEqual(false);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0364
	 * @tc.name testIsProgram_03
	 * @tc.desc Test isProgram.
	 */
	it('testIsProgram_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testIsProgram_03 test start ...66');
		var shader = gl.createShader(gl.VERTEX_SHADER);
		const programError = gl.getError();
		console.info("createProgram --> programError: " + programError);
		const isProgram = gl.isProgram(shader);
		console.info("createProgram --> isProgram: " + isProgram);
		expect(isProgram).assertEqual(false);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0365
	 * @tc.name testIsProgram_04
	 * @tc.desc Test isProgram.
	 */
	it('testIsProgram_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testIsProgram_04 test start ...66');
		var texture = gl.createTexture();
		const programError = gl.getError();
		console.info("createProgram --> programError: " + programError);
		const isProgram = gl.isProgram(texture);
		console.info("createProgram --> isProgram: " + isProgram);
		expect(isProgram).assertEqual(true);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0366
	 * @tc.name testIsRenderbuffer
	 * @tc.desc Test isRenderbuffer.
	 */
	it('testIsRenderbuffer', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testIsRenderbuffer test start ...66');
		var renderbuffer = gl.createRenderbuffer();
		const isrenderbuffer = gl.isRenderbuffer(renderbuffer);
		console.info("createRenderbuffer --> isRenderbuffer: " + isrenderbuffer);
		expect(isrenderbuffer).assertEqual(false);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0367
	 * @tc.name testIsRenderbuffer_01
	 * @tc.desc Test isRenderbuffer.
	 */
	it('testIsRenderbuffer_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testIsRenderbuffer_01 test start ...66');
		var renderbuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderbuffer);
		const isrenderbuffer = gl.isRenderbuffer(renderbuffer);
		console.info("createRenderbuffer --> isRenderbuffer: " + isrenderbuffer);
		expect(isrenderbuffer).assertEqual(true);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0368
	 * @tc.name testIsRenderbuffer_02
	 * @tc.desc Test isRenderbuffer.
	 */
	it('testIsRenderbuffer_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testIsRenderbuffer_02 test start ...66');
		var framebuffer = gl.createFramebuffer();
		const isrenderbuffer = gl.isRenderbuffer(framebuffer);
		console.info("createRenderbuffer --> isRenderbuffer: " + isrenderbuffer);
		expect(isrenderbuffer).assertEqual(true);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0369
	 * @tc.name testIsRenderbuffer_03
	 * @tc.desc Test isRenderbuffer.
	 */
	it('testIsRenderbuffer_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testIsRenderbuffer_03 test start ...66');
		var programobject = gl.createProgram();
		const isrenderbuffer = gl.isRenderbuffer(programobject);
		console.info("createRenderbuffer --> isRenderbuffer: " + isrenderbuffer);
		expect(isrenderbuffer).assertEqual(false);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0370
	 * @tc.name testIsRenderbuffer_04
	 * @tc.desc Test isRenderbuffer.
	 */
	it('testIsRenderbuffer_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testIsRenderbuffer_04 test start ...66');
		var texture = gl.createTexture();
		const isrenderbuffer = gl.isRenderbuffer(texture);
		console.info("createRenderbuffer --> isRenderbuffer: " + isrenderbuffer);
		expect(isrenderbuffer).assertEqual(true);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0371
	 * @tc.name testIsShader
	 * @tc.desc Test isShader.
	 */
	it('testIsShader', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testIsShader test start ...66');
		var shader = gl.createShader(gl.VERTEX_SHADER)
		const isShader = gl.isShader(shader);
		console.info("createShader --> isShader: " + isShader);
		expect(isShader).assertEqual(true);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0372
	 * @tc.name testIsShader_01
	 * @tc.desc Test isShader.
	 */
	it('testIsShader_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testIsShader_01 test start ...66');
		var shader = gl.createShader(gl.FRAGMENT_SHADER)
		const isShader = gl.isShader(shader);
		console.info("createShader --> isShader: " + isShader);
		expect(isShader).assertEqual(true);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0373
	 * @tc.name testIsShader_02
	 * @tc.desc Test isShader.
	 */
	it('testIsShader_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testIsShader_02 test start ...66');
		var renderbuffer = gl.createRenderbuffer();
		const isShader = gl.isShader(renderbuffer);
		console.info("createShader --> isShader: " + isShader);
		expect(isShader).assertEqual(true);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0374
	 * @tc.name testIsShader_03
	 * @tc.desc Test isShader.
	 */
	it('testIsShader_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testIsShader_03 test start ...66');
		var framebuffer = gl.createFramebuffer();
		const isShader = gl.isShader(framebuffer);
		console.info("createShader --> isShader: " + isShader);
		expect(isShader).assertEqual(true);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0375
	 * @tc.name testIsShader_04
	 * @tc.desc Test isShader.
	 */
	it('testIsShader_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testIsShader_04 test start ...66');
		var programobject = gl.createProgram();
		const isShader = gl.isShader(programobject);
		console.info("createShader --> isShader: " + isShader);
		expect(isShader).assertEqual(false);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0376
	 * @tc.name testIsTexture
	 * @tc.desc Test isTexture.
	 */
	it('testIsTexture', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testIsTexture test start ...66');
		var texture = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, texture);
		const isTexture = gl.isTexture(texture);
		console.info("createShader --> isTexture: " + isTexture);
		expect(isTexture).assertEqual(true);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0377
	 * @tc.name testIsTexture_01
	 * @tc.desc Test isTexture.
	 */
	it('testIsTexture_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testIsTexture_01 test start ...66');
		var texture = gl.createTexture();
		const isTexture = gl.isTexture(texture);
		console.info("createShader --> isTexture: " + isTexture);
		expect(isTexture).assertEqual(false);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0378
	 * @tc.name testIsTexture_02
	 * @tc.desc Test isTexture.
	 */
	it('testIsTexture_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testIsTexture_02 test start ...66');
		var programobject = gl.createProgram();
		const isTexture = gl.isTexture(programobject);
		console.info("createShader --> isTexture: " + isTexture);
		expect(isTexture).assertEqual(false);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0379
	 * @tc.name testIsTexture_03
	 * @tc.desc Test isTexture.
	 */
	it('testIsTexture_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testIsTexture_03 test start ...66');
		var renderbuffer = gl.createRenderbuffer();
		const isTexture = gl.isTexture(renderbuffer);
		console.info("createShader --> isTexture: " + isTexture);
		expect(isTexture).assertEqual(false);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0380
	 * @tc.name testIsTexture_04
	 * @tc.desc Test isTexture.
	 */
	it('testIsTexture_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testIsTexture_04 test start ...66');
		var framebuffer = gl.createFramebuffer();
		const isTexture = gl.isTexture(framebuffer);
		console.info("createShader --> isTexture: " + isTexture);
		expect(isTexture).assertEqual(false);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0381
	 * @tc.name testGetShaderParameter_1
	 * @tc.desc Test getShaderParameter.
	 */
	it('testGetShaderParameter_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getShaderParameter test start ...' + JSON.stringify(gl));
		const vertexShader = gl.createShader(gl.VERTEX_SHADER);
		//        const fragmentShader = gl.createShader(gl.FRAGMENT_SHADER);
		gl.deleteShader(vertexShader);
		const shaderType = gl.getShaderParameter(vertexShader, gl.SHADER_TYPE);
		console.info('shaderType' + shaderType);
		expect(shaderType).assertEqual(120);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0382
	 * @tc.name testLineWidth
	 * @tc.desc Test lineWidth.
	 */
	it('testLineWidth', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testLineWidth test start ...66');
		gl.lineWidth(5);
		const windtherror = gl.getError();
		console.info("windtherror: " + windtherror);
		expect(windtherror).assertEqual(gl.INVALID_VALUE);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0383
	 * @tc.name testLineWidth_01
	 * @tc.desc Test lineWidth.
	 */
	it('testLineWidth_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testLineWidth_01 test start ...66');
		gl.lineWidth(-1);
		const windtherror = gl.getError();
		console.info("windtherror: " + windtherror);
		expect(windtherror).assertEqual(gl.INVALID_VALUE);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0384
	 * @tc.name testLineWidth_02
	 * @tc.desc Test lineWidth.
	 */
	it('testLineWidth_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testLineWidth_02 test start ...66');
		gl.lineWidth(0);
		const windtherror = gl.getError();
		console.info("windtherror: " + windtherror);
		expect(windtherror).assertEqual(gl.INVALID_VALUE);
		done();
	})


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0385
	 * @tc.name testLinkProgram
	 * @tc.desc Test linkProgram.
	 */
	it('testLinkProgram', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testLinkProgram test start ...66');
		console.info('jsWebGL testLinkProgram test start ...' + JSON.stringify(gl));
		var shaderProg = gl.createProgram();
		gl.linkProgram(shaderProg);
		const linkProgramError = gl.getError();
		console.info("linkProgramError: " + linkProgramError);
		expect(linkProgramError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0386
	 * @tc.name testLinkProgram_01
	 * @tc.desc Test linkProgram.
	 */
	it('testLinkProgram_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testLinkProgram_01 test start ...66');
		var renderbuffer = gl.createRenderbuffer();
		gl.linkProgram(renderbuffer);
		const linkProgramError = gl.getError();
		console.info("linkProgramError: " + linkProgramError);
		expect(linkProgramError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0387
	 * @tc.name testLinkProgram_02
	 * @tc.desc Test linkProgram.
	 */
	it('testLinkProgram_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testLinkProgram_02 test start ...66');
		const framebuffer = gl.createFramebuffer();
		gl.linkProgram(framebuffer);
		const linkProgramError = gl.getError();
		console.info("linkProgramError: " + linkProgramError);
		expect(linkProgramError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0388
	 * @tc.name testLinkProgram_03
	 * @tc.desc Test linkProgram.
	 */
	it('testLinkProgram_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testLinkProgram_03 test start ...66');
		var shader = gl.createShader(gl.VERTEX_SHADER);
		gl.linkProgram(shader);
		const linkProgramError = gl.getError();
		console.info("linkProgramError: " + linkProgramError);
		expect(linkProgramError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0389
	 * @tc.name testLinkProgram_04
	 * @tc.desc Test linkProgram.
	 */
	it('testLinkProgram_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testLinkProgram_04 test start ...66');
		gl.linkProgram("-123");
		const linkProgramError = gl.getError();
		console.info("linkProgramError: " + linkProgramError);
		expect(linkProgramError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0390
	 * @tc.name testPixelStorei
	 * @tc.desc Test pixelStorei.
	 */
	it('testPixelStorei', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testPixelStorei test start ...66');
		var tex = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, tex);
		gl.pixelStorei(gl.PACK_ALIGNMENT, 4);
		gl.pixelStorei(gl.UNPACK_ALIGNMENT, 8);
		const packValue = gl.getParameter(gl.PACK_ALIGNMENT);
		const unpackValue = gl.getParameter(gl.UNPACK_ALIGNMENT);
		console.info("packValue: " + packValue);
		console.info("unpackValue: " + unpackValue);
		const pixelStoreiError = gl.getError();
		console.info("pixelStoreiError: " + pixelStoreiError);
		expect(pixelStoreiError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0391
	 * @tc.name testPixelStorei_01
	 * @tc.desc Test pixelStorei.
	 */
	it('testPixelStorei_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testPixelStorei_01 test start ...66');
		var tex = gl.createTexture();
		gl.pixelStorei(gl.UNPACK_ALIGNMENT, 8);
		const pixelStoreiError = gl.getError();
		console.info("pixelStoreiError: " + pixelStoreiError);
		expect(pixelStoreiError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0392
	 * @tc.name testPixelStorei_02
	 * @tc.desc Test pixelStorei.
	 */
	it('testPixelStorei_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testPixelStorei_02 test start ...66');
		var tex = gl.createTexture();
		gl.pixelStorei(gl.LINE_LOOP, 8);
		const pixelStoreiError = gl.getError();
		console.info("pixelStoreiError: " + pixelStoreiError);
		expect(pixelStoreiError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0393
	 * @tc.name testPixelStorei_03
	 * @tc.desc Test pixelStorei.
	 */
	it('testPixelStorei_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testPixelStorei_03 test start ...66');
		var tex = gl.createTexture();
		gl.pixelStorei(gl.ONE_MINUS_SRC_COLOR, 8);
		const pixelStoreiError = gl.getError();
		console.info("pixelStoreiError: " + pixelStoreiError);
		expect(pixelStoreiError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0394
	 * @tc.name testPixelStorei_04
	 * @tc.desc Test pixelStorei.
	 */
	it('testPixelStorei_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testPixelStorei_04 test start ...66');
		var tex = gl.createTexture();
		gl.pixelStorei(gl.ONE_MINUS_SRC_COLOR, -1);
		const pixelStoreiError = gl.getError();
		console.info("pixelStoreiError: " + pixelStoreiError);
		expect(pixelStoreiError).assertEqual(gl.INVALID_VALUE);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0395
	 * @tc.name testPolygonOffset
	 * @tc.desc Test polygonOffset.
	 */
	it('testPolygonOffset', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testPolygonOffset test start ...66');
		gl.enable(gl.POLYGON_OFFSET_FILL);
		gl.polygonOffset(2, 3);
		const polygonOffsetError = gl.getError();
		console.info("polygonOffsetError: " + polygonOffsetError);
		expect(polygonOffsetError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0396
	 * @tc.name testPolygonOffset_01
	 * @tc.desc Test polygonOffset.
	 */
	it('testPolygonOffset_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testPolygonOffset_01 test start ...66');
		gl.enable(gl.TRIANGLE_STRIP);
		gl.polygonOffset(-2, 3);
		const polygonOffsetError = gl.getError();
		console.info("polygonOffsetError: " + polygonOffsetError);
		expect(polygonOffsetError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0397
	 * @tc.name testPolygonOffset_02
	 * @tc.desc Test polygonOffset.
	 */
	it('testPolygonOffset_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testPolygonOffset_02 test start ...66');
		gl.enable(gl.TRIANGLES);
		gl.polygonOffset(-2, -3);
		const polygonOffsetError = gl.getError();
		console.info("polygonOffsetError: " + polygonOffsetError);
		expect(polygonOffsetError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0398
	 * @tc.name testPolygonOffset_03
	 * @tc.desc Test polygonOffset.
	 */
	it('testPolygonOffset_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testPolygonOffset_03 test start ...66');
		gl.enable(gl.LINES);
		gl.polygonOffset(0, 0);
		const polygonOffsetError = gl.getError();
		console.info("polygonOffsetError: " + polygonOffsetError);
		expect(polygonOffsetError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0399
	 * @tc.name testPolygonOffset_04
	 * @tc.desc Test polygonOffset.
	 */
	it('testPolygonOffset_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testPolygonOffset_04 test start ...66');
		gl.enable(gl.LINE_STRIP);
		gl.polygonOffset(0, 0);
		const polygonOffsetError = gl.getError();
		console.info("polygonOffsetError: " + polygonOffsetError);
		expect(polygonOffsetError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0400
	 * @tc.name testBufferData
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData test start ...66');
		console.info('jsWebGL testBufferData test start ...' + JSON.stringify(gl));
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ARRAY_BUFFER, 1024, gl.STATIC_DRAW);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0401
	 * @tc.name testBufferData_01
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_01 test start ...66');
		var buffer = gl.createBuffer();
		gl.bufferData(gl.ARRAY_BUFFER, 1024, gl.INVALID_ENUM);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0402
	 * @tc.name testBufferData_02
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_02 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ARRAY_BUFFER, 1024, gl.SRC_COLOR);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0403
	 * @tc.name testBufferData_03
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_03', 0, async function(done) {
		console.info('jsWebGL testBufferData_03 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ARRAY_BUFFER, 1024, gl.SRC_ALPHA);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0404
	 * @tc.name testBufferData_04
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_04 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, 1024, gl.SRC_ALPHA);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0405
	 * @tc.name testBufferData_1
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_1 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ARRAY_BUFFER, new ArrayBuffer(8), gl.STATIC_DRAW);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0406
	 * @tc.name testBufferData_1_01
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_1_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_1_01 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new ArrayBuffer(8), gl.STATIC_DRAW);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0407
	 * @tc.name testBufferData_1_02
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_1_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_1_02 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new ArrayBuffer(8), gl.ONE_MINUS_SRC_ALPHA);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0408
	 * @tc.name testBufferData_1_03
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_1_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_1_03 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new ArrayBuffer(8), gl.ONE_MINUS_DST_ALPHA);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0409
	 * @tc.name testBufferData_1_04
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_1_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_1_03 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, -1, gl.INVALID_VALUE);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(gl.INVALID_VALUE);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0410
	 * @tc.name testBufferSubData
	 * @tc.desc Test bufferSubData.
	 */
	it('testBufferSubData', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferSubData test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ARRAY_BUFFER, 1024, gl.STATIC_DRAW);
		gl.bufferSubData(gl.COPY_READ_BUFFER, 512, new ArrayBuffer(8));
		const bufferSubDataError = gl.getError();
		console.info("bufferSubDataError: " + bufferSubDataError);
		expect(bufferSubDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0411
	 * @tc.name testBufferSubData_1
	 * @tc.desc Test bufferData.
	 */
	it('testBufferSubData_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferSubData_1 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ARRAY_BUFFER, 1024, gl.TRIANGLE_FAN);
		gl.bufferSubData(gl.TRIANGLE_FAN, 512, new ArrayBuffer(8));
		const bufferSubDataError = gl.getError();
		console.info("bufferSubDataError: " + bufferSubDataError);
		expect(bufferSubDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0412
	 * @tc.name testBufferSubData_2
	 * @tc.desc Test bufferSubData.
	 */
	it('testBufferSubData_2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferSubData_2 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, 1024, gl.STATIC_DRAW);
		gl.bufferSubData(gl.COPY_READ_BUFFER, 512, new ArrayBuffer(8));
		const bufferSubDataError = gl.getError();
		console.info("bufferSubDataError: " + bufferSubDataError);
		expect(bufferSubDataError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0413
	 * @tc.name testBufferSubData_3
	 * @tc.desc Test bufferSubData.
	 */
	it('testBufferSubData_3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferSubData_3 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, 1024, gl.TRIANGLE_STRIP);
		gl.bufferSubData(gl.COPY_READ_BUFFER, 512, new ArrayBuffer(8));
		const bufferSubDataError = gl.getError();
		console.info("bufferSubDataError: " + bufferSubDataError);
		expect(bufferSubDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0414
	 * @tc.name testBufferSubData_4
	 * @tc.desc Test bufferSubData.
	 */
	it('testBufferSubData_4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferSubData_4 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, 1024, gl.TRIANGLE_STRIP);
		gl.bufferSubData(gl.COPY_READ_BUFFER, 512, new ArrayBuffer(8));
		const bufferSubDataError = gl.getError();
		console.info("bufferSubDataError: " + bufferSubDataError);
		expect(bufferSubDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0415
	 * @tc.name testCompressedTexImage2D
	 * @tc.desc Test compressedTexImage2D.
	 */
	it('testCompressedTexImage2D', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCompressedTexImage2D test start ...66');
		var texture = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, texture);
		gl.compressedTexImage2D(gl.TEXTURE_2D, 0, 0x83F3, 512, 512, 0, 0);
		const compressedTexImage2DError = gl.getError();
		console.info("compressedTexImage2DError: " + compressedTexImage2DError);
		expect(compressedTexImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0416
	 * @tc.name testCompressedTexImage2D_01
	 * @tc.desc Test compressedTexImage2D.
	 */
	it('testCompressedTexImage2D_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCompressedTexImage2D_01 test start ...66');
		var texture = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_3D, texture);
		gl.compressedTexImage2D(gl.TEXTURE_3D, [0, 0], 0x83F3, 512, 512, -1, 0);
		const compressedTexImage2DError = gl.getError();
		console.info("compressedTexImage2DError: " + compressedTexImage2DError);
		expect(compressedTexImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0417
	 * @tc.name testCompressedTexImage2D_02
	 * @tc.desc Test compressedTexImage2D.
	 */
	it('testCompressedTexImage2D_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCompressedTexImage2D_02 test start ...66');
		var texture = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_3D, texture);
		gl.compressedTexImage2D(gl.SRC_COLOR, -1, 0x83F3, -1, [-1, -1], 0, 0);
		const compressedTexImage2DError = gl.getError();
		console.info("compressedTexImage2DError: " + compressedTexImage2DError);
		expect(compressedTexImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0418
	 * @tc.name testCompressedTexImage2D_03
	 * @tc.desc Test compressedTexImage2D.
	 */
	it('testCompressedTexImage2D_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCompressedTexImage2D_03 test start ...66');
		var program = gl.createProgram();
		gl.bindTexture(gl.TEXTURE_3D, program);
		gl.compressedTexImage2D(gl.TEXTURE_3D, 0, [0x83F3, 0x83F3], 512, 512, 0, 0);
		const compressedTexImage2DError = gl.getError();
		console.info("compressedTexImage2DError: " + compressedTexImage2DError);
		expect(compressedTexImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0419
	 * @tc.name testCompressedTexImage2D_04
	 * @tc.desc Test compressedTexImage2D.
	 */
	it('testCompressedTexImage2D_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCompressedTexImage2D_04 test start ...66');
		var framebuffer = gl.createFramebuffer();
		gl.bindTexture(gl.TEXTURE_3D, framebuffer);
		gl.compressedTexImage2D(gl.TEXTURE_2D, 0, 0x0307, 512, 512, 0, "abc");
		const compressedTexImage2DError = gl.getError();
		console.info("compressedTexImage2DError: " + compressedTexImage2DError);
		expect(compressedTexImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0420
	 * @tc.name testCompressedTexSubImage2D_1
	 * @tc.desc Test compressedTexSubImage2D.
	 */
	it('testCompressedTexSubImage2D_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCompressedTexSubImage2D_1 test start ...66');
		var texture = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, texture);
		gl.compressedTexImage2D(gl.TEXTURE_2D, 0, 0x83F3, 512, 512, 0, 0);
		gl.compressedTexSubImage2D(gl.TEXTURE_2D, 0, 256, 256, 512, 512, 0x83F3, gl
			.PIXEL_UNPACK_BUFFER, 0);
		const compressedTexSubImage2DError = gl.getError();
		console.info("compressedTexSubImage2DError: " + compressedTexSubImage2DError);
		expect(compressedTexSubImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0421
	 * @tc.name testReadPixels
	 * @tc.desc Test readPixels.
	 */
	it('testReadPixels', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testReadPixels test start ...66');
		var pixels = new Uint8Array(gl.drawingBufferWidth * gl.drawingBufferHeight * 4);
		gl.readPixels(0, 0, gl.drawingBufferWidth, gl.drawingBufferHeight, gl.RGBA, gl
			.UNSIGNED_BYTE, pixels);
		const readPixelsError = gl.getError();
		console.info("readPixelsError: " + readPixelsError);
		expect(readPixelsError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0422
	 * @tc.name testReadPixels_01
	 * @tc.desc Test readPixels.
	 */
	it('testReadPixels_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testReadPixels_01 test start ...66');
		var pixels = new Uint8Array(gl.drawingBufferWidth * gl.drawingBufferHeight * 4);
		gl.readPixels(0, 0, gl.drawingBufferWidth, gl.drawingBufferHeight, gl.RGBA, gl.DST_COLOR,
			pixels);
		const readPixelsError = gl.getError();
		console.info("readPixelsError: " + readPixelsError);
		expect(readPixelsError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0423
	 * @tc.name testReadPixels_02
	 * @tc.desc Test readPixels.
	 */
	it('testReadPixels_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testReadPixels_02 test start ...66');
		var pixels = new Uint8Array(gl.drawingBufferWidth * gl.drawingBufferHeight * 4);
		gl.readPixels(0, 0, gl.drawingBufferWidth, gl.drawingBufferHeight, gl.RGBA, gl.DST_COLOR,
			0);
		const readPixelsError = gl.getError();
		console.info("readPixelsError: " + readPixelsError);
		expect(readPixelsError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0424
	 * @tc.name testReadPixels_03
	 * @tc.desc Test readPixels.
	 */
	it('testReadPixels_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testReadPixels_03 test start ...66');
		var pixels = new Uint8Array(gl.drawingBufferWidth * gl.drawingBufferHeight * 4);
		gl.readPixels(0, 0, gl.drawingBufferWidth, gl.drawingBufferHeight, -1, gl.DST_COLOR, -2);
		const readPixelsError = gl.getError();
		console.info("readPixelsError: " + readPixelsError);
		expect(readPixelsError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0425
	 * @tc.name testReadPixels_04
	 * @tc.desc Test readPixels.
	 */
	it('testReadPixels_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testReadPixels_04 test start ...66');
		var pixels = new Uint8Array(gl.drawingBufferWidth * gl.drawingBufferHeight * 4);
		gl.readPixels(-1, -1, gl.drawingBufferWidth, gl.drawingBufferHeight, -1, gl.CONSTANT_COLOR,
			0);
		const readPixelsError = gl.getError();
		console.info("readPixelsError: " + readPixelsError);
		expect(readPixelsError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0426
	 * @tc.name testTexImage2D
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGB8, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0427
	 * @tc.name testTexImage2D_01
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D_01 test start ...66');
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGB8, 512, 512, [0, 0], gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0428
	 * @tc.name testTexImage2D_02
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D_02 test start ...66');
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGB8, 512, 512, 0, gl.RGB8, gl.CONSTANT_ALPHA, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0429
	 * @tc.name testTexImage2D_03
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D_03 test start ...66');
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGB8, 512, 512, 0, gl.RGB8, gl.FUNC_SUBTRACT, -1);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0430
	 * @tc.name testTexImage2D_04
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D_04 test start ...66');
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl.DST_COLOR, 512, 512, 0, gl.RGB8, gl.CONSTANT_ALPHA, -1);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0431
	 * @tc.name testTexSubImage2D
	 * @tc.desc Test texSubImage2D.
	 */
	it('testTexSubImage2D', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexSubImage2D test start ...66');
		console.info('jsWebGL testTexSubImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(16);
		const z = new Float32Array(buffer, 0, 4);
		gl.texSubImage2D(gl.TEXTURE_2D, 0, 0, 0, 512, 512, gl.RED, gl.FLOAT, z, 0);
		const ttexSubImage2DError = gl.getError();
		console.info("ttexSubImage2DError: " + ttexSubImage2DError);
		expect(ttexSubImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0432
	 * @tc.name testUniform1fv
	 * @tc.desc Test uniform1fv.
	 */
	it('testUniform1fv', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform1fv test start ...66');
		const programObj = createProgram(gl);
		const uniformlocationObj = gl.getUniformLocation(programObj, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniform1fv(uniformlocationObj, flaot32list);
		const uniform1fvError = gl.getError();
		console.info("uniform1fvError: " + uniform1fvError);
		const errorCode = gl.getError();
		expect(uniform1fvError).assertEqual(0);

		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0433
	 * @tc.name testUniform1fv_1
	 * @tc.desc Test uniform1fv.
	 */
	it('testUniform1fv_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform1fv_1 test start ...66');
		const shader = gl.createShader(gl.VERTEX_SHADER);
		const uniformlocationObj = gl.getUniformLocation(shader, "a_Position1");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniform1fv(uniformlocationObj, flaot32list);
		const uniform1fvError = gl.getError();
		console.info("uniform1fvError: " + uniform1fvError);
		expect(uniform1fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0434
	 * @tc.name testUniform1fv_2
	 * @tc.desc Test uniform1fv.
	 */
	it('testUniform1fv_2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform1fv_2 test start ...66');
		const framebuffer = gl.createFramebuffer();
		const uniformlocationObj = gl.getUniformLocation(framebuffer, "a_Position1");
		const flaot32list = new Float32Array([-1, -2]);
		gl.uniform1fv(uniformlocationObj, flaot32list);
		const uniform1fvError = gl.getError();
		console.info("uniform1fvError: " + uniform1fvError);
		const errorCode = gl.getError();
		expect(uniform1fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0435
	 * @tc.name testUniform1fv_3
	 * @tc.desc Test uniform1fv.
	 */
	it('testUniform1fv_3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform1fv_3 test start ...66');
		const programobject = gl.createProgram();
		const uniformlocationObj = gl.getUniformLocation(programobject, "a_Position1");
		const flaot32list = new Float32Array([-1, -2]);
		gl.uniform1fv(uniformlocationObj, flaot32list);
		const uniform1fvError = gl.getError();
		console.info("uniform1fvError: " + uniform1fvError);
		const errorCode = gl.getError();
		expect(uniform1fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0436
	 * @tc.name testUniform1fv_4
	 * @tc.desc Test uniform1fv.
	 */
	it('testUniform1fv_4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform1fv_4 test start ...66');
		const renderbuffer = gl.createRenderbuffer();
		const uniformlocationObj = gl.getUniformLocation(renderbuffer, "a_Position1");
		const flaot32list = new Float32Array([-1, -2]);
		gl.uniform1fv(uniformlocationObj, flaot32list);
		const uniform1fvError = gl.getError();
		console.info("uniform1fvError: " + uniform1fvError);
		const errorCode = gl.getError();
		expect(uniform1fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0437
	 * @tc.name testUniform2fv
	 * @tc.desc Test uniform2fv.
	 */
	it('testUniform2fv', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform2fv test start ...66');
		const programObj = createProgram(gl);
		const uniformlocationObj = gl.getUniformLocation(programObj, "123");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniform2fv(uniformlocationObj, flaot32list);
		const uniform2fvError = gl.getError();
		console.info("uniform2fvError: " + uniform2fvError);
		expect(uniform2fvError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0438
	 * @tc.name testUniform2fv_1
	 * @tc.desc Test uniform2fv.
	 */
	it('testUniform2fv_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform2fv_1 test start ...66');
		const renderbuffer = gl.createRenderbuffer();
		const uniformlocationObj = gl.getUniformLocation(renderbuffer, "123");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniform2fv(uniformlocationObj, flaot32list);
		const uniform2fvError = gl.getError();
		console.info("uniform2fvError: " + uniform2fvError);
		expect(uniform2fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0439
	 * @tc.name testUniform2fv_2
	 * @tc.desc Test uniform2fv.
	 */
	it('testUniform2fv_2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform2fv_2 test start ...66');
		const programObj = gl.createProgram();
		const uniformlocationObj = gl.getUniformLocation(programObj, "123");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniform2fv(uniformlocationObj, flaot32list);
		const uniform2fvError = gl.getError();
		console.info("uniform2fvError: " + uniform2fvError);
		expect(uniform2fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0440
	 * @tc.name testUniform2fv_3
	 * @tc.desc Test uniform2fv.
	 */
	it('testUniform2fv_3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform2fv_3 test start ...66');
		const framebuffer = gl.createFramebuffer();
		const uniformlocationObj = gl.getUniformLocation(framebuffer, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniform2fv(uniformlocationObj, flaot32list);
		const uniform2fvError = gl.getError();
		console.info("uniform2fvError: " + uniform2fvError);
		expect(uniform2fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0441
	 * @tc.name testUniform2fv_4
	 * @tc.desc Test uniform2fv.
	 */
	it('testUniform2fv_4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform2fv_4 test start ...66');
		const shader = gl.createShader(gl.VERTEX_SHADER);
		const uniformlocationObj = gl.getUniformLocation(shader, "a_Position1");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniform2fv(uniformlocationObj, flaot32list);
		const uniform2fvError = gl.getError();
		console.info("uniform2fvError1: " + uniform2fvError);
		expect(uniform2fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0442
	 * @tc.name testUniform3fv
	 * @tc.desc Test uniform3fv.
	 */
	it('testUniform3fv', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform3fv test start ...66');
		const programObj = createProgram(gl);
		const uniformlocationObj = gl.getUniformLocation(programObj, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniform3fv(uniformlocationObj, flaot32list);
		const uniform3fvError = gl.getError();
		console.info("uniform3fvError: " + uniform3fvError);
		expect(uniform3fvError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0443
	 * @tc.name testUniform3fv_1
	 * @tc.desc Test uniform3fv.
	 */
	it('testUniform3fv_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform3fv_1 test start ...66');
		const programObj = gl.createProgram();
		const uniformlocationObj = gl.getUniformLocation(programObj, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniform3fv(uniformlocationObj, flaot32list);
		const uniform3fvError = gl.getError();
		console.info("uniform3fvError: " + uniform3fvError);
		expect(uniform3fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0444
	 * @tc.name testUniform3fv_2
	 * @tc.desc Test uniform3fv.
	 */
	it('testUniform3fv_2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform3fv_2 test start ...66');
		const framebuffer = gl.createFramebuffer();
		const uniformlocationObj = gl.getUniformLocation(framebuffer, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniform3fv(uniformlocationObj, flaot32list);
		const uniform3fvError = gl.getError();
		console.info("uniform3fvError: " + uniform3fvError);
		expect(uniform3fvError).assertEqual(gl.NO_ERROR);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0445
	 * @tc.name testUniform3fv_3
	 * @tc.desc Test uniform3fv.
	 */
	it('testUniform3fv_3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform3fv_3 test start ...66');
		const renderbuffer = gl.createRenderbuffer();
		const uniformlocationObj = gl.getUniformLocation(renderbuffer, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniform3fv(uniformlocationObj, flaot32list);
		const uniform3fvError = gl.getError();
		console.info("uniform3fvError: " + uniform3fvError);
		expect(uniform3fvError).assertEqual(gl.NO_ERROR);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0446
	 * @tc.name testUniform3fv_4
	 * @tc.desc Test uniform3fv.
	 */
	it('testUniform3fv_4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform3fv_4 test start ...66');
		const shader = gl.createShader(gl.VERTEX_SHADER);
		const uniformlocationObj = gl.getUniformLocation(shader, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniform3fv(uniformlocationObj, flaot32list);
		const uniform3fvError = gl.getError();
		console.info("uniform3fvError: " + uniform3fvError);
		expect(uniform3fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0447
	 * @tc.name testUniform4fv
	 * @tc.desc Test uniform4fv.
	 */
	it('testUniform4fv', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform4fv test start ...66');
		const programObj = createProgram(gl);
		const uniformlocationObj = gl.getUniformLocation(programObj, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniform4fv(uniformlocationObj, flaot32list);
		const uniform4fvError = gl.getError();
		console.info("uniform4fvError: " + uniform4fvError);
		expect(uniform4fvError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0448
	 * @tc.name testUniform4fv_01
	 * @tc.desc Test uniform4fv.
	 */
	it('testUniform4fv_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform4fv_01 test start ...66');
		const programObj = gl.createProgram();
		const uniformlocationObj = gl.getUniformLocation(programObj, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniform4fv(uniformlocationObj, flaot32list);
		const uniform4fvError = gl.getError();
		console.info("uniform4fvError: " + uniform4fvError);
		expect(uniform4fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0449
	 * @tc.name testUniform4fv_02
	 * @tc.desc Test uniform4fv.
	 */
	it('testUniform4fv_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform4fv_02 test start ...66');
		const renderbuffer = gl.createRenderbuffer();
		const uniformlocationObj = gl.getUniformLocation(renderbuffer, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniform4fv(uniformlocationObj, flaot32list);
		const uniform4fvError = gl.getError();
		console.info("uniform4fvError: " + uniform4fvError);
		expect(uniform4fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0450
	 * @tc.name testUniform4fv_03
	 * @tc.desc Test uniform4fv.
	 */
	it('testUniform4fv_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform4fv_03 test start ...66');
		const framebuffer = gl.createFramebuffer();
		const uniformlocationObj = gl.getUniformLocation(framebuffer, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniform4fv(uniformlocationObj, flaot32list);
		const uniform4fvError = gl.getError();
		console.info("uniform4fvError: " + uniform4fvError);
		expect(uniform4fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0451
	 * @tc.name testUniform4fv_04
	 * @tc.desc Test uniform4fv.
	 */
	it('testUniform4fv_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform4fv_04 test start ...66');
		const shader = gl.createShader(gl.VERTEX_SHADER);
		const uniformlocationObj = gl.getUniformLocation(shader, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniform4fv(uniformlocationObj, flaot32list);
		const uniform4fvError = gl.getError();
		console.info("uniform4fvError: " + uniform4fvError);
		expect(uniform4fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0452
	 * @tc.name testUniform1iv
	 * @tc.desc Test uniform1iv.
	 */
	it('testUniform1iv', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform1iv test start ...66');
		const programObj = createProgram(gl);
		const uniformlocationObj = gl.getUniformLocation(programObj, "a_Position");
		const int32list = new Int32Array([1, 2]);
		gl.uniform1iv(uniformlocationObj, int32list);
		const uniform1ivError = gl.getError();
		console.info("uniform1ivError: " + uniform1ivError);
		expect(uniform1ivError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0453
	 * @tc.name testUniform1iv_01
	 * @tc.desc Test uniform1iv.
	 */
	it('testUniform1iv_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform1iv_01 test start ...66');
		const programObj = gl.createProgram();
		const uniformlocationObj = gl.getUniformLocation(programObj, "a_Position");
		const int32list = new Int32Array([1, 2]);
		gl.uniform1iv(uniformlocationObj, int32list);
		const uniform1ivError = gl.getError();
		console.info("uniform1ivError: " + uniform1ivError);
		expect(uniform1ivError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0454
	 * @tc.name testUniform1iv_02
	 * @tc.desc Test uniform1iv.
	 */
	it('testUniform1iv_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform1iv_02 test start ...66');
		const renderbuffer = gl.createRenderbuffer();
		const uniformlocationObj = gl.getUniformLocation(renderbuffer, "a_Position");
		const int32list = new Int32Array([1, 2]);
		gl.uniform1iv(uniformlocationObj, int32list);
		const uniform1ivError = gl.getError();
		console.info("uniform1ivError: " + uniform1ivError);
		expect(uniform1ivError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0455
	 * @tc.name testUniform1iv_03
	 * @tc.desc Test uniform1iv.
	 */
	it('testUniform1iv_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform1iv_03 test start ...66');
		const framebuffer = gl.createFramebuffer();
		const uniformlocationObj = gl.getUniformLocation(framebuffer, "a_Position");
		const int32list = new Int32Array([1, 2]);
		gl.uniform1iv(uniformlocationObj, int32list);
		const uniform1ivError = gl.getError();
		console.info("uniform1ivError: " + uniform1ivError);
		expect(uniform1ivError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0456
	 * @tc.name testUniform1iv_04
	 * @tc.desc Test uniform1iv.
	 */
	it('testUniform1iv_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform1iv_04 test start ...66');
		const shader = gl.createShader(gl.VERTEX_SHADER);
		const uniformlocationObj = gl.getUniformLocation(shader, "a_Position");
		const int32list = new Int32Array([1, 2]);
		gl.uniform1iv(uniformlocationObj, int32list);
		const uniform1ivError = gl.getError();
		console.info("uniform1ivError: " + uniform1ivError);
		expect(uniform1ivError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0457
	 * @tc.name testUniform2iv
	 * @tc.desc Test uniform2iv.
	 */
	it('testUniform2iv', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform2iv test start ...66');
		const programObj = createProgram(gl);
		const uniformlocationObj = gl.getUniformLocation(programObj, "a_Position");
		const int32list = new Int32Array([1, 2]);
		gl.uniform2iv(uniformlocationObj, int32list);
		const uniform2ivError = gl.getError();
		console.info("testUniform2ivError: " + uniform2ivError);
		expect(uniform2ivError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0458
	 * @tc.name testUniform2iv_01
	 * @tc.desc Test uniform2iv.
	 */
	it('testUniform2iv_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform2iv_01 test start ...66');
		const programObj = gl.createProgram();
		const uniformlocationObj = gl.getUniformLocation(programObj, "a_Position");
		const int32list = new Int32Array([1, 2]);
		gl.uniform2iv(uniformlocationObj, int32list);
		const uniform2ivError = gl.getError();
		console.info("testUniform2ivError: " + uniform2ivError);
		expect(uniform2ivError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0459
	 * @tc.name testUniform2iv_02
	 * @tc.desc Test uniform2iv.
	 */
	it('testUniform2iv_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform2iv_02 test start ...66');
		const renderbuffer = gl.createRenderbuffer();
		const uniformlocationObj = gl.getUniformLocation(renderbuffer, "a_Position");
		const int32list = new Int32Array([1, 2]);
		gl.uniform2iv(uniformlocationObj, int32list);
		const uniform2ivError = gl.getError();
		console.info("testUniform2ivError: " + uniform2ivError);
		expect(uniform2ivError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0460
	 * @tc.name testUniform2iv_03
	 * @tc.desc Test uniform2iv.
	 */
	it('testUniform2iv_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform2iv_03 test start ...66');
		const framebuffer = gl.createFramebuffer();
		const uniformlocationObj = gl.getUniformLocation(framebuffer, "a_Position");
		const int32list = new Int32Array([1, 2]);
		gl.uniform2iv(uniformlocationObj, int32list);
		const uniform2ivError = gl.getError();
		console.info("testUniform2ivError: " + uniform2ivError);
		expect(uniform2ivError).assertEqual(gl.NO_ERROR);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0461
	 * @tc.name testUniform2iv_04
	 * @tc.desc Test uniform2iv.
	 */
	it('testUniform2iv_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform2iv_04 test start ...66');
		const shader = gl.createShader(gl.VERTEX_SHADER);
		const uniformlocationObj = gl.getUniformLocation(shader, "a_Position");
		const int32list = new Int32Array([1, 2]);
		gl.uniform2iv(uniformlocationObj, int32list);
		const uniform2ivError = gl.getError();
		console.info("testUniform2ivError: " + uniform2ivError);
		expect(uniform2ivError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0462
	 * @tc.name testUniform3iv
	 * @tc.desc Test uniform3iv.
	 */
	it('testUniform3iv', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform3iv test start ...66');
		const programObj = createProgram(gl);
		const uniformlocationObj = gl.getUniformLocation(programObj, "a_Position");
		const int32list = new Int32Array([1, 2]);
		gl.uniform3iv(uniformlocationObj, int32list);
		const uniform3ivError = gl.getError();
		console.info("testUniform3ivError: " + uniform3ivError);
		expect(uniform3ivError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0463
	 * @tc.name testUniform3iv_01
	 * @tc.desc Test uniform3iv.
	 */
	it('testUniform3iv_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform3iv_01 test start ...66');
		const programObj = gl.createProgram();
		const uniformlocationObj = gl.getUniformLocation(programObj, "a_Position");
		const int32list = new Int32Array([1, 2]);
		gl.uniform3iv(uniformlocationObj, int32list);
		const uniform3ivError = gl.getError();
		console.info("testUniform3ivError: " + uniform3ivError);
		expect(uniform3ivError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0464
	 * @tc.name testUniform3iv_02
	 * @tc.desc Test uniform3iv.
	 */
	it('testUniform3iv_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform3iv_02 test start ...66');
		const renderbuffer = gl.createRenderbuffer();
		const uniformlocationObj = gl.getUniformLocation(renderbuffer, "a_Position");
		const int32list = new Int32Array([1, 2]);
		gl.uniform3iv(uniformlocationObj, int32list);
		const uniform3ivError = gl.getError();
		console.info("testUniform3ivError: " + uniform3ivError);
		expect(uniform3ivError).assertEqual(gl.NO_ERROR);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0465
	 * @tc.name testUniform3iv_03
	 * @tc.desc Test uniform3iv.
	 */
	it('testUniform3iv_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform3iv_03 test start ...66');
		var texture = gl.createTexture();
		const uniformlocationObj = gl.getUniformLocation(texture, "a_Position");
		const int32list = new Int32Array([1, 2]);
		gl.uniform3iv(uniformlocationObj, int32list);
		const uniform3ivError = gl.getError();
		console.info("testUniform3ivError: " + uniform3ivError);
		expect(uniform3ivError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0466
	 * @tc.name testUniform3iv_04
	 * @tc.desc Test uniform3iv.
	 */
	it('testUniform3iv_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform3iv_04 test start ...66');
		const shader = gl.createShader(gl.VERTEX_SHADER);
		const uniformlocationObj = gl.getUniformLocation(shader, "a_Position");
		const int32list = new Int32Array([1, 2]);
		gl.uniform3iv(uniformlocationObj, int32list);
		const uniform3ivError = gl.getError();
		console.info("testUniform3ivError: " + uniform3ivError);
		expect(uniform3ivError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0467
	 * @tc.name testUniform4iv
	 * @tc.desc Test uniform4iv.
	 */
	it('testUniform4iv', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform4iv test start ...66');
		const programObj = createProgram(gl);
		const uniformlocationObj = gl.getUniformLocation(programObj, "a_Position");
		const int32list = new Int32Array([1, 2]);
		gl.uniform4iv(uniformlocationObj, int32list);
		const uniform4ivError = gl.getError();
		console.info("testUniform4ivError: " + uniform4ivError);
		expect(uniform4ivError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0468
	 * @tc.name testUniform4iv_01
	 * @tc.desc Test uniform4iv.
	 */
	it('testUniform4iv_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform4iv_01 test start ...66');
		const programObj = gl.createProgram();
		const uniformlocationObj = gl.getUniformLocation(programObj, "a_Position");
		const int32list = new Int32Array([1, 2]);
		gl.uniform4iv(uniformlocationObj, int32list);
		const uniform4ivError = gl.getError();
		console.info("testUniform4ivError: " + uniform4ivError);
		expect(uniform4ivError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0469
	 * @tc.name testUniform4iv_02
	 * @tc.desc Test uniform4iv.
	 */
	it('testUniform4iv_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform4iv_02 test start ...66');
		const buffer = gl.createBuffer();
		const uniformlocationObj = gl.getUniformLocation(buffer, "a_Position");
		const int32list = new Int32Array([1, 2]);
		gl.uniform4iv(uniformlocationObj, int32list);
		const uniform4ivError = gl.getError();
		console.info("testUniform4ivError: " + uniform4ivError);
		expect(uniform4ivError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0470
	 * @tc.name testUniform4iv_03
	 * @tc.desc Test uniform4iv.
	 */
	it('testUniform4iv_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform4iv_03 test start ...66');
		const buffer = gl.createBuffer();
		const uniformlocationObj = gl.getUniformLocation(buffer, "a_Position");
		const int32list = new Int32Array([1, 2]);
		gl.uniform4iv(uniformlocationObj, int32list);
		const uniform4ivError = gl.getError();
		console.info("testUniform4ivError: " + uniform4ivError);
		expect(uniform4ivError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0471
	 * @tc.name testUniform4iv_04
	 * @tc.desc Test Uniform4uiv.
	 */
	it('testUniform4iv_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform4iv_03 test start ...66');
		const shader = gl.createShader(gl.VERTEX_SHADER);
		const uniformlocationObj = gl.getUniformLocation(shader, "a_Position");
		const int32list = new Int32Array([1, 2]);
		gl.uniform4iv(uniformlocationObj, int32list);
		const uniform4ivError = gl.getError();
		console.info("testUniform4ivError: " + uniform4ivError);
		expect(uniform4ivError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0472
	 * @tc.name testUniformMatrix2fv
	 * @tc.desc Test uniformMatrix2fv.
	 */
	it('testUniformMatrix2fv', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniformMatrix2fv test start ...66');
		const programObj = createProgram(gl);
		const uniformlocationObj = gl.getUniformLocation(programObj, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniformMatrix2fv(uniformlocationObj, true, flaot32list);
		const uniformMatrix2fvError = gl.getError();
		console.info("uniformMatrix2fvError: " + uniformMatrix2fvError);
		expect(uniformMatrix2fvError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0473
	 * @tc.name testUniformMatrix2fv_01
	 * @tc.desc Test uniformMatrix2fv.
	 */
	it('testUniformMatrix2fv_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniformMatrix2fv_01 test start ...66');
		const programObj = gl.createProgram();
		const uniformlocationObj = gl.getUniformLocation(programObj, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniformMatrix2fv(uniformlocationObj, true, flaot32list);
		const uniformMatrix2fvError = gl.getError();
		console.info("uniformMatrix2fvError: " + uniformMatrix2fvError);
		expect(uniformMatrix2fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0474
	 * @tc.name testUniformMatrix2fv_02
	 * @tc.desc Test uniformMatrix2fv.
	 */
	it('testUniformMatrix2fv_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniformMatrix2fv_02 test start ...66');
		const buffer = gl.createBuffer();
		const uniformlocationObj = gl.getUniformLocation(buffer, "a_Position1");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniformMatrix2fv(uniformlocationObj, true, flaot32list);
		const uniformMatrix2fvError = gl.getError();
		console.info("uniformMatrix2fvError: " + uniformMatrix2fvError);
		expect(uniformMatrix2fvError).assertEqual(gl.NO_ERROR);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0475
	 * @tc.name testUniformMatrix2fv_03
	 * @tc.desc Test uniformMatrix2fv.
	 */
	it('testUniformMatrix2fv_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniformMatrix2fv_03 test start ...66');
		const buffer = gl.createBuffer();
		const uniformlocationObj = gl.getUniformLocation(buffer, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniformMatrix2fv(uniformlocationObj, true, flaot32list);
		const uniformMatrix2fvError = gl.getError();
		console.info("uniformMatrix2fvError: " + uniformMatrix2fvError);
		expect(uniformMatrix2fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0476
	 * @tc.name testUniformMatrix2fv_04
	 * @tc.desc Test uniformMatrix2fv.
	 */
	it('testUniformMatrix2fv_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniformMatrix2fv_04 test start ...66');
		const shader = gl.createShader(gl.VERTEX_SHADER);
		const uniformlocationObj = gl.getUniformLocation(shader, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniformMatrix2fv(uniformlocationObj, true, flaot32list);
		const uniformMatrix2fvError = gl.getError();
		console.info("uniformMatrix2fvError: " + uniformMatrix2fvError);
		expect(uniformMatrix2fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0477
	 * @tc.name testUniformMatrix3fv
	 * @tc.desc Test uniformMatrix3fv.
	 */
	it('testUniformMatrix3fv', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniformMatrix3fv test start ...66');
		const programObj = createProgram(gl);
		const uniformlocationObj = gl.getUniformLocation(programObj, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniformMatrix3fv(uniformlocationObj, true, flaot32list);
		const uniformMatrix3fvError = gl.getError();
		console.info("uniformMatrix3fvError: " + uniformMatrix3fvError);
		expect(uniformMatrix3fvError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0478
	 * @tc.name testUniformMatrix3fv_01
	 * @tc.desc Test uniformMatrix3fv.
	 */
	it('testUniformMatrix3fv_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniformMatrix3fv_01 test start ...66');
		const programObj = gl.createProgram();
		const uniformlocationObj = gl.getUniformLocation(programObj, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniformMatrix3fv(uniformlocationObj, true, flaot32list);
		const uniformMatrix3fvError = gl.getError();
		console.info("uniformMatrix3fvError: " + uniformMatrix3fvError);
		expect(uniformMatrix3fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0479
	 * @tc.name testUniformMatrix3fv_02
	 * @tc.desc Test uniformMatrix3fv.
	 */
	it('testUniformMatrix3fv_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniformMatrix3fv_02 test start ...66');
		const fragmentShader = gl.createShader(gl.FRAGMENT_SHADER);
		const uniformlocationObj = gl.getUniformLocation(fragmentShader, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniformMatrix3fv(uniformlocationObj, true, flaot32list);
		const uniformMatrix3fvError = gl.getError();
		console.info("uniformMatrix3fvError: " + uniformMatrix3fvError);
		expect(uniformMatrix3fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0480
	 * @tc.name testUniformMatrix3fv_03
	 * @tc.desc Test uniformMatrix3fv.
	 */
	it('testUniformMatrix3fv_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniformMatrix3fv_03 test start ...66');
		const fragmentShader = gl.createShader(gl.FRAGMENT_SHADER);
		const uniformlocationObj = gl.getUniformLocation(fragmentShader, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniformMatrix3fv(uniformlocationObj, true, flaot32list);
		const uniformMatrix3fvError = gl.getError();
		console.info("uniformMatrix3fvError: " + uniformMatrix3fvError);
		expect(uniformMatrix3fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0481
	 * @tc.name testUniformMatrix3fv_04
	 * @tc.desc Test uniformMatrix3fv.
	 */
	it('testUniformMatrix3fv_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniformMatrix3fv_04 test start ...66');
		var shader = gl.createShader(gl.VERTEX_SHADER);
		const uniformlocationObj = gl.getUniformLocation(shader, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniformMatrix3fv(uniformlocationObj, true, flaot32list);
		const uniformMatrix3fvError = gl.getError();
		console.info("uniformMatrix3fvError: " + uniformMatrix3fvError);
		expect(uniformMatrix3fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0482
	 * @tc.name testUniformMatrix4fv
	 * @tc.desc Test uniformMatrix4fv.
	 */
	it('testUniformMatrix4fv', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniformMatrix4fv test start ...66');
		const programObj = createProgram(gl);
		const uniformlocationObj = gl.getUniformLocation(programObj, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		const a = gl.uniformMatrix4fv(uniformlocationObj, true, flaot32list);
		console.info("a: " + a);
		const uniformMatrix4fvError = gl.getError();
		console.info("uniformMatrix4fvError: " + uniformMatrix4fvError);
		expect(uniformMatrix4fvError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0483
	 * @tc.name testUniformMatrix4fv_01
	 * @tc.desc Test uniformMatrix4fv.
	 */
	it('testUniformMatrix4fv_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniformMatrix4fv_01 test start ...66');
		const programObj = gl.createProgram();
		const uniformlocationObj = gl.getUniformLocation(programObj, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		const a = gl.uniformMatrix4fv(uniformlocationObj, true, flaot32list);
		console.info("a: " + a);
		const uniformMatrix4fvError = gl.getError();
		console.info("uniformMatrix4fvError: " + uniformMatrix4fvError);
		expect(uniformMatrix4fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0484
	 * @tc.name testUniformMatrix4fv_02
	 * @tc.desc Test uniformMatrix4fv.
	 */
	it('testUniformMatrix4fv_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniformMatrix4fv_02 test start ...66');
		const fragmentShader = gl.createShader(gl.FRAGMENT_SHADER);
		const uniformlocationObj = gl.getUniformLocation(fragmentShader, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		const a = gl.uniformMatrix4fv(uniformlocationObj, true, flaot32list);
		console.info("a: " + a);
		const uniformMatrix4fvError = gl.getError();
		console.info("uniformMatrix4fvError: " + uniformMatrix4fvError);
		expect(uniformMatrix4fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0485
	 * @tc.name testUniformMatrix4fv_03
	 * @tc.desc Test uniformMatrix4fv.
	 */
	it('testUniformMatrix4fv_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniformMatrix4fv_03 test start ...66');
		var fragmentShader = gl.createShader(gl.FRAGMENT_SHADER);
		const uniformlocationObj = gl.getUniformLocation(fragmentShader, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		const a = gl.uniformMatrix4fv(uniformlocationObj, true, flaot32list);
		console.info("a: " + a);
		const uniformMatrix4fvError = gl.getError();
		console.info("uniformMatrix4fvError: " + uniformMatrix4fvError);
		expect(uniformMatrix4fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0486
	 * @tc.name testUniformMatrix4fv_04
	 * @tc.desc Test uniformMatrix4fv.
	 */
	it('testUniformMatrix4fv_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniformMatrix4fv_04 test start ...66');
		const shader = gl.createShader(gl.VERTEX_SHADER);
		const uniformlocationObj = gl.getUniformLocation(shader, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		const a = gl.uniformMatrix4fv(uniformlocationObj, true, flaot32list);
		console.info("a: " + a);
		const uniformMatrix4fvError = gl.getError();
		console.info("uniformMatrix4fvError: " + uniformMatrix4fvError);
		expect(uniformMatrix4fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0487
	 * @tc.name testCopyBufferSubData
	 * @tc.desc Test copyBufferSubData.
	 */
	it('testCopyBufferSubData', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCopyBufferSubData test start ...66');
		console.info('jsWebGL testCopyBufferSubData test start ...' + JSON.stringify(gl));
		const srcBuffer = gl.createBuffer();
		const dstBuffer = gl.createBuffer();
		const vertices = [1, 2];
		const data = new Float32Array(vertices);
		const length = vertices.length * 4;
		gl.bindBuffer(gl.ARRAY_BUFFER, srcBuffer);
		gl.bufferData(gl.ARRAY_BUFFER, data, gl.STATIC_DRAW);
		gl.bindBuffer(gl.COPY_READ_BUFFER, srcBuffer);
		gl.bindBuffer(gl.ARRAY_BUFFER, dstBuffer);
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(length), gl.STATIC_DRAW);
		gl2.copyBufferSubData(gl.COPY_READ_BUFFER, gl.ARRAY_BUFFER, 0, 0, length);
		const copyBufferSubDataError = gl.getError();
		console.info("copyBufferSubDataError: " + copyBufferSubDataError);
		expect(copyBufferSubDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0488
	 * @tc.name testCopyBufferSubData_01
	 * @tc.desc Test copyBufferSubData.
	 */
	it('testCopyBufferSubData_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCopyBufferSubData_01 test start ...66');
		const srcBuffer = gl.createBuffer();
		const dstBuffer = gl.createBuffer();
		const vertices = [1, 2];
		const data = new Float32Array(vertices);
		const length = vertices.length * 4;
		gl.bindBuffer(gl.ARRAY_BUFFER, srcBuffer);
		gl.bufferData(gl.ARRAY_BUFFER, data, gl.STATIC_DRAW);

		gl.bindBuffer(gl.COPY_READ_BUFFER, srcBuffer);
		gl.bindBuffer(gl.ARRAY_BUFFER, dstBuffer);
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(length), gl.STATIC_DRAW);
		gl2.copyBufferSubData(gl.ONE_MINUS_CONSTANT_ALPHA, gl.ONE_MINUS_CONSTANT_ALPHA, 0, 0, 0);
		const copyBufferSubDataError = gl.getError();
		console.info("copyBufferSubDataError: " + copyBufferSubDataError);
		expect(copyBufferSubDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0489
	 * @tc.name testCopyBufferSubData_02
	 * @tc.desc Test copyBufferSubData.
	 */
	it('testCopyBufferSubData_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCopyBufferSubData_02 test start ...66');
		const srcBuffer = gl.createBuffer();
		const dstBuffer = gl.createBuffer();
		const vertices = [1, 2];
		const data = new Float32Array(vertices);
		const length = vertices.length * 4;
		gl.bindBuffer(gl.ARRAY_BUFFER, srcBuffer);
		gl.bufferData(gl.ARRAY_BUFFER, data, gl.STATIC_DRAW);

		gl.bindBuffer(gl.COPY_READ_BUFFER, srcBuffer);
		gl.bindBuffer(gl.ARRAY_BUFFER, dstBuffer);
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(length), gl.STATIC_DRAW);
		gl2.copyBufferSubData(gl.SRC_ALPHA_SATURATE, gl.SRC_ALPHA_SATURATE, 0, 0, 0);
		const copyBufferSubDataError = gl.getError();
		console.info("copyBufferSubDataError: " + copyBufferSubDataError);
		expect(copyBufferSubDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0490
	 * @tc.name testCopyBufferSubData_03
	 * @tc.desc Test copyBufferSubData.
	 */
	it('testCopyBufferSubData_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCopyBufferSubData_03 test start ...66');
		const srcBuffer = gl.createBuffer();
		const dstBuffer = gl.createBuffer();
		const vertices = [1, 2];
		const data = new Float32Array(vertices);
		const length = vertices.length * 4;
		gl.bindBuffer(gl.ARRAY_BUFFER, srcBuffer);
		gl.bufferData(gl.ARRAY_BUFFER, data, gl.STATIC_DRAW);
		gl.bindBuffer(gl.COPY_READ_BUFFER, srcBuffer);
		gl.bindBuffer(gl.ARRAY_BUFFER, dstBuffer);
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(length), gl.STATIC_DRAW);
		gl2.copyBufferSubData(gl.SRC_ALPHA_SATURATE, gl.SRC_ALPHA_SATURATE, 0, 0, length);
		const copyBufferSubDataError = gl.getError();
		console.info("copyBufferSubDataError: " + copyBufferSubDataError);
		expect(copyBufferSubDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0491
	 * @tc.name testCopyBufferSubData_04
	 * @tc.desc Test copyBufferSubData.
	 */
	it('testCopyBufferSubData_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCopyBufferSubData_04 test start ...66');
		const srcBuffer = gl.createBuffer();
		const dstBuffer = gl.createBuffer();
		const vertices = [1, 2];
		const data = new Float32Array(vertices);
		const length = vertices.length * 4;
		gl.bindBuffer(gl.ARRAY_BUFFER, srcBuffer);
		gl.bufferData(gl.ARRAY_BUFFER, data, gl.STATIC_DRAW);
		gl.bindBuffer(gl.COPY_READ_BUFFER, srcBuffer);
		gl.bindBuffer(gl.ARRAY_BUFFER, dstBuffer);
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(length), gl.STATIC_DRAW);
		gl2.copyBufferSubData(gl.SRC_ALPHA_SATURATE, gl.SRC_ALPHA_SATURATE, 0, 0, gl
			.ONE_MINUS_CONSTANT_COLOR);
		const copyBufferSubDataError = gl.getError();
		console.info("copyBufferSubDataError: " + copyBufferSubDataError);
		expect(copyBufferSubDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0492
	 * @tc.name testGetBufferSubData
	 * @tc.desc Test getBufferSubData.
	 */
	it('testGetBufferSubData', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testGetBufferSubData test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		const vertices = [1, 2];
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);
		var arrBuffer = new ArrayBuffer(vertices.length * Float32Array.BYTES_PER_ELEMENT);
		gl2.getBufferSubData(gl.ARRAY_BUFFER, 0, new Float32Array(arrBuffer), 0, 0);
		const getBufferSubDataError = gl.getError();
		console.info("getBufferSubDataError: " + getBufferSubDataError);
		expect(getBufferSubDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0493
	 * @tc.name testGetBufferSubData_01
	 * @tc.desc Test getBufferSubData.
	 */
	it('testGetBufferSubData_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testGetBufferSubData_01 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		const vertices = [1, 2];
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);
		var arrBuffer = new ArrayBuffer(vertices.length * Float32Array.BYTES_PER_ELEMENT);
		gl2.getBufferSubData(gl.SRC_ALPHA_SATURATE, 0, new Float32Array(arrBuffer), 0, 0);
		const getBufferSubDataError = gl.getError();
		console.info("getBufferSubDataError: " + getBufferSubDataError);
		expect(getBufferSubDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0494
	 * @tc.name testGetBufferSubData_02
	 * @tc.desc Test getBufferSubData.
	 */
	it('testGetBufferSubData_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testGetBufferSubData_02 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		const vertices = [1, 2];
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);
		var arrBuffer = new ArrayBuffer(vertices.length * Float32Array.BYTES_PER_ELEMENT);
		gl2.getBufferSubData(gl.FUNC_SUBTRACT, 0, new Float32Array(arrBuffer), 0, 0);
		const getBufferSubDataError = gl.getError();
		console.info("getBufferSubDataError: " + getBufferSubDataError);
		expect(getBufferSubDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0495
	 * @tc.name testGetBufferSubData_03
	 * @tc.desc Test getBufferSubData.
	 */
	it('testGetBufferSubData_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testGetBufferSubData_03 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		const vertices = [1, 2];
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);
		var arrBuffer = new ArrayBuffer(vertices.length * Float32Array.BYTES_PER_ELEMENT);
		gl2.getBufferSubData(gl.BLEND_DST_RGB, 0, new Int32Array(arrBuffer), 0, 0);
		const getBufferSubDataError = gl.getError();
		console.info("getBufferSubDataError: " + getBufferSubDataError);
		expect(getBufferSubDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0496
	 * @tc.name testGetBufferSubData_04
	 * @tc.desc Test getBufferSubData.
	 */
	it('testGetBufferSubData_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testGetBufferSubData_04 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		const vertices = [1, 2];
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.BLEND_SRC_ALPHA);
		var arrBuffer = new ArrayBuffer(vertices.length * Float32Array.BYTES_PER_ELEMENT);
		gl2.getBufferSubData(gl.ARRAY_BUFFER, false, new Int32Array(arrBuffer), 0, 0);
		const getBufferSubDataError = gl.getError();
		console.info("getBufferSubDataError: " + getBufferSubDataError);
		expect(getBufferSubDataError).assertEqual(gl.INVALID_VALUE);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0497
	 * @tc.name testBlitFramebuffer
	 * @tc.desc Test blitFramebuffer.
	 */
	it('testBlitFramebuffer', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBlitFramebuffer test start ...66');
		console.info('jsWebGL testBlitFramebuffer test start ...' + JSON.stringify(gl));
		gl2.blitFramebuffer(0, 0, 400, 1080, 0, 0, 400, 1080, gl2.COLOR_BUFFER_BIT, gl2.NEAREST);
		const blitFramebufferError = gl.getError();
		console.info("blitFramebufferError: " + blitFramebufferError);
		expect(blitFramebufferError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0498
	 * @tc.name testFramebufferTextureLayer
	 * @tc.desc Test framebufferTextureLayer.
	 */
	it('testFramebufferTextureLayer', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testFramebufferTextureLayer test start ...66');
		console.info('jsWebGL testFramebufferTextureLayer test start ...' + JSON.stringify(gl));
		var texture = gl.createTexture();
		gl2.framebufferTextureLayer(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0,
			texture, 0, 8)
		const framebufferTextureLayerError = gl.getError();
		console.info("framebufferTextureLayerError: " + framebufferTextureLayerError);
		expect(framebufferTextureLayerError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0499
	 * @tc.name testInvalidateFramebuffer
	 * @tc.desc Test invalidateFramebuffer.
	 */
	it('testInvalidateFramebuffer', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testInvalidateFramebuffer test start ...66');
		console.info('jsWebGL testInvalidateFramebuffer test start ...' + JSON.stringify(gl));
		gl2.invalidateFramebuffer(gl.READ_FRAMEBUFFER,
			[gl.COLOR_ATTACHMENT0, gl.COLOR_ATTACHMENT1]);
		const invalidateFramebufferError = gl.getError();
		console.info("invalidateFramebufferError: " + invalidateFramebufferError);
		expect(invalidateFramebufferError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0500
	 * @tc.name testInvalidateSubFramebuffer
	 * @tc.desc Test invalidateSubFramebuffer.
	 */
	it('testInvalidateSubFramebuffer', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testInvalidateSubFramebuffer test start ...66');
		console.info('jsWebGL testInvalidateSubFramebuffer test start ...' + JSON.stringify(gl));
		gl2.invalidateSubFramebuffer(gl.READ_FRAMEBUFFER,
			[gl.COLOR_ATTACHMENT0, gl.COLOR_ATTACHMENT1],
			0, 0, 256, 256);
		const invalidateSubFramebufferError = gl.getError();
		console.info("invalidateSubFramebufferError: " + invalidateSubFramebufferError);
		expect(invalidateSubFramebufferError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0501
	 * @tc.name testReadBuffer
	 * @tc.desc Test readBuffer.
	 */
	it('testReadBuffer', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testReadBuffer test start ...66');
		console.info('jsWebGL testReadBuffer test start ...' + JSON.stringify(gl));
		gl2.readBuffer(gl.COLOR_ATTACHMENT0);
		const readBufferError = gl.getError();
		console.info("readBufferError: " + readBufferError);
		expect(readBufferError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0502
	 * @tc.name testGetInternalformatParameter
	 * @tc.desc Test getInternalformatParameter.
	 */
	it('testGetInternalformatParameter', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testGetInternalformatParameter test start ...66');
		var samples = gl2.getInternalformatParameter(gl.RENDERBUFFER, gl2.RGBA8, gl.SAMPLES);
		console.info("samples: " + samples);
		const int32list = [-1, -1, -1];
		expect(samples.toString() != int32list.toString()).assertEqual(true);
		//判断数据类型是否正确
		//        expect(samples.constructor).assertEqual(int32list.constructor);
		//判断数据值是否正确
		//        expect(samples.toString()).assertEqual(int32list.toString());
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0503
	 * @tc.name testGetInternalformatParameter_01
	 * @tc.desc Test getInternalformatParameter.
	 */
	it('testGetInternalformatParameter_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testGetInternalformatParameter_01 test start ...66');
		var samples = gl2.getInternalformatParameter(gl.RENDERBUFFER, gl2.RGBA8, gl.SAMPLES);
		console.info("samples: " + samples);
		const int32list = new Int32Array([-1, -1, -1]);
		expect(samples.constructor).assertEqual(int32list.constructor);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0504
	 * @tc.name testRenderbufferStorageMultisample
	 * @tc.desc Test renderbufferStorageMultisample.
	 */
	it('testRenderbufferStorageMultisample', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testRenderbufferStorageMultisample test start ...66');
		gl2.renderbufferStorageMultisample(gl.RENDERBUFFER, 4, gl.RBGA4, 256, 256);
		const renderbufferStorageMultisampleError = gl.getError();
		console.info("renderbufferStorageMultisampleError: " + renderbufferStorageMultisampleError);
		expect(renderbufferStorageMultisampleError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0505
	 * @tc.name testTexStorage2D
	 * @tc.desc Test texStorage2D.
	 */
	it('testTexStorage2D', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexStorage2D test start ...66');
		console.info('jsWebGL testTexStorage2D test start ...' + JSON.stringify(gl));
		gl2.texStorage2D(gl.TEXTURE_2D, 1, gl.RGB8, 256, 256);
		const texStorage2DError = gl.getError();
		console.info("texStorage2DError: " + texStorage2DError);
		expect(texStorage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0506
	 * @tc.name testTransformFeedbackVaryings
	 * @tc.desc Test transformFeedbackVaryings.
	 */
	it('testTransformFeedbackVaryings', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTransformFeedbackVaryings test start ...66');
		console.info('jsWebGL testTransformFeedbackVaryings test start ...' + JSON.stringify(gl));
		const program = gl.createProgram();
		var transformFeedbackOutputs = ['gl_Position', 'anotherOutput'];
		gl2.transformFeedbackVaryings(program, transformFeedbackOutputs, gl.INTERLEAVED_ATTRIBS);
		const transformFeedbackVaryingsError = gl.getError();
		console.info("transformFeedbackVaryingsError: " + transformFeedbackVaryingsError);
		expect(transformFeedbackVaryingsError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0507
	 * @tc.name testGetTransformFeedbackVarying
	 * @tc.desc Test getTransformFeedbackVarying.
	 */
	it('testGetTransformFeedbackVarying', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetTransformFeedbackVarying test start ...66');
			console.info('jsWebGL testGetTransformFeedbackVarying test start ...' + JSON.stringify(
				gl));
			const programObj = createProgram(gl);
			const programError = gl.getError();
			console.info("programError: " + programError);
			var getTransformFeedbackVaryingobject = gl2.getTransformFeedbackVarying(programObj, 0);
			console.info("getTransformFeedbackVaryingobject: " + getTransformFeedbackVaryingobject);
			const getTransformFeedbackVaryingError = gl.getError();
			console.info("getTransformFeedbackVaryingError: " + getTransformFeedbackVaryingError);
			expect(getTransformFeedbackVaryingError).assertEqual(gl.NO_ERROR);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetTransformFeedbackVarying has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0508
	 * @tc.name testPauseTransformFeedback
	 * @tc.desc Test pauseTransformFeedback.
	 */
	it('testPauseTransformFeedback', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testPauseTransformFeedback test start ...66');
			console.info('jsWebGL testPauseTransformFeedback test start ...' + JSON.stringify(gl));
			const transformFeedback = gl2.createTransformFeedback();
			gl2.bindTransformFeedback(gl.TRANSFORM_FEEDBACK, transformFeedback);
			gl2.beginTransformFeedback(gl2.TRIANGLES);
			gl2.pauseTransformFeedback();
			const pauseTransformFeedbackError = gl.getError();
			console.info("pauseTransformFeedbackError: " + pauseTransformFeedbackError);
			expect(pauseTransformFeedbackError).assertEqual(gl.INVALID_OPERATION);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testPauseTransformFeedback has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0509
	 * @tc.name testResumeTransformFeedback
	 * @tc.desc Test resumeTransformFeedback.
	 */
	it('testResumeTransformFeedback', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testPauseTransformFeedback test start ...66');
			console.info('jsWebGL testPauseTransformFeedback test start ...' + JSON.stringify(gl));
			gl2.resumeTransformFeedback();
			const resumeTransformFeedbackError = gl.getError();
			console.info("resumeTransformFeedbackError: " + resumeTransformFeedbackError);
			expect(resumeTransformFeedbackError).assertEqual(gl.INVALID_OPERATION);
			//deleteContext();
			done();
		} catch (e) {
			console.log("resumeTransformFeedbackError has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0510
	 * @tc.name testBindBufferBase
	 * @tc.desc Test bindBufferBase.
	 */
	it('testBindBufferBase', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testBindBufferBase test start ...66');
			console.info('jsWebGL testBindBufferBase test start ...' + JSON.stringify(gl));
			let buffer = gl.createBuffer();
			gl2.bindBufferBase(gl.TRANSFORM_FEEDBACK_BUFFER, 0, buffer);
			const bindBufferBaseError = gl.getError();
			console.info("bindBufferBaseError: " + bindBufferBaseError);
			expect(bindBufferBaseError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("bindBufferBaseError has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0511
	 * @tc.name testGetIndexedParameter
	 * @tc.desc Test getIndexedParameter.
	 */
	it('testGetIndexedParameter', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetIndexedParameter test start ...66');
			console.info('jsWebGL testGetIndexedParameter test start ...' + JSON.stringify(gl));
			const WebGLBufferobject = gl2.getIndexedParameter(gl.TRANSFORM_FEEDBACK_BUFFER_BINDING,
				0);
			const isBuffer = gl.isBuffer(WebGLBufferobject);
			expect(isBuffer).assertEqual(false);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetIndexedParameter has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0512
	 * @tc.name testGetIndexedParameter_01
	 * @tc.desc Test getIndexedParameter.
	 */
	it('testGetIndexedParameter_01', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetIndexedParameter_01 test start ...66');
			console.info('jsWebGL testGetIndexedParameter_01 test start ...' + JSON.stringify(gl));
			const reurGLsizeiptr = gl2.getIndexedParameter(gl.TRANSFORM_FEEDBACK_BUFFER_SIZE, 0);
			const typeGLsizeiptr = typeof(reurGLsizeiptr);
			expect(typeGLsizeiptr).assertInstanceOf('String');
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetIndexedParameter_01 has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0513
	 * @tc.name testGetIndexedParameter_02
	 * @tc.desc Test getIndexedParameter.
	 */
	it('testGetIndexedParameter_02', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetIndexedParameter_02 test start ...66');
			console.info('jsWebGL testGetIndexedParameter_02 test start ...' + JSON.stringify(gl));
			const retuGLintptr = gl2.getIndexedParameter(gl.TRANSFORM_FEEDBACK_BUFFER_START, 0);
			const typeGLintptr = typeof(retuGLintptr);
			expect(typeGLintptr).assertInstanceOf('String');
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetIndexedParameter_02 has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0514
	 * @tc.name testGetIndexedParameter_03
	 * @tc.desc Test getIndexedParameter.
	 */
	it('testGetIndexedParameter_03', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetIndexedParameter_03 test start ...66');
			console.info('jsWebGL testGetIndexedParameter_03 test start ...' + JSON.stringify(gl));
			const WebGLBufferobject = gl2.getIndexedParameter(gl.UNIFORM_BUFFER_BINDING, 0);
			const isBuffer = gl.isBuffer(WebGLBufferobject);
			expect(isBuffer).assertEqual(false);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetIndexedParameter_03 has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0515
	 * @tc.name testGetIndexedParameter_04
	 * @tc.desc Test getIndexedParameter.
	 */
	it('testGetIndexedParameter_04', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetIndexedParameter_04 test start ...66');
			console.info('jsWebGL testGetIndexedParameter_04 test start ...' + JSON.stringify(gl));
			const reurGLsizeiptr = gl2.getIndexedParameter(gl.UNIFORM_BUFFER_SIZE, 0);
			const typeGLsizeiptr = typeof(reurGLsizeiptr);
			expect(typeGLsizeiptr).assertInstanceOf('String');
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetIndexedParameter_04 has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0516
	 * @tc.name testGetIndexedParameter_05
	 * @tc.desc Test getIndexedParameter.
	 */
	it('testGetIndexedParameter_05', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetIndexedParameter_05 test start ...66');
			console.info('jsWebGL testGetIndexedParameter_05 test start ...' + JSON.stringify(gl));
			const retuGLintptr = gl2.getIndexedParameter(gl.UNIFORM_BUFFER_START, 0);
			const typeGLintptr = typeof(retuGLintptr);
			expect(typeGLintptr).assertInstanceOf('String');
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetIndexedParameter_05 has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0517
	 * @tc.name testGetUniformIndices
	 * @tc.desc Test getUniformIndices.
	 */
	it('testGetUniformIndices', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetUniformIndices test start ...66');
			console.info('jsWebGL testGetUniformIndices test start ...' + JSON.stringify(gl));
			const program = gl.createProgram();
			const retuGLintptr = gl2.getUniformIndices(program, ['UBORed', 'UBOGreen', 'UBOBlue']);
			console.log("testGetUniformIndices has failed for " + retuGLintptr)
			const int32list = [-1, -1, -1];
			console.log("testGetUniformIndices has failed for " + int32list)
			//判断数据类型是否正确
			expect(retuGLintptr.constructor).assertEqual(int32list.constructor);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetUniformIndices has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0518
	 * @tc.name testGetUniformIndices_01
	 * @tc.desc Test getUniformIndices.
	 */
	it('testGetUniformIndices_01', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetUniformIndices_01 test start ...66');
			const program = gl.createProgram();
			const retuGLintptr = gl2.getUniformIndices(program, ['UBORed', 'UBOGreen', 'UBOBlue']);
			console.log("testGetUniformIndices has failed for " + retuGLintptr)
			const int32list = [-1, -1, -1];
			console.log("testGetUniformIndices_01 has failed for " + int32list)
			//判断数据值是否正确
			expect(retuGLintptr.toString() != int32list.toString()).assertEqual(true);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetUniformIndices has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0519
	 * @tc.name testGetActiveUniforms
	 * @tc.desc Test getActiveUniforms.
	 */
	it('testGetActiveUniforms', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetActiveUniforms test start ...66');
			console.info('jsWebGL testGetActiveUniforms test start ...' + JSON.stringify(gl));
			//        const programObj = createProgram(gl);


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



			const uniformIndices = [1, 2, 3];
			const uniformType = gl2.getActiveUniforms(programObj, uniformIndices, gl2.UNIFORM_TYPE)
			const glenumList = [-1, -1, -1];
			expect(uniformType.toString() != glenumList.toString()).assertEqual(true);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetActiveUniforms has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0520
	 * @tc.name testGetActiveUniforms_01
	 * @tc.desc Test getActiveUniforms.
	 */
	it('testGetActiveUniforms_01', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetActiveUniforms_01 test start ...66');
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

			const uniformIndices = [1, 2, 3];
			const uniformType = gl2.getActiveUniforms(programObj, uniformIndices, gl2.UNIFORM_TYPE)
			const glenumList = [-1, -1, -1];
			expect(uniformType.constructor).assertEqual(glenumList.constructor);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetActiveUniforms has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0521
	 * @tc.name testGetActiveUniforms_02
	 * @tc.desc Test getActiveUniforms.
	 */
	it('testGetActiveUniforms_02', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetActiveUniforms_02 test start ...66');
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

			const uniformIndices = [4, 5, 6];
			const uniformSize = gl2.getActiveUniforms(programObj, uniformIndices, gl2.UNIFORM_SIZE)
			const gluintList = [-1, 0, 0];
			expect(uniformSize.toString() != gluintList.toString()).assertEqual(true);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetActiveUniforms_02 has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0522
	 * @tc.name testGetActiveUniforms_03
	 * @tc.desc Test getActiveUniforms.
	 */
	it('testGetActiveUniforms_03', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetActiveUniforms_03 test start ...66');
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
			const uniformIndices = [7, 8, 9];
			const uniformBlockIndex = gl2.getActiveUniforms(programObj, uniformIndices, gl2
				.UNIFORM_BLOCK_INDEX)
			const glintlist = [-1, 0, 0];
			expect(uniformBlockIndex.toString() != glintlist.toString()).assertEqual(true);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetActiveUniforms_03 has failed for " + e)
			expect(null).assertFail()
		}
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0523
	 * @tc.name testGetActiveUniforms_04
	 * @tc.desc Test getActiveUniforms.
	 */
	it('testGetActiveUniforms_04', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetActiveUniforms_04 test start ...66');
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
			const uniformIndices = [1, 2, 3];
			const uniformOffset = gl2.getActiveUniforms(programObj, uniformIndices, gl2
				.UNIFORM_OFFSET)
			const glintlist = [-1, 0, 0];
			expect(uniformOffset.toString() != glintlist.toString()).assertEqual(true);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetActiveUniforms_04 has failed for " + e)
			expect(null).assertFail()
		}
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0524
	 * @tc.name testGetActiveUniforms_05
	 * @tc.desc Test getActiveUniforms.
	 */
	it('testGetActiveUniforms_05', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetActiveUniforms_05 test start ...66');
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
			const uniformIndices = [1, 2, 3];
			const uniformStride = gl2.getActiveUniforms(programObj, uniformIndices, gl2
				.UNIFORM_ARRAY_STRIDE)
			const glintlist = [-1, 0, 0];
			expect(uniformStride.toString() != glintlist.toString()).assertEqual(true);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetActiveUniforms_05 has failed for " + e)
			expect(null).assertFail()
		}
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0525
	 * @tc.name testGetActiveUniforms_06
	 * @tc.desc Test getActiveUniforms.
	 */
	it('testGetActiveUniforms_06', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetActiveUniforms_06 test start ...66');
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
			const uniformIndices = [1, 2, 3];
			const uniformMatrixStride = gl2.getActiveUniforms(programObj, uniformIndices, gl2
				.UNIFORM_MATRIX_STRIDE)
			const glbooleanlist = [-1, 0, 0];
			expect(uniformMatrixStride.toString() != glbooleanlist.toString()).assertEqual(true);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetActiveUniforms_06 has failed for " + e)
			expect(null).assertFail()
		}
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0526
	 * @tc.name testGetActiveUniforms_07
	 * @tc.desc Test getActiveUniforms.
	 */
	it('testGetActiveUniforms_07', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetActiveUniforms_07 test start ...66');
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
			const uniformIndices = [-1, -2, -3];
			const uniformOffset = gl2.getActiveUniforms(programObj, uniformIndices, gl2
				.UNIFORM_IS_ROW_MAJOR)
			const glintlist = [-1, false, false];
			//判断数据值是否正确
			expect(uniformOffset.toString() != glintlist.toString()).assertEqual(true);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetActiveUniforms_07 has failed for " + e)
			expect(null).assertFail()
		}
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0527
	 * @tc.name testUseProgram
	 * @tc.desc Test useProgram.
	 */
	it('testUseProgram', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testUseProgram test start ...66');
			console.info('jsWebGL testUseProgram test start ...' + JSON.stringify(gl));
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
			const program = gl.createProgram();
			console.log("testUseProgram has failed for " + program)
			const useProgramError1 = gl.getError();
			console.info("useProgramError: " + useProgramError1);
			const renderBufferValue1 = gl.getParameter(gl.CURRENT_PROGRAM);
			console.log("testUseProgram has failed for " + program)
			gl.attachShader(program, vertexShader);
			gl.attachShader(program, fragmentShader);
			gl.linkProgram(program);
			gl.useProgram(program);
			const useProgramError = gl.getError();
			console.info("useProgramError: " + useProgramError);
			expect(useProgramError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testUseProgram has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0528
	 * @tc.name testValidateProgram
	 * @tc.desc Test validateProgram.
	 */
	it('testValidateProgram', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testValidateProgram test start ...66');
			console.info('jsWebGL testValidateProgram test start ...' + JSON.stringify(gl));
			const program = gl.createProgram();
			gl.validateProgram(program);
			const validateProgramError = gl.getError();
			console.info("validateProgramError: " + validateProgramError);
			expect(validateProgramError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testValidateProgram has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0529
	 * @tc.name testVertexAttrib1f
	 * @tc.desc Test vertexAttrib1f.
	 */
	it('testVertexAttrib1f', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttrib1f test start ...66');
			console.info('jsWebGL testVertexAttrib1f test start ...' + JSON.stringify(gl));
			gl.vertexAttrib1f(1, 10.0);
			const vertexAttrib1fError = gl.getError();
			console.info("vertexAttrib1fError: " + vertexAttrib1fError);
			expect(vertexAttrib1fError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testVertexAttrib1f has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0530
	 * @tc.name testVertexAttrib2f
	 * @tc.desc Test vertexAttrib2f.
	 */
	it('testVertexAttrib2f', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttrib2f test start ...66');
			gl.vertexAttrib2f(1, 10.0);
			const vertexAttrib2fError = gl.getError();
			console.info("vertexAttrib2fError: " + vertexAttrib2fError);
			expect(vertexAttrib2fError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testVertexAttrib2f has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0531
	 * @tc.name testVertexAttrib3f
	 * @tc.desc Test vertexAttrib3f.
	 */
	it('testVertexAttrib3f', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testvertexAttrib3f test start ...66');
			console.info('jsWebGL testvertexAttrib3f test start ...' + JSON.stringify(gl));
			gl.vertexAttrib3f(1, 10.0);
			const vertexAttrib3fError = gl.getError();
			console.info("vertexAttrib3fError: " + vertexAttrib3fError);
			expect(vertexAttrib3fError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testVertexAttrib3f has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0532
	 * @tc.name testVertexAttrib4f
	 * @tc.desc Test vertexAttrib4f.
	 */
	it('testVertexAttrib4f', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttrib4f test start ...66');
			console.info('jsWebGL testVertexAttrib4f test start ...' + JSON.stringify(gl));
			gl.vertexAttrib4f(1, 10.0);
			const vertexAttrib4fError = gl.getError();
			console.info("vertexAttrib4fError: " + vertexAttrib4fError);
			expect(vertexAttrib4fError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testvertexAttrib4f has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0533
	 * @tc.name testVertexAttrib1fv
	 * @tc.desc Test vertexAttrib1fv.
	 */
	it('testVertexAttrib1fv', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttrib1fv test start ...66');
			console.info('jsWebGL testVertexAttrib1fv test start ...' + JSON.stringify(gl));
			const floatArray = new Float32Array([10.0, 5.0, 2.0]);
			gl.vertexAttrib1fv(1, floatArray);
			const vertexAttrib1fvError = gl.getError();
			console.info("vertexAttrib1fvError: " + vertexAttrib1fvError);
			expect(vertexAttrib1fvError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testVertexAttrib1fv has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0534
	 * @tc.name testVertexAttrib2fv
	 * @tc.desc Test vertexAttrib2fv.
	 */
	it('testVertexAttrib2fv', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttrib2fv test start ...66');
			console.info('jsWebGL testVertexAttrib2fv test start ...' + JSON.stringify(gl));
			const floatArray = new Float32Array([10.0, 5.0, 2.0]);
			gl.vertexAttrib2fv(1, floatArray);
			const vertexAttrib2fvError = gl.getError();
			console.info("vertexAttrib2fvError: " + vertexAttrib2fvError);
			expect(vertexAttrib2fvError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testVertexAttrib2fv has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0535
	 * @tc.name testVertexAttrib3fv
	 * @tc.desc Test vertexAttrib3fv.
	 */
	it('testVertexAttrib3fv', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttrib3fv test start ...66');
			console.info('jsWebGL testVertexAttrib3fv test start ...' + JSON.stringify(gl));
			const floatArray = new Float32Array([10.0, 5.0, 2.0]);
			gl.vertexAttrib3fv(1, floatArray);
			const vertexAttrib3fvError = gl.getError();
			console.info("vertexAttrib3fvError: " + vertexAttrib3fvError);
			expect(vertexAttrib3fvError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testVertexAttrib3fv has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0536
	 * @tc.name testVertexAttrib4fv
	 * @tc.desc Test vertexAttrib4fv.
	 */
	it('testVertexAttrib4fv', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttrib4fv test start ...66');
			console.info('jsWebGL testVertexAttrib4fv test start ...' + JSON.stringify(gl));
			const floatArray = new Float32Array([10.0, 5.0, 2.0]);
			gl.vertexAttrib4fv(1, floatArray);
			const vertexAttrib4fvError = gl.getError();
			console.info("vertexAttrib4fvError: " + vertexAttrib4fvError);
			expect(vertexAttrib4fvError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testVertexAttrib4fv has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0537
	 * @tc.name testVertexAttribPointer
	 * @tc.desc Test vertexAttribPointer.
	 */
	it('testVertexAttribPointer', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttribPointer test start ...66');
			console.info('jsWebGL testVertexAttribPointer test start ...' + JSON.stringify(gl));
			gl.vertexAttribPointer(0, 3, gl.FLOAT, false, 20, 0);
			const vertexAttribPointerError = gl.getError();
			console.info("vertexAttribPointerError: " + vertexAttribPointerError);
			expect(vertexAttribPointerError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testVertexAttribPointer has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0538
	 * @tc.name testViewport
	 * @tc.desc Test viewport.
	 */
	it('testViewport', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testViewport test start ...66');
			console.info('jsWebGL testViewport test start ...' + JSON.stringify(gl));
			gl.viewport(1, 2, 3, 4);
			const viewportError = gl.getError();
			console.info("viewportError: " + viewportError);
			expect(viewportError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testViewport has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0539
	 * @tc.name testTexStorage3D
	 * @tc.desc Test texStorage3D.
	 */
	it('testTexStorage3D', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexStorage3D test start ...66');
			console.info('jsWebGL testTexStorage3D test start ...' + JSON.stringify(gl));
			gl2.texStorage3D(gl.TEXTURE_3D, 1, gl.RGB8, 256, 256, 256);
			const texStorage3DError = gl.getError();
			console.info("texStorage3DError: " + texStorage3DError);
			expect(texStorage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexStorage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0540
	 * @tc.name testTexImage3D
	 * @tc.desc Test texImage3D.
	 */
	it('testTexImage3D', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexImage3D test start ...66');
			console.info('jsWebGL testTexImage3D test start ...' + JSON.stringify(gl));
			gl2.texImage3D(gl2.TEXTURE_3D, 0, gl.RGBA, 1, 1, 1, 0, gl.RGBA, gl.UNSIGNED_BYTE, 0);
			const texImage3DError = gl.getError();
			console.info("texImage3DError: " + texImage3DError);
			expect(texImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexImage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0541
	 * @tc.name testTexImage3D_01
	 * @tc.desc Test texImage3D.
	 */
	it('testTexImage3D_01', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexImage3D_01 test start ...66');
			console.info('jsWebGL testTexImage3D_01 test start ...' + JSON.stringify(gl));
			const buffer = new ArrayBuffer(8);
			gl2.texImage3D(gl2.TEXTURE_3D, 0, gl.RGBA, 1, 1, 1, 0, gl.RGBA, gl.UNSIGNED_BYTE,
				buffer);
			const texImage3DError = gl.getError();
			console.info("texImage3DError: " + texImage3DError);
			expect(texImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexImage3D_01 has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0542
	 * @tc.name testTexImage3D_02
	 * @tc.desc Test texImage3D.
	 */
	it('testTexImage3D_02', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexImage3D_02 test start ...66');
			console.info('jsWebGL testTexImage3D_02 test start ...' + JSON.stringify(gl));
			const buffer = new ArrayBuffer(8);
			gl2.texImage3D(gl2.TEXTURE_3D, 0, gl.RGBA, 1, 1, 1, 0, gl.RGBA, gl.UNSIGNED_BYTE,
				buffer, 0);
			const texImage3DError = gl.getError();
			console.info("texImage3DError: " + texImage3DError);
			expect(texImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexImage3D_02 has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0543
	 * @tc.name testTexSubImage3D
	 * @tc.desc Test texSubImage3D.
	 */
	it('testTexSubImage3D', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexSubImage3D test start ...66');
			console.info('jsWebGL testTexSubImage3D test start ...' + JSON.stringify(gl));
			gl2.texSubImage3D(gl.TEXTURE_3D, 0, 0, 0, 0, 200, 200, 1, gl.RGBA, gl.UNSIGNED_BYTE, 1);
			const texSubImage3DError = gl.getError();
			console.info("texSubImage3DError: " + texSubImage3DError);
			expect(texSubImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexSubImage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0544
	 * @tc.name testTexSubImage3D_01
	 * @tc.desc Test texSubImage3D.
	 */
	it('testTexSubImage3D_01', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexSubImage3D_01 test start ...66');
			console.info('jsWebGL testTexSubImage3D_01 test start ...' + JSON.stringify(gl));
			const buffer = new ArrayBuffer(16);
			const view = new DataView(buffer, 12, 4);
			gl2.texSubImage3D(gl.TEXTURE_3D, 0, 0, 0, 0, 200, 200, 1, gl.RGBA, gl.UNSIGNED_BYTE,
				buffer, 0);
			const texSubImage3DError = gl.getError();
			console.info("texSubImage3DError: " + texSubImage3DError);
			expect(texSubImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexSubImage3D_01 has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0545
	 * @tc.name testCopyTexSubImage3D
	 * @tc.desc Test copyTexSubImage3D.
	 */
	it('testCopyTexSubImage3D', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testCopyTexSubImage3D test start ...66');
			console.info('jsWebGL testCopyTexSubImage3D test start ...' + JSON.stringify(gl));
			gl2.copyTexSubImage3D(gl.TEXTURE_3D, 0, 0, 0, 0, 0, 0, 16, 16);
			const copyTexSubImage3DError = gl.getError();
			console.info("copyTexSubImage3DError: " + copyTexSubImage3DError);
			expect(copyTexSubImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testCopyTexSubImage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0546
	 * @tc.name testCompressedTexImage3D_01
	 * @tc.desc Test compressedTexImage3D.
	 */
	it('testCompressedTexImage3D_01', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testCompressedTexImage3D_01 test start ...66');
			console.info('jsWebGL testCompressedTexImage3D_01 test start ...' + JSON.stringify(gl));
			gl2.compressedTexImage3D(gl.TEXTURE_3D, 0, 0x83F0, 512, 512, 512, 0, gl2
				.PIXEL_UNPACK_BUFFER, 0);
			const compressedTexImage3DError = gl.getError();
			console.info("compressedTexImage3DError: " + compressedTexImage3DError);
			expect(compressedTexImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testCompressedTexImage3D_01 has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0547
	 * @tc.name testCompressedTexImage3D_02
	 * @tc.desc Test compressedTexImage3D.
	 */
	it('testCompressedTexImage3D_02', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testCompressedTexImage3D_02 test start ...66');
			console.info('jsWebGL testCompressedTexImage3D_02 test start ...' + JSON.stringify(gl));
			const buffer = new ArrayBuffer(8);
			gl2.compressedTexImage3D(gl.TEXTURE_3D, 0, 0x83F0, 512, 512, 512, 0, buffer, 0, 0);
			const compressedTexImage3DError = gl.getError();
			console.info("compressedTexImage3DError: " + compressedTexImage3DError);
			expect(compressedTexImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testCompressedTexImage3D_02 has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0548
	 * @tc.name testCompressedTexSubImage3D
	 * @tc.desc Test compressedTexSubImage3D.
	 */
	it('testCompressedTexSubImage3D', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testCompressedTexSubImage3D test start ...66');
			console.info('jsWebGL testCompressedTexSubImage3D test start ...' + JSON.stringify(gl));
			gl2.compressedTexSubImage3D(gl.TEXTURE_3D, 0, 0, 0, 0, 512, 512, 512, gl2
				.COMPRESSED_R11_EAC,
				gl2.PIXEL_UNPACK_BUFFER, 0);
			const compressedTexSubImage3DError = gl.getError();
			console.info("compressedTexSubImage3DError: " + compressedTexSubImage3DError);
			expect(compressedTexSubImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testCompressedTexSubImage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0549
	 * @tc.name testCompressedTexSubImage3D_01
	 * @tc.desc Test compressedTexSubImage3D.
	 */
	it('testCompressedTexSubImage3D_01', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testCompressedTexSubImage3D test start ...66');
			console.info('jsWebGL testCompressedTexSubImage3D test start ...' + JSON.stringify(gl));
			const buffer = new ArrayBuffer(8);
			gl2.compressedTexSubImage3D(gl.TEXTURE_3D, 0, 0, 0, 0, 512, 512, 512, gl2
				.COMPRESSED_R11_EAC, buffer, 0, 256);
			const compressedTexSubImage3DError = gl.getError();
			console.info("compressedTexSubImage3DError: " + compressedTexSubImage3DError);
			expect(compressedTexSubImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testCompressedTexSubImage3D_01 has failed for " + e)
			expect(null).assertFail()
		}
	});



	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0550
	 * @tc.name testGetActiveAttribError
	 * @tc.desc Test getActiveAttrib.
	 */
	it('testGetActiveAttribError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getActiveAttrib test start ...' + JSON.stringify(gl));
		const program = gl.createProgram();
		const info = gl.getActiveAttrib();
		expect(info).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0551
	 * @tc.name testGetActiveUniformError
	 * @tc.desc Test getActiveUniform.
	 */
	it('testGetActiveUniformError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getActiveUniform test start ...' + JSON.stringify(gl));
		const program = gl.createProgram();
		const info = gl.getActiveUniform(program);
		expect(info).assertEqual(undefined);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0552
	 * @tc.name testGetAttachedShadersError
	 * @tc.desc Test getAttachedShaders.
	 */
	it('testGetAttachedShadersError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getAttachedShaders test start ...' + JSON.stringify(gl));
		const shaderArray = gl.getAttachedShaders();
		console.info('getAttachedShaders shaderArray' + shaderArray);
		expect(shaderArray).assertEqual(undefined);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0553
	 * @tc.name testGetBufferParameterError
	 * @tc.desc Test getBufferParameter.
	 */
	it('testGetBufferParameterError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getBufferParameter test start ...' + JSON.stringify(gl));
		const buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ARRAY_BUFFER, 1024, gl.STATIC_DRAW);
		const bufferSize = gl.getBufferParameter(gl.ARRAY_BUFFER);
		expect(bufferSize).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0554
	 * @tc.name testGetParameterError
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameterError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		var textureParameter = gl.getParameter();
		console.info("activeTexture --> getParameter: " + textureParameter);
		expect(textureParameter).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0555
	 * @tc.name testGetError_error
	 * @tc.desc Test getParameter.
	 */
	it('testGetError_error', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.enable(gl.SCISSOR_TEST);
		const scissorErrorCode = gl.scissor(0);
		console.info("scissorErrorCode: " + scissorErrorCode);
		expect(scissorErrorCode).assertEqual(undefined);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0556
	 * @tc.name testGetFramebufferAttachmentParameterError
	 * @tc.desc Test frontFace.
	 */
	it('testGetFramebufferAttachmentParameterError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getFramebufferAttachmentParameter test start ...' + JSON.stringify(
			gl));
		const framebuffer = gl.getFramebufferAttachmentParameter(gl
			.FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE);
		console.info('Framebuffer' + framebuffer);
		expect(framebuffer).assertEqual(undefined);
		done();
	});




	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0557
	 * @tc.name testGetProgramInfoLog
	 * @tc.desc Test getProgramInfoLog.
	 */
	it('testGetProgramInfoLog', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getProgramInfoLog test start ...' + JSON.stringify(gl));
		const program = gl.createProgram();
		// Attach pre-existing shaders
		gl.attachShader(program, VSHADER_SOURCE);
		gl.attachShader(program, FSHADER_SOURCE);
		gl.linkProgram(program);
		gl.validateProgram(program);
		const info = gl.getProgramInfoLog();
		gl.useProgram(program);
		expect(info).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0558
	 * @tc.name testGetShaderPrecisionFormatError
	 * @tc.desc Test getShaderPrecisionFormat.
	 */
	it('testGetShaderPrecisionFormatError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getShaderPrecisionFormat test start ...' + JSON.stringify(gl));
		const shaderPrecisionFormat = gl.getShaderPrecisionFormat();
		console.info('shaderPrecisionFormat' + JSON.stringify(shaderPrecisionFormat));
		expect(shaderPrecisionFormat).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0559
	 * @tc.name testGetShaderInfoLogError
	 * @tc.desc Test getShaderInfoLog.
	 */
	it('testGetShaderInfoLogError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getShaderInfoLog test start ...' + JSON.stringify(gl));
		var shader = gl.createShader(gl.VERTEX_SHADER);
		gl.shaderSource(shader, "shaderCode");

		/* compile shader source code. */
		gl.compileShader(shader);
		const message = gl.getShaderInfoLog();
		console.info("getShaderInfoLog message: " + message);
		const type = (message === null);
		expect(type).assertEqual(true);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0560
	 * @tc.name testGetShaderSource
	 * @tc.desc Test getShaderSource.
	 */
	it('testGetShaderSourceError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getShaderSource test start ...' + JSON.stringify(gl));
		var shader = gl.createShader(gl.VERTEX_SHADER);
		gl.shaderSource(shader, 'originalSource');

		var source = gl.getShaderSource();
		console.info("getShaderSource source: " + source);
		expect(source).assertEqual(undefined);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0561
	 * @tc.name testGetVertexAttribFirst
	 * @tc.desc Test getVertexAttrib.
	 */
	it('testGetVertexAttribFirstError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getVertexAttrib test start ...' + JSON.stringify(gl));
		const type = gl.getVertexAttrib(0);
		console.info("getVertexAttrib: type" + type);
		expect(type).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0562
	 * @tc.name testGetVertexAttribOffset
	 * @tc.desc Test getVertexAttribOffset.
	 */
	it('testGetVertexAttribOffsetError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getVertexAttribOffset test start ...' + JSON.stringify(gl));
		const index = gl.getVertexAttribOffset();
		console.info("getVertexAttribOffset: index" + index);
		expect(index).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0563
	 * @tc.name testIsBufferError
	 * @tc.desc Test isBuffer.
	 */
	it('testIsBufferError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL isBuffer test start ...' + JSON.stringify(gl));
		const buffer = gl.createBuffer();
		const isBuffer1 = gl.isBuffer(buffer);
		console.info('isBuffer1' + isBuffer1);
		expect(isBuffer1).assertEqual(false);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0564
	 * @tc.name testIsEnabledError
	 * @tc.desc Test isEnabled.
	 */
	it('testIsEnabledError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL isEnabled test start ...' + JSON.stringify(gl));
		gl.disable(gl.STENCIL_TEST);
		const isEnabledValue2 = gl.isEnabled(gl.STENCIL_TEST);
		console.info("isEnabledValue2: " + isEnabledValue2);
		expect(isEnabledValue2).assertEqual(false);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0565
	 * @tc.name testRenderbufferStorageError
	 * @tc.desc Test renderbufferStorage.
	 */
	it('testRenderbufferStorageError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL renderbufferStorage test start ...' + JSON.stringify(gl));
		const returnValue = gl.renderbufferStorage(256, 256);
		console.info("getRenderbufferParameter returnValue: " + returnValue);
		expect(returnValue).assertEqual(undefined);
		done();
	});



	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0566
	 * @tc.name testSampleCoverageError
	 * @tc.desc Test sampleCoverage.
	 */
	it('testSampleCoverageError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL sampleCoverage test start ...' + JSON.stringify(gl));
		//initContext();
		console.info('jsWebGL sampleCoverage test start ...' + JSON.stringify(gl));

		const sampleCoverageInvert = gl.sampleCoverage();

		console.info("sampleCoverageInvert: " + sampleCoverageInvert);

		expect(sampleCoverageInvert).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0567
	 * @tc.name testScissorError
	 * @tc.desc Test scissor.
	 */
	it('testScissorError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL scissor test start ...' + JSON.stringify(gl));
		const scissorValue = gl.scissor(200);
		console.info("scissorValue: " + scissorValue);
		expect(scissorValue).assertEqual(undefined);

		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0568
	 * @tc.name testShaderSourceError
	 * @tc.desc Test shaderSource.
	 */
	it('testShaderSourceError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL shaderSource test start ...' + JSON.stringify(gl));
		var shader = gl.createShader(gl.VERTEX_SHADER);
		var source = gl.shaderSource(shader);
		console.info("shaderSource source: " + source);
		expect(source).assertEqual(undefined);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0569
	 * @tc.name testStencilFuncError
	 * @tc.desc Test stencilFunc.
	 */
	it('testStencilFuncError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL stencilFunc test start ...' + JSON.stringify(gl));
		gl.enable(gl.STENCIL_TEST);
		const stencilFuncParameter = gl.stencilFunc(gl.LESS);
		console.info("stencilFunc stencilFuncParameter: " + stencilFuncParameter);
		expect(stencilFuncParameter).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0570
	 * @tc.name testStencilFuncSeparateError
	 * @tc.desc Test stencilFuncSeparate.
	 */
	it('testStencilFuncSeparateError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL stencilFuncSeparate test start ...' + JSON.stringify(gl));
		gl.enable(gl.STENCIL_TEST);
		const stencilFuncSeparateParameter = gl.stencilFuncSeparate(gl.FRONT);
		console.info("stencilFuncSeparate stencilFuncSeparateParameter: " +
			stencilFuncSeparateParameter);
		expect(stencilFuncSeparateParameter).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0571
	 * @tc.name testStencilMaskError
	 * @tc.desc Test stencilMask.
	 */
	it('testStencilMaskError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL stencilMask test start ...' + JSON.stringify(gl));
		const stencilMaskParameter = gl.stencilMask();
		console.info("stencilMask stencilMaskParameter: " + stencilMaskParameter);
		expect(stencilMaskParameter).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0572
	 * @tc.name testStencilMaskSeparateError
	 * @tc.desc Test stencilMaskSeparate.
	 */
	it('testStencilMaskSeparateError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL stencilMaskSeparate test start ...' + JSON.stringify(gl));
		const stencilMaskSeparateParameter = gl.stencilMaskSeparate(gl.FRONT);
		console.info("stencilMaskSeparate stencilMaskSeparateParameter: " +
			stencilMaskSeparateParameter);
		expect(stencilMaskSeparateParameter).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0573
	 * @tc.name testStencilOpError
	 * @tc.desc Test stencilOp.
	 */
	it('testStencilOpError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL stencilOp test start ...' + JSON.stringify(gl));
		gl.enable(gl.STENCIL_TEST);
		const stencilOpParameter = gl.stencilOp();
		console.info("stencilOp stencilOpParameter: " + stencilOpParameter);
		expect(stencilOpParameter).assertEqual(undefined);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0574
	 * @tc.name testStencilOpSeparate
	 * @tc.desc Test stencilOpSeparate.
	 */
	it('testStencilOpSeparateError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL stencilOpSeparate test start ...' + JSON.stringify(gl));
		gl.enable(gl.STENCIL_TEST);
		const stencilOpSeparateParameter = gl.stencilOpSeparate(gl.FRONT, gl.INCR, gl.DECR, gl
			.INVERT);
		console.info("stencilOpSeparate stencilOpSeparateParameter: " + stencilOpSeparateParameter);
		expect(stencilOpSeparateParameter).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0575
	 * @tc.name testTexParameterfError
	 * @tc.desc Test texParameterf.
	 */
	it('testTexParameterfError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL texParameterf test start ...' + JSON.stringify(gl));
		const texParameterValue = gl.texParameterf();
		console.info('texParameterValue' + texParameterValue);
		expect(texParameterValue).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0576
	 * @tc.name testTexParameteriError
	 * @tc.desc Test texParameteri.
	 */
	it('testTexParameteriError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL texParameteri test start ...' + JSON.stringify(gl));
		const texParameterValue = gl.texParameteri();
		console.info('texParameterfValue' + texParameterValue);
		expect(texParameterValue).assertEqual(undefined);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0577
	 * @tc.name testGetUniformBlockIndexError
	 * @tc.desc Test getUniformBlockIndex.
	 */
	it('testGetUniformBlockIndexError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 getUniformBlockIndex test start ...' + JSON.stringify(gl2));
		const program = gl.createProgram();
		const blockIndex = gl2.getUniformBlockIndex(program);
		gl2.uniformBlockBinding(program, blockIndex, 1);
		console.info("uniformBlockBinding blockIndex: " + blockIndex);
		expect(blockIndex).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0578
	 * @tc.name testGetActiveUniformBlockParameterError
	 * @tc.desc Test getActiveUniformBlockParameter.
	 */
	it('testGetActiveUniformBlockParameterError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 getActiveUniformBlockParameter test start ...' + JSON.stringify(
			gl2));
		const program = globalFunction();
		const uniformLocation = gl.getUniformLocation(program, "a_Position");
		const int32Array = new Int32Array([1, 2]);
		gl2.uniform1iv(uniformLocation, int32Array);
		const blockIndex = gl2.getUniformBlockIndex(program, 'a_Position');
		gl2.uniformBlockBinding(program, blockIndex, 1);
		const blockSize = gl2.getActiveUniformBlockParameter();
		console.info("getActiveUniformBlockParameter blockSize" + blockSize);
		expect(blockSize).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0579
	 * @tc.name testGetActiveUniformBlockNameError
	 * @tc.desc Test getActiveUniformBlockName.
	 */
	it('testGetActiveUniformBlockNameError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 getActiveUniformBlockName test start ...' + JSON.stringify(gl2));
		const program = gl.createProgram();
		const blockIndex = gl2.getUniformBlockIndex(program, 'UBOData');
		console.info("getActiveUniformBlockName blockIndex" + blockIndex);

		gl2.uniformBlockBinding(program, blockIndex, 1);
		const blockName = gl2.getActiveUniformBlockName(program);
		console.info("blockName" + blockName);
		expect(blockName).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0580
	 * @tc.name testUniformBlockBindingError
	 * @tc.desc Test uniformBlockBinding.
	 */
	it('testUniformBlockBindingError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniformBlockBinding test start ...' + JSON.stringify(gl2));
		const program = gl.createProgram();
		const blockIndex = gl2.getUniformBlockIndex(program);
		gl2.uniformBlockBinding(program, blockIndex, 1);
		const blockName = gl2.getActiveUniformBlockName(program);
		console.info("blockName" + blockName);

		expect(blockName).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0581
	 * @tc.name testCreateVertexArrayError
	 * @tc.desc Test createVertexArray.
	 */
	it('testCreateVertexArrayError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 createVertexArray test start ...' + JSON.stringify(gl2));
		const vao = gl2.createVertexArray();
		const isVertexArray1 = gl2.isVertexArray(vao);
		console.info("isVertexArray1" + isVertexArray1);
		const isVertexArray = gl2.isVertexArray(vao);
		console.info("isVertexArray" + isVertexArray);
		expect(isVertexArray).assertEqual(false);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0582
	 * @tc.name testBindVertexArray
	 * @tc.desc Test bindVertexArray.
	 */
	it('testBindVertexArrayError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 bindVertexArray test start ...' + JSON.stringify(gl2));
		const vao = gl2.createVertexArray();
		const isVertexArray1 = gl2.isVertexArray(vao);
		console.info("isVertexArray1" + isVertexArray1);
		gl2.bindVertexArray();
		const isVertexArray = gl2.isVertexArray(vao);
		console.info("isVertexArray" + isVertexArray);
		expect(isVertexArray).assertEqual(false);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0583
	 * @tc.name testDeleteVertexArray
	 * @tc.desc Test deleteVertexArray.
	 */
	it('testDeleteVertexArrayError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 deleteVertexArray test start ...' + JSON.stringify(gl2));
		const vao = gl2.createVertexArray();
		gl2.bindVertexArray(vao);
		const isVertexArray1 = gl2.isVertexArray(vao);
		console.info("isVertexArray1" + isVertexArray1);
		gl2.deleteVertexArray(vao);
		const isVertexArray = gl2.isVertexArray(vao);
		console.info("isVertexArray" + isVertexArray);
		expect(isVertexArray).assertEqual(false);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0584
	 * @tc.name testIsVertexArrayError
	 * @tc.desc Test bindVertexArray.
	 */
	it('testIsVertexArray', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 isVertexArray test start ...' + JSON.stringify(gl2));
		const vao = gl2.createVertexArray();
		const isVertexArray1 = gl2.isVertexArray(vao);
		console.info("isVertexArray1" + isVertexArray1);
		gl2.bindVertexArray(vao);
		const isVertexArray = gl2.isVertexArray(vao);
		console.info("isVertexArray" + isVertexArray);
		expect(isVertexArray).assertEqual(true);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0585
	 * @tc.name testIsVertexArrayError
	 * @tc.desc Test bindVertexArray.
	 */
	it('testIsVertexArrayError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 isVertexArray test start ...' + JSON.stringify(gl2));
		const vao = gl2.createVertexArray();
		const isVertexArray1 = gl2.isVertexArray(vao);
		console.info("isVertexArray1" + isVertexArray1);
		gl2.bindVertexArray();
		const isVertexArray = gl2.isVertexArray();
		console.info("isVertexArray" + isVertexArray);
		expect(isVertexArray).assertEqual(false);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0586
	 * @tc.name testTexImage2DError
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2DError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texImage2D test start ...' + JSON.stringify(gl2));
		gl2.texImage2D(-gl.TEXTURE_2D, -0, -32, -512, -512, -0, -32, -32, -new ArrayBuffer(8));
		const errorCode = gl.getError();
		console.info("webgl2test texImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(1281);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0587
	 * @tc.name testTexSubImage2DError
	 * @tc.desc Test texSubImage2D.
	 */
	it('testTexSubImage2DError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texSubImage2D test start ...' + JSON.stringify(gl2));
		gl2.texSubImage2D(-gl.TEXTURE_2D, -0, -0, -0, -512, -512, -gl.RED, gl.FLOAT, -
			new ArrayBuffer(16), -0);
		const errorCode = gl.getError();
		console.info("webgl2test texSubImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(1280);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0588
	 * @tc.name testCompressedTexImage2DError
	 * @tc.desc Test compressedTexImage2D.
	 */
	it('testCompressedTexImage2DError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 compressedTexImage2D test start ...' + JSON.stringify(gl2));
		gl2.compressedTexImage2D(-gl.TEXTURE_2D, -0, -0x83F3, -512, -512, -0, -gl
			.PIXEL_UNPACK_BUFFER, -0);
		const errorCode = gl.getError();
		console.info("webgl2test compressedTexImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(1280);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0589
	 * @tc.name testCompressedTexSubImage2DError
	 * @tc.desc Test compressedTexSubImage2D.
	 */
	it('testCompressedTexSubImage2DError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 compressedTexSubImage2D test start ...' + JSON.stringify(gl2));
		gl2.compressedTexSubImage2D(-gl.TEXTURE_2D, -0, -256, -256, -512, -512, -0x83F3, -gl
			.PIXEL_UNPACK_BUFFER, 0);
		const errorCode = gl.getError();
		console.info("webgl2test compressedTexSubImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(1281);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0590
	 * @tc.name testBufferSubDataError
	 * @tc.desc Test bufferData.
	 */
	it('testBufferSubDataError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 bufferData test start ...' + JSON.stringify(gl2));
		gl2.bufferSubData(-gl.COPY_READ_BUFFER, -512, -new ArrayBuffer(8));
		const errorCode = gl.getError();
		console.info("webgl2test bufferSubData getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0591
	 * @tc.name webgl_test_getContextAttributes2
	 * @tc.desc Test getContextAttributes.
	 */
	it('webgl_test_getContextAttributes2', 0, async function(done) {
		//initContext();
		console.info("webgltest into getContextAttributes");
		const getContextAttributesValue = gl.getContextAttributes("error");
		console.info("webgltest getContextAttributes: " + getContextAttributesValue);
		expect(getContextAttributesValue).assertEqual(null);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0592
	 * @tc.name webgl_test_isContextLost2
	 * @tc.desc Test isContextLost.
	 */
	it('webgl_test_isContextLost2', 0, async function(done) {
		//initContext();
		console.info("webgltest into isContextLost");
		const isContextLostValue = gl.isContextLost("error");
		console.info("webgltest isContextLost: " + isContextLostValue);
		expect(isContextLostValue).assertEqual(false);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0593
	 * @tc.name webgl_test_getSupportedExtensions2
	 * @tc.desc Test getSupportedExtensions.
	 */
	it('webgl_test_getSupportedExtensions2', 0, async function(done) {
		//initContext();
		console.info("webgltest into getSupportedExtensions");
		const getSupportedExtensionsValue = gl.getSupportedExtensions("error");
		console.info("webgltest getSupportedExtensions: " + getSupportedExtensionsValue);
		expect(typeof(getSupportedExtensionsValue)).assertEqual('object');
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0595
	 * @tc.name webgl_test_getExtension2
	 * @tc.desc Test getExtension.
	 */
	it('webgl_test_getExtension2', 0, async function(done) {
		//initContext();
		console.info("webgltest into getExtension");
		const getExtensionValue = gl.getExtension("error");
		console.info("webgltest getExtension: " + getExtensionValue);
		expect(getExtensionValue).assertEqual(undefined);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0596
	 * @tc.name webgl_test_activeTexture2
	 * @tc.desc Test activeTexture.
	 */
	it('webgl_test_activeTexture2', 0, async function(done) {
		//initContext();
		console.info("webgltest into activeTexture");
		gl.activeTexture(gl.TEXTURE8B4C);
		const activeTextureParameter = gl.getParameter(gl.ACTIVE_TEXTURE);
		console.info("webgltest activeTexture --> getParameter: " + activeTextureParameter);
		expect(activeTextureParameter).assertEqual(33984);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0597
	 * @tc.name webgl_test_activeTexture_12
	 * @tc.desc Test activeTexture.
	 */
	it('webgl_test_activeTexture_12', 0, async function(done) {
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
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0598
	 * @tc.name webgl_test_attachShader2
	 * @tc.desc Test attachShader.
	 */
	it('webgl_test_attachShader2', 0, async function(done) {
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
		gl.attachShader(programObj, 'vertexShader');
		gl.attachShader(programObj, 'fragmentShader');
		gl.linkProgram(programObj);
		gl.useProgram(programObj);

		let errorCode = gl.getError();
		console.info("webgltest attachShader getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0599
	 * @tc.name webgl_test_bindAttribLocation2
	 * @tc.desc Test bindAttribLocation.
	 */
	it('webgl_test_bindAttribLocation2', 0, async function(done) {
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

		gl.bindAttribLocation(programObj, 1, 'V_color');

		let errorCode = gl.getError();
		console.info("webgltest bindAttribLocation getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0600
	 * @tc.name webgl_test_bindBuffer2
	 * @tc.desc Test bindBuffer.
	 */
	it('webgl_test_bindBuffer2', 0, async function(done) {
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
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0601
	 * @tc.name webgl_test_bindBuffer_12
	 * @tc.desc Test bindBuffer.
	 */
	it('webgl_test_bindBuffer_12', 0, async function(done) {
		//initContext();
		console.info("webgltest into bindBuffer");

		var buffer = gl.createBuffer();

		// bind前isBuffer为false
		const isBufferValue0 = gl.isBuffer(buffer);
		console.info("webgltest isBufferValue0: " + isBufferValue0);
		expect(isBufferValue0).assertEqual(false);

		gl.bindBuffer(gl.ARRAY_BUFFER, 'buffer');

		//bind后isBuffer为true
		const isBufferValue = gl.isBuffer(buffer);
		console.info("isBufferValue: " + isBufferValue);
		expect(isBufferValue).assertEqual(false);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0602
	 * @tc.name webgl_test_bindFramebuffer2
	 * @tc.desc Test bindFramebuffer.
	 */
	it('webgl_test_bindFramebuffer2', 0, async function(done) {
		//initContext();
		console.info("webgltest into bindFramebuffer");

		var framebuffer = gl.createFramebuffer();

		const isFramebuffer1 = gl.isFramebuffer(framebuffer);
		console.info("createFramebuffer --> isFramebuffer1: " + isFramebuffer1);
		expect(isFramebuffer1).assertEqual(false);

		// bind
		gl.bindFramebuffer(gl.FRAMEBUFFER, 'framebuffer');

		const isFrameBuffer2 = gl.isFramebuffer(framebuffer);
		console.info("createFramebuffer --> bindFramebuffer --> isFramebuffer_2: " +
			isFrameBuffer2);
		expect(isFrameBuffer2).assertEqual(false);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0603
	 * @tc.name webgl_test_bindRenderbuffer2
	 * @tc.desc Test bindRenderbuffer.
	 */
	it('webgl_test_bindRenderbuffer2', 0, async function(done) {
		//initContext();
		console.info("webgltest into bindRenderbuffer");

		var renderbuffer = gl.createRenderbuffer();

		//不 bindRenderbuffer, 那么下面会返回 false
		const isRenderBuffer0 = gl.isRenderbuffer(renderbuffer);
		console.info("createRenderbuffer --> isRenderbuffer0: " + isRenderBuffer0);
		expect(isRenderBuffer0).assertEqual(false);

		gl.bindRenderbuffer(gl.RENDERBUFFER, 'renderbuffer');

		// bindRenderbuffer后, isRenderbuffer返回true
		const isRenderBuffer1 = gl.isRenderbuffer(renderbuffer);
		console.info("createRenderbuffer --> bindRenderbuffer --> isRenderbuffer: " +
			isRenderBuffer1);
		expect(isRenderBuffer1).assertEqual(false);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0604
	 * @tc.name webgl_test_bindTexture2
	 * @tc.desc Test bindTexture.
	 */
	it('webgl_test_bindTexture2', 0, async function(done) {
		//initContext();
		console.info("webgltest into bindTexture");

		var texture = gl.createTexture();

		const isTexture0 = gl.isTexture(texture);
		console.info("createTexture --> isTexture: " + isTexture0);
		expect(isTexture0).assertEqual(false);

		gl.bindTexture(gl.TEXTURE_2D, 'texture');

		const isTexture1 = gl.isTexture(texture);
		console.info("createTexture --> bindTexture --> isTexture: " + isTexture1);
		expect(isTexture1).assertEqual(false);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0605
	 * @tc.name webgl_test_blendColor2
	 * @tc.desc Test blendColor.
	 */
	it('webgl_test_blendColor2', 0, async function(done) {
		//initContext();
		console.info("webgltest into blendColor");

		gl.blendColor(0, 0.5, -1, 1);
		const blendColorValue = gl.getParameter(gl.BLEND_COLOR);
		console.info("blendColor --> getParameter: " + blendColorValue);

		var blendColorArr = new Float32Array([0, 0.5, -1, 1]);

		// expect(blendColorValue).assertEqual(blendColorArr);

		// 判断数据类型是否正确
		expect(blendColorValue.constructor).assertEqual(blendColorArr.constructor);
		// 判断数据值是否正确
		expect(blendColorValue.toString()).assertEqual(blendColorArr.toString());

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0606
	 * @tc.name webgl_test_blendColor_12
	 * @tc.desc Test blendColor.
	 */
	it('webgl_test_blendColor_12', 0, async function(done) {
		//initContext();
		console.info("webgltest into blendColor");

		gl.blendColor(1, 1, 1, -1);
		const blendColorValue = gl.getParameter(gl.BLEND_COLOR);
		console.info("blendColor --> getParameter: " + blendColorValue);

		var blendColorArr = new Float32Array([1, 1, 1, -1]);

		// expect(blendColorValue).assertEqual(blendColorArr);

		// 判断数据类型是否正确
		expect(blendColorValue.constructor).assertEqual(blendColorArr.constructor);
		// 判断数据值是否正确
		expect(blendColorValue.toString()).assertEqual(blendColorArr.toString());

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0607
	 * @tc.name webgl_test_blendEquation2
	 * @tc.desc Test blendEquation.
	 */
	it('webgl_test_blendEquation2', 0, async function(done) {
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
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0608
	 * @tc.name webgl_test_blendEquation_12
	 * @tc.desc Test blendEquation.
	 */
	it('webgl_test_blendEquation_12', 0, async function(done) {
		//initContext();
		console.info("webgltest into blendEquation");

		gl.blendEquation(gl.FUNC_ADD);

		const blendEquationValue2 = gl.getParameter(gl.BLEND_EQUATION_ALPHA);
		console.info("blendEquation --> getParameter: " + blendEquationValue2);
		expect(blendEquationValue2).assertEqual(gl.FUNC_ADD);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0609
	 * @tc.name webgl_test_blendEquationSeparate2
	 * @tc.desc Test blendEquationSeparate.
	 */
	it('webgl_test_blendEquationSeparate2', 0, async function(done) {
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
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0610
	 * @tc.name webgl_test_blendFunc2
	 * @tc.desc Test blendFunc.
	 */
	it('webgl_test_blendFunc2', 0, async function(done) {
		//initContext();
		console.info("webgltest into blendFunc");

		gl.enable(gl.BLEND);

		gl.blendFunc(gl.SRC_COLOR);

		const blendFuncValue = gl.getParameter(gl.BLEND_SRC_RGB);
		console.info("blendFunc --> getParameter: " + blendFuncValue);
		expect(blendFuncValue).assertEqual(gl.SRC_COLOR);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0611
	 * @tc.name webgl_test_blendFuncSeparate2
	 * @tc.desc Test blendFuncSeparate.
	 */
	it('webgl_test_blendFuncSeparate2', 0, async function(done) {
		//initContext();
		console.info("webgltest into blendFuncSeparate");

		gl.enable(gl.BLEND);

		gl.blendFuncSeparate(gl.SRC_COLOR, gl.DST_COLOR, gl.ONE);

		const blendFuncSeparateParameter = gl.getParameter(gl.BLEND_SRC_RGB)
		console.info("blendFuncSeparate --> getParameter: " + blendFuncSeparateParameter);
		expect(blendFuncSeparateParameter).assertEqual(gl.SRC_COLOR);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0612
	 * @tc.name webgl_test_checkFramebufferStatus22
	 * @tc.desc Test checkFramebufferStatus.
	 */
	it('webgl_test_checkFramebufferStatus22', 0, async function(done) {
		//initContext();
		console.info("webgltest into checkFramebufferStatus");

		//        var framebuffer = gl.createFramebuffer();
		gl.bindFramebuffer(gl.FRAMEBUFFER, 'framebuffer');

		const checkFramebufferStatus = gl.checkFramebufferStatus(gl.FRAMEBUFFER);
		console.info("createFramebuffer --> bindFramebuffer --> checkFramebufferStatus: " +
			checkFramebufferStatus);
		expect(checkFramebufferStatus).assertEqual(gl.FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0613
	 * @tc.name webgl_test_clear2
	 * @tc.desc Test clear.
	 */
	it('webgl_test_clear2', 0, async function(done) {
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
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0614
	 * @tc.name webgl_test_clearColor2
	 * @tc.desc Test clearColor.
	 */
	it('webgl_test_clearColor2', 0, async function(done) {
		//initContext();
		console.info("webgltest into clearColor");
		gl.clearColor(1, 0.5, 0.5, -3);

		const clearColorParameter = gl.getParameter(gl.COLOR_CLEAR_VALUE);

		var float32ArrayValue = new Float32Array([1, 0.5, 0.5, -3]);

		console.info("webgltest clearColor: " + clearColorParameter);
		expect(clearColorParameter.toString()).assertEqual(float32ArrayValue.toString());
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0615
	 * @tc.name webgl_test_clearDepth2
	 * @tc.desc Test clearDepth.
	 */
	it('webgl_test_clearDepth2', 0, async function(done) {
		//initContext();
		console.info("webgltest into clearDepth");

		//用于设置深度缓冲区的深度清除值。
		gl.clearDepth(-0.5);
		//若要获取当前深度清除值，查询DEPTH_CLEAR_VALUE 常量。
		const clearDepthValue = gl.getParameter(gl.DEPTH_CLEAR_VALUE);
		// 0.5
		console.info("webgltest clearDepth --> getParameter: " + clearDepthValue);

		expect(clearDepthValue).assertEqual(0);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0616
	 * @tc.name webgl_test_clearDepth_22
	 * @tc.desc Test clearDepth.
	 */
	it('webgl_test_clearDepth_22', 0, async function(done) {
		//initContext();
		console.info("webgltest into clearDepth");

		//用于设置深度缓冲区的深度清除值。
		gl.clearDepth(-8);
		//若要获取当前深度清除值，查询DEPTH_CLEAR_VALUE 常量。
		const clearDepthValue = gl.getParameter(gl.DEPTH_CLEAR_VALUE);
		// 0.5
		console.info("webgltest clearDepth --> getParameter: " + clearDepthValue);

		expect(clearDepthValue).assertEqual(0);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0617
	 * @tc.name webgl_test_clearStencil2
	 * @tc.desc Test clearStencil.
	 */
	it('webgl_test_clearStencil2', 0, async function(done) {
		//initContext();
		console.info("webgltest into clearStencil");

		gl.clearStencil(-1);
		//要获取当前模板清除值，请查询STENCIL_CLEAR_VALUE 常量。
		const clearStencilValue = gl.getParameter(gl.STENCIL_CLEAR_VALUE);
		console.info("webgltest clearStencil --> getParameter: " + clearStencilValue);
		expect(clearStencilValue).assertEqual(-1);

		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0618
	 * @tc.name webgl_test_clearStencil_12
	 * @tc.desc Test clearStencil.
	 */
	it('webgl_test_clearStencil_12', 0, async function(done) {
		//initContext();
		console.info("webgltest into clearStencil");

		gl.clearStencil(-20);
		//要获取当前模板清除值，请查询STENCIL_CLEAR_VALUE 常量。
		const clearStencilValue = gl.getParameter(gl.STENCIL_CLEAR_VALUE);
		console.info("webgltest clearStencil --> getParameter: " + clearStencilValue);
		expect(clearStencilValue).assertEqual(-20);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0619
	 * @tc.name webgl_test_colorMask22
	 * @tc.desc Test colorMask.
	 */
	it('webgl_test_colorMask22', 0, async function(done) {
		//initContext();
		console.info("webgltest into colorMask");

		gl.colorMask(true, true, 2, false);
		// 要获取当前的颜色掩码，请查询COLOR_WRITEMASK返回Array.
		const colorMaskValue = gl.getParameter(gl.COLOR_WRITEMASK);
		// [true, true, true, false]
		console.info("webgltest colorMask --> getParameter: " + colorMaskValue);

		expect(colorMaskValue.toString()).assertEqual('true,true,true,false');
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0620
	 * @tc.name webgl_test_colorMask_12
	 * @tc.desc Test colorMask.
	 */
	it('webgl_test_colorMask_12', 0, async function(done) {
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
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0621
	 * @tc.name webgl_test_compileShader2
	 * @tc.desc Test compileShader.
	 */
	it('webgl_test_compileShader2', 0, async function(done) {
		//initContext();
		console.info("webgltest into compileShader");

		var shader = gl.createShader(gl.VERTEX_SHADER);
		gl.shaderSource(shader, 1);
		gl.compileShader(shader);

		let errorCode = gl.getError();
		console.info("webgltest compileShader getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_FRAMEBUFFER_OPERATION);

		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0622
	 * @tc.name webgl_test_createProgram2
	 * @tc.desc Test createProgram.
	 */
	it('webgl_test_createProgram2', 0, async function(done) {
		//initContext();
		console.info("webgltest into createProgram");
		const program = gl.createProgram("error");
		const isProgram = gl.isProgram(program);
		console.info("webgltest createProgram isProgram: " + isProgram);
		expect(isProgram).assertEqual(true);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0623
	 * @tc.name webgl_test_createShader2
	 * @tc.desc Test createShader.
	 */
	it('webgl_test_createShader2', 0, async function(done) {
		//initContext();
		console.info("webgltest into createShader");

		const shader = gl.createShader('VERTEX_SHADER');
		const isShader = gl.isShader(shader);
		console.info("webgltest createShader isShader: " + isShader);
		expect(isShader).assertEqual(false);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0624
	 * @tc.name webgl_test_deleteFramebuffer2
	 * @tc.desc Test deleteFramebuffer.
	 */
	it('webgl_test_deleteFramebuffer2', 0, async function(done) {
		//initContext();
		console.info("webgltest into deleteFramebuffer");

		var framebuffer = gl.createFramebuffer();
		gl.bindFramebuffer(gl.FRAMEBUFFER, framebuffer);

		const isFramebuffer1 = gl.isFramebuffer(framebuffer);
		console.info("webgltest createFramebuffer bindFramebuffer isFramebuffer1: " +
			isFramebuffer1);
		expect(isFramebuffer1).assertEqual(true);

		gl.deleteFramebuffer();

		const isFramebuffer2 = gl.isFramebuffer(framebuffer);
		console.info("webgltest createFramebuffer bindFramebuffer isFramebuffer2: " +
			isFramebuffer2);
		expect(isFramebuffer2).assertEqual(true);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0625
	 * @tc.name webgl_test_deleteRenderbuffer2
	 * @tc.desc Test deleteRenderbuffer.
	 */
	it('webgl_test_deleteRenderbuffer2', 0, async function(done) {
		//initContext();
		console.info("webgltest into deleteRenderbuffer");

		const renderbuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderbuffer);
		const isRenderbuffer1 = gl.isRenderbuffer(renderbuffer);
		console.info("webgltest createRenderbuffer bindRenderbuffer isRenderbuffer1: " +
			isRenderbuffer1);
		expect(isRenderbuffer1).assertEqual(true);

		gl.deleteRenderbuffer();

		const isRenderbuffer2 = gl.isRenderbuffer(renderbuffer);
		console.info(
			"webgltest createRenderbuffer bindRenderbuffer  deleteRenderbuffer isRenderbuffer2: " +
			isRenderbuffer2);
		expect(isRenderbuffer2).assertEqual(true);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0626
	 * @tc.name webgl_test_detachShader2
	 * @tc.desc Test detachShader.
	 */
	it('webgl_test_detachShader2', 0, async function(done) {
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
		gl.detachShader(program, "vertexShader"); //从一个WebGLProgram中分离一个先前附加的片段或者顶点着色器;
		gl.detachShader(program, "fragmentShader");
		gl.deleteShader(vertexShader);
		gl.deleteShader(fragmentShader);

		let errorCode = gl.getError();
		console.info("webgltest uniform3uiv getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0627
	 * @tc.name webgl_test_disableVertexAttribArray2
	 * @tc.desc Test disableVertexAttribArray.
	 */
	it('webgl_test_disableVertexAttribArray2', 0, async function(done) {
		//initContext();
		console.info("webgltest into disableVertexAttribArray");
		gl.disableVertexAttribArray("error");
		let errorCode = gl.getError();
		console.info("webgltest disableVertexAttribArray getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0628
	 * @tc.name webgl_test_enableVertexAttribArray2
	 * @tc.desc Test enableVertexAttribArray.
	 */
	it('webgl_test_enableVertexAttribArray2', 0, async function(done) {
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
		expect(errorCode).assertEqual(gl.INVALID_FRAMEBUFFER_OPERATION);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0629
	 * @tc.name webgl_test_finish2
	 * @tc.desc Test finish.
	 */
	it('webgl_test_finish2', 0, async function(done) {
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
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0630
	 * @tc.name webgl_test_flush2
	 * @tc.desc Test flush.
	 */
	it('webgl_test_flush2', 0, async function(done) {
		//initContext();
		console.info("webgltest into flush");
		gl.flush("error");
		let errorCode = gl.getError();
		console.info("webgltest flush getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});



	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0631
	 * @tc.name webgl_test_createQuery2
	 * @tc.desc Test createQuery.
	 */
	it('webgl_test_createQuery2', 0, async function(done) {
		//initContext();
		console.info("webgltest into createQuery");

		const query = gl2.createQuery('error');
		gl2.beginQuery(0x8C2F, query);

		const currentQuery = gl2.getQuery(0x8C2F, 0x8865);
		const isQuery = gl2.isQuery(currentQuery);

		console.info("webgltest createQuery isQuery: " + isQuery);
		expect(isQuery).assertEqual(false);
		//deleteContext();
		done();
	});



	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0632
	 * @tc.name webgl_test_beginQuery2
	 * @tc.desc Test beginQuery.
	 */
	it('webgl_test_beginQuery2', 0, async function(done) {
		//initContext();
		console.info("webgltest into beginQuery");

		var query = gl2.createQuery();
		gl2.beginQuery(0x8C2F, 'query');

		const currentQuery = gl2.getQuery(0x8C2F, 0x8865);
		const isQuery1 = gl2.isQuery(currentQuery);
		console.info("webgltest createQuery isQuery1: " + isQuery1);
		expect(isQuery1).assertEqual(false);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0633
	 * @tc.name webgl_test_deleteQuery2
	 * @tc.desc Test deleteQuery.
	 */
	it('webgl_test_deleteQuery2', 0, async function(done) {
		//initContext();
		console.info("webgltest into deleteQuery");

		var query = gl2.createQuery();
		gl2.beginQuery(0x8C2F, query);

		const currentQuery = gl2.getQuery(0x8C2F, 0x8865);
		const isQuery1 = gl2.isQuery(currentQuery);
		console.info("webgltest createQuery isQuery1: " + isQuery1);
		expect(isQuery1).assertEqual(false);

		gl2.deleteQuery('currentQuery');

		const isQuery2 = gl2.isQuery(currentQuery);
		console.info("webgltest createQuery deleteQuery isQuery2: " + isQuery2);

		expect(isQuery2).assertEqual(false);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0634
	 * @tc.name webgl_test_beginQuery22
	 * @tc.desc Test beginQuery.
	 */
	it('webgl_test_beginQuery22', 0, async function(done) {
		//initContext();
		console.info("webgltest into beginQuery");

		var query = gl2.createQuery();
		gl2.beginQuery(0x8C2F, 'query');

		const currentQuery = gl2.getQuery(0x8C2F, 0x8865);
		const isQuery1 = gl2.isQuery(currentQuery);
		console.info("webgltest createQuery isQuery1: " + isQuery1);
		expect(isQuery1).assertEqual(false);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0635
	 * @tc.name webgl_test_endQuery2
	 * @tc.desc Test endQuery.
	 */
	it('webgl_test_endQuery2', 0, async function(done) {
		//initContext();
		console.info("webgltest into endQuery");

		var query = gl2.createQuery();
		gl2.beginQuery(0x8C2F, query);

		const currentQuery = gl2.getQuery(0x8C2F, 0x8865);
		const isQuery1 = gl2.isQuery(currentQuery);
		console.info("webgltest createQuery isQuery1: " + isQuery1);
		expect(isQuery1).assertEqual(false);

		gl2.endQuery('0x8C2F');

		const isQuery2 = gl2.isQuery(currentQuery);
		console.info("webgltest createQuery isQuery2: " + isQuery2);
		expect(isQuery2).assertEqual(false);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0636
	 * @tc.name webgl_test_getQueryParameter22
	 * @tc.desc Test getQueryParameter.
	 */
	it('webgl_test_getQueryParameter22', 0, async function(done) {
		//initContext();
		console.info("webgltest into getQueryParameter");
		var query = gl2.createQuery();
		gl2.beginQuery(0x8C2F, query);

		var currentQuery = gl2.getQuery(0x8C2F, 0x8865);
		const isQuery1 = gl2.isQuery(currentQuery);
		console.info("webgltest createQuery isQuery1: " + isQuery1);
		expect(isQuery1).assertEqual(false);

		// 返回一个GLboolean 指示查询结果是否可用。
		var result = gl2.getQueryParameter(currentQuery, '0x8867');

		console.info("webgltest createQuery beginQuery getQueryParameter: " + result);
		expect(result).assertEqual(undefined);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0637
	 * @tc.name webgl_test_createSampler2
	 * @tc.desc Test createSampler.
	 */
	it('webgl_test_createSampler2', 0, async function(done) {
		//initContext();
		console.info("webgltest into createSampler");
		const sampler = gl2.createSampler('error');
		const isSampler1 = gl2.isSampler(sampler);
		console.info("webgltest createSampler isSampler1: " + isSampler1);
		expect(isSampler1).assertEqual(true);

		gl2.bindSampler(0, sampler);
		const isSampler2 = gl2.isSampler(sampler);
		console.info("webgltest createSampler bindSampler isSampler2: " + isSampler2);
		expect(isSampler2).assertEqual(true);

		gl2.deleteSampler(sampler);
		const isSampler3 = gl2.isSampler(sampler);
		console.info("webgltest createSampler bindSampler deleteSampler isSampler3: " + isSampler3);
		expect(isSampler3).assertEqual(false);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0638
	 * @tc.name webgl_test_deleteSampler2
	 * @tc.desc Test deleteSampler.
	 */
	it('webgl_test_deleteSampler2', 0, async function(done) {
		//initContext();
		console.info("webgltest into deleteSampler");
		const sampler = gl2.createSampler();
		const isSampler1 = gl2.isSampler(sampler);
		console.info("webgltest createSampler isSampler1: " + isSampler1);
		expect(isSampler1).assertEqual(true);

		gl2.bindSampler(0, sampler);
		const isSampler2 = gl2.isSampler(sampler);
		console.info("webgltest createSampler bindSampler isSampler2: " + isSampler2);
		expect(isSampler2).assertEqual(true);

		gl2.deleteSampler('sampler');
		const isSampler3 = gl2.isSampler(sampler);
		console.info("webgltest createSampler bindSampler deleteSampler isSampler3: " + isSampler3);
		expect(isSampler3).assertEqual(true);
		//deleteContext();
		done();
	});



	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0639
	 * @tc.name webgl_test_bindSampler2
	 * @tc.desc Test bindSampler.
	 */
	it('webgl_test_bindSampler2', 0, async function(done) {
		//initContext();
		console.info("webgltest into bindSampler");
		const sampler = gl2.createSampler();
		const isSampler1 = gl2.isSampler(sampler);
		console.info("webgltest createSampler isSampler1: " + isSampler1);
		expect(isSampler1).assertEqual(true);

		gl2.bindSampler(0, 'sampler');
		const isSampler2 = gl2.isSampler(sampler);
		console.info("webgltest createSampler bindSampler isSampler2: " + isSampler2);
		expect(isSampler2).assertEqual(true);

		gl2.deleteSampler(sampler);
		const isSampler3 = gl2.isSampler(sampler);
		console.info("webgltest createSampler bindSampler deleteSampler isSampler3: " + isSampler3);
		expect(isSampler3).assertEqual(false);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0640
	 * @tc.name webgl_test_samplerParameteri2
	 * @tc.desc Test samplerParameteri.
	 */
	it('webgl_test_samplerParameteri2', 0, async function(done) {
		//initContext();
		console.info("webgltest into samplerParameteri");
		let sampler = gl2.createSampler();
		gl2.samplerParameteri(sampler, gl.TEXTURE_MAG_FILTER);

		let errorCode = gl.getError();
		console.info("webgltest samplerParameteri getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		gl2.deleteSampler(sampler);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0641
	 * @tc.name webgl_test_samplerParameterf2
	 * @tc.desc Test samplerParameterf.
	 */
	it('webgl_test_samplerParameterf2', 0, async function(done) {
		//initContext();
		console.info("webgltest into samplerParameterf");

		let sampler = gl2.createSampler();
		//        gl2.bindSampler(0, sampler);
		gl2.samplerParameterf(sampler, gl.TEXTURE_MAG_FILTER);

		let errorCode = gl.getError();
		console.info("webgltest samplerParameterf getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		gl2.deleteSampler(sampler);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0642
	 * @tc.name webgl_test_getSamplerParameter_12
	 * @tc.desc Test getSamplerParameter.
	 */
	it('webgl_test_getSamplerParameter_12', 0, async function(done) {
		//initContext();
		console.info("webgltest into getSamplerParameter");

		let sampler = gl2.createSampler();

		const samplerParameter = gl2.getSamplerParameter(sampler, '0x884D');

		console.info("webgltest createSampler getSamplerParameter_1: " + samplerParameter);
		expect(samplerParameter).assertEqual(undefined);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0643
	 * @tc.name webgl_test_fenceSync2
	 * @tc.desc Test fenceSync.
	 */
	it('webgl_test_fenceSync2', 0, async function(done) {
		//initContext();
		console.info("webgltest into fenceSync");

		let sync = gl2.fenceSync('0x9117', 0);
		let status = gl2.clientWaitSync(sync, 0, 0);
		gl2.waitSync(sync, 0, -1);
		const isSync1 = gl2.isSync(sync);
		console.info("webgltest fenceSync isSync1: " + isSync1);
		expect(isSync1).assertEqual(false);

		gl2.deleteSync(sync);

		const isSync2 = gl2.isSync(sync);
		console.info("webgltest fenceSync deleteSync isSync2: " + isSync2);
		expect(isSync2).assertEqual(false);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0644
	 * @tc.name webgl_test_isSync2
	 * @tc.desc Test isSync.
	 */
	it('webgl_test_isSync2', 0, async function(done) {
		//initContext();
		console.info("webgltest into isSync");
		let sync = gl2.fenceSync(0x9117, 0);
		let status = gl2.clientWaitSync(sync, 0, 0);
		gl2.waitSync(sync, 0, -1);
		const isSync1 = gl2.isSync(sync);
		console.info("webgltest fenceSync isSync1: " + isSync1);
		expect(isSync1).assertEqual(true);

		gl2.deleteSync(sync);

		const isSync2 = gl2.isSync('sync');
		console.info("webgltest fenceSync deleteSync isSync2: " + isSync2);
		expect(isSync2).assertEqual(false);
		//deleteContext();
		done();
	});



	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0645
	 * @tc.name webgl_test_deleteSync2
	 * @tc.desc Test deleteSync.
	 */
	it('webgl_test_deleteSync2', 0, async function(done) {
		//initContext();
		console.info("webgltest into deleteSync");
		let sync = gl2.fenceSync(0x9117, 0);
		let status = gl2.clientWaitSync(sync, 0, 0);
		gl2.waitSync(sync, 0, -1);
		const isSync1 = gl2.isSync(sync);
		console.info("webgltest fenceSync isSync1: " + isSync1);
		expect(isSync1).assertEqual(true);

		gl2.deleteSync('sync');

		const isSync2 = gl2.isSync(sync);
		console.info("webgltest fenceSync deleteSync isSync2: " + isSync2);
		expect(isSync2).assertEqual(true);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0646
	 * @tc.name webgl_test_clientWaitSync2
	 * @tc.desc Test clientWaitSync.
	 */
	it('webgl_test_clientWaitSync2', 0, async function(done) {
		//initContext();
		console.info("webgltest into clientWaitSync");
		let sync = gl2.fenceSync(0x9117, 0);
		let status = gl2.clientWaitSync('sync', 0, 0);
		gl2.waitSync(sync, 0, -1);
		const isSync1 = gl2.isSync(sync);
		console.info("webgltest fenceSync isSync1: " + isSync1);
		expect(isSync1).assertEqual(true);

		console.info("webgltest fenceSync clientWaitSync: " + status);

		expect(status).assertEqual(0);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0647
	 * @tc.name webgl_test_waitSync2
	 * @tc.desc Test waitSync.
	 */
	it('webgl_test_waitSync2', 0, async function(done) {
		//initContext();
		console.info("webgltest into waitSync");

		let sync = gl2.fenceSync(gl.SYNC_GPU_COMMANDS_COMPLETE, 0);
		gl2.waitSync('sync', 0, gl.TIMEOUT_IGNORED);

		let errorCode = gl.getError();
		console.info("webgltest framebufferTexture2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0648
	 * @tc.name webgl_test_createTransformFeedback2
	 * @tc.desc Test createTransformFeedback.
	 */
	it('webgl_test_createTransformFeedback2', 0, async function(done) {
		//initContext();
		console.info("webgltest into createTransformFeedback");

		const transformFeedback = gl2.createTransformFeedback();


		console.info("webgltest into createTransformFeedback: " + transformFeedback)

		gl2.bindTransformFeedback(gl2.TRANSFORM_FEEDBACK, transformFeedback);

		gl2.beginTransformFeedback(gl2.TRIANGLES);

		const isTransformFeedback2 = gl2.isTransformFeedback(transformFeedback);

		console.info("BindTransformFeedback  : " + isTransformFeedback2);
		expect(isTransformFeedback2).assertEqual(true);

		gl2.deleteTransformFeedback(transformFeedback);
		const isTransformFeedback3 = gl2.isTransformFeedback(transformFeedback);
		console.info("webgltest createTransformFeedback bindTransformFeedback " +
			"deleteTransformFeedback isTransformFeedback3: " + isTransformFeedback3);
		expect(isTransformFeedback3).assertEqual(false);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0649
	 * @tc.name webgl_test_isTransformFeedback2
	 * @tc.desc Test isTransformFeedback.
	 */
	it('webgl_test_isTransformFeedback2', 0, async function(done) {
		//initContext();
		console.info("webgltest into isTransformFeedback");
		let transformFeedback = gl2.createTransformFeedback();
		const isTransformFeedback1 = gl2.isTransformFeedback(transformFeedback);
		console.info("webgltest createTransformFeedback isTransformFeedback1: " +
			isTransformFeedback1);
		expect(isTransformFeedback1).assertEqual(false);

		gl2.bindTransformFeedback(gl2.TRANSFORM_FEEDBACK, transformFeedback);

		const isTransformFeedback2 = gl2.isTransformFeedback(transformFeedback);
		console.info(
			"webgltest createTransformFeedback bindTransformFeedback isTransformFeedback2: " +
			isTransformFeedback2);
		expect(isTransformFeedback2).assertEqual(true);

		gl2.deleteTransformFeedback(transformFeedback);
		const isTransformFeedback3 = gl2.isTransformFeedback(transformFeedback);
		console.info("webgltest createTransformFeedback bindTransformFeedback " +
			"deleteTransformFeedback isTransformFeedback3: " + isTransformFeedback3);
		expect(isTransformFeedback3).assertEqual(false);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0650
	 * @tc.name webgl_test_bindTransformFeedback2
	 * @tc.desc Test bindTransformFeedback.
	 */
	it('webgl_test_bindTransformFeedback2', 0, async function(done) {
		//initContext();
		console.info("webgltest into bindTransformFeedback");
		let transformFeedback = gl2.createTransformFeedback();
		const isTransformFeedback1 = gl2.isTransformFeedback(transformFeedback);
		console.info("webgltest createTransformFeedback isTransformFeedback1: " +
			isTransformFeedback1);
		expect(isTransformFeedback1).assertEqual(false);

		gl2.bindTransformFeedback(gl2.TRANSFORM_FEEDBACK, transformFeedback);

		const isTransformFeedback2 = gl2.isTransformFeedback(transformFeedback);
		console.info("webgltest createTransformFeedback bindTransformFeedback " +
			"isTransformFeedback2: " + isTransformFeedback2);
		expect(isTransformFeedback2).assertEqual(true);

		gl2.deleteTransformFeedback(transformFeedback);
		const isTransformFeedback3 = gl2.isTransformFeedback(transformFeedback);
		console.info("webgltest createTransformFeedback bindTransformFeedback " +
			"deleteTransformFeedback isTransformFeedback3: " + isTransformFeedback3);
		expect(isTransformFeedback3).assertEqual(false);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0651
	 * @tc.name webgl_test_beginTransformFeedback2
	 * @tc.desc Test beginTransformFeedback.
	 */
	it('webgl_test_beginTransformFeedback2', 0, async function(done) {
		//initContext();
		console.info("webgltest into beginTransformFeedback");

		let transformFeedback = gl2.createTransformFeedback();

		gl2.beginTransformFeedback('TRIANGLES');

		let errorCode = gl.getError();
		console.info("webgltest framebufferTexture2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0652
	 * @tc.name webgl_test_endTransformFeedback2
	 * @tc.desc Test endTransformFeedback.
	 */
	it('webgl_test_endTransformFeedback2', 0, async function(done) {
		//initContext();
		console.info("webgltest into endTransformFeedback");
		let transformFeedback = gl2.createTransformFeedback();
		gl2.bindTransformFeedback(gl2.TRANSFORM_FEEDBACK, transformFeedback);
		gl2.beginTransformFeedback(gl.TRIANGLES);
		gl.drawArrays(gl.TRIANGLES, 0, 3);
		gl2.endTransformFeedback('error');
		let errorCode = gl.getError();
		console.info("webgltest framebufferTexture2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_FRAMEBUFFER_OPERATION);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0653
	 * @tc.name webgl_test_deleteTransformFeedback2
	 * @tc.desc Test deleteTransformFeedback.
	 */
	it('webgl_test_deleteTransformFeedback2', 0, async function(done) {
		//initContext();
		console.info("webgltest into deleteTransformFeedback");
		const transformFeedback = gl2.createTransformFeedback();

		gl2.bindTransformFeedback(gl2.TRANSFORM_FEEDBACK, transformFeedback);

		const isTransformFeedback2 = gl2.isTransformFeedback(transformFeedback);
		console.info("webgltest createTransformFeedback bindTransformFeedback " +
			"isTransformFeedback2: " + isTransformFeedback2);
		expect(isTransformFeedback2).assertEqual(true);

		gl2.deleteTransformFeedback('transformFeedback');
		const isTransformFeedback3 = gl2.isTransformFeedback(transformFeedback);
		console.info(
			"webgltest createTransformFeedback bindTransformFeedback deleteTransformFeedback " +
			"isTransformFeedback3: " + isTransformFeedback3);
		expect(isTransformFeedback3).assertEqual(true);
		//deleteContext();
		done();
	});




	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0654
	 * @tc.name testUseProgramError
	 * @tc.desc Test useProgram.
	 */
	it('testUseProgramError', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testUseProgram test start ...66');
			console.info('jsWebGL testUseProgram test start ...' + JSON.stringify(gl));
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
			const program = gl.createProgram();
			console.log("testUseProgram has failed for " + program)
			const useProgramError1 = gl.getError();
			console.info("useProgramError: " + useProgramError1);
			const renderBufferValue1 = gl.getParameter(gl.CURRENT_PROGRAM);
			console.log("testUseProgram has failed for " + program)
			gl.attachShader(program, vertexShader);
			gl.attachShader(program, fragmentShader);
			gl.linkProgram(program);
			gl.useProgram(false);
			const useProgramError = gl.getError();
			console.info("useProgramError: " + useProgramError);
			expect(useProgramError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testUseProgram has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0655
	 * @tc.name testValidateProgramError
	 * @tc.desc Test validateProgram.
	 */
	it('testValidateProgramError', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testValidateProgram test start ...66');
			console.info('jsWebGL testValidateProgram test start ...' + JSON.stringify(gl));
			const program = gl.createProgram();
			gl.validateProgram(false);
			const validateProgramError = gl.getError();
			console.info("validateProgramError: " + validateProgramError);
			expect(validateProgramError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testValidateProgram has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0656
	 * @tc.name testVertexAttrib1fError
	 * @tc.desc Test vertexAttrib1f.
	 */
	it('testVertexAttrib1fError', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttrib1f test start ...66');
			console.info('jsWebGL testVertexAttrib1f test start ...' + JSON.stringify(gl));
			gl.vertexAttrib1f(false, 10.0);
			const vertexAttrib1fError = gl.getError();
			console.info("vertexAttrib1fError: " + vertexAttrib1fError);
			expect(vertexAttrib1fError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testVertexAttrib1f has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0657
	 * @tc.name testVertexAttrib2fError
	 * @tc.desc Test vertexAttrib2f.
	 */
	it('testVertexAttrib2fError', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttrib2f test start ...66');
			gl.vertexAttrib2f(false, 10.0);
			const vertexAttrib2fError = gl.getError();
			console.info("vertexAttrib2fError: " + vertexAttrib2fError);
			expect(vertexAttrib2fError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testVertexAttrib2f has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0658
	 * @tc.name testVertexAttrib3fError
	 * @tc.desc Test vertexAttrib3f.
	 */
	it('testVertexAttrib3fError', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testvertexAttrib3f test start ...66');
			console.info('jsWebGL testvertexAttrib3f test start ...' + JSON.stringify(gl));
			gl.vertexAttrib3f(false, 10.0);
			const vertexAttrib3fError = gl.getError();
			console.info("vertexAttrib3fError: " + vertexAttrib3fError);
			expect(vertexAttrib3fError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testVertexAttrib3f has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0659
	 * @tc.name testVertexAttrib4fError
	 * @tc.desc Test vertexAttrib4f.
	 */
	it('testVertexAttrib4fError', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttrib4f test start ...66');
			console.info('jsWebGL testVertexAttrib4f test start ...' + JSON.stringify(gl));
			gl.vertexAttrib4f(false, 10.0);
			const vertexAttrib4fError = gl.getError();
			console.info("vertexAttrib4fError: " + vertexAttrib4fError);
			expect(vertexAttrib4fError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testvertexAttrib4f has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0660
	 * @tc.name testVertexAttrib1fvError
	 * @tc.desc Test vertexAttrib1fv.
	 */
	it('testVertexAttrib1fvError', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttrib1fv test start ...66');
			console.info('jsWebGL testVertexAttrib1fv test start ...' + JSON.stringify(gl));
			const floatArray = new Float32Array([10.0, 5.0, 2.0]);
			gl.vertexAttrib1fv(false, floatArray);
			const vertexAttrib1fvError = gl.getError();
			console.info("vertexAttrib1fvError: " + vertexAttrib1fvError);
			expect(vertexAttrib1fvError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testVertexAttrib1fv has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0661
	 * @tc.name testVertexAttrib2fvError
	 * @tc.desc Test vertexAttrib2fv.
	 */
	it('testVertexAttrib2fvError', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttrib2fv test start ...66');
			console.info('jsWebGL testVertexAttrib2fv test start ...' + JSON.stringify(gl));
			const floatArray = new Float32Array([10.0, 5.0, 2.0]);
			gl.vertexAttrib2fv(false, floatArray);
			const vertexAttrib2fvError = gl.getError();
			console.info("vertexAttrib2fvError: " + vertexAttrib2fvError);
			expect(vertexAttrib2fvError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testVertexAttrib2fv has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0662
	 * @tc.name testVertexAttrib3fvError
	 * @tc.desc Test vertexAttrib3fv.
	 */
	it('testVertexAttrib3fvError', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttrib3fv test start ...66');
			console.info('jsWebGL testVertexAttrib3fv test start ...' + JSON.stringify(gl));
			const floatArray = new Float32Array([10.0, 5.0, 2.0]);
			gl.vertexAttrib3fv(false, floatArray);
			const vertexAttrib3fvError = gl.getError();
			console.info("vertexAttrib3fvError: " + vertexAttrib3fvError);
			expect(vertexAttrib3fvError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testVertexAttrib3fv has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0663
	 * @tc.name testVertexAttrib4fvError
	 * @tc.desc Test vertexAttrib4fv.
	 */
	it('testVertexAttrib4fvError', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttrib4fv test start ...66');
			console.info('jsWebGL testVertexAttrib4fv test start ...' + JSON.stringify(gl));
			const floatArray = new Float32Array([10.0, 5.0, 2.0]);
			gl.vertexAttrib4fv(false, floatArray);
			const vertexAttrib4fvError = gl.getError();
			console.info("vertexAttrib4fvError: " + vertexAttrib4fvError);
			expect(vertexAttrib4fvError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testVertexAttrib4fv has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0664
	 * @tc.name testVertexAttribPointerError
	 * @tc.desc Test vertexAttribPointer.
	 */
	it('testVertexAttribPointerError', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttribPointer test start ...66');
			console.info('jsWebGL testVertexAttribPointer test start ...' + JSON.stringify(gl));
			gl.vertexAttribPointer(0, 3, gl.FLOAT, false, 20, 0);
			const vertexAttribPointerError = gl.getError();
			console.info("vertexAttribPointerError: " + vertexAttribPointerError);
			expect(vertexAttribPointerError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testVertexAttribPointer has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0665
	 * @tc.name testViewportError
	 * @tc.desc Test viewport.
	 */
	it('testViewportError', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testViewport test start ...66');
			console.info('jsWebGL testViewport test start ...' + JSON.stringify(gl));
			gl.viewport(false, 2, 3, 4);
			const viewportError = gl.getError();
			console.info("viewportError: " + viewportError);
			expect(viewportError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testViewport has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0666
	 * @tc.name testTexStorage3DError
	 * @tc.desc Test texStorage3D.
	 */
	it('testTexStorage3DError', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexStorage3D test start ...66');
			console.info('jsWebGL testTexStorage3D test start ...' + JSON.stringify(gl));
			gl2.texStorage3D(gl.TEXTURE_3D, false, gl.RGB8, 256, 256, 256);
			const texStorage3DError = gl.getError();
			console.info("texStorage3DError: " + texStorage3DError);
			expect(texStorage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexStorage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0667
	 * @tc.name testTexImage3DError
	 * @tc.desc Test texImage3D.
	 */
	it('testTexImage3DError', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexImage3D test start ...66');
			console.info('jsWebGL testTexImage3D test start ...' + JSON.stringify(gl));
			gl2.texImage3D(gl2.TEXTURE_3D, false, gl.RGBA, 1, 1, 1, 0, gl.RGBA, gl.UNSIGNED_BYTE,
				0);
			const texImage3DError = gl.getError();
			console.info("texImage3DError: " + texImage3DError);
			expect(texImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexImage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0668
	 * @tc.name testTexImage3D_01Error
	 * @tc.desc Test texImage3D.
	 */
	it('testTexImage3D_01Error', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexImage3D_01Error test start ...66');
			console.info('jsWebGL testTexImage3D_01Error test start ...' + JSON.stringify(gl));
			const buffer = new ArrayBuffer(8);
			gl2.texImage3D(gl2.TEXTURE_3D, false, gl.RGBA, 1, 1, 1, 0, gl.RGBA, gl.UNSIGNED_BYTE,
				buffer);
			const texImage3DError = gl.getError();
			console.info("texImage3DError: " + texImage3DError);
			expect(texImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexImage3D_01 has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0669
	 * @tc.name testTexImage3D_02Error
	 * @tc.desc Test texImage3D.
	 */
	it('testTexImage3D_02Error', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexImage3D_02Error test start ...66');
			console.info('jsWebGL testTexImage3D_02Error test start ...' + JSON.stringify(gl));
			const buffer = new ArrayBuffer(8);
			gl2.texImage3D(gl2.TEXTURE_3D, false, gl.RGBA, 1, 1, 1, 0, gl.RGBA, gl.UNSIGNED_BYTE,
				buffer, 0);
			const texImage3DError = gl.getError();
			console.info("texImage3DError: " + texImage3DError);
			expect(texImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexImage3D_02 has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0670
	 * @tc.name testTexSubImage3D_01Error
	 * @tc.desc Test texSubImage3D.
	 */
	it('testTexSubImage3D_01Error', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexSubImage3D_01Error test start ...66');
			console.info('jsWebGL testTexSubImage3D_01Error test start ...' + JSON.stringify(gl));
			const buffer = new ArrayBuffer(16);
			const view = new DataView(buffer, 12, 4);
			gl2.texSubImage3D(gl.TEXTURE_3D, false, 0, 0, 0, 200, 200, 1, gl.RGBA, gl.UNSIGNED_BYTE,
				buffer, 0);
			const texSubImage3DError = gl.getError();
			console.info("texSubImage3DError: " + texSubImage3DError);
			expect(texSubImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexSubImage3D_01 has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0671
	 * @tc.name testTexSubImage3DError
	 * @tc.desc Test texSubImage3D.
	 */
	it('testTexSubImage3DError', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexSubImage3DError test start ...66');
			console.info('jsWebGL testTexSubImage3DError test start ...' + JSON.stringify(gl));
			gl2.texSubImage3D(gl.TEXTURE_3D, false, 0, 0, 0, 200, 200, 1, gl.RGBA, gl.UNSIGNED_BYTE,
				1);
			const texSubImage3DError = gl.getError();
			console.info("texSubImage3DError: " + texSubImage3DError);
			expect(texSubImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexSubImage3DError has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0672
	 * @tc.name testCopyTexSubImage3DError
	 * @tc.desc Test copyTexSubImage3D.
	 */
	it('testCopyTexSubImage3DError', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testCopyTexSubImage3DError test start ...66');
			console.info('jsWebGL testCopyTexSubImage3DError test start ...' + JSON.stringify(gl));
			gl2.copyTexSubImage3D(gl.TEXTURE_3D, false, 0, 0, 0, 0, 0, 16, 16);
			const copyTexSubImage3DError = gl.getError();
			console.info("copyTexSubImage3DError: " + copyTexSubImage3DError);
			expect(copyTexSubImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testCopyTexSubImage3DError has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0673
	 * @tc.name testCompressedTexImage3D_01Error
	 * @tc.desc Test compressedTexImage3D.
	 */
	it('testCompressedTexImage3D_01Error', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testCompressedTexImage3D_01Error test start ...66');
			console.info('jsWebGL testCompressedTexImage3D_01Error test start ...' + JSON.stringify(
				gl));
			gl2.compressedTexImage3D(gl.TEXTURE_3D, false, 0x83F0, 512, 512, 512, 0, gl2
				.PIXEL_UNPACK_BUFFER, 0);
			const compressedTexImage3DError = gl.getError();
			console.info("compressedTexImage3DError: " + compressedTexImage3DError);
			expect(compressedTexImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testCompressedTexImage3D_01Error has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0674
	 * @tc.name testCompressedTexImage3D_02Error
	 * @tc.desc Test compressedTexImage3D.
	 */
	it('testCompressedTexImage3D_02Error', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testCompressedTexImage3D_02Error test start ...66');
			console.info('jsWebGL testCompressedTexImage3D_02Error test start ...' + JSON.stringify(
				gl));
			const buffer = new ArrayBuffer(8);
			gl2.compressedTexImage3D(gl.TEXTURE_3D, false, 0x83F0, 512, 512, 512, 0, buffer, 0, 0);
			const compressedTexImage3DError = gl.getError();
			console.info("compressedTexImage3DError: " + compressedTexImage3DError);
			expect(compressedTexImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testCompressedTexImage3D_02Error has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0675
	 * @tc.name testCompressedTexSubImage3DError
	 * @tc.desc Test compressedTexSubImage3D.
	 */
	it('testCompressedTexSubImage3DError', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testCompressedTexSubImage3DError test start ...66');
			console.info('jsWebGL testCompressedTexSubImage3DError test start ...' + JSON.stringify(
				gl));
			gl2.compressedTexSubImage3D(gl.TEXTURE_3D, false, 0, 0, 0, 512, 512, 512, gl2
				.COMPRESSED_R11_EAC,
				gl2.PIXEL_UNPACK_BUFFER, 0);
			const compressedTexSubImage3DError = gl.getError();
			console.info("compressedTexSubImage3DError: " + compressedTexSubImage3DError);
			expect(compressedTexSubImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testCompressedTexSubImage3DError has failed for " + e)
			expect(null).assertFail()
		}
	})


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0676
	 * @tc.name testCompressedTexSubImage3D_01Error
	 * @tc.desc Test compressedTexSubImage3D.
	 */
	it('testCompressedTexSubImage3D_01Error', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testCompressedTexSubImage3D_01Error test start ...66');
			console.info('jsWebGL testCompressedTexSubImage3D_01Error test start ...' + JSON
				.stringify(gl));
			const buffer = new ArrayBuffer(8);
			gl2.compressedTexSubImage3D(gl.TEXTURE_3D, false, 0, 0, 0, 512, 512, 512, gl2
				.COMPRESSED_R11_EAC,
				buffer, 0, 256);
			const compressedTexSubImage3DError = gl.getError();
			console.info("testCompressedTexSubImage3D_01Error: " + compressedTexSubImage3DError);
			expect(compressedTexSubImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testCompressedTexSubImage3D_01Error has failed for " + e)
			expect(null).assertFail()
		}
	})


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0677
	 * @tc.name webgl_test_getFragDataLocationError
	 * @tc.desc Test getFragDataLocation.
	 */
	it('webgl_test_getFragDataLocationError', 0, async function(done) {
		//initContext();
		console.info("webgltest into getFragDataLocation");

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

		gl2.getFragDataLocation(programObj, false);

		let errorCode = gl.getError();
		console.info("webgltest webgl_test_getFragDataLocationError getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0678
	 * @tc.name webgl_test_vertexAttribI4uiError
	 * @tc.desc Test vertexAttribI4ui.
	 */
	it('webgl_test_vertexAttribI4uiError', 0, async function(done) {
		//initContext();
		console.info("webgltest into vertexAttribI4ui");

		const a = gl2.vertexAttribI4ui(false, 10, 11, 12, 13);

		let errorCode = gl.getError();
		console.info("webgltest vertexAttribI4ui getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0679
	 * @tc.name webgl_test_vertexAttribIPointerError
	 * @tc.desc Test vertexAttribIPointer.
	 */
	it('webgl_test_vertexAttribIPointerError', 0, async function(done) {
		//initContext();
		console.info("webgltest into vertexAttribIPointer");

		const a = gl2.vertexAttribIPointer(false, 4, gl.UNSIGNED_BYTE, 20, 16);

		let errorCode = gl.getError();
		console.info("webgltest vertexAttribIPointer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0680
	 * @tc.name webgl_test_drawArraysInstancedError
	 * @tc.desc Test drawArraysInstanced.
	 */
	it('webgl_test_drawArraysInstancedError', 0, async function(done) {
		//initContext();
		console.info("webgltest into drawArraysInstanced");

		const a = gl2.drawArraysInstanced(gl.POINTS, false, 8, 4);

		let errorCode = gl.getError();
		console.info("webgltest drawArraysInstanced getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0681
	 * @tc.name webgl_test_drawElementsInstancedError
	 * @tc.desc Test drawElementsInstanced.
	 */
	it('webgl_test_drawElementsInstancedError', 0, async function(done) {
		//initContext();
		console.info("webgltest into drawElementsInstanced");

		const a = gl2.drawElementsInstanced(gl.POINTS, false, gl.UNSIGNED_SHORT, 0, 4);

		let errorCode = gl.getError();
		console.info("webgltest drawElementsInstanced getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0682
	 * @tc.name webgl_test_drawRangeElementsError
	 * @tc.desc Test drawRangeElements.
	 */
	it('webgl_test_drawRangeElementsError', 0, async function(done) {
		//initContext();
		console.info("webgltest into drawRangeElements");

		const a = gl2.drawRangeElements(gl.POINTS, false, 7, 8, gl.UNSIGNED_BYTE, 0);

		let errorCode = gl.getError();
		console.info("webgltest drawRangeElements getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);

		//deleteContext();
		done();
	});



	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0683
	 * @tc.name testCompressedTexImage2D_0415
	 * @tc.desc Test compressedTexImage2D.
	 */
	it('testCompressedTexImage2D_0415', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCompressedTexImage2D_0415 test start ...66');
		const buf = new ArrayBuffer(8);
		const view = new DataView(buf, 0);
		view[0] = 2;
		var texture = gl.createTexture();
		gl.bindTexture(texture, texture);
		gl.compressedTexImage2D(-texture, -8, -"abc", -8, -512, -8, view);
		const compressedTexImage2DError = gl.getError();
		console.info("compressedTexImage2DError: " + compressedTexImage2DError);
		expect(compressedTexImage2DError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0684
	 * @tc.name testCompressedTexImage2D_0416
	 * @tc.desc Test compressedTexImage2D.
	 */
	it('testCompressedTexImage2D_0416', 0, async function(done) {
		//initContext();
		console.info('jsWebGL GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0415_02 test start ...66');
		const buf = new ArrayBuffer(8);
		const view = new DataView(buf, 0);
		view[0] = 2;
		var texture = gl.createTexture();
		gl.bindTexture(texture, texture);
		gl.compressedTexImage2D(-texture, -8, -texture, -8, -512, -8, view);
		const compressedTexImage2DError = gl.getError();
		console.info("compressedTexImage2DError: " + compressedTexImage2DError);
		expect(compressedTexImage2DError).assertEqual(gl.INVALID_ENUM);
		done();
	})


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0685
	 * @tc.name testCompressedTexSubImage2D_0420
	 * @tc.desc Test compressedTexSubImage2D.
	 */
	it('testCompressedTexSubImage2D_0420', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCompressedTexSubImage2D_0420 test start ...66');
		const buf = new ArrayBuffer(8);
		const view = new DataView(buf, 0);
		view[0] = 2;
		var texture = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, texture);
		gl.compressedTexImage2D(gl.TEXTURE_2D, 0, 0x83F3, 512, 512, 0, -0);
		gl.compressedTexSubImage2D(-texture, -0, -256, -texture, -512, -0x83F3, -gl
			.PIXEL_UNPACK_BUFFER, view);
		const compressedTexSubImage2DError = gl.getError();
		console.info("compressedTexSubImage2DError: " + compressedTexSubImage2DError);
		expect(compressedTexSubImage2DError).assertEqual(gl.INVALID_VALUE);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0686
	 * @tc.name testCompressedTexSubImage2D_0421
	 * @tc.desc Test compressedTexSubImage2D.
	 */
	it('testCompressedTexSubImage2D_0421', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCompressedTexSubImage2D_0421 test start ...66');
		const buf = new ArrayBuffer(8);
		const view = new DataView(buf, 0);
		view[0] = 2;
		var texture = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, texture);
		gl.compressedTexImage2D(gl.TEXTURE_2D, 0, 0x83F3, 512, 512, 0, -0);
		gl.compressedTexSubImage2D(-texture, -8, -256, -256, -512, -0x83F3, -gl.PIXEL_UNPACK_BUFFER,
			view);
		const compressedTexSubImage2DError = gl.getError();
		console.info("compressedTexSubImage2DError: " + compressedTexSubImage2DError);
		expect(compressedTexSubImage2DError).assertEqual(gl.INVALID_VALUE);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_687
	 * @tc.name testReadPixels_422
	 * @tc.desc Test readPixels.
	 */
	it('testReadPixels_422', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testReadPixels_422 test start ...66');
		const buf = new ArrayBuffer(8);
		const view = new DataView(buf, 0);
		view[0] = 2;
		var pixels = new Uint8Array(gl.drawingBufferWidth * gl.drawingBufferHeight * 4);
		gl.readPixels(-0, 0, -1, -1, -1, gl.ONE_MINUS_SRC_ALPHA, view);
		const readPixelsError = gl.getError();
		console.info("readPixelsError: " + readPixelsError);
		expect(readPixelsError).assertEqual(gl.INVALID_VALUE);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0688
	 * @tc.name testReadPixels_423
	 * @tc.desc Test readPixels.
	 */
	it('testReadPixels_423', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testReadPixels_423 test start ...66');
		const buf = new ArrayBuffer(8);
		const view = new DataView(buf, 0);
		view[0] = 2;
		var pixels = new Uint8Array(gl.drawingBufferWidth * gl.drawingBufferHeight * 4);
		gl.readPixels(0, 0, -1, -1, -1, gl.ONE_MINUS_SRC_ALPHA, view);
		const readPixelsError = gl.getError();
		console.info("readPixelsError: " + readPixelsError);
		expect(readPixelsError).assertEqual(gl.INVALID_VALUE);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0689
	 * @tc.name testTexImage2D_0426
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_0426', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D_0426 test start ...66');
		const buf = new ArrayBuffer(8);
		const view = new DataView(buf, 0);
		view[0] = 2;
		gl.texImage2D(-gl.TEXTURE_2D, -0, -gl.RGB8, -512, -512, -0, -gl.RGB8, gl.UNSIGNED_BYTE,
			view);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(gl.INVALID_VALUE);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0690
	 * @tc.name testTexImage2D_0427
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_0427', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D_0427 test start ...66');
		const buf = new ArrayBuffer(8);
		const view = new DataView(buf, 0);
		view[0] = 2;
		gl.texImage2D(-gl.TEXTURE_2D, -0, -gl.RGB8, -512, -512, -0, -gl.RGB8, gl.UNSIGNED_BYTE,
			view);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(gl.INVALID_VALUE);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0694
	 * @tc.name testTexSubImage2D_01
	 * @tc.desc Test texSubImage2D.
	 */
	it('testTexSubImage2D_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexSubImage2D test start ...66');
		console.info('jsWebGL testTexSubImage2D test start ...' + JSON.stringify(gl));
		const buf = new ArrayBuffer(8);
		const view = new DataView(buf, 0);
		view[0] = 2;
		gl.texSubImage2D(gl.TEXTURE_2D, 0, 0, 0, 512, 512, -gl.RED, -gl.FLOAT, view);
		const ttexSubImage2DError = gl.getError();
		console.info("ttexSubImage2DError: " + ttexSubImage2DError);
		expect(ttexSubImage2DError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0695
	 * @tc.name testTexSubImage2D_02
	 * @tc.desc Test texSubImage2D.
	 */
	it('testTexSubImage2D_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexSubImage2D_02 test start ...66');
		const buf = new ArrayBuffer(8);
		const view = new DataView(buf, 0);
		view[0] = 2;
		gl.texSubImage2D(-gl.TEXTURE_2D, -gl.RED, 0, 0, -0, 512, -gl.RED, -gl.FLOAT, view);
		const ttexSubImage2DError = gl.getError();
		console.info("ttexSubImage2DError: " + ttexSubImage2DError);
		expect(ttexSubImage2DError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0699
	 * @tc.name testBlitFramebuffer_01
	 * @tc.desc Test blitFramebuffer.
	 */
	it('testBlitFramebuffer_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBlitFramebuffer_01 test start ...66');
		gl2.blitFramebuffer(-0, -0, -400, 1080, 0, -0, 400, 1080, -gl2.COLOR_BUFFER_BIT, -gl2
			.NEAREST);
		const blitFramebufferError = gl.getError();
		console.info("blitFramebufferError: " + blitFramebufferError);
		expect(blitFramebufferError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0700
	 * @tc.name testBlitFramebuffer_02
	 * @tc.desc Test blitFramebuffer.
	 */
	it('testBlitFramebuffer_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBlitFramebuffer_02 test start ...66');
		gl2.blitFramebuffer(-gl2.NEAREST, -0, -400, -gl2.NEAREST, -0, -0, 400, 1080,
			-gl2.COLOR_BUFFER_BIT, -gl2.NEAREST);
		const blitFramebufferError = gl.getError();
		console.info("blitFramebufferError: " + blitFramebufferError);
		expect(blitFramebufferError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0701
	 * @tc.name testFramebufferTextureLayer_01
	 * @tc.desc Test framebufferTextureLayer.
	 */
	it('testFramebufferTextureLayer_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testFramebufferTextureLayer_01 test start ...66');
		const shader = gl.createShader(gl.VERTEX_SHADER);
		gl2.framebufferTextureLayer(-gl.FRAMEBUFFER, -gl.COLOR_ATTACHMENT0, shader, -0, -8)
		const framebufferTextureLayerError = gl.getError();
		console.info("framebufferTextureLayerError: " + framebufferTextureLayerError);
		expect(framebufferTextureLayerError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0702
	 * @tc.name testFramebufferTextureLayer_02
	 * @tc.desc Test framebufferTextureLayer.
	 */
	it('testFramebufferTextureLayer_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testFramebufferTextureLayer_02 test start ...66');
		const texture = gl.createTexture();
		gl.bindTexture(gl2.TEXTURE_2D_ARRAY, texture);
		gl2.texStorage3D(gl2.TEXTURE_2D_ARRAY, 1, gl2.RGBA8, 256, 256, 8);
		gl.texParameteri(gl2.TEXTURE_CUBE_MAP, gl2.TEXTURE_MAG_FILTER, gl2.NEAREST);
		const framebuffer = gl.createFramebuffer();
		gl.bindFramebuffer(gl.FRAMEBUFFER, framebuffer);
		gl2.framebufferTextureLayer(gl.FRAMEBUFFER, gl2.COLOR_ATTACHMENT0, texture, 0, 8)
		const framebufferTextureLayerError = gl.getError();
		console.info("framebufferTextureLayerError: " + framebufferTextureLayerError);
		expect(framebufferTextureLayerError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0703
	 * @tc.name testInvalidateFramebuffer_01
	 * @tc.desc Test invalidateFramebuffer.
	 */
	it('testInvalidateFramebuffer_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testInvalidateFramebuffer_01 test start ...66');
		gl2.invalidateFramebuffer(-0,
			[-0, -gl.COLOR_ATTACHMENT1]);
		const invalidateFramebufferError = gl.getError();
		console.info("invalidateFramebufferError: " + invalidateFramebufferError);
		expect(invalidateFramebufferError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0704
	 * @tc.name testInvalidateFramebuffer_02
	 * @tc.desc Test invalidateFramebuffer.
	 */
	it('testInvalidateFramebuffer_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testInvalidateFramebuffer_02 test start ...66');
		gl2.invalidateFramebuffer(-gl.READ_FRAMEBUFFER,
			[gl.COLOR_ATTACHMENT0, -gl.COLOR_ATTACHMENT1]);
		const invalidateFramebufferError = gl.getError();
		console.info("invalidateFramebufferError: " + invalidateFramebufferError);
		expect(invalidateFramebufferError).assertEqual(gl.INVALID_ENUM);
		done();
	})


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0705
	 * @tc.name testInvalidateSubFramebuffer_01
	 * @tc.desc Test invalidateSubFramebuffer.
	 */
	it('testInvalidateSubFramebuffer_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testInvalidateSubFramebuffer_01 test start ...66');
		console.info('jsWebGL testInvalidateSubFramebuffer test start ...' + JSON.stringify(gl));
		gl2.invalidateSubFramebuffer(-gl.READ_FRAMEBUFFER,
			[gl.COLOR_ATTACHMENT0, -gl.COLOR_ATTACHMENT1],
			0, 0, 256, 256);
		const invalidateSubFramebufferError = gl.getError();
		console.info("invalidateSubFramebufferError: " + invalidateSubFramebufferError);
		expect(invalidateSubFramebufferError).assertEqual(gl.INVALID_ENUM);
		done();
	})


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0706
	 * @tc.name testInvalidateSubFramebuffer_01
	 * @tc.desc Test invalidateSubFramebuffer.
	 */
	it('testInvalidateSubFramebuffer_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testInvalidateSubFramebuffer_02 test start ...66');
		console.info('jsWebGL testInvalidateSubFramebuffer test start ...' + JSON.stringify(gl));
		gl2.invalidateSubFramebuffer(-gl.READ_FRAMEBUFFER,
			[-0, -gl.COLOR_ATTACHMENT1],
			0, -0, 256, -256);
		const invalidateSubFramebufferError = gl.getError();
		console.info("invalidateSubFramebufferError: " + invalidateSubFramebufferError);
		expect(invalidateSubFramebufferError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0707
	 * @tc.name testReadBuffer_01
	 * @tc.desc Test readBuffer.
	 */
	it('testReadBuffer_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testReadBuffer_01 test start ...66');
		gl2.readBuffer(-gl.COLOR_ATTACHMENT0);
		const readBufferError = gl.getError();
		console.info("readBufferError: " + readBufferError);
		expect(readBufferError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0708
	 * @tc.name testReadBuffer_02
	 * @tc.desc Test readBuffer.
	 */
	it('testReadBuffer_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testReadBuffer_02 test start ...66');
		gl2.readBuffer(gl.INVALID_ENUM);
		const readBufferError = gl.getError();
		console.info("readBufferError: " + readBufferError);
		expect(readBufferError).assertEqual(gl.INVALID_ENUM);
		done();
	})


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0709
	 * @tc.name testGetInternalformatParameter_02
	 * @tc.desc Test getInternalformatParameter.
	 */
	it('testGetInternalformatParameter_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testGetInternalformatParameter_02 test start ...66');
		var samples = gl2.getInternalformatParameter(gl.RENDERBUFFER, gl2.RGBA8, gl.SAMPLES);
		console.info("samples: " + samples);
		const int32list = [-1, -1, -1];
		expect(samples.constructor == int32list.constructor).assertEqual(false);
		done();
	})


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0710
	 * @tc.name testRenderbufferStorageMultisample_01
	 * @tc.desc Test renderbufferStorageMultisample.
	 */
	it('testRenderbufferStorageMultisample_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testRenderbufferStorageMultisample test start ...66');
		gl2.renderbufferStorageMultisample(-gl.RENDERBUFFER, -4, -gl.RBGA4, -256, -256);
		const renderbufferStorageMultisampleError = gl.getError();
		console.info("renderbufferStorageMultisampleError: " + renderbufferStorageMultisampleError);
		expect(renderbufferStorageMultisampleError).assertEqual(gl.INVALID_ENUM);
		done();
	})


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0711
	 * @tc.name testRenderbufferStorageMultisample_02
	 * @tc.desc Test renderbufferStorageMultisample.
	 */
	it('testRenderbufferStorageMultisample_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testRenderbufferStorageMultisample test start ...66');
		gl2.renderbufferStorageMultisample(gl.ONE_MINUS_DST_COLOR, -0, -0, 256, 256);
		const renderbufferStorageMultisampleError = gl.getError();
		console.info("renderbufferStorageMultisampleError: " + renderbufferStorageMultisampleError);
		expect(renderbufferStorageMultisampleError).assertEqual(gl.INVALID_ENUM);
		done();
	})


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0712
	 * @tc.name testTexStorage2D_01
	 * @tc.desc Test texStorage2D.
	 */
	it('testTexStorage2D_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexStorage2D_01 test start ...66');
		gl2.texStorage2D(-gl.TEXTURE_2D, -1, -gl.RGB8, -256, -256);
		const texStorage2DError = gl.getError();
		console.info("texStorage2DError: " + texStorage2DError);
		expect(texStorage2DError).assertEqual(gl.INVALID_VALUE);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0713
	 * @tc.name testTexStorage2D_02
	 * @tc.desc Test texStorage2D.
	 */
	it('testTexStorage2D_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexStorage2D_02 test start ...66');
		gl2.texStorage2D(-gl.SRC_ALPHA_SATURATE, -0, -gl.RGB8, 256, -256);
		const texStorage2DError = gl.getError();
		console.info("texStorage2DError: " + texStorage2DError);
		expect(texStorage2DError).assertEqual(gl.INVALID_VALUE);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0714
	 * @tc.name testTransformFeedbackVaryings_01
	 * @tc.desc Test transformFeedbackVaryings.
	 */
	it('testTransformFeedbackVaryings_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTransformFeedbackVaryings_01 test start ...66');
		const program = gl.createProgram();
		var transformFeedbackOutputs = ['-gl_Position', 'anotherOutput'];
		gl2.transformFeedbackVaryings(program, transformFeedbackOutputs, -gl.INTERLEAVED_ATTRIBS);
		const transformFeedbackVaryingsError = gl.getError();
		console.info("transformFeedbackVaryingsError: " + transformFeedbackVaryingsError);
		expect(transformFeedbackVaryingsError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0715
	 * @tc.name testTransformFeedbackVaryings_02
	 * @tc.desc Test transformFeedbackVaryings.
	 */
	it('testTransformFeedbackVaryings_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTransformFeedbackVaryings_02 test start ...66');
		const program = gl.createProgram();
		var transformFeedbackOutputs = [-'gl_Position', -'anotherOutput'];
		gl2.transformFeedbackVaryings(program, transformFeedbackOutputs, gl.ONE_MINUS_DST_COLOR);
		const transformFeedbackVaryingsError = gl.getError();
		console.info("transformFeedbackVaryingsError: " + transformFeedbackVaryingsError);
		expect(transformFeedbackVaryingsError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0716
	 * @tc.name testGetTransformFeedbackVarying_01
	 * @tc.desc Test getTransformFeedbackVarying.
	 */
	it('testGetTransformFeedbackVarying_01', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetTransformFeedbackVarying_01 test start ...66');
			const programObj = createProgram(gl);
			var getTransformFeedbackVaryingobject = gl2.getTransformFeedbackVarying(-programObj, -
				0);
			console.info("getTransformFeedbackVaryingobject: " + getTransformFeedbackVaryingobject);
			const getTransformFeedbackVaryingError = gl.getError();
			console.info("getTransformFeedbackVaryingError: " + getTransformFeedbackVaryingError);
			expect(getTransformFeedbackVaryingError).assertEqual(gl.NO_ERROR);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetTransformFeedbackVarying has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0717
	 * @tc.name testBindBufferBase_01
	 * @tc.desc Test bindBufferBase.
	 */
	it('testBindBufferBase_01', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testBindBufferBase_01 test start ...66');
			let buffer = gl.createBuffer();
			gl2.bindBufferBase(-gl.TRANSFORM_FEEDBACK_BUFFER, -0, buffer);
			const bindBufferBaseError = gl.getError();
			console.info("bindBufferBaseError: " + bindBufferBaseError);
			expect(bindBufferBaseError).assertEqual(gl.INVALID_ENUM);
			//deleteContext();
			done();
		} catch (e) {
			console.log("bindBufferBaseError has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0718
	 * @tc.name testBindBufferBase_02
	 * @tc.desc Test bindBufferBase.
	 */
	it('testBindBufferBase_02', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testBindBufferBase_02 test start ...66');
			var programobject = gl.createProgram();
			gl2.bindBufferBase(-gl.SRC_ALPHA_SATURATE, -0, programobject);
			const bindBufferBaseError = gl.getError();
			console.info("bindBufferBaseError: " + bindBufferBaseError);
			expect(bindBufferBaseError).assertEqual(gl.INVALID_ENUM);
			//deleteContext();
			done();
		} catch (e) {
			console.log("bindBufferBaseError has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0719
	 * @tc.name testGetTransformFeedbackVarying_02
	 * @tc.desc Test getTransformFeedbackVarying.
	 */
	it('testGetTransformFeedbackVarying_02', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetTransformFeedbackVarying_02 test start ...66');
			const programObj = gl.createProgram();
			var getTransformFeedbackVaryingobject = gl2.getTransformFeedbackVarying(programObj, 0);
			console.info("getTransformFeedbackVaryingobject: " + getTransformFeedbackVaryingobject);
			const getTransformFeedbackVaryingError = gl.getError();
			console.info("getTransformFeedbackVaryingError: " + getTransformFeedbackVaryingError);
			expect(getTransformFeedbackVaryingError).assertEqual(gl.INVALID_VALUE);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetTransformFeedbackVarying has failed for " + e)
			expect(null).assertFail()
		}
	})


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0720
	 * @tc.name bindBufferRange
	 * @tc.desc Test bindBufferRange.
	 */
	it('bindBufferRange', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL bindBufferRange test start ...66');
			const buffer = gl.createBuffer();
			gl2.bindBufferRange(gl.TRANSFORM_FEEDBACK_BUFFER, 1, buffer, 0, 4);
			const bindBufferRangeError = gl.getError();
			console.info("bindBufferRangeError: " + bindBufferRangeError);
			expect(bindBufferRangeError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("bindBufferRange has failed for " + e)
			expect(null).assertFail()
		}
	})


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0721
	 * @tc.name bindBufferRange_01
	 * @tc.desc Test bindBufferRange.
	 */
	it('bindBufferRange_01', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL bindBufferRange_01 test start ...66');
			const buffer = gl.createBuffer();
			gl2.bindBufferRange(-gl.TRANSFORM_FEEDBACK_BUFFER, -0, buffer, -0, -4);
			const bindBufferRangeError = gl.getError();
			console.info("bindBufferRangeError: " + bindBufferRangeError);
			expect(bindBufferRangeError).assertEqual(gl.INVALID_VALUE);
			//deleteContext();
			done();
		} catch (e) {
			console.log("bindBufferRange has failed for " + e)
			expect(null).assertFail()
		}
	})


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0722
	 * @tc.name bindBufferRange_02
	 * @tc.desc Test bindBufferRange.
	 */
	it('bindBufferRange_02', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL bindBufferRange_01 test start ...66');
			const buffer = gl.createBuffer();
			gl2.bindBufferRange(-gl.TRANSFORM_FEEDBACK_BUFFER, -1, buffer, -0, 4);
			const bindBufferRangeError = gl.getError();
			console.info("bindBufferRangeError: " + bindBufferRangeError);
			expect(bindBufferRangeError).assertEqual(gl.INVALID_ENUM);
			//deleteContext();
			done();
		} catch (e) {
			console.log("bindBufferRange has failed for " + e)
			expect(null).assertFail()
		}
	})


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0723
	 * @tc.name testIsShader_05
	 * @tc.desc Test isShader.
	 */
	it('testIsShader_05', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testIsShader_05 test start ...66');
		var programobject = gl.createProgram();
		const isShader = gl.isShader(-0);
		console.info("createShader --> isShader: " + isShader);
		expect(isShader).assertEqual(false);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0724
	 * @tc.name testLinkProgram_05
	 * @tc.desc Test linkProgram.
	 */
	it('testLinkProgram_05', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testLinkProgram_05 test start ...66');
		const fragmentShader = gl.createShader(gl.FRAGMENT_SHADER);
		gl.linkProgram(fragmentShader);
		const linkProgramError = gl.getError();
		console.info("linkProgramError: " + linkProgramError);
		expect(linkProgramError).assertEqual(gl.INVALID_OPERATION);
		done();
	})




	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0725
	 * @tc.name testPauseTransformFeedback_01
	 * @tc.desc Test pauseTransformFeedback.
	 */
	it('testPauseTransformFeedback_01', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testPauseTransformFeedback_01 test start ...66');
			const transformFeedback = gl2.createTransformFeedback();
			gl2.bindTransformFeedback(gl.TRANSFORM_FEEDBACK, transformFeedback);
			gl2.beginTransformFeedback(gl2.TRIANGLES);
			gl2.pauseTransformFeedback();
			const pauseTransformFeedbackError = gl.getError();
			console.info("pauseTransformFeedbackError: " + pauseTransformFeedbackError);
			expect(pauseTransformFeedbackError == gl.INVALID_OPERATION).assertEqual(true);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testPauseTransformFeedback_01 has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0726
	 * @tc.name testResumeTransformFeedback_01
	 * @tc.desc Test resumeTransformFeedback.
	 */
	it('testResumeTransformFeedback_01', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testResumeTransformFeedback_01 test start ...66');
			gl2.resumeTransformFeedback();
			const resumeTransformFeedbackError = gl.getError();
			console.info("resumeTransformFeedbackError: " + resumeTransformFeedbackError);
			expect(resumeTransformFeedbackError == gl.INVALID_OPERATION).assertEqual(true);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testResumeTransformFeedback_01 has failed for " + e)
			expect(null).assertFail()
		}
	});



	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0727
	 * @tc.name webgl_test_blendEquation2
	 * @tc.desc Test blendEquation.
	 */
	it('webgl_test_blendEquation2', 0, async function(done) {
		//initContext();
		console.info("webgltest into blendEquation");

		gl.blendEquation(gl.FUNC_REVERSE_SUBTRACT);

		const blendEquationValue = gl.getParameter(gl.BLEND_EQUATION_RGB);
		console.info("blendEquation --> getParameter: " + blendEquationValue);
		expect(blendEquationValue).assertEqual(gl.FUNC_REVERSE_SUBTRACT);

		//deleteContext();
		done();
	});



	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0728
	 * @tc.name webgl_test_activeTexture_2
	 * @tc.desc Test activeTexture.
	 */
	it('webgl_test_activeTexture_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into activeTexture");
		gl.activeTexture(gl.TEXTURE2);
		const activeTextureParameter = gl.getParameter(gl.ACTIVE_TEXTURE);
		console.info("webgltest activeTexture --> getParameter: " + activeTextureParameter);
		expect(activeTextureParameter).assertEqual(33986);
		//deleteContext();
		done();
	});



	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0729
	 * @tc.name webgl_test_activeTexture_3
	 * @tc.desc Test activeTexture.
	 */
	it('webgl_test_activeTexture_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into activeTexture");
		gl.activeTexture(gl.TEXTURE3);
		const activeTextureParameter = gl.getParameter(gl.ACTIVE_TEXTURE);
		console.info("webgltest activeTexture --> getParameter: " + activeTextureParameter);
		expect(activeTextureParameter).assertEqual(33987);
		//deleteContext();
		done();
	});



	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0730
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

		gl.bindAttribLocation(programObj, -1, 'a_Position');

		let errorCode = gl.getError();
		console.info("webgltest bindAttribLocation getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_VALUE);
		//deleteContext();
		done();
	});




	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0731
	 * @tc.name webgl_test_bindBuffer_2
	 * @tc.desc Test bindBuffer.
	 */
	it('webgl_test_bindBuffer_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into bindBuffer");

		var buffer = gl.createBuffer();

		gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, buffer);

		//bind后isBuffer为true
		const isBufferValue = gl.isBuffer(buffer);
		console.info("isBufferValue: " + isBufferValue);
		expect(isBufferValue).assertEqual(true);

		//deleteContext();
		done();
	});



	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0732
	 * @tc.name webgl_test_bindTexture_2
	 * @tc.desc Test bindTexture.
	 */
	it('webgl_test_bindTexture_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into bindTexture");

		var texture = gl.createTexture();

		const isTexture0 = gl.isTexture(texture);
		console.info("createTexture --> isTexture: " + isTexture0);
		expect(isTexture0).assertEqual(false);

		gl.bindTexture(gl.TEXTURE_CUBE_MAP, texture);

		const isTexture1 = gl.isTexture(texture);
		console.info("createTexture --> bindTexture --> isTexture: " + isTexture1);
		expect(isTexture1).assertEqual(true);

		//deleteContext();
		done();
	});



	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0733
	 * @tc.name webgl_test_blendEquation__2
	 * @tc.desc Test blendEquation.
	 */
	it('webgl_test_blendEquation__2', 0, async function(done) {
		//initContext();
		console.info("webgltest into blendEquation");

		gl.blendEquation(gl.FUNC_ADD);

		const blendEquationValue = gl.getParameter(gl.BLEND_EQUATION_RGB);
		console.info("blendEquation --> getParameter: " + blendEquationValue);
		expect(blendEquationValue).assertEqual(32774);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0734
	 * @tc.name webgl_test_blendEquation_3
	 * @tc.desc Test blendEquation.
	 */
	it('webgl_test_blendEquation_3', 0, async function(done) {
		//initContext();
		console.info("webgltest into blendEquation");

		gl.blendEquation(gl.FUNC_SUBTRACT);

		const blendEquationValue = gl.getParameter(gl.BLEND_EQUATION_RGB);
		console.info("blendEquation --> getParameter: " + blendEquationValue);
		expect(blendEquationValue).assertEqual(gl.FUNC_SUBTRACT);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0735
	 * @tc.name webgl_test_blendEquationSeparate_2
	 * @tc.desc Test blendEquationSeparate.
	 */
	it('webgl_test_blendEquationSeparate_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into blendEquationSeparate");
		gl.blendEquationSeparate(gl.FUNC_ADD, gl.FUNC_REVERSE_SUBTRACT);

		const blendEquationSeparateValue = gl.getParameter(gl.BLEND_EQUATION_RGB);
		console.info("blendEquationSeparate --> getParameter: " + blendEquationSeparateValue);
		expect(blendEquationSeparateValue).assertEqual(gl.FUNC_ADD);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0736
	 * @tc.name webgl_test_blendEquationSeparate_3
	 * @tc.desc Test blendEquationSeparate.
	 */
	it('webgl_test_blendEquationSeparate_3', 0, async function(done) {
		//initContext();
		console.info("webgltest into blendEquationSeparate");
		gl.blendEquationSeparate(gl.FUNC_SUBTRACT, gl.FUNC_REVERSE_SUBTRACT);

		const blendEquationSeparateValue = gl.getParameter(gl.BLEND_EQUATION_RGB);
		console.info("blendEquationSeparate --> getParameter: " + blendEquationSeparateValue);
		expect(blendEquationSeparateValue).assertEqual(gl.FUNC_SUBTRACT);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0737
	 * @tc.name webgl_test_blendFunc_2
	 * @tc.desc Test blendFunc.
	 */
	it('webgl_test_blendFunc_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into blendFunc");

		gl.enable(gl.BLEND);

		gl.blendFunc(gl.ZERO, gl.ONE);

		const blendFuncValue = gl.getParameter(gl.BLEND_SRC_RGB);
		console.info("blendFunc --> getParameter: " + blendFuncValue);
		expect(blendFuncValue).assertEqual(gl.ZERO);

		//deleteContext();
		done();
	});



	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0738
	 * @tc.name webgl_test_blendFunc_3
	 * @tc.desc Test blendFunc.
	 */
	it('webgl_test_blendFunc_3', 0, async function(done) {
		//initContext();
		console.info("webgltest into blendFunc");

		gl.enable(gl.BLEND);

		gl.blendFunc(gl.SRC_ALPHA, gl.DST_ALPHA);

		const blendFuncValue = gl.getParameter(gl.BLEND_SRC_RGB);
		console.info("blendFunc --> getParameter: " + blendFuncValue);
		expect(blendFuncValue).assertEqual(gl.SRC_ALPHA);

		//deleteContext();
		done();
	});



	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0739
	 * @tc.name webgl_test_blendFuncSeparate_2
	 * @tc.desc Test blendFuncSeparate.
	 */
	it('webgl_test_blendFuncSeparate_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into blendFuncSeparate");

		gl.enable(gl.BLEND);

		gl.blendFuncSeparate(gl.DST_ALPHA, gl.CONSTANT_COLOR, gl.CONSTANT_ALPHA, gl
			.ONE_MINUS_CONSTANT_ALPHA);

		const blendFuncSeparateParameter = gl.getParameter(gl.BLEND_SRC_RGB)
		console.info("blendFuncSeparate --> getParameter: " + blendFuncSeparateParameter);
		expect(blendFuncSeparateParameter).assertEqual(gl.DST_ALPHA);

		//deleteContext();
		done();
	});



	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0740
	 * @tc.name webgl_test_blendFuncSeparate_3
	 * @tc.desc Test blendFuncSeparate.
	 */
	it('webgl_test_blendFuncSeparate_3', 0, async function(done) {
		//initContext();
		console.info("webgltest into blendFuncSeparate");

		gl.enable(gl.BLEND);

		gl.blendFuncSeparate(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA, gl.DST_ALPHA, gl
			.ONE_MINUS_DST_ALPHA);

		const blendFuncSeparateParameter = gl.getParameter(gl.BLEND_SRC_RGB)
		console.info("blendFuncSeparate --> getParameter: " + blendFuncSeparateParameter);
		expect(blendFuncSeparateParameter).assertEqual(gl.SRC_ALPHA);

		//deleteContext();
		done();
	});



	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0741
	 * @tc.name webgl_test_checkFramebufferStatus22
	 * @tc.desc Test checkFramebufferStatus.
	 */
	it('webgl_test_checkFramebufferStatus22', 0, async function(done) {
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
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0742
	 * @tc.name webgl_test_clear_2
	 * @tc.desc Test clear.
	 */
	it('webgl_test_clear_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into clear");
		gl.clear(gl.DEPTH_BUFFER_BIT);
		var float32Array = new Float32Array([0, 0, 0, 0]);
		const clearParameter = gl.getParameter(gl.DEPTH_CLEAR_VALUE);
		console.info("webgltest clear getParameter: " + clearParameter);
		expect(typeof(clearParameter)).assertEqual('number');
		//deleteContext();
		done();
	});



	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0743
	 * @tc.name webgl_test_clear_3
	 * @tc.desc Test clear.
	 */
	it('webgl_test_clear_3', 0, async function(done) {
		//initContext();
		console.info("webgltest into clear");
		gl.clear(gl.STENCIL_BUFFER_BIT);
		var float32Array = new Float32Array([0, 0, 0, 0]);
		const clearParameter = gl.getParameter(gl.STENCIL_CLEAR_VALUE);
		console.info("webgltest clear getParameter: " + clearParameter);
		expect(typeof(clearParameter)).assertEqual('number');
		//deleteContext();
		done();
	});



	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0744
	 * @tc.name webgl_test_clearColor_2
	 * @tc.desc Test clearColor.
	 */
	it('webgl_test_clearColor_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into clearColor");
		gl.clearColor(1, 3, 0.5, 3);

		const clearColorParameter = gl.getParameter(gl.COLOR_CLEAR_VALUE);

		var float32ArrayValue = new Float32Array([1, 3, 0.5, 3]);

		console.info("webgltest clearColor: " + clearColorParameter);
		expect(clearColorParameter.toString()).assertEqual(float32ArrayValue.toString());
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0745
	 * @tc.name webgl_test_copyTexImage2D_2
	 * @tc.desc Test copyTexImage2D.
	 */
	it('webgl_test_copyTexImage2D_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into copyTexImage2D");
		gl.copyTexImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_X, 0, gl.ALPHA, 0, 0, 512, 512, 0);
		let errorCode = gl.getError();
		console.info("webgltest copyTexImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_FRAMEBUFFER_OPERATION);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0746
	 * @tc.name webgl_test_copyTexImage2D_3
	 * @tc.desc Test copyTexImage2D.
	 */
	it('webgl_test_copyTexImage2D_3', 0, async function(done) {
		//initContext();
		console.info("webgltest into copyTexImage2D");
		gl.copyTexImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_Y, 0, gl.LUMINANCE, 0, 0, 512, 512, 0);
		let errorCode = gl.getError();
		console.info("webgltest copyTexImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_FRAMEBUFFER_OPERATION);

		//deleteContext();
		done();
	});



	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0747
	 * @tc.name webgl_test_copyTexSubImage2D_2
	 * @tc.desc Test copyTexSubImage2D.
	 */
	it('webgl_test_copyTexSubImage2D_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into copyTexSubImage2D");

		gl.copyTexSubImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_X, 0, 0, 0, 0, 0, 16, 16);
		let errorCode = gl.getError();
		console.info("webgltest copyTexSubImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0748
	 * @tc.name webgl_test_copyTexSubImage2D_3
	 * @tc.desc Test copyTexSubImage2D.
	 */
	it('webgl_test_copyTexSubImage2D_3', 0, async function(done) {
		//initContext();
		console.info("webgltest into copyTexSubImage2D");

		gl.copyTexSubImage2D(gl.TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, 0, 0, 0, 0, 16, 16);
		let errorCode = gl.getError();
		console.info("webgltest copyTexSubImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0749
	 * @tc.name webgl_test_copyTexSubImage2D_4
	 * @tc.desc Test copyTexSubImage2D.
	 */
	it('webgl_test_copyTexSubImage2D_4', 0, async function(done) {
		//initContext();
		console.info("webgltest into copyTexSubImage2D");

		gl.copyTexSubImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_Z, 0, 0, 0, 0, 0, 16, 16);
		let errorCode = gl.getError();
		console.info("webgltest copyTexSubImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0750
	 * @tc.name webgl_test_createShader_2
	 * @tc.desc Test createShader.
	 */
	it('webgl_test_createShader_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into createShader");

		const shader = gl.createShader(gl.FRAGMENT_SHADER);
		const isShader = gl.isShader(shader);
		console.info("webgltest createShader isShader: " + isShader);
		expect(isShader).assertEqual(true);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0751
	 * @tc.name webgl_test_disable_22
	 * @tc.desc Test disable.
	 */
	it('webgl_test_disable_22', 0, async function(done) {
		//initContext();
		console.info("webgltest into disable");
		gl.enable(gl.BLEND);
		const isEnabled2 = gl.isEnabled(gl.BLEND);
		// true
		console.info("webgltest disable isEnabled2: " + isEnabled2);
		expect(isEnabled2).assertEqual(true);

		gl.disable(gl.BLEND);
		// 要检查功能是否被禁用，请使用以下WebGLRenderingContext.isEnabled()方法：
		const isEnabled = gl.isEnabled(gl.BLEND);
		// false
		console.info("webgltest disable isEnabled: " + isEnabled);
		expect(isEnabled).assertEqual(false);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0752
	 * @tc.name webgl_test_disable_23
	 * @tc.desc Test disable.
	 */
	it('webgl_test_disable_23', 0, async function(done) {
		//initContext();
		console.info("webgltest into disable");
		gl.enable(gl.BLEND);
		const isEnabled2 = gl.isEnabled(gl.BLEND);
		// true
		console.info("webgltest disable isEnabled2: " + isEnabled2);
		expect(isEnabled2).assertEqual(true);

		gl.disable(gl.BLEND);
		// 要检查功能是否被禁用，请使用以下WebGLRenderingContext.isEnabled()方法：
		const isEnabled = gl.isEnabled(gl.BLEND);
		// false
		console.info("webgltest disable isEnabled: " + isEnabled);
		expect(isEnabled).assertEqual(false);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0753
	 * @tc.name webgl_test_disable_24
	 * @tc.desc Test disable.
	 */
	it('webgl_test_disable_24', 0, async function(done) {
		//initContext();
		console.info("webgltest into disable");
		gl.enable(gl.DEPTH_TEST);
		const isEnabled2 = gl.isEnabled(gl.DEPTH_TEST);
		// true
		console.info("webgltest disable isEnabled2: " + isEnabled2);
		expect(isEnabled2).assertEqual(true);

		gl.disable(gl.DEPTH_TEST);
		// 要检查功能是否被禁用，请使用以下WebGLRenderingContext.isEnabled()方法：
		const isEnabled = gl.isEnabled(gl.DEPTH_TEST);
		// false
		console.info("webgltest disable isEnabled: " + isEnabled);
		expect(isEnabled).assertEqual(false);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0754
	 * @tc.name webgl_test_disable_25
	 * @tc.desc Test disable.
	 */
	it('webgl_test_disable_25', 0, async function(done) {
		//initContext();
		console.info("webgltest into disable");
		gl.enable(gl.POLYGON_OFFSET_FILL);
		const isEnabled2 = gl.isEnabled(gl.POLYGON_OFFSET_FILL);
		// true
		console.info("webgltest disable isEnabled2: " + isEnabled2);
		expect(isEnabled2).assertEqual(true);

		gl.disable(gl.POLYGON_OFFSET_FILL);
		// 要检查功能是否被禁用，请使用以下WebGLRenderingContext.isEnabled()方法：
		const isEnabled = gl.isEnabled(gl.POLYGON_OFFSET_FILL);
		// false
		console.info("webgltest disable isEnabled: " + isEnabled);
		expect(isEnabled).assertEqual(false);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0755
	 * @tc.name webgl_test_disable_26
	 * @tc.desc Test disable.
	 */
	it('webgl_test_disable_26', 0, async function(done) {
		//initContext();
		console.info("webgltest into disable");
		gl.enable(gl.SCISSOR_TEST);
		const isEnabled2 = gl.isEnabled(gl.SCISSOR_TEST);
		// true
		console.info("webgltest disable isEnabled2: " + isEnabled2);
		expect(isEnabled2).assertEqual(true);

		gl.disable(gl.SCISSOR_TEST);
		// 要检查功能是否被禁用，请使用以下WebGLRenderingContext.isEnabled()方法：
		const isEnabled = gl.isEnabled(gl.SCISSOR_TEST);
		// false
		console.info("webgltest disable isEnabled: " + isEnabled);
		expect(isEnabled).assertEqual(false);

		//deleteContext();
		done();
	});



	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0756
	 * @tc.name webgl_test_disableVertexAttribArray_2
	 * @tc.desc Test disableVertexAttribArray.
	 */
	it('webgl_test_disableVertexAttribArray_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into disableVertexAttribArray");
		gl.disableVertexAttribArray(-1);
		let errorCode = gl.getError();
		console.info("webgltest disableVertexAttribArray getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_VALUE);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0757
	 * @tc.name webgl_test_drawArrays2
	 * @tc.desc Test drawArrays.
	 */
	it('webgl_test_drawArrays2', 0, async function(done) {
		//initContext();
		console.info("webgltest into drawArrays");

		gl.drawArrays(gl.LINE_STRIP, 0, 8);
		let errorCode = gl.getError();
		console.info("webgltest drawArrays getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_FRAMEBUFFER_OPERATION);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0758
	 * @tc.name webgl_test_drawArrays3
	 * @tc.desc Test drawArrays.
	 */
	it('webgl_test_drawArrays3', 0, async function(done) {
		//initContext();
		console.info("webgltest into drawArrays");

		gl.drawArrays(gl.LINE_LOOP, 0, 8);
		let errorCode = gl.getError();
		console.info("webgltest drawArrays getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_FRAMEBUFFER_OPERATION);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0759
	 * @tc.name webgl_test_drawArrays4
	 * @tc.desc Test drawArrays.
	 */
	it('webgl_test_drawArrays4', 0, async function(done) {
		//initContext();
		console.info("webgltest into drawArrays");

		gl.drawArrays(gl.LINES, 0, 8);
		let errorCode = gl.getError();
		console.info("webgltest drawArrays getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_FRAMEBUFFER_OPERATION);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0760
	 * @tc.name webgl_test_drawArrays5
	 * @tc.desc Test drawArrays.
	 */
	it('webgl_test_drawArrays5', 0, async function(done) {
		//initContext();
		console.info("webgltest into drawArrays");

		gl.drawArrays(gl.TRIANGLE_STRIP, 0, 8);
		let errorCode = gl.getError();
		console.info("webgltest drawArrays getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_FRAMEBUFFER_OPERATION);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0761
	 * @tc.name webgl_test_drawArrays6
	 * @tc.desc Test drawArrays.
	 */
	it('webgl_test_drawArrays6', 0, async function(done) {
		//initContext();
		console.info("webgltest into drawArrays");

		gl.drawArrays(gl.TRIANGLE_FAN, 0, 8);
		let errorCode = gl.getError();
		console.info("webgltest drawArrays getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_FRAMEBUFFER_OPERATION);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0762
	 * @tc.name webgl_test_drawArrays7
	 * @tc.desc Test drawArrays.
	 */
	it('webgl_test_drawArrays7', 0, async function(done) {
		//initContext();
		console.info("webgltest into drawArrays");

		gl.drawArrays(gl.TRIANGLES, 0, 8);
		let errorCode = gl.getError();
		console.info("webgltest drawArrays getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_FRAMEBUFFER_OPERATION);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0763
	 * @tc.name webgl_test_drawElements22
	 * @tc.desc Test drawElements.
	 */
	it('webgl_test_drawElements22', 0, async function(done) {
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
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0764
	 * @tc.name webgl_test_drawElements23
	 * @tc.desc Test drawElements.
	 */
	it('webgl_test_drawElements23', 0, async function(done) {
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
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0765
	 * @tc.name webgl_test_drawElements24
	 * @tc.desc Test drawElements.
	 */
	it('webgl_test_drawElements24', 0, async function(done) {
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
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0766
	 * @tc.name webgl_test_enable
	 * @tc.desc Test enable.
	 */
	it('webgl_test_enable', 0, async function(done) {
		//initContext();
		console.info("webgltest into enable");

		gl.enable(gl.BLEND);

		const isEnabled = gl.isEnabled(gl.BLEND);
		// true
		console.info("webgltest enable isEnabled: " + isEnabled);
		expect(isEnabled).assertEqual(true);

		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0767
	 * @tc.name webgl_test_enable22
	 * @tc.desc Test enable.
	 */
	it('webgl_test_enable22', 0, async function(done) {
		//initContext();
		console.info("webgltest into enable");

		gl.enable(gl.CULL_FACE);

		const isEnabled = gl.isEnabled(gl.CULL_FACE);
		// true
		console.info("webgltest enable isEnabled: " + isEnabled);
		expect(isEnabled).assertEqual(true);

		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0768
	 * @tc.name webgl_test_enable23
	 * @tc.desc Test enable.
	 */
	it('webgl_test_enable23', 0, async function(done) {
		//initContext();
		console.info("webgltest into enable");

		gl.enable(gl.DEPTH_TEST);

		const isEnabled = gl.isEnabled(gl.DEPTH_TEST);
		// true
		console.info("webgltest enable isEnabled: " + isEnabled);
		expect(isEnabled).assertEqual(true);

		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0769
	 * @tc.name webgl_test_enable24
	 * @tc.desc Test enable.
	 */
	it('webgl_test_enable24', 0, async function(done) {
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
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0770
	 * @tc.name webgl_test_enable25
	 * @tc.desc Test enable.
	 */
	it('webgl_test_enable25', 0, async function(done) {
		//initContext();
		console.info("webgltest into enable");

		gl.enable(gl.POLYGON_OFFSET_FILL);

		const isEnabled = gl.isEnabled(gl.POLYGON_OFFSET_FILL);
		// true
		console.info("webgltest enable isEnabled: " + isEnabled);
		expect(isEnabled).assertEqual(true);

		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0771
	 * @tc.name webgl_test_enable26
	 * @tc.desc Test enable.
	 */
	it('webgl_test_enable26', 0, async function(done) {
		//initContext();
		console.info("webgltest into enable");

		gl.enable(gl.SAMPLE_ALPHA_TO_COVERAGE);

		const isEnabled = gl.isEnabled(gl.SAMPLE_ALPHA_TO_COVERAGE);
		// true
		console.info("webgltest enable isEnabled: " + isEnabled);
		expect(isEnabled).assertEqual(true);

		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0772
	 * @tc.name webgl_test_enable27
	 * @tc.desc Test enable.
	 */
	it('webgl_test_enable27', 0, async function(done) {
		//initContext();
		console.info("webgltest into enable");

		gl.enable(gl.SAMPLE_COVERAGE);

		const isEnabled = gl.isEnabled(gl.SAMPLE_COVERAGE);
		// true
		console.info("webgltest enable isEnabled: " + isEnabled);
		expect(isEnabled).assertEqual(true);

		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0773
	 * @tc.name webgl_test_enable28
	 * @tc.desc Test enable.
	 */
	it('webgl_test_enable28', 0, async function(done) {
		//initContext();
		console.info("webgltest into enable");

		gl.enable(gl.SCISSOR_TEST);

		const isEnabled = gl.isEnabled(gl.SCISSOR_TEST);
		// true
		console.info("webgltest enable isEnabled: " + isEnabled);
		expect(isEnabled).assertEqual(true);

		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0774
	 * @tc.name webgl_test_enable29
	 * @tc.desc Test enable.
	 */
	it('webgl_test_enable29', 0, async function(done) {
		//initContext();
		console.info("webgltest into enable");

		gl.enable(gl.STENCIL_TEST);

		const isEnabled = gl.isEnabled(gl.STENCIL_TEST);
		// true
		console.info("webgltest enable isEnabled: " + isEnabled);
		expect(isEnabled).assertEqual(true);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0775
	 * @tc.name webgl_test_framebufferRenderbuffer22
	 * @tc.desc Test framebufferRenderbuffer.
	 */
	it('webgl_test_framebufferRenderbuffer22', 0, async function(done) {
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
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0776
	 * @tc.name webgl_test_framebufferRenderbuffer23
	 * @tc.desc Test framebufferRenderbuffer.
	 */
	it('webgl_test_framebufferRenderbuffer23', 0, async function(done) {
		//initContext();
		console.info("webgltest into framebufferRenderbuffer");

		const renderbuffer = gl.createRenderbuffer();
		gl.framebufferRenderbuffer(gl.DRAW_FRAMEBUFFER, gl.DEPTH_ATTACHMENT, gl.RENDERBUFFER,
			renderbuffer);

		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0777
	 * @tc.name webgl_test_framebufferRenderbuffer24
	 * @tc.desc Test framebufferRenderbuffer.
	 */
	it('webgl_test_framebufferRenderbuffer24', 0, async function(done) {
		//initContext();
		console.info("webgltest into framebufferRenderbuffer");

		const renderbuffer = gl.createRenderbuffer();
		gl.framebufferRenderbuffer(gl.DRAW_FRAMEBUFFER, gl.DEPTH_STENCIL_ATTACHMENT, gl
			.RENDERBUFFER, renderbuffer);

		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0778
	 * @tc.name webgl_test_framebufferRenderbuffer25
	 * @tc.desc Test framebufferRenderbuffer.
	 */
	it('webgl_test_framebufferRenderbuffer25', 0, async function(done) {
		//initContext();
		console.info("webgltest into framebufferRenderbuffer");

		const renderbuffer = gl.createRenderbuffer();
		gl.framebufferRenderbuffer(gl.DRAW_FRAMEBUFFER, gl.STENCIL_ATTACHMENT, gl.RENDERBUFFER,
			renderbuffer);

		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0779
	 * @tc.name webgl_test_framebufferTexture2D22
	 * @tc.desc Test framebufferTexture2D.
	 */
	it('webgl_test_framebufferTexture2D22', 0, async function(done) {
		//initContext();
		console.info("webgltest into framebufferTexture2D");
		const texture = gl.createTexture();
		gl.framebufferTexture2D(gl.DRAW_FRAMEBUFFER, gl.DEPTH_ATTACHMENT, gl.TEXTURE_2D, texture,
			0);

		let errorCode = gl.getError();
		console.info("webgltest framebufferTexture2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0780
	 * @tc.name webgl_test_framebufferTexture2D23
	 * @tc.desc Test framebufferTexture2D.
	 */
	it('webgl_test_framebufferTexture2D23', 0, async function(done) {
		//initContext();
		console.info("webgltest into framebufferTexture2D");
		const texture = gl.createTexture();
		gl.framebufferTexture2D(gl.DRAW_FRAMEBUFFER, gl.STENCIL_ATTACHMENT, gl.TEXTURE_2D, texture,
			0);

		let errorCode = gl.getError();
		console.info("webgltest framebufferTexture2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0781
	 * @tc.name webgl_test_framebufferTexture2D24
	 * @tc.desc Test framebufferTexture2D.
	 */
	it('webgl_test_framebufferTexture2D24', 0, async function(done) {
		//initContext();
		console.info("webgltest into framebufferTexture2D");
		const texture = gl.createTexture();
		gl.framebufferTexture2D(gl.DRAW_FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl
			.TEXTURE_CUBE_MAP_POSITIVE_X, texture, 0);

		let errorCode = gl.getError();
		console.info("webgltest framebufferTexture2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0782
	 * @tc.name webgl_test_framebufferTexture2D25
	 * @tc.desc Test framebufferTexture2D.
	 */
	it('webgl_test_framebufferTexture2D25', 0, async function(done) {
		//initContext();
		console.info("webgltest into framebufferTexture2D");
		const texture = gl.createTexture();
		gl.framebufferTexture2D(gl.DRAW_FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl
			.TEXTURE_CUBE_MAP_POSITIVE_Y, texture, 0);

		let errorCode = gl.getError();
		console.info("webgltest framebufferTexture2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0783
	 * @tc.name webgl_test_framebufferTexture2D26
	 * @tc.desc Test framebufferTexture2D.
	 */
	it('webgl_test_framebufferTexture2D26', 0, async function(done) {
		//initContext();
		console.info("webgltest into framebufferTexture2D");
		const texture = gl.createTexture();
		gl.framebufferTexture2D(gl.DRAW_FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl
			.TEXTURE_CUBE_MAP_POSITIVE_Z, texture, 0);

		let errorCode = gl.getError();
		console.info("webgltest framebufferTexture2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0784
	 * @tc.name webgl_test_framebufferTexture2D25
	 * @tc.desc Test framebufferTexture2D.
	 */
	it('webgl_test_framebufferTexture2D25', 0, async function(done) {
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
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0785
	 * @tc.name webgl_test_drawBuffers22
	 * @tc.desc Test drawBuffers.
	 */
	it('webgl_test_drawBuffers22', 0, async function(done) {
		//initContext();
		console.info("webgltest into drawBuffers");

		gl2.drawBuffers([gl2.BACK, gl2.COLOR_ATTACHMENT2]);

		let errorCode = gl.getError();
		console.info("webgltest drawBuffers getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});




	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0786
	 * @tc.name webgl_test_drawBuffers23
	 * @tc.desc Test drawBuffers.
	 */
	it('webgl_test_drawBuffers23', 0, async function(done) {
		//initContext();
		console.info("webgltest into drawBuffers");

		gl2.drawBuffers([gl2.NONE, gl2.COLOR_ATTACHMENT3]);

		let errorCode = gl.getError();
		console.info("webgltest drawBuffers getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0787
	 * @tc.name webgl_test_drawBuffers24
	 * @tc.desc Test drawBuffers.
	 */
	it('webgl_test_drawBuffers24', 0, async function(done) {
		//initContext();
		console.info("webgltest into drawBuffers");

		gl2.drawBuffers([gl2.BACK, gl2.COLOR_ATTACHMENT4]);

		let errorCode = gl.getError();
		console.info("webgltest drawBuffers getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0788
	 * @tc.name webgl_test_drawBuffers25
	 * @tc.desc Test drawBuffers.
	 */
	it('webgl_test_drawBuffers25', 0, async function(done) {
		//initContext();
		console.info("webgltest into drawBuffers");

		gl2.drawBuffers([gl2.NONE, gl2.COLOR_ATTACHMENT5]);

		let errorCode = gl.getError();
		console.info("webgltest drawBuffers getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0789
	 * @tc.name webgl_test_drawBuffers26
	 * @tc.desc Test drawBuffers.
	 */
	it('webgl_test_drawBuffers26', 0, async function(done) {
		//initContext();
		console.info("webgltest into drawBuffers");

		gl2.drawBuffers([gl2.BACK, gl2.COLOR_ATTACHMENT6]);

		let errorCode = gl.getError();
		console.info("webgltest drawBuffers getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});




	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0790
	 * @tc.name webgl_test_clearBufferfv22
	 * @tc.desc Test clearBufferfv.
	 */
	it('webgl_test_clearBufferfv22', 0, async function(done) {
		//initContext();
		console.info("webgltest into clearBufferfv");

		gl2.clearBufferfv(gl.DEPTH, 0, [0.0, 0.0, 0.0, 0.0]);

		let errorCode = gl.getError();
		console.info("webgltest webgl_test_clearBufferfv getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0791
	 * @tc.name webgl_test_clearBufferfv_22
	 * @tc.desc Test clearBufferfv.
	 */
	it('webgl_test_clearBufferfv_22', 0, async function(done) {
		//initContext();
		console.info("webgltest into clearBufferfv");

		gl2.clearBufferfv(gl.STENCIL, 0, [0.0, 0.0, 0.0, 0.0]);

		let errorCode = gl.getError();
		console.info("webgltest webgl_test_clearBufferfv getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0792
	 * @tc.name webgl_test_clearBufferfv23
	 * @tc.desc Test clearBufferfv.
	 */
	it('webgl_test_clearBufferfv23', 0, async function(done) {
		//initContext();
		console.info("webgltest into clearBufferfv");

		gl2.clearBufferfv(gl.DEPTH_STENCIL, 0, [0.0, 0.0, 0.0, 0.0]);

		let errorCode = gl.getError();
		console.info("webgltest webgl_test_clearBufferfv getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_ENUM);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0793
	 * @tc.name webgl_test_clearBufferiv22
	 * @tc.desc Test clearBufferiv.
	 */
	it('webgl_test_clearBufferiv22', 0, async function(done) {
		//initContext();
		console.info("webgltest into clearBufferiv");

		gl2.clearBufferiv(gl.DEPTH, 0, new Int32Array([0, 1, 0, 0.5]));

		let errorCode = gl.getError();
		console.info("webgltest clearBufferiv getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0794
	 * @tc.name webgl_test_clearBufferiv23
	 * @tc.desc Test clearBufferiv.
	 */
	it('webgl_test_clearBufferiv23', 0, async function(done) {
		//initContext();
		console.info("webgltest into clearBufferiv");

		gl2.clearBufferiv(gl.STENCIL, 0, new Int32Array([0, 1, 0, 0.5]));

		let errorCode = gl.getError();
		console.info("webgltest clearBufferiv getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0795
	 * @tc.name webgl_test_clearBufferiv24
	 * @tc.desc Test clearBufferiv.
	 */
	it('webgl_test_clearBufferiv24', 0, async function(done) {
		//initContext();
		console.info("webgltest into clearBufferiv");

		gl2.clearBufferiv(gl.DEPTH_STENCIL, 0, new Int32Array([0, 1, 0, 0.5]));

		let errorCode = gl.getError();
		console.info("webgltest clearBufferiv getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0796
	 * @tc.name webgl_test_clearBufferuiv23
	 * @tc.desc Test clearBufferuiv.
	 */
	it('webgl_test_clearBufferuiv23', 0, async function(done) {
		//initContext();
		console.info("webgltest into clearBufferuiv");

		gl2.clearBufferuiv(gl.DEPTH, 0, new Uint32Array([0, 1, 0, 0]));

		let errorCode = gl.getError();
		console.info("webgltest clearBufferuiv getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0797
	 * @tc.name webgl_test_clearBufferuiv24
	 * @tc.desc Test clearBufferuiv.
	 */
	it('webgl_test_clearBufferuiv24', 0, async function(done) {
		//initContext();
		console.info("webgltest into clearBufferuiv");

		gl2.clearBufferuiv(gl.STENCIL, 0, new Uint32Array([0, 1, 0, 0]));

		let errorCode = gl.getError();
		console.info("webgltest clearBufferuiv getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});



	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0798
	 * @tc.name webgl_test_clearBufferuiv25
	 * @tc.desc Test clearBufferuiv.
	 */
	it('webgl_test_clearBufferuiv25', 0, async function(done) {
		//initContext();
		console.info("webgltest into clearBufferuiv");

		gl2.clearBufferuiv(gl.DEPTH_STENCIL, 0, new Uint32Array([0, 1, 0, 0]));

		let errorCode = gl.getError();
		console.info("webgltest clearBufferuiv getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0799
	 * @tc.name webgl_test_clearBufferfi22
	 * @tc.desc Test clearBufferfi.
	 */
	it('webgl_test_clearBufferfi22', 0, async function(done) {
		//initContext();
		console.info("webgltest into clearBufferfi");

		gl2.clearBufferfi(gl.DEPTH, 0, 1.0, 0);

		let errorCode = gl.getError();
		console.info("webgltest clearBufferfi getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0800
	 * @tc.name webgl_test_clearBufferfi23
	 * @tc.desc Test clearBufferfi.
	 */
	it('webgl_test_clearBufferfi23', 0, async function(done) {
		//initContext();
		console.info("webgltest into clearBufferfi");

		gl2.clearBufferfi(gl.STENCIL, 0, 1.0, 0);

		let errorCode = gl.getError();
		console.info("webgltest clearBufferfi getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0801
	 * @tc.name webgl_test_clearBufferfi24
	 * @tc.desc Test clearBufferfi.
	 */
	it('webgl_test_clearBufferfi24', 0, async function(done) {
		//initContext();
		console.info("webgltest into clearBufferfi");

		gl2.clearBufferfi(gl.DEPTH_STENCIL, 0, 1.0, 0);

		let errorCode = gl.getError();
		console.info("webgltest clearBufferfi getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_FRAMEBUFFER_OPERATION);
		//deleteContext();
		done();
	});



	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0802
	 * @tc.name webgl_test_beginQuery22
	 * @tc.desc Test beginQuery.
	 */
	it('webgl_test_beginQuery22', 0, async function(done) {
		//initContext();
		console.info("webgltest into beginQuery");

		var query = gl2.createQuery();
		gl2.beginQuery(gl2.TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, query);

		const currentQuery = gl2.getQuery(gl2.TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, gl2
			.CURRENT_QUERY);
		const isQuery1 = gl2.isQuery(currentQuery);
		console.info("webgltest createQuery isQuery1: " + isQuery1);
		expect(isQuery1).assertEqual(true);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0803
	 * @tc.name webgl_test_beginQuery23
	 * @tc.desc Test beginQuery.
	 */
	it('webgl_test_beginQuery23', 0, async function(done) {
		//initContext();
		console.info("webgltest into beginQuery");

		var query = gl2.createQuery();
		gl2.beginQuery(0x8D6A, query);

		const currentQuery = gl2.getQuery(gl2.ANY_SAMPLES_PASSED_CONSERVATIVE, gl2.CURRENT_QUERY);
		//        const currentQuery = gl2.getQuery(0x8D6A, 0x8865);
		const isQuery1 = gl2.isQuery(currentQuery);
		console.info("webgltest createQuery isQuery1: " + isQuery1);
		expect(isQuery1).assertEqual(false);
		//deleteContext();
		done();
	});



	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0804
	 * @tc.name webgl_test_endQuery23
	 * @tc.desc Test endQuery.
	 */
	it('webgl_test_endQuery23', 0, async function(done) {
		//initContext();
		console.info("webgltest into endQuery");

		var query = gl2.createQuery();
		gl2.beginQuery(gl2.ANY_SAMPLES_PASSED_CONSERVATIVE, query);

		const currentQuery = gl2.getQuery(gl2.ANY_SAMPLES_PASSED_CONSERVATIVE, gl2.CURRENT_QUERY);
		const isQuery1 = gl2.isQuery(currentQuery);
		console.info("webgltest createQuery isQuery1: " + isQuery1);
		expect(isQuery1).assertEqual(false);

		gl2.endQuery(0x8C2F);

		const isQuery2 = gl2.isQuery(currentQuery);
		console.info("webgltest createQuery isQuery2: " + isQuery2);
		expect(isQuery2).assertEqual(false);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0805
	 * @tc.name webgl_test_endQuery24
	 * @tc.desc Test endQuery.
	 */
	it('webgl_test_endQuery24', 0, async function(done) {
		//initContext();
		console.info("webgltest into endQuery");

		var query = gl2.createQuery();
		gl2.beginQuery(gl2.TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, query);

		const currentQuery = gl2.getQuery(gl2.TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, gl2
			.CURRENT_QUERY);
		const isQuery1 = gl2.isQuery(currentQuery);
		console.info("webgltest createQuery isQuery1: " + isQuery1);
		expect(isQuery1).assertEqual(true);

		gl2.endQuery(0x8C2F);

		const isQuery2 = gl2.isQuery(currentQuery);
		console.info("webgltest createQuery isQuery2: " + isQuery2);
		expect(isQuery2).assertEqual(true);

		//deleteContext();
		done();
	});



	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0806
	 * @tc.name webgl_test_getQuery23
	 * @tc.desc Test endQuery.
	 */
	it('webgl_test_getQuery23', 0, async function(done) {
		//initContext();
		console.info("webgltest into endQuery");

		var query = gl2.createQuery();
		gl2.beginQuery(gl2.ANY_SAMPLES_PASSED_CONSERVATIVE, query);

		const currentQuery = gl2.getQuery(gl2.ANY_SAMPLES_PASSED_CONSERVATIVE, gl2.CURRENT_QUERY);
		const isQuery1 = gl2.isQuery(currentQuery);
		console.info("webgltest createQuery isQuery1: " + isQuery1);
		expect(isQuery1).assertEqual(false);

		gl2.endQuery(0x8C2F);

		const isQuery2 = gl2.isQuery(currentQuery);
		console.info("webgltest createQuery isQuery2: " + isQuery2);
		expect(isQuery2).assertEqual(false);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0807
	 * @tc.name webgl_test_getQuery24
	 * @tc.desc Test endQuery.
	 */
	it('webgl_test_getQuery24', 0, async function(done) {
		//initContext();
		console.info("webgltest into endQuery");

		var query = gl2.createQuery();
		gl2.beginQuery(gl2.TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, query);

		const currentQuery = gl2.getQuery(gl2.TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, gl2
			.CURRENT_QUERY);
		const isQuery1 = gl2.isQuery(currentQuery);
		console.info("webgltest createQuery isQuery1: " + isQuery1);
		expect(isQuery1).assertEqual(true);

		gl2.endQuery(0x8C2F);

		const isQuery2 = gl2.isQuery(currentQuery);
		console.info("webgltest createQuery isQuery2: " + isQuery2);
		expect(isQuery2).assertEqual(true);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0808
	 * @tc.name webgl_test_bindSampler23
	 * @tc.desc Test bindSampler.
	 */
	it('webgl_test_bindSampler23', 0, async function(done) {
		//initContext();
		console.info("webgltest into bindSampler");
		const sampler = gl2.createSampler();
		const isSampler1 = gl2.isSampler(sampler);
		console.info("webgltest createSampler isSampler1: " + isSampler1);
		expect(isSampler1).assertEqual(true);

		gl2.bindSampler(-1, sampler);
		const isSampler2 = gl2.isSampler(sampler);
		console.info("webgltest createSampler bindSampler isSampler2: " + isSampler2);
		expect(isSampler2).assertEqual(true);

		gl2.deleteSampler(sampler);
		const isSampler3 = gl2.isSampler(sampler);
		console.info("webgltest createSampler bindSampler deleteSampler isSampler3: " + isSampler3);
		expect(isSampler3).assertEqual(false);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0809
	 * @tc.name webgl_test_samplerParameterf24
	 * @tc.desc Test samplerParameterf.
	 */
	it('webgl_test_samplerParameterf24', 0, async function(done) {
		//initContext();
		console.info("webgltest into samplerParameterf");

		let sampler = gl2.createSampler();
		//        gl2.bindSampler(0, sampler);
		gl2.samplerParameterf(sampler, gl.TEXTURE_COMPARE_FUNC, gl.NEAREST);

		let errorCode = gl.getError();
		console.info("webgltest samplerParameterf getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		gl2.deleteSampler(sampler);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0810
	 * @tc.name webgl_test_samplerParameterf25
	 * @tc.desc Test samplerParameterf.
	 */
	it('webgl_test_samplerParameterf25', 0, async function(done) {
		//initContext();
		console.info("webgltest into samplerParameterf");

		let sampler = gl2.createSampler();
		//        gl2.bindSampler(0, sampler);
		gl2.samplerParameterf(sampler, gl.TEXTURE_COMPARE_MODE, gl.NEAREST);

		let errorCode = gl.getError();
		console.info("webgltest samplerParameterf getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		gl2.deleteSampler(sampler);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0811
	 * @tc.name webgl_test_samplerParameterf26
	 * @tc.desc Test samplerParameterf.
	 */
	it('webgl_test_samplerParameterf26', 0, async function(done) {
		//initContext();
		console.info("webgltest into samplerParameterf");

		let sampler = gl2.createSampler();
		//        gl2.bindSampler(0, sampler);
		gl2.samplerParameterf(sampler, gl.TEXTURE_MAX_LOD, gl.NEAREST);

		let errorCode = gl.getError();
		console.info("webgltest samplerParameterf getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		gl2.deleteSampler(sampler);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0812
	 * @tc.name webgl_test_samplerParameterf27
	 * @tc.desc Test samplerParameterf.
	 */
	it('webgl_test_samplerParameterf27', 0, async function(done) {
		//initContext();
		console.info("webgltest into samplerParameterf");

		let sampler = gl2.createSampler();
		//        gl2.bindSampler(0, sampler);
		gl2.samplerParameterf(sampler, gl.TEXTURE_MIN_FILTER, gl.NEAREST);

		let errorCode = gl.getError();
		console.info("webgltest samplerParameterf getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		gl2.deleteSampler(sampler);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0813
	 * @tc.name webgl_test_samplerParameterf28
	 * @tc.desc Test samplerParameterf.
	 */
	it('webgl_test_samplerParameterf28', 0, async function(done) {
		//initContext();
		console.info("webgltest into samplerParameterf");

		let sampler = gl2.createSampler();
		//        gl2.bindSampler(0, sampler);
		gl2.samplerParameterf(sampler, gl.TEXTURE_MIN_LOD, gl.NEAREST);

		let errorCode = gl.getError();
		console.info("webgltest samplerParameterf getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		gl2.deleteSampler(sampler);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0814
	 * @tc.name webgl_test_samplerParameterf29
	 * @tc.desc Test samplerParameterf.
	 */
	it('webgl_test_samplerParameterf29', 0, async function(done) {
		//initContext();
		console.info("webgltest into samplerParameterf");

		let sampler = gl2.createSampler();
		//        gl2.bindSampler(0, sampler);
		gl2.samplerParameterf(sampler, gl.TEXTURE_WRAP_R, gl.NEAREST);

		let errorCode = gl.getError();
		console.info("webgltest samplerParameterf getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		gl2.deleteSampler(sampler);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0815
	 * @tc.name webgl_test_samplerParameterf30
	 * @tc.desc Test samplerParameterf.
	 */
	it('webgl_test_samplerParameterf30', 0, async function(done) {
		//initContext();
		console.info("webgltest into samplerParameterf");

		let sampler = gl2.createSampler();
		//        gl2.bindSampler(0, sampler);
		gl2.samplerParameterf(sampler, gl.TEXTURE_WRAP_S, gl.NEAREST);

		let errorCode = gl.getError();
		console.info("webgltest samplerParameterf getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_ENUM);
		gl2.deleteSampler(sampler);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0816
	 * @tc.name webgl_test_samplerParameterf31
	 * @tc.desc Test samplerParameterf.
	 */
	it('webgl_test_samplerParameterf31', 0, async function(done) {
		//initContext();
		console.info("webgltest into samplerParameterf");

		let sampler = gl2.createSampler();
		//        gl2.bindSampler(0, sampler);
		gl2.samplerParameterf(sampler, gl.TEXTURE_WRAP_T, gl.NEAREST);

		let errorCode = gl.getError();
		console.info("webgltest samplerParameterf getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_ENUM);
		gl2.deleteSampler(sampler);
		//deleteContext();
		done();
	});



	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0817
	 * @tc.name webgl_test_samplerParameteri24
	 * @tc.desc Test samplerParameteri.
	 */
	it('webgl_test_samplerParameteri24', 0, async function(done) {
		//initContext();
		console.info("webgltest into samplerParameteri");
		let sampler = gl2.createSampler();
		gl2.samplerParameteri(sampler, gl.TEXTURE_COMPARE_FUNC, gl.NEAREST);

		let errorCode = gl.getError();
		console.info("webgltest samplerParameteri getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		gl2.deleteSampler(sampler);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0818
	 * @tc.name webgl_test_samplerParameteri25
	 * @tc.desc Test samplerParameteri.
	 */
	it('webgl_test_samplerParameteri25', 0, async function(done) {
		//initContext();
		console.info("webgltest into samplerParameteri");
		let sampler = gl2.createSampler();
		gl2.samplerParameteri(sampler, gl.TEXTURE_COMPARE_MODE, gl.NEAREST);

		let errorCode = gl.getError();
		console.info("webgltest samplerParameteri getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		gl2.deleteSampler(sampler);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0819
	 * @tc.name webgl_test_samplerParameteri26
	 * @tc.desc Test samplerParameteri.
	 */
	it('webgl_test_samplerParameteri26', 0, async function(done) {
		//initContext();
		console.info("webgltest into samplerParameteri");
		let sampler = gl2.createSampler();
		gl2.samplerParameteri(sampler, gl.TEXTURE_MAX_LOD, gl.NEAREST);

		let errorCode = gl.getError();
		console.info("webgltest samplerParameteri getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		gl2.deleteSampler(sampler);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0820
	 * @tc.name webgl_test_samplerParameteri27
	 * @tc.desc Test samplerParameteri.
	 */
	it('webgl_test_samplerParameteri27', 0, async function(done) {
		//initContext();
		console.info("webgltest into samplerParameteri");
		let sampler = gl2.createSampler();
		gl2.samplerParameteri(sampler, gl.TEXTURE_MIN_FILTER, gl.NEAREST);

		let errorCode = gl.getError();
		console.info("webgltest samplerParameteri getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		gl2.deleteSampler(sampler);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0821
	 * @tc.name webgl_test_samplerParameteri28
	 * @tc.desc Test samplerParameteri.
	 */
	it('webgl_test_samplerParameteri28', 0, async function(done) {
		//initContext();
		console.info("webgltest into samplerParameteri");
		let sampler = gl2.createSampler();
		gl2.samplerParameteri(sampler, gl.TEXTURE_MIN_LOD, gl.NEAREST);

		let errorCode = gl.getError();
		console.info("webgltest samplerParameteri getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		gl2.deleteSampler(sampler);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0822
	 * @tc.name webgl_test_samplerParameteri29
	 * @tc.desc Test samplerParameteri.
	 */
	it('webgl_test_samplerParameteri29', 0, async function(done) {
		//initContext();
		console.info("webgltest into samplerParameteri");
		let sampler = gl2.createSampler();
		gl2.samplerParameteri(sampler, gl.TEXTURE_WRAP_R, gl.NEAREST);

		let errorCode = gl.getError();
		console.info("webgltest samplerParameteri getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		gl2.deleteSampler(sampler);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0823
	 * @tc.name webgl_test_samplerParameteri211
	 * @tc.desc Test samplerParameteri.
	 */
	it('webgl_test_samplerParameteri211', 0, async function(done) {
		//initContext();
		console.info("webgltest into samplerParameteri");
		let sampler = gl2.createSampler();
		gl2.samplerParameteri(sampler, gl.TEXTURE_WRAP_S, gl.NEAREST);

		let errorCode = gl.getError();
		console.info("webgltest samplerParameteri getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_ENUM);
		gl2.deleteSampler(sampler);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0824
	 * @tc.name webgl_test_samplerParameteri212
	 * @tc.desc Test samplerParameteri.
	 */
	it('webgl_test_samplerParameteri212', 0, async function(done) {
		//initContext();
		console.info("webgltest into samplerParameteri");
		let sampler = gl2.createSampler();
		gl2.samplerParameteri(sampler, gl.TEXTURE_WRAP_T, gl.NEAREST);

		let errorCode = gl.getError();
		console.info("webgltest samplerParameteri getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_ENUM);
		gl2.deleteSampler(sampler);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0825
	 * @tc.name testGetShaderPrecisionFormat
	 * @tc.desc Test getShaderPrecisionFormat.
	 */
	it('testGetShaderPrecisionFormat', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getShaderPrecisionFormat test start ...' + JSON.stringify(gl));
		const shaderPrecisionFormat = gl.getShaderPrecisionFormat(gl.VERTEX_SHADER, gl.HIGH_INT);
		console.info('shaderPrecisionFormat' + JSON.stringify(shaderPrecisionFormat));
		const type = ((typeof shaderPrecisionFormat) === "object")
		expect(type).assertEqual(true);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0826
	 * @tc.name testGetActiveUniformBlockParameter_1
	 * @tc.desc Test getActiveUniformBlockParameter.
	 */
	it('testGetActiveUniformBlockParameter_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 getActiveUniformBlockParameter test start ...' + JSON.stringify(
			gl2));
		const program = globalFunction();
		const uniformLocation = gl.getUniformLocation(program, "a_Position");
		const int32Array = new Int32Array([1, 2]);
		gl2.uniform1iv(uniformLocation, int32Array);
		const blockIndex = gl2.getUniformBlockIndex(program, 'a_Position');
		gl2.uniformBlockBinding(program, blockIndex, 1);
		const blockSize = gl2.getActiveUniformBlockParameter(program, blockIndex, gl2
			.UNIFORM_BLOCK_BINDING);
		console.info("getActiveUniformBlockParameter blockSize" + blockSize);
		const errorCode = gl.getError();
		console.info("jsWebGL getFramebufferAttachmentParameter errorCode: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_VALUE);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0827
	 * @tc.name testGetActiveUniformBlockParameter_2
	 * @tc.desc Test getActiveUniformBlockParameter.
	 */
	it('testGetActiveUniformBlockParameter_2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 getActiveUniformBlockParameter test start ...' + JSON.stringify(
			gl2));
		const program = globalFunction();
		const uniformLocation = gl.getUniformLocation(program, "a_Position");
		const int32Array = new Int32Array([1, 2]);
		gl2.uniform1iv(uniformLocation, int32Array);
		const blockIndex = gl2.getUniformBlockIndex(program, 'a_Position');
		gl2.uniformBlockBinding(program, blockIndex, 1);
		const blockSize = gl2.getActiveUniformBlockParameter(program, blockIndex, gl2
			.UNIFORM_BLOCK_ACTIVE_UNIFORMS);
		console.info("getActiveUniformBlockParameter blockSize" + blockSize);
		const errorCode = gl.getError();
		console.info("jsWebGL getFramebufferAttachmentParameter errorCode: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_VALUE);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0828
	 * @tc.name testGetActiveUniformBlockParameter_3
	 * @tc.desc Test getActiveUniformBlockParameter.
	 */
	it('testGetActiveUniformBlockParameter_3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 getActiveUniformBlockParameter test start ...' + JSON.stringify(
			gl2));
		const program = globalFunction();
		const uniformLocation = gl.getUniformLocation(program, "a_Position");
		const int32Array = new Int32Array([1, 2]);
		gl2.uniform1iv(uniformLocation, int32Array);
		const blockIndex = gl2.getUniformBlockIndex(program, 'a_Position');
		gl2.uniformBlockBinding(program, blockIndex, 1);
		const blockSize = gl2.getActiveUniformBlockParameter(program, blockIndex,
			gl2.UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES);
		console.info("getActiveUniformBlockParameter blockSize" + blockSize);
		const errorCode = gl.getError();
		console.info("jsWebGL getFramebufferAttachmentParameter errorCode: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_VALUE);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0829
	 * @tc.name testGetActiveUniformBlockParameter_4
	 * @tc.desc Test getActiveUniformBlockParameter.
	 */
	it('testGetActiveUniformBlockParameter_4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 getActiveUniformBlockParameter test start ...' + JSON.stringify(
			gl2));
		const program = globalFunction();
		const uniformLocation = gl.getUniformLocation(program, "a_Position");
		const int32Array = new Int32Array([1, 2]);
		gl2.uniform1iv(uniformLocation, int32Array);
		const blockIndex = gl2.getUniformBlockIndex(program, 'a_Position');
		gl2.uniformBlockBinding(program, blockIndex, 1);
		const blockSize = gl2.getActiveUniformBlockParameter(program, blockIndex,
			gl2.UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER);
		console.info("getActiveUniformBlockParameter blockSize" + blockSize);
		const errorCode = gl.getError();
		console.info("jsWebGL getFramebufferAttachmentParameter errorCode: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_VALUE);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0830
	 * @tc.name testGetIndexedParameter_01_01
	 * @tc.desc Test getIndexedParameter.
	 */
	it('testGetIndexedParameter_01_01', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetIndexedParameter_01_01 test start ...66');
			console.info('jsWebGL testGetIndexedParameter_01 test start ...' + JSON.stringify(gl));
			const reurGLsizeiptr = gl2.getIndexedParameter(-gl.TRANSFORM_FEEDBACK_BUFFER_SIZE, -0);
			const typeGLsizeiptr = typeof(reurGLsizeiptr);
			expect(typeGLsizeiptr == 'String').assertEqual(false);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetIndexedParameter_01 has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0831
	 * @tc.name testGetIndexedParameter_02_01
	 * @tc.desc Test getIndexedParameter.
	 */
	it('testGetIndexedParameter_02_01', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetIndexedParameter_02_01 test start ...66');
			console.info('jsWebGL testGetIndexedParameter_02 test start ...' + JSON.stringify(gl));
			const retuGLintptr = gl2.getIndexedParameter(-gl.TRANSFORM_FEEDBACK_BUFFER_START, -1);
			const typeGLintptr = typeof(retuGLintptr);
			expect(typeGLintptr == 'String').assertEqual(false);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetIndexedParameter_02 has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0832
	 * @tc.name testGetIndexedParameter_03
	 * @tc.desc Test getIndexedParameter.
	 */
	it('testGetIndexedParameter_03_01', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetIndexedParameter_03_01 test start ...66');
			console.info('jsWebGL testGetIndexedParameter_03 test start ...' + JSON.stringify(gl));
			const WebGLBufferobject = gl2.getIndexedParameter(gl.UNIFORM_BUFFER_BINDING, -0);
			const isBuffer = gl.isBuffer(WebGLBufferobject);
			expect(isBuffer).assertEqual(false);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetIndexedParameter_03 has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0833
	 * @tc.name testGetIndexedParameter_04
	 * @tc.desc Test getIndexedParameter.
	 */
	it('testGetIndexedParameter_04_01', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetIndexedParameter_04_01 test start ...66');
			console.info('jsWebGL testGetIndexedParameter_04 test start ...' + JSON.stringify(gl));
			const reurGLsizeiptr = gl2.getIndexedParameter(-gl.UNIFORM_BUFFER_SIZE, null);
			const typeGLsizeiptr = typeof(reurGLsizeiptr);
			expect(typeGLsizeiptr == 'String').assertEqual(false);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetIndexedParameter_04 has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0834
	 * @tc.name testGetIndexedParameter_05_01
	 * @tc.desc Test getIndexedParameter.
	 */
	it('testGetIndexedParameter_05_01', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetIndexedParameter_05 test start ...66');
			console.info('jsWebGL testGetIndexedParameter_05 test start ...' + JSON.stringify(gl));
			const retuGLintptr = gl2.getIndexedParameter(-gl.UNIFORM_BUFFER_START, -0);
			const typeGLintptr = typeof(retuGLintptr);
			expect(typeGLintptr == 'String').assertEqual(false);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetIndexedParameter_05 has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0835
	 * @tc.name testGetUniformIndices_01_01
	 * @tc.desc Test getUniformIndices.
	 */
	it('testGetUniformIndices_01_01', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetUniformIndices_01_01 test start ...66');
			console.info('jsWebGL testGetUniformIndices test start ...' + JSON.stringify(gl));
			const program = createProgram(gl);
			const retuGLintptr = gl2.getUniformIndices(program, ['-1', '-1', '-1']);
			console.log("testGetUniformIndices_01_01 has failed for " + retuGLintptr)
			const int32list = [-1, -1, -1];
			console.log("testGetUniformIndices_01_01 has failed for " + int32list)
			//判断数据类型是否正确
			expect(int32list == 'String').assertEqual(false);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetUniformIndices_01_01 has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0836
	 * @tc.name testGetUniformIndices_01_02
	 * @tc.desc Test getUniformIndices.
	 */
	it('testGetUniformIndices_01_02', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetUniformIndices_01_02 test start ...66');
			const program = createProgram(gl);
			const retuGLintptr = gl2.getUniformIndices(program, ['-0', '-0', '-0']);
			console.log("testGetUniformIndices_01_02 has failed for " + retuGLintptr)
			const int32list = [-1, -1, -1];
			console.log("testGetUniformIndices_01_02 has failed for " + int32list)
			//判断数据类型是否正确
			expect(int32list == 'String').assertEqual(false);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetUniformIndices_01_02 has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0837
	 * @tc.name testGetUniformIndices_01_03
	 * @tc.desc Test getUniformIndices.
	 */
	it('testGetUniformIndices_01_03', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetUniformIndices_01_03 test start ...66');
			const program = createProgram(gl);
			const retuGLintptr = gl2.getUniformIndices(program, [null, null, null]);
			console.log("testGetUniformIndices_01_03 has failed for " + retuGLintptr)
			const int32list = [-1, -1, -1];
			console.log("testGetUniformIndices_01_03 has failed for " + int32list)
			//判断数据类型是否正确
			expect(int32list == 'String').assertEqual(false);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetUniformIndices_01_03 has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0838
	 * @tc.name testPixelStorei_1_02
	 * @tc.desc Test pixelStorei.
	 */
	it('testPixelStorei_1_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testPixelStorei_1_02 test start ...66');
		var tex = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, tex);
		gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, 4);
		const unpackValue = gl.getParameter(gl.UNPACK_FLIP_Y_WEBGL);
		console.info("unpackValue: " + unpackValue);
		const pixelStoreiError = gl.getError();
		console.info("pixelStoreiError: " + pixelStoreiError);
		expect(pixelStoreiError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0839
	 * @tc.name testPixelStorei_1_03
	 * @tc.desc Test pixelStorei.
	 */
	it('testPixelStorei_1_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testPixelStorei_1_03 test start ...66');
		var tex = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, tex);
		gl.pixelStorei(gl.UNPACK_PREMULTIPLY_ALPHA_WEBGL, 4);
		const unpackValue = gl.getParameter(gl.UNPACK_PREMULTIPLY_ALPHA_WEBGL);
		console.info("unpackValue: " + unpackValue);
		const pixelStoreiError = gl.getError();
		console.info("pixelStoreiError: " + pixelStoreiError);
		expect(pixelStoreiError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0840
	 * @tc.name testPixelStorei_1_04
	 * @tc.desc Test pixelStorei.
	 */
	it('testPixelStorei_1_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testPixelStorei_1_04 test start ...66');
		var tex = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, tex);
		gl.pixelStorei(gl.UNPACK_PREMULTIPLY_ALPHA_WEBGL, 4);
		const unpackValue = gl.getParameter(gl.UNPACK_COLORSPACE_CONVERSION_WEBGL);
		console.info("unpackValue: " + unpackValue);
		const pixelStoreiError = gl.getError();
		console.info("pixelStoreiError: " + pixelStoreiError);
		expect(pixelStoreiError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0841
	 * @tc.name testPixelStorei_1_05
	 * @tc.desc Test pixelStorei.
	 */
	it('testPixelStorei_1_05', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testPixelStorei_1_05 test start ...66');
		var tex = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, tex);
		gl.pixelStorei(gl2.PACK_ROW_LENGTH, 4);
		const unpackValue = gl.getParameter(gl2.PACK_ROW_LENGTH);
		console.info("unpackValue: " + unpackValue);
		const pixelStoreiError = gl.getError();
		console.info("pixelStoreiError: " + pixelStoreiError);
		expect(pixelStoreiError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0842
	 * @tc.name testPixelStorei_1_06
	 * @tc.desc Test pixelStorei.
	 */
	it('testPixelStorei_1_06', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testPixelStorei_1_06 test start ...66');
		var tex = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, tex);
		gl.pixelStorei(gl2.PACK_SKIP_PIXELS, 4);
		const unpackValue = gl.getParameter(gl2.PACK_SKIP_PIXELS);
		console.info("unpackValue: " + unpackValue);
		const pixelStoreiError = gl.getError();
		console.info("pixelStoreiError: " + pixelStoreiError);
		expect(pixelStoreiError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0843
	 * @tc.name testPixelStorei_1_07
	 * @tc.desc Test pixelStorei.
	 */
	it('testPixelStorei_1_07', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testPixelStorei_1_07 test start ...66');
		var tex = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, tex);
		gl.pixelStorei(gl2.PACK_SKIP_ROWS, 4);
		const unpackValue = gl.getParameter(gl2.PACK_SKIP_ROWS);
		console.info("unpackValue: " + unpackValue);
		const pixelStoreiError = gl.getError();
		console.info("pixelStoreiError: " + pixelStoreiError);
		expect(pixelStoreiError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0844
	 * @tc.name testPixelStorei_1_08
	 * @tc.desc Test pixelStorei.
	 */
	it('testPixelStorei_1_08', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testPixelStorei_1_08 test start ...66');
		var tex = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, tex);
		gl.pixelStorei(gl2.UNPACK_ROW_LENGTH, 4);
		const unpackValue = gl.getParameter(gl2.UNPACK_ROW_LENGTH);
		console.info("unpackValue: " + unpackValue);
		const pixelStoreiError = gl.getError();
		console.info("pixelStoreiError: " + pixelStoreiError);
		expect(pixelStoreiError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0845
	 * @tc.name testPixelStorei_1_09
	 * @tc.desc Test pixelStorei.
	 */
	it('testPixelStorei_1_09', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testPixelStorei_1_09 test start ...66');
		var tex = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, tex);
		gl.pixelStorei(gl2.UNPACK_IMAGE_HEIGHT, 4);
		const unpackValue = gl.getParameter(gl2.UNPACK_IMAGE_HEIGHT);
		console.info("unpackValue: " + unpackValue);
		const pixelStoreiError = gl.getError();
		console.info("pixelStoreiError: " + pixelStoreiError);
		expect(pixelStoreiError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0846
	 * @tc.name testPixelStorei_1_10
	 * @tc.desc Test pixelStorei.
	 */
	it('testPixelStorei_1_10', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testPixelStorei_1_10 test start ...66');
		var tex = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, tex);
		gl.pixelStorei(gl2.UNPACK_SKIP_PIXELS, 4);
		const unpackValue = gl.getParameter(gl2.UNPACK_SKIP_PIXELS);
		console.info("unpackValue: " + unpackValue);
		const pixelStoreiError = gl.getError();
		console.info("pixelStoreiError: " + pixelStoreiError);
		expect(pixelStoreiError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0847
	 * @tc.name testPixelStorei_1_11
	 * @tc.desc Test pixelStorei.
	 */
	it('testPixelStorei_1_11', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testPixelStorei_1_11 test start ...66');
		var tex = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, tex);
		gl.pixelStorei(gl2.UNPACK_SKIP_ROWS, 4);
		const unpackValue = gl.getParameter(gl2.UNPACK_SKIP_ROWS);
		console.info("unpackValue: " + unpackValue);
		const pixelStoreiError = gl.getError();
		console.info("pixelStoreiError: " + pixelStoreiError);
		expect(pixelStoreiError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0848
	 * @tc.name testPixelStorei_1_12
	 * @tc.desc Test pixelStorei.
	 */
	it('testPixelStorei_1_12', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testPixelStorei_1_12 test start ...66');
		var tex = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, tex);
		gl.pixelStorei(gl2.UNPACK_SKIP_IMAGES, 4);
		const unpackValue = gl.getParameter(gl2.UNPACK_SKIP_IMAGES);
		console.info("unpackValue: " + unpackValue);
		const pixelStoreiError = gl.getError();
		console.info("pixelStoreiError: " + pixelStoreiError);
		expect(pixelStoreiError).assertEqual(0);
		done();
	})


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0849
	 * @tc.name testBufferData_80
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_80', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_80 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, 1024, gl.STATIC_DRAW);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0850
	 * @tc.name testBufferData_81
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_81', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_81 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.COPY_READ_BUFFER, buffer);
		gl.bufferData(gl2.COPY_READ_BUFFER, 1024, gl.STATIC_DRAW);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0851
	 * @tc.name testBufferData_82
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_82', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_82 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.COPY_WRITE_BUFFER, buffer);
		gl.bufferData(gl2.COPY_WRITE_BUFFER, 1024, gl.STATIC_DRAW);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0852
	 * @tc.name testBufferData_83
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_83', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_83 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.TRANSFORM_FEEDBACK_BUFFER, buffer);
		gl.bufferData(gl2.TRANSFORM_FEEDBACK_BUFFER, 1024, gl.STATIC_DRAW);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0853
	 * @tc.name testBufferData_84
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_84', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_84 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.UNIFORM_BUFFER, buffer);
		gl.bufferData(gl2.UNIFORM_BUFFER, 1024, gl.STATIC_DRAW);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0854
	 * @tc.name testBufferData_85
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_85', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_85 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.PIXEL_PACK_BUFFER, buffer);
		gl.bufferData(gl2.PIXEL_PACK_BUFFER, 1024, gl.STATIC_DRAW);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0855
	 * @tc.name testBufferData_86
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_86', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_86 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.PIXEL_UNPACK_BUFFER, buffer);
		gl.bufferData(gl2.PIXEL_UNPACK_BUFFER, 1024, gl.STATIC_DRAW);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0856
	 * @tc.name testBufferData_80_01
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_80_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_80_01 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, 1024, gl.DYNAMIC_DRAW);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0857
	 * @tc.name testBufferData_81_01
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_81_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_81_01 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.COPY_READ_BUFFER, buffer);
		gl.bufferData(gl2.COPY_READ_BUFFER, 1024, gl.STREAM_DRAW);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0858
	 * @tc.name testBufferData_82_01
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_82_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_82_01 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.COPY_WRITE_BUFFER, buffer);
		gl.bufferData(gl2.COPY_WRITE_BUFFER, 1024, gl2.STATIC_READ);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0859
	 * @tc.name testBufferData_83_01
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_83_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_83_01 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.TRANSFORM_FEEDBACK_BUFFER, buffer);
		gl.bufferData(gl2.TRANSFORM_FEEDBACK_BUFFER, 1024, gl2.DYNAMIC_READ);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0860
	 * @tc.name testBufferData_84_01
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_84_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_84_01 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.UNIFORM_BUFFER, buffer);
		gl.bufferData(gl2.UNIFORM_BUFFER, 1024, gl2.STREAM_READ);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0861
	 * @tc.name testBufferData_85_01
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_85_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_85_01 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.PIXEL_PACK_BUFFER, buffer);
		gl.bufferData(gl2.PIXEL_PACK_BUFFER, 1024, gl2.STATIC_COPY);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0862
	 * @tc.name testBufferData_86_01
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_86_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_86_01 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.PIXEL_UNPACK_BUFFER, buffer);
		gl.bufferData(gl2.PIXEL_UNPACK_BUFFER, 1024, gl2.DYNAMIC_COPY);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0863
	 * @tc.name testBufferData_87_01
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_87_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_87_01 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.PIXEL_UNPACK_BUFFER, buffer);
		gl.bufferData(gl2.PIXEL_UNPACK_BUFFER, 1024, gl2.STREAM_COPY);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0864
	 * @tc.name testBufferData_80_1
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_80_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_80_1 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new ArrayBuffer(8), gl.STATIC_DRAW);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0865
	 * @tc.name testBufferData_81_1
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_81_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_81_1 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.COPY_READ_BUFFER, buffer);
		gl.bufferData(gl2.COPY_READ_BUFFER, new ArrayBuffer(8), gl.STATIC_DRAW);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0866
	 * @tc.name testBufferData_82_1
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_82_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_82_1 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.COPY_WRITE_BUFFER, buffer);
		gl.bufferData(gl2.COPY_WRITE_BUFFER, new ArrayBuffer(8), gl.STATIC_DRAW);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0867
	 * @tc.name testBufferData_83_1
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_83_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_83_1 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.TRANSFORM_FEEDBACK_BUFFER, buffer);
		gl.bufferData(gl2.TRANSFORM_FEEDBACK_BUFFER, new ArrayBuffer(8), gl.STATIC_DRAW);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0868
	 * @tc.name testBufferData_84_1
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_84_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_84_1 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.UNIFORM_BUFFER, buffer);
		gl.bufferData(gl2.UNIFORM_BUFFER, new ArrayBuffer(8), gl.STATIC_DRAW);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0869
	 * @tc.name testBufferData_85_1
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_85_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_85_1 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.PIXEL_PACK_BUFFER, buffer);
		gl.bufferData(gl2.PIXEL_PACK_BUFFER, new ArrayBuffer(8), gl.STATIC_DRAW);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0870
	 * @tc.name testBufferData_86_1
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_86_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_86_1 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.PIXEL_UNPACK_BUFFER, buffer);
		gl.bufferData(gl2.PIXEL_UNPACK_BUFFER, new ArrayBuffer(8), gl.STATIC_DRAW);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0871
	 * @tc.name testBufferData_80_1_01
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_80_1_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_80_1_01 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new ArrayBuffer(8), gl.DYNAMIC_DRAW);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0872
	 * @tc.name testBufferData_81_01
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_81_1_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_81_1_01 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.COPY_READ_BUFFER, buffer);
		gl.bufferData(gl2.COPY_READ_BUFFER, new ArrayBuffer(8), gl.STREAM_DRAW);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0873
	 * @tc.name testBufferData_82_1_01
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_82_1_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_82_1_01 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.COPY_WRITE_BUFFER, buffer);
		gl.bufferData(gl2.COPY_WRITE_BUFFER, new ArrayBuffer(8), gl2.STATIC_READ);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0874
	 * @tc.name testBufferData_83_1_01
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_83_1_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_83_1_01 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.TRANSFORM_FEEDBACK_BUFFER, buffer);
		gl.bufferData(gl2.TRANSFORM_FEEDBACK_BUFFER, new ArrayBuffer(8), gl2.DYNAMIC_READ);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0875
	 * @tc.name testBufferData_84_1_01
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_84_1_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_84_1_01 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.UNIFORM_BUFFER, buffer);
		gl.bufferData(gl2.UNIFORM_BUFFER, new ArrayBuffer(8), gl2.STREAM_READ);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0876
	 * @tc.name testBufferData_85_1_01
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_85_1_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_85_1_01 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.PIXEL_PACK_BUFFER, buffer);
		gl.bufferData(gl2.PIXEL_PACK_BUFFER, new ArrayBuffer(8), gl2.STATIC_COPY);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0877
	 * @tc.name testBufferData_86_1_01
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_86_1_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_86_1_01 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.PIXEL_UNPACK_BUFFER, buffer);
		gl.bufferData(gl2.PIXEL_UNPACK_BUFFER, new ArrayBuffer(8), gl2.DYNAMIC_COPY);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0878
	 * @tc.name testBufferData_87_1_01
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_87_1_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_87_1_01 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.PIXEL_UNPACK_BUFFER, buffer);
		gl.bufferData(gl2.PIXEL_UNPACK_BUFFER, new ArrayBuffer(8), gl2.STREAM_COPY);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0879
	 * @tc.name testBufferSubData_1_02
	 * @tc.desc Test bufferSubData.
	 */
	it('testBufferSubData_1_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferSubData_1_02 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ARRAY_BUFFER, 1024, gl.STATIC_DRAW);
		gl.bufferSubData(gl.ARRAY_BUFFER, 512, new ArrayBuffer(8));
		const bufferSubDataError = gl.getError();
		console.info("bufferSubDataError: " + bufferSubDataError);
		expect(bufferSubDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0880
	 * @tc.name testBufferSubData_1_03
	 * @tc.desc Test bufferSubData.
	 */
	it('testBufferSubData_1_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferSubData_1_03 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ARRAY_BUFFER, 1024, gl.STATIC_DRAW);
		gl.bufferSubData(gl.ELEMENT_ARRAY_BUFFER, 512, new ArrayBuffer(8));
		const bufferSubDataError = gl.getError();
		console.info("bufferSubDataError: " + bufferSubDataError);
		expect(bufferSubDataError).assertEqual(gl.INVALID_VALUE);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0881
	 * @tc.name testBufferSubData_1_04
	 * @tc.desc Test bufferSubData.
	 */
	it('testBufferSubData_1_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferSubData_1_04 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ARRAY_BUFFER, 1024, gl.STATIC_DRAW);
		gl.bufferSubData(gl2.COPY_WRITE_BUFFER, 512, new ArrayBuffer(8));
		const bufferSubDataError = gl.getError();
		console.info("bufferSubDataError: " + bufferSubDataError);
		expect(bufferSubDataError).assertEqual(gl.INVALID_VALUE);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0882
	 * @tc.name testBufferSubData_1_05
	 * @tc.desc Test bufferSubData.
	 */
	it('testBufferSubData_1_05', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferSubData_1_05 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ARRAY_BUFFER, 1024, gl.STATIC_DRAW);
		gl.bufferSubData(gl2.TRANSFORM_FEEDBACK_BUFFER, 512, new ArrayBuffer(8));
		const bufferSubDataError = gl.getError();
		console.info("bufferSubDataError: " + bufferSubDataError);
		expect(bufferSubDataError).assertEqual(gl.INVALID_VALUE);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0883
	 * @tc.name testBufferSubData_1_06
	 * @tc.desc Test bufferSubData.
	 */
	it('testBufferSubData_1_06', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferSubData_1_06 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ARRAY_BUFFER, 1024, gl.STATIC_DRAW);
		gl.bufferSubData(gl2.UNIFORM_BUFFER, 512, new ArrayBuffer(8));
		const bufferSubDataError = gl.getError();
		console.info("bufferSubDataError: " + bufferSubDataError);
		expect(bufferSubDataError).assertEqual(gl.INVALID_VALUE);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0884
	 * @tc.name testBufferSubData_1_07
	 * @tc.desc Test bufferSubData.
	 */
	it('testBufferSubData_1_07', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferSubData_1_07 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ARRAY_BUFFER, 1024, gl.STATIC_DRAW);
		gl.bufferSubData(gl2.PIXEL_PACK_BUFFER, 512, new ArrayBuffer(8));
		const bufferSubDataError = gl.getError();
		console.info("bufferSubDataError: " + bufferSubDataError);
		expect(bufferSubDataError).assertEqual(gl.INVALID_VALUE);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0885
	 * @tc.name testBufferSubData_1_08
	 * @tc.desc Test bufferSubData.
	 */
	it('testBufferSubData_1_08', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferSubData_1_08 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ARRAY_BUFFER, 1024, gl.STATIC_DRAW);
		gl.bufferSubData(gl2.PIXEL_UNPACK_BUFFER, 512, new ArrayBuffer(8));
		const bufferSubDataError = gl.getError();
		console.info("bufferSubDataError: " + bufferSubDataError);
		expect(bufferSubDataError).assertEqual(gl.INVALID_VALUE);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0886
	 * @tc.name testCompressedTexImage2D_1_01
	 * @tc.desc Test compressedTexImage2D.
	 */
	it('testCompressedTexImage2D_1_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCompressedTexImage2D_1_01 test start ...66');
		var texture = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, texture);
		gl.compressedTexImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_X, 0, 0x83F3, 512, 512, 0, 0);
		const compressedTexImage2DError = gl.getError();
		console.info("compressedTexImage2DError: " + compressedTexImage2DError);
		expect(compressedTexImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0887
	 * @tc.name testCompressedTexImage2D_1_02
	 * @tc.desc Test compressedTexImage2D.
	 */
	it('testCompressedTexImage2D_1_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCompressedTexImage2D_1_02 test start ...66');
		var texture = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, texture);
		gl.compressedTexImage2D(gl.TEXTURE_CUBE_MAP_NEGATIVE_X, 0, 0x83F3, 512, 512, 0, 0);
		const compressedTexImage2DError = gl.getError();
		console.info("compressedTexImage2DError: " + compressedTexImage2DError);
		expect(compressedTexImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0888
	 * @tc.name testCompressedTexImage2D_1_03
	 * @tc.desc Test compressedTexImage2D.
	 */
	it('testCompressedTexImage2D_1_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCompressedTexImage2D_1_03 test start ...66');
		var texture = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, texture);
		gl.compressedTexImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_Y, 0, 0x83F3, 512, 512, 0, 0);
		const compressedTexImage2DError = gl.getError();
		console.info("compressedTexImage2DError: " + compressedTexImage2DError);
		expect(compressedTexImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0889
	 * @tc.name testCompressedTexImage2D_1_04
	 * @tc.desc Test compressedTexImage2D.
	 */
	it('testCompressedTexImage2D_1_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCompressedTexImage2D_1_04 test start ...66');
		var texture = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, texture);
		gl.compressedTexImage2D(gl.TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, 0x83F3, 512, 512, 0, 0);
		const compressedTexImage2DError = gl.getError();
		console.info("compressedTexImage2DError: " + compressedTexImage2DError);
		expect(compressedTexImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0890
	 * @tc.name testCompressedTexImage2D_1_05
	 * @tc.desc Test compressedTexImage2D.
	 */
	it('testCompressedTexImage2D_1_05', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCompressedTexImage2D_1_05 test start ...66');
		var texture = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, texture);
		gl.compressedTexImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_Z, 0, 0x83F3, 512, 512, 0, 0);
		const compressedTexImage2DError = gl.getError();
		console.info("compressedTexImage2DError: " + compressedTexImage2DError);
		expect(compressedTexImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0891
	 * @tc.name testCompressedTexImage2D_1_06
	 * @tc.desc Test compressedTexImage2D.
	 */
	it('testCompressedTexImage2D_1_06', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCompressedTexImage2D_1_06 test start ...66');
		var texture = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, texture);
		gl.compressedTexImage2D(gl.TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, 0x83F3, 512, 512, 0, 0);
		const compressedTexImage2DError = gl.getError();
		console.info("compressedTexImage2DError: " + compressedTexImage2DError);
		expect(compressedTexImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0892
	 * @tc.name testCompressedTexSubImage2D_1_01
	 * @tc.desc Test compressedTexSubImage2D.
	 */
	it('testCompressedTexSubImage2D_1_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCompressedTexSubImage2D_1_01 test start ...66');
		var texture = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, texture);
		gl.compressedTexImage2D(gl.TEXTURE_2D, 0, 0x83F3, 512, 512, 0, 0);
		gl.compressedTexSubImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_X, 0, 256, 256, 512, 512, 0x83F3,
			gl.PIXEL_UNPACK_BUFFER, 0);
		const compressedTexSubImage2DError = gl.getError();
		console.info("compressedTexSubImage2DError: " + compressedTexSubImage2DError);
		expect(compressedTexSubImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0893
	 * @tc.name testCompressedTexSubImage2D_1_02
	 * @tc.desc Test compressedTexSubImage2D.
	 */
	it('testCompressedTexSubImage2D_1_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCompressedTexSubImage2D_1_02 test start ...66');
		var texture = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, texture);
		gl.compressedTexImage2D(gl.TEXTURE_2D, 0, 0x83F3, 512, 512, 0, 0);
		gl.compressedTexSubImage2D(gl.TEXTURE_CUBE_MAP_NEGATIVE_X, 0, 256, 256, 512, 512, 0x83F3,
			gl.PIXEL_UNPACK_BUFFER, 0);
		const compressedTexSubImage2DError = gl.getError();
		console.info("compressedTexSubImage2DError: " + compressedTexSubImage2DError);
		expect(compressedTexSubImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0894
	 * @tc.name testCompressedTexSubImage2D_1_03
	 * @tc.desc Test compressedTexSubImage2D.
	 */
	it('testCompressedTexSubImage2D_1_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCompressedTexSubImage2D_1_03 test start ...66');
		var texture = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, texture);
		gl.compressedTexImage2D(gl.TEXTURE_2D, 0, 0x83F3, 512, 512, 0, 0);
		gl.compressedTexSubImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_Y, 0, 256, 256, 512, 512, 0x83F3,
			gl.PIXEL_UNPACK_BUFFER, 0);
		const compressedTexSubImage2DError = gl.getError();
		console.info("compressedTexSubImage2DError: " + compressedTexSubImage2DError);
		expect(compressedTexSubImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0895
	 * @tc.name testCompressedTexSubImage2D_1_04
	 * @tc.desc Test compressedTexSubImage2D.
	 */
	it('testCompressedTexSubImage2D_1_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCompressedTexSubImage2D_1_04 test start ...66');
		var texture = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, texture);
		gl.compressedTexImage2D(gl.TEXTURE_2D, 0, 0x83F3, 512, 512, 0, 0);
		gl.compressedTexSubImage2D(gl.TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, 256, 256, 512, 512, 0x83F3,
			gl.PIXEL_UNPACK_BUFFER, 0);
		const compressedTexSubImage2DError = gl.getError();
		console.info("compressedTexSubImage2DError: " + compressedTexSubImage2DError);
		expect(compressedTexSubImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0896
	 * @tc.name testCompressedTexSubImage2D_1_05
	 * @tc.desc Test compressedTexSubImage2D.
	 */
	it('testCompressedTexSubImage2D_1_05', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCompressedTexSubImage2D_1_05 test start ...66');
		var texture = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, texture);
		gl.compressedTexImage2D(gl.TEXTURE_2D, 0, 0x83F3, 512, 512, 0, 0);
		gl.compressedTexSubImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_Z, 0, 256, 256, 512, 512, 0x83F3,
			gl.PIXEL_UNPACK_BUFFER, 0);
		const compressedTexSubImage2DError = gl.getError();
		console.info("compressedTexSubImage2DError: " + compressedTexSubImage2DError);
		expect(compressedTexSubImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0897
	 * @tc.name testCompressedTexSubImage2D_1_06
	 * @tc.desc Test compressedTexSubImage2D.
	 */
	it('testCompressedTexSubImage2D_1_06', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCompressedTexSubImage2D_1_06 test start ...66');
		var texture = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, texture);
		gl.compressedTexImage2D(gl.TEXTURE_2D, 0, 0x83F3, 512, 512, 0, 0);
		gl.compressedTexSubImage2D(gl.TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, 256, 256, 512, 512, 0x83F3,
			gl.PIXEL_UNPACK_BUFFER, 0);
		const compressedTexSubImage2DError = gl.getError();
		console.info("compressedTexSubImage2DError: " + compressedTexSubImage2DError);
		expect(compressedTexSubImage2DError).assertEqual(0);
		done();
	})


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0898
	 * @tc.name testTexImage2D_1_01
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_1_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D_1_01 test start ...66');
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_X, 0, gl.RGB8, 512, 512, 0, gl.RGB8, gl
			.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0899
	 * @tc.name testTexImage2D_1_02
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_1_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D_1_02 test start ...66');
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_CUBE_MAP_NEGATIVE_X, 0, gl.RGB8, 512, 512, 0, gl.RGB8, gl
			.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0900
	 * @tc.name testTexImage2D_1_03
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_1_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D_1_03 test start ...66');
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_Y, 0, gl.RGB8, 512, 512, 0, gl.RGB8, gl
			.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0901
	 * @tc.name testTexImage2D_1_04
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_1_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D_1_04 test start ...66');
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, gl.RGB8, 512, 512, 0, gl.RGB8, gl
			.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0902
	 * @tc.name testTexImage2D_1_05
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_1_05', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D_1_05 test start ...66');
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_Z, 0, gl.RGB8, 512, 512, 0, gl.RGB8, gl
			.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0903
	 * @tc.name testTexImage2D_1_06
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_1_06', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D_1_06 test start ...66');
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, gl.RGB8, 512, 512, 0, gl.RGB8, gl
			.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0904
	 * @tc.name testTexImage2D_1_07
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_1_07', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D_1_07 test start ...66');
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGB8, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0911
	 * @tc.name testTexSubImage2D_1_01
	 * @tc.desc Test texSubImage2D.
	 */
	it('testTexSubImage2D_1_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexSubImage2D_1_01 test start ...66');
		const buf = new ArrayBuffer(8);
		const view = new DataView(buf, 0);
		view[0] = 2;
		gl.texSubImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_X, 0, 0, 0, 512, 512, -gl.RED, -gl.FLOAT,
			view);
		const ttexSubImage2DError = gl.getError();
		console.info("ttexSubImage2DError: " + ttexSubImage2DError);
		expect(ttexSubImage2DError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0912
	 * @tc.name testTexSubImage2D_1_02
	 * @tc.desc Test texSubImage2D.
	 */
	it('testTexSubImage2D_1_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexSubImage2D_1_02 test start ...66');
		const buf = new ArrayBuffer(8);
		const view = new DataView(buf, 0);
		view[0] = 2;
		gl.texSubImage2D(gl.TEXTURE_CUBE_MAP_NEGATIVE_X, 0, 0, 0, 512, 512, -gl.RED, -gl.FLOAT,
			view);
		const ttexSubImage2DError = gl.getError();
		console.info("ttexSubImage2DError: " + ttexSubImage2DError);
		expect(ttexSubImage2DError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0913
	 * @tc.name testTexSubImage2D_1_03
	 * @tc.desc Test texSubImage2D.
	 */
	it('testTexSubImage2D_1_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexSubImage2D_1_03 test start ...66');
		const buf = new ArrayBuffer(8);
		const view = new DataView(buf, 0);
		view[0] = 2;
		gl.texSubImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_Y, 0, 0, 0, 512, 512, -gl.RED, -gl.FLOAT,
			view);
		const ttexSubImage2DError = gl.getError();
		console.info("ttexSubImage2DError: " + ttexSubImage2DError);
		expect(ttexSubImage2DError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0914
	 * @tc.name testTexSubImage2D_1_04
	 * @tc.desc Test texSubImage2D.
	 */
	it('testTexSubImage2D_1_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexSubImage2D_1_04 test start ...66');
		const buf = new ArrayBuffer(8);
		const view = new DataView(buf, 0);
		view[0] = 2;
		gl.texSubImage2D(gl.TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, 0, 0, 512, 512, -gl.RED, -gl.FLOAT,
			view);
		const ttexSubImage2DError = gl.getError();
		console.info("ttexSubImage2DError: " + ttexSubImage2DError);
		expect(ttexSubImage2DError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0915
	 * @tc.name testTexSubImage2D_1_05
	 * @tc.desc Test texSubImage2D.
	 */
	it('testTexSubImage2D_1_05', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexSubImage2D_1_05 test start ...66');
		const buf = new ArrayBuffer(8);
		const view = new DataView(buf, 0);
		view[0] = 2;
		gl.texSubImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_Z, 0, 0, 0, 512, 512, -gl.RED, -gl.FLOAT,
			view);
		const ttexSubImage2DError = gl.getError();
		console.info("ttexSubImage2DError: " + ttexSubImage2DError);
		expect(ttexSubImage2DError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0916
	 * @tc.name testTexSubImage2D_1_06
	 * @tc.desc Test texSubImage2D.
	 */
	it('testTexSubImage2D_1_06', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexSubImage2D_1_06 test start ...66');
		const buf = new ArrayBuffer(8);
		const view = new DataView(buf, 0);
		view[0] = 2;
		gl.texSubImage2D(gl.TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, 0, 0, 512, 512, -gl.RED, -gl.FLOAT,
			view);
		const ttexSubImage2DError = gl.getError();
		console.info("ttexSubImage2DError: " + ttexSubImage2DError);
		expect(ttexSubImage2DError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0923
	 * @tc.name testCopyBufferSubData_1_01
	 * @tc.desc Test copyBufferSubData.
	 */
	it('testCopyBufferSubData_1_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCopyBufferSubData_1_01 test start ...66');
		const srcBuffer = gl.createBuffer();
		const dstBuffer = gl.createBuffer();
		const vertices = [1, 2];
		const data = new Float32Array(vertices);
		const length = vertices.length * 4;
		gl.bindBuffer(gl.ARRAY_BUFFER, srcBuffer);
		gl.bufferData(gl.ARRAY_BUFFER, data, gl.STATIC_DRAW);
		gl.bindBuffer(gl.COPY_READ_BUFFER, srcBuffer);
		gl.bindBuffer(gl.ARRAY_BUFFER, dstBuffer);
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(length), gl.STATIC_DRAW);
		gl2.copyBufferSubData(gl.ELEMENT_ARRAY_BUFFER, gl.ARRAY_BUFFER, 0, 0, length);
		const copyBufferSubDataError = gl.getError();
		console.info("copyBufferSubDataError: " + copyBufferSubDataError);
		expect(copyBufferSubDataError).assertEqual(gl.INVALID_VALUE);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0924
	 * @tc.name testCopyBufferSubData_1_02
	 * @tc.desc Test copyBufferSubData.
	 */
	it('testCopyBufferSubData_1_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCopyBufferSubData_1_02 test start ...66');
		const srcBuffer = gl.createBuffer();
		const dstBuffer = gl.createBuffer();
		const vertices = [1, 2];
		const data = new Float32Array(vertices);
		const length = vertices.length * 4;
		gl.bindBuffer(gl.ARRAY_BUFFER, srcBuffer);
		gl.bufferData(gl.ARRAY_BUFFER, data, gl.STATIC_DRAW);
		gl.bindBuffer(gl.COPY_READ_BUFFER, srcBuffer);
		gl.bindBuffer(gl.ARRAY_BUFFER, dstBuffer);
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(length), gl.STATIC_DRAW);
		gl2.copyBufferSubData(gl.COPY_READ_BUFFER, gl.ARRAY_BUFFER, 0, 0, length);
		const copyBufferSubDataError = gl.getError();
		console.info("copyBufferSubDataError: " + copyBufferSubDataError);
		expect(copyBufferSubDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0925
	 * @tc.name testCopyBufferSubData_1_03
	 * @tc.desc Test copyBufferSubData.
	 */
	it('testCopyBufferSubData_1_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCopyBufferSubData_1_03 test start ...66');
		const srcBuffer = gl.createBuffer();
		const dstBuffer = gl.createBuffer();
		const vertices = [1, 2];
		const data = new Float32Array(vertices);
		const length = vertices.length * 4;
		gl.bindBuffer(gl.ARRAY_BUFFER, srcBuffer);
		gl.bufferData(gl.ARRAY_BUFFER, data, gl.STATIC_DRAW);
		gl.bindBuffer(gl.COPY_READ_BUFFER, srcBuffer);
		gl.bindBuffer(gl.ARRAY_BUFFER, dstBuffer);
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(length), gl.STATIC_DRAW);
		gl2.copyBufferSubData(gl.COPY_WRITE_BUFFER, gl.ARRAY_BUFFER, 0, 0, length);
		const copyBufferSubDataError = gl.getError();
		console.info("copyBufferSubDataError: " + copyBufferSubDataError);
		expect(copyBufferSubDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0926
	 * @tc.name testCopyBufferSubData_1_04
	 * @tc.desc Test copyBufferSubData.
	 */
	it('testCopyBufferSubData_1_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCopyBufferSubData_1_04 test start ...66');
		const srcBuffer = gl.createBuffer();
		const dstBuffer = gl.createBuffer();
		const vertices = [1, 2];
		const data = new Float32Array(vertices);
		const length = vertices.length * 4;
		gl.bindBuffer(gl.ARRAY_BUFFER, srcBuffer);
		gl.bufferData(gl.ARRAY_BUFFER, data, gl.STATIC_DRAW);
		gl.bindBuffer(gl.COPY_READ_BUFFER, srcBuffer);
		gl.bindBuffer(gl.ARRAY_BUFFER, dstBuffer);
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(length), gl.STATIC_DRAW);
		gl2.copyBufferSubData(gl.TRANSFORM_FEEDBACK_BUFFER, gl.ARRAY_BUFFER, 0, 0, length);
		const copyBufferSubDataError = gl.getError();
		console.info("copyBufferSubDataError: " + copyBufferSubDataError);
		expect(copyBufferSubDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0927
	 * @tc.name testCopyBufferSubData_1_05
	 * @tc.desc Test copyBufferSubData.
	 */
	it('testCopyBufferSubData_1_05', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCopyBufferSubData_1_05 test start ...66');
		const srcBuffer = gl.createBuffer();
		const dstBuffer = gl.createBuffer();
		const vertices = [1, 2];
		const data = new Float32Array(vertices);
		const length = vertices.length * 4;
		gl.bindBuffer(gl.ARRAY_BUFFER, srcBuffer);
		gl.bufferData(gl.ARRAY_BUFFER, data, gl.STATIC_DRAW);
		gl.bindBuffer(gl.COPY_READ_BUFFER, srcBuffer);
		gl.bindBuffer(gl.ARRAY_BUFFER, dstBuffer);
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(length), gl.STATIC_DRAW);
		gl2.copyBufferSubData(gl.UNIFORM_BUFFER, gl.ARRAY_BUFFER, 0, 0, length);
		const copyBufferSubDataError = gl.getError();
		console.info("copyBufferSubDataError: " + copyBufferSubDataError);
		expect(copyBufferSubDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0928
	 * @tc.name testCopyBufferSubData_1_06
	 * @tc.desc Test copyBufferSubData.
	 */
	it('testCopyBufferSubData_1_06', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCopyBufferSubData_1_06 test start ...66');
		const srcBuffer = gl.createBuffer();
		const dstBuffer = gl.createBuffer();
		const vertices = [1, 2];
		const data = new Float32Array(vertices);
		const length = vertices.length * 4;
		gl.bindBuffer(gl.ARRAY_BUFFER, srcBuffer);
		gl.bufferData(gl.ARRAY_BUFFER, data, gl.STATIC_DRAW);
		gl.bindBuffer(gl.COPY_READ_BUFFER, srcBuffer);
		gl.bindBuffer(gl.ARRAY_BUFFER, dstBuffer);
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(length), gl.STATIC_DRAW);
		gl2.copyBufferSubData(gl.PIXEL_PACK_BUFFER, gl.ARRAY_BUFFER, 0, 0, length);
		const copyBufferSubDataError = gl.getError();
		console.info("copyBufferSubDataError: " + copyBufferSubDataError);
		expect(copyBufferSubDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0929
	 * @tc.name testCopyBufferSubData_1_07
	 * @tc.desc Test copyBufferSubData.
	 */
	it('testCopyBufferSubData_1_07', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCopyBufferSubData_1_07 test start ...66');
		const srcBuffer = gl.createBuffer();
		const dstBuffer = gl.createBuffer();
		const vertices = [1, 2];
		const data = new Float32Array(vertices);
		const length = vertices.length * 4;
		gl.bindBuffer(gl.ARRAY_BUFFER, srcBuffer);
		gl.bufferData(gl.ARRAY_BUFFER, data, gl.STATIC_DRAW);
		gl.bindBuffer(gl.COPY_READ_BUFFER, srcBuffer);
		gl.bindBuffer(gl.ARRAY_BUFFER, dstBuffer);
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(length), gl.STATIC_DRAW);
		gl2.copyBufferSubData(gl.PIXEL_UNPACK_BUFFER, gl.ARRAY_BUFFER, 0, 0, length);
		const copyBufferSubDataError = gl.getError();
		console.info("copyBufferSubDataError: " + copyBufferSubDataError);
		expect(copyBufferSubDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0930
	 * @tc.name testGetBufferSubData_1_01
	 * @tc.desc Test getBufferSubData.
	 */
	it('testGetBufferSubData_1_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testGetBufferSubData_1_01 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		const vertices = [1, 2];
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);
		var arrBuffer = new ArrayBuffer(vertices.length * Float32Array.BYTES_PER_ELEMENT);
		gl2.getBufferSubData(gl.ELEMENT_ARRAY_BUFFER, 0, new Float32Array(arrBuffer), 0, 0);
		const getBufferSubDataError = gl.getError();
		console.info("getBufferSubDataError: " + getBufferSubDataError);
		expect(getBufferSubDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0931
	 * @tc.name testGetBufferSubData_1_02
	 * @tc.desc Test getBufferSubData.
	 */
	it('testGetBufferSubData_1_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testGetBufferSubData_1_02 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		const vertices = [1, 2];
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);
		var arrBuffer = new ArrayBuffer(vertices.length * Float32Array.BYTES_PER_ELEMENT);
		gl2.getBufferSubData(gl.COPY_READ_BUFFER, 0, new Float32Array(arrBuffer), 0, 0);
		const getBufferSubDataError = gl.getError();
		console.info("getBufferSubDataError: " + getBufferSubDataError);
		expect(getBufferSubDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0932
	 * @tc.name testGetBufferSubData_1_03
	 * @tc.desc Test getBufferSubData.
	 */
	it('testGetBufferSubData_1_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testGetBufferSubData_1_03 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		const vertices = [1, 2];
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);
		var arrBuffer = new ArrayBuffer(vertices.length * Float32Array.BYTES_PER_ELEMENT);
		gl2.getBufferSubData(gl.COPY_WRITE_BUFFER, 0, new Float32Array(arrBuffer), 0, 0);
		const getBufferSubDataError = gl.getError();
		console.info("getBufferSubDataError: " + getBufferSubDataError);
		expect(getBufferSubDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0933
	 * @tc.name testGetBufferSubData_1_04
	 * @tc.desc Test getBufferSubData.
	 */
	it('testGetBufferSubData_1_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testGetBufferSubData_1_04 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		const vertices = [1, 2];
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);
		var arrBuffer = new ArrayBuffer(vertices.length * Float32Array.BYTES_PER_ELEMENT);
		gl2.getBufferSubData(gl.TRANSFORM_FEEDBACK_BUFFER, 0, new Float32Array(arrBuffer), 0, 0);
		const getBufferSubDataError = gl.getError();
		console.info("getBufferSubDataError: " + getBufferSubDataError);
		expect(getBufferSubDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0934
	 * @tc.name testGetBufferSubData_1_05
	 * @tc.desc Test getBufferSubData.
	 */
	it('testGetBufferSubData_1_05', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testGetBufferSubData_1_05 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		const vertices = [1, 2];
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);
		var arrBuffer = new ArrayBuffer(vertices.length * Float32Array.BYTES_PER_ELEMENT);
		gl2.getBufferSubData(gl.UNIFORM_BUFFER, 0, new Float32Array(arrBuffer), 0, 0);
		const getBufferSubDataError = gl.getError();
		console.info("getBufferSubDataError: " + getBufferSubDataError);
		expect(getBufferSubDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0935
	 * @tc.name testGetBufferSubData_1_06
	 * @tc.desc Test getBufferSubData.
	 */
	it('testGetBufferSubData_1_06', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testGetBufferSubData_1_06 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		const vertices = [1, 2];
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);
		var arrBuffer = new ArrayBuffer(vertices.length * Float32Array.BYTES_PER_ELEMENT);
		gl2.getBufferSubData(gl.PIXEL_PACK_BUFFER, 0, new Float32Array(arrBuffer), 0, 0);
		const getBufferSubDataError = gl.getError();
		console.info("getBufferSubDataError: " + getBufferSubDataError);
		expect(getBufferSubDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0936
	 * @tc.name testGetBufferSubData_1_07
	 * @tc.desc Test getBufferSubData.
	 */
	it('testGetBufferSubData_1_07', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testGetBufferSubData_1_07 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		const vertices = [1, 2];
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);
		var arrBuffer = new ArrayBuffer(vertices.length * Float32Array.BYTES_PER_ELEMENT);
		gl2.getBufferSubData(gl.PIXEL_UNPACK_BUFFER, 0, new Float32Array(arrBuffer), 0, 0);
		const getBufferSubDataError = gl.getError();
		console.info("getBufferSubDataError: " + getBufferSubDataError);
		expect(getBufferSubDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0937
	 * @tc.name testFramebufferTextureLayer_1_01
	 * @tc.desc Test framebufferTextureLayer.
	 */
	it('testFramebufferTextureLayer_1_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testFramebufferTextureLayer_1_01 test start ...66');
		const texture = gl.createTexture();
		gl.bindTexture(gl2.TEXTURE_2D_ARRAY, texture);
		gl2.texStorage3D(gl2.TEXTURE_2D_ARRAY, 1, gl2.RGBA8, 256, 256, 8);
		gl.texParameteri(gl2.TEXTURE_CUBE_MAP, gl2.TEXTURE_MAG_FILTER, gl2.NEAREST);
		const framebuffer = gl.createFramebuffer();
		gl.bindFramebuffer(gl.FRAMEBUFFER, framebuffer);
		gl2.framebufferTextureLayer(gl.DRAW_FRAMEBUFFER, gl2.COLOR_ATTACHMENT0, texture, 0, 8)
		const framebufferTextureLayerError = gl.getError();
		console.info("framebufferTextureLayerError: " + framebufferTextureLayerError);
		expect(framebufferTextureLayerError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0938
	 * @tc.name testFramebufferTextureLayer_1_02
	 * @tc.desc Test framebufferTextureLayer.
	 */
	it('testFramebufferTextureLayer_1_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testFramebufferTextureLayer_1_02 test start ...66');
		const texture = gl.createTexture();
		gl.bindTexture(gl2.TEXTURE_2D_ARRAY, texture);
		gl2.texStorage3D(gl2.TEXTURE_2D_ARRAY, 1, gl2.RGBA8, 256, 256, 8);
		gl.texParameteri(gl2.TEXTURE_CUBE_MAP, gl2.TEXTURE_MAG_FILTER, gl2.NEAREST);
		const framebuffer = gl.createFramebuffer();
		gl.bindFramebuffer(gl.FRAMEBUFFER, framebuffer);
		gl2.framebufferTextureLayer(gl.READ_FRAMEBUFFER, gl2.COLOR_ATTACHMENT0, texture, 0, 8)
		const framebufferTextureLayerError = gl.getError();
		console.info("framebufferTextureLayerError: " + framebufferTextureLayerError);
		expect(framebufferTextureLayerError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0939
	 * @tc.name testFramebufferTextureLayer_1_03
	 * @tc.desc Test framebufferTextureLayer.
	 */
	it('testFramebufferTextureLayer_1_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testFramebufferTextureLayer_1_03 test start ...66');
		const texture = gl.createTexture();
		gl.bindTexture(gl2.TEXTURE_2D_ARRAY, texture);
		gl2.texStorage3D(gl2.TEXTURE_2D_ARRAY, 1, gl2.RGBA8, 256, 256, 8);
		gl.texParameteri(gl2.TEXTURE_CUBE_MAP, gl2.TEXTURE_MAG_FILTER, gl2.NEAREST);
		const framebuffer = gl.createFramebuffer();
		gl.bindFramebuffer(gl.FRAMEBUFFER, framebuffer);
		gl2.framebufferTextureLayer(gl.FRAMEBUFFER, gl2.DEPTH_ATTACHMENT, texture, 0, 8)
		const framebufferTextureLayerError = gl.getError();
		console.info("framebufferTextureLayerError: " + framebufferTextureLayerError);
		expect(framebufferTextureLayerError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0940
	 * @tc.name testFramebufferTextureLayer_1_04
	 * @tc.desc Test framebufferTextureLayer.
	 */
	it('testFramebufferTextureLayer_1_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testFramebufferTextureLayer_1_04 test start ...66');
		const texture = gl.createTexture();
		gl.bindTexture(gl2.TEXTURE_2D_ARRAY, texture);
		gl2.texStorage3D(gl2.TEXTURE_2D_ARRAY, 1, gl2.RGBA8, 256, 256, 8);
		gl.texParameteri(gl2.TEXTURE_CUBE_MAP, gl2.TEXTURE_MAG_FILTER, gl2.NEAREST);
		const framebuffer = gl.createFramebuffer();
		gl.bindFramebuffer(gl.FRAMEBUFFER, framebuffer);
		gl2.framebufferTextureLayer(gl.FRAMEBUFFER, gl2.STENCIL_ATTACHMENT, texture, 0, 8)
		const framebufferTextureLayerError = gl.getError();
		console.info("framebufferTextureLayerError: " + framebufferTextureLayerError);
		expect(framebufferTextureLayerError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0941
	 * @tc.name testFramebufferTextureLayer_1_05
	 * @tc.desc Test framebufferTextureLayer.
	 */
	it('testFramebufferTextureLayer_1_05', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testFramebufferTextureLayer_1_05 test start ...66');
		const texture = gl.createTexture();
		gl.bindTexture(gl2.TEXTURE_2D_ARRAY, texture);
		gl2.texStorage3D(gl2.TEXTURE_2D_ARRAY, 1, gl2.RGBA8, 256, 256, 8);
		gl.texParameteri(gl2.TEXTURE_CUBE_MAP, gl2.TEXTURE_MAG_FILTER, gl2.NEAREST);
		const framebuffer = gl.createFramebuffer();
		gl.bindFramebuffer(gl.FRAMEBUFFER, framebuffer);
		gl2.framebufferTextureLayer(gl.FRAMEBUFFER, gl2.DEPTH_STENCIL_ATTACHMENT, texture, 0, 8)
		const framebufferTextureLayerError = gl.getError();
		console.info("framebufferTextureLayerError: " + framebufferTextureLayerError);
		expect(framebufferTextureLayerError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0942
	 * @tc.name testInvalidateFramebuffer_1_01
	 * @tc.desc Test invalidateFramebuffer.
	 */
	it('testInvalidateFramebuffer_1_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testInvalidateFramebuffer_1_01 test start ...66');
		gl2.invalidateFramebuffer(gl.DRAW_FRAMEBUFFER,
			[gl.COLOR_ATTACHMENT0, gl.COLOR_ATTACHMENT1]);
		const invalidateFramebufferError = gl.getError();
		console.info("invalidateFramebufferError: " + invalidateFramebufferError);
		expect(invalidateFramebufferError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0943
	 * @tc.name testInvalidateFramebuffer_1_02
	 * @tc.desc Test invalidateFramebuffer.
	 */
	it('testInvalidateFramebuffer_1_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testInvalidateFramebuffer_1_02 test start ...66');
		gl2.invalidateFramebuffer(gl.FRAMEBUFFER,
			[gl.COLOR_ATTACHMENT0, gl.COLOR_ATTACHMENT1]);
		const invalidateFramebufferError = gl.getError();
		console.info("invalidateFramebufferError: " + invalidateFramebufferError);
		expect(invalidateFramebufferError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0944
	 * @tc.name testInvalidateFramebuffer_1_03
	 * @tc.desc Test invalidateFramebuffer.
	 */
	it('testInvalidateFramebuffer_1_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testInvalidateFramebuffer_1_03 test start ...66');
		gl2.invalidateFramebuffer(gl.READ_FRAMEBUFFER,
			[gl.COLOR_ATTACHMENT0, gl.DEPTH_ATTACHMENT]);
		const invalidateFramebufferError = gl.getError();
		console.info("invalidateFramebufferError: " + invalidateFramebufferError);
		expect(invalidateFramebufferError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0945
	 * @tc.name testInvalidateFramebuffer_1_04
	 * @tc.desc Test invalidateFramebuffer.
	 */
	it('testInvalidateFramebuffer_1_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testInvalidateFramebuffer_1_04 test start ...66');
		gl2.invalidateFramebuffer(gl.READ_FRAMEBUFFER,
			[gl.STENCIL_ATTACHMENT, gl.COLOR_ATTACHMENT1]);
		const invalidateFramebufferError = gl.getError();
		console.info("invalidateFramebufferError: " + invalidateFramebufferError);
		expect(invalidateFramebufferError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0946
	 * @tc.name testInvalidateFramebuffer_1_05
	 * @tc.desc Test invalidateFramebuffer.
	 */
	it('testInvalidateFramebuffer_1_05', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testInvalidateFramebuffer_1_05 test start ...66');
		gl2.invalidateFramebuffer(gl.READ_FRAMEBUFFER,
			[gl.DEPTH_STENCIL_ATTACHMENT, gl.COLOR_ATTACHMENT1]);
		const invalidateFramebufferError = gl.getError();
		console.info("invalidateFramebufferError: " + invalidateFramebufferError);
		expect(invalidateFramebufferError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0947
	 * @tc.name testInvalidateFramebuffer_1_06
	 * @tc.desc Test invalidateFramebuffer.
	 */
	it('testInvalidateFramebuffer_1_06', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testInvalidateFramebuffer_1_06 test start ...66');
		gl2.invalidateFramebuffer(gl.READ_FRAMEBUFFER,
			[gl.DEPTH_ATTACHMENT, gl.STENCIL_ATTACHMENT]);
		const invalidateFramebufferError = gl.getError();
		console.info("invalidateFramebufferError: " + invalidateFramebufferError);
		expect(invalidateFramebufferError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0948
	 * @tc.name testInvalidateFramebuffer_1_01
	 * @tc.desc Test invalidateFramebuffer.
	 */
	it('testInvalidateFramebuffer_1_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testInvalidateFramebuffer_1_01 test start ...66');
		gl2.invalidateFramebuffer(gl.READ_FRAMEBUFFER,
			[gl.COLOR_ATTACHMENT0, gl.COLOR_ATTACHMENT1]);
		const invalidateFramebufferError = gl.getError();
		console.info("invalidateFramebufferError: " + invalidateFramebufferError);
		expect(invalidateFramebufferError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0949
	 * @tc.name testInvalidateSubFramebuffer_1_01
	 * @tc.desc Test invalidateSubFramebuffer.
	 */
	it('testInvalidateSubFramebuffer_1_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testInvalidateSubFramebuffer_1_01 test start ...66');
		gl2.invalidateSubFramebuffer(gl.FRAMEBUFFER,
			[gl.COLOR_ATTACHMENT0, gl.COLOR_ATTACHMENT1],
			0, 0, 256, 256);
		const invalidateSubFramebufferError = gl.getError();
		console.info("invalidateSubFramebufferError: " + invalidateSubFramebufferError);
		expect(invalidateSubFramebufferError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0950
	 * @tc.name testInvalidateSubFramebuffer_1_02
	 * @tc.desc Test invalidateSubFramebuffer.
	 */
	it('testInvalidateSubFramebuffer_1_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testInvalidateSubFramebuffer_1_02 test start ...66');
		gl2.invalidateSubFramebuffer(gl.DRAW_FRAMEBUFFER,
			[gl.COLOR_ATTACHMENT0, gl.COLOR_ATTACHMENT1],
			0, 0, 256, 256);
		const invalidateSubFramebufferError = gl.getError();
		console.info("invalidateSubFramebufferError: " + invalidateSubFramebufferError);
		expect(invalidateSubFramebufferError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0951
	 * @tc.name testInvalidateSubFramebuffer_1_03
	 * @tc.desc Test invalidateSubFramebuffer.
	 */
	it('testInvalidateSubFramebuffer_1_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testInvalidateSubFramebuffer_1_03 test start ...66');
		gl2.invalidateSubFramebuffer(gl.READ_FRAMEBUFFER,
			[gl.DEPTH_ATTACHMENT, gl.COLOR_ATTACHMENT1],
			0, 0, 256, 256);
		const invalidateSubFramebufferError = gl.getError();
		console.info("invalidateSubFramebufferError: " + invalidateSubFramebufferError);
		expect(invalidateSubFramebufferError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0952
	 * @tc.name testInvalidateSubFramebuffer_1_04
	 * @tc.desc Test invalidateSubFramebuffer.
	 */
	it('testInvalidateSubFramebuffer_1_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testInvalidateSubFramebuffer_1_04 test start ...66');
		gl2.invalidateSubFramebuffer(gl.READ_FRAMEBUFFER,
			[gl.COLOR_ATTACHMENT0, gl.STENCIL_ATTACHMENT],
			0, 0, 256, 256);
		const invalidateSubFramebufferError = gl.getError();
		console.info("invalidateSubFramebufferError: " + invalidateSubFramebufferError);
		expect(invalidateSubFramebufferError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0953
	 * @tc.name testInvalidateSubFramebuffer_1_05
	 * @tc.desc Test invalidateSubFramebuffer.
	 */
	it('testInvalidateSubFramebuffer_1_05', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testInvalidateSubFramebuffer_1_05 test start ...66');
		gl2.invalidateSubFramebuffer(gl.READ_FRAMEBUFFER,
			[gl.DEPTH_STENCIL_ATTACHMENT, gl.STENCIL_ATTACHMENT],
			0, 0, 256, 256);
		const invalidateSubFramebufferError = gl.getError();
		console.info("invalidateSubFramebufferError: " + invalidateSubFramebufferError);
		expect(invalidateSubFramebufferError).assertEqual(0);
		done();
	})


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0954
	 * @tc.name testRenderbufferStorageMultisample_1_01
	 * @tc.desc Test renderbufferStorageMultisample.
	 */
	it('testRenderbufferStorageMultisample_1_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testRenderbufferStorageMultisample_1_01 test start ...66');
		gl2.renderbufferStorageMultisample(gl.RENDERBUFFER, 4, gl.R8, 256, 256);
		const renderbufferStorageMultisampleError = gl.getError();
		console.info("renderbufferStorageMultisampleError: " + renderbufferStorageMultisampleError);
		expect(renderbufferStorageMultisampleError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0955
	 * @tc.name testRenderbufferStorageMultisample_1_02
	 * @tc.desc Test renderbufferStorageMultisample.
	 */
	it('testRenderbufferStorageMultisample_1_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testRenderbufferStorageMultisample_1_02 test start ...66');
		gl2.renderbufferStorageMultisample(gl.RENDERBUFFER, 4, gl.R8UI, 256, 256);
		const renderbufferStorageMultisampleError = gl.getError();
		console.info("renderbufferStorageMultisampleError: " + renderbufferStorageMultisampleError);
		expect(renderbufferStorageMultisampleError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0956
	 * @tc.name testRenderbufferStorageMultisample_1_03
	 * @tc.desc Test renderbufferStorageMultisample.
	 */
	it('testRenderbufferStorageMultisample_1_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testRenderbufferStorageMultisample_1_03 test start ...66');
		gl2.renderbufferStorageMultisample(gl.RENDERBUFFER, 4, gl.R8I, 256, 256);
		const renderbufferStorageMultisampleError = gl.getError();
		console.info("renderbufferStorageMultisampleError: " + renderbufferStorageMultisampleError);
		expect(renderbufferStorageMultisampleError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0957
	 * @tc.name testRenderbufferStorageMultisample_1_04
	 * @tc.desc Test renderbufferStorageMultisample.
	 */
	it('testRenderbufferStorageMultisample_1_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testRenderbufferStorageMultisample_1_04 test start ...66');
		gl2.renderbufferStorageMultisample(gl.RENDERBUFFER, 4, gl.R16UI, 256, 256);
		const renderbufferStorageMultisampleError = gl.getError();
		console.info("renderbufferStorageMultisampleError: " + renderbufferStorageMultisampleError);
		expect(renderbufferStorageMultisampleError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0958
	 * @tc.name testRenderbufferStorageMultisample_1_05
	 * @tc.desc Test renderbufferStorageMultisample.
	 */
	it('testRenderbufferStorageMultisample_1_05', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testRenderbufferStorageMultisample_1_05 test start ...66');
		gl2.renderbufferStorageMultisample(gl.RENDERBUFFER, 4, gl.R16I, 256, 256);
		const renderbufferStorageMultisampleError = gl.getError();
		console.info("renderbufferStorageMultisampleError: " + renderbufferStorageMultisampleError);
		expect(renderbufferStorageMultisampleError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0959
	 * @tc.name testRenderbufferStorageMultisample_1_06
	 * @tc.desc Test renderbufferStorageMultisample.
	 */
	it('testRenderbufferStorageMultisample_1_06', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testRenderbufferStorageMultisample_1_06 test start ...66');
		gl2.renderbufferStorageMultisample(gl.RENDERBUFFER, 4, gl.R32UI, 256, 256);
		const renderbufferStorageMultisampleError = gl.getError();
		console.info("renderbufferStorageMultisampleError: " + renderbufferStorageMultisampleError);
		expect(renderbufferStorageMultisampleError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0960
	 * @tc.name testRenderbufferStorageMultisample_1_07
	 * @tc.desc Test renderbufferStorageMultisample.
	 */
	it('testRenderbufferStorageMultisample_1_07', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testRenderbufferStorageMultisample_1_07 test start ...66');
		gl2.renderbufferStorageMultisample(gl.RENDERBUFFER, 4, gl.R32I, 256, 256);
		const renderbufferStorageMultisampleError = gl.getError();
		console.info("renderbufferStorageMultisampleError: " + renderbufferStorageMultisampleError);
		expect(renderbufferStorageMultisampleError).assertEqual(0);
		done();
	})


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0961
	 * @tc.name testRenderbufferStorageMultisample_1_08
	 * @tc.desc Test renderbufferStorageMultisample.
	 */
	it('testRenderbufferStorageMultisample_1_08', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testRenderbufferStorageMultisample_1_08 test start ...66');
		gl2.renderbufferStorageMultisample(gl.RENDERBUFFER, 4, gl.RG8, 256, 256);
		const renderbufferStorageMultisampleError = gl.getError();
		console.info("renderbufferStorageMultisampleError: " + renderbufferStorageMultisampleError);
		expect(renderbufferStorageMultisampleError).assertEqual(0);
		done();
	})


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0962
	 * @tc.name testTexStorage2D_1_01
	 * @tc.desc Test texStorage2D.
	 */
	it('testTexStorage2D_1_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexStorage2D_1_01 test start ...66');
		gl2.texStorage2D(gl.TEXTURE_CUBE_MAP, 1, gl.RGB8, 256, 256);
		const texStorage2DError = gl.getError();
		console.info("texStorage2DError: " + texStorage2DError);
		expect(texStorage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0963
	 * @tc.name testBindBufferBase_1_01
	 * @tc.desc Test bindBufferBase.
	 */
	it('testBindBufferBase_1_01', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testBindBufferBase_1_01 test start ...66');
			let buffer = gl.createBuffer();
			gl2.bindBufferBase(gl.UNIFORM_BUFFER, 0, buffer);
			const bindBufferBaseError = gl.getError();
			console.info("bindBufferBaseError: " + bindBufferBaseError);
			expect(bindBufferBaseError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("bindBufferBaseError has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0964
	 * @tc.name bindBufferRange_1_01
	 * @tc.desc Test bindBufferRange.
	 */
	it('bindBufferRange_1_01', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL bindBufferRange_1_01 test start ...66');
			const buffer = gl.createBuffer();
			gl2.bindBufferRange(gl.UNIFORM_BUFFER, 1, buffer, 0, 4);
			const bindBufferRangeError = gl.getError();
			console.info("bindBufferRangeError: " + bindBufferRangeError);
			expect(bindBufferRangeError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("bindBufferRange has failed for " + e)
			expect(null).assertFail()
		}
	})



	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0965
	 * @tc.name testVertexAttrib2f2
	 * @tc.desc Test vertexAttrib2f.
	 */
	it('testVertexAttrib2f2', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttrib2f test start ...66');
			gl.vertexAttrib2f(1, 10.0, 10.0);
			const vertexAttrib2fError = gl.getError();
			console.info("vertexAttrib2fError: " + vertexAttrib2fError);
			expect(vertexAttrib2fError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testVertexAttrib2f has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0966
	 * @tc.name testVertexAttrib3f2
	 * @tc.desc Test vertexAttrib3f.
	 */
	it('testVertexAttrib3f2', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testvertexAttrib3f test start ...66');
			console.info('jsWebGL testvertexAttrib3f test start ...' + JSON.stringify(gl));
			gl.vertexAttrib3f(1, 10.0, 10.0, 10.0);
			const vertexAttrib3fError = gl.getError();
			console.info("vertexAttrib3fError: " + vertexAttrib3fError);
			expect(vertexAttrib3fError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testVertexAttrib3f has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0967
	 * @tc.name testVertexAttrib4f2
	 * @tc.desc Test vertexAttrib4f.
	 */
	it('testVertexAttrib4f2', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttrib4f test start ...66');
			console.info('jsWebGL testVertexAttrib4f test start ...' + JSON.stringify(gl));
			gl.vertexAttrib4f(1, 10.0, 10.0, 10.0, 10.0);
			const vertexAttrib4fError = gl.getError();
			console.info("vertexAttrib4fError: " + vertexAttrib4fError);
			expect(vertexAttrib4fError).assertEqual(gl.NO_ERROR);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testvertexAttrib4f has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0968
	 * @tc.name testVertexAttrib1f2
	 * @tc.desc Test vertexAttrib1f.
	 */
	it('testVertexAttrib1f2', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttrib1f test start ...66');
			console.info('jsWebGL testVertexAttrib1f test start ...' + JSON.stringify(gl));
			gl.vertexAttrib1f(-1, 10.0);
			const vertexAttrib1fError = gl.getError();
			console.info("vertexAttrib1fError: " + vertexAttrib1fError);
			expect(vertexAttrib1fError).assertEqual(gl.INVALID_VALUE);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testVertexAttrib1f has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0969
	 * @tc.name testVertexAttrib2f3
	 * @tc.desc Test vertexAttrib2f.
	 */
	it('testVertexAttrib2f3', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttrib2f test start ...66');
			gl.vertexAttrib2f(-1, 10.0, 10.0);
			const vertexAttrib2fError = gl.getError();
			console.info("vertexAttrib2fError: " + vertexAttrib2fError);
			expect(vertexAttrib2fError).assertEqual(gl.INVALID_VALUE);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testVertexAttrib2f has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0970
	 * @tc.name testVertexAttrib3f3
	 * @tc.desc Test vertexAttrib3f.
	 */
	it('testVertexAttrib3f3', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testvertexAttrib3f test start ...66');
			console.info('jsWebGL testvertexAttrib3f test start ...' + JSON.stringify(gl));
			gl.vertexAttrib3f(-1, 10.0, 10.0, 10.0);
			const vertexAttrib3fError = gl.getError();
			console.info("vertexAttrib3fError: " + vertexAttrib3fError);
			expect(vertexAttrib3fError).assertEqual(gl.INVALID_VALUE);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testVertexAttrib3f has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0971
	 * @tc.name testVertexAttrib4f22
	 * @tc.desc Test vertexAttrib4f.
	 */
	it('testVertexAttrib4f22', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttrib4f test start ...66');
			console.info('jsWebGL testVertexAttrib4f test start ...' + JSON.stringify(gl));
			gl.vertexAttrib4f(-1, 10.0, 10.0, 10.0, 10.0);
			const vertexAttrib4fError = gl.getError();
			console.info("vertexAttrib4fError: " + vertexAttrib4fError);
			expect(vertexAttrib4fError).assertEqual(gl.INVALID_VALUE);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testvertexAttrib4f has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0972
	 * @tc.name testVertexAttribPointer2
	 * @tc.desc Test vertexAttribPointer.
	 */
	it('testVertexAttribPointer2', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttribPointer test start ...66');
			console.info('jsWebGL testVertexAttribPointer test start ...' + JSON.stringify(gl));
			gl.vertexAttribPointer(1, 4, gl.BYTE, true, 20, 12);
			const vertexAttribPointerError = gl.getError();
			console.info("vertexAttribPointerError: " + vertexAttribPointerError);
			expect(vertexAttribPointerError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testVertexAttribPointer has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0973
	 * @tc.name testVertexAttribPointer3
	 * @tc.desc Test vertexAttribPointer.
	 */
	it('testVertexAttribPointer3', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttribPointer test start ...66');
			console.info('jsWebGL testVertexAttribPointer test start ...' + JSON.stringify(gl));
			gl.vertexAttribPointer(2, 2, gl.UNSIGNED_SHORT, true, 20, 16);
			const vertexAttribPointerError = gl.getError();
			console.info("vertexAttribPointerError: " + vertexAttribPointerError);
			expect(vertexAttribPointerError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testVertexAttribPointer has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0974
	 * @tc.name testVertexAttribPointer4
	 * @tc.desc Test vertexAttribPointer.
	 */
	it('testVertexAttribPointer4', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttribPointer test start ...66');
			console.info('jsWebGL testVertexAttribPointer test start ...' + JSON.stringify(gl));
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
			const locPosition = gl.getAttribLocation(programObj, 'position');
			gl.vertexAttribPointer(locPosition, 3, gl.FLOAT, false, 20, 0);
			const vertexAttribPointerError = gl.getError();
			console.info("vertexAttribPointerError: " + vertexAttribPointerError);
			expect(vertexAttribPointerError).assertEqual(gl.INVALID_VALUE);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testVertexAttribPointer has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0975
	 * @tc.name testVertexAttribPointer5
	 * @tc.desc Test vertexAttribPointer.
	 */
	it('testVertexAttribPointer5', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttribPointer test start ...66');
			console.info('jsWebGL testVertexAttribPointer test start ...' + JSON.stringify(gl));
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
			const locNormal = gl.getAttribLocation(programObj, 'normal');
			gl.vertexAttribPointer(locNormal, 4, gl.BYTE, true, 20, 12);
			const vertexAttribPointerError = gl.getError();
			console.info("vertexAttribPointerError: " + vertexAttribPointerError);
			expect(vertexAttribPointerError).assertEqual(gl.INVALID_VALUE);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testVertexAttribPointer has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0976
	 * @tc.name testVertexAttribPointer6
	 * @tc.desc Test vertexAttribPointer.
	 */
	it('testVertexAttribPointer6', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttribPointer test start ...66');
			console.info('jsWebGL testVertexAttribPointer test start ...' + JSON.stringify(gl));
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
			const locTexUV = gl.getAttribLocation(programObj, 'texUV');
			gl.vertexAttribPointer(locTexUV, 2, gl.UNSIGNED_SHORT, true, 20, 16);
			const vertexAttribPointerError = gl.getError();
			console.info("vertexAttribPointerError: " + vertexAttribPointerError);
			expect(vertexAttribPointerError).assertEqual(gl.INVALID_VALUE);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testVertexAttribPointer has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0977
	 * @tc.name testTexImage3D2
	 * @tc.desc Test texImage3D.
	 */
	it('testTexImage3D2', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexImage3D test start ...66');
			console.info('jsWebGL testTexImage3D test start ...' + JSON.stringify(gl));
			gl2.texImage3D(gl2.TEXTURE_3D, 0, gl.R8, 1, 1, 1, 0, gl.RGBA, gl.UNSIGNED_BYTE, 0);
			const texImage3DError = gl.getError();
			console.info("texImage3DError: " + texImage3DError);
			expect(texImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexImage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0978
	 * @tc.name testTexImage3D3
	 * @tc.desc Test texImage3D.
	 */
	it('testTexImage3D3', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexImage3D test start ...66');
			console.info('jsWebGL testTexImage3D test start ...' + JSON.stringify(gl));
			gl2.texImage3D(gl2.TEXTURE_3D, 0, gl.LUMINANCE, 1, 1, 1, 0, gl.RGBA, gl.UNSIGNED_BYTE,
				0);
			const texImage3DError = gl.getError();
			console.info("texImage3DError: " + texImage3DError);
			expect(texImage3DError).assertEqual(gl.INVALID_OPERATION);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexImage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0979
	 * @tc.name testTexImage3D4
	 * @tc.desc Test texImage3D.
	 */
	it('testTexImage3D4', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexImage3D test start ...66');
			console.info('jsWebGL testTexImage3D test start ...' + JSON.stringify(gl));
			gl2.texImage3D(gl2.TEXTURE_3D, 0, gl.RGB8UI, 1, 1, 1, 0, gl.RGBA, gl.UNSIGNED_BYTE, 0);
			const texImage3DError = gl.getError();
			console.info("texImage3DError: " + texImage3DError);
			expect(texImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexImage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0980
	 * @tc.name testTexSubImage3D
	 * @tc.desc Test texSubImage3D.
	 */
	it('testTexSubImage3D', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexSubImage3D test start ...66');
			console.info('jsWebGL testTexSubImage3D test start ...' + JSON.stringify(gl));
			gl2.texSubImage3D(gl.TEXTURE_3D, 0, 0, 0, 0, 200, 200, 1, gl.RGB8UI, gl.UNSIGNED_BYTE,
				1);
			const texSubImage3DError = gl.getError();
			console.info("texSubImage3DError: " + texSubImage3DError);
			expect(texSubImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexSubImage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0981
	 * @tc.name testTexSubImage3D
	 * @tc.desc Test texSubImage3D.
	 */
	it('testTexSubImage3D', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexSubImage3D test start ...66');
			console.info('jsWebGL testTexSubImage3D test start ...' + JSON.stringify(gl));
			gl2.texSubImage3D(gl.TEXTURE_3D, 0, 0, 0, 0, 200, 200, 1, gl.R8, gl.UNSIGNED_BYTE, 1);
			const texSubImage3DError = gl.getError();
			console.info("texSubImage3DError: " + texSubImage3DError);
			expect(texSubImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexSubImage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0982
	 * @tc.name testTexSubImage3D
	 * @tc.desc Test texSubImage3D.
	 */
	it('testTexSubImage3D', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexSubImage3D test start ...66');
			console.info('jsWebGL testTexSubImage3D test start ...' + JSON.stringify(gl));
			gl2.texSubImage3D(gl.TEXTURE_3D, 0, 0, 0, 0, 200, 200, 1, gl.LUMINANCE, gl
				.UNSIGNED_BYTE, 1);
			const texSubImage3DError = gl.getError();
			console.info("texSubImage3DError: " + texSubImage3DError);
			expect(texSubImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexSubImage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0983
	 * @tc.name testCompressedTexImage3D
	 * @tc.desc Test compressedTexImage3D.
	 */
	it('testCompressedTexImage3D', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testCompressedTexImage3D test start ...66');
			console.info('jsWebGL testCompressedTexImage3D test start ...' + JSON.stringify(gl));
			const buffer = new ArrayBuffer(8);
			gl2.compressedTexImage3D(gl.TEXTURE_3D, 0, 0x83F0, 512, 512, 0, buffer);
			const compressedTexImage3DError = gl.getError();
			console.info("compressedTexImage3DError: " + compressedTexImage3DError);
			expect(compressedTexImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testCompressedTexImage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0984
	 * @tc.name testCompressedTexImage3D
	 * @tc.desc Test compressedTexImage3D.
	 */
	it('testCompressedTexImage3D', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testCompressedTexImage3D test start ...66');
			console.info('jsWebGL testCompressedTexImage3D test start ...' + JSON.stringify(gl));
			const buffer = new ArrayBuffer(8);
			gl2.compressedTexImage3D(gl.TEXTURE_3D, 0, 0x83F0, 512, 512, 0, buffer, 0, 0);
			const compressedTexImage3DError = gl.getError();
			console.info("compressedTexImage3DError: " + compressedTexImage3DError);
			expect(compressedTexImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testCompressedTexImage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0985
	 * @tc.name testCompressedTexImage3D
	 * @tc.desc Test compressedTexImage3D.
	 */
	it('testCompressedTexImage3D', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testCompressedTexImage3D test start ...66');
			console.info('jsWebGL testCompressedTexImage3D test start ...' + JSON.stringify(gl));
			gl2.compressedTexImage3D(gl.TEXTURE_3D, 0, 0x83F0, 512, 512, 0, gl2.PIXEL_UNPACK_BUFFER,
				0);
			const compressedTexImage3DError = gl.getError();
			console.info("compressedTexImage3DError: " + compressedTexImage3DError);
			expect(compressedTexImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testCompressedTexImage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0986
	 * @tc.name testCompressedTexImage3D
	 * @tc.desc Test compressedTexImage3D.
	 */
	it('testCompressedTexImage3D', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testCompressedTexImage3D test start ...66');
			console.info('jsWebGL testCompressedTexImage3D test start ...' + JSON.stringify(gl));
			const buffer = new ArrayBuffer(8);
			gl2.compressedTexImage3D(gl.TEXTURE_3D, false, 0x83F0, 512, 512, 0, buffer);
			const compressedTexImage3DError = gl.getError();
			console.info("compressedTexImage3DError: " + compressedTexImage3DError);
			expect(compressedTexImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testCompressedTexImage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0987
	 * @tc.name testCompressedTexImage3D
	 * @tc.desc Test compressedTexImage3D.
	 */
	it('testCompressedTexImage3D', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testCompressedTexImage3D test start ...66');
			console.info('jsWebGL testCompressedTexImage3D test start ...' + JSON.stringify(gl));
			const buffer = new ArrayBuffer(8);
			gl2.compressedTexImage3D(gl.TEXTURE_3D, false, 0x83F0, 512, 512, 0, buffer, 0, 0);
			const compressedTexImage3DError = gl.getError();
			console.info("compressedTexImage3DError: " + compressedTexImage3DError);
			expect(compressedTexImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testCompressedTexImage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0988
	 * @tc.name testCompressedTexImage3D
	 * @tc.desc Test compressedTexImage3D.
	 */
	it('testCompressedTexImage3D', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testCompressedTexImage3D test start ...66');
			console.info('jsWebGL testCompressedTexImage3D test start ...' + JSON.stringify(gl));
			gl2.compressedTexImage3D(gl.TEXTURE_3D, false, 0x83F0, 512, 512, 0, gl2
				.PIXEL_UNPACK_BUFFER, 0);
			const compressedTexImage3DError = gl.getError();
			console.info("compressedTexImage3DError: " + compressedTexImage3DError);
			expect(compressedTexImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testCompressedTexImage3D has failed for " + e)
			expect(null).assertFail()
		}
	})


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0989
	 * @tc.name testFrontFace2
	 * @tc.desc Test frontFace.
	 */
	it('testFrontFace2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL frontFace test start ...' + JSON.stringify(gl));
		gl.enable(gl.CULL_FACE);
		gl.cullFace(gl.FRONT_AND_BACK);
		gl.frontFace(gl.CCW);
		const frontFaceParameter = gl.getParameter(gl.CULL_FACE_MODE);
		console.info('frontFace -> frontFaceParameter' + frontFaceParameter);
		gl.disable(gl.CULL_FACE);
		expect(frontFaceParameter).assertEqual(gl.FRONT_AND_BACK);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0990
	 * @tc.name testGenerateMipmap2
	 * @tc.desc Test generateMipmap.
	 */
	it('testGenerateMipmap2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL generateMipmap test start ...' + JSON.stringify(gl));
		gl.generateMipmap(gl2.TEXTURE_CUBE_MAP);
		const error = gl.getError();
		console.info('generateMipmap error ' + error);
		expect(error).assertEqual(0);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0991
	 * @tc.name testGetBufferParameter3
	 * @tc.desc Test getBufferParameter.
	 */
	it('testGetBufferParameter3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getBufferParameter test start ...' + JSON.stringify(gl));
		const buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ARRAY_BUFFER, 1024, gl.STATIC_DRAW);
		const bufferSize = gl.getBufferParameter(gl.ELEMENT_ARRAY_BUFFER, gl.BUFFER_SIZE);
		console.info('bufferSize' + bufferSize);
		expect(bufferSize).assertEqual(8);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0992
	 * @tc.name testGetFramebufferAttachmentParameter1
	 * @tc.desc Test getFramebufferAttachmentParameter.
	 */
	it('testGetFramebufferAttachmentParameter1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getFramebufferAttachmentParameter test start ...' + JSON.stringify(
			gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.RENDERBUFFER,
			renderBuffer);
		const framebuffer = gl.getFramebufferAttachmentParameter(gl.FRAMEBUFFER, gl
			.DEPTH_ATTACHMENT,
			gl.FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE);
		console.info('Framebuffer' + framebuffer);
		const errorCode = gl.getError();
		console.info("jsWebGL getFramebufferAttachmentParameter errorCode: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0993
	 * @tc.name testGetFramebufferAttachmentParameter2
	 * @tc.desc Test getFramebufferAttachmentParameter.
	 */
	it('testGetFramebufferAttachmentParameter2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getFramebufferAttachmentParameter test start ...' + JSON.stringify(
			gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.RENDERBUFFER,
			renderBuffer);
		const framebuffer = gl.getFramebufferAttachmentParameter(gl.FRAMEBUFFER, gl
			.STENCIL_ATTACHMENT,
			gl.FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE);
		console.info('Framebuffer' + framebuffer);
		const errorCode = gl.getError();
		console.info("jsWebGL getFramebufferAttachmentParameter errorCode: " + errorCode);
		expect(errorCode).assertEqual(0);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0994
	 * @tc.name testGetFramebufferAttachmentParameter3
	 * @tc.desc Test getFramebufferAttachmentParameter.
	 */
	it('testGetFramebufferAttachmentParameter3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getFramebufferAttachmentParameter test start ...' + JSON.stringify(
			gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.RENDERBUFFER,
			renderBuffer);
		const framebuffer = gl.getFramebufferAttachmentParameter(gl.FRAMEBUFFER, gl
			.DEPTH_STENCIL_ATTACHMENT,
			gl.FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE);
		console.info('Framebuffer' + framebuffer);
		const errorCode = gl.getError();
		console.info("jsWebGL getFramebufferAttachmentParameter errorCode: " + errorCode);
		expect(errorCode).assertEqual(0);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0995
	 * @tc.name testGetShaderPrecisionFormat1
	 * @tc.desc Test getShaderPrecisionFormat.
	 */
	it('testGetShaderPrecisionFormat1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getShaderPrecisionFormat test start ...' + JSON.stringify(gl));
		const shaderPrecisionFormat = gl.getShaderPrecisionFormat(gl.FRAGMENT_SHADER, gl.LOW_FLOAT);
		console.info('shaderPrecisionFormat' + JSON.stringify(shaderPrecisionFormat));
		const type = ((typeof shaderPrecisionFormat) === "object")
		expect(type).assertEqual(true);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0996
	 * @tc.name testGetShaderPrecisionFormat2
	 * @tc.desc Test getShaderPrecisionFormat.
	 */
	it('testGetShaderPrecisionFormat2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getShaderPrecisionFormat test start ...' + JSON.stringify(gl));
		const shaderPrecisionFormat = gl.getShaderPrecisionFormat(gl.FRAGMENT_SHADER, gl
			.HIGH_FLOAT);
		console.info('shaderPrecisionFormat' + JSON.stringify(shaderPrecisionFormat));
		const type = ((typeof shaderPrecisionFormat) === "object")
		expect(type).assertEqual(true);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0997
	 * @tc.name testGetShaderPrecisionFormat3
	 * @tc.desc Test getShaderPrecisionFormat.
	 */
	it('testGetShaderPrecisionFormat3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getShaderPrecisionFormat test start ...' + JSON.stringify(gl));
		const shaderPrecisionFormat = gl.getShaderPrecisionFormat(gl.FRAGMENT_SHADER, gl.LOW_INT);
		console.info('shaderPrecisionFormat' + JSON.stringify(shaderPrecisionFormat));
		const type = ((typeof shaderPrecisionFormat) === "object")
		expect(type).assertEqual(true);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0998
	 * @tc.name testGetShaderPrecisionFormat4
	 * @tc.desc Test getShaderPrecisionFormat.
	 */
	it('testGetShaderPrecisionFormat4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getShaderPrecisionFormat test start ...' + JSON.stringify(gl));
		const shaderPrecisionFormat = gl.getShaderPrecisionFormat(gl.FRAGMENT_SHADER, gl
			.MEDIUM_INT);
		console.info('shaderPrecisionFormat' + JSON.stringify(shaderPrecisionFormat));
		const type = ((typeof shaderPrecisionFormat) === "object")
		expect(type).assertEqual(true);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0999
	 * @tc.name testGetTexParameter3
	 * @tc.desc Test getTexParameter.
	 */
	it('testGetTexParameter3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getTexParameter test start ...' + JSON.stringify(gl));
		gl.texParameterf(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
		const texParameterValue = gl.getTexParameter(gl.TEXTURE_CUBE_MAP, gl.TEXTURE_MAG_FILTER);
		console.info('texParameterfValue' + texParameterValue);
		expect(texParameterValue).assertEqual(gl.LINEAR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1000
	 * @tc.name testGetActiveUniformBlockParameter1
	 * @tc.desc Test getActiveUniformBlockParameter.
	 */
	it('testGetActiveUniformBlockParameter1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 getActiveUniformBlockParameter test start ...' + JSON.stringify(
			gl2));
		const program = globalFunction();
		const uniformLocation = gl.getUniformLocation(program, "a_Position");
		const int32Array = new Int32Array([1, 2]);
		gl2.uniform1iv(uniformLocation, int32Array);
		const blockIndex = gl2.getUniformBlockIndex(program, 'a_Position');
		gl2.uniformBlockBinding(program, blockIndex, 1);
		const blockSize = gl2.getActiveUniformBlockParameter(program, blockIndex,
			gl.UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER);
		console.info("getActiveUniformBlockParameter blockSize" + blockSize);
		const errorCode = gl.getError();
		console.info("jsWebGL getActiveUniformBlockParameter errorCode: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_VALUE);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1001
	 * @tc.name testBufferDataFirst
	 * @tc.desc Test bufferData.
	 */
	it('testBufferDataFirst', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 bufferData test start ...' + JSON.stringify(gl2));
		const buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl2.bufferData(gl2.ELEMENT_ARRAY_BUFFER, 1024, gl2.DYNAMIC_DRAW);
		const errorCode = gl.getError();
		console.info("webgl2test bufferData getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1002
	 * @tc.name testBufferData__2
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData__2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 bufferData test start ...' + JSON.stringify(gl2));
		const buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl2.bufferData(gl2.ELEMENT_ARRAY_BUFFER, 1024, gl2.DYNAMIC_DRAW);
		const errorCode = gl.getError();
		console.info("webgl2test bufferData getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1003
	 * @tc.name testBufferData__3
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData__3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 bufferData test start ...' + JSON.stringify(gl2));
		const buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl2.bufferData(gl2.COPY_READ_BUFFER, 1024, gl2.STREAM_DRAW);
		const errorCode = gl.getError();
		console.info("webgl2test bufferData getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1004
	 * @tc.name testBufferData__4
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData__4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 bufferData test start ...' + JSON.stringify(gl2));
		const buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl2.bufferData(gl2.COPY_READ_BUFFER, 1024, gl2.STREAM_DRAW);
		const errorCode = gl.getError();
		console.info("webgl2test bufferData getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1005
	 * @tc.name testBufferData__3
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData__3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 bufferData test start ...' + JSON.stringify(gl2));
		const buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl2.bufferData(gl2.COPY_READ_BUFFER, 1024, gl2.STREAM_DRAW);
		const errorCode = gl.getError();
		console.info("webgl2test bufferData getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1006
	 * @tc.name testBufferSubData__1
	 * @tc.desc Test bufferData.
	 */
	it('testBufferSubData__1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 bufferData test start ...' + JSON.stringify(gl2));
		gl2.bufferSubData(gl2.ELEMENT_ARRAY_BUFFER, 512, new ArrayBuffer(8));
		const errorCode = gl.getError();
		console.info("webgl2test bufferSubData getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1007
	 * @tc.name testBufferSubData__2
	 * @tc.desc Test bufferData.
	 */
	it('testBufferSubData__2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 bufferData test start ...' + JSON.stringify(gl2));
		gl2.bufferSubData(gl.ARRAY_BUFFER, 512, new ArrayBuffer(8));
		const errorCode = gl.getError();
		console.info("webgl2test bufferSubData getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_VALUE);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1008
	 * @tc.name testBufferSubData__3
	 * @tc.desc Test bufferData.
	 */
	it('testBufferSubData__3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 bufferData test start ...' + JSON.stringify(gl2));
		gl2.bufferSubData(gl.COPY_WRITE_BUFFER, 512, new ArrayBuffer(8));
		const errorCode = gl.getError();
		console.info("webgl2test bufferSubData getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1009
	 * @tc.name testTexImage2D__1
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D__1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texImage2D(gl2.TEXTURE_CUBE_MAP_POSITIVE_X, 0, 32, 512, 512, 0, 32, 32, view);
		const errorCode = gl.getError();
		console.info("webgl2test texImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1010
	 * @tc.name testTexImage2D__2
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D__2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texImage2D(gl2.TEXTURE_CUBE_MAP_POSITIVE_Y, 0, 32, 512, 512, 0, 32, 32, view);
		const errorCode = gl.getError();
		console.info("webgl2test texImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1011
	 * @tc.name testTexImage2D__3
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D__3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texImage2D(gl2.TEXTURE_CUBE_MAP_POSITIVE_Z, 0, 32, 512, 512, 0, 32, 32, view);
		const errorCode = gl.getError();
		console.info("webgl2test texImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1012
	 * @tc.name testTexSubImage2D__1
	 * @tc.desc Test texSubImage2D.
	 */
	it('testTexSubImage2D__1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texSubImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texSubImage2D(gl2.TEXTURE_CUBE_MAP_POSITIVE_X, 0, 0, 0, 512, 512, gl.RED, gl.FLOAT,
			view, 0);
		const errorCode = gl.getError();
		console.info("webgl2test texSubImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1013
	 * @tc.name testTexSubImage2D__2
	 * @tc.desc Test texSubImage2D.
	 */
	it('testTexSubImage2D_2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texSubImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texSubImage2D(gl2.TEXTURE_CUBE_MAP_POSITIVE_Y, 0, 0, 0, 512, 512, gl.RED, gl.FLOAT,
			view, 0);
		const errorCode = gl.getError();
		console.info("webgl2test texSubImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1014
	 * @tc.name testTexSubImage2D__3
	 * @tc.desc Test texSubImage2D.
	 */
	it('testTexSubImage2D_3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texSubImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texSubImage2D(gl2.TEXTURE_CUBE_MAP_POSITIVE_Z, 0, 0, 0, 512, 512, gl.RED, gl.FLOAT,
			view, 0);
		const errorCode = gl.getError();
		console.info("webgl2test texSubImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1015
	 * @tc.name testCompressedTexImage2D__1
	 * @tc.desc Test compressedTexImage2D.
	 */
	it('testCompressedTexImage2D__1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 compressedTexImage2D test start ...' + JSON.stringify(gl2));
		gl2.compressedTexImage2D(gl2.TEXTURE_CUBE_MAP_POSITIVE_X, 0, 0x83F3, 512, 512, 0, gl
			.PIXEL_UNPACK_BUFFER, 0);
		const errorCode = gl.getError();
		console.info("webgl2test compressedTexImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1016
	 * @tc.name testCompressedTexImage2D__2
	 * @tc.desc Test compressedTexImage2D.
	 */
	it('testCompressedTexImage2D__2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 compressedTexImage2D test start ...' + JSON.stringify(gl2));
		gl2.compressedTexImage2D(gl2.TEXTURE_CUBE_MAP_POSITIVE_Y, 0, 0x83F3, 512, 512, 0, gl
			.PIXEL_UNPACK_BUFFER, 0);
		const errorCode = gl.getError();
		console.info("webgl2test compressedTexImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1017
	 * @tc.name testCompressedTexImage2D__3
	 * @tc.desc Test compressedTexImage2D.
	 */
	it('testCompressedTexImage2D__3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 compressedTexImage2D test start ...' + JSON.stringify(gl2));
		gl2.compressedTexImage2D(gl2.TEXTURE_CUBE_MAP_POSITIVE_Z, 0, 0x83F3, 512, 512, 0, gl
			.PIXEL_UNPACK_BUFFER, 0);
		const errorCode = gl.getError();
		console.info("webgl2test compressedTexImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1018
	 * @tc.name testCompressedTexSubImage2D__1
	 * @tc.desc Test compressedTexSubImage2D.
	 */
	it('testCompressedTexSubImage2D__1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 compressedTexSubImage2D test start ...' + JSON.stringify(gl2));
		gl2.compressedTexSubImage2D(gl2.TEXTURE_CUBE_MAP_POSITIVE_X, 0, 256, 256, 512, 512, 0x83F3,
			gl.PIXEL_UNPACK_BUFFER, 0);
		const errorCode = gl.getError();
		console.info("webgl2test compressedTexSubImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1019
	 * @tc.name testCompressedTexSubImage2D__2
	 * @tc.desc Test compressedTexSubImage2D.
	 */
	it('testCompressedTexSubImage2D__2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 compressedTexSubImage2D test start ...' + JSON.stringify(gl2));
		gl2.compressedTexSubImage2D(gl2.TEXTURE_CUBE_MAP_POSITIVE_Y, 0, 256, 256, 512, 512, 0x83F3,
			gl.PIXEL_UNPACK_BUFFER, 0);
		const errorCode = gl.getError();
		console.info("webgl2test compressedTexSubImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1020
	 * @tc.name testCompressedTexSubImage2D__3
	 * @tc.desc Test compressedTexSubImage2D.
	 */
	it('testCompressedTexSubImage2D__3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 compressedTexSubImage2D test start ...' + JSON.stringify(gl2));
		gl2.compressedTexSubImage2D(gl2.TEXTURE_CUBE_MAP_POSITIVE_Z, 0, 256, 256, 512, 512, 0x83F3,
			gl.PIXEL_UNPACK_BUFFER, 0);
		const errorCode = gl.getError();
		console.info("webgl2test compressedTexSubImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1021
	 * @tc.name testHint1
	 * @tc.desc Test hint.
	 */
	it('testHint1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL hint test start ...' + JSON.stringify(gl));
		gl.hint(gl.GENERATE_MIPMAP_HINT, gl.NICEST);
		gl.generateMipmap(gl2.TEXTURE_2D);
		const error = gl.getError();
		console.info('error' + error);
		expect(error).assertEqual(0);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1022
	 * @tc.name testHint2
	 * @tc.desc Test hint.
	 */
	it('testHint2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL hint test start ...' + JSON.stringify(gl));
		gl.hint(gl.GENERATE_MIPMAP_HINT, gl.DONT_CARE);
		gl.generateMipmap(gl2.TEXTURE_2D);
		const error = gl.getError();
		console.info('error' + error);
		expect(error).assertEqual(0);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1023
	 * @tc.name testIsEnabled1
	 * @tc.desc Test isEnabled.
	 */
	it('testIsEnabled1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL isEnabled test start ...' + JSON.stringify(gl));

		gl.enable(gl.CULL_FACE);
		const isEnabledValue1 = gl.isEnabled(gl.CULL_FACE);
		console.info("isEnabledValue1: " + isEnabledValue1);
		expect(isEnabledValue1).assertEqual(true);

		// false
		// 启用或说明给定的能力项，使用WebGLRenderingContext.enable()方法和WebGLRenderingContext.disable()方法：
		gl.disable(gl.CULL_FACE);
		const isEnabledValue2 = gl.isEnabled(gl.CULL_FACE);
		console.info("isEnabledValue2: " + isEnabledValue2);
		expect(isEnabledValue2).assertEqual(false);

		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1024
	 * @tc.name testIsEnabled2
	 * @tc.desc Test isEnabled.
	 */
	it('testIsEnabled2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL isEnabled test start ...' + JSON.stringify(gl));

		gl.enable(gl.BLEND);
		const isEnabledValue1 = gl.isEnabled(gl.BLEND);
		console.info("isEnabledValue1: " + isEnabledValue1);
		expect(isEnabledValue1).assertEqual(true);

		// false
		// 启用或说明给定的能力项，使用WebGLRenderingContext.enable()方法和WebGLRenderingContext.disable()方法：
		gl.disable(gl.BLEND);
		const isEnabledValue2 = gl.isEnabled(gl.BLEND);
		console.info("isEnabledValue2: " + isEnabledValue2);
		expect(isEnabledValue2).assertEqual(false);

		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1025
	 * @tc.name testIsEnabled3
	 * @tc.desc Test isEnabled.
	 */
	it('testIsEnabled3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL isEnabled test start ...' + JSON.stringify(gl));

		gl.enable(gl.POLYGON_OFFSET_FILL);
		const isEnabledValue1 = gl.isEnabled(gl.POLYGON_OFFSET_FILL);
		console.info("isEnabledValue1: " + isEnabledValue1);
		expect(isEnabledValue1).assertEqual(true);

		// false
		// 启用或说明给定的能力项，使用WebGLRenderingContext.enable()方法和WebGLRenderingContext.disable()方法：
		gl.disable(gl.POLYGON_OFFSET_FILL);
		const isEnabledValue2 = gl.isEnabled(gl.POLYGON_OFFSET_FILL);
		console.info("isEnabledValue2: " + isEnabledValue2);
		expect(isEnabledValue2).assertEqual(false);

		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1026
	 * @tc.name testIsEnabled4
	 * @tc.desc Test isEnabled.
	 */
	it('testIsEnabled4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL isEnabled test start ...' + JSON.stringify(gl));

		gl.enable(gl.SAMPLE_COVERAGE);
		const isEnabledValue1 = gl.isEnabled(gl.SAMPLE_COVERAGE);
		console.info("isEnabledValue1: " + isEnabledValue1);
		expect(isEnabledValue1).assertEqual(true);

		// false
		// 启用或说明给定的能力项，使用WebGLRenderingContext.enable()方法和WebGLRenderingContext.disable()方法：
		gl.disable(gl.SAMPLE_COVERAGE);
		const isEnabledValue2 = gl.isEnabled(gl.SAMPLE_COVERAGE);
		console.info("isEnabledValue2: " + isEnabledValue2);
		expect(isEnabledValue2).assertEqual(false);

		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1027
	 * @tc.name testRenderbufferStorage1
	 * @tc.desc Test renderbufferStorage.
	 */
	it('testRenderbufferStorage1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL renderbufferStorage test start ...' + JSON.stringify(gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.renderbufferStorage(gl.RENDERBUFFER, gl.RGB565, 256, 256);

		const width = gl.getRenderbufferParameter(gl.RENDERBUFFER, gl.RENDERBUFFER_INTERNAL_FORMAT);
		expect(width).assertEqual(gl.RGB565);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1028
	 * @tc.name testRenderbufferStorage2
	 * @tc.desc Test renderbufferStorage.
	 */
	it('testRenderbufferStorage2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL renderbufferStorage test start ...' + JSON.stringify(gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.renderbufferStorage(gl.RENDERBUFFER, gl.RGB5_A1, 256, 256);

		const width = gl.getRenderbufferParameter(gl.RENDERBUFFER, gl.RENDERBUFFER_INTERNAL_FORMAT);
		expect(width).assertEqual(gl.RGB5_A1);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1029
	 * @tc.name testRenderbufferStorage3
	 * @tc.desc Test renderbufferStorage.
	 */
	it('testRenderbufferStorage3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL renderbufferStorage test start ...' + JSON.stringify(gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.renderbufferStorage(gl.RENDERBUFFER, gl.DEPTH_STENCIL, 256, 256);

		const width = gl.getRenderbufferParameter(gl.RENDERBUFFER, gl.RENDERBUFFER_INTERNAL_FORMAT);
		expect(width).assertEqual(gl.RGBA4);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1030
	 * @tc.name testStencilFunc1
	 * @tc.desc Test stencilFunc.
	 */
	it('testStencilFunc1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL stencilFunc test start ...' + JSON.stringify(gl));
		gl.enable(gl.STENCIL_TEST);
		gl.stencilFunc(gl.NEVER, 0, 0b1110011);
		const stencilFuncParameter = gl.getParameter(gl.STENCIL_FUNC);
		console.info("stencilFunc stencilFuncParameter: " + stencilFuncParameter);
		expect(stencilFuncParameter).assertEqual(gl.NEVER);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1031
	 * @tc.name testStencilFunc2
	 * @tc.desc Test stencilFunc.
	 */
	it('testStencilFunc2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL stencilFunc test start ...' + JSON.stringify(gl));
		gl.enable(gl.STENCIL_TEST);
		gl.stencilFunc(gl.EQUAL, 0, 0b1110011);
		const stencilFuncParameter = gl.getParameter(gl.STENCIL_FUNC);
		console.info("stencilFunc stencilFuncParameter: " + stencilFuncParameter);
		expect(stencilFuncParameter).assertEqual(gl.EQUAL);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1032
	 * @tc.name testStencilFunc3
	 * @tc.desc Test stencilFunc.
	 */
	it('testStencilFunc3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL stencilFunc test start ...' + JSON.stringify(gl));
		gl.enable(gl.STENCIL_TEST);
		gl.stencilFunc(gl.GREATER, 0, 0b1110011);
		const stencilFuncParameter = gl.getParameter(gl.STENCIL_FUNC);
		console.info("stencilFunc stencilFuncParameter: " + stencilFuncParameter);
		expect(stencilFuncParameter).assertEqual(gl.GREATER);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1033
	 * @tc.name testStencilFuncSeparate1
	 * @tc.desc Test stencilFuncSeparate.
	 */
	it('testStencilFuncSeparate1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL stencilFuncSeparate test start ...' + JSON.stringify(gl));
		gl.enable(gl.STENCIL_TEST);
		gl.stencilFuncSeparate(gl.BACK, gl.EQUAL, 0.2, 1110011);
		const stencilFuncSeparateParameter = gl.getParameter(gl.STENCIL_FUNC);
		console.info("stencilFuncSeparate stencilFuncSeparateParameter: " +
			stencilFuncSeparateParameter);
		expect(stencilFuncSeparateParameter).assertEqual(gl.GREATER);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1034
	 * @tc.name testStencilFuncSeparate2
	 * @tc.desc Test stencilFuncSeparate.
	 */
	it('testStencilFuncSeparate2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL stencilFuncSeparate test start ...' + JSON.stringify(gl));
		gl.enable(gl.STENCIL_TEST);
		gl.stencilFuncSeparate(gl.FRONT_AND_BACK, gl.LEQUAL, 0.2, 1110011);
		const stencilFuncSeparateParameter = gl.getParameter(gl.STENCIL_FUNC);
		console.info("stencilFuncSeparate stencilFuncSeparateParameter: " +
			stencilFuncSeparateParameter);
		expect(stencilFuncSeparateParameter).assertEqual(gl.LEQUAL);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1035
	 * @tc.name testStencilMaskSeparate1
	 * @tc.desc Test stencilMaskSeparate.
	 */
	it('testStencilMaskSeparate1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL stencilMaskSeparate test start ...' + JSON.stringify(gl));
		gl.stencilMaskSeparate(gl.BACK, 110101);
		const stencilMaskSeparateParameter = gl.getParameter(gl.STENCIL_WRITEMASK);
		console.info("stencilMaskSeparate stencilMaskSeparateParameter: " +
			stencilMaskSeparateParameter);
		expect(stencilMaskSeparateParameter).assertEqual(110101);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1036
	 * @tc.name testStencilMaskSeparate2
	 * @tc.desc Test stencilMaskSeparate.
	 */
	it('testStencilMaskSeparate2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL stencilMaskSeparate test start ...' + JSON.stringify(gl));
		gl.stencilMaskSeparate(gl.FRONT_AND_BACK, 110101);
		const stencilMaskSeparateParameter = gl.getParameter(gl.STENCIL_WRITEMASK);
		console.info("stencilMaskSeparate stencilMaskSeparateParameter: " +
			stencilMaskSeparateParameter);
		expect(stencilMaskSeparateParameter).assertEqual(110101);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1037
	 * @tc.name webgl_test_bindFramebuffer_1
	 * @tc.desc Test bindFramebuffer.
	 */
	it('webgl_test_bindFramebuffer_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into bindFramebuffer");

		var framebuffer = gl.createFramebuffer();

		const isFramebuffer1 = gl.isFramebuffer(framebuffer);
		console.info("createFramebuffer --> isFramebuffer1: " + isFramebuffer1);
		expect(isFramebuffer1).assertEqual(false);

		// bind
		gl.bindFramebuffer(gl2.READ_FRAMEBUFFER, framebuffer);

		const isFrameBuffer2 = gl.isFramebuffer(framebuffer);
		console.info("createFramebuffer --> bindFramebuffer --> isFramebuffer_2: " +
			isFrameBuffer2);
		expect(isFrameBuffer2).assertEqual(true);

		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1038
	 * @tc.name webgl_test_bindFramebuffer_2
	 * @tc.desc Test bindFramebuffer.
	 */
	it('webgl_test_bindFramebuffer_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into bindFramebuffer");

		var framebuffer = gl.createFramebuffer();

		const isFramebuffer1 = gl.isFramebuffer(framebuffer);
		console.info("createFramebuffer --> isFramebuffer1: " + isFramebuffer1);
		expect(isFramebuffer1).assertEqual(false);

		// bind
		gl.bindFramebuffer(gl2.DRAW_FRAMEBUFFER, framebuffer);

		const isFrameBuffer2 = gl.isFramebuffer(framebuffer);
		console.info("createFramebuffer --> bindFramebuffer --> isFramebuffer_2: " +
			isFrameBuffer2);
		expect(isFrameBuffer2).assertEqual(true);

		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1039
	 * @tc.name testGetRenderbufferParameter_1
	 * @tc.desc Test getRenderbufferParameter.
	 */
	it('testGetRenderbufferParameter_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getRenderbufferParameter test start ...' + JSON.stringify(gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.renderbufferStorage(gl.RENDERBUFFER, gl.RGBA4, 256, 256);

		const height = gl.getRenderbufferParameter(gl.RENDERBUFFER, gl.RENDERBUFFER_HEIGHT);
		expect(height).assertEqual(256);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1040
	 * @tc.name testGetRenderbufferParameter_2
	 * @tc.desc Test getRenderbufferParameter.
	 */
	it('testGetRenderbufferParameter_2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getRenderbufferParameter test start ...' + JSON.stringify(gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.renderbufferStorage(gl.RENDERBUFFER, gl.RGBA4, 256, 256);

		const size = gl.getRenderbufferParameter(gl.RENDERBUFFER, gl.RENDERBUFFER_BLUE_SIZE);
		expect(size).assertEqual(4);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1041
	 * @tc.name testGetRenderbufferParameter_3
	 * @tc.desc Test getRenderbufferParameter.
	 */
	it('testGetRenderbufferParameter_3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getRenderbufferParameter test start ...' + JSON.stringify(gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.renderbufferStorage(gl.RENDERBUFFER, gl.RGBA4, 256, 256);

		const size = gl.getRenderbufferParameter(gl.RENDERBUFFER, gl.RENDERBUFFER_RED_SIZE);
		expect(size).assertEqual(4);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1042
	 * @tc.name testGetRenderbufferParameter_4
	 * @tc.desc Test getRenderbufferParameter.
	 */
	it('testGetRenderbufferParameter_4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getRenderbufferParameter test start ...' + JSON.stringify(gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.renderbufferStorage(gl.RENDERBUFFER, gl.RGBA4, 256, 256);

		const size = gl.getRenderbufferParameter(gl.RENDERBUFFER, gl.RENDERBUFFER_ALPHA_SIZE);
		expect(size).assertEqual(4);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1043
	 * @tc.name testGetRenderbufferParameter_5
	 * @tc.desc Test getRenderbufferParameter.
	 */
	it('testGetRenderbufferParameter_5', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getRenderbufferParameter test start ...' + JSON.stringify(gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.renderbufferStorage(gl.RENDERBUFFER, gl.RGBA4, 256, 256);
		const size = gl.getRenderbufferParameter(gl.RENDERBUFFER, gl.RENDERBUFFER_DEPTH_SIZE);
		expect(size).assertEqual(0);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1044
	 * @tc.name testGetRenderbufferParameter_6
	 * @tc.desc Test getRenderbufferParameter.
	 */
	it('testGetRenderbufferParameter_6', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getRenderbufferParameter test start ...' + JSON.stringify(gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl2.RENDERBUFFER, renderBuffer);
		gl2.renderbufferStorageMultisample(gl2.RENDERBUFFER, 4, gl.RBGA4, 256, 256);
		const size = gl.getRenderbufferParameter(gl2.RENDERBUFFER, gl2.RENDERBUFFER_SAMPLES);
		expect(size).assertEqual();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1045
	 * @tc.name testGetRenderbufferParameter_7
	 * @tc.desc Test getRenderbufferParameter.
	 */
	it('testGetRenderbufferParameter_7', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getRenderbufferParameter test start ...' + JSON.stringify(gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.renderbufferStorage(gl.RENDERBUFFER, gl.RGBA4, 256, 256);

		const size = gl.getRenderbufferParameter(gl.RENDERBUFFER, gl.RENDERBUFFER_STENCIL_SIZE);
		expect(size).assertEqual(0);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1046
	 * @tc.name webgl_test_copyTexSubImage2D1
	 * @tc.desc Test copyTexSubImage2D.
	 */
	it('webgl_test_copyTexSubImage2D1', 0, async function(done) {
		//initContext();
		console.info("webgltest into copyTexSubImage2D");

		gl.copyTexSubImage2D(gl.TEXTURE_CUBE_MAP_NEGATIVE_X, 0, 0, 0, 0, 0, 16, 16);
		let errorCode = gl.getError();
		console.info("webgltest copyTexSubImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_ENUM);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1047
	 * @tc.name webgl_test_copyTexSubImage2D2
	 * @tc.desc Test copyTexSubImage2D.
	 */
	it('webgl_test_copyTexSubImage2D2', 0, async function(done) {
		//initContext();
		console.info("webgltest into copyTexSubImage2D");
		gl.copyTexSubImage2D(gl.TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, 0, 0, 0, 0, 16, 16);
		let errorCode = gl.getError();
		console.info("webgltest copyTexSubImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1048
	 * @tc.name webgl_test_drawElements_3
	 * @tc.desc Test drawElements.
	 */
	it('webgl_test_drawElements_3', 0, async function(done) {
		//initContext();
		console.info("webgltest into drawElements");

		gl.drawElements(gl.LINE_STRIP, 8, gl.UNSIGNED_BYTE, 0);

		let errorCode = gl.getError();
		console.info("webgltest drawElements getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_FRAMEBUFFER_OPERATION);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1049
	 * @tc.name webgl_test_drawElements_4
	 * @tc.desc Test drawElements.
	 */
	it('webgl_test_drawElements_4', 0, async function(done) {
		//initContext();
		console.info("webgltest into drawElements");

		gl.drawElements(gl.POINTS, 8, gl.UNSIGNED_INT, 0);

		let errorCode = gl.getError();
		console.info("webgltest drawElements getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_FRAMEBUFFER_OPERATION);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1050
	 * @tc.name webgl_test_drawElements_5
	 * @tc.desc Test drawElements.
	 */
	it('webgl_test_drawElements_5', 0, async function(done) {
		//initContext();
		console.info("webgltest into drawElements");

		gl.drawElements(gl.LINE_LOOP, 8, gl.UNSIGNED_INT, 0);

		let errorCode = gl.getError();
		console.info("webgltest drawElements getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_FRAMEBUFFER_OPERATION);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1051
	 * @tc.name webgl_test_drawElements_6
	 * @tc.desc Test drawElements.
	 */
	it('webgl_test_drawElements_6', 0, async function(done) {
		//initContext();
		console.info("webgltest into drawElements");

		gl.drawElements(gl.LINES, 8, gl.UNSIGNED_INT, 0);

		let errorCode = gl.getError();
		console.info("webgltest drawElements getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_FRAMEBUFFER_OPERATION);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1052
	 * @tc.name webgl_test_framebufferRenderbuffer1
	 * @tc.desc Test framebufferRenderbuffer.
	 */
	it('webgl_test_framebufferRenderbuffer1', 0, async function(done) {
		//initContext();
		console.info("webgltest into framebufferRenderbuffer");

		const renderbuffer = gl.createRenderbuffer();
		gl.framebufferRenderbuffer(gl.READ_FRAMEBUFFER, gl.COLOR_ATTACHMENT1, gl.RENDERBUFFER,
			renderbuffer);

		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1053
	 * @tc.name webgl_test_framebufferRenderbuffer2
	 * @tc.desc Test framebufferRenderbuffer.
	 */
	it('webgl_test_framebufferRenderbuffer2', 0, async function(done) {
		//initContext();
		console.info("webgltest into framebufferRenderbuffer");

		const renderbuffer = gl.createRenderbuffer();
		gl.framebufferRenderbuffer(gl.READ_FRAMEBUFFER, gl.COLOR_ATTACHMENT2, gl.RENDERBUFFER,
			renderbuffer);

		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1054
	 * @tc.name webgl_test_framebufferRenderbuffer3
	 * @tc.desc Test framebufferRenderbuffer.
	 */
	it('webgl_test_framebufferRenderbuffer3', 0, async function(done) {
		//initContext();
		console.info("webgltest into framebufferRenderbuffer");

		const renderbuffer = gl.createRenderbuffer();
		gl.framebufferRenderbuffer(gl.READ_FRAMEBUFFER, gl.COLOR_ATTACHMENT3, gl.RENDERBUFFER,
			renderbuffer);

		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1055
	 * @tc.name webgl_test_framebufferRenderbuffer4
	 * @tc.desc Test framebufferRenderbuffer.
	 */
	it('webgl_test_framebufferRenderbuffer4', 0, async function(done) {
		//initContext();
		console.info("webgltest into framebufferRenderbuffer");

		const renderbuffer = gl.createRenderbuffer();
		gl.framebufferRenderbuffer(gl.READ_FRAMEBUFFER, gl.COLOR_ATTACHMENT4, gl.RENDERBUFFER,
			renderbuffer);

		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1056
	 * @tc.name webgl_test_framebufferRenderbuffer5
	 * @tc.desc Test framebufferRenderbuffer.
	 */
	it('webgl_test_framebufferRenderbuffer5', 0, async function(done) {
		//initContext();
		console.info("webgltest into framebufferRenderbuffer");

		const renderbuffer = gl.createRenderbuffer();
		gl.framebufferRenderbuffer(gl.READ_FRAMEBUFFER, gl.COLOR_ATTACHMENT5, gl.RENDERBUFFER,
			renderbuffer);

		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1057
	 * @tc.name webgl_test_framebufferRenderbuffer6
	 * @tc.desc Test framebufferRenderbuffer.
	 */
	it('webgl_test_framebufferRenderbuffer6', 0, async function(done) {
		//initContext();
		console.info("webgltest into framebufferRenderbuffer");

		const renderbuffer = gl.createRenderbuffer();
		gl.framebufferRenderbuffer(gl.READ_FRAMEBUFFER, gl.COLOR_ATTACHMENT6, gl.RENDERBUFFER,
			renderbuffer);

		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1058
	 * @tc.name webgl_test_framebufferRenderbuffer7
	 * @tc.desc Test framebufferRenderbuffer.
	 */
	it('webgl_test_framebufferRenderbuffer7', 0, async function(done) {
		//initContext();
		console.info("webgltest into framebufferRenderbuffer");

		const renderbuffer = gl.createRenderbuffer();
		gl.framebufferRenderbuffer(gl.READ_FRAMEBUFFER, gl.COLOR_ATTACHMENT7, gl.RENDERBUFFER,
			renderbuffer);

		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1059
	 * @tc.name webgl_test_framebufferRenderbuffer8
	 * @tc.desc Test framebufferRenderbuffer.
	 */
	it('webgl_test_framebufferRenderbuffer8', 0, async function(done) {
		//initContext();
		console.info("webgltest into framebufferRenderbuffer");

		const renderbuffer = gl.createRenderbuffer();
		gl.framebufferRenderbuffer(gl.READ_FRAMEBUFFER, gl.COLOR_ATTACHMENT8, gl.RENDERBUFFER,
			renderbuffer);

		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1060
	 * @tc.name webgl_test_framebufferRenderbuffer9
	 * @tc.desc Test framebufferRenderbuffer.
	 */
	it('webgl_test_framebufferRenderbuffer9', 0, async function(done) {
		//initContext();
		console.info("webgltest into framebufferRenderbuffer");

		const renderbuffer = gl.createRenderbuffer();
		gl.framebufferRenderbuffer(gl.READ_FRAMEBUFFER, gl.COLOR_ATTACHMENT9, gl.RENDERBUFFER,
			renderbuffer);

		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1061
	 * @tc.name webgl_test_framebufferRenderbuffer10
	 * @tc.desc Test framebufferRenderbuffer.
	 */
	it('webgl_test_framebufferRenderbuffer10', 0, async function(done) {
		//initContext();
		console.info("webgltest into framebufferRenderbuffer");

		const renderbuffer = gl.createRenderbuffer();
		gl.framebufferRenderbuffer(gl.READ_FRAMEBUFFER, gl.COLOR_ATTACHMENT10, gl.RENDERBUFFER,
			renderbuffer);

		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1062
	 * @tc.name webgl_test_framebufferRenderbuffer11
	 * @tc.desc Test framebufferRenderbuffer.
	 */
	it('webgl_test_framebufferRenderbuffer11', 0, async function(done) {
		//initContext();
		console.info("webgltest into framebufferRenderbuffer");

		const renderbuffer = gl.createRenderbuffer();
		gl.framebufferRenderbuffer(gl.READ_FRAMEBUFFER, gl.COLOR_ATTACHMENT11, gl.RENDERBUFFER,
			renderbuffer);

		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1063
	 * @tc.name webgl_test_framebufferRenderbuffer12
	 * @tc.desc Test framebufferRenderbuffer.
	 */
	it('webgl_test_framebufferRenderbuffer12', 0, async function(done) {
		//initContext();
		console.info("webgltest into framebufferRenderbuffer");

		const renderbuffer = gl.createRenderbuffer();
		gl.framebufferRenderbuffer(gl.READ_FRAMEBUFFER, gl.COLOR_ATTACHMENT12, gl.RENDERBUFFER,
			renderbuffer);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1064
	 * @tc.name webgl_test_framebufferRenderbuffer13
	 * @tc.desc Test framebufferRenderbuffer.
	 */
	it('webgl_test_framebufferRenderbuffer13', 0, async function(done) {
		//initContext();
		console.info("webgltest into framebufferRenderbuffer");

		const renderbuffer = gl.createRenderbuffer();
		gl.framebufferRenderbuffer(gl.READ_FRAMEBUFFER, gl.COLOR_ATTACHMENT13, gl.RENDERBUFFER,
			renderbuffer);

		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1065
	 * @tc.name webgl_test_framebufferRenderbuffer14
	 * @tc.desc Test framebufferRenderbuffer.
	 */
	it('webgl_test_framebufferRenderbuffer14', 0, async function(done) {
		//initContext();
		console.info("webgltest into framebufferRenderbuffer");

		const renderbuffer = gl.createRenderbuffer();
		gl.framebufferRenderbuffer(gl.READ_FRAMEBUFFER, gl.COLOR_ATTACHMENT14, gl.RENDERBUFFER,
			renderbuffer);

		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1066
	 * @tc.name webgl_test_framebufferRenderbuffer15
	 * @tc.desc Test framebufferRenderbuffer.
	 */
	it('webgl_test_framebufferRenderbuffer15', 0, async function(done) {
		//initContext();
		console.info("webgltest into framebufferRenderbuffer");

		const renderbuffer = gl.createRenderbuffer();
		gl.framebufferRenderbuffer(gl.READ_FRAMEBUFFER, gl.COLOR_ATTACHMENT15, gl.RENDERBUFFER,
			renderbuffer);

		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1067
	 * @tc.name testGetParameter_1
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.BLEND_DST_ALPHA);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1068
	 * @tc.name testGetParameter_2
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.ALIASED_POINT_SIZE_RANGE);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1069
	 * @tc.name testGetParameter_3
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.BLUE_BITS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_FRAMEBUFFER_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1070
	 * @tc.name testGetParameter_4
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.ALPHA_BITS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_FRAMEBUFFER_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1071
	 * @tc.name testGetParameter_5
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_5', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.COMPRESSED_TEXTURE_FORMATS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1072
	 * @tc.name testGetParameter_6
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_6', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.ELEMENT_ARRAY_BUFFER_BINDING);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1073
	 * @tc.name testGetParameter_7
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_7', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.FRONT_FACE);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1074
	 * @tc.name testGetParameter_8
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_8', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.GREEN_BITS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_FRAMEBUFFER_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1075
	 * @tc.name testGetParameter_9
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_9', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.IMPLEMENTATION_COLOR_READ_FORMAT);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_FRAMEBUFFER_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1076
	 * @tc.name testGetParameter_10
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_10', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.IMPLEMENTATION_COLOR_READ_TYPE);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_FRAMEBUFFER_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1077
	 * @tc.name testGetParameter_11
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_11', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.MAX_COMBINED_TEXTURE_IMAGE_UNITS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1078
	 * @tc.name testGetParameter_12
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_12', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.MAX_CUBE_MAP_TEXTURE_SIZE);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1079
	 * @tc.name testGetParameter_13
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_13', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.MAX_FRAGMENT_UNIFORM_VECTORS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1080
	 * @tc.name testGetParameter_14
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_14', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.MAX_RENDERBUFFER_SIZE);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1081
	 * @tc.name testGetParameter_15
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_15', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.MAX_TEXTURE_IMAGE_UNITS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1082
	 * @tc.name testGetParameter_16
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_16', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.MAX_TEXTURE_SIZE);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1083
	 * @tc.name testGetParameter_17
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_17', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.MAX_VARYING_VECTORS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1084
	 * @tc.name testGetParameter_18
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_18', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.MAX_VERTEX_ATTRIBS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1085
	 * @tc.name testGetParameter_19
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_19', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.MAX_VERTEX_TEXTURE_IMAGE_UNITS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1086
	 * @tc.name testGetParameter_20
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_20', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.MAX_VERTEX_UNIFORM_VECTORS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1087
	 * @tc.name testGetParameter_21
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_21', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.POLYGON_OFFSET_FACTOR);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1088
	 * @tc.name testGetParameter_22
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_22', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.POLYGON_OFFSET_UNITS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1089
	 * @tc.name testGetParameter_23
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_23', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.RED_BITS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_FRAMEBUFFER_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1090
	 * @tc.name testGetParameter_24
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_24', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.RENDERER);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1091
	 * @tc.name testGetParameter_25
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_25', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.SAMPLE_BUFFERS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1092
	 * @tc.name testGetParameter_26
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_26', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.STENCIL_BACK_FAIL);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1093
	 * @tc.name testGetParameter_27
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_27', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.STENCIL_BACK_FUNC);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1094
	 * @tc.name testGetParameter_28
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_28', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.STENCIL_BACK_PASS_DEPTH_FAIL);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1095
	 * @tc.name testGetParameter_29
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_29', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.STENCIL_BACK_PASS_DEPTH_PASS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1096
	 * @tc.name testGetParameter_30
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_30', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.STENCIL_BACK_REF);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});



	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1097
	 * @tc.name testGetParameter_31
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_31', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.STENCIL_BACK_VALUE_MASK);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1098
	 * @tc.name testGetParameter_32
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_32', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.STENCIL_BACK_WRITEMASK);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1099
	 * @tc.name testGetParameter_33
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_33', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.STENCIL_BITS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_FRAMEBUFFER_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1100
	 * @tc.name testGetParameter_34
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_34', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.STENCIL_PASS_DEPTH_FAIL);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1101
	 * @tc.name testGetParameter_35
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_35', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.STENCIL_PASS_DEPTH_PASS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1102
	 * @tc.name testGetParameter_36
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_36', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.STENCIL_REF);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1103
	 * @tc.name testGetParameter_37
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_37', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.STENCIL_VALUE_MASK);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1104
	 * @tc.name testGetParameter_38
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_38', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.SUBPIXEL_BITS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1105
	 * @tc.name testGetParameter_39
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_39', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.TEXTURE_BINDING_CUBE_MAP);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1106
	 * @tc.name testGetParameter_40
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_40', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.VENDOR);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1107
	 * @tc.name testGetParameter_41
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_41', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.VERSION);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1108
	 * @tc.name testGetParameter_webgl2_1
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.COPY_READ_BUFFER_BINDING);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1109
	 * @tc.name testGetParameter_webgl2_2
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.COPY_WRITE_BUFFER_BINDING);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1110
	 * @tc.name testGetParameter_webgl2_3
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.DRAW_FRAMEBUFFER_BINDING);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1111
	 * @tc.name testGetParameter_webgl2_4
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.FRAGMENT_SHADER_DERIVATIVE_HINT);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1112
	 * @tc.name testGetParameter_webgl2_5
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_5', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.MAX_3D_TEXTURE_SIZE);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1113
	 * @tc.name testGetParameter_webgl2_6
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_6', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.MAX_ARRAY_TEXTURE_LAYERS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1114
	 * @tc.name testGetParameter_webgl2_7
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_7', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.MAX_CLIENT_WAIT_TIMEOUT_WEBGL);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1115
	 * @tc.name testGetParameter_webgl2_8
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_8', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.MAX_COLOR_ATTACHMENTS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1116
	 * @tc.name testGetParameter_webgl2_9
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_9', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1117
	 * @tc.name testGetParameter_webgl2_10
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_10', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.MAX_COMBINED_UNIFORM_BLOCKS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1118
	 * @tc.name testGetParameter_webgl2_11
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_11', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1119
	 * @tc.name testGetParameter_webgl2_12
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_12', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.MAX_DRAW_BUFFERS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1120
	 * @tc.name testGetParameter_webgl2_13
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_13', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.MAX_ELEMENT_INDEX);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1121
	 * @tc.name testGetParameter_webgl2_14
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_14', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.MAX_ELEMENTS_INDICES);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1122
	 * @tc.name testGetParameter_webgl2_15
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_15', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.MAX_ELEMENTS_VERTICES);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1123
	 * @tc.name testGetParameter_webgl2_16
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_16', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.MAX_FRAGMENT_INPUT_COMPONENTS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1124
	 * @tc.name testGetParameter_webgl2_17
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_17', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.MAX_FRAGMENT_UNIFORM_BLOCKS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1125
	 * @tc.name testGetParameter_webgl2_18
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_18', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.MAX_FRAGMENT_UNIFORM_COMPONENTS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1126
	 * @tc.name testGetParameter_webgl2_19
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_19', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.MAX_PROGRAM_TEXEL_OFFSET);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1127
	 * @tc.name testGetParameter_webgl2_20
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_20', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.MAX_SAMPLES);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1128
	 * @tc.name testGetParameter_webgl2_21
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_21', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.MAX_SERVER_WAIT_TIMEOUT);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1129
	 * @tc.name testGetParameter_webgl2_22
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_22', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.MAX_TEXTURE_LOD_BIAS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1130
	 * @tc.name testGetParameter_webgl2_23
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_23', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1131
	 * @tc.name testGetParameter_webgl2_24
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_24', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1132
	 * @tc.name testGetParameter_webgl2_25
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_25', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1133
	 * @tc.name testGetParameter_webgl2_26
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_26', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.MAX_UNIFORM_BLOCK_SIZE);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1134
	 * @tc.name testGetParameter_webgl2_27
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_27', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.MAX_UNIFORM_BUFFER_BINDINGS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1135
	 * @tc.name testGetParameter_webgl2_28
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_28', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.MAX_VARYING_COMPONENTS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1136
	 * @tc.name testGetParameter_webgl2_29
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_29', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.MAX_VERTEX_OUTPUT_COMPONENTS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1137
	 * @tc.name testGetParameter_webgl2_30
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_30', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.MAX_VERTEX_UNIFORM_BLOCKS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1138
	 * @tc.name testGetParameter_webgl2_31
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_31', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.MAX_VERTEX_UNIFORM_COMPONENTS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1139
	 * @tc.name testGetParameter_webgl2_32
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_32', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.MIN_PROGRAM_TEXEL_OFFSET);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1140
	 * @tc.name testGetParameter_webgl2_33
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_33', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.PIXEL_PACK_BUFFER_BINDING);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1141
	 * @tc.name testGetParameter_webgl2_34
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_34', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.PIXEL_UNPACK_BUFFER_BINDING);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1142
	 * @tc.name testGetParameter_webgl2_35
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_35', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.READ_BUFFER);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1143
	 * @tc.name testGetParameter_webgl2_36
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_36', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.READ_FRAMEBUFFER_BINDING);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1144
	 * @tc.name testGetParameter_webgl2_37
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_37', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.SAMPLER_BINDING);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1145
	 * @tc.name testGetParameter_webgl2_38
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_38', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.TEXTURE_BINDING_2D_ARRAY);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1146
	 * @tc.name testGetParameter_webgl2_39
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_39', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.TEXTURE_BINDING_3D);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1147
	 * @tc.name testGetParameter_webgl2_40
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_40', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.TRANSFORM_FEEDBACK_ACTIVE);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1148
	 * @tc.name testGetParameter_webgl2_41
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_41', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.TRANSFORM_FEEDBACK_BINDING);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1149
	 * @tc.name testGetParameter_webgl2_42
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_42', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.TRANSFORM_FEEDBACK_PAUSED);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1150
	 * @tc.name testGetParameter_webgl2_43
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_43', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.UNIFORM_BUFFER_OFFSET_ALIGNMENT);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1151
	 * @tc.name testGetParameter_webgl2_44
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_44', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.VERTEX_ARRAY_BINDING);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1152
	 * @tc.name testGetProgramParameter_1
	 * @tc.desc Test getProgramParameter.
	 */
	it('testGetProgramParameter_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getProgramParameter test start ...' + JSON.stringify(gl));
		const program = globalFunction();
		const attachedShaders = gl.getProgramParameter(program, gl.ACTIVE_UNIFORMS);
		console.info('attachedShaders' + attachedShaders);
		expect(attachedShaders).assertEqual(0);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1153
	 * @tc.name testGetProgramParameter_2
	 * @tc.desc Test getProgramParameter.
	 */
	it('testGetProgramParameter_2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getProgramParameter test start ...' + JSON.stringify(gl));
		const program = globalFunction();
		const attachedShaders = gl.getProgramParameter(program, gl2.TRANSFORM_FEEDBACK_BUFFER_MODE);
		console.info('attachedShaders' + attachedShaders);
		expect(attachedShaders).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1154
	 * @tc.name testGetProgramParameter_3
	 * @tc.desc Test getProgramParameter.
	 */
	it('testGetProgramParameter_3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getProgramParameter test start ...' + JSON.stringify(gl));
		const program = globalFunction();
		const attachedShaders = gl.getProgramParameter(program, gl2.TRANSFORM_FEEDBACK_VARYINGS);
		console.info('attachedShaders' + attachedShaders);
		expect(attachedShaders).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1155
	 * @tc.name testGetProgramParameter_4
	 * @tc.desc Test getProgramParameter.
	 */
	it('testGetProgramParameter_4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getProgramParameter test start ...' + JSON.stringify(gl));
		const program = globalFunction();
		const attachedShaders = gl.getProgramParameter(program, gl2.ACTIVE_UNIFORM_BLOCKS);
		console.info('attachedShaders' + attachedShaders);
		expect(attachedShaders).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1156
	 * @tc.name testTexImage2D_1_1
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_1_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texImage2D(gl.TEXTURE_2D, 0, gl2.RG16F, 1, 1, 0, gl2.RG, gl2.HALF_FLOAT, view, 0);
		const errorCode = gl.getError();
		console.info("webgl2test texImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1157
	 * @tc.name testTexImage2D_1_2
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_1_2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texImage2D(gl.TEXTURE_2D, 0, gl2.R32F, 1, 1, 0, gl2.RED, gl.FLOAT, view, 0);
		const errorCode = gl.getError();
		console.info("webgl2test texImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1158
	 * @tc.name testTexImage2D_1_3
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_1_3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texImage2D(gl.TEXTURE_2D, 0, gl2.RG32F, 1, 1, 0, gl2.RG, gl.FLOAT, view, 0);
		const errorCode = gl.getError();
		console.info("webgl2test texImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1159
	 * @tc.name testTexImage2D_1_4
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_1_4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texImage2D(gl.TEXTURE_2D, 0, gl2.RG8UI, 1, 1, 0, gl2.RG_INTEGER, gl.UNSIGNED_BYTE, view,
			0);
		const errorCode = gl.getError();
		console.info("webgl2test texImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1160
	 * @tc.name testTexImage2D_1_5
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_1_5', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texImage2D(gl.TEXTURE_2D, 0, gl2.RG16UI, 1, 1, 0, gl2.RG, gl2
			.UNSIGNED_INT_2_10_10_10_REV, view, 0);
		const errorCode = gl.getError();
		console.info("webgl2test texImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1161
	 * @tc.name testTexImage2D_1_6
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_1_6', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texImage2D(gl.TEXTURE_2D, 0, gl2.R11F_G11F_B10F, 1, 1, 0, gl.RGB,
			gl2.UNSIGNED_INT_10F_11F_11F_REV, view, 0);
		const errorCode = gl.getError();
		console.info("webgl2test texImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1162
	 * @tc.name testTexImage2D_1_7
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_1_7', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texImage2D(gl.TEXTURE_2D, 0, gl2.RGB9_E5, 1, 1, 0, gl.RGB, gl2.HALF_FLOAT, view, 0);
		const errorCode = gl.getError();
		console.info("webgl2test texImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1163
	 * @tc.name testTexImage2D_1_8
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_1_8', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texImage2D(gl.TEXTURE_2D, 0, gl2.RGB16F, 1, 1, 0, gl.RGB, gl2.HALF_FLOAT, view, 0);
		const errorCode = gl.getError();
		console.info("webgl2test texImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1164
	 * @tc.name testTexImage2D_1_9
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_1_9', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texImage2D(gl.TEXTURE_2D, 0, gl2.RGB32F, 1, 1, 0, gl.RGB, gl.FLOAT, view, 0);
		const errorCode = gl.getError();
		console.info("webgl2test texImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1165
	 * @tc.name testTexImage2D_1_10
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_1_10', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texImage2D(gl.TEXTURE_2D, 0, gl2.SRGB8_ALPHA8, 1, 1, 0, gl.RGBA, gl.UNSIGNED_BYTE, view,
			0);
		const errorCode = gl.getError();
		console.info("webgl2test texImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1166
	 * @tc.name testTexImage2D_1_11
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_1_11', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texImage2D(gl.TEXTURE_2D, 0, gl2.RGB10_A2, 1, 1, 0, gl.RGBA, gl2
			.UNSIGNED_INT_2_10_10_10_REV, view, 0);
		const errorCode = gl.getError();
		console.info("webgl2test texImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1167
	 * @tc.name testTexImage2D_1_12
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_1_12', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texImage2D(gl.TEXTURE_2D, 0, gl2.RGBA16F, 1, 1, 0, gl.RGBA, gl2.HALF_FLOAT, view, 0);
		const errorCode = gl.getError();
		console.info("webgl2test texImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1168
	 * @tc.name testTexImage2D_1_13
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_1_13', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texImage2D(gl.TEXTURE_2D, 0, gl2.RGBA32F, 1, 1, 0, gl.RGBA, gl.FLOAT, view, 0);
		const errorCode = gl.getError();
		console.info("webgl2test texImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1169
	 * @tc.name testTexImage2D_1_14
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_1_14', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texImage2D(gl.TEXTURE_2D, 0, gl2.RGBA8UI, 1, 1, 0, gl2.RGBA_INTEGER, gl.UNSIGNED_BYTE,
			view, 0);
		const errorCode = gl.getError();
		console.info("webgl2test texImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1170
	 * @tc.name testTexImage2D_1_15
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_1_15', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texImage2D(gl.TEXTURE_2D, 0, gl2.RG16UI, 1, 1, 0, gl2.RG, gl2.UNSIGNED_INT_5_9_9_9_REV,
			view, 0);
		const errorCode = gl.getError();
		console.info("webgl2test texImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1171
	 * @tc.name testTexImage2D_1_16
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_1_16', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texImage2D(gl.TEXTURE_2D, 0, gl2.RG16UI, 1, 1, 0, gl2.RG, gl2
			.UNSIGNED_INT_10F_11F_11F_REV, view, 0);
		const errorCode = gl.getError();
		console.info("webgl2test texImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1172
	 * @tc.name testTexImage2D_1_16
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_1_16', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texImage2D(gl.TEXTURE_2D, 0, gl2.RG16UI, 1, 1, 0, gl2.RG, gl2
			.FLOAT_32_UNSIGNED_INT_24_8_REV, view, 0);
		const errorCode = gl.getError();
		console.info("webgl2test texImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1173
	 * @tc.name testTexImage2D_1_18
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_1_18', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texImage2D(gl.TEXTURE_2D, 0, gl2.RG16UI, 1, 1, 0, gl2.RG, gl2.UNSIGNED_INT_24_8, view,
			0);
		const errorCode = gl.getError();
		console.info("webgl2test texImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1174
	 * @tc.name testTexSubImage2D_1_1
	 * @tc.desc Test texSubImage2D.
	 */
	it('testTexSubImage2D_1_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texSubImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texSubImage2D(gl2.TEXTURE_2D, 0, 0, 0, 512, 512, gl2.RG, gl.FLOAT, view, 0);
		const errorCode = gl.getError();
		console.info("webgl2test texSubImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1175
	 * @tc.name testTexSubImage2D_1_2
	 * @tc.desc Test texSubImage2D.
	 */
	it('testTexSubImage2D_1_2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texSubImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texSubImage2D(gl2.TEXTURE_2D, 0, 0, 0, 512, 512, gl2.RED_INTEGER, gl.FLOAT, view, 0);
		const errorCode = gl.getError();
		console.info("webgl2test texSubImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1176
	 * @tc.name testTexSubImage2D_1_3
	 * @tc.desc Test texSubImage2D.
	 */
	it('testTexSubImage2D_1_3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texSubImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texSubImage2D(gl2.TEXTURE_2D, 0, 0, 0, 512, 512, gl2.RG_INTEGER, gl.FLOAT, view, 0);
		const errorCode = gl.getError();
		console.info("webgl2test texSubImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1177
	 * @tc.name testTexSubImage2D_1_4
	 * @tc.desc Test texSubImage2D.
	 */
	it('testTexSubImage2D_1_4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texSubImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texSubImage2D(gl2.TEXTURE_2D, 0, 0, 0, 512, 512, gl2.RGB_INTEGER, gl.FLOAT, view, 0);
		const errorCode = gl.getError();
		console.info("webgl2test texSubImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1178
	 * @tc.name testTexSubImage2D_1_5
	 * @tc.desc Test texSubImage2D.
	 */
	it('testTexSubImage2D_1_5', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texSubImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texSubImage2D(gl2.TEXTURE_2D, 0, 0, 0, 512, 512, gl2.RGBA_INTEGER, gl.FLOAT, view, 0);
		const errorCode = gl.getError();
		console.info("webgl2test texSubImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1179
	 * @tc.name webgl_test_activeTexture4_1
	 * @tc.desc Test activeTexture.
	 */
	it('webgl_test_activeTexture4_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into activeTexture");
		gl.activeTexture(gl.TEXTURE4);
		const activeTextureParameter = gl.getParameter(gl.ACTIVE_TEXTURE);
		console.info("webgltest activeTexture --> getParameter: " + activeTextureParameter);
		expect(activeTextureParameter).assertEqual(33988);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1180
	 * @tc.name webgl_test_activeTexture5_1
	 * @tc.desc Test activeTexture.
	 */
	it('webgl_test_activeTexture5_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into activeTexture");
		gl.activeTexture(gl.TEXTURE5);
		const activeTextureParameter = gl.getParameter(gl.ACTIVE_TEXTURE);
		console.info("webgltest activeTexture --> getParameter: " + activeTextureParameter);
		expect(activeTextureParameter).assertEqual(33989);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1181
	 * @tc.name webgl_test_activeTexture6_1
	 * @tc.desc Test activeTexture.
	 */
	it('webgl_test_activeTexture6_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into activeTexture");
		gl.activeTexture(gl.TEXTURE6);
		const activeTextureParameter = gl.getParameter(gl.ACTIVE_TEXTURE);
		console.info("webgltest activeTexture --> getParameter: " + activeTextureParameter);
		expect(activeTextureParameter).assertEqual(33990);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1182
	 * @tc.name webgl_test_activeTexture7_1
	 * @tc.desc Test activeTexture.
	 */
	it('webgl_test_activeTexture7_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into activeTexture");
		gl.activeTexture(gl.TEXTURE7);
		const activeTextureParameter = gl.getParameter(gl.ACTIVE_TEXTURE);
		console.info("webgltest activeTexture --> getParameter: " + activeTextureParameter);
		expect(activeTextureParameter).assertEqual(33991);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1183
	 * @tc.name webgl_test_activeTexture8_1
	 * @tc.desc Test activeTexture.
	 */
	it('webgl_test_activeTexture8_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into activeTexture");
		gl.activeTexture(gl.TEXTURE8);
		const activeTextureParameter = gl.getParameter(gl.ACTIVE_TEXTURE);
		console.info("webgltest activeTexture --> getParameter: " + activeTextureParameter);
		expect(activeTextureParameter).assertEqual(33992);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1184
	 * @tc.name webgl_test_activeTexture9_1
	 * @tc.desc Test activeTexture.
	 */
	it('webgl_test_activeTexture9_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into activeTexture");
		gl.activeTexture(gl.TEXTURE9);
		const activeTextureParameter = gl.getParameter(gl.ACTIVE_TEXTURE);
		console.info("webgltest activeTexture --> getParameter: " + activeTextureParameter);
		expect(activeTextureParameter).assertEqual(33993);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1185
	 * @tc.name webgl_test_activeTexture10_1
	 * @tc.desc Test activeTexture.
	 */
	it('webgl_test_activeTexture10_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into activeTexture");
		gl.activeTexture(gl.TEXTURE10);
		const activeTextureParameter = gl.getParameter(gl.ACTIVE_TEXTURE);
		console.info("webgltest activeTexture --> getParameter: " + activeTextureParameter);
		expect(activeTextureParameter).assertEqual(33994);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1186
	 * @tc.name webgl_test_activeTexture11_1
	 * @tc.desc Test activeTexture.
	 */
	it('webgl_test_activeTexture11_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into activeTexture");
		gl.activeTexture(gl.TEXTURE11);
		const activeTextureParameter = gl.getParameter(gl.ACTIVE_TEXTURE);
		console.info("webgltest activeTexture --> getParameter: " + activeTextureParameter);
		expect(activeTextureParameter).assertEqual(33995);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1187
	 * @tc.name webgl_test_activeTexture12_1
	 * @tc.desc Test activeTexture.
	 */
	it('webgl_test_activeTexture12_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into activeTexture");
		gl.activeTexture(gl.TEXTURE12);
		const activeTextureParameter = gl.getParameter(gl.ACTIVE_TEXTURE);
		console.info("webgltest activeTexture --> getParameter: " + activeTextureParameter);
		expect(activeTextureParameter).assertEqual(33996);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1188
	 * @tc.name webgl_test_activeTexture13_1
	 * @tc.desc Test activeTexture.
	 */
	it('webgl_test_activeTexture13_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into activeTexture");
		gl.activeTexture(gl.TEXTURE13);
		const activeTextureParameter = gl.getParameter(gl.ACTIVE_TEXTURE);
		console.info("webgltest activeTexture --> getParameter: " + activeTextureParameter);
		expect(activeTextureParameter).assertEqual(33997);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1189
	 * @tc.name webgl_test_activeTexture14_1
	 * @tc.desc Test activeTexture.
	 */
	it('webgl_test_activeTexture14_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into activeTexture");
		gl.activeTexture(gl.TEXTURE14);
		const activeTextureParameter = gl.getParameter(gl.ACTIVE_TEXTURE);
		console.info("webgltest activeTexture --> getParameter: " + activeTextureParameter);
		expect(activeTextureParameter).assertEqual(33998);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1190
	 * @tc.name webgl_test_activeTexture15_1
	 * @tc.desc Test activeTexture.
	 */
	it('webgl_test_activeTexture15_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into activeTexture");
		gl.activeTexture(gl.TEXTURE15);
		const activeTextureParameter = gl.getParameter(gl.ACTIVE_TEXTURE);
		console.info("webgltest activeTexture --> getParameter: " + activeTextureParameter);
		expect(activeTextureParameter).assertEqual(33999);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1191
	 * @tc.name webgl_test_activeTexture16_1
	 * @tc.desc Test activeTexture.
	 */
	it('webgl_test_activeTexture16_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into activeTexture");
		gl.activeTexture(gl.TEXTURE16);
		const activeTextureParameter = gl.getParameter(gl.ACTIVE_TEXTURE);
		console.info("webgltest activeTexture --> getParameter: " + activeTextureParameter);
		expect(activeTextureParameter).assertEqual(34000);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1192
	 * @tc.name webgl_test_activeTexture17_1
	 * @tc.desc Test activeTexture.
	 */
	it('webgl_test_activeTexture17_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into activeTexture");
		gl.activeTexture(gl.TEXTURE17);
		const activeTextureParameter = gl.getParameter(gl.ACTIVE_TEXTURE);
		console.info("webgltest activeTexture --> getParameter: " + activeTextureParameter);
		expect(activeTextureParameter).assertEqual(34001);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1193
	 * @tc.name webgl_test_activeTexture18_1
	 * @tc.desc Test activeTexture.
	 */
	it('webgl_test_activeTexture18_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into activeTexture");
		gl.activeTexture(gl.TEXTURE18);
		const activeTextureParameter = gl.getParameter(gl.ACTIVE_TEXTURE);
		console.info("webgltest activeTexture --> getParameter: " + activeTextureParameter);
		expect(activeTextureParameter).assertEqual(34002);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1194
	 * @tc.name webgl_test_activeTexture19_1
	 * @tc.desc Test activeTexture.
	 */
	it('webgl_test_activeTexture19_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into activeTexture");
		gl.activeTexture(gl.TEXTURE19);
		const activeTextureParameter = gl.getParameter(gl.ACTIVE_TEXTURE);
		console.info("webgltest activeTexture --> getParameter: " + activeTextureParameter);
		expect(activeTextureParameter).assertEqual(34003);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1195
	 * @tc.name webgl_test_activeTexture20_1
	 * @tc.desc Test activeTexture.
	 */
	it('webgl_test_activeTexture20_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into activeTexture");
		gl.activeTexture(gl.TEXTURE20);
		const activeTextureParameter = gl.getParameter(gl.ACTIVE_TEXTURE);
		console.info("webgltest activeTexture --> getParameter: " + activeTextureParameter);
		expect(activeTextureParameter).assertEqual(34004);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1196
	 * @tc.name webgl_test_activeTexture21_1
	 * @tc.desc Test activeTexture.
	 */
	it('webgl_test_activeTexture21_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into activeTexture");
		gl.activeTexture(gl.TEXTURE21);
		const activeTextureParameter = gl.getParameter(gl.ACTIVE_TEXTURE);
		console.info("webgltest activeTexture --> getParameter: " + activeTextureParameter);
		expect(activeTextureParameter).assertEqual(34005);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1197
	 * @tc.name webgl_test_activeTexture22_1
	 * @tc.desc Test activeTexture.
	 */
	it('webgl_test_activeTexture22_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into activeTexture");
		gl.activeTexture(gl.TEXTURE22);
		const activeTextureParameter = gl.getParameter(gl.ACTIVE_TEXTURE);
		console.info("webgltest activeTexture --> getParameter: " + activeTextureParameter);
		expect(activeTextureParameter).assertEqual(34006);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1198
	 * @tc.name webgl_test_activeTexture23_1
	 * @tc.desc Test activeTexture.
	 */
	it('webgl_test_activeTexture23_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into activeTexture");
		gl.activeTexture(gl.TEXTURE23);
		const activeTextureParameter = gl.getParameter(gl.ACTIVE_TEXTURE);
		console.info("webgltest activeTexture --> getParameter: " + activeTextureParameter);
		expect(activeTextureParameter).assertEqual(34007);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1199
	 * @tc.name webgl_test_activeTexture24_1
	 * @tc.desc Test activeTexture.
	 */
	it('webgl_test_activeTexture24_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into activeTexture");
		gl.activeTexture(gl.TEXTURE24);
		const activeTextureParameter = gl.getParameter(gl.ACTIVE_TEXTURE);
		console.info("webgltest activeTexture --> getParameter: " + activeTextureParameter);
		expect(activeTextureParameter).assertEqual(34008);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1200
	 * @tc.name webgl_test_activeTexture25_1
	 * @tc.desc Test activeTexture.
	 */
	it('webgl_test_activeTexture25_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into activeTexture");
		gl.activeTexture(gl.TEXTURE25);
		const activeTextureParameter = gl.getParameter(gl.ACTIVE_TEXTURE);
		console.info("webgltest activeTexture --> getParameter: " + activeTextureParameter);
		expect(activeTextureParameter).assertEqual(34009);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1201
	 * @tc.name webgl_test_activeTexture26_1
	 * @tc.desc Test activeTexture.
	 */
	it('webgl_test_activeTexture26_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into activeTexture");
		gl.activeTexture(gl.TEXTURE26);
		const activeTextureParameter = gl.getParameter(gl.ACTIVE_TEXTURE);
		console.info("webgltest activeTexture --> getParameter: " + activeTextureParameter);
		expect(activeTextureParameter).assertEqual(34010);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1202
	 * @tc.name webgl_test_activeTexture27_1
	 * @tc.desc Test activeTexture.
	 */
	it('webgl_test_activeTexture27_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into activeTexture");
		gl.activeTexture(gl.TEXTURE27);
		const activeTextureParameter = gl.getParameter(gl.ACTIVE_TEXTURE);
		console.info("webgltest activeTexture --> getParameter: " + activeTextureParameter);
		expect(activeTextureParameter).assertEqual(34011);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1203
	 * @tc.name webgl_test_activeTexture28_1
	 * @tc.desc Test activeTexture.
	 */
	it('webgl_test_activeTexture28_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into activeTexture");
		gl.activeTexture(gl.TEXTURE28);
		const activeTextureParameter = gl.getParameter(gl.ACTIVE_TEXTURE);
		console.info("webgltest activeTexture --> getParameter: " + activeTextureParameter);
		expect(activeTextureParameter).assertEqual(34012);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1204
	 * @tc.name webgl_test_activeTexture29_1
	 * @tc.desc Test activeTexture.
	 */
	it('webgl_test_activeTexture29_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into activeTexture");
		gl.activeTexture(gl.TEXTURE29);
		const activeTextureParameter = gl.getParameter(gl.ACTIVE_TEXTURE);
		console.info("webgltest activeTexture --> getParameter: " + activeTextureParameter);
		expect(activeTextureParameter).assertEqual(34013);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1205
	 * @tc.name webgl_test_activeTexture30_1
	 * @tc.desc Test activeTexture.
	 */
	it('webgl_test_activeTexture30_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into activeTexture");
		gl.activeTexture(gl.TEXTURE30);
		const activeTextureParameter = gl.getParameter(gl.ACTIVE_TEXTURE);
		console.info("webgltest activeTexture --> getParameter: " + activeTextureParameter);
		expect(activeTextureParameter).assertEqual(34014);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1206
	 * @tc.name webgl_test_activeTexture31_1
	 * @tc.desc Test activeTexture.
	 */
	it('webgl_test_activeTexture31_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into activeTexture");
		gl.activeTexture(gl.TEXTURE31);
		const activeTextureParameter = gl.getParameter(gl.ACTIVE_TEXTURE);
		console.info("webgltest activeTexture --> getParameter: " + activeTextureParameter);
		expect(activeTextureParameter).assertEqual(34015);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1207
	 * @tc.name webgl_test_blendEquation_7
	 * @tc.desc Test blendEquation.
	 */
	it('webgl_test_blendEquation_7', 0, async function(done) {
		//initContext();
		console.info("webgltest into blendEquation");

		gl.blendEquation(gl2.MIN);

		const blendEquationValue = gl.getParameter(gl.BLEND_EQUATION_RGB);
		console.info("blendEquation --> getParameter: " + blendEquationValue);
		expect(blendEquationValue).assertEqual(32775);

		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1208
	 * @tc.name webgl_test_blendEquation_2
	 * @tc.desc Test blendEquation.
	 */
	it('webgl_test_blendEquation_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into blendEquation");

		gl.blendEquation(gl2.MAX);

		const blendEquationValue = gl.getParameter(gl.BLEND_EQUATION_RGB);
		console.info("blendEquation --> getParameter: " + blendEquationValue);
		expect(blendEquationValue).assertEqual(32776);

		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1209
	 * @tc.name webgl_test_copyTexImage2D_1
	 * @tc.desc Test copyTexImage2D.
	 */
	it('webgl_test_copyTexImage2D_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into copyTexImage2D");
		gl.copyTexImage2D(gl.TEXTURE_2D, 0, gl.LUMINANCE_ALPHA, 0, 0, 512, 512, 0);
		let errorCode = gl.getError();
		console.info("webgltest copyTexImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_FRAMEBUFFER_OPERATION);

		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1210
	 * @tc.name webgl_test_disable_40
	 * @tc.desc Test disable.
	 */
	it('webgl_test_disable_40', 0, async function(done) {
		//initContext();
		console.info("webgltest into disable");

		gl.disable(gl2.RASTERIZER_DISCARD);
		// 要检查功能是否被禁用，请使用以下WebGLRenderingContext.isEnabled()方法：
		const isEnabled = gl.isEnabled(gl.DITHER);
		// false
		console.info("webgltest disable isEnabled: " + isEnabled);
		expect(isEnabled).assertEqual(true);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1211
	 * @tc.name webgl_test_clientWaitSync_2
	 * @tc.desc Test clientWaitSync.
	 */
	it('webgl_test_clientWaitSync_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into clientWaitSync");
		let sync = gl2.fenceSync(0x9117, 0);
		let status = gl2.clientWaitSync(sync, gl.SYNC_FLUSH_COMMANDS_BIT, 0);
		gl2.waitSync(sync, 0, -1);
		const isSync1 = gl2.isSync(sync);
		console.info("webgltest fenceSync isSync1: " + isSync1);
		expect(isSync1).assertEqual(true);

		console.info("webgltest fenceSync clientWaitSync: " + status);

		expect(status).assertEqual(0);

		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1212
	 * @tc.name webgl_test_getSyncParameter_1
	 * @tc.desc Test getSyncParameter.
	 */
	it('webgl_test_getSyncParameter_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into getSyncParameter");

		console.info("webgltest into isSync");
		let sync = gl2.fenceSync(0x9117, 0);
		let status = gl2.clientWaitSync(sync, 0, 0);
		gl2.waitSync(sync, 0, -1);
		const isSync1 = gl2.isSync(sync);
		console.info("webgltest fenceSync isSync1: " + isSync1);
		expect(isSync1).assertEqual(true);
		const syncParameter = gl2.getSyncParameter(sync, 0x9112);

		console.info("webgltest fenceSync getSyncParameter: " + syncParameter);
		expect(syncParameter).assertEqual(gl2.SYNC_FENCE);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1213
	 * @tc.name testReadPixelsFirst_1
	 * @tc.desc Test readPixels.
	 */
	it('testReadPixelsFirst_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 readPixels test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.readPixels(0, 0, 512, 512, gl2.RGBA, gl2.UNSIGNED_SHORT_5_6_5, view);
		const errorCode = gl.getError();
		console.info("webgl2test readPixels getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1214
	 * @tc.name testReadPixelsFirst_2
	 * @tc.desc Test readPixels.
	 */
	it('testReadPixelsFirst_2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 readPixels test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.readPixels(0, 0, 512, 512, gl2.RGBA, gl2.UNSIGNED_SHORT_4_4_4_4, view);
		const errorCode = gl.getError();
		console.info("webgl2test readPixels getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1215
	 * @tc.name testReadPixelsFirst_3
	 * @tc.desc Test readPixels.
	 */
	it('testReadPixelsFirst_3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 readPixels test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.readPixels(0, 0, 512, 512, gl2.RGBA, gl2.UNSIGNED_SHORT_5_5_5_1, view);
		const errorCode = gl.getError();
		console.info("webgl2test readPixels getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1216
	 * @tc.name testReadPixelsFirst_4
	 * @tc.desc Test readPixels.
	 */
	it('testReadPixelsFirst_4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 readPixels test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.readPixels(0, 0, 512, 512, gl2.RGBA, gl2.SHORT, view);
		const errorCode = gl.getError();
		console.info("webgl2test readPixels getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1217
	 * @tc.name testReadPixelsFirst_5
	 * @tc.desc Test readPixels.
	 */
	it('testReadPixelsFirst_5', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 readPixels test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.readPixels(0, 0, 512, 512, gl2.RGBA, gl2.INT, view);
		const errorCode = gl.getError();
		console.info("webgl2test readPixels getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1218
	 * @tc.name testTexImage2D_10_1
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_10_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.R8_SNORM, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1219
	 * @tc.name testTexImage2D_11_1
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_11_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.RG8_SNORM, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1220
	 * @tc.name testTexImage2D_11_0
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_11_0', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.RGB8_SNORM, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1221
	 * @tc.name testTexImage2D_11_1
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_11_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.RGBA8_SNORM, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1222
	 * @tc.name testTexImage2D_11_2
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_11_2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.RGB10_A2UI, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1223
	 * @tc.name testTexImage2D_11_3
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_11_3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.SRGB8, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1224
	 * @tc.name testTexImage2D_11_4
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_11_4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.R32F, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1225
	 * @tc.name testTexImage2D_11_5
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_11_5', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.RG8I, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1226
	 * @tc.name testTexImage2D_11_6
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_11_6', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.RG16I, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1227
	 * @tc.name testTexImage2D_11_7
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_11_7', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.RG32I, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1228
	 * @tc.name testTexImage2D_11_8
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_11_8', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.RG32UI, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1229
	 * @tc.name testTexImage2D_11_9
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_11_9', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.RGB8I, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1230
	 * @tc.name testTexImage2D_11_10
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_11_10', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.RGB16I, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1231
	 * @tc.name testTexImage2D_11_11
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_11_11', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.RGB16UI, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1232
	 * @tc.name testTexImage2D_11_12
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_11_12', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.RGB32I, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1233
	 * @tc.name testTexImage2D_11_13
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_11_13', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.RGB32UI, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1234
	 * @tc.name testTexImage2D_11_14
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_11_14', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.RGBA8I, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1235
	 * @tc.name testTexImage2D_11_15
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_11_15', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.RGBA16I, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1236
	 * @tc.name testTexImage2D_11_16
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_11_16', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.RGBA16UI, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1237
	 * @tc.name testTexImage2D_11_17
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_11_17', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.RGBA32I, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1238
	 * @tc.name testTexImage2D_11_18
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_11_18', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.RGBA32UI, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1239
	 * @tc.name testTexImage2D_11_19
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_11_19', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.DEPTH_COMPONENT, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE,
			z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1240
	 * @tc.name testRenderbufferStorageMultisample_1
	 * @tc.desc Test renderbufferStorageMultisample.
	 */
	it('testRenderbufferStorageMultisample_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testRenderbufferStorageMultisample test start ...66');
		gl2.renderbufferStorageMultisample(gl.RENDERBUFFER, 4, gl.DEPTH_COMPONENT16, 256, 256);
		const renderbufferStorageMultisampleError = gl.getError();
		console.info("renderbufferStorageMultisampleError: " + renderbufferStorageMultisampleError);
		expect(renderbufferStorageMultisampleError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1241
	 * @tc.name testRenderbufferStorageMultisample_2
	 * @tc.desc Test renderbufferStorageMultisample.
	 */
	it('testRenderbufferStorageMultisample_2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testRenderbufferStorageMultisample test start ...66');
		gl2.renderbufferStorageMultisample(gl.RENDERBUFFER, 4, gl.DEPTH_COMPONENT24, 256, 256);
		const renderbufferStorageMultisampleError = gl.getError();
		console.info("renderbufferStorageMultisampleError: " + renderbufferStorageMultisampleError);
		expect(renderbufferStorageMultisampleError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1242
	 * @tc.name testRenderbufferStorageMultisample_3
	 * @tc.desc Test renderbufferStorageMultisample.
	 */
	it('testRenderbufferStorageMultisample_3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testRenderbufferStorageMultisample test start ...66');
		gl2.renderbufferStorageMultisample(gl.RENDERBUFFER, 4, gl.DEPTH_COMPONENT32F, 256, 256);
		const renderbufferStorageMultisampleError = gl.getError();
		console.info("renderbufferStorageMultisampleError: " + renderbufferStorageMultisampleError);
		expect(renderbufferStorageMultisampleError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1243
	 * @tc.name testRenderbufferStorageMultisample_4
	 * @tc.desc Test renderbufferStorageMultisample.
	 */
	it('testRenderbufferStorageMultisample_4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testRenderbufferStorageMultisample test start ...66');
		gl2.renderbufferStorageMultisample(gl.RENDERBUFFER, 4, gl.DEPTH24_STENCIL8, 256, 256);
		const renderbufferStorageMultisampleError = gl.getError();
		console.info("renderbufferStorageMultisampleError: " + renderbufferStorageMultisampleError);
		expect(renderbufferStorageMultisampleError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1244
	 * @tc.name testRenderbufferStorageMultisample_5
	 * @tc.desc Test renderbufferStorageMultisample.
	 */
	it('testRenderbufferStorageMultisample_5', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testRenderbufferStorageMultisample test start ...66');
		gl2.renderbufferStorageMultisample(gl.RENDERBUFFER, 4, gl.DEPTH32F_STENCIL8, 256, 256);
		const renderbufferStorageMultisampleError = gl.getError();
		console.info("renderbufferStorageMultisampleError: " + renderbufferStorageMultisampleError);
		expect(renderbufferStorageMultisampleError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1245
	 * @tc.name testRenderbufferStorageMultisample_6
	 * @tc.desc Test renderbufferStorageMultisample.
	 */
	it('testRenderbufferStorageMultisample_6', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testRenderbufferStorageMultisample test start ...66');
		gl2.renderbufferStorageMultisample(gl.RENDERBUFFER, 4, gl.STENCIL_INDEX8, 256, 256);
		const renderbufferStorageMultisampleError = gl.getError();
		console.info("renderbufferStorageMultisampleError: " + renderbufferStorageMultisampleError);
		expect(renderbufferStorageMultisampleError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1246
	 * @tc.name testTransformFeedbackVaryings_7
	 * @tc.desc Test transformFeedbackVaryings.
	 */
	it('testTransformFeedbackVaryings_7', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTransformFeedbackVaryings test start ...66');
		console.info('jsWebGL testTransformFeedbackVaryings test start ...' + JSON.stringify(gl));
		const program = gl.createProgram();
		var transformFeedbackOutputs = ['gl_Position', 'anotherOutput'];
		gl2.transformFeedbackVaryings(program, transformFeedbackOutputs, gl.SEPARATE_ATTRIBS);
		const transformFeedbackVaryingsError = gl.getError();
		console.info("transformFeedbackVaryingsError: " + transformFeedbackVaryingsError);
		expect(transformFeedbackVaryingsError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1247
	 * @tc.name testGetVertexAttrib_EnumSize
	 * @tc.desc Test getVertexAttrib.
	 */
	it('testGetVertexAttrib_EnumSize', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getVertexAttrib test start ...' + JSON.stringify(gl));
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		const bufferVertexAttrib = gl.getVertexAttrib(0, gl.VERTEX_ATTRIB_ARRAY_SIZE);
		const isBuffer = gl.isBuffer(bufferVertexAttrib);
		console.info("getVertexAttrib: bufferVertexAttrib" + bufferVertexAttrib);
		expect(isBuffer).assertEqual(false);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1248
	 * @tc.name testGetVertexAttrib_EnumStride
	 * @tc.desc Test getVertexAttrib.
	 */
	it('testGetVertexAttrib_EnumStride', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getVertexAttrib test start ...' + JSON.stringify(gl));
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		const bufferVertexAttrib = gl.getVertexAttrib(0, gl.VERTEX_ATTRIB_ARRAY_STRIDE);
		const isBuffer = gl.isBuffer(bufferVertexAttrib);
		console.info("getVertexAttrib: bufferVertexAttrib" + bufferVertexAttrib);
		expect(isBuffer).assertEqual(false);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1249
	 * @tc.name testGetVertexAttrib_EnumType
	 * @tc.desc Test getVertexAttrib.
	 */
	it('testGetVertexAttrib_EnumType', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getVertexAttrib test start ...' + JSON.stringify(gl));
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		const bufferVertexAttrib = gl.getVertexAttrib(0, gl.VERTEX_ATTRIB_ARRAY_TYPE);
		const isBuffer = gl.isBuffer(bufferVertexAttrib);
		console.info("getVertexAttrib: bufferVertexAttrib" + bufferVertexAttrib);
		expect(isBuffer).assertEqual(false);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1250
	 * @tc.name testGetVertexAttrib_EnumNormalized
	 * @tc.desc Test getVertexAttrib.
	 */
	it('testGetVertexAttrib_EnumNormalized', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getVertexAttrib test start ...' + JSON.stringify(gl));
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		const bufferVertexAttrib = gl.getVertexAttrib(0, gl.VERTEX_ATTRIB_ARRAY_NORMALIZED);
		const isBuffer = gl.isBuffer(bufferVertexAttrib);
		console.info("getVertexAttrib: bufferVertexAttrib" + bufferVertexAttrib);
		expect(isBuffer).assertEqual(false);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1251
	 * @tc.name testRenderbufferStorage_DepthComponent16
	 * @tc.desc Test renderbufferStorage.
	 */
	it('testRenderbufferStorage_DepthComponent16', 0, async function(done) {
		//initContext();
		console.info('jsWebGL renderbufferStorage test start ...' + JSON.stringify(gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.renderbufferStorage(gl.RENDERBUFFER, gl.DEPTH_COMPONENT16, 256, 256);

		const width = gl.getRenderbufferParameter(gl.RENDERBUFFER, gl.RENDERBUFFER_INTERNAL_FORMAT);
		expect(width).assertEqual(gl.DEPTH_COMPONENT16);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1252
	 * @tc.name testRenderbufferStorage_StencilIndex8
	 * @tc.desc Test renderbufferStorage
	 */
	it('testRenderbufferStorage_StencilIndex8', 0, async function(done) {
		//initContext();
		console.info('jsWebGL renderbufferStorage test start ...' + JSON.stringify(gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.renderbufferStorage(gl.RENDERBUFFER, gl.STENCIL_INDEX8, 256, 256);

		const width = gl.getRenderbufferParameter(gl.RENDERBUFFER, gl.RENDERBUFFER_INTERNAL_FORMAT);
		expect(width).assertEqual(gl.STENCIL_INDEX8);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1253
	 * @tc.name testStencilOp_KeepReplaceIncrWrap
	 * @tc.desc Test stencilOp
	 */
	it('testStencilOp_KeepReplaceIncrWrap', 0, async function(done) {
		//initContext();
		console.info('jsWebGL stencilOp test start ...' + JSON.stringify(gl));
		gl.enable(gl.STENCIL_TEST);
		gl.stencilOp(gl.KEEP, gl.REPLACE, gl.INCR_WRAP);
		const stencilOpParameter = gl.getParameter(gl.STENCIL_FAIL);
		console.info("stencilOp stencilOpParameter: " + stencilOpParameter);
		expect(stencilOpParameter).assertEqual(gl.KEEP);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1254
	 * @tc.name testStencilOp_KeepReplaceDecrWrap
	 * @tc.desc Test stencilOp
	 */
	it('testStencilOp_KeepReplaceDecrWrap', 0, async function(done) {
		//initContext();
		console.info('jsWebGL stencilOp test start ...' + JSON.stringify(gl));
		gl.enable(gl.STENCIL_TEST);
		gl.stencilOp(gl.KEEP, gl.REPLACE, gl.DECR_WRAP);
		const stencilOpParameter = gl.getParameter(gl.STENCIL_FAIL);
		console.info("stencilOp stencilOpParameter: " + stencilOpParameter);
		expect(stencilOpParameter).assertEqual(gl.KEEP);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1255
	 * @tc.name testTexParameterf_TextureBaseLevel
	 * @tc.desc Test texParameterf.
	 */
	it('testTexParameterf_TextureBaseLevel', 0, async function(done) {
		//initContext();
		console.info('jsWebGL texParameterf test start ...' + JSON.stringify(gl));
		gl.texParameterf(gl.TEXTURE_2D, gl2.TEXTURE_BASE_LEVEL, gl.LINEAR);
		const texParameterValue = gl.getTexParameter(gl.TEXTURE_2D, gl2.TEXTURE_BASE_LEVEL);
		console.info('texParameterfValue' + texParameterValue);
		expect(texParameterValue).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1256
	 * @tc.name testTexParameterf_TextureMaxLevel
	 * @tc.desc Test texParameterf.
	 */
	it('testTexParameterf_TextureMaxLevel', 0, async function(done) {
		//initContext();
		console.info('jsWebGL texParameterf test start ...' + JSON.stringify(gl));
		gl.texParameterf(gl.TEXTURE_2D, gl2.TEXTURE_MAX_LEVEL, gl.LINEAR);
		const texParameterValue = gl.getTexParameter(gl.TEXTURE_2D, gl2.TEXTURE_MAX_LEVEL);
		console.info('texParameterfValue' + texParameterValue);
		expect(texParameterValue).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1257
	 * @tc.name testVertexAttribPointer_Short
	 * @tc.desc Test vertexAttribPointer.
	 */
	it('testVertexAttribPointer_Short', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttribPointer test start ...66');
			console.info('jsWebGL testVertexAttribPointer test start ...' + JSON.stringify(gl));
			gl.vertexAttribPointer(0, 3, gl.SHORT, false, 20, 0);
			const vertexAttribPointerError = gl.getError();
			console.info("vertexAttribPointerError: " + vertexAttribPointerError);
			expect(vertexAttribPointerError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testVertexAttribPointer has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1258
	 * @tc.name testTexStorage3D_Srgb8
	 * @tc.desc Test texStorage3D.
	 */
	it('testTexStorage3D_Srgb8', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexStorage3D test start ...66');
			console.info('jsWebGL testTexStorage3D test start ...' + JSON.stringify(gl));
			gl2.texStorage3D(gl2.TEXTURE_3D, 1, gl2.SRGB8, 256, 256, 256);
			const texStorage3DError = gl.getError();
			console.info("texStorage3DError: " + texStorage3DError);
			expect(texStorage3DError).assertEqual(gl.INVALID_OPERATION);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexStorage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1259
	 * @tc.name testTexStorage3D_Rgba32f
	 * @tc.desc Test texStorage3D.
	 */
	it('testTexStorage3D_Rgba32f', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexStorage3D test start ...66');
			console.info('jsWebGL testTexStorage3D test start ...' + JSON.stringify(gl));
			gl2.texStorage3D(gl2.TEXTURE_3D, 1, gl2.RGBA32F, 256, 256, 256);
			const texStorage3DError = gl.getError();
			console.info("texStorage3DError: " + texStorage3DError);
			expect(texStorage3DError).assertEqual(gl.INVALID_OPERATION);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexStorage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1260
	 * @tc.name testTexStorage3D_Rgb32f
	 * @tc.desc Test texStorage3D.
	 */
	it('testTexStorage3D_Rgb32f', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexStorage3D test start ...66');
			console.info('jsWebGL testTexStorage3D test start ...' + JSON.stringify(gl));
			gl2.texStorage3D(gl2.TEXTURE_3D, 1, gl2.RGB32F, 256, 256, 256);
			const texStorage3DError = gl.getError();
			console.info("texStorage3DError: " + texStorage3DError);
			expect(texStorage3DError).assertEqual(gl.INVALID_OPERATION);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexStorage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1261
	 * @tc.name testTexStorage3D_Rgba16f
	 * @tc.desc Test texStorage3D.
	 */
	it('testTexStorage3D_Rgba16f', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexStorage3D test start ...66');
			console.info('jsWebGL testTexStorage3D test start ...' + JSON.stringify(gl));
			gl2.texStorage3D(gl2.TEXTURE_3D, 1, gl2.RGBA16F, 256, 256, 256);
			const texStorage3DError = gl.getError();
			console.info("texStorage3DError: " + texStorage3DError);
			expect(texStorage3DError).assertEqual(gl.INVALID_OPERATION);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexStorage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1262
	 * @tc.name testTexStorage3D_Rgb16f
	 * @tc.desc Test texStorage3D.
	 */
	it('testTexStorage3D_Rgb16f', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexStorage3D test start ...66');
			console.info('jsWebGL testTexStorage3D test start ...' + JSON.stringify(gl));
			gl2.texStorage3D(gl2.TEXTURE_3D, 1, gl2.RGB16F, 256, 256, 256);
			const texStorage3DError = gl.getError();
			console.info("texStorage3DError: " + texStorage3DError);
			expect(texStorage3DError).assertEqual(gl.INVALID_OPERATION);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexStorage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1263
	 * @tc.name testTexStorage3D_R11FG11FB10F
	 * @tc.desc Test texStorage3D.
	 */
	it('testTexStorage3D_R11FG11FB10F', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexStorage3D test start ...66');
			console.info('jsWebGL testTexStorage3D test start ...' + JSON.stringify(gl));
			gl2.texStorage3D(gl2.TEXTURE_3D, 1, gl2.R11F_G11F_B10F, 256, 256, 256);
			const texStorage3DError = gl.getError();
			console.info("texStorage3DError: " + texStorage3DError);
			expect(texStorage3DError).assertEqual(gl.INVALID_OPERATION);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexStorage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1264
	 * @tc.name testTexStorage3D_RGB9E5
	 * @tc.desc Test texStorage3D.
	 */
	it('testTexStorage3D_RGB9E5', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexStorage3D test start ...66');
			console.info('jsWebGL testTexStorage3D test start ...' + JSON.stringify(gl));
			gl2.texStorage3D(gl2.TEXTURE_3D, 1, gl2.RGB9_E5, 256, 256, 256);
			const texStorage3DError = gl.getError();
			console.info("texStorage3DError: " + texStorage3DError);
			expect(texStorage3DError).assertEqual(gl.INVALID_OPERATION);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexStorage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1265
	 * @tc.name testTexStorage3D_RGBA8UI
	 * @tc.desc Test texStorage3D.
	 */
	it('testTexStorage3D_RGBA8UI', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexStorage3D test start ...66');
			console.info('jsWebGL testTexStorage3D test start ...' + JSON.stringify(gl));
			gl2.texStorage3D(gl2.TEXTURE_3D, 1, gl2.RGBA8UI, 256, 256, 256);
			const texStorage3DError = gl.getError();
			console.info("texStorage3DError: " + texStorage3DError);
			expect(texStorage3DError).assertEqual(gl.INVALID_OPERATION);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexStorage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1266
	 * @tc.name testTexStorage3D_R32F
	 * @tc.desc Test texStorage3D.
	 */
	it('testTexStorage3D_R32F', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexStorage3D test start ...66');
			console.info('jsWebGL testTexStorage3D test start ...' + JSON.stringify(gl));
			gl2.texStorage3D(gl2.TEXTURE_3D, 1, gl2.R32F, 256, 256, 256);
			const texStorage3DError = gl.getError();
			console.info("texStorage3DError: " + texStorage3DError);
			expect(texStorage3DError).assertEqual(gl.INVALID_OPERATION);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexStorage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1267
	 * @tc.name testTexStorage3D_RG16F
	 * @tc.desc Test texStorage3D.
	 */
	it('testTexStorage3D_RG16F', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexStorage3D test start ...66');
			console.info('jsWebGL testTexStorage3D test start ...' + JSON.stringify(gl));
			gl2.texStorage3D(gl2.TEXTURE_3D, 1, gl2.RG16F, 256, 256, 256);
			const texStorage3DError = gl.getError();
			console.info("texStorage3DError: " + texStorage3DError);
			expect(texStorage3DError).assertEqual(gl.INVALID_OPERATION);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexStorage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1268
	 * @tc.name testTexStorage3D_RG32F
	 * @tc.desc Test texStorage3D.
	 */
	it('testTexStorage3D_RG32F', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexStorage3D test start ...66');
			console.info('jsWebGL testTexStorage3D test start ...' + JSON.stringify(gl));
			gl2.texStorage3D(gl2.TEXTURE_3D, 1, gl2.RG32F, 256, 256, 256);
			const texStorage3DError = gl.getError();
			console.info("texStorage3DError: " + texStorage3DError);
			expect(texStorage3DError).assertEqual(gl.INVALID_OPERATION);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexStorage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1269
	 * @tc.name testTexImage3D_SRGB8ALPHA8
	 * @tc.desc Test texImage3D.
	 */
	it('testTexImage3D_SRGB8ALPHA8', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexImage3D test start ...66');
			console.info('jsWebGL testTexImage3D test start ...' + JSON.stringify(gl));
			gl2.texImage3D(gl2.TEXTURE_3D, 0, gl2.SRGB8_ALPHA8, 1, 1, 1, 0, gl.RGBA, gl2
				.UNSIGNED_BYTE, 0);
			const texImage3DError = gl.getError();
			console.info("texImage3DError: " + texImage3DError);
			expect(texImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexImage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1270
	 * @tc.name testTexImage3D_HALFFLOAT
	 * @tc.desc Test texImage3D.
	 */
	it('testTexImage3D_HALFFLOAT', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexImage3D test start ...66');
			console.info('jsWebGL testTexImage3D test start ...' + JSON.stringify(gl));
			gl2.texImage3D(gl2.TEXTURE_3D, 0, gl2.SRGB8_ALPHA8, 1, 1, 1, 0, gl.RGBA, gl2.HALF_FLOAT,
				0);
			const texImage3DError = gl.getError();
			console.info("texImage3DError: " + texImage3DError);
			expect(texImage3DError).assertEqual(gl.INVALID_OPERATION);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexImage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1271
	 * @tc.name testTexImage3D_UNSIGNED_INT_2_10_10_10_REV
	 * @tc.desc Test texImage3D.
	 */
	it('testTexImage3D_UNSIGNED_INT_2_10_10_10_REV', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexImage3D test start ...66');
			console.info('jsWebGL testTexImage3D test start ...' + JSON.stringify(gl));
			gl2.texImage3D(gl2.TEXTURE_3D, 0, gl2.SRGB8_ALPHA8, 1, 1, 1, 0, gl.RGBA,
				gl2.UNSIGNED_INT_2_10_10_10_REV, 0);
			const texImage3DError = gl.getError();
			console.info("texImage3DError: " + texImage3DError);
			expect(texImage3DError).assertEqual(gl.INVALID_OPERATION);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexImage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1272
	 * @tc.name testTexImage3D_UNSIGNED_INT_10F_11F_11F_REV
	 * @tc.desc Test texImage3D.
	 */
	it('testTexImage3D_UNSIGNED_INT_10F_11F_11F_REV', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexImage3D test start ...66');
			console.info('jsWebGL testTexImage3D test start ...' + JSON.stringify(gl));
			gl2.texImage3D(gl2.TEXTURE_3D, 0, gl2.SRGB8_ALPHA8, 1, 1, 1, 0, gl.RGBA,
				gl2.UNSIGNED_INT_10F_11F_11F_REV, 0);
			const texImage3DError = gl.getError();
			console.info("texImage3DError: " + texImage3DError);
			expect(texImage3DError).assertEqual(gl.INVALID_OPERATION);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexImage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1273
	 * @tc.name testTexImage3D_UNSIGNED_INT_5_9_9_9_REV
	 * @tc.desc Test texImage3D.
	 */
	it('testTexImage3D_UNSIGNED_INT_5_9_9_9_REV', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexImage3D test start ...66');
			console.info('jsWebGL testTexImage3D test start ...' + JSON.stringify(gl));
			gl2.texImage3D(gl2.TEXTURE_3D, 0, gl2.SRGB8_ALPHA8, 1, 1, 1, 0, gl.RGBA, gl2
				.UNSIGNED_INT_5_9_9_9_REV, 0);
			const texImage3DError = gl.getError();
			console.info("texImage3DError: " + texImage3DError);
			expect(texImage3DError).assertEqual(gl.INVALID_OPERATION);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexImage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1274
	 * @tc.name testTexImage3D_UNSIGNED_INT_24_8
	 * @tc.desc Test texImage3D.
	 */
	it('testTexImage3D_UNSIGNED_INT_24_8', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexImage3D test start ...66');
			console.info('jsWebGL testTexImage3D test start ...' + JSON.stringify(gl));
			gl2.texImage3D(gl2.TEXTURE_3D, 0, gl2.SRGB8_ALPHA8, 1, 1, 1, 0, gl.RGBA, gl2
				.UNSIGNED_INT_24_8, 0);
			const texImage3DError = gl.getError();
			console.info("texImage3DError: " + texImage3DError);
			expect(texImage3DError).assertEqual(gl.INVALID_OPERATION);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexImage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1275
	 * @tc.name testGetParameter_webgl2_45
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_45', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.DRAW_BUFFER0);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1276
	 * @tc.name testGetParameter_webgl2_46
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_46', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.DRAW_BUFFER1);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1277
	 * @tc.name testGetParameter_webgl2_47
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_47', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.DRAW_BUFFER2);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1278
	 * @tc.name testGetParameter_webgl2_48
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_48', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.DRAW_BUFFER3);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1279
	 * @tc.name testGetParameter_webgl2_49
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_49', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.DRAW_BUFFER4);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1280
	 * @tc.name testGetParameter_webgl2_50
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_50', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.DRAW_BUFFER5);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1281
	 * @tc.name testGetParameter_webgl2_51
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_51', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.DRAW_BUFFER6);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1282
	 * @tc.name testGetParameter_webgl2_52
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_52', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.DRAW_BUFFER8);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1283
	 * @tc.name testGetParameter_webgl2_53
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_53', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.DRAW_BUFFER9);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1284
	 * @tc.name testGetParameter_webgl2_54
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_54', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.DRAW_BUFFER10);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1285
	 * @tc.name testGetParameter_webgl2_55
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_55', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.DRAW_BUFFER11);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1286
	 * @tc.name testGetParameter_webgl2_56
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_56', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.DRAW_BUFFER12);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1287
	 * @tc.name testGetParameter_webgl2_57
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_57', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.DRAW_BUFFER13);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1288
	 * @tc.name testGetParameter_webgl2_58
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_58', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.DRAW_BUFFER14);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1289
	 * @tc.name testGetParameter_webgl2_59
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_59', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.DRAW_BUFFER15);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1290
	 * @tc.name testGetParameter_webgl2_60
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_60', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.DRAW_BUFFER7);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1291
	 * @tc.name testGetFramebufferAttachmentParameter3_1
	 * @tc.desc Test getFramebufferAttachmentParameter.
	 */
	it('testGetFramebufferAttachmentParameter3_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getFramebufferAttachmentParameter test start ...' + JSON.stringify(
			gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.RENDERBUFFER,
			renderBuffer);
		const framebuffer = gl.getFramebufferAttachmentParameter(gl.FRAMEBUFFER, gl
			.DEPTH_STENCIL_ATTACHMENT,
			gl2.FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE);
		console.info('Framebuffer' + framebuffer);
		const errorCode = gl.getError();
		console.info("jsWebGL getFramebufferAttachmentParameter errorCode: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1292
	 * @tc.name testGetFramebufferAttachmentParameter3_2
	 * @tc.desc Test getFramebufferAttachmentParameter.
	 */
	it('testGetFramebufferAttachmentParameter3_2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getFramebufferAttachmentParameter test start ...' + JSON.stringify(
			gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.RENDERBUFFER,
			renderBuffer);
		const framebuffer = gl.getFramebufferAttachmentParameter(gl.FRAMEBUFFER, gl
			.DEPTH_STENCIL_ATTACHMENT,
			gl2.FRAMEBUFFER_ATTACHMENT_BLUE_SIZE);
		console.info('Framebuffer' + framebuffer);
		const errorCode = gl.getError();
		console.info("jsWebGL getFramebufferAttachmentParameter errorCode: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1293
	 * @tc.name testGetFramebufferAttachmentParameter3_3
	 * @tc.desc Test getFramebufferAttachmentParameter.
	 */
	it('testGetFramebufferAttachmentParameter3_3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getFramebufferAttachmentParameter test start ...' + JSON.stringify(
			gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.RENDERBUFFER,
			renderBuffer);
		const framebuffer = gl.getFramebufferAttachmentParameter(gl.FRAMEBUFFER, gl
			.DEPTH_STENCIL_ATTACHMENT,
			gl2.FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING);
		console.info('Framebuffer' + framebuffer);
		const errorCode = gl.getError();
		console.info("jsWebGL getFramebufferAttachmentParameter errorCode: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1294
	 * @tc.name testGetFramebufferAttachmentParameter3_4
	 * @tc.desc Test getFramebufferAttachmentParameter.
	 */
	it('testGetFramebufferAttachmentParameter3_4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getFramebufferAttachmentParameter test start ...' + JSON.stringify(
			gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.RENDERBUFFER,
			renderBuffer);
		const framebuffer = gl.getFramebufferAttachmentParameter(gl.FRAMEBUFFER, gl
			.DEPTH_STENCIL_ATTACHMENT,
			gl2.FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE);
		console.info('Framebuffer' + framebuffer);
		const errorCode = gl.getError();
		console.info("jsWebGL getFramebufferAttachmentParameter errorCode: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1295
	 * @tc.name testGetFramebufferAttachmentParameter3_5
	 * @tc.desc Test getFramebufferAttachmentParameter.
	 */
	it('testGetFramebufferAttachmentParameter3_5', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getFramebufferAttachmentParameter test start ...' + JSON.stringify(
			gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.RENDERBUFFER,
			renderBuffer);
		const framebuffer = gl.getFramebufferAttachmentParameter(gl.FRAMEBUFFER, gl
			.DEPTH_STENCIL_ATTACHMENT,
			gl2.FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE);
		console.info('Framebuffer' + framebuffer);
		const errorCode = gl.getError();
		console.info("jsWebGL getFramebufferAttachmentParameter errorCode: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1296
	 * @tc.name testGetFramebufferAttachmentParameter3_6
	 * @tc.desc Test getFramebufferAttachmentParameter.
	 */
	it('testGetFramebufferAttachmentParameter3_6', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getFramebufferAttachmentParameter test start ...' + JSON.stringify(
			gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.RENDERBUFFER,
			renderBuffer);
		const framebuffer = gl.getFramebufferAttachmentParameter(gl.FRAMEBUFFER, gl
			.DEPTH_STENCIL_ATTACHMENT,
			gl2.FRAMEBUFFER_ATTACHMENT_GREEN_SIZE);
		console.info('Framebuffer' + framebuffer);
		const errorCode = gl.getError();
		console.info("jsWebGL getFramebufferAttachmentParameter errorCode: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1297
	 * @tc.name testGetFramebufferAttachmentParameter3_7
	 * @tc.desc Test getFramebufferAttachmentParameter.
	 */
	it('testGetFramebufferAttachmentParameter3_7', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getFramebufferAttachmentParameter test start ...' + JSON.stringify(
			gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.RENDERBUFFER,
			renderBuffer);
		const framebuffer = gl.getFramebufferAttachmentParameter(gl.FRAMEBUFFER, gl
			.DEPTH_STENCIL_ATTACHMENT,
			gl.FRAMEBUFFER_ATTACHMENT_OBJECT_NAME);
		console.info('Framebuffer' + framebuffer);
		const errorCode = gl.getError();
		console.info("jsWebGL getFramebufferAttachmentParameter errorCode: " + errorCode);
		expect(errorCode).assertEqual(0);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1298
	 * @tc.name testGetFramebufferAttachmentParameter3_8
	 * @tc.desc Test getFramebufferAttachmentParameter.
	 */
	it('testGetFramebufferAttachmentParameter3_8', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getFramebufferAttachmentParameter test start ...' + JSON.stringify(
			gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.RENDERBUFFER,
			renderBuffer);
		const framebuffer = gl.getFramebufferAttachmentParameter(gl.FRAMEBUFFER, gl
			.DEPTH_STENCIL_ATTACHMENT,
			gl2.FRAMEBUFFER_ATTACHMENT_RED_SIZE);
		console.info('Framebuffer' + framebuffer);
		const errorCode = gl.getError();
		console.info("jsWebGL getFramebufferAttachmentParameter errorCode: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1299
	 * @tc.name testGetFramebufferAttachmentParameter3_9
	 * @tc.desc Test getFramebufferAttachmentParameter.
	 */
	it('testGetFramebufferAttachmentParameter3_9', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getFramebufferAttachmentParameter test start ...' + JSON.stringify(
			gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.RENDERBUFFER,
			renderBuffer);
		const framebuffer = gl.getFramebufferAttachmentParameter(gl.FRAMEBUFFER, gl
			.DEPTH_STENCIL_ATTACHMENT,
			gl2.FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE);
		console.info('Framebuffer' + framebuffer);
		const errorCode = gl.getError();
		console.info("jsWebGL getFramebufferAttachmentParameter errorCode: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1300
	 * @tc.name testGetFramebufferAttachmentParameter3_10
	 * @tc.desc Test getFramebufferAttachmentParameter.
	 */
	it('testGetFramebufferAttachmentParameter3_10', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getFramebufferAttachmentParameter test start ...' + JSON.stringify(
			gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.RENDERBUFFER,
			renderBuffer);
		const framebuffer = gl.getFramebufferAttachmentParameter(gl.FRAMEBUFFER, gl
			.DEPTH_STENCIL_ATTACHMENT,
			gl.FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE);
		console.info('Framebuffer' + framebuffer);
		const errorCode = gl.getError();
		console.info("jsWebGL getFramebufferAttachmentParameter errorCode: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1301
	 * @tc.name testGetFramebufferAttachmentParameter3_11
	 * @tc.desc Test getFramebufferAttachmentParameter.
	 */
	it('testGetFramebufferAttachmentParameter3_11', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getFramebufferAttachmentParameter test start ...' + JSON.stringify(
			gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.RENDERBUFFER,
			renderBuffer);
		const framebuffer = gl.getFramebufferAttachmentParameter(gl.FRAMEBUFFER, gl
			.DEPTH_STENCIL_ATTACHMENT,
			gl2.FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER);
		console.info('Framebuffer' + framebuffer);
		const errorCode = gl.getError();
		console.info("jsWebGL getFramebufferAttachmentParameter errorCode: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1302
	 * @tc.name testGetFramebufferAttachmentParameter3_12
	 * @tc.desc Test getFramebufferAttachmentParameter.
	 */
	it('testGetFramebufferAttachmentParameter3_12', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getFramebufferAttachmentParameter test start ...' + JSON.stringify(
			gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.RENDERBUFFER,
			renderBuffer);
		const framebuffer = gl.getFramebufferAttachmentParameter(gl.FRAMEBUFFER, gl
			.DEPTH_STENCIL_ATTACHMENT,
			gl.FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL);
		console.info('Framebuffer' + framebuffer);
		const errorCode = gl.getError();
		console.info("jsWebGL getFramebufferAttachmentParameter errorCode: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1303
	 * @tc.name testGetFramebufferAttachmentParameter3_13
	 * @tc.desc Test getFramebufferAttachmentParameter.
	 */
	it('testGetFramebufferAttachmentParameter3_13', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getFramebufferAttachmentParameter test start ...' + JSON.stringify(
			gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.RENDERBUFFER,
			renderBuffer);
		const framebuffer = gl.getFramebufferAttachmentParameter(gl.FRAMEBUFFER, gl
			.DEPTH_STENCIL_ATTACHMENT,
			gl.FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE);
		console.info('Framebuffer' + framebuffer);
		const errorCode = gl.getError();
		console.info("jsWebGL getFramebufferAttachmentParameter errorCode: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1304
	 * @tc.name testGetFramebufferAttachmentParameter3_14
	 * @tc.desc Test getFramebufferAttachmentParameter.
	 */
	it('testGetFramebufferAttachmentParameter3_14', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getFramebufferAttachmentParameter test start ...' + JSON.stringify(
			gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.RENDERBUFFER,
			renderBuffer);
		const framebuffer = gl.getFramebufferAttachmentParameter(gl.FRAMEBUFFER, gl
			.DEPTH_STENCIL_ATTACHMENT,
			gl.FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE);
		console.info('Framebuffer' + framebuffer);
		const errorCode = gl.getError();
		console.info("jsWebGL getFramebufferAttachmentParameter errorCode: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1305
	 * @tc.name testGetFramebufferAttachmentParameter3_15
	 * @tc.desc Test getFramebufferAttachmentParameter.
	 */
	it('testGetFramebufferAttachmentParameter3_15', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getFramebufferAttachmentParameter test start ...' + JSON.stringify(
			gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.RENDERBUFFER,
			renderBuffer);
		const framebuffer = gl.getFramebufferAttachmentParameter(gl.FRAMEBUFFER, gl
			.DEPTH_STENCIL_ATTACHMENT,
			gl2.FRAMEBUFFER_DEFAULT);
		console.info('Framebuffer' + framebuffer);
		const errorCode = gl.getError();
		console.info("jsWebGL getFramebufferAttachmentParameter errorCode: " + errorCode);
		expect(errorCode).assertEqual(0);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1306
	 * @tc.name webgl_test_checkFramebufferStatus2_1
	 * @tc.desc Test checkFramebufferStatus.
	 */
	it('webgl_test_checkFramebufferStatus2_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into checkFramebufferStatus");

		var framebuffer = gl.createFramebuffer();
		gl.bindFramebuffer(gl.FRAMEBUFFER, framebuffer);

		const checkFramebufferStatus = gl.checkFramebufferStatus(gl2
			.FRAMEBUFFER_INCOMPLETE_MULTISAMPLE);
		console.info("createFramebuffer --> bindFramebuffer --> checkFramebufferStatus: " +
			checkFramebufferStatus);
		expect(checkFramebufferStatus).assertEqual(0);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1307
	 * @tc.name webgl_test_checkFramebufferStatus2_2
	 * @tc.desc Test checkFramebufferStatus.
	 */
	it('webgl_test_checkFramebufferStatus2_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into checkFramebufferStatus");

		var framebuffer = gl.createFramebuffer();
		gl.bindFramebuffer(gl.FRAMEBUFFER, framebuffer);

		const checkFramebufferStatus = gl.checkFramebufferStatus(gl
			.FRAMEBUFFER_INCOMPLETE_ATTACHMENT);
		console.info("createFramebuffer --> bindFramebuffer --> checkFramebufferStatus: " +
			checkFramebufferStatus);
		expect(checkFramebufferStatus).assertEqual(0);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1308
	 * @tc.name webgl_test_checkFramebufferStatus2_3
	 * @tc.desc Test checkFramebufferStatus.
	 */
	it('webgl_test_checkFramebufferStatus2_3', 0, async function(done) {
		//initContext();
		console.info("webgltest into checkFramebufferStatus");

		var framebuffer = gl.createFramebuffer();
		gl.bindFramebuffer(gl.FRAMEBUFFER, framebuffer);

		const checkFramebufferStatus = gl.checkFramebufferStatus(gl
			.FRAMEBUFFER_INCOMPLETE_DIMENSIONS);
		console.info("createFramebuffer --> bindFramebuffer --> checkFramebufferStatus: " +
			checkFramebufferStatus);
		expect(checkFramebufferStatus).assertEqual(0);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1309
	 * @tc.name webgl_test_checkFramebufferStatus2_4
	 * @tc.desc Test checkFramebufferStatus.
	 */
	it('webgl_test_checkFramebufferStatus2_4', 0, async function(done) {
		//initContext();
		console.info("webgltest into checkFramebufferStatus");

		var framebuffer = gl.createFramebuffer();
		gl.bindFramebuffer(gl.FRAMEBUFFER, framebuffer);

		const checkFramebufferStatus = gl.checkFramebufferStatus(gl.FRAMEBUFFER_UNSUPPORTED);
		console.info("createFramebuffer --> bindFramebuffer --> checkFramebufferStatus: " +
			checkFramebufferStatus);
		expect(checkFramebufferStatus).assertEqual(0);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1310
	 * @tc.name testGetVertexAttribFirst_1
	 * @tc.desc Test getVertexAttrib.
	 */
	it('testGetVertexAttribFirst_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getVertexAttrib test start ...' + JSON.stringify(gl));
		const vertexBuffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, vertexBuffer);
		const shaderProgram = globalFunction();
		const aVertexPosition = gl.getAttribLocation(shaderProgram, "a_position");

		gl.enableVertexAttribArray(aVertexPosition);
		gl.vertexAttribPointer(aVertexPosition, 3, gl.FLOAT, false, 20, 0);

		gl.drawArrays(gl.TRIANGLES, 0, 8);
		gl.vertexAttrib1f(0, 2.8);
		const type = gl.getVertexAttrib(0, gl.CURRENT_VERTEX_ATTRIB);
		console.info("getVertexAttrib: type" + type);
		expect(type.toString()).assertEqual([2.799999952316284, 0, 0, 1].toString());
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1311
	 * @tc.name testGetParameter_webgl_1
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.BLEND_EQUATION);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(1281);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1312
	 * @tc.name testGetParameter_webgl_2
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl_2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.DEPTH_BITS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(1286);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1313
	 * @tc.name testGetVertexAttribFirst_2
	 * @tc.desc Test getVertexAttrib.
	 */
	it('testGetVertexAttribFirst_2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getVertexAttrib test start ...' + JSON.stringify(gl));
		const vertexBuffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, vertexBuffer);
		const shaderProgram = globalFunction();
		const aVertexPosition = gl.getAttribLocation(shaderProgram, "a_position");

		gl.enableVertexAttribArray(aVertexPosition);
		gl.vertexAttribPointer(aVertexPosition, 3, gl.FLOAT, false, 20, 0);

		gl.drawArrays(gl.TRIANGLES, 0, 8);
		gl.vertexAttrib1f(0, 2.8);
		const type = gl.getVertexAttrib(0, gl2.VERTEX_ATTRIB_ARRAY_INTEGER);
		console.info("getVertexAttrib: type" + type);
		expect(type).assertEqual(false);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1314
	 * @tc.name testGetVertexAttribFirst_3
	 * @tc.desc Test getVertexAttrib.
	 */
	it('testGetVertexAttribFirst_3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getVertexAttrib test start ...' + JSON.stringify(gl));
		const vertexBuffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, vertexBuffer);
		const shaderProgram = globalFunction();
		const aVertexPosition = gl.getAttribLocation(shaderProgram, "a_position");

		gl.enableVertexAttribArray(aVertexPosition);
		gl.vertexAttribPointer(aVertexPosition, 3, gl.FLOAT, false, 20, 0);

		gl.drawArrays(gl.TRIANGLES, 0, 8);
		gl.vertexAttrib1f(0, 2.8);
		const type = gl.getVertexAttrib(0, gl2.VERTEX_ATTRIB_ARRAY_DIVISOR);
		console.info("getVertexAttrib: type" + type);
		expect(type).assertEqual(0);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1315
	 * @tc.name testGetTexParameterFirst_1
	 * @tc.desc Test getTexParameter.
	 */
	it('testGetTexParameterFirst_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getTexParameter test start ...' + JSON.stringify(gl));
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST_MIPMAP_NEAREST);
		const texParameterValue = gl.getTexParameter(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER);
		console.info('texParameterfValue' + texParameterValue);
		expect(texParameterValue).assertEqual(gl.NEAREST_MIPMAP_NEAREST);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1316
	 * @tc.name testGetTexParameterFirst_2
	 * @tc.desc Test getTexParameter.
	 */
	it('testGetTexParameterFirst_2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getTexParameter test start ...' + JSON.stringify(gl));
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST_MIPMAP_LINEAR);
		const texParameterValue = gl.getTexParameter(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER);
		console.info('texParameterfValue' + texParameterValue);
		expect(texParameterValue).assertEqual(gl.NEAREST_MIPMAP_LINEAR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1317
	 * @tc.name testGetTexParameterFirst_3
	 * @tc.desc Test getTexParameter.
	 */
	it('testGetTexParameterFirst_3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getTexParameter test start ...' + JSON.stringify(gl));
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR_MIPMAP_LINEAR);
		const texParameterValue = gl.getTexParameter(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER);
		console.info('texParameterfValue' + texParameterValue);
		expect(texParameterValue).assertEqual(gl.LINEAR_MIPMAP_LINEAR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1318
	 * @tc.name webgl_test_activeTexture_webgl_1
	 * @tc.desc Test activeTexture.
	 */
	it('webgl_test_activeTexture_webgl_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into activeTexture");
		gl.activeTexture(gl.TEXTURE);
		const activeTextureParameter = gl.getParameter(gl.ACTIVE_TEXTURE);
		console.info("webgltest activeTexture --> getParameter: " + activeTextureParameter);
		expect(activeTextureParameter).assertEqual(33984);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1319
	 * @tc.name testGetUniform_11
	 * @tc.desc Test getUniform.
	 */
	it('testGetUniform_11', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getUniform test start ...' + JSON.stringify(gl));
		const programObj = globalFunction();

		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");
		const uniform = gl.getUniform(programObj, uniformLocation);
		let type = [];
		if ((typeof uniform) === 'boolean') {
			type = [gl.BOOL];
		} else if ((typeof uniform) === 'number') {
			type = [gl.SAMPLER_2D, gl.SAMPLER_CUBE];
		} else {
			type = [gl.FLOAT_VEC2, gl.FLOAT_VEC3, gl.FLOAT_VEC4, gl.INT_VEC2, gl.INT_VEC3, gl
				.INT_VEC4, gl.FLOAT_MAT2,
				gl.FLOAT_MAT3, gl.FLOAT_MAT4, gl.BOOL_VEC2, gl.BOOL_VEC3, gl.BOOL_VEC4
			]
		}
		const typeBool = type.length > 0
		console.info('getUniformLocation uniform' + uniform);
		expect(typeBool).assertEqual(true);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1320
	 * @tc.name testGetTexParameterError_1
	 * @tc.desc Test getTexParameter.
	 */
	it('testGetTexParameterError_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getTexParameter test start ...' + JSON.stringify(gl));
		const texParameterValue = gl.getTexParameter(gl.TEXTURE_2D, gl2.TEXTURE_IMMUTABLE_FORMAT);
		console.info('texParameterfValue' + texParameterValue);
		const type = (texParameterValue === gl2.TEXTURE_IMMUTABLE_FORMAT);
		expect(type).assertEqual(false);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1321
	 * @tc.name testGetTexParameterError_2
	 * @tc.desc Test getTexParameter.
	 */
	it('testGetTexParameterError_2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getTexParameter test start ...' + JSON.stringify(gl));
		const texParameterValue = gl.getTexParameter(gl.TEXTURE_2D, gl2.TEXTURE_IMMUTABLE_LEVELS);
		console.info('texParameterfValue' + texParameterValue);
		const type = (texParameterValue === gl2.TEXTURE_IMMUTABLE_LEVELS);
		expect(type).assertEqual(false);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1322
	 * @tc.name testGetUniform_11_1
	 * @tc.desc Test getUniform.
	 */
	it('testGetUniform_11_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getUniform test start ...' + JSON.stringify(gl));
		const programObj = globalFunction();

		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");
		const uniform = gl.getUniform(programObj, uniformLocation);
		let type = [];
		if ((typeof uniform) === 'number') {
			type = [gl2.SIGNED_NORMALIZED, gl2.UNSIGNED_NORMALIZED, gl2.SAMPLER_3D, gl2
				.SAMPLER_2D_SHADOW,
				gl2.SAMPLER_2D_ARRAY, gl2.SAMPLER_2D_ARRAY_SHADOW, gl2.SAMPLER_CUBE_SHADOW, gl2
				.INT_SAMPLER_2D,
				gl2.INT_SAMPLER_3D, gl2.INT_SAMPLER_CUBE, gl2.INT_SAMPLER_2D_ARRAY, gl2
				.UNSIGNED_INT_SAMPLER_2D,
				gl2.UNSIGNED_INT_SAMPLER_3D, gl2.UNSIGNED_INT_SAMPLER_CUBE, gl2
				.UNSIGNED_INT_SAMPLER_2D_ARRAY
			];
		} else {
			type = [gl2.UNSIGNED_INT_VEC2, gl2.UNSIGNED_INT_VEC3,
				gl2.UNSIGNED_INT_VEC4, gl2.FLOAT_MAT2x3, gl2.FLOAT_MAT2x4, gl2.FLOAT_MAT3x2, gl2
				.FLOAT_MAT3x4,
				gl2.FLOAT_MAT4x2, gl2.FLOAT_MAT4x3, gl.BOOL_VEC2, gl.BOOL_VEC3, gl.BOOL_VEC4
			]
		}
		const typeBool = type.length > 0
		console.info('getUniformLocation uniform' + uniform);
		expect(typeBool).assertEqual(true);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1323
	 * @tc.name testTexImage3D_INT_2_10_10_10_REV
	 * @tc.desc Test testTexImage3D.
	 */
	it('testTexImage3D_INT_2_10_10_10_REV', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexImage3D test start ...66');
			console.info('jsWebGL testTexImage3D test start ...' + JSON.stringify(gl));
			gl2.texImage3D(gl2.TEXTURE_3D, 0, gl2.SRGB8_ALPHA8, 1, 1, 1, 0, gl.RGBA, gl2
				.INT_2_10_10_10_REV, 0);
			const texImage3DError = gl.getError();
			console.info("texImage3DError: " + texImage3DError);
			expect(texImage3DError).assertEqual(gl.INVALID_ENUM);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexImage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1324
	 * @tc.name testTexImage2D2_1
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D2_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texImage2D test start ...' + JSON.stringify(gl2));
		gl2.texImage2D(gl2.TEXTURE_2D, 0, gl2.SRGB, 512, 512, 0, gl.RGB, gl.UNSIGNED_BYTE, 0);
		const errorCode = gl.getError();
		console.info("webgl2test texImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1325
	 * @tc.name testGetTexParameter3_1
	 * @tc.desc Test getTexParameter.
	 */
	it('testGetTexParameter3_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getTexParameter test start ...' + JSON.stringify(gl));
		gl.texParameterf(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.REPEAT);
		const texParameterValue = gl.getTexParameter(gl.TEXTURE_CUBE_MAP, gl.TEXTURE_WRAP_S);
		console.info('texParameterfValue' + texParameterValue);
		expect(texParameterValue).assertEqual(gl.REPEAT);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1326
	 * @tc.name testGetTexParameter3_2
	 * @tc.desc Test getTexParameter.
	 */
	it('testGetTexParameter3_2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getTexParameter test start ...' + JSON.stringify(gl));
		gl.texParameterf(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
		const texParameterValue = gl.getTexParameter(gl.TEXTURE_CUBE_MAP, gl.TEXTURE_WRAP_S);
		console.info('texParameterfValue' + texParameterValue);
		expect(texParameterValue).assertEqual(10497);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1327
	 * @tc.name testGetTexParameter3_3
	 * @tc.desc Test getTexParameter.
	 */
	it('testGetTexParameter3_3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getTexParameter test start ...' + JSON.stringify(gl));
		gl.texParameterf(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.MIRRORED_REPEAT);
		const texParameterValue = gl.getTexParameter(gl.TEXTURE_CUBE_MAP, gl.TEXTURE_WRAP_S);
		console.info('texParameterfValue' + texParameterValue);
		expect(texParameterValue).assertEqual(10497);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1328
	 * @tc.name webgl_test_clearBufferfv_22
	 * @tc.desc Test clearBufferfv.
	 */
	it('webgl_test_clearBufferfv_22', 0, async function(done) {
		//initContext();
		console.info("webgltest into clearBufferfv");

		gl2.clearBufferfv(gl2.INVALID_INDEX, 0, [0.0, 0.0, 0.0, 0.0]);

		let errorCode = gl.getError();
		console.info("webgltest webgl_test_clearBufferfv getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_ENUM);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1329
	 * @tc.name testGetFramebufferAttachmentParameter3_10_1
	 * @tc.desc Test getFramebufferAttachmentParameter.
	 */
	it('testGetFramebufferAttachmentParameter3_10_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getFramebufferAttachmentParameter test start ...' + JSON.stringify(
			gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.RENDERBUFFER,
			renderBuffer);
		const framebuffer = gl.getFramebufferAttachmentParameter(gl.FRAMEBUFFER, gl
			.DEPTH_STENCIL_ATTACHMENT,
			gl.FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE);
		console.info('Framebuffer' + framebuffer);
		const errorCode = gl.getError();
		const type = (errorCode === gl.OUT_OF_MEMORY)
		console.info("jsWebGL getFramebufferAttachmentParameter errorCode: " + errorCode);
		expect(type).assertEqual(false);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1330
	 * @tc.name testGetParameter_20_1
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_20_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.BROWSER_DEFAULT_WEBGL);
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1331
	 * @tc.name webgl_test_clearBufferfv_22
	 * @tc.desc Test clearBufferfv.
	 */
	it('webgl_test_clearBufferfv_22', 0, async function(done) {
		//initContext();
		console.info("webgltest into clearBufferfv");

		gl2.clearBufferfv(gl2.INVALID_INDEX, 0, [0.0, 0.0, 0.0, 0.0]);

		let errorCode = gl.getError();
		console.info("webgltest webgl_test_clearBufferfv getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_ENUM);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1332
	 * @tc.name testGetTexParameter3_4
	 * @tc.desc Test getTexParameter.
	 */
	it('testGetTexParameter3_4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getTexParameter test start ...' + JSON.stringify(gl));
		gl.texParameterf(gl.TEXTURE_2D, gl2.TEXTURE_COMPARE_MODE, gl.COMPARE_REF_TO_TEXTURE);
		const texParameterValue = gl.getTexParameter(gl.TEXTURE_CUBE_MAP, gl2.TEXTURE_COMPARE_MODE);
		console.info('texParameterfValue' + texParameterValue);
		expect(texParameterValue).assertEqual(gl.COMPARE_REF_TO_TEXTURE);
		done();
	});
	
	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1335
	 * @tc.name testRangeMin
	 * @tc.desc Test RangeMin.
	 */
	it('testRangeMin', 0, async function(done) {
		console.info('jsWebGL testRangeMin test start');
		gl.getShaderPrecisionFormat(gl.VERTEX_SHADER, gl.MEDIUM_FLOAT).rangeMin;
		gl.getShaderPrecisionFormat(gl.FRAGMENT_SHADER, gl.LOW_INT).rangeMin;
		const min = gl.getShaderPrecisionFormat(gl.FRAGMENT_SHADER, gl.LOW_INT).rangeMin;
		const max = gl.getShaderPrecisionFormat(gl.VERTEX_SHADER, gl.MEDIUM_FLOAT).rangeMax;
		expect(min).assertEqual(24);
		expect(max).assertEqual(127);
		done();
	});
	
	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1335
	 * @tc.name testAttributeBae
	 * @tc.desc Test AttributeBae.
	 */
	it('testAttributeBae', 0, async function(done) {
		console.info('jsWebGL testAttributeBae test start');
		var frameBuffer = gl.createBuffer();
		gl.bindBuffer(gl.FRAMEBUFFER, frameBuffer);
		const framebufferParameter = gl.getParameter(gl.FRAMEBUFFER_BINDING);
		const isFramebuffer = gl.isFramebuffer(framebufferParameter);
		expect(isFamebuffer).assertEqual(true);
		done();
	});
	
	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1335
	 * @tc.name testTypeBase
	 * @tc.desc Test TypeBase.
	 */
	it('testTypeBase', 0, async function(done) {
		console.info('jsWebGL testTypeBasetest start');
		const progamObj = globalFunction();
		const info = gl.getActiveAttrib(programObj, 0);
		info.size = 123;
		info.name = 'name';
		info.type = Number;
		const size = info.size;
		const name  = info.name;
		const type = info.type;
		expect(size).assertEqual(info.size);
		expect(name).assertEqual(info.name);
		expect(type).assertEqual(info.type);
		done();
	});
	
	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1335
	 * @tc.name testWebGLTypeBase
	 * @tc.desc Test WebGLTypeBase.
	 */
	it('testWebGLTypeBase', 0, async function(done) {
		console.info('jsWebGL testWebGLTypeBase test start');
		var x =1 ;
		expect(x).assertEqual(gl.ONE);
		var x1 = 0x8892;
		expect(x1).assertEqual(gl.ARRAY_BUFFER);
		var x2 = 0x0405;
		expect(x2).assertEqual(gl.BACK);
		var x3 = 0;
		expect(x3).assertEqual(gl.NO_ERROR);
		var x4 = 0x0502;
		expect(x4).assertEqual(gl.INVALID_OPERATION);
		var x5 = 0x80AA;
		expect(x5).assertEqual(gl.SAMPLE_COVERAGE_VALUE);
		var x6 = 0x80AB;
		expect(x6).assertEqual(gl.SAMPLE_COVERAGE_INVERT);
		var x7 = 0x1400;
		expect(x7).assertEqual(gl.BYTE);
		var x8 = 0x1404;
		expect(x8).assertEqual(gl.INT);
		var x9 = 0x1406;
		expect(x9).assertEqual(gl.FLOAT);
		var x10 = 0x1907;
		expect(x10).assertEqual(gl.RGB);
		var x11 = 0x8B89;
		expect(x11).assertEqual(gl.ACTIVE_ATTRIBUTES);
		var x12 = 0x0DE1;
		expect(x12).assertEqual(gl.TEXTURE_2D);
		
		var x13 = 0x1702;
		expect(x13).assertEqual(gl.TEXTURE);
		
		var x14 = 0x84C0;
		expect(x14).assertEqual(gl.TEXTURE0);
		var x15 = 0x8D40;
		expect(x15).assertEqual(gl.FRAMEBUFFER);
		var x16 = 0x8D41;
		expect(x16).assertEqual(gl.RENDERBUFFER);
		var x17 = 0x8894;
		expect(x17).assertEqual(gl.ARRAY_BUFFER);
		var x18 = 0;
		expect(x18).assertEqual(gl.NONE);
		var x19 =0x9242;
		expect(x19).assertEqual(gl.CONTEXT_LOST_WEBGL);
		var x20 = 0x8892;
		expect(x20).assertEqual(gl.ARRAY_BUFFER)
		done();
	});
	
	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1335
	 * @tc.name testRendingContextBase
	 * @tc.desc Test RendingContextBase.
	 */
	it('testRendingContextBase', 0, async function(done) {
		console.info('jsWebGL testRendingContextBase test start');
		const buffer = gl.createBuffer();
		expect(typeof(buffer)).assertEqual('object');
		
		var x1 = gl2.COLOR;
		expect(x1).assertEqual(6144);
	
		var x2 = gl2.STENCIL;
		expect(x2).assertEqual(6146);
	
		var x3 = gl2.MIN;
		expect(x3).assertEqual(32775);
	
		var x4 = gl2.MAX;
		expect(x4).assertEqual(32776);
	
		var x5 = gl2.RG;
		expect(x5).assertEqual(33319);
	
		var x6 = gl2.SYNC_CONDITION;
		expect(x6).assertEqual(37139);
	
		var x7 = gl2.SYNC_STATUS;
		expect(x7).assertEqual(37140);
	
		var x8 = gl2.SYNC_FLAGS;
		expect(x8).assertEqual(37141);
		
		done();
		
	});
	
	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1335
	 * @tc.name testAttributeBase
	 * @tc.desc Test testAttributeBase.
	 */
	it('testAttributeBase', 0, async function(done) {
		console.info('jsWebGL testAttributeBase test start');
		var attribute = gl.getContextAttributes();
		expect(atttribute.desynchronized).assertEqual(false);
		expect(atttribute.antialias).assertEqual(true);
		expect(atttribute.premultipliedAlpha).assertEqual(true);
		expect(atttribute.preserveDrawingBuffer).assertEqual(false);
		expect(atttribute.failIfMajorPerformanceCaveat).assertEqual(false);
		done()
	});
})
