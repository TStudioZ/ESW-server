package zahalto1;

import java.io.IOException;

public class Main {

    public static void main(String[] args) {
        try {
            NIOServer.start(12345);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
