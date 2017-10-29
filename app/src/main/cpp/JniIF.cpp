#include <thread>
#include <android/native_window_jni.h>
#include <android/log.h>
#include "ActiveObject.h"
#include "CppSurface.h"
#include "JniIF.h"

#ifdef __cplusplus
extern "C" {
#endif

void Java_com_test_cppsurfaceviewv2_NativeFunc_surfaceCreated(JNIEnv *pEnv, jclass type, jobject surface, jint surfaceid) {
	__android_log_print(ANDROID_LOG_DEBUG ,"aaaaa", "aaaaa NewCpp surfaceCreated() surfaceid=%d", surfaceid);
	/* 受信スレッド生成/開始 */
	std::thread th(ActionScheduler::ThreadLoop, surfaceid);
	th.detach();

	/* スレッド生成完了待ち */
	while(ActionScheduler::isInitializeing(surfaceid))
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

	/* ネィティブWindow取得 */
	ANativeWindow *pWindow = ANativeWindow_fromSurface(pEnv, surface);

	/* サーフェース生成要求送信 */
	ActionScheduler::sendMessage(surfaceid, std::bind(CppSurface::ReqInitCppSurface, surfaceid, pWindow));
	return;
}

void Java_com_test_cppsurfaceviewv2_NativeFunc_surfaceChanged(JNIEnv *env, jclass type, jint surfaceid, jint width, jint height) {
	__android_log_print(ANDROID_LOG_DEBUG, "aaaaa", "aaaaa NewCpp surfaceChanged() surfaceid=%d w=%d h=%d", surfaceid, width, height);
	/* サーフェース前準備要求送信 */
	ActionScheduler::sendMessage(surfaceid, std::bind(CppSurface::ReqPreDrawCppSurface, surfaceid, width, height));
	return;
}

void Java_com_test_cppsurfaceviewv2_NativeFunc_draw(JNIEnv *env, jclass type, jint surfaceid) {
	__android_log_print(ANDROID_LOG_DEBUG ,"aaaaa", "aaaaa NewCpp draw() surfaceid=%d", surfaceid);
	/* サーフェース描画要求送信 */
	ActionScheduler::sendMessage(surfaceid, std::bind(CppSurface::ReqDrawCppSurface, surfaceid));
	return;
}

void Java_com_test_cppsurfaceviewv2_NativeFunc_surfaceDestroyed(JNIEnv *env, jclass type, jint surfaceid) {
	__android_log_print(ANDROID_LOG_DEBUG ,"aaaaa", "aaaaa NewCpp surfaceDestroyed() surfaceid=%d", surfaceid);
	/* サーフェース描画要求送信 */
	ActionScheduler::sendMessage(surfaceid, std::bind(CppSurface::ReqDestroyCppSurface, surfaceid));
	/* サーフェース破棄要求送信 */
	ActionScheduler::sendMessage(surfaceid, D_THREADEND_MESSAGE);
	return;
}

#ifdef __cplusplus
}
#endif
