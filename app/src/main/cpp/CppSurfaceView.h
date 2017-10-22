#ifndef TESTNATIVESURFACE_H
#define TESTNATIVESURFACE_H

#include <jni.h>
#include <android/native_window.h>
#include <pthread.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jint JNICALL Java_com_test_cppsurfaceviewtrans_NativeFunc_surfaceCreated(JNIEnv *env, jclass type, jobject surface);
JNIEXPORT void JNICALL Java_com_test_cppsurfaceviewtrans_NativeFunc_surfaceChanged(JNIEnv *env, jclass type, jint SurfaceId, jint width, jint height);
JNIEXPORT void JNICALL Java_com_test_cppsurfaceviewtrans_NativeFunc_draw(JNIEnv *env, jclass type, jint SurfaceId);
JNIEXPORT void JNICALL Java_com_test_cppsurfaceviewtrans_NativeFunc_surfaceDestroyed(JNIEnv *env, jclass type, jint SurfaceId);

#ifdef __cplusplus
}
#endif

class CppSurface {
private:
    const char *VERTEXSHADER =
		"attribute vec4 vPosition;\n"
		"uniform mat4 u_rotMatrix;\n"
		"void main() {\n"
		"    gl_Position = u_rotMatrix * vPosition;\n"
		"}\n";

    const char *FRAGMENTSHADER =
		"precision mediump float;\n"
		"void main() {\n"
		"    gl_FragColor = vec4(1.0, 0.0, 1.0, 0.2);\n"
		"}\n";

private:
    GLuint createProgram(const char *vertexshader, const char *fragmentshader);
    GLuint loadShader(int i, const char *vertexshader);
    void checkGlError(const char *argstr);

public:
    ANativeWindow *mWindow = NULL;
    EGLDisplay mEGLDisplay = NULL;
    EGLContext mEGLContext = NULL;
    EGLSurface mEGLSurface = NULL;
    GLuint mProgram = -1;
	int DspW = 0;
	int DspH = 0;

	/* 移動 */
	static const int AMOUNTOFMOVE = -5;
	float mMoveX = AMOUNTOFMOVE;
	float mMoveY = AMOUNTOFMOVE;
	GLuint mu_rotMatrixHandle = -1;
	float mxPos = 100;
	float myPos = 130;

public:
    CppSurface(int surfaceid);
    virtual ~CppSurface();
    void initEGL(ANativeWindow *pWindow);
    void initGL();

public:	/* for System */
	CppSurface(const CppSurface &obj) {
		mu_rotMatrixHandle = obj.mu_rotMatrixHandle;
		mWindow = obj.mWindow;
		mEGLDisplay = obj.mEGLDisplay;
		mEGLContext = obj.mEGLContext;
		mEGLSurface = obj.mEGLSurface;
		mProgram = obj.mProgram;
	}
	CppSurface(const CppSurface&& obj) {
		mu_rotMatrixHandle = std::move(obj.mu_rotMatrixHandle);
		mWindow = std::move(obj.mWindow);
		mEGLDisplay = std::move(obj.mEGLDisplay);
		mEGLContext = std::move(obj.mEGLContext);
		mEGLSurface = std::move(obj.mEGLSurface);
		mProgram = std::move(obj.mProgram);
	}
};

#endif //TESTNATIVESURFACE_H
