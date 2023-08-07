#include "gtest/gtest.h"
#include <jni.h>
#include <Android/log.h>
#include <AndroidExtensions/Globals.h>
#include <AndroidExtensions/JavaWrappers.h>
#include <atomic>
#include <Shared/Tests.h>

std::atomic_bool stdoutLogging{false};
std::thread* stdoutPump{nullptr};

void pumpStdoutToLog() {
    using namespace std;
    __android_log_print(ANDROID_LOG_INFO, "GTEST_SETUP", "Setting up STDOUT pipe to adb log");
    int stdoutPipe[2];
    pipe(stdoutPipe);
    dup2(stdoutPipe[1], STDOUT_FILENO);
    FILE *exitEndFd = fdopen(stdoutPipe[0], "r");
    stringstream outStm;
    int c;
    stdoutLogging = true;
    // It is okay to keep it running like this.
    while (stdoutLogging) {
        c = fgetc(exitEndFd);
        if (c == '\n' || c == EOF) {
            __android_log_print(ANDROID_LOG_ERROR, "APP_STDOUT", "%s", outStm.str().c_str());
            outStm.str("");
        } else {
            outStm << (char) c;
        }
        if (c == EOF) {
            break;
        }
    }
    fclose(exitEndFd);
}

void gtestLogInit() {
    if (!stdoutPump) {
        stdoutPump = new std::thread(pumpStdoutToLog);
        usleep(400);
        fflush(stdout);
    }
}

void gtestLogFinish() {
    if (stdoutPump) {
        stdoutLogging = false;
        fflush(stdout);
        stdoutPump->join();
    }
}

extern "C" JNIEXPORT jint JNICALL
Java_com_babylonnative_unittests_Native_javaScriptTests(JNIEnv* env, jclass clazz, jobject context) {
    JavaVM* javaVM{};
    if (env->GetJavaVM(&javaVM) != JNI_OK)
    {
        throw std::runtime_error{"Failed to get Java VM"};
    }

    gtestLogInit();

    android::global::Initialize(javaVM, context);

    auto testResult = Run();
    gtestLogFinish();
    return testResult;
}
