package zahalto1;

import java.nio.channels.SelectableChannel;
import java.nio.channels.SelectionKey;

abstract class NIOHandler implements Runnable {

    private final SelectableChannel selectableChannel;
    private final int selectableOps;
    private SelectionKey selectionKey = null;

    protected NIOHandler(SelectableChannel selectableChannel, int selectableOps) {
        this.selectableChannel = selectableChannel;
        this.selectableOps = selectableOps;
    }

    public SelectableChannel getSelectableChannel() {
        return selectableChannel;
    }

    public int getSelectableOps() {
        return selectableOps;
    }

    public SelectionKey getSelectionKey() {
        return selectionKey;
    }

    public void setSelectionKey(SelectionKey selectionKey) {
        this.selectionKey = selectionKey;
    }
}
