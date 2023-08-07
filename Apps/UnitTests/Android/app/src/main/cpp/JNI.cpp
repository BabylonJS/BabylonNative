#include "gtest/gtest.h"
#include <jni.h>
#include <Android/log.h>
#include <AndroidExtensions/Globals.h>
#include <AndroidExtensions/JavaWrappers.h>
#include <atomic>
#include <Shared/Tests.h>

static int pfd[2];
static int fd_saved[2];
void *thread_func(void*)
{
    ssize_t rdsz;
    char buf[128];
    while((rdsz = read(pfd[0], buf, sizeof buf - 1)) > 0) {
        if(buf[rdsz - 1] == '\n') --rdsz;
        buf[rdsz] = 0;  /* add null-terminator */
        __android_log_write(ANDROID_LOG_DEBUG, "UnitTests", buf);
    }
    __android_log_write(ANDROID_LOG_DEBUG, "UnitTests", "Logger shutdown");
    return 0;
}

void start_logger()
{
    pthread_t thr;

    /* make stdout line-buffered and stderr unbuffered */
    fd_saved[0] = setvbuf(stdout, 0, _IOLBF, 0);
    fd_saved[1] = setvbuf(stderr, 0, _IONBF, 0);

    /* create the pipe and redirect stdout and stderr */
    pipe(pfd);
    dup2(pfd[1], 1);
    dup2(pfd[1], 2);

    /* spawn the logging thread */
    if(pthread_create(&thr, 0, thread_func, 0) == -1)
    {
        return;
    }
    pthread_detach(thr);
}

void stop_logger()
{
    close(pfd[1]);
    close(pfd[0]);
    setvbuf(stdout, NULL, fd_saved[0], 0);
    setvbuf(stderr, NULL, fd_saved[1], 0);
}

extern "C" JNIEXPORT jint JNICALL
Java_com_babylonnative_unittests_Native_javaScriptTests(JNIEnv* env, jclass clazz, jobject context) {
    JavaVM* javaVM{};
    if (env->GetJavaVM(&javaVM) != JNI_OK)
    {
        throw std::runtime_error{"Failed to get Java VM"};
    }

    start_logger();

    android::global::Initialize(javaVM, context);
    auto testResult = Run();

    stop_logger();
    return testResult;
}
