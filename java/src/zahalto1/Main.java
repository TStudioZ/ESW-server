package zahalto1;

import java.io.IOException;

public class Main {

    public static void main(String[] args) {
        if (args.length == 0) {
            System.out.println("Missing port argument");
            return;
        }
        try {
            NIOServer.start(Integer.parseInt(args[0]));
        } catch (NumberFormatException e) {
            System.out.println("Wrong port number");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
