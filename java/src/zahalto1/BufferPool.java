package zahalto1;

import java.io.ByteArrayOutputStream;
import java.nio.ByteBuffer;
import java.util.concurrent.ConcurrentLinkedQueue;

public class BufferPool {

    private final int BUFFER_CAPACITY = 8192;
    private static final int INIT_SIZE = 50;

    private ConcurrentLinkedQueue<ByteBuffer> buffers;
    private ConcurrentLinkedQueue<byte[]> bufferArrays;
    private ConcurrentLinkedQueue<char[]> charBufferArrays;
    private ConcurrentLinkedQueue<ByteArrayOutputStream> outputStreamBuffers;

    public BufferPool() {
        this.buffers = new ConcurrentLinkedQueue<>();
        this.bufferArrays = new ConcurrentLinkedQueue<>();
        this.charBufferArrays = new ConcurrentLinkedQueue<>();
        this.outputStreamBuffers = new ConcurrentLinkedQueue<>();
        for (int i = 0; i < INIT_SIZE; i++) {
            allocateBuffer();
            allocateBufferArray();
            allocateCharBufferArray();
            allocateOutputStreamBuffer();
        }
    }

    private void allocateBuffer() {
        buffers.add(ByteBuffer.allocate(BUFFER_CAPACITY));
    }

    private void allocateBufferArray() {
        bufferArrays.add(new byte[BUFFER_CAPACITY]);
    }

    private void allocateCharBufferArray() {
        charBufferArrays.add(new char[BUFFER_CAPACITY]);
    }

    private void allocateOutputStreamBuffer() {
        outputStreamBuffers.add(new ByteArrayOutputStream());
    }

    public ByteBuffer getByteBuffer() {
        //System.out.println("Allocated buffers: " + buffers.size());
        /*ByteBuffer allocatedBuffer = buffers.poll();
        if (allocatedBuffer == null) {
            allocatedBuffer = ByteBuffer.allocate(BUFFER_CAPACITY);
        }
        //System.out.println("Giving a buffer");
        return allocatedBuffer;*/
        return ByteBuffer.allocate(BUFFER_CAPACITY);
    }

    public byte[] getBufferArray() {
        /*byte[] allocatedBufferArray = bufferArrays.poll();
        if (allocatedBufferArray == null) {
            allocatedBufferArray = new byte[BUFFER_CAPACITY];
        }
        return allocatedBufferArray;*/
        return new byte[BUFFER_CAPACITY];
    }

    public char[] getCharBufferArray() {
        /*char[] allocatedCharBufferArray = charBufferArrays.poll();
        if (allocatedCharBufferArray == null) {
            allocatedCharBufferArray = new char[BUFFER_CAPACITY];
        }
        return allocatedCharBufferArray;*/
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
        //System.out.println("Buffer returned");
        //buffers.add(buffer);
    }

    public void returnBufferArray(byte[] bufferArray) {
        //bufferArrays.add(bufferArray);
    }

    public void returnCharBufferArray(char[] charBufferArray) {
        //charBufferArrays.add(charBufferArray);
    }

    public void returnOutputStreamBuffer(ByteArrayOutputStream baos) {
        baos.reset();
        outputStreamBuffers.add(baos);
    }
}
