# ProGuard rules consumed by applications using this AAR.
# Keep the JNI bridge so applications calling its native methods are not stripped.
-keep class com.library.babylonnative.** { *; }
