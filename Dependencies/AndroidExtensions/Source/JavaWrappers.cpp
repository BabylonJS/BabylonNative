#include <AndroidExtensions/JavaWrappers.h>
#include <AndroidExtensions/Globals.h>
#include <android/surface_texture.h>
#include <android/surface_texture_jni.h>
#include <android/asset_manager_jni.h>
#include <android/native_window_jni.h>
#include <algorithm>
#include <vector>

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
        , m_class{static_cast<jclass>(m_env->NewGlobalRef(m_env->FindClass(className)))}
    {
    }

    Class::Class(const jclass classObj)
        : m_env{GetEnvForCurrentThread()}
        , m_class{static_cast<jclass>(m_env->NewGlobalRef(classObj))}
    {
    }

    Class::~Class()
    {
        JClass(nullptr);
    }

    Class::Class(const Class& other)
        : Class(other.m_class)
    {
    }

    Class& Class::operator=(const Class& other)
    {
        if (this != &other)
        {
            // Clear out the jclass using the current environment before updating to the
            // other Class's environment and jclass. The jclass should be used in conjunction
            // with the environment it was created in.
            JClass(nullptr);
            m_env = other.m_env;
            JClass(other.JClass());
        }

        return *this;
    }

    Class::Class(Class&& other)
        : m_env{other.m_env}
        , m_class{other.m_class}
    {
        other.m_class = nullptr;
        other.m_env = nullptr;
    }

    Class& Class::operator=(Class&& other)
    {
        m_env = other.m_env;
        m_class = other.m_class;
        other.m_class = nullptr;
        other.m_env = nullptr;

        return *this;
    }

    Class::operator jclass() const
    {
        return m_class;
    };

    bool Class::IsAssignableFrom(Class otherClass)
    {
        return m_env->IsAssignableFrom(m_class, otherClass.m_class);
    };

    jclass Class::JClass() const
    {
        return m_class;
    }

    void Class::JClass(jclass classObj)
    {
        if (m_class) {
            m_env->DeleteGlobalRef(m_class);
        }

        m_class = classObj;

        if (m_class)
        {
            m_class = static_cast<jclass>(m_env->NewGlobalRef(classObj));
        }
    }

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

    Object::Object(jclass classRef)
        : m_env{GetEnvForCurrentThread()}
        , m_class{classRef}
        , m_object{nullptr}
    {
    }

    Object::Object(jobject object)
        : m_env{GetEnvForCurrentThread()}
        , m_class{m_env->GetObjectClass(object)}
        , m_object{m_env->NewGlobalRef(object)}
    {
    }

    Object::~Object()
    {
        JObject(nullptr);
    }

    Object::Object(const Object& other)
        : Object(other.m_object)
    {
    }

    Object& Object::operator=(const Object& other)
    {
        if (this != &other)
        {
            // Clear out the jobject using the current environment before updating to the
            // other Object's environment and jobject. The jobject should be used in conjunction
            // with the environment it was created in.
            JObject(nullptr);
            m_env = other.m_env;
            m_class = other.m_class;
            JObject(other.m_object);
        }

        return *this;
    }

    Object::Object(Object&& other)
        : m_env{other.m_env}
        , m_class{std::move(other.m_class)}
        , m_object{other.m_object}
    {
        other.m_object = nullptr;
        other.m_env = nullptr;
    }

    Object& Object::operator=(Object&& other)
    {
        m_env = other.m_env;
        m_class = std::move(other.m_class);
        m_object = other.m_object;
        other.m_object = nullptr;
        other.m_env = nullptr;

        return *this;
    }

    jobject Object::JObject() const
    {
        return m_object;
    }

    void Object::JObject(jobject object)
    {
        if (m_object)
        {
            m_env->DeleteGlobalRef(m_object);
        }

        m_object = object;

        if (m_object)
        {
            m_object = m_env->NewGlobalRef(object);
        }
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
        return {(jstring)m_env->CallObjectMethod(JObject(), m_env->GetMethodID(m_class, "getMessage", "()Ljava/lang/String;"))};
    }

    const char* Throwable::what() const noexcept
    {
        return m_message.c_str();
    }
}

namespace java::websocket
{
    jclass WebSocketClient::s_webSocketClass{};
    std::vector<std::pair<jobject, WebSocketClient*>> WebSocketClient::s_instances;

    WebSocketClient::WebSocketClient(std::string url, std::function<void()> open_callback, std::function<void()> close_callback, std::function<void(std::string)> message_callback, std::function<void()> error_callback)
        : Object{s_webSocketClass}
        , m_openCallback{std::move(open_callback)}
        , m_messageCallback{std::move(message_callback)}
        , m_closeCallback{std::move(close_callback)}
        , m_errorCallback{std::move(error_callback)}
    {
        static JNINativeMethod methods[] =
        {
            {"closeCallback", "()V", (void*)OnClose},
            {"openCallback", "()V", (void*)OnOpen},
            {"messageCallback", "(Ljava/lang/String;)V", (void*)OnMessage},
            {"errorCallback", "()V", (void*)OnError},
        };
        m_env->RegisterNatives(m_class, methods, 4);

        JObject(m_env->NewObject(m_class, m_env->GetMethodID(m_class, "<init>", "(Ljava/lang/String;)V"), m_env->NewStringUTF(url.c_str())));

        s_instances.push_back(std::make_pair(JObject(), this));
    }

    WebSocketClient::~WebSocketClient()
    {
        const auto it = std::find_if(s_instances.begin(), s_instances.end(), [this](const auto &it)
        {
            return it.second == this;
        });

        if (it != s_instances.end())
        {
            s_instances.erase(it);
        }
    }

    void WebSocketClient::OnOpen(JNIEnv* env, jobject obj) 
    {
        auto itObject = FindInstance(env, obj);
        if (itObject != nullptr)
        {
            itObject->m_openCallback();
        }
    }

    void WebSocketClient::OnMessage(JNIEnv* env, jobject obj, jstring message) 
    {
        auto itObject = FindInstance(env, obj);
        if (itObject != nullptr)
        {
            java::lang::String mystr{message};
            itObject->m_messageCallback(mystr);
        }
    }

    void WebSocketClient::OnClose(JNIEnv* env, jobject obj) 
    {
        auto itObject = FindInstance(env, obj);
        if (itObject != nullptr)
        {
            itObject->m_closeCallback();
        }
    }

    void WebSocketClient::OnError(JNIEnv* env, jobject obj) 
    {
        auto itObject = FindInstance(env, obj);
        if (itObject != nullptr)
        {
            itObject->m_errorCallback();
        }
    }

    java::websocket::WebSocketClient* WebSocketClient::FindInstance(JNIEnv* env, jobject obj)
    {
        const auto it = std::find_if(s_instances.begin(), s_instances.end(), [&obj, &env](const auto &it)
        {
            return env->IsSameObject(obj, it.first);
        });

        ThrowIfFaulted(env);

        if (it != s_instances.end())
        {
            return it->second;
        }
        else
        {
            return nullptr;
        }
    }

    void WebSocketClient::Open()
    {
        jmethodID connectSocket{m_env->GetMethodID(m_class, "connectBlocking", "()Z")};
        m_env->CallBooleanMethod(JObject(), connectSocket);
        ThrowIfFaulted(m_env);
    }

    void WebSocketClient::Send(std::string message)
    {
        jmethodID sendMessage{m_env->GetMethodID(m_class, "send", "(Ljava/lang/String;)V")};
        m_env->CallVoidMethod(JObject(), sendMessage, m_env->NewStringUTF(message.c_str()));
        ThrowIfFaulted(m_env);
    }

    void WebSocketClient::Close()
    {
        jmethodID closeWebSocket{m_env->GetMethodID(m_class, "close", "()V")};
        m_env->CallVoidMethod(JObject(), closeWebSocket);
        ThrowIfFaulted(m_env);
    }

    void WebSocketClient::InitializeJavaWebSocketClass(jclass webSocketClass, JNIEnv* env)
    {
        s_webSocketClass = (jclass) env->NewGlobalRef(webSocketClass);
    }
    void WebSocketClient::DestructJavaWebSocketClass(JNIEnv* env)
    {
        env->DeleteGlobalRef(s_webSocketClass);
    }
}

namespace java::io
{
    ByteArrayOutputStream::ByteArrayOutputStream()
        : Object{"java/io/ByteArrayOutputStream"}
    {
        JObject(m_env->NewObject(m_class, m_env->GetMethodID(m_class, "<init>", "()V")));
    }

    ByteArrayOutputStream::ByteArrayOutputStream(int size)
        : Object{"java/io/ByteArrayOutputStream"}
    {
        JObject(m_env->NewObject(m_class, m_env->GetMethodID(m_class, "<init>", "(I)V"), size));
    }

    ByteArrayOutputStream::ByteArrayOutputStream(jobject object)
        : Object{object}
    {
    }

    void ByteArrayOutputStream::Write(lang::ByteArray b, int off, int len)
    {
        m_env->CallVoidMethod(JObject(), m_env->GetMethodID(m_class, "write", "([BII)V"), (jbyteArray)b, off, len);
    }

    lang::ByteArray ByteArrayOutputStream::ToByteArray() const
    {
        return {(jbyteArray)m_env->CallObjectMethod(JObject(), m_env->GetMethodID(m_class, "toByteArray", "()[B"))};
    }

    lang::String ByteArrayOutputStream::ToString(const char* charsetName) const
    {
        jmethodID method{m_env->GetMethodID(m_class, "toString", "(Ljava/lang/String;)Ljava/lang/String;")};
        return {(jstring)m_env->CallObjectMethod(JObject(), method, m_env->NewStringUTF(charsetName))};
    }

    InputStream::InputStream(jobject object)
        : Object{object}
    {
    }

    int InputStream::Read(lang::ByteArray byteArray) const
    {
        return m_env->CallIntMethod(JObject(), m_env->GetMethodID(m_class, "read", "([B)I"), (jbyteArray)byteArray);
    }

    OutputStream::OutputStream(jobject object)
        : Object{object}
    {
    }

    OutputStreamWriter::OutputStreamWriter(jobject object)
        : Object{"java/io/OutputStreamWriter"}
    {
        JObject(m_env->NewObject(m_class, m_env->GetMethodID(m_class, "<init>", "(Ljava/io/OutputStream;)V"), object));
    }

    void OutputStreamWriter::Write(std::string postBody)
    {
        jmethodID writeMethod = m_env->GetMethodID(m_class, "write", "(Ljava/lang/String;)V");
        jstring postBodyJstr = m_env->NewStringUTF(postBody.c_str());
        m_env->CallVoidMethod(JObject(), writeMethod, postBodyJstr);
        ThrowIfFaulted(m_env);
    }

    void OutputStreamWriter::Close()
    {
        m_env->CallVoidMethod(JObject(), m_env->GetMethodID(m_class, "close", "()V"));
        ThrowIfFaulted(m_env);
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
        auto responseCode = m_env->CallIntMethod(JObject(), m_env->GetMethodID(m_class, "getResponseCode", "()I"));
        ThrowIfFaulted(m_env);
        return responseCode;
    }

    void HttpURLConnection::SetRequestMethod(const std::string& requestMethod)
    {
        if (requestMethod != "POST" && requestMethod != "GET")
        {
            throw std::runtime_error("Only POST and GET are supported as arguments to setRequestMethod.");
        }
        jstring requestMethodJstr = m_env->NewStringUTF(requestMethod.c_str());
        m_env->CallVoidMethod(JObject(), m_env->GetMethodID(m_class, "setRequestMethod", "(Ljava/lang/String;)V"), requestMethodJstr);
        ThrowIfFaulted(m_env);
    }

    URL::URL(lang::String url)
        : Object{"java/net/URL"}
    {
        JObject(m_env->NewObject(m_class, m_env->GetMethodID(m_class, "<init>", "(Ljava/lang/String;)V"), (jstring)url));
        ThrowIfFaulted(m_env);
    }

    URL::URL(jobject object)
        : Object{object}
    {
    }

    URLConnection URL::OpenConnection()
    {
        auto urlConnection{m_env->CallObjectMethod(JObject(), m_env->GetMethodID(m_class, "openConnection", "()Ljava/net/URLConnection;"))};
        ThrowIfFaulted(m_env);
        return {urlConnection};
    }

    lang::String URL::ToString()
    {
        auto string{(jstring)m_env->CallObjectMethod(JObject(), m_env->GetMethodID(m_class, "toString", "()Ljava/lang/String;"))};
        ThrowIfFaulted((m_env));
        return {string};
    }

    URLConnection::URLConnection(jobject object)
        : Object{object}
    {
    }

    bool URLConnection::GetDoOutput() const
    {
        auto output = m_env->CallBooleanMethod(JObject(), m_env->GetMethodID(m_class, "getDoOutput", "()Z"));
        ThrowIfFaulted(m_env);
        return {static_cast<bool>(output)};
    }

    void URLConnection::SetDoOutput(bool n)
    {
        m_env->CallVoidMethod(JObject(),  m_env->GetMethodID(m_class, "setDoOutput", "(Z)V"), true);
        ThrowIfFaulted(m_env);
    }

    void URLConnection::SetRequestProperty(const std::string& key, const std::string& value)
    {
        jstring propertyName = m_env->NewStringUTF(key.c_str());
        jstring propertyValue = m_env->NewStringUTF(value.c_str());
        m_env->CallVoidMethod(JObject(), m_env->GetMethodID(m_class, "setRequestProperty", "(Ljava/lang/String;Ljava/lang/String;)V"), propertyName, propertyValue);
        ThrowIfFaulted(m_env);
    }

    void URLConnection::Connect()
    {
        m_env->CallVoidMethod(JObject(), m_env->GetMethodID(m_class, "connect", "()V"));
        ThrowIfFaulted(m_env);
    }

    URL URLConnection::GetURL() const
    {
        auto url{m_env->CallObjectMethod(JObject(), m_env->GetMethodID(m_class, "getURL", "()Ljava/net/URL;"))};
        ThrowIfFaulted(m_env);
        return {url};
    }

    int URLConnection::GetContentLength() const
    {
        auto contentLength{m_env->CallIntMethod(JObject(), m_env->GetMethodID(m_class, "getContentLength", "()I"))};
        ThrowIfFaulted(m_env);
        return contentLength;
    }

    io::InputStream URLConnection::GetInputStream() const
    {
        auto inputStream{m_env->CallObjectMethod(JObject(), m_env->GetMethodID(m_class, "getInputStream", "()Ljava/io/InputStream;"))};
        ThrowIfFaulted(m_env);
        return {inputStream};
    }

    io::OutputStream URLConnection::GetOutputStream() const
    {
        auto outputStream = m_env->CallObjectMethod(JObject(), m_env->GetMethodID(m_class, "getOutputStream", "()Ljava/io/OutputStream;"));
        ThrowIfFaulted(m_env);
        return {outputStream};
    }

    lang::String URLConnection::GetHeaderField(int n) const
    {
        auto result{(jstring)m_env->CallObjectMethod(JObject(), m_env->GetMethodID(m_class, "getHeaderField", "(I)Ljava/lang/String;"), n)};
        ThrowIfFaulted(m_env);
        return {result};
    }

    lang::String URLConnection::GetHeaderFieldKey(int n) const
    {
        auto result{(jstring)m_env->CallObjectMethod(JObject(), m_env->GetMethodID(m_class, "getHeaderFieldKey", "(I)Ljava/lang/String;"), n)};
        ThrowIfFaulted(m_env);
        return {result};
    }

    URLConnection::operator HttpURLConnection() const
    {
        return {JObject()};
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
        m_env->CallVoidMethod(JObject(), m_env->GetMethodID(m_class, "requestPermissions", "([Ljava/lang/String;I)V"), permissionArray, permissionRequestID);
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
        return {m_env->CallObjectMethod(JObject(), m_env->GetMethodID(m_class, "getApplicationContext", "()Landroid/content/Context;"))};
    }

    res::AssetManager Context::getAssets() const
    {
        return {m_env->CallObjectMethod(JObject(), m_env->GetMethodID(m_class, "getAssets", "()Landroid/content/res/AssetManager;"))};
    }

    jobject Context::getSystemService(const char* serviceName)
    {
        return m_env->CallObjectMethod(JObject(), m_env->GetMethodID(m_class, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;"), m_env->NewStringUTF(serviceName));
    }

    res::Resources Context::getResources() {
        return {m_env->CallObjectMethod(JObject(), m_env->GetMethodID(m_class, "getResources", "()Landroid/content/res/Resources;"))};
    }

    bool Context::checkSelfPermission(jstring systemPermissionName)
    {
        // Get the package manager, and get the value that represents a successful permission grant.
        jclass packageManager{m_env->FindClass("android/content/pm/PackageManager")};
        jfieldID permissionGrantedId{m_env->GetStaticFieldID(packageManager, "PERMISSION_GRANTED", "I")};
        jint permissionGrantedValue{m_env->GetStaticIntField(packageManager, permissionGrantedId)};

        // Perform the actual permission check by checking against the android context object.
        jint permissionCheckResult{m_env->CallIntMethod(JObject(), m_env->GetMethodID(m_class, "checkSelfPermission", "(Ljava/lang/String;)I"), systemPermissionName)};
        ThrowIfFaulted(m_env);
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
        return AAssetManager_fromJava(m_env, JObject());
    }

    Configuration::Configuration(jobject object)
        : Object(object)
    {
    }

    int Configuration::getDensityDpi()
    {
        return m_env->GetIntField(JObject(), m_env->GetFieldID(m_class, "densityDpi", "I"));
    }

    Resources::Resources(jobject object)
        : Object(object)
    {
    }

    Configuration Resources::getConfiguration()
    {
        return {m_env->CallObjectMethod(JObject(), m_env->GetMethodID(m_class, "getConfiguration", "()Landroid/content/res/Configuration;"))};
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
        return m_env->CallIntMethod(JObject(), m_env->GetMethodID(m_class, "getRotation", "()I"));
    }

    WindowManager::WindowManager(jobject object)
        : Object(object)
    {
    }

    Display WindowManager::getDefaultDisplay()
    {
        return {m_env->CallObjectMethod(JObject(), m_env->GetMethodID(m_class, "getDefaultDisplay", "()Landroid/view/Display;"))};
    }

    Surface::Surface(android::graphics::SurfaceTexture& surfaceTexture)
        : Object("android/view/Surface")
    {
        JObject(m_env->NewObject(m_class, m_env->GetMethodID(m_class, "<init>", "(Landroid/graphics/SurfaceTexture;)V"), (jobject)surfaceTexture));
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
        auto scheme{(jstring)m_env->CallObjectMethod(JObject(), m_env->GetMethodID(m_class, "getScheme", "()Ljava/lang/String;"))};
        ThrowIfFaulted(m_env);
        return {scheme};
    }

    java::lang::String Uri::getPath() const
    {
        auto path{(jstring)m_env->CallObjectMethod(JObject(), m_env->GetMethodID(m_class, "getPath", "()Ljava/lang/String;"))};
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
        JObject(m_env->NewObject(m_class, m_env->GetMethodID(m_class, "<init>", "(I)V"), texture));
    }

    void SurfaceTexture::updateTexImage() const
    {
        if (JObject()) {
            m_env->CallVoidMethod(JObject(), m_env->GetMethodID(m_class, "updateTexImage", "()V"));
        }
    }

    void SurfaceTexture::setDefaultBufferSize(int width, int height)
    {
        if (JObject()) {
            m_env->CallVoidMethod(JObject(), m_env->GetMethodID(m_class, "setDefaultBufferSize", "(II)V"), width, height);
        }
    }

}