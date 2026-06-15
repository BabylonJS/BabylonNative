#import <Cocoa/Cocoa.h>
#import <Shared/Diagnostics.h>

int main(int argc, const char * argv[]) {
    Diagnostics::Initialize();

    return NSApplicationMain(argc, argv);
}
