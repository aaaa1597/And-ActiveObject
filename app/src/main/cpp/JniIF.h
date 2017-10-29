#ifndef JNIIF_H
#define JNIIF_H

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL Java_com_test_cppsurfaceviewv2_NativeFunc_surfaceCreated(JNIEnv *env, jclass type, jobject surface, jint surfaceid);
JNIEXPORT void JNICALL Java_com_test_cppsurfaceviewv2_NativeFunc_surfaceChanged(JNIEnv *env, jclass type, jint surfaceid, jint width, jint height);
JNIEXPORT void JNICALL Java_com_test_cppsurfaceviewv2_NativeFunc_draw(JNIEnv *env, jclass type, jint surfaceid);
JNIEXPORT void JNICALL Java_com_test_cppsurfaceviewv2_NativeFunc_surfaceDestroyed(JNIEnv *env, jclass type, jint surfaceid);

#ifdef __cplusplus
}
#endif

#endif //JNIIF_H
