#include "UrlRequest_Base.h"

#include <curl/curl.h>
#include <unistd.h>
#include <filesystem>
#include <cassert>
#include <sstream>

namespace
{
    void curl_check(CURLcode code)
    {
        if (code != CURLE_OK)
        {
            throw std::runtime_error{(std::stringstream{} << "CURL call failed with code (" << code << ")").str()};
        }
    }

    void curl_check(CURLUcode code)
    {
        if (code != CURLUE_OK)
        {
            throw std::runtime_error{(std::stringstream{} << "CURLU call failed with code (" << code << ")").str()};
        }
    }
}

namespace UrlLib
{
    class UrlRequest::Impl : public ImplBase
    {
    public:
        ~Impl()
        {
            Cleanup();
        }

        void Open(UrlMethod method, const std::string& url)
        {
            Cleanup();

            m_method = method;

            m_curl = curl_easy_init();
            if (m_curl)
            {
                m_curlu = curl_url();
                if (!m_curlu)
                {
                    throw std::runtime_error{"Out of memory"};
                }

                CURLUcode rc = curl_url_set(m_curlu, CURLUPART_URL, url.data(), CURLU_URLENCODE | CURLU_NON_SUPPORT_SCHEME);
                if (rc != CURLUE_OK)
                {
                    throw std::runtime_error{"Unable to build URL"};
                }

                char* scheme{};
                auto schemeScope = gsl::finally([&scheme] { curl_free(scheme); });
                if (curl_url_get(m_curlu, CURLUPART_SCHEME, &scheme, 0) == CURLUE_OK)
                {
                    if (std::strcmp(scheme, "app") == 0)
                    {
                        curl_check(curl_url_set(m_curlu, CURLUPART_SCHEME, "file", 0));

                        char exe[1024];
                        int ret = readlink("/proc/self/exe", exe, std::size(exe) - 1);
                        if (ret == -1)
                        {
                            throw std::runtime_error{"Unable to get executable location"};
                        }
                        exe[ret] = 0;

                        char* host{};
                        auto hostScope = gsl::finally([&host] { curl_free(host); });
                        curl_check(curl_url_get(m_curlu, CURLUPART_HOST, &host, 0));

                        char* path{};
                        auto pathScope = gsl::finally([&path] { curl_free(path); });
                        curl_check(curl_url_get(m_curlu, CURLUPART_PATH, &path, 0));

                        auto newPath = std::filesystem::path{exe}.parent_path() / host / (path + 1);
                        curl_check(curl_url_set(m_curlu, CURLUPART_PATH, newPath.generic_u8string().data(), 0));

                        m_file = true;
                    }
                    else if (std::strcmp(scheme, "file") == 0)
                    {
                        m_file = true;
                    }
                }

                curl_check(curl_easy_setopt(m_curl, CURLOPT_CURLU, m_curlu));
                curl_check(curl_easy_setopt(m_curl, CURLOPT_HEADERDATA, this));
                curl_check(curl_easy_setopt(m_curl, CURLOPT_HEADERFUNCTION, HeaderCallback));
                curl_check(curl_easy_setopt(m_curl, CURLOPT_FOLLOWLOCATION, 1L));
            }
        }

        arcana::task<void, std::exception_ptr> SendAsync()
        {
            switch (m_responseType)
            {
                case UrlResponseType::String:
                {
                    return PerformAsync(m_responseString);
                }
                case UrlResponseType::Buffer:
                {
                    return PerformAsync(m_responseBuffer);
                }
            }

            throw std::runtime_error{"Invalid response type"};
        }

        gsl::span<const std::byte> ResponseBuffer() const
        {
            return m_responseBuffer;
        }

    private:
        void Cleanup()
        {
            if (m_thread.has_value())
            {
                m_thread->join();
                m_thread = {};
            }

            if (m_curlu)
            {
                curl_url_cleanup(m_curlu);
                m_curlu = nullptr;
            }

            if (m_curl)
            {
                curl_easy_cleanup(m_curl);
                m_curl = nullptr;
            }
        }


        static void Append(std::string& string, char* buffer, size_t nitems)
        {
            string.insert(string.end(), buffer, buffer + nitems);
        }

        static void Append(std::vector<std::byte>& byteVector, char* buffer, size_t nitems)
        {
            auto bytes = reinterpret_cast<std::byte*>(buffer);
            byteVector.insert(byteVector.end(), bytes, bytes + nitems);   
        }

        template<typename DataT>
        arcana::task<void, std::exception_ptr> PerformAsync(DataT& data)
        {
            data.clear();

            curl_write_callback callback = [](char* buffer, size_t /*size*/, size_t nitems, void* userData) {
                auto& data = *static_cast<DataT*>(userData);
                Append(data, buffer, nitems);
                return nitems;
            };

            curl_check(curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, callback));
            curl_check(curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &data));

            arcana::task_completion_source<void, std::exception_ptr> taskCompletionSource{};

            m_thread.emplace([this, taskCompletionSource]() mutable
            {
                curl_check(curl_easy_perform(m_curl));

                long codep{};
                curl_check(curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &codep));
                if (codep == 0 && m_file)
                {
                    // File scheme always returns 0
                    m_statusCode = UrlStatusCode::Ok;
                }
                else
                {
                    m_statusCode = static_cast<UrlStatusCode>(codep);
                }

                taskCompletionSource.complete();
            });

            return taskCompletionSource.as_task();
        }

        static size_t HeaderCallback(char* buffer, size_t size, size_t nitems, void* userdata)
        {
            if (nitems > 0)
            {
                char* bufferEnd = buffer + nitems;

                char* keyStart = buffer;
                char* keyEnd = keyStart;
                for (; keyEnd < bufferEnd; ++keyEnd)
                {
                    if (*keyEnd == ':')
                    {
                        break;
                    }
                }

                if (keyEnd != bufferEnd)
                {
                    char* valueStart = keyEnd + 1;
                    for (; valueStart < bufferEnd; ++valueStart)
                    {
                        if (*valueStart != ' ')
                        {
                            break;
                        }
                    }

                    char* valueEnd = bufferEnd;
                    for (; valueEnd - 1 >= valueStart; --valueEnd)
                    {
                        auto ch = *(valueEnd - 1);
                        if (ch != '\r' && ch != '\n' && ch != ' ')
                        {
                            break;
                        }
                    }

                    std::string key{keyStart, keyEnd};
                    std::string value{valueStart, valueEnd};
                    static_cast<Impl*>(userdata)->m_headers.insert({std::move(key), std::move(value)});
                }
            }

            return nitems * size;
        }

        std::vector<std::byte> m_responseBuffer{};
        CURL* m_curl{};
        CURLU* m_curlu{};
        bool m_file{};
        std::optional<std::thread> m_thread{};
    };
}

#include "UrlRequest_Shared.h"
