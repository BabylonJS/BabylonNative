#import <Cocoa/Cocoa.h>
#import <Babylon/DebugTrace.h>

int main(int argc, const char * argv[]) {
    Babylon::DebugTrace::EnableDebugTrace(true);
    Babylon::DebugTrace::SetTraceOutput([](const char* trace) { NSLog(@"%s", trace); });

    return NSApplicationMain(argc, argv);
}
