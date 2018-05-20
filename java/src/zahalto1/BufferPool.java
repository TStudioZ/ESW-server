package zahalto1;

import java.io.ByteArrayOutputStream;
import java.nio.ByteBuffer;
import java.util.concurrent.ConcurrentLinkedQueue;

public class BufferPool {

    public static final int BUFFER_CAPACITY = 262144;
    private static final int INIT_SIZE = 75;

    private ConcurrentLinkedQueue<ByteBuffer> buffers;
    private ConcurrentLinkedQueue<ByteArrayOutputStream> outputStreamBuffers;

    public BufferPool() {
        this.buffers = new ConcurrentLinkedQueue<>();
        this.outputStreamBuffers = new ConcurrentLinkedQueue<>();
        for (int i = 0; i < INIT_SIZE; i++) {
            allocateBuffer();
            allocateOutputStreamBuffer();
        }
    }

    private void allocateBuffer() {
        buffers.add(ByteBuffer.allocate(BUFFER_CAPACITY));
    }

    private void allocateOutputStreamBuffer() {
        outputStreamBuffers.add(new ByteArrayOutputStream());
    }

    public ByteBuffer getByteBuffer() {
        ByteBuffer allocatedBuffer = buffers.poll();
        if (allocatedBuffer == null) {
            allocatedBuffer = ByteBuffer.allocate(BUFFER_CAPACITY);
        }
        return allocatedBuffer;
    }

    public byte[] getBufferArray() {
        return new byte[BUFFER_CAPACITY];
    }

    public char[] getCharBufferArray() {
        return new char[BUFFER_CAPACITY];
    }

    public ByteArrayOutputStream getOutputStreamBuffer() {
        ByteArrayOutputStream baos = outputStreamBuffers.poll();
        if (baos == null) {
            baos = new ByteArrayOutputStream();
        }
        return baos;
    }

    public void returnBuffer(ByteBuffer buffer) {
        buffer.clear();
        buffers.add(buffer);
    }

    public void returnOutputStreamBuffer(ByteArrayOutputStream baos) {
        baos.reset();
        outputStreamBuffers.add(baos);
    }
}
