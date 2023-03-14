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
})
