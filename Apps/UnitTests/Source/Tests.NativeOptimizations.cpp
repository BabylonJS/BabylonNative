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

TEST(NativeOptimizations, VertexTransformsAcceptArrayMatricesAndDefaultRanges)
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
                (function () {
                    const values = [1, 2, 3, 0, 4, 5, 6, 0, 7, 8, 9, 0, 10, 11, 12, 1];
                    const transforms = [
                        ["_TransformVector3Coordinates", 3, [1, 2, 3], [40, 47, 54]],
                        ["_TransformVector3Normals", 3, [1, 2, 3], [30, 36, 42]],
                        ["_TransformVector4Normals", 4, [1, 2, 3, 4], [30, 36, 42, 4]],
                    ];
                    for (const matrix of [new Float32Array(values), values]) {
                        for (const [name, stride, input, expected] of transforms) {
                            for (const range of [[], [undefined, undefined], [0, stride]]) {
                                const data = new Float32Array(input);
                                _native[name](data, { _m: matrix }, ...range);
                                if (Array.from(data).join(",") !== expected.join(",")) {
                                    throw new Error(name + " produced " + Array.from(data).join(","));
                                }
                            }
                        }
                    }
                    for (const Type of [Uint16Array, Uint32Array, Int32Array]) {
                        const indices = new Type([0, 1, 2, 3, 4, 5]);
                        _native._FlipFaces(indices);
                        if (Array.from(indices).join(",") !== "0,2,1,3,5,4") {
                            throw new Error("_FlipFaces did not default to the whole array");
                        }
                    }
                })();
            )", "native-vertex-transforms.js");
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
        ADD_FAILURE() << "Timed out waiting for native vertex transforms";
        std::quick_exit(1);
    }
    EXPECT_NO_THROW(completion.get());
#endif
}

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
                            // Babylon.js pads the index buffer to a multiple of 16; padding must
                            // point one past the last splat rather than read past the positions.
                            const padded = new Float32Array(16).fill(-1);
                            _native.sortSplats(modelView, positions, padded, rightHanded);
                            for (let index = 0; index < padded.length; ++index) {
                                const want = index < expected.length ? expected[index] : expected.length;
                                if (padded[index] !== want) {
                                    throw new Error("Unexpected padded splat index for typed=" + typed +
                                        ", rightHanded=" + rightHanded + ", index=" + index + ": " + padded[index]);
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
                        // JSI wraps exceptions thrown from host functions with this prefix.
                        if (!(error instanceof Error) ||
                            (error.message !== message && error.message !== "Exception in HostFunction: " + message)) {
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

TEST(NativeOptimizations, SplatIndexSortingHonorsIntervalsAndParts)
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
                        0, 0, 1, 1,
                        0, 0, 3, 1,
                        0, 0, 2, 1,
                        0, 0, 4, 1
                    ]);
                    const vertexCount = 4;
                    function matrix(typed, entries) {
                        const m = typed ? new Float32Array(16) : new Array(16).fill(0);
                        for (const [index, value] of entries) {
                            m[index] = value;
                        }
                        return { _m: m };
                    }
                    const identity = [[0, 1], [5, 1], [10, 1], [15, 1]];
                    function sort(view, worlds, indexCount, intervals, partIndices, rightHanded, sourcePositions) {
                        const indices = new Float32Array(indexCount).fill(-1);
                        const result = _native.sortSplatIndices(
                            view, worlds, sourcePositions || positions, indices, vertexCount, intervals, partIndices, !!rightHanded);
                        return { result, indices: Array.from(indices).join(",") };
                    }
                    function expectSort(label, actual, result, indices) {
                        if (actual.result !== result || actual.indices !== indices) {
                            throw new Error(label + " returned " + actual.result + " with " + actual.indices);
                        }
                    }
                    for (const typed of [true, false]) {
                        const view = matrix(typed, identity);
                        const world = matrix(typed, identity);
                        // The second part moves its splats 10 units farther away.
                        const farWorld = matrix(typed, [...identity, [14, 10]]);
                        expectSort("whole mesh", sort(view, [world], 6, null, null), true, "3,1,2,0,4,4");
                        expectSort("one interval", sort(view, [world], 4, new Uint32Array([1, 2]), null), true, "1,2,4,4");
                        expectSort("two intervals", sort(view, [world], 4, new Uint32Array([0, 1, 2, 2]), undefined), true, "3,2,0,4");
                        expectSort("empty part indices", sort(view, [world], 4, null, new Uint8Array(0)), true, "3,1,2,0");
                        expectSort("parts", sort(view, [world, farWorld], 4, null, new Uint8Array([1, 0, 0, 0])), true, "0,3,1,2");
                        // A part index without a world matrix leaves the previous order untouched.
                        expectSort("stale part", sort(view, [world, farWorld], 4, null, new Uint8Array([0, 2, 0, 0])), false, "-1,-1,-1,-1");

                        // With projectionPlaneTilt=0.1, clip w puts the nearer splat first. View-space z does not.
                        const tiltedPositions = new Float32Array([
                            0, -0.1, 5, 1,
                            0,  0.1, 4.99, 1,
                            0,    0, 1, 1,
                            0,    0, 2, 1
                        ]);
                        const fartherClipW = tiltedPositions[2] + 0.1 * tiltedPositions[1];
                        const nearerClipW = tiltedPositions[6] + 0.1 * tiltedPositions[5];
                        if (!(fartherClipW < nearerClipW)) {
                            throw new Error("Tilted projection must reverse clip-w and view-depth order");
                        }
                        expectSort("tilted perspective", sort(view, [world], 4, null, null, false, tiltedPositions), true, "0,1,3,2");

                        // Reversed-depth orthographic projection is deliberately absent from the sort input.
                        const rightHandedPositions = new Float32Array([
                            0, 0, -5, 1,
                            0, 0, -4, 1,
                            0, 0, -2, 1,
                            0, 0, -1, 1
                        ]);
                        expectSort("right-handed reverse-depth orthographic",
                            sort(view, [world], 4, null, null, true, rightHandedPositions), true, "0,1,2,3");
                    }
                    function expectError(args, message) {
                        let error;
                        try {
                            _native.sortSplatIndices(...args);
                        } catch (caught) {
                            error = caught;
                        }
                        // JSI wraps exceptions thrown from host functions with this prefix.
                        if (!(error instanceof Error) ||
                            (error.message !== message && error.message !== "Exception in HostFunction: " + message)) {
                            throw new Error("Expected: " + message + "; received: " + String(error));
                        }
                    }
                    const view = matrix(true, identity);
                    const world = matrix(true, identity);
                    expectError([view, [], positions, new Float32Array(4), vertexCount, null, null, false],
                        "sortSplatIndices requires at least one world matrix.");
                    expectError([view, [world], positions, new Float32Array(4), 5, null, null, false],
                        "sortSplatIndices requires 4 position floats per splat.");
                    expectError([view, [world], positions, new Float32Array(4), vertexCount, new Uint32Array([3, 2]), null, false],
                        "sortSplatIndices requires intervals within vertexCount.");
                    expectError([view, [world], positions, new Float32Array(4), vertexCount, new Uint32Array([0, 2, 2]), null, false],
                        "sortSplatIndices requires intervals to hold [start, count] pairs.");
                    expectError([view, [world], positions, new Float32Array(3), vertexCount, null, null, false],
                        "sortSplatIndices requires indices to hold every active splat.");
                    expectError([view, [world], positions, new Float32Array(4), vertexCount, [0, 4], null, false],
                        "sortSplatIndices requires intervals to be a Uint32Array or null and partIndices to be a Uint8Array or null.");
                })();
            )", "native-splat-index-sorting.js");
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
        ADD_FAILURE() << "Timed out waiting for NativeOptimizations splat index sorting";
        std::quick_exit(1);
    }
    EXPECT_NO_THROW(completion.get());
#endif
}
