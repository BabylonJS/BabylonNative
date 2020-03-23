#pragma once
#include <Babylon/JsRuntime.h>
#include <android/looper.h>

#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <vector>

#include <unistd.h>

// Justin's Dispatch for ReactNative integration
class DispatchPayload
{
public:
    DispatchPayload(Napi::Env env, std::function<void(Napi::Env)> func)
        : m_env{env}
        , m_func{func}
    {
    }

    DispatchPayload(const DispatchPayload&) = delete;
    DispatchPayload(DispatchPayload&&) = delete;
    DispatchPayload& operator=(const DispatchPayload&) = delete;
    
    void Call()
    {
        m_func(m_env);
    }

//private:
    Napi::Env m_env;
    std::function<void(Napi::Env)> m_func{};
};

// Simpler Dispatch for network
class DispatchPayloadDirect
{
public:
    DispatchPayloadDirect(std::function<void()> func)
        : m_func{func}
    {
    }

    DispatchPayloadDirect(const DispatchPayloadDirect&) = delete;
    DispatchPayloadDirect(DispatchPayloadDirect&&) = delete;
    DispatchPayloadDirect& operator=(const DispatchPayloadDirect&) = delete;
    
    void Call()
    {
        m_func();
    }
    std::function<void()> m_func{};
};

template<typename T> void DispatchLooperFunction(ALooper* looper, std::function<void()> func)
{
    ALooper_acquire(looper);
    
    int fileDescriptors[2];
    pipe(fileDescriptors);
    const int FD_READ = fileDescriptors[0];
    const int FD_WRITE = fileDescriptors[1];

    ALooper_addFd(looper, FD_READ, ALOOPER_POLL_CALLBACK, ALOOPER_EVENT_INPUT, [](int fd, int, void* data)
    {
        T* payloadPtr{};
        
        ssize_t bytesRead = read(fd, &payloadPtr, sizeof(decltype(payloadPtr)));
        if (bytesRead != sizeof(decltype(payloadPtr)))
        {
            std::stringstream ss{};
            ss << "Bytes read was " << bytesRead << " instead of " << sizeof(decltype(payloadPtr));
            throw;
        }

        payloadPtr->Call();
        delete payloadPtr;

        auto* looper = reinterpret_cast<ALooper*>(data);
        ALooper_removeFd(looper, fd);
        close(fd);
        
        return 0;
    }, looper);

    auto* payloadPtr = new T(std::move(func));
    if (write(FD_WRITE, &payloadPtr, sizeof(decltype(payloadPtr))) != sizeof(decltype(payloadPtr)))
    {
        throw; // Fail! Tell me what's the matter!!
    }
    close(FD_WRITE);

    ALooper_release(looper);
}

inline void log(const char* str)
{
    __android_log_print(ANDROID_LOG_VERBOSE, "BabylonNative", "%s", str);
}
