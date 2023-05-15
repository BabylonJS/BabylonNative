#import <WebSocket_Apple_ObjC.h>

@interface WebSocket_ObjC() <NSURLSessionWebSocketDelegate>
{
    NSURLSessionWebSocketTask *webSocketTask API_AVAILABLE(ios(13.0));
    NSURLSession *session;
    NSString *websocketUrl;
    // store callbacks here
    void (^openCallback)();
    void (^closeCallback)(int, NSString *);
    void (^messageCallback)(NSString *);
    void (^errorCallback)(NSString *);
}
@end

@implementation WebSocket_ObjC

- (instancetype)initWithCallbacks:(NSString *)url onOpen:(void (^)(void))onOpen onClose:(void (^)(int, NSString *))onClose onMessage:(void (^)(NSString *))onMessage onError:(void (^)(NSString *))onError
{
    self = [super init];
    websocketUrl = url;
    openCallback = onOpen;
    closeCallback = onClose;
    messageCallback = onMessage;
    errorCallback = onError;
    return self;
}


-(void) open
{
    session = [NSURLSession sessionWithConfiguration:[NSURLSessionConfiguration defaultSessionConfiguration] delegate:self delegateQueue:[[NSOperationQueue alloc] init]];
    webSocketTask = [session webSocketTaskWithURL:[NSURL URLWithString:websocketUrl]];
    [webSocketTask resume];
}

- (void) close 
{
    [webSocketTask cancel];
    NSString *closeStr = [NSString stringWithUTF8String:"Normal close."];
    [self invalidateAndCancelWithCloseCode:1000 reason:closeStr];
}

- (void) sendMessage:(NSString *)message
{
    [webSocketTask sendMessage: [[NSURLSessionWebSocketMessage alloc] initWithString:message] completionHandler:^(NSError * _Nullable error)
    {
        if (error) 
        {
            NSString *errorMessage = [error localizedDescription];
            errorCallback(errorMessage);
        }
    }];
}

- (void)receiveMessage
{
    [webSocketTask receiveMessageWithCompletionHandler:^(NSURLSessionWebSocketMessage * _Nullable message, NSError * _Nullable error)
    {
        if (error) 
        {
            NSString *errorMessage = [error localizedDescription];
            errorCallback(errorMessage);
        }
        else if (message.type == NSURLSessionWebSocketMessageTypeString)
        {
            messageCallback(message.string);
        }
        [self receiveMessage];
    }];
}

- (void)URLSession:(NSURLSession *)session webSocketTask:(NSURLSessionWebSocketTask *)webSocketTask didOpenWithProtocol:(NSString *)protocol  API_AVAILABLE(ios(13.0))
{
    openCallback();
    // run the loop to receive messages
    [self receiveMessage];
}

- (void)URLSession:(NSURLSession *)session task:(NSURLSessionTask *)task didCompleteWithError:(NSError *)error 
{
    if (error)
    {
        NSString *errorMessage = [error localizedDescription];
        errorCallback(errorMessage);
    }
}

- (void)URLSession:(NSURLSession *)session webSocketTask:(NSURLSessionWebSocketTask *)webSocketTask didCloseWithCloseCode:(NSInteger)code reason:(NSData *)reason  API_AVAILABLE(ios(13.0))
{
    NSString *reasonStr = [[NSString alloc] initWithData:reason encoding:NSUTF8StringEncoding];
    if (code != 1000)
    {
        errorCallback(reasonStr);
    }

    [self invalidateAndCancelWithCloseCode:(int)code reason:reasonStr];
}

- (void)invalidateAndCancelWithCloseCode:(int)code reason:(NSString *) reason
{
    webSocketTask = nil;
    [session invalidateAndCancel];
    session = nil;
    closeCallback(code, reason);
}

@end
