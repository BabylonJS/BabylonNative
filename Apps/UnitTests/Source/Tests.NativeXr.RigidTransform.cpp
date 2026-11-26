#include <Babylon/AppRuntime.h>
#include "../../../Dependencies/xr/Include/XR.h"
#include "../../../Plugins/NativeXr/Source/XRRigidTransform.h"
#include <gtest/gtest.h>
#include <future>

namespace
{
    bool EvaluateTransform(const char* script)
    {
        Babylon::AppRuntime runtime;
        std::promise<bool> result;
        runtime.Dispatch([&](Napi::Env env) {
            try
            {
                Babylon::XRRigidTransform::Initialize(env);
                result.set_value(Napi::Eval(env, script, "rigid-transform.js").ToBoolean().Value());
            }
            catch (...)
            {
                result.set_exception(std::current_exception());
            }
        });
        return result.get_future().get();
    }
}

TEST(NativeXrRigidTransform, DefaultsMissingComponentsAndCopiesPosition)
{
    EXPECT_TRUE(EvaluateTransform(R"(
        const position = {z: -2};
        const t = new XRRigidTransform(position);
        position.z = 7;
        const id = new XRRigidTransform();
        t.position !== position && t.position.x === 0 && t.position.z === -2 && t.position.w === 1 &&
            t.orientation.w === 1 && t.matrix[14] === -2 && id.matrix[0] === 1 && id.matrix[15] === 1
    )"));
}

TEST(NativeXrRigidTransform, NormalizesOrientationBeforeBuildingMatrix)
{
    EXPECT_TRUE(EvaluateTransform(R"(
        const q = {z: 2, w: 0};
        const t = new XRRigidTransform(undefined, q);
        const pureX = x => new XRRigidTransform(null, {x, w: 0}).orientation.x;
        t.orientation !== q && q.z === 2 && t.orientation.z === 1 && t.position.x === 0 &&
            t.matrix[0] === -1 && t.matrix[5] === -1 && t.matrix[10] === 1 &&
            pureX(Number.MAX_VALUE) === 1 && pureX(Number.MIN_VALUE) === 1
    )"));
}

TEST(NativeXrRigidTransform, ReadsMembersOnceAndPreservesConversionExceptions)
{
    EXPECT_TRUE(EvaluateTransform(R"(
        let reads = 0;
        const t = new XRRigidTransform({get x() { ++reads; return "3"; }});
        const sentinel = new Error("application getter");
        let caught;
        try { new XRRigidTransform({get x() { throw sentinel; }}); } catch (e) { caught = e; }
        reads === 1 && t.position.x === 3 && caught === sentinel
    )"));
}

TEST(NativeXrRigidTransform, RejectsInvalidPointsAndZeroQuaternion)
{
    EXPECT_TRUE(EvaluateTransform(R"(
        function errorName(...args) {
            try { new XRRigidTransform(...args); } catch (e) { return e.name; }
        }
        errorName({w: 0}) === "TypeError" && errorName({x: Infinity}) === "TypeError" &&
            errorName({}, {y: NaN}) === "TypeError" && errorName(42) === "TypeError" &&
            errorName(null, {w: 0}) === "InvalidStateError"
    )"));
}
