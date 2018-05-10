package zahalto1;

import java.io.IOException;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.util.concurrent.ConcurrentLinkedQueue;

public class NIOReactor implements Runnable {

    private final Selector selector;
    private final ConcurrentLinkedQueue<NIOHandler> handlersToRegister;

    NIOReactor() throws IOException {
        this.selector = Selector.open();
        this.handlersToRegister = new ConcurrentLinkedQueue<>();

        Thread t = new Thread(this);
        //t.setDaemon(true);
        t.start();
    }

    void register(NIOHandler handler) {
        handlersToRegister.add(handler);
        selector.wakeup();
    }

    @Override
    public void run() {
        //System.out.println("Starting a NIOReactor");
        try {
            while (true) {
                selector.select();
                for (SelectionKey sk : selector.selectedKeys()) {
                    if (sk.attachment() != null) {
                        ((NIOHandler) sk.attachment()).run();
                    }
                }
                selector.selectedKeys().clear();
                NIOHandler handler;
                while ((handler = handlersToRegister.poll()) != null) {
                    SelectionKey sk = handler.getSelectableChannel().register(
                            selector, handler.getSelectableOps(), handler);
                    handler.setSelectionKey(sk);
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
