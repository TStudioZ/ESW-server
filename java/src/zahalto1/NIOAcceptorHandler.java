package zahalto1;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.nio.channels.SelectionKey;
import java.nio.channels.ServerSocketChannel;
import java.nio.channels.SocketChannel;

public class NIOAcceptorHandler extends NIOHandler {

    private final NIOReactor[] reactors;
    private final ServerSocketChannel serverSocketChannel;
    private int reactorIndex;

    private final WordsCounter wordsCounter;

    static NIOAcceptorHandler newNIOAcceptorHandlerInstance(NIOReactor[] reactors, int port) throws IOException {
        ServerSocketChannel socketChannel = ServerSocketChannel.open();
        socketChannel.socket().bind(new InetSocketAddress(port));
        socketChannel.configureBlocking(false);
        return new NIOAcceptorHandler(reactors, socketChannel, SelectionKey.OP_ACCEPT);
    }

    private NIOAcceptorHandler(NIOReactor[] reactors, ServerSocketChannel serverSocketChannel, int selectableOps) {
        super(serverSocketChannel, selectableOps);
        this.reactors = reactors;
        this.serverSocketChannel = serverSocketChannel;
        this.reactorIndex = 0;

        this.wordsCounter = new WordsCounter();
    }

    @Override
    public void run() {
        try {
            //System.out.println("Accepting a SocketChannel...");
            SocketChannel socketChannel = serverSocketChannel.accept();
            //System.out.println("SocketChannel accepted");
            if (socketChannel != null) {
                reactors[reactorIndex].register(new NIOClientHandler(socketChannel, wordsCounter));
                reactorIndex = (reactorIndex + 1) % reactors.length;
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
