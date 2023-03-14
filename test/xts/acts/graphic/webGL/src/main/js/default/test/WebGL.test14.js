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
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1337
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
})
