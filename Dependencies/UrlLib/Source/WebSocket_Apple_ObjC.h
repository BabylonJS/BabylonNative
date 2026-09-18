#import <Foundation/Foundation.h>

API_AVAILABLE(ios(13.0))
@interface WebSocket_ObjC : NSObject
- (instancetype)initWithCallbacks:(NSString *)url onOpen:(void (^)(void))onOpen onClose:(void (^)(int, NSString *))onClose onMessage:(void (^)(NSString *))onMessage onError:(void (^)(NSString *))onError;
- (void)open API_AVAILABLE(ios(13.0));
- (void)close API_AVAILABLE(ios(13.0));
- (void)sendMessage:(NSString *)message  API_AVAILABLE(ios(13.0));
- (void)receiveMessage API_AVAILABLE(ios(13.0));

@end
