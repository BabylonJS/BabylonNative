#include <AndroidExtensions/JavaWrappers.h>
#include <AndroidExtensions/Globals.h>
#include <android/surface_texture.h>
#include <android/surface_texture_jni.h>
#include <android/asset_manager_jni.h>
#include <android/native_window_jni.h>

using namespace android::global;

namespace
{
    void ThrowIfFaulted(JNIEnv* env)
    {
        if (env->ExceptionCheck())
        {
            auto jthrowable{env->ExceptionOccurred()};
            env->ExceptionClear();
            throw java::lang::Throwable{jthrowable};
        }
    }
}

namespace java::lang
{
    ByteArray::ByteArray(int size)
        : m_env{GetEnvForCurrentThread()}
        , m_byteArray{m_env->NewByteArray(size)}
    {
    }

    ByteArray::ByteArray(jbyteArray byteArray)
        : m_env{GetEnvForCurrentThread()}
        , m_byteArray{byteArray}
    {
    }

    ByteArray::operator jbyteArray() const
    {
        return m_byteArray;
    }

    ByteArray::operator std::vector<std::byte>() const
    {
        std::vector<std::byte> result{static_cast<size_t>(m_env->GetArrayLength(m_byteArray))};
        std::memcpy(result.data(), m_env->GetByteArrayElements(m_byteArray, nullptr), result.size());
        return result;
    }

    Class::Class(const char* className)
        : m_env{GetEnvForCurrentThread()}
        , m_class{m_env->FindClass(className)}
    {
    }

    Class::Class(const jclass classObj)
        : m_env{GetEnvForCurrentThread()}
        , m_class{classObj}
    {
    }

    Class::operator jclass() const
    {
        return m_class;
    };

    bool Class::IsAssignableFrom(Class otherClass)
    {
        return m_env->IsAssignableFrom(m_class, otherClass.m_class);
    };

    Object::operator jobject() const
    {
        return m_object;
    }

    Object::Object(const char* className)
        : m_env{GetEnvForCurrentThread()}
        , m_class{m_env->FindClass(className)}
        , m_object{nullptr}
    {
    }

    Object::Object(jobject object)
            : m_env{GetEnvForCurrentThread()}
            , m_class{m_env->GetObjectClass(object)}
            , m_object{object}
    {
    }

    Class Object::GetClass()
    {
        return m_class;
    }

    String::String(jstring string)
        : m_env{GetEnvForCurrentThread()}
        , m_string{string}
    {
    }

    String::String(const char* string)
        : m_env{GetEnvForCurrentThread()}
        , m_string{m_env->NewStringUTF(string)}
    {
    }

    String::operator jstring() const
    {
        return m_string;
    }

    String::operator std::string() const
    {
        if (m_string == nullptr)
        {
            // Java strings can be null, but an std::string cannot be null.
            // If there is a possibility that the underlying Java string is null, you should test for that using (jstring != nullptr) before trying to implicitly convert.
            throw std::runtime_error("Tried to implicitly convert null Java String to C++ String");
        }
        const char* buffer{m_env->GetStringUTFChars(m_string, nullptr)};
        std::string str{buffer};
        m_env->ReleaseStringUTFChars(m_string, buffer);
        return str;
    }

    Throwable::Throwable(jthrowable throwable)
        : Object{throwable}
        , m_throwableRef{m_env->NewGlobalRef(throwable)}
        , m_message{GetMessage()}
    {
    }

    Throwable::~Throwable()
    {
        m_env->DeleteGlobalRef(m_throwableRef);
    }

    String Throwable::GetMessage() const
    {
        return {(jstring)m_env->CallObjectMethod(m_object, m_env->GetMethodID(m_class, "getMessage", "()Ljava/lang/String;"))};
    }

    const char* Throwable::what() const noexcept
    {
        return m_message.c_str();
    }
}

namespace java::io
{
    ByteArrayOutputStream::ByteArrayOutputStream()
        : Object{"java/io/ByteArrayOutputStream"}
    {
        m_object = m_env->NewObject(m_class, m_env->GetMethodID(m_class, "<init>", "()V"));
    }

    ByteArrayOutputStream::ByteArrayOutputStream(int size)
        : Object{"java/io/ByteArrayOutputStream"}
    {
        m_object = m_env->NewObject(m_class, m_env->GetMethodID(m_class, "<init>", "(I)V"), size);
    }

    ByteArrayOutputStream::ByteArrayOutputStream(jobject object)
        : Object{object}
    {
    }

    void ByteArrayOutputStream::Write(lang::ByteArray b, int off, int len)
    {
        m_env->CallVoidMethod(m_object, m_env->GetMethodID(m_class, "write", "([BII)V"), (jbyteArray)b, off, len);
    }

    lang::ByteArray ByteArrayOutputStream::ToByteArray() const
    {
        return {(jbyteArray)m_env->CallObjectMethod(m_object, m_env->GetMethodID(m_class, "toByteArray", "()[B"))};
    }

    lang::String ByteArrayOutputStream::ToString(const char* charsetName) const
    {
        jmethodID method{m_env->GetMethodID(m_class, "toString", "(Ljava/lang/String;)Ljava/lang/String;")};
        return {(jstring)m_env->CallObjectMethod(m_object, method, m_env->NewStringUTF(charsetName))};
    }

    InputStream::InputStream(jobject object)
        : Object{object}
    {
    }

    int InputStream::Read(lang::ByteArray byteArray) const
    {
        return m_env->CallIntMethod(m_object, m_env->GetMethodID(m_class, "read", "([B)I"), (jbyteArray)byteArray);
    }
}

namespace java::net
{
    HttpURLConnection::HttpURLConnection(jobject object)
        : Object{object}
    {
    }

    lang::Class HttpURLConnection::Class()
    {
        return {"java/net/HttpURLConnection"};
    }

    int HttpURLConnection::GetResponseCode() const
    {
        auto responseCode = m_env->CallIntMethod(m_object, m_env->GetMethodID(m_class, "getResponseCode", "()I"));
        ThrowIfFaulted(m_env);
        return responseCode;
    }

    URL::URL(lang::String url)
        : Object{"java/net/URL"}
    {
        m_object = m_env->NewObject(m_class, m_env->GetMethodID(m_class, "<init>", "(Ljava/lang/String;)V"), (jstring)url);
        ThrowIfFaulted(m_env);
    }

    URL::URL(jobject object)
        : Object{object}
    {
    }

    URLConnection URL::OpenConnection()
    {
        auto urlConnection{m_env->CallObjectMethod(m_object, m_env->GetMethodID(m_class, "openConnection", "()Ljava/net/URLConnection;"))};
        ThrowIfFaulted(m_env);
        return {urlConnection};
    }

    lang::String URL::ToString()
    {
        auto string{(jstring)m_env->CallObjectMethod(m_object, m_env->GetMethodID(m_class, "toString", "()Ljava/lang/String;"))};
        ThrowIfFaulted((m_env));
        return {string};
    }

    URLConnection::URLConnection(jobject object)
        : Object{object}
    {
    }

    void URLConnection::Connect()
    {
        m_env->CallVoidMethod(m_object, m_env->GetMethodID(m_class, "connect", "()V"));
        ThrowIfFaulted(m_env);
    }

    URL URLConnection::GetURL() const
    {
        auto url{m_env->CallObjectMethod(m_object, m_env->GetMethodID(m_class, "getURL", "()Ljava/net/URL;"))};
        ThrowIfFaulted(m_env);
        return {url};
    }

    int URLConnection::GetContentLength() const
    {
        auto contentLength{m_env->CallIntMethod(m_object, m_env->GetMethodID(m_class, "getContentLength", "()I"))};
        ThrowIfFaulted(m_env);
        return contentLength;
    }

    io::InputStream URLConnection::GetInputStream() const
    {
        auto inputStream{m_env->CallObjectMethod(m_object, m_env->GetMethodID(m_class, "getInputStream", "()Ljava/io/InputStream;"))};
        ThrowIfFaulted(m_env);
        return {inputStream};
    }

    URLConnection::operator HttpURLConnection() const
    {
        return {m_object};
    }
}

namespace android
{
    jstring ManifestPermission::CAMERA()
    {
        return getPermissionName("CAMERA");
    }

    jstring ManifestPermission::getPermissionName(const char* permissionName)
    {
        JNIEnv* env{GetEnvForCurrentThread()};
        jclass cls{env->FindClass("android/Manifest$permission")};
        jfieldID permId{env->GetStaticFieldID(cls, permissionName, "Ljava/lang/String;")};
        return (jstring)env->GetStaticObjectField(cls, permId);
    }
}

namespace android::app
{
    Activity::Activity(jobject object)
        : Object{object}
    {
    }

    void Activity::requestPermissions(jstring systemPermissionName, int permissionRequestID)
    {
        jobjectArray permissionArray{m_env->NewObjectArray(
            1,
            m_env->FindClass("java/lang/String"),
            systemPermissionName)};
        m_env->CallVoidMethod(m_object, m_env->GetMethodID(m_class, "requestPermissions", "([Ljava/lang/String;I)V"), permissionArray, permissionRequestID);
        m_env->DeleteLocalRef(permissionArray);
    }
}

namespace android::content
{
    Context::Context(jobject object)
        : Object{object}
    {
    }

    Context Context::getApplicationContext()
    {
        return {m_env->CallObjectMethod(m_object, m_env->GetMethodID(m_class, "getApplicationContext", "()Landroid/content/Context;"))};
    }

    res::AssetManager Context::getAssets() const
    {
        return {m_env->CallObjectMethod(m_object, m_env->GetMethodID(m_class, "getAssets", "()Landroid/content/res/AssetManager;"))};
    }

    jobject Context::getSystemService(const char* serviceName)
    {
        return m_env->CallObjectMethod(m_object, m_env->GetMethodID(m_class, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;"), m_env->NewStringUTF(serviceName));
    }

    res::Resources Context::getResources() {
        return {m_env->CallObjectMethod(m_object, m_env->GetMethodID(m_class, "getResources", "()Landroid/content/res/Resources;"))};
    }

    bool Context::checkSelfPermission(jstring systemPermissionName)
    {
        // Get the package manager, and get the value that represents a successful permission grant.
        jclass packageManager{m_env->FindClass("android/content/pm/PackageManager")};
        jfieldID permissionGrantedId{m_env->GetStaticFieldID(packageManager, "PERMISSION_GRANTED", "I")};
        jint permissionGrantedValue{m_env->GetStaticIntField(packageManager, permissionGrantedId)};

        // Perform the actual permission check by checking against the android context object.
        jint permissionCheckResult{m_env->CallIntMethod(m_object, m_env->GetMethodID(m_class, "checkSelfPermission", "(Ljava/lang/String;)I"), systemPermissionName)};
        return permissionGrantedValue == permissionCheckResult;
    }
}

namespace android::content::res
{
    AssetManager::AssetManager(jobject object)
        : Object(object)
    {
    }

    AssetManager::operator AAssetManager*() const
    {
        return AAssetManager_fromJava(m_env, m_object);
    }

    Configuration::Configuration(jobject object)
        : Object(object)
    {
    }

    int Configuration::getDensityDpi()
    {
        return m_env->GetIntField(m_object, m_env->GetFieldID(m_class, "densityDpi", "I"));
    }

    Resources::Resources(jobject object)
        : Object(object)
    {
    }

    Configuration Resources::getConfiguration()
    {
        return {m_env->CallObjectMethod(m_object, m_env->GetMethodID(m_class, "getConfiguration", "()Landroid/content/res/Configuration;"))};
    }
}

namespace android::view
{
    Display::Display(jobject object)
            : Object(object)
    {
    }

    int Display::getRotation()
    {
        return m_env->CallIntMethod(m_object, m_env->GetMethodID(m_class, "getRotation", "()I"));
    }

    WindowManager::WindowManager(jobject object)
        : Object(object)
    {
    }

    Display WindowManager::getDefaultDisplay()
    {
        return {m_env->CallObjectMethod(m_object, m_env->GetMethodID(m_class, "getDefaultDisplay", "()Landroid/view/Display;"))};
    }

    Surface::Surface(android::graphics::SurfaceTexture& surfaceTexture)
        : Object("android/view/Surface")
    {
        m_object = m_env->NewObject(m_class, m_env->GetMethodID(m_class, "<init>", "(Landroid/graphics/SurfaceTexture;)V"), (jobject)surfaceTexture);
    }
}

namespace android::net
{
    Uri::Uri(jobject object)
        : Object{object}
    {
    }

    java::lang::String Uri::getScheme() const
    {
        auto scheme{(jstring)m_env->CallObjectMethod(m_object, m_env->GetMethodID(m_class, "getScheme", "()Ljava/lang/String;"))};
        ThrowIfFaulted(m_env);
        return {scheme};
    }

    java::lang::String Uri::getPath() const
    {
        auto path{(jstring)m_env->CallObjectMethod(m_object, m_env->GetMethodID(m_class, "getPath", "()Ljava/lang/String;"))};
        ThrowIfFaulted(m_env);
        return {path};
    }

    Uri Uri::Parse(java::lang::String uriString)
    {
        JNIEnv* env{GetEnvForCurrentThread()};
        jclass cls{env->FindClass("android/net/Uri")};
        auto uri{env->CallStaticObjectMethod(cls, env->GetStaticMethodID(cls, "parse", "(Ljava/lang/String;)Landroid/net/Uri;"), (jstring)uriString)};
        ThrowIfFaulted(env);
        return {uri};
    }
}

namespace android::graphics
{
    SurfaceTexture::SurfaceTexture()
        : Object("android/graphics/SurfaceTexture")
    {
    }

    void SurfaceTexture::InitWithTexture(int texture)
    {
        m_object = m_env->NewObject(m_class, m_env->GetMethodID(m_class, "<init>", "(I)V"), texture);
    }

    void SurfaceTexture::updateTexImage() const
    {
        if (m_object) {
            m_env->CallVoidMethod(m_object, m_env->GetMethodID(m_class, "updateTexImage", "()V"));
        }
    }
}