#include <gtest/gtest.h>
#ifdef HAS_NATIVE_OPTIMIZATIONS
#include <Babylon/AppRuntime.h>
#include <Babylon/Plugins/NativeOptimizations.h>
#include <napi/env.h>

#include <chrono>
#include <cstdlib>
#include <future>
#include <stdexcept>
#endif

TEST(NativeOptimizations, SplatSortingAcceptsTypedAndNumberArrayMatrices)
{
#ifndef HAS_NATIVE_OPTIMIZATIONS
    GTEST_SKIP() << "NativeOptimizations is disabled";
#else
    std::promise<void> completed;
    auto completion = completed.get_future();
    Babylon::AppRuntime runtime{};
    runtime.Dispatch([&](Napi::Env env) {
        try
        {
            Babylon::Plugins::NativeOptimizations::Initialize(env);
            Napi::Eval(env, R"(
                (function() {
                    const positions = new Float32Array([
                         1, 0,  0, 1,
                         0, 1,  0, 1,
                         0, 0,  1, 1,
                         1, 1,  1, 1,
                        -1, 0, -1, 1
                    ]);
                    for (const typed of [true, false]) {
                        const matrix = typed ? new Float32Array(16) : new Array(16).fill(0);
                        matrix[2] = 2;
                        matrix[6] = -3;
                        matrix[10] = 5;
                        const modelView = { _m: matrix };
                        for (const rightHanded of [false, true]) {
                            const indices = new Float32Array(5);
                            _native.sortSplats(modelView, positions, indices, rightHanded);
                            // Distinct depths [2, -3, 5, 4, -7] give an algorithm-independent ordering.
                            const expected = rightHanded ? [4, 1, 0, 3, 2] : [2, 3, 0, 1, 4];
                            for (let index = 0; index < indices.length; ++index) {
                                if (indices[index] !== expected[index]) {
                                    throw new Error("Unexpected splat order for typed=" + typed +
                                        ", rightHanded=" + rightHanded + ", index=" + index);
                                }
                            }
                            const single = new Float32Array([99]);
                            _native.sortSplats(modelView, positions.subarray(0, 4), single, rightHanded);
                            if (single[0] !== 0) {
                                throw new Error("Unexpected single-splat index");
                            }
                            _native.sortSplats(modelView, new Float32Array(0), new Float32Array(0), rightHanded);
                        }
                    }
                    function expectMatrixError(matrix, message) {
                        let error;
                        try {
                            _native.sortSplats({ _m: matrix }, positions, new Float32Array(5), false);
                        } catch (caught) {
                            error = caught;
                        }
                        // JSI may wrap native errors without preserving the Error prototype.
                        if (!error || error.message !== message) {
                            throw new Error("Invalid matrix must report: " + message +
                                "; received: " + String(error));
                        }
                    }
                    for (const matrix of [undefined, null, {}, 42, new Uint8Array(16), new Float64Array(16)]) {
                        expectMatrixError(matrix, "sortSplats requires modelView._m to be a Float32Array or Array.");
                    }
                    for (const component of [2, 6, 10]) {
                        const matrix = new Array(16).fill(0);
                        delete matrix[component];
                        expectMatrixError(matrix, "sortSplats requires modelView._m[2], [6], [10] to be numbers.");
                        for (const value of [undefined, null, "2", false, {}]) {
                            matrix[component] = value;
                            expectMatrixError(matrix, "sortSplats requires modelView._m[2], [6], [10] to be numbers.");
                        }
                    }
                })();
            )", "native-splat-matrix-storage.js");
            completed.set_value();
        }
        catch (const Napi::Error& error)
        {
            completed.set_exception(std::make_exception_ptr(std::runtime_error{Napi::GetErrorString(error)}));
        }
        catch (...)
        {
            completed.set_exception(std::current_exception());
        }
    });
    if (completion.wait_for(std::chrono::seconds{30}) != std::future_status::ready)
    {
        // AppRuntime teardown joins the worker; returning would hang if that worker is stuck.
        ADD_FAILURE() << "Timed out waiting for NativeOptimizations matrix storage regression";
        std::quick_exit(1);
    }
    EXPECT_NO_THROW(completion.get());
#endif
}
