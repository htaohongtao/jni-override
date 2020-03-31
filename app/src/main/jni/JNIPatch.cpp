#include "JNIOverrideContext.h"
#include <vector>
#include <android/log.h>

extern void callstackDump(std::string &dump);
class GlobalRefItem {
public:
    jobject ref;
    std::string stack;
};
std::vector<GlobalRefItem> g_vecItem;

typedef jobject     (*JNI_NewGlobalRef)(JNIEnv*, jobject);
jobject NewGlobalRefOverride(JNIEnv *env, jobject obj) {
    JNI_BEGIN_OVERRIDE(env, NewGlobalRef);
    //std::vector<GlobalRefItem> &vecItem = *context->obtainObject<std::vector<GlobalRefItem>>("GlobalRefContext");

    jobject g = ((JNI_NewGlobalRef)originFun)(env, obj);//调用原始的env->NewGlobalRef(obj);
    if (g == NULL) {
        return NULL;
    }

    GlobalRefItem item;
    item.ref = g;
    callstackDump(item.stack); //记录函数调用堆栈
    g_vecItem.push_back(item);

    return g;
}

typedef void        (*JNI_DeleteGlobalRef)(JNIEnv*, jobject);
void  DeleteGlobalRefOverride(JNIEnv* env, jobject object) {
    JNI_BEGIN_OVERRIDE(env, DeleteGlobalRef);
//    std::vector<GlobalRefItem> &vecItem = *context->obtainObject<std::vector<GlobalRefItem>>("GlobalRefContext");
    for (size_t i = 0; i < g_vecItem.size(); i++) {
        if (object == g_vecItem[i].ref) {
            g_vecItem.erase(g_vecItem.begin() + i);
            break;
        }
    }

    ((JNI_DeleteGlobalRef)originFun)(env, object);
}

void JNI_PrintAllGlobalRef(JNIEnv *env) {
    JNIOverrideContext *context = JNI_GET_OVERRIDE_CONTEXT(env);
    if (context == NULL) {
        return;
    }
//    std::vector<GlobalRefItem> &vecItem = *context->obtainObject<std::vector<GlobalRefItem>>("GlobalRefContext");
    __android_log_print(ANDROID_LOG_DEBUG, "MD_DEBUG", "JNI_PrintAllGlobalRef count:%d",  g_vecItem.size());
    for (size_t i = 0; i < g_vecItem.size(); i++) {
        __android_log_print(ANDROID_LOG_DEBUG, "MD_DEBUG", "%s",  g_vecItem[i].stack.c_str());
    }
}

void JNIPatchEnv(JNIEnv *env) {
    JNI_OVERRIDE_FUNCTION(env, NewGlobalRef, NewGlobalRefOverride);
    JNI_OVERRIDE_FUNCTION(env, DeleteGlobalRef, DeleteGlobalRefOverride);
}

