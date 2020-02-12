#include "NetworkUtils.h"

#include <curl/curl.h>

namespace Babylon
{
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

        std::string absoluteUrl{ buf };

        curl_free(buf);
        curl_url_cleanup(curl);

        return std::move(absoluteUrl);
    }

    template<typename DataT>
    arcana::task<DataT, std::exception_ptr> LoadUrlAsync(std::string url)
    {
        arcana::task_completion_source<DataT, std::exception_ptr> taskCompletionSource{};

        std::thread{ [taskCompletionSource, url = std::move(url)] () mutable
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

#if (ANDROID)
                /*
                * /!\ warning! this is a security issue
                * https://github.com/BabylonJS/BabylonNative/issues/96
                */
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
#endif

                auto result = curl_easy_perform(curl);
                if (result != CURLE_OK)
                {
                    throw std::exception();
                }

                curl_easy_cleanup(curl);

                taskCompletionSource.complete(std::move(data));
            }
        } }.detach();

        return taskCompletionSource.as_task();
    }

    arcana::task<std::string, std::exception_ptr> LoadUrlToStringAsync(std::string url)
    {
        return LoadUrlAsync<std::string>(std::move(url));
    }

    arcana::task<std::vector<uint8_t>, std::exception_ptr> LoadUrlToBytesAsync(std::string url)
    {
        return LoadUrlAsync<std::vector<uint8_t>>(std::move(url));
    }
}