package zahalto1;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.SelectionKey;
import java.nio.channels.SocketChannel;

public class NIOClientHandler extends NIOHandler {

    private final int BUFFER_CAPACITY = 4;
    private final int WRITE_BUFFER_CAPACITY = 256;

    private final SocketChannel socketChannel;
    private final WordsCounter wordsCounter;

    private ByteBuffer readBuffer;
    private ByteBuffer writeBuffer = null;

    private byte[] bytes = null;
    private ByteArrayOutputStream received = null;

    NIOClientHandler(SocketChannel socketChannel, WordsCounter wordsCounter) throws IOException {
        super(socketChannel, SelectionKey.OP_READ);
        this.socketChannel = socketChannel;
        this.socketChannel.configureBlocking(false);
        this.wordsCounter = wordsCounter;

        this.readBuffer = ByteBuffer.allocate(BUFFER_CAPACITY);
        this.bytes = new byte[BUFFER_CAPACITY];
        this.received = new ByteArrayOutputStream();
    }

    private void read() throws IOException {
        System.out.println("NIOClientHandler reading");

        int readBytes;
        while ((readBytes = socketChannel.read(readBuffer)) > 0) {
            readBuffer.flip();
            readBuffer.get(bytes, 0, readBytes);
            received.write(bytes, 0, readBytes);
            readBuffer.clear();
        }

        System.out.println("NIOClientHandler read bytes: " + readBytes);
        if (readBytes == -1) {
            getSelectionKey().cancel();
            socketChannel.close();
        } else if (readBytes == 0) {
            System.out.println("Received message: [" + received + "]");
            SelectionKey sk = getSelectionKey();
            sk.interestOps(0);
            sk.selector().wakeup();
            NIOServer.executorService.execute(this::process);
        }
    }

    private void write(boolean setWriteInterest) throws IOException {
        System.out.println("NIOClientHandler writing");
        SelectionKey sk = getSelectionKey();
        if (socketChannel.write(writeBuffer) == -1) {
            sk.cancel();
            socketChannel.close();
        } else if (writeBuffer.remaining() > 0) {
            if (setWriteInterest) {
                sk.interestOps(SelectionKey.OP_WRITE);
            }
        } else {
            readBuffer = writeBuffer;
            readBuffer.clear();
            writeBuffer = null;
            sk.interestOps(SelectionKey.OP_READ);
        }
        if (setWriteInterest) {
            sk.selector().wakeup();
        }
    }

    private void process() {
        System.out.println("NIOClientHandler processing");
        try {

            String receivedString = new String(received.toByteArray(), "UTF-8");
            this.received.reset();
            HttpRequest httpRequest = HttpHelper.getHttpRequest(receivedString);

            String statusCode = null;
            String response = null;
            HttpRequest.Command command = httpRequest.getCommand();
            if (command != null) {
                if (command == HttpRequest.Command.DATA) {
                    String[] words = httpRequest.getData().split(" ");
                    for (int i = 0; i < words.length; i++) {
                        wordsCounter.addWord(words[i]);
                    }

                    statusCode = HttpHelper.HTTP_NO_CONTENT;
                    response = "OK";
                } else if (command == HttpRequest.Command.COUNT) {
                    int wordCount = wordsCounter.getWordCountReset();

                    statusCode = HttpHelper.HTTP_OK;
                    response = "" + wordCount;
                } else {
                    statusCode = HttpHelper.HTTP_BAD_REQUEST;
                    response = "Unknown command";
                }
            } else {
                statusCode = HttpHelper.HTTP_BAD_REQUEST;
                response = "Bad request";
            }

            readBuffer.clear();
            writeBuffer = ByteBuffer.allocate(WRITE_BUFFER_CAPACITY);

            String httpResponse = HttpHelper.getHttpResponse(statusCode, response);

            writeBuffer.put(httpResponse.getBytes());
            writeBuffer.flip();
            readBuffer = null;

            received = null;

            write(true);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void run() {
        SelectionKey sk = getSelectionKey();
        try {
            if (sk.isReadable()) {
                read();
            }
            else if (sk.isWritable()) {
                write(false);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
