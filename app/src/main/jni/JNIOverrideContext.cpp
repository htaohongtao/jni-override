#include "JNIOverrideContext.h"

#define JNI_FUN_COUNT (sizeof(JNINativeInterface) / sizeof(JNI_FUN))
#define JNI_FUN_INDEX_VALID(index) (index >= 4 && index < JNI_FUN_COUNT)

JNIOverrideContext::JNIOverrideContext(JNIEnv *env) {
    mOverrideFun = *env->functions;
    mOriginFun = *env->functions;

    mOverrideFun.reserved0 = this;
    env->functions = &mOverrideFun;
}

void JNIOverrideContext::overrideFun(int index, JNI_FUN fun) {
    ((JNI_FUN*)&mOverrideFun)[index] = fun;
}

JNI_FUN JNIOverrideContext::getOriginFun(int funIndex) {
    if (!JNI_FUN_INDEX_VALID(funIndex)) {
        return NULL;
    }
    return ((JNI_FUN*)&mOriginFun)[funIndex];
}


void JNIBeginOverride(JNIEnv *env, int funIndex, JNIOverrideContext **context, JNI_FUN *originFun) {
    *context = (JNIOverrideContext*)((JNINativeInterface*)env->functions)->reserved0;
    *originFun = (*context)->getOriginFun(funIndex);
}

void JNIOverrideFunction(JNIEnv *env, int funIndex, JNI_FUN funAddr) {
    JNIOverrideContext *context;
    if (env->functions->reserved0 == NULL) {
        context = new JNIOverrideContext(env);
    } else {
        context = (JNIOverrideContext*)env->functions->reserved0;
    }
    context->overrideFun(funIndex, funAddr);
}


