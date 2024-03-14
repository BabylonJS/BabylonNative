#include <AndroidExtensions/StdoutLogger.h>
#include <android/log.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

namespace
{
    int fd_stdout[2];
    int fd_stderr[2];

    void thread_func(int fd, int prio)
    {
        FILE* stream = fdopen(fd, "r");

        while (true)
        {
            char* line = nullptr;
            size_t n = 0;
            ssize_t nread = getline(&line, &n, stream);
            if (nread == -1)
            {
                break;
            }

            line[nread - 1] = '\0';
            __android_log_write(prio, "StdoutLogger", line);
            free(line);
        }

        fclose(stream);
    }

    void* thread_func_stdout(void*)
    {
        thread_func(fd_stdout[0], ANDROID_LOG_INFO);
        return 0;
    }

    void* thread_func_stderr(void*)
    {
        thread_func(fd_stderr[0], ANDROID_LOG_ERROR);
        return 0;
    }

    void redirect(int fd[2], int fd_redirect, void*(*thread_func)(void*))
    {
        // create the pipe and redirect
        pipe(fd);
        dup2(fd[1], fd_redirect);

        // spawn the thread
        pthread_t thr;
        if (pthread_create(&thr, 0, thread_func, 0) == -1)
        {
            return;
        }

        pthread_detach(thr);
    }
}

namespace android::StdoutLogger
{
    void Start()
    {
        redirect(fd_stdout, fileno(stdout), thread_func_stdout);
        redirect(fd_stderr, fileno(stderr), thread_func_stderr);
    }

    void Stop()
    {
        close(fd_stdout[1]);
        close(fd_stdout[0]);

        close(fd_stderr[1]);
        close(fd_stderr[0]);
   }
}