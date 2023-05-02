#import <WebSocket_Apple_ObjC.h>

@interface WebSocket_ObjC() <NSURLSessionWebSocketDelegate>
{
    NSURLSessionWebSocketTask *webSocketTask API_AVAILABLE(ios(13.0));
    NSURLSession *session;
    NSString *websocketUrl;
    // store callbacks here
    void (^openCallback)();
    void (^closeCallback)();
    void (^messageCallback)(NSString *);
    void (^errorCallback)();
}
@end

@implementation WebSocket_ObjC

- (instancetype)initWithCallbacks:(NSString *)url onOpen:(void (^)(void))onOpen onClose:(void (^)(void))onClose onMessage:(void (^)(NSString *))onMessage onError:(void (^)(void))onError
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
    [self invalidateAndCancel];
}

- (void) sendMessage:(NSString *)message
{
    [webSocketTask sendMessage: [[NSURLSessionWebSocketMessage alloc] initWithString:message] completionHandler:^(NSError * _Nullable error)
    {
        if (error) 
        {
            errorCallback();
        } 
    }];
}

- (void)receiveMessage
{
    [webSocketTask receiveMessageWithCompletionHandler:^(NSURLSessionWebSocketMessage * _Nullable message, NSError * _Nullable error)
    {
        if (error) 
        {
            errorCallback();
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
        errorCallback();
    }
}

- (void)URLSession:(NSURLSession *)session webSocketTask:(NSURLSessionWebSocketTask *)webSocketTask didCloseWithCloseCode:(NSInteger)code reason:(NSData *)reason  API_AVAILABLE(ios(13.0))
{
    if (code != 1000)
    {
        errorCallback();
    }
    [self invalidateAndCancel];
}

- (void)invalidateAndCancel
{
    webSocketTask = nil;
    [session invalidateAndCancel];
    session = nil;
    closeCallback();
}

@end
