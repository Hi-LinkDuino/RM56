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
})
