#include <jni.h>
JavaVM *g_vm;
JNIEnv* getEnv()
{
    JNIEnv *env;
    g_vm->GetEnv((void**)&env,JNI_VERSION_1_4);

    return env;
}

extern void JNIPatchEnv(JNIEnv *env);
JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    g_vm = vm;
    JNIPatchEnv(getEnv());
    return JNI_VERSION_1_4;
}

void JNI_PrintAllGlobalRef(JNIEnv *env);
extern "C" JNIEXPORT void Java_com_example_jni_override_JNIOverrideTest_test(JNIEnv *env, jclass clazz) {
    jobject ref1 = env->NewGlobalRef(clazz);
    jobject ref2 = env->NewGlobalRef(clazz);
    env->DeleteGlobalRef(ref1);
    JNI_PrintAllGlobalRef(env);
}