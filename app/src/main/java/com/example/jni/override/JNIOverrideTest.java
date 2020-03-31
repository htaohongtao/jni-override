package com.example.jni.override;

public class JNIOverrideTest {
    static {
        System.loadLibrary("jni-override");
    }
    public static native void test();
}
