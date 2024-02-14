#include <AndroidExtensions/StdoutLogger.h>
#include <unistd.h>
#include <android/log.h>
#include <pthread.h>
#include <stdio.h>

namespace
{
    static int pfd[2];
    static int fd_saved[2];
}

namespace android::StdoutLogger
{
    static void* ThreadFunc(void*)
    {
        ssize_t rdsz;
        char buf[128];
        while ((rdsz = read(pfd[0], buf, sizeof buf - 1)) > 0)
        {
            if (buf[rdsz - 1] == '\n') --rdsz;
            buf[rdsz] = 0;
            __android_log_write(ANDROID_LOG_DEBUG, "StdoutLogger", buf);
        }
        __android_log_write(ANDROID_LOG_DEBUG, "StdoutLogger", "Shutdown");
        return 0;
    }

    void Start()
    {
        pthread_t thr;

        // make stdout line-buffered and stderr unbuffered
        fd_saved[0] = setvbuf(stdout, 0, _IOLBF, 0);
        fd_saved[1] = setvbuf(stderr, 0, _IONBF, 0);

        // create the pipe and redirect stdout and stderr
        pipe(pfd);
        dup2(pfd[1], 1);
        dup2(pfd[1], 2);

        // spawn the logging thread
        if (pthread_create(&thr, 0, ThreadFunc, 0) == -1)
        {
            return;
        }
        pthread_detach(thr);
    }

    void Stop()
    {
        close(pfd[1]);
        close(pfd[0]);
        setvbuf(stdout, NULL, fd_saved[0], 0);
        setvbuf(stderr, NULL, fd_saved[1], 0);
    }
}