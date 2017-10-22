#include <stdlib.h>
#include <map>
#include <jni.h>
#include <android/log.h>
#include <android/native_window_jni.h>
#include "CppSurfaceView.h"
#include "ActiveObject.h"

std::map<int, CppSurface> gSurfaceList;

/* シーケンスNo生成 */
static int createSeqNo() {
    static int seqno = 0;
    return seqno++;
}

#ifdef __cplusplus
extern "C" {
#endif

jint Java_com_test_cppsurfaceviewtrans_NativeFunc_surfaceCreated(JNIEnv *pEnv, jclass type, jobject asurface) {
	/* シーケンスNo生成 */
	int SurfaceId = createSeqNo();

	/* メッセージ実行スレッド生成/開始 */
	std::thread th(ActionScheduler::ThreadLoop, SurfaceId);
	th.detach();

	ANativeWindow *pWindow = ANativeWindow_fromSurface(pEnv, asurface);

	while(ActionScheduler::isInitializeing(SurfaceId))
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

	/* 初期化処理を送信 */
	ActionScheduler::sendMessage(SurfaceId, [SurfaceId, pWindow](){
		__android_log_print(ANDROID_LOG_DEBUG, "aaaaa", "aaaaa Initialize() s SurfaceId=%d", SurfaceId);
		/* CppSurfaceクラス生成と登録 */
		gSurfaceList.insert(std::make_pair(SurfaceId, CppSurface(SurfaceId)));

		/* CppSurface初期化 */
		std::map<int, CppSurface>::iterator fingitr = gSurfaceList.find(SurfaceId);
		if(fingitr == gSurfaceList.end())
			return -1;

		CppSurface &cppsurface = fingitr->second;
		cppsurface.mWindow = pWindow;
		cppsurface.initEGL(pWindow);
		cppsurface.initGL();
		__android_log_print(ANDROID_LOG_DEBUG, "aaaaa", "aaaaa Initialize() e SurfaceId=%d", SurfaceId);
  		return 0;
	});

	/* シーケンスNo返却 */
    return SurfaceId;
}

void Java_com_test_cppsurfaceviewtrans_NativeFunc_surfaceChanged(JNIEnv *pEnv, jclass type, jint SurfaceId, jint width, jint height) {
	/* Viewport設定を送信 */
	ActionScheduler::sendMessage(SurfaceId, [SurfaceId, width, height](){
		__android_log_print(ANDROID_LOG_DEBUG, "aaaaa", "aaaaa Viewport設定() s SurfaceId=%d w=%d h=%d", SurfaceId, width ,height);

		std::map<int, CppSurface>::iterator fingitr = gSurfaceList.find(SurfaceId);
		if(fingitr == gSurfaceList.end())
			return -1;

		/* Viewport設定 */
		glViewport(0, 0, width, height);
		fingitr->second.DspW = width;
		fingitr->second.DspH = height;
		__android_log_print(ANDROID_LOG_DEBUG, "aaaaa", "aaaaa Viewport設定() e SurfaceId=%d", SurfaceId);
		return 0;
	});

	/* Pre描画を送信 */
	ActionScheduler::sendMessage(SurfaceId, [SurfaceId](){
		__android_log_print(ANDROID_LOG_DEBUG, "aaaaa", "aaaaa Pre描画() s SurfaceId=%d", SurfaceId);

		std::map<int, CppSurface>::iterator fingitr = gSurfaceList.find(SurfaceId);
		if(fingitr == gSurfaceList.end())
			return -1;

		CppSurface &cppsurface = fingitr->second;
		GLuint ma_PositionHandle = glGetAttribLocation(cppsurface.mProgram, "vPosition");
		cppsurface.mu_rotMatrixHandle = glGetUniformLocation(cppsurface.mProgram, "u_rotMatrix");

		glUseProgram(cppsurface.mProgram);
		static const GLfloat vertexes[] = {0,0.5, -0.5,-0.5, 0.5,-0.5};
		glVertexAttribPointer(ma_PositionHandle, 2, GL_FLOAT, GL_FALSE, 0, vertexes);
		glEnableVertexAttribArray(ma_PositionHandle);

		cppsurface.mxPos = 0;
		cppsurface.myPos = 0;
		glClearColor(1, 0.5f, 0.2f, 0.3f);

		__android_log_print(ANDROID_LOG_DEBUG, "aaaaa", "aaaaa Pre描画() e SurfaceId=%d", SurfaceId);
		return 0;
	});

}

void Java_com_test_cppsurfaceviewtrans_NativeFunc_draw(JNIEnv *env, jclass type, jint SurfaceId) {
	/* 描画更新 */
	ActionScheduler::sendMessage(SurfaceId, [SurfaceId] {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		std::map<int, CppSurface>::iterator fingitr = gSurfaceList.find(SurfaceId);
		if(fingitr == gSurfaceList.end())
			return -1;

		CppSurface &cppsurface = fingitr->second;
		cppsurface.mxPos += cppsurface.mMoveX;
		cppsurface.myPos += cppsurface.mMoveY;
		if((cppsurface.mxPos > (2*cppsurface.DspW)) || (cppsurface.mxPos < 0)) cppsurface.mMoveX = -cppsurface.mMoveX;
		if((cppsurface.myPos > (2*cppsurface.DspH)) || (cppsurface.myPos < 0)) cppsurface.mMoveY = -cppsurface.mMoveY;
		float translateMatrix[] = {
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			cppsurface.mxPos/cppsurface.DspW-1, cppsurface.myPos/cppsurface.DspH-1, 1, 1
		};
		glUniformMatrix4fv(cppsurface.mu_rotMatrixHandle, 1, false, translateMatrix);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
		eglSwapBuffers(cppsurface.mEGLDisplay, cppsurface.mEGLSurface);
		return 0;
	});
}

void Java_com_test_cppsurfaceviewtrans_NativeFunc_surfaceDestroyed(JNIEnv *pEnv, jclass type, jint SurfaceId) {
	ActionScheduler::sendMessage(SurfaceId, [SurfaceId] {
		std::map<int, CppSurface>::iterator fingitr = gSurfaceList.find(SurfaceId);
		if(fingitr == gSurfaceList.end())
			return -1;

		CppSurface &cppsurface = fingitr->second;
		ANativeWindow_release(cppsurface.mWindow);
		cppsurface.mWindow = NULL;
		gSurfaceList.erase(SurfaceId);

		return D_THREADEND_MESSAGE;
	});
}

#ifdef __cplusplus
}
#endif

/**************/
/* CppSurface */
/**************/
CppSurface::CppSurface(int id) {

}

void CppSurface::initEGL(ANativeWindow *pWindow) {
	EGLint major, minor;
	mEGLDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	eglInitialize(mEGLDisplay, &major, &minor);

	/* 設定取得 */
	const EGLint configAttributes[] = {
		EGL_LEVEL, 0,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		/* 透過設定 */
		EGL_ALPHA_SIZE, EGL_OPENGL_BIT,
		/*EGL_BUFFER_SIZE, 32 */  /* ARGB8888用 */
		EGL_DEPTH_SIZE, 16,
		EGL_NONE
	};

	EGLConfig config;
	EGLint numConfigs;
	eglChooseConfig(mEGLDisplay, configAttributes, &config, 1, &numConfigs);

	/* context生成 */
	const EGLint contextAttributes[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};
	mEGLContext = eglCreateContext(mEGLDisplay, config, EGL_NO_CONTEXT, contextAttributes);

	/* ウィンドウバッファサイズとフォーマットを設定 */
	EGLint format;
	eglGetConfigAttrib(mEGLDisplay, config, EGL_NATIVE_VISUAL_ID, &format);

	/* ウィンドウ バッファの形式とサイズを変更 */
	ANativeWindow_setBuffersGeometry(pWindow, 0, 0, format);
	mWindow = pWindow;

	/* surface生成 */
	mEGLSurface = eglCreateWindowSurface(mEGLDisplay, config, mWindow, NULL);
	if(mEGLSurface == EGL_NO_SURFACE) {
		__android_log_print(ANDROID_LOG_ERROR, "CppSurfaceView", "surface生成 失敗!!");
		return;
	}

	/* context再生成 */
	mEGLContext = eglCreateContext(mEGLDisplay, config, EGL_NO_CONTEXT, contextAttributes);

	/* 作成したsurface/contextを関連付け */
	if(eglMakeCurrent(mEGLDisplay, mEGLSurface, mEGLSurface, mEGLContext) == EGL_FALSE) {
		__android_log_print(ANDROID_LOG_ERROR, "CppSurfaceView", "surface/contextの関連付け 失敗!!");
		return;
	}

	/* チェック */
	EGLint w,h;
	eglQuerySurface(mEGLDisplay, mEGLSurface, EGL_WIDTH, &w);
	eglQuerySurface(mEGLDisplay, mEGLSurface, EGL_HEIGHT,&h);
//    glViewport(0,0,w,h);
//    __android_log_print(ANDROID_LOG_DEBUG, "aaaaa", "aaaaa w=%d h=%d", w,h);
}

void CppSurface::initGL() {
	mProgram = createProgram(VERTEXSHADER, FRAGMENTSHADER);
}

GLuint CppSurface::createProgram(const char *vertexshader, const char *fragmentshader) {
	GLuint vhandle = loadShader(GL_VERTEX_SHADER, vertexshader);
	if(vhandle == GL_FALSE) return GL_FALSE;

	GLuint fhandle = loadShader(GL_FRAGMENT_SHADER, fragmentshader);
	if(fhandle == GL_FALSE) return GL_FALSE;

	GLuint programhandle = glCreateProgram();
	if(programhandle == GL_FALSE) {
		checkGlError("glCreateProgram");
		return GL_FALSE;
	}

	glAttachShader(programhandle, vhandle);
	checkGlError("glAttachShader(VERTEX_SHADER)");
	glAttachShader(programhandle, fhandle);
	checkGlError("glAttachShader(FRAGMENT_SHADER)");

	glLinkProgram(programhandle);
	GLint linkStatus = GL_FALSE;
	glGetProgramiv(programhandle, GL_LINK_STATUS, &linkStatus);
	if(linkStatus != GL_TRUE) {
		GLint bufLen = 0;
		glGetProgramiv(programhandle, GL_INFO_LOG_LENGTH, &bufLen);
		if(bufLen) {
			char *logstr = (char*)malloc(bufLen);
			glGetProgramInfoLog(mProgram, bufLen, NULL, logstr);
//			__android_log_print(ANDROID_LOG_ERROR, "CppSurface", "%d glLinkProgram() Fail!!\n %s", mId, logstr);
			free(logstr);
		}
		glDeleteProgram(programhandle);
		programhandle = GL_FALSE;
	}

	return programhandle;
}

GLuint CppSurface::loadShader(int shadertype, const char *sourcestring) {
	GLuint shaderhandle = glCreateShader(shadertype);
	if(!shaderhandle) return GL_FALSE;

	glShaderSource(shaderhandle, 1, &sourcestring, NULL);
	glCompileShader(shaderhandle);

	GLint compiled = GL_FALSE;
	glGetShaderiv(shaderhandle, GL_COMPILE_STATUS, &compiled);
	if(!compiled) {
		GLint infoLen = 0;
		glGetShaderiv(shaderhandle, GL_INFO_LOG_LENGTH, &infoLen);
		if(infoLen) {
			char *logbuf = (char*)malloc(infoLen);
			if(logbuf) {
				glGetShaderInfoLog(shaderhandle, infoLen, NULL, logbuf);
//				__android_log_print(ANDROID_LOG_ERROR, "CppSurface", "%d shader failuer!!\n%s", mId, logbuf);
				free(logbuf);
			}
		}
		glDeleteShader(shaderhandle);
		shaderhandle = GL_FALSE;
	}

	return shaderhandle;
}

void CppSurface::checkGlError(const char *argstr) {
	for(GLuint error = glGetError(); error; error = glGetError())
//		__android_log_print(ANDROID_LOG_ERROR, "CppSurface", "%d after %s errcode=%d", mId, argstr, error);
		__android_log_print(ANDROID_LOG_ERROR, "CppSurface", "after %s errcode=%d", argstr, error);
}

CppSurface::~CppSurface() {
}
