#pragma once

namespace Babylon
{
    class AppRuntimeV8 final
    {
    public:
        // Enable the inspector using a certain port so that
        // the next AppRuntime instances created after this 
        // function call will spawn an inspector protocol
        // server using this port.
        // Returns whether the inspector is supported in the
        // current compilation configuration.
        static bool EnableInspector(unsigned short port);

        // Disable the inspector so that future AppRuntime 
        // instances do not spawn an inspector protocol server.
        static void DisableInspector();
    };
}