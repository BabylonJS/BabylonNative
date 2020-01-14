#include <XMLHttpRequest/XMLHttpRequest.h>

#include <android/native_window.h> // requires ndk r5 or newer
#include <android/native_window_jni.h> // requires ndk r5 or newer
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

namespace Babylon
{
    class XMLHttpRequestAndroid : public XMLHttpRequestBase<XMLHttpRequestAndroid>
    {
    public:
        explicit XMLHttpRequestAndroid(const Napi::CallbackInfo& info)
            : XMLHttpRequestBase{ info }
        {
        }

        static void CreateInstance(Napi::Env env, AAssetManager* assetMgrNative)
        {
            m_assetMgrNative = assetMgrNative;
            XMLHttpRequestBase<XMLHttpRequestAndroid>::CreateInstance(env);
        }

    protected:

        void XMLHttpRequest::SendAsync()
        {
            // try to load apk asset
            auto data = GetAssetContents(m_url);
            if (!asset.empty())
            {
                if (m_responseType.empty() || m_responseType == XMLHttpRequestTypes::ResponseType::Text)
                {
                    m_responseText = std::move(data);
                }
                else if (m_responseType == XMLHttpRequestTypes::ResponseType::ArrayBuffer)
                {
                    m_response = Napi::Persistent(Napi::ArrayBuffer::New(Env(), data.size()));
                    memcpy(m_response.Value().Data(), data.data(), data.size());
                }
                else
                {
                    throw std::exception();
                }
                m_status = HTTPStatusCode::Ok;
                SetReadyState(ReadyState::Done);
            }
            else
            {
                return XMLHttpRequest::SendAsync();
            }
        }
        
        std::vector<char> GetAssetContents(const char* filename)
        {
            std::string filenameStr{filename};
            if (filenameStr.substr(0, strlen(Root)) == std::string(Root))
            {
                filename += strlen(Root) + 1; // + "/"
            }
            std::vector<char> buffer;
            AAsset *asset = AAssetManager_open(m_assetMgrNative, filename,
                                               AASSET_MODE_UNKNOWN);
            if (asset != nullptr)
            {
                size_t size = AAsset_getLength64(asset);
                buffer.resize(size);
                AAsset_read(asset, buffer.data(), size);
                AAsset_close(asset);
            }
            return buffer;
        }
        
        AAssetManager* m_assetMgrNative;
    }
}
