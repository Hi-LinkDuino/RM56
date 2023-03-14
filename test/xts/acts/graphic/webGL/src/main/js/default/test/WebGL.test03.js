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
})
