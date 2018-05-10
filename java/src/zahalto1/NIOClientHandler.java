package zahalto1;

import java.io.*;
import java.nio.ByteBuffer;
import java.nio.channels.SelectionKey;
import java.nio.channels.SocketChannel;
import java.util.UUID;
import java.util.zip.GZIPInputStream;

public class NIOClientHandler extends NIOHandler {

    private final int BUFFER_CAPACITY = 8192;
    private final int WRITE_BUFFER_CAPACITY = 256;

    private final SocketChannel socketChannel;
    private final WordsCounter wordsCounter;

    private ByteBuffer readBuffer;
    private ByteBuffer writeBuffer = null;

    private byte[] bytes = null;
    //private ByteArrayOutputStream received = null;

    private final HttpRequest httpRequest;

    NIOClientHandler(SocketChannel socketChannel, WordsCounter wordsCounter) throws IOException {
        super(socketChannel, SelectionKey.OP_READ);
        this.socketChannel = socketChannel;
        this.socketChannel.configureBlocking(false);
        this.wordsCounter = wordsCounter;

        this.readBuffer = ByteBuffer.allocate(BUFFER_CAPACITY);
        this.bytes = new byte[BUFFER_CAPACITY];
        //this.received = new ByteArrayOutputStream();
        this.httpRequest = new HttpRequest();
    }

    private void saveReceivedBytesToFile(byte[] bytes) {
        try (FileOutputStream fos = new FileOutputStream(new File("input", UUID.randomUUID().toString()))) {
            fos.write(bytes);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void saveReceivedWordsToFile(String[] words) {
        try (FileOutputStream fos = new FileOutputStream(new File("words", UUID.randomUUID().toString()))) {
            for (String word : words) {
                fos.write(word.getBytes());
                fos.write(0x0d);
                fos.write(0x0a);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void read() throws IOException {
        //System.out.println("NIOClientHandler reading");

        int readBytes;
        while ((readBytes = socketChannel.read(readBuffer)) > 0) {
            readBuffer.flip();
            readBuffer.get(bytes, 0, readBytes);
            //received.write(bytes, 0, readBytes);
            httpRequest.addData(bytes, readBytes);
            readBuffer.clear();
            //System.out.println("Read bytes: " + readBytes);
        }

        SelectionKey sk = getSelectionKey();
        //System.out.println("NIOClientHandler read bytes: " + readBytes);
        if (readBytes == -1) {
            System.out.println("Closing connection");
            sk.cancel();
            socketChannel.close();
        } else if (httpRequest.isDataParsed()) {
            //System.out.println("Received message: [" + received + "]");
            System.out.println("Data received");
            sk.interestOps(0);
            sk.selector().wakeup();
            NIOServer.executorService.execute(this::process);
        }
    }

    private void write(boolean setWriteInterest) throws IOException {
        //System.out.println("NIOClientHandler writing");
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

    private boolean processWords(HttpRequest httpRequest) throws IOException {
        GZIPInputStream gis = new GZIPInputStream(new ByteArrayInputStream(httpRequest.getData()));
        BufferedReader br = new BufferedReader(new InputStreamReader(gis, "UTF-8"));
        Writer sw = new StringWriter();
        char[] buffer = new char[8192];
        int readChars = 0;
        while ((readChars = br.read(buffer)) > 0) {
            sw.write(buffer, 0, readChars);
        }
        String stringData = sw.toString();
        gis.close();
        br.close();
        sw.close();

        //System.out.println(stringData);

        //Matcher m = Pattern.compile("(?U)(\\w+)").matcher(stringData);
        /*Matcher m = Pattern.compile("(\\S+)").matcher(stringData);
        String word;
        while (m.find()) {
            word = m.group();
            //System.out.println(word);
            wordsCounter.addWord(word);
        }*/

        String[] words = stringData.split("\\s+");
        for (int i = 0; i < words.length; i++) {
            wordsCounter.addWord(words[i]);
            //System.out.println(words[i]);
        }
        //saveReceivedWordsToFile(words);
        return true;
    }

    private void process() {
        //System.out.println("NIOClientHandler processing");
        try {
            //byte[] receivedBytes = received.toByteArray();
            //saveReceivedBytesToFile(receivedBytes);
            //HttpRequest httpRequest = HttpHelper.getHttpRequest(receivedBytes);
            //this.received.reset();
            //saveReceivedBytesToFile(httpRequest.getData());

            String statusCode = null;
            String response = null;
            HttpRequest.Command command = httpRequest.getCommand();
            HttpRequest.Method method = httpRequest.getMethod();
            if (command != null) {
                if (method == HttpRequest.Method.POST && command == HttpRequest.Command.DATA) {
                    try {
                        processWords(httpRequest);
                        statusCode = HttpHelper.HTTP_NO_CONTENT;
                        response = "OK";

                        System.out.println("POST data");
                    } catch (IOException e) {
                        System.out.println("Error processing data");
                        e.printStackTrace();
                    }

                } else if (method == HttpRequest.Method.GET && command == HttpRequest.Command.COUNT) {
                    int wordCount = wordsCounter.getWordCountReset();

                    statusCode = HttpHelper.HTTP_OK;
                    response = "" + wordCount;

                    System.out.println("GET count (" + wordCount + ")");
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
