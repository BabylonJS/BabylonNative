#import <WebSocket_Apple_ObjC.h>

@interface WebSocket_ObjC() <NSURLSessionWebSocketDelegate>
{
    NSURLSessionWebSocketTask* webSocketTask API_AVAILABLE(ios(13.0));
    NSURLSession* session;
    NSString* websocketUrl;
    // store callbacks here
    void (^openCallback)();
    void (^closeCallback)(int, NSString*);
    void (^messageCallback)(NSString*);
    void (^errorCallback)(NSString*);
}
@end

@implementation WebSocket_ObjC

- (instancetype)initWithCallbacks:(NSString*)url onOpen:(void (^)(void))onOpen onClose:(void (^)(int, NSString*))onClose onMessage:(void (^)(NSString*))onMessage onError:(void (^)(NSString*))onError
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
    NSURLSessionWebSocketTask* task;
    @synchronized(self)
    {
        session = [NSURLSession sessionWithConfiguration:[NSURLSessionConfiguration defaultSessionConfiguration] delegate:self delegateQueue:[[NSOperationQueue alloc] init]];
        webSocketTask = [session webSocketTaskWithURL:[NSURL URLWithString:websocketUrl]];
        task = webSocketTask;
    }
    [task resume];
}

- (void) close 
{
    [self invalidateAndCancelWithCloseCode:1000 reason:[NSString stringWithUTF8String:"Normal close."]];
}

- (void) sendMessage:(NSString*)message
{
    NSURLSessionWebSocketTask* task;
    @synchronized(self)
    {
        task = webSocketTask;
    }
    if (!task)
    {
        return;
    }
    [task sendMessage: [[NSURLSessionWebSocketMessage alloc] initWithString:message] completionHandler:^(NSError* _Nullable error)
    {
        if (error) 
        {
            [self invalidateAndCancelWithCloseCode:1006 reason:[error localizedDescription]];
        }
    }];
}

- (void)receiveMessage
{
    NSURLSessionWebSocketTask* task;
    @synchronized(self)
    {
        task = webSocketTask;
    }
    if (!task)
    {
        return;
    }
    [task receiveMessageWithCompletionHandler:^(NSURLSessionWebSocketMessage* _Nullable message, NSError* _Nullable error)
    {
        if (error) 
        {
            [self invalidateAndCancelWithCloseCode:1006 reason:[error localizedDescription]];
            return;
        }
        else if (message.type == NSURLSessionWebSocketMessageTypeString)
        {
            void (^callback)(NSString*);
            @synchronized(self)
            {
                callback = messageCallback;
            }
            if (callback)
            {
                callback(message.string);
            }
        }
        [self receiveMessage];
    }];
}

- (void)URLSession:(NSURLSession*)session webSocketTask:(NSURLSessionWebSocketTask*)webSocketTask didOpenWithProtocol:(NSString*)protocol  API_AVAILABLE(ios(13.0))
{
    void (^callback)();
    @synchronized(self)
    {
        callback = openCallback;
    }
    if (callback)
    {
        callback();
    }
    // run the loop to receive messages
    [self receiveMessage];
}

- (void)URLSession:(NSURLSession*)theSession task:(NSURLSessionTask*)task didCompleteWithError:(NSError*)error 
{
    if (error)
    {
        [self invalidateAndCancelWithCloseCode:1006 reason:[error localizedDescription]];
    }
}

- (void)URLSession:(NSURLSession*)session webSocketTask:(NSURLSessionWebSocketTask*)webSocketTask didCloseWithCloseCode:(NSInteger)code reason:(NSData*)reason  API_AVAILABLE(ios(13.0))
{
    NSString* reasonStr = [[NSString alloc] initWithData:reason encoding:NSUTF8StringEncoding];
    [self invalidateAndCancelWithCloseCode:(int)code reason:reasonStr];
}

// Close-once helper. Fires onError for abnormal codes, then onClose exactly once.
// Callbacks are invoked outside the lock to avoid reentrancy issues.
- (void)invalidateAndCancelWithCloseCode:(int)code reason:(NSString*) reason
{
    NSURLSession* sessionToInvalidate;
    void (^errorCb)(NSString*);
    void (^closeCb)(int, NSString*);

    @synchronized(self)
    {
        if (!session)
        {
            return;
        }
        webSocketTask = nil;
        sessionToInvalidate = session;
        session = nil;
        errorCb = (code != 1000) ? errorCallback : nil;
        closeCb = closeCallback;
    }

    [sessionToInvalidate invalidateAndCancel];
    if (errorCb)
    {
        errorCb(reason);
    }
    if (closeCb)
    {
        closeCb(code, reason);
    }
}

@end
