package zahalto1;

import java.io.IOException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class NIOServer {

    private final static NIOReactor[] reactors;
    static ExecutorService executorService = Executors.newWorkStealingPool();

    static {
        reactors = new NIOReactor[4];
        try {
            for (int i = 0; i < reactors.length; i++) {
                reactors[i] = new NIOReactor();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static void start(int port) throws IOException {
        reactors[0].register(NIOAcceptorHandler.newNIOAcceptorHandlerInstance(reactors, port));
        System.out.println("Server is running on port " + port);
    }
}
