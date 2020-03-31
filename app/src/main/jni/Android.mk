LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := jni-override
LOCAL_SRC_FILES :=  CallStack.cpp JNIOverrideContext.cpp JNIPatch.cpp JNIOverrideTest.cpp
LOCAL_LDLIBS += -llog

include $(BUILD_SHARED_LIBRARY)