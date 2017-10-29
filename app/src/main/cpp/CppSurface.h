#ifndef CPPSURFACE_H
#define CPPSURFACE_H

#include <map>
#include <vector>
#include <utility>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include "GLDraw.h"

class CppSurface {
public:	/* for Public */
	static int ReqInitCppSurface(int surfaceid, ANativeWindow *pWindow);
	static int ReqPreDrawCppSurface(int surfaceid, int width, int height);
	static int ReqDrawCppSurface(int surfaceid);
	static int ReqDestroyCppSurface(int surfaceid);

private:
	void initEGL(ANativeWindow *pWindow);
	void finEGL();

private:
	int mSurfaceid;
	ANativeWindow *mWindow = nullptr;
	EGLDisplay mEGLDisplay = nullptr;
	EGLContext mEGLContext = nullptr;
	EGLSurface mEGLSurface = nullptr;
	std::vector<GLDraw*> mDrawItems;

public:	/* for System */
	CppSurface(int surfaceid):mSurfaceid(surfaceid){ __android_log_print(ANDROID_LOG_DEBUG, "aaaaa", "CppSurface::CppSurface(mSurfaceid=%d this=%d)", mSurfaceid, this); };
	virtual ~CppSurface(){__android_log_print(ANDROID_LOG_DEBUG, "aaaaa", "CppSurface::~CppSurface(mSurfaceid=%d this=%d)", mSurfaceid, this);};
	CppSurface(const CppSurface &obj) {
		mSurfaceid = obj.mSurfaceid;
		mWindow = obj.mWindow;
		mEGLDisplay = obj.mEGLDisplay;
		mEGLContext = obj.mEGLContext;
		mEGLSurface = obj.mEGLSurface;
		mDrawItems = obj.mDrawItems;
		__android_log_print(ANDROID_LOG_DEBUG, "aaaaa", "CppSurface::CppSurface(const CppSurface &obj) mSurfaceid=%d obj.mSurfaceid=%d this=%d obj=%d", mSurfaceid, obj.mSurfaceid, this, &obj);
	}
	CppSurface(const CppSurface&& obj) {
		mSurfaceid = std::move(obj.mSurfaceid);
		mWindow = std::move(obj.mWindow);
		mEGLDisplay = std::move(obj.mEGLDisplay);
		mEGLContext = std::move(obj.mEGLContext);
		mEGLSurface = std::move(obj.mEGLSurface);
		mDrawItems = std::move(obj.mDrawItems);
		__android_log_print(ANDROID_LOG_DEBUG, "aaaaa", "CppSurface::CppSurface(const CppSurface &&obj) mSurfaceid=%d obj.mSurfaceid=%d this=%d obj=%d", mSurfaceid, obj.mSurfaceid, this, &obj);
	}
};

#endif //CPPSURFACE_H
