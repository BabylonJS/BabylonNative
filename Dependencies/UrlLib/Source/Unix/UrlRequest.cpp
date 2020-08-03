#include <UrlLib/UrlLib.h>
#include <arcana/threading/task.h>
#include <arcana/threading/task_schedulers.h>
#include <curl/curl.h>

namespace UrlLib
{
    class UrlRequest::Impl
    {
        using ByteArray = std::vector<std::byte>;
    public:
        ~Impl()
        {
            Abort();
        }

        void Abort()
        {
            m_cancellationSource.cancel();
        }

        void Open(UrlMethod method, std::string url)
        {
            m_method = method;
            m_url = std::move(url);
        }

        UrlResponseType ResponseType() const
        {
            return m_responseType;
        }

        void ResponseType(UrlResponseType value)
        {
            m_responseType = value;
        }

        arcana::task<void, std::exception_ptr> SendAsync()
        {
            m_taskCompletionSource = {};
            switch (m_responseType)
            {
                case UrlResponseType::String:
                {
                    LoadFile(m_responseString);
                    break;
                }
                case UrlResponseType::Buffer:
                {
                    LoadFile(m_responseBuffer);
                    break;
                }
            }
            return m_taskCompletionSource.as_task();
        }

        UrlStatusCode StatusCode() const
        {
            return m_statusCode;
        }

        gsl::cstring_span<> ResponseUrl()
        {
            return m_responseUrl;
        }

        gsl::cstring_span<> ResponseString()
        {
            return m_responseString;
        }

        gsl::span<const std::byte> ResponseBuffer() const
        {
            return m_responseBuffer;
        }

    private:
        struct CurlMulti
        {
            CurlMulti()
            : m_multiHandle(curl_multi_init())
            {
                assert(m_multiHandle != nullptr);
                curl_multi_setopt(m_multiHandle, CURLMOPT_MAX_TOTAL_CONNECTIONS, 200);
                curl_multi_setopt(m_multiHandle, CURLMOPT_MAX_HOST_CONNECTIONS, 6L);
                std::thread([this](){
                    Loop();
                }).detach();
            }

            ~CurlMulti()
            {
                auto errCode = curl_multi_cleanup(m_multiHandle);
                assert(errCode == CURLM_OK);
            }

            void AddHandle(CURL* handle)
            {
                std::lock_guard<std::mutex> guard(m_mutex);
                CURLMcode errCode = curl_multi_add_handle(m_multiHandle, handle); 
                assert(errCode == CURLM_OK);
                (void)errCode;
            }

        private:
            CURLM* m_multiHandle;
            // can't add new handle when curl is inside a callback. To be certain, a mutex is used when performing calls.
            std::mutex m_mutex;

            void Loop()
            {
                while (true) 
                {
                    int stillRunning;
                    int numfds;
                    int msgsLeft;
                    curl_multi_wait(m_multiHandle, NULL, 0, 1000, &numfds);
                    {
                        std::lock_guard<std::mutex> guard(m_mutex);
                        curl_multi_perform(m_multiHandle, &stillRunning);
                    }
                    
                    // See how the transfers went
                    CURLMsg* m = nullptr;
                    while ((m = curl_multi_info_read(m_multiHandle, &msgsLeft))) 
                    {
                        if (m->msg == CURLMSG_DONE) 
                        {
                            CURL* handle = m->easy_handle;
                            UrlRequest::Impl *request;
                            curl_easy_getinfo(handle, CURLINFO_PRIVATE, &request);
                            if (m->data.result == CURLE_OK) 
                            {
                                long codep;
                                curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &codep);

                                // codep 0 for file access
                                if (codep == 0 || codep == 200) 
                                {
                                    request->m_statusCode = UrlStatusCode::Ok;
                                }
                            }
                            request->m_taskCompletionSource.complete();

                            curl_multi_remove_handle(m_multiHandle, handle);
                            curl_easy_cleanup(handle);
                        }
                    }
                }
            }
        };

        static void Append(std::string& string, char* buffer, size_t nitems)
        {
            string.insert(string.end(), buffer, buffer + nitems);
        }

        static void Append(ByteArray& byteArray, char* buffer, size_t nitems)
        {
            auto bytes = reinterpret_cast<std::byte const*>(buffer);
            byteArray.insert(byteArray.end(), bytes, bytes + nitems);   
        }

        template<typename DataT> void LoadFile(DataT& data)
        {
            auto curl = curl_easy_init();
            if (curl)
            {
                data.clear();
                curl_easy_setopt(curl, CURLOPT_URL, m_url.data());
                curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

                curl_write_callback callback = [](char* buffer, size_t /*size*/, size_t nitems, void* userData) {
                    auto& data = *static_cast<DataT*>(userData);
                    Append(data, buffer, nitems);
                    return nitems;
                };

                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
                curl_easy_setopt(curl, CURLOPT_PRIVATE, this);
                s_curlMulti.AddHandle(curl);
            }
        }

        static inline CurlMulti s_curlMulti{};
        arcana::cancellation_source m_cancellationSource{};
        arcana::task_completion_source<void, std::exception_ptr> m_taskCompletionSource{};
        UrlResponseType m_responseType{UrlResponseType::String};
        UrlMethod m_method{UrlMethod::Get};
        UrlStatusCode m_statusCode{UrlStatusCode::None};
        std::string m_url{};
        std::string m_responseUrl{};
        std::string m_responseString{};
        ByteArray m_responseBuffer{};
    };
}

#include <Shared/UrlRequest.h>
