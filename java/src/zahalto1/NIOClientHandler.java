package zahalto1;

import sun.misc.IOUtils;

import java.io.*;
import java.nio.ByteBuffer;
import java.nio.channels.SelectionKey;
import java.nio.channels.SocketChannel;
import java.util.StringTokenizer;
import java.util.zip.GZIPInputStream;

public class NIOClientHandler extends NIOHandler {

    private final SocketChannel socketChannel;
    private final WordsCounter wordsCounter;

    private final BufferPool bufferPool;
    private final ByteBuffer readBuffer;

    private byte[] bytes;

    private final HttpRequest httpRequest;

    NIOClientHandler(SocketChannel socketChannel, WordsCounter wordsCounter, BufferPool bufferPool) throws IOException {
        super(socketChannel, SelectionKey.OP_READ);
        this.socketChannel = socketChannel;
        this.socketChannel.configureBlocking(false);
        this.wordsCounter = wordsCounter;

        this.bufferPool = bufferPool;
        this.readBuffer = bufferPool.getByteBuffer();
        this.bytes = bufferPool.getBufferArray();
        this.httpRequest = new HttpRequest(bufferPool.getOutputStreamBuffer());
    }

    private void returnBuffers() {
        bufferPool.returnBuffer(readBuffer);
    }

    private void read() throws IOException {
        int readBytes;
        while ((readBytes = socketChannel.read(readBuffer)) > 0) {
            readBuffer.flip();
            readBuffer.get(bytes, 0, readBytes);
            httpRequest.addData(bytes, readBytes);
            readBuffer.clear();
        }

        SelectionKey sk = getSelectionKey();
        if (readBytes == -1) {
            returnBuffers();
            sk.cancel();
            socketChannel.close();
        } else if (httpRequest.isDataParsed()) {
            sk.interestOps(0);
            sk.selector().wakeup();

            NIOServer.executorService.execute(this::process);
        }
    }

    private void write(boolean setWriteInterest) throws IOException {
        SelectionKey sk = getSelectionKey();
        if (socketChannel.write(readBuffer) == -1) {
            returnBuffers();
            sk.cancel();
            socketChannel.close();
        } else if (readBuffer.remaining() > 0) {
            if (setWriteInterest) {
                sk.interestOps(SelectionKey.OP_WRITE);
            }
        } else {
            readBuffer.clear();
            sk.interestOps(SelectionKey.OP_READ);
        }
        if (setWriteInterest) {
            sk.selector().wakeup();
        }
    }

    private boolean processWords(HttpRequest httpRequest) throws IOException {
        GZIPInputStream gis = new GZIPInputStream(new BufferedInputStream(
                new ByteArrayInputStream(httpRequest.getData())), BufferPool.BUFFER_CAPACITY);
        bufferPool.returnOutputStreamBuffer(httpRequest.getReceived());

        String stringData = new String(IOUtils.readFully(gis, -1, true));

        gis.close();

        StringTokenizer st = new StringTokenizer(stringData);
        while (st.hasMoreTokens()) {
            wordsCounter.addWord(st.nextToken());
        }
        return true;
    }

    private void process() {
        try {
            String statusCode = null;
            String response = null;
            HttpRequest.Command command = httpRequest.getCommand();
            HttpRequest.Method method = httpRequest.getMethod();
            if (command != null && method != null && !httpRequest.isError()) {
                if (method == HttpRequest.Method.POST && command == HttpRequest.Command.DATA) {
                    try {
                        long timeStart = System.currentTimeMillis();
                        processWords(httpRequest);
                        long timeNow = System.currentTimeMillis();
                        System.out.println("Processing time (ms): " + (timeNow - timeStart));
                        statusCode = HttpHelper.HTTP_NO_CONTENT;
                        response = "OK";
                    } catch (IOException e) {
                        System.out.println("Error processing data");
                        e.printStackTrace();
                    }

                } else if (method == HttpRequest.Method.GET && command == HttpRequest.Command.COUNT) {
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

            String httpResponse = HttpHelper.getHttpResponse(statusCode, response);

            readBuffer.put(httpResponse.getBytes());
            readBuffer.flip();

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
