#include <Babylon/AppRuntime.h>
#include <Babylon/ScriptLoader.h>
#include <Babylon/Polyfills/IndexedDB.h>
#include <gtest/gtest.h>
#include <atomic>
#include <chrono>
#include <future>
#include <iostream>
#include <string>

TEST(IndexedDB, PreservesHostImplementation)
{
    std::promise<void> done;
    Babylon::AppRuntime runtime{};

    runtime.Dispatch([&done](Napi::Env env) {
        auto global = env.Global();
        auto hostIndexedDB = Napi::Object::New(env);
        global.Set("indexedDB", hostIndexedDB);

        Babylon::Polyfills::IndexedDB::Initialize(env);
        EXPECT_TRUE(global.Get("indexedDB").StrictEquals(hostIndexedDB));

        Babylon::Polyfills::IndexedDB::Initialize(env);
        EXPECT_TRUE(global.Get("indexedDB").StrictEquals(hostIndexedDB));
        done.set_value();
    });

    done.get_future().get();
}

TEST(IndexedDB, InstallsBrowserGlobals)
{
    std::promise<void> done;
    Babylon::AppRuntime runtime{};

    runtime.Dispatch([&done](Napi::Env env) {
        try
        {
            auto global = env.Global();
            Babylon::Polyfills::IndexedDB::Initialize(env);

            EXPECT_TRUE(global.Get("globalThis").StrictEquals(global));
            auto indexedDB = global.Get("indexedDB");
            EXPECT_TRUE(indexedDB.IsObject());
            if (indexedDB.IsObject())
            {
                EXPECT_TRUE(indexedDB.As<Napi::Object>().Get("open").IsFunction());
            }
            EXPECT_TRUE(global.Get("IDBKeyRange").IsFunction());
            EXPECT_TRUE(global.Get("IDBTransaction").IsFunction());

            Babylon::Polyfills::IndexedDB::Initialize(env);
            EXPECT_TRUE(global.Get("indexedDB").StrictEquals(indexedDB));
        }
        catch (const std::exception& error)
        {
            ADD_FAILURE() << "IndexedDB initialization failed: " << error.what();
        }
        catch (...)
        {
            ADD_FAILURE() << "IndexedDB initialization failed";
        }
        done.set_value();
    });

    done.get_future().get();
}
