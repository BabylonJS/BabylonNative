import org.java_websocket.client.WebSocketClient;
import org.java_websocket.handshake.ServerHandshake;
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
        this.closeCallback(code, reason);
    }

    @Override
    public void onError(Exception ex)
    {
        this.errorCallback(ex.getMessage());
    }

    public native void openCallback();
    public native void closeCallback(int code, String reason);
    public native void messageCallback(String message);
    public native void errorCallback(String message);
}
