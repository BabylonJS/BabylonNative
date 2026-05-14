#import <Cocoa/Cocoa.h>
#import <Babylon/DebugTrace.h>
#import <Shared/Diagnostics.h>

int main(int argc, const char * argv[]) {
    Diagnostics::Initialize();

    Babylon::DebugTrace::EnableDebugTrace(true);
    Babylon::DebugTrace::SetTraceOutput([](const char* trace) { NSLog(@"%s", trace); });

    return NSApplicationMain(argc, argv);
}
