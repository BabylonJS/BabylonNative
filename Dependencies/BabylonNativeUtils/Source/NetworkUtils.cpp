#include "NetworkUtils.h"

#if (ANDROID)
namespace Babylon
{
    void AndroidLoadURLFunction(const std::string& url, std::function<void(const std::string& content)> func);
    void AndroidLoadURLFunction(const std::string& url, std::function<void(const std::vector<uint8_t>& content)> contentHandler);
    std::string AndroidGetAbsoluteUrl(const std::string& url, const std::string& rootUrl);
}
#else
#include <curl/curl.h>
#endif

namespace Babylon
{
#if (ANDROID)
    std::string GetAbsoluteUrl(const std::string& url, const std::string& rootUrl)
    {
        return AndroidGetAbsoluteUrl(url, rootUrl);
    }

    template<typename DataT>
    arcana::task<DataT, std::exception_ptr> LoadUrlAsync(std::string url)
    {
        arcana::task_completion_source<DataT, std::exception_ptr> taskCompletionSource{};
        std::thread{[taskCompletionSource, url = std::move(url)] () mutable
        {
            AndroidLoadURLFunction(url, [taskCompletionSource](const DataT& content) mutable
               {
                   taskCompletionSource.complete(std::move(content));
               }
            );
        }}.detach();

        return taskCompletionSource.as_task();
    }
#else
    std::string GetAbsoluteUrl(const std::string& url, const std::string& rootUrl)
    {
        auto curl = curl_url();

        auto code = curl_url_set(curl, CURLUPART_URL, url.data(), 0);

        // If input could not be turned into a valid URL, try using it as a regular URL.
        if (code == CURLUE_MALFORMED_INPUT)
        {
            code = curl_url_set(curl, CURLUPART_URL, (rootUrl + "/" + url).data(), 0);
        }

        if (code != CURLUE_OK)
        {
            throw std::exception{};
        }

        char* buf;
        code = curl_url_get(curl, CURLUPART_URL, &buf, 0);

        if (code != CURLUE_OK)
        {
            throw std::exception{};
        }

        std::string absoluteUrl{buf};

        curl_free(buf);
        curl_url_cleanup(curl);

        return std::move(absoluteUrl);
    }

    template<typename DataT>
    arcana::task<DataT, std::exception_ptr> LoadUrlAsync(std::string url)
    {
        arcana::task_completion_source<DataT, std::exception_ptr> taskCompletionSource{};

        std::thread{[taskCompletionSource, url = std::move(url)] () mutable
        {
            try
            {
                DataT data{};

            auto curl = curl_easy_init();
            if (curl)
            {
                curl_easy_setopt(curl, CURLOPT_URL, url.data());
                curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

                    curl_write_callback callback = [](char* buffer, size_t size, size_t nitems, void* userData) {
                        auto& data = *static_cast<DataT*>(userData);
                        data.insert(data.end(), buffer, buffer + nitems);
                        return nitems;
                    };

                    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
                    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);

                auto result = curl_easy_perform(curl);
                if (result != CURLE_OK)
                {
                    throw std::exception();
                }

                    curl_easy_cleanup(curl);

                    taskCompletionSource.complete(std::move(data));
                }
            }
            catch (...)
            {
                taskCompletionSource.complete(arcana::make_unexpected(std::current_exception()));
            }
        } }.detach();

        return taskCompletionSource.as_task();
    }
#endif

    arcana::task<std::string, std::exception_ptr> LoadTextAsync(std::string url)
    {
        return LoadUrlAsync<std::string>(std::move(url));
    }

    arcana::task<std::vector<uint8_t>, std::exception_ptr> LoadBinaryAsync(std::string url)
    {
        return LoadUrlAsync<std::vector<uint8_t>>(std::move(url));
    }
}