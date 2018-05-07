package zahalto1;

import java.time.ZoneOffset;
import java.time.ZonedDateTime;
import java.time.format.DateTimeFormatter;

abstract class HttpHelper {

    static final String HTTP_OK = "200 OK";
    static final String HTTP_NO_CONTENT = "204 No Content";
    static final String HTTP_BAD_REQUEST = "400 Bad Request";
    private static final String NEW_LINE = "\r\n";

    static HttpRequest getHttpRequest(String message) {
        String[] lines = message.split(NEW_LINE);
        String[] header = lines[0].split(" ");
        if (header.length < 2) {
            return new HttpRequest(null, null);
        }
        HttpRequest.Method method = null;
        if (header[0].equals("POST")) {
            method = HttpRequest.Method.POST;
        } else if (header[0].equals("GET")) {
            method = HttpRequest.Method.GET;
        }
        HttpRequest.Command cmd = null;
        if (header[1].equals("/esw/myserver/data")) {
            cmd = HttpRequest.Command.DATA;
        } else if (header[1].equals("/esw/myserver/count")) {
            cmd = HttpRequest.Command.COUNT;
        }
        String data = lines[lines.length - 1];
        return new HttpRequest(method, cmd, data);
    }

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
