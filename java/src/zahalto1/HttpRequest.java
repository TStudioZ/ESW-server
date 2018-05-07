package zahalto1;

public class HttpRequest {

    enum Method {
        GET,
        POST
    }

    enum Command {
        DATA,
        COUNT
    }

    private final Method method;
    private final Command command;
    private final String data;

    public HttpRequest(Method method, Command command) {
        this.method = method;
        this.command = command;
        this.data = null;
    }

    public HttpRequest(Method method, Command command, String data) {
        this.method = method;
        this.command = command;
        this.data = data;
    }

    public Method getMethod() {
        return method;
    }

    public Command getCommand() {
        return command;
    }

    public String getData() {
        return data;
    }
}
