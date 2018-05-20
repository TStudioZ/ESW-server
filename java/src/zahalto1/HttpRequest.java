package zahalto1;

import java.io.ByteArrayOutputStream;
import java.io.IOException;

public class HttpRequest {

    enum Method {
        GET,
        POST
    }

    enum Command {
        DATA,
        COUNT
    }

    private Method method;
    private Command command;
    private final ByteArrayOutputStream received;

    Method getMethod() {
        return method;
    }

    Command getCommand() {
        return command;
    }

    public ByteArrayOutputStream getReceived() {
        return received;
    }

    public byte[] getData() {
        return received.toByteArray();
    }

    public HttpRequest(ByteArrayOutputStream buffer) {
        this.received = buffer;
    }

    private static final String NEW_LINE = "\r\n";
    private void parseHeader() throws IOException {
        String messageString = new String(received.toByteArray(), "UTF-8");

        String[] lines = messageString.split(NEW_LINE);
        String[] header = lines[0].split(" ");
        if (header.length < 2) {
            error = true;
        }
        if (header[0].equalsIgnoreCase("POST")) {
            method = HttpRequest.Method.POST;
        } else if (header[0].equalsIgnoreCase("GET")) {
            method = HttpRequest.Method.GET;
        }
        if (header[1].equals("/esw/myserver/data")) {
            command = HttpRequest.Command.DATA;
        } else if (header[1].equals("/esw/myserver/count")) {
            command = HttpRequest.Command.COUNT;
        }

        for (int i = 0; i < lines.length; i++) {
            if (lines[i].startsWith("Content-Length")) {
                String[] parts = lines[i].split(": ");
                contentSize = Integer.parseInt(parts[1]);
                break;
            }
        }

        received.reset();
    }

    private boolean error = false;

    public boolean isError() {
        return error;
    }

    private boolean headerParsed = false;

    public boolean isHeaderParsed() {
        return headerParsed;
    }

    private boolean dataParsed = false;

    public boolean isDataParsed() {
        return dataParsed;
    }

    private int readBytes = 0;
    private int dataOffset = 0;
    private int contentSize = 0;
    private byte prev1 = 0x0, prev2 = 0x0;
    private byte curr1 = 0x0, curr2 = 0x0;
    public void addData(byte[] data, int len) throws IOException {
        int index = 0;

        if (!headerParsed) {
            if (index >= len)
                return;
            if (readBytes == 0) {
                prev1 = data[index++];
                received.write(prev1);
                readBytes++;
                if (index >= len)
                    return;
            }
            if (readBytes == 1) {
                prev2 = data[index++];
                received.write(prev2);
                readBytes++;
                if (index >= len)
                    return;
            }
            if (readBytes == 2) {
                curr1 = data[index++];
                received.write(curr1);
                readBytes++;
                if (index >= len)
                    return;
            }

            for (; index < len; index++, readBytes++) {
                curr2 = data[index];
                received.write(curr2);
                if (curr1 == 0x0d && curr2 == 0x0a
                        && prev1 == 0x0d && prev2 == 0x0a) {
                    dataOffset = readBytes + 1;
                    index++;
                    parseHeader();
                    headerParsed = true;
                    break;
                }
                prev1 = prev2;
                prev2 = curr1;
                curr1 = curr2;
            }
        }

        if (headerParsed) {
            received.write(data, index, len - index);
            readBytes += len;
            if (readBytes - dataOffset >= contentSize) {
                dataParsed = true;
            }
        }
    }
}
