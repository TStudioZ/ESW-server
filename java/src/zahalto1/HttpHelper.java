package zahalto1;

import java.time.ZoneOffset;
import java.time.ZonedDateTime;
import java.time.format.DateTimeFormatter;

abstract class HttpHelper {

    static final String HTTP_OK = "200 OK";
    static final String HTTP_NO_CONTENT = "204 No Content";
    static final String HTTP_BAD_REQUEST = "400 Bad Request";
    private static final String NEW_LINE = "\r\n";

    /*static HttpRequest getHttpRequest(byte[] message) throws IOException {
        String messageString = new String(message, "UTF-8");

        String[] lines = messageString.split(NEW_LINE);
        String[] header = lines[0].split(" ");
        if (header.length < 2) {
            return new HttpRequest(null, null);
        }
        HttpRequest.Method method = null;
        if (header[0].equalsIgnoreCase("POST")) {
            method = HttpRequest.Method.POST;
        } else if (header[0].equalsIgnoreCase("GET")) {
            method = HttpRequest.Method.GET;
        }
        HttpRequest.Command cmd = null;
        if (header[1].equals("/esw/myserver/data")) {
            cmd = HttpRequest.Command.DATA;
        } else if (header[1].equals("/esw/myserver/count")) {
            cmd = HttpRequest.Command.COUNT;
        }

        int contentSize = 0;
        for (int i = 0; i < lines.length; i++) {
            if (lines[i].startsWith("Content-Length")) {
                String[] parts = lines[i].split(": ");
                contentSize = Integer.parseInt(parts[1]);
                break;
            }
        }

        ByteArrayInputStream data = null;
        if (contentSize > 0) {
            int offset = 2;
            byte prev1 = message[0], prev2 = message[1];
            byte curr1, curr2;
            for (int i = 2; i < message.length - 1; i++, offset++) {
                curr1 = message[i];
                curr2 = message[i + 1];
                if (curr1 == 0x0d && curr2 == 0x0a
                        && prev1 == 0x0d && prev2 == 0x0a) {
                    offset += 2;
                    break;
                }
                prev1 = prev2;
                prev2 = curr1;
            }
            //data = Arrays.copyOfRange(message, offset, message.length);
            data = new ByteArrayInputStream(message, offset, contentSize);
        }
        return new HttpRequest(method, cmd, data);
    }*/

    static String getHttpResponse(String code, String data) {
        StringBuilder sb = new StringBuilder();

        // Status Code
        sb.append("HTTP/1.1 ");
        sb.append(code);
        sb.append(NEW_LINE);

        // Date
        sb.append("Date: ");
        sb.append(DateTimeFormatter.RFC_1123_DATE_TIME.format(ZonedDateTime.now(ZoneOffset.UTC)));
        sb.append(NEW_LINE);

        // Content-Type
        sb.append("Content-Type: text/plain");
        sb.append(NEW_LINE);

        // Content-Length
        sb.append("Content-Length: ");
        if (data != null) {
            sb.append(String.valueOf(data.length()));
        } else {
            sb.append("0");
        }
        sb.append(NEW_LINE);

        // Content
        if (data != null) {
            sb.append(NEW_LINE);
            sb.append(data);
            sb.append(NEW_LINE);
        }

        return sb.toString();
    }
}
