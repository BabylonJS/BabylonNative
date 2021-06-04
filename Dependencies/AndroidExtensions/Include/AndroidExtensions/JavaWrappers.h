#pragma once

#include <jni.h>
#include <string>
#include <vector>
#include <cstddef>
#include <android/asset_manager.h>
#include <android/native_window.h>

// --------------------
// Forward Declarations
// --------------------

namespace java::lang
{
    class ByteArray;
    class Object;
    class String;
}

namespace java::io
{
    class ByteArrayOutputStream;
    class InputStream;
}

namespace java::net
{
    class HttpURLConnection;
    class URL;
    class URLConnection;
}

namespace android
{
    class ManifestPermission;
}

namespace android::app
{
    class Activity;
}

namespace android::content
{
    class Context;
}

namespace android::content::res
{
    class AssetManager;
    class Resources;
    class Configuration;
}

namespace android::net
{
    class Uri;
}

// ------------
// Declarations
// ------------

namespace java::lang
{
    class ByteArray
    {
    public:
        ByteArray(int size);
        ByteArray(jbyteArray byteArray);

        operator jbyteArray() const;

        operator std::vector<std::byte>() const;

    protected:
        JNIEnv* m_env;
        jbyteArray m_byteArray;
    };

    class Class
    {
    public:
        Class(const char* className);
        Class(const jclass classObj);

        operator jclass() const;

        bool IsAssignableFrom(Class otherClass);

    protected:
        JNIEnv* m_env;
        const jclass m_class;
    };

    class Object
    {
    public:
        operator jobject() const;
        Class GetClass();

    protected:
        Object(const char* className);
        Object(jobject object);

        JNIEnv* m_env;
        Class m_class;
        jobject m_object;
    };

    class String
    {
    public:
        String(jstring string);
        String(const char* string);

        operator jstring() const;

        operator std::string() const;

    protected:
        JNIEnv* m_env;
        jstring m_string;
    };

    class Throwable : public Object, public std::exception
    {
    public:
        Throwable(jthrowable throwable);
        ~Throwable();

        String GetMessage() const;

        const char* what() const noexcept override;

    private:
        jobject m_throwableRef;
        std::string m_message;
    };
}

namespace java::io
{
    class ByteArrayOutputStream : public lang::Object
    {
    public:
        ByteArrayOutputStream();
        ByteArrayOutputStream(int size);
        ByteArrayOutputStream(jobject object);

        void Write(lang::ByteArray b, int off, int len);

        lang::ByteArray ToByteArray() const;

        lang::String ToString(const char* charsetName) const;
    };

    class InputStream : public lang::Object
    {
    public:
        InputStream(jobject object);

        int Read(lang::ByteArray byteArray) const;
    };
}

namespace java::net
{
    class HttpURLConnection : public lang::Object
    {
    public:
        static lang::Class Class();

        HttpURLConnection(jobject object);

        int GetResponseCode() const;
    };

    class URL : public lang::Object
    {
    public:
        URL(jobject object);
        URL(lang::String url);

        URLConnection OpenConnection();

        lang::String ToString();
    };

    class URLConnection : public lang::Object
    {
    public:
        URLConnection(jobject object);

        void Connect();

        URL GetURL() const;

        int GetContentLength() const;

        io::InputStream GetInputStream() const;

        explicit operator HttpURLConnection() const;
    };
}

namespace android
{
    class ManifestPermission
    {
    public:
        static jstring CAMERA();

    private:
        static jstring getPermissionName(const char* permissionName);
    };
}

namespace android::app
{
    class Activity : public java::lang::Object
    {
    public:
        Activity(jobject object);

        void requestPermissions(jstring systemPermissionName, int permissionRequestID);
    };
}

namespace android::content
{
    class Context : public java::lang::Object
    {
    public:
        Context(jobject object);

        Context getApplicationContext();

        res::AssetManager getAssets() const;

        res::Resources getResources();

        template<typename ServiceT>
        ServiceT getSystemService()
        {
            return {getSystemService(ServiceT::ServiceName)};
        };

        jobject getSystemService(const char* serviceName);

        bool checkSelfPermission(jstring systemPermissionName);
    };
}

namespace android::content::res
{
    class AssetManager : public java::lang::Object
    {
    public:
        AssetManager(jobject object);

        operator AAssetManager*() const;
    };

    class Resources : public java::lang::Object
    {
    public:
        Resources(jobject object);

        Configuration getConfiguration();
    };

    class Configuration : public java::lang::Object
    {
    public:
        Configuration(jobject object);

        int getDensityDpi();
    };
}

namespace android::graphics
{
    class SurfaceTexture : public java::lang::Object
    {
    public:
        SurfaceTexture();
        void InitWithTexture(int texture);
        void updateTexImage() const;
    };
}

namespace android::view
{
    class Display : public java::lang::Object
    {
    public:
        Display(jobject object);

        int getRotation();
    };

    class WindowManager : public java::lang::Object
    {
    public:
        static constexpr const char* ServiceName{"window"};
        WindowManager(jobject object);

        Display getDefaultDisplay();
    };

    class Surface : public java::lang::Object
    {
    public:
        Surface(android::graphics::SurfaceTexture& surfaceTexture);
    };
}

namespace android::net
{
    class Uri : public java::lang::Object
    {
    public:
        Uri(jobject object);

        java::lang::String getScheme() const;

        java::lang::String getPath() const;

        static Uri Parse(java::lang::String uriString);
    };
}
