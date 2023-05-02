package com.jsruntimehost.unittests;
import org.java_websocket.client.WebSocketClient;
import org.java_websocket.handshake.ServerHandshake;
import org.java_websocket.framing.CloseFrame;
import java.net.URI;
import java.net.URISyntaxException;

public class WebSocket extends WebSocketClient {
    public WebSocket(String url) throws URISyntaxException 
    {
        super(new URI(url));
    }

    @Override
    public void onOpen(ServerHandshake handshakedata)
    {
        this.openCallback();
    }

    @Override
    public void onMessage(String message)
    {
        this.messageCallback(message);
    }

    @Override
    public void onClose(int code, String reason, boolean remote)
    {
        // For all other close frames, we need to trigger the error callback
        if (code != CloseFrame.NORMAL && code != CloseFrame.ABNORMAL_CLOSE && code != CloseFrame.NEVER_CONNECTED)
        {
            this.errorCallback();
        }

        this.closeCallback();
    }

    @Override
    public void onError(Exception ex)
    {
        this.errorCallback();
    }

    public native void openCallback();
    public native void closeCallback();
    public native void messageCallback(String message);
    public native void errorCallback();
}
