#ifndef __JNIOverrideContext_h__
#define __JNIOverrideContext_h__
#include <jni.h>
#include <map>
#include <string>

//JNINativeInterfaces可以看作是一个函数指针的数组，即JNI_FUN*
typedef void* JNI_FUN;

/*获得函数在函数指针数组中的索引，比如:
 * int funIndex1 = JNI_FUN_INDEX(GetVersion);//funIndex1的值是4
 * int funIndex2 = JNI_FUN_INDEX(DefineClass);//funIndex2的值是5
 * int funIndex3 = JNI_FUN_INDEX(FindClass);//funIndex3的值是6
 * */
#define JNI_FUN_INDEX(funName) (((char*)&((JNINativeInterface*)0)->funName - (char*)0) / sizeof(JNI_FUN))

/* 重写一个函数。假设你已经定义了一个新的NewGlobalRef，函数名称为NewGlobalRef_Override,我们可以这么重写NewGlobalRef为ewGlobalRef_Override：
 * JNI_OVERRIDE_FUNCTION(env, NewGlobalRef, NewGlobalRef_NEW)；
 */
#define JNI_OVERRIDE_FUNCTION(env, funName, funAddr) JNIOverrideFunction(env, JNI_FUN_INDEX(funName), (JNI_FUN)funAddr)

class JNIOverrideContext {
public:
    /*构造函数主要干三件事：
     * 复制env->functions到mOverrideFun
     * 复制env->functions到mOriginFun
     * 改变env->functions的指向，指向mOverrideFun
     * */
    JNIOverrideContext(JNIEnv *env);

    /*重写一个函数
     * funIndex 函数在函数数组中的索引
     * funAddr 新的函数地址
     */
    void overrideFun(int funIndex, JNI_FUN funAddr);

     /*获得JNIEnv中原来的函数,即重写之前的函数
     * funIndex 函数在函数数组中的索引
     */
    JNI_FUN getOriginFun(int funIndex);


    /*这个看代码吧，效果更好*/
    template <typename T>  T* obtainObject(const std::string &objName) {
        if (mObjMap[objName] == NULL) {
            mObjMap[objName] = new T;
        }
        return (T*)mObjMap[objName];
    }

private:
    JNINativeInterface mOverrideFun;
    JNINativeInterface mOriginFun;
    std::map<std::string, void*> mObjMap;
};

/*每个重写的实现函数开头的第一句都是这个宏。这个宏主要定义以下三个变量：
 * JNIOverrideContext *context; //获得重写上下文
 * JNI_FUN originFun;//获得原来的函数
 * int funIndex;获得函数在函数数组中的索引
 * */
#define JNI_BEGIN_OVERRIDE(env, funName)\
JNIOverrideContext *context;\
JNI_FUN originFun;\
int funIndex = JNI_FUN_INDEX(funName);\
JNIBeginOverride(env, funIndex, &context, &originFun);

/*获得重写上下文对象*/
#define JNI_GET_OVERRIDE_CONTEXT(env)  ((JNIOverrideContext*)((JNINativeInterface*)env->functions)->reserved0)

void JNIOverrideFunction(JNIEnv *env, int funIndex, JNI_FUN funAddr);
void JNIBeginOverride(JNIEnv *env, int funIndex, JNIOverrideContext **context, JNI_FUN *originFun);
#endif