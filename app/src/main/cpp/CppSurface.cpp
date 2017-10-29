#include <android/native_window.h>
#include <android/log.h>
#include <mutex>
#include "CppSurface.h"

static std::map<int , CppSurface> mSurfaces;
static std::mutex mMutexforCppSurfaceList;
static std::mutex mMutexforDrawItemList;

template <typename Of, typename What>
inline bool instanceof(const What w) {
	return dynamic_cast<const Of*>(w) != 0;
}

/* サーフェース生成/初期化要求 */
int CppSurface::ReqInitCppSurface(int surfaceid, ANativeWindow *pWindow) {
	__android_log_print(ANDROID_LOG_DEBUG, "aaaaa", "aaaaa ReqInitCppSurface() SurfaceId=%d", surfaceid);

	/* CppSurfaceクラス生成 */
	mMutexforCppSurfaceList.lock();
	if(mSurfaces.find(surfaceid) == mSurfaces.end())
		mSurfaces.insert(std::make_pair(surfaceid, CppSurface(surfaceid)));
	mMutexforCppSurfaceList.unlock();

	mMutexforCppSurfaceList.lock();
	std::map<int , CppSurface>::iterator itr = mSurfaces.find(surfaceid);
	mMutexforCppSurfaceList.unlock();
	itr->second.initEGL(pWindow);

	mMutexforDrawItemList.lock();
	std::vector<GLDraw*> &Drawers = itr->second.mDrawItems;
	Drawers.push_back(new GLDrawTriangle());
	/* ... DrawTriangleなどを追加 ... */
	mMutexforDrawItemList.unlock();

	for(int lpct = 0; lpct < Drawers.size(); lpct++)
		Drawers[lpct]->Init();

	glClearColor(0, 0, 0, 0);

	return 0;
}

/* EGL初期化 */
void CppSurface::initEGL(ANativeWindow *pWindow) {

	mWindow = pWindow;

	/* デフォルトEGLDisplay取得 */
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

	/* config選択 */
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
	ANativeWindow_setBuffersGeometry(mWindow, 0, 0, format);

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
	glViewport(0,0,w,h);
	__android_log_print(ANDROID_LOG_DEBUG, "aaaaa", "glViewport(%d,%d) 1", w,h);

	return;
}

/* サーフェース前準備要求 */
int CppSurface::ReqPreDrawCppSurface(int surfaceid, int width, int height) {
	__android_log_print(ANDROID_LOG_DEBUG, "aaaaa", "ReqPreDrawCppSurface surfaceid=%d (%d,%d) 1", surfaceid, width, height);

	glClearColor(0, 0, 0, 0);
	glViewport(0,0,width,height);

	/* 対象サーフェース取得 */
	mMutexforCppSurfaceList.lock();
	std::map<int , CppSurface>::iterator itr = mSurfaces.find(surfaceid);
	mMutexforCppSurfaceList.unlock();
	if(itr == mSurfaces.end())
		return -1;

	/* 各DrawerのPreDrawを呼出す */
	mMutexforDrawItemList.lock();
	std::vector<GLDraw*> &Drawers = itr->second.mDrawItems;
	mMutexforDrawItemList.unlock();
	for(int lpct = 0; lpct < Drawers.size(); lpct++)
		Drawers[lpct]->PreDraw(width, height);

	return 0;
}

int CppSurface::ReqDrawCppSurface(int surfaceid) {
	__android_log_print(ANDROID_LOG_DEBUG, "aaaaa", "ReqDrawCppSurface surfaceid=%d", surfaceid);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* 対象サーフェース取得 */
	mMutexforCppSurfaceList.lock();
	std::map<int , CppSurface>::iterator itr = mSurfaces.find(surfaceid);
	mMutexforCppSurfaceList.unlock();
	if(itr == mSurfaces.end())
		return -1;

	/* 各DrawerのDrawを呼出す */
	mMutexforDrawItemList.lock();
	std::vector<GLDraw*> &Drawers = itr->second.mDrawItems;
	mMutexforDrawItemList.unlock();
	for(int lpct = 0; lpct < Drawers.size(); lpct++)
		Drawers[lpct]->Draw();

	eglSwapBuffers(itr->second.mEGLDisplay, itr->second.mEGLSurface);

	return 0;
}

int CppSurface::ReqDestroyCppSurface(int surfaceid) {
	__android_log_print(ANDROID_LOG_DEBUG, "aaaaa", "ReqDestroyCppSurface s surfaceid=%d", surfaceid);
	mMutexforCppSurfaceList.lock();
	std::map<int , CppSurface>::iterator itr = mSurfaces.find(surfaceid);
	if(itr == mSurfaces.end()) {
		mMutexforCppSurfaceList.unlock();
		return -1;
	}

	mMutexforDrawItemList.lock();
	std::vector<GLDraw*> &Drawers = itr->second.mDrawItems;
	for(int lpct = 0; lpct < Drawers.size(); lpct++) {
		Drawers[lpct]->Fin();
		delete Drawers[lpct];
	}
	Drawers.clear();
	mMutexforDrawItemList.unlock();

	itr->second.finEGL();
	mSurfaces.erase(surfaceid);
	mMutexforCppSurfaceList.unlock();

	__android_log_print(ANDROID_LOG_DEBUG, "aaaaa", "ReqDestroyCppSurface e surfaceid=%d", surfaceid);
	return 0;
}

void CppSurface::finEGL() {
	__android_log_print(ANDROID_LOG_DEBUG, "aaaaa", "CppSurface::finEGL()");
	eglMakeCurrent(mEGLDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroySurface(mEGLDisplay, mEGLSurface);
	eglDestroyContext(mEGLDisplay, mEGLContext);
	eglTerminate(mEGLDisplay);
	return;
}
