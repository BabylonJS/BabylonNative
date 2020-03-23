package BabylonNative;

import java.net.*;
import java.io.*;
import java.util.Scanner;

public class Network {
    public static String loadURLString(String urlString) {
        try {
            return new Scanner(new URL(urlString).openStream(), "UTF-8").useDelimiter("\\A").next();
        } catch (MalformedURLException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return "";
    }
    public static byte[] loadURLBuffer(String urlString) {
        try {
            URL url = new URL(urlString);
            ByteArrayOutputStream baos = new ByteArrayOutputStream();
            InputStream is = url.openStream ();
            byte[] byteChunk = new byte[4096];
            int n;
            while ( (n = is.read(byteChunk)) > 0 ) {
                baos.write(byteChunk, 0, n);
            }
            return baos.toByteArray();
        } catch (MalformedURLException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return new byte[0];
    }
}
