#pragma once

// route stdout to android log so printf outputs can be visible in logcat
namespace android::StdoutLogger
{
    void Start();
    void Stop();
}