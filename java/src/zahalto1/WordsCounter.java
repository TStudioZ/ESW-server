package zahalto1;

import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;

public class WordsCounter {

    private final ConcurrentMap<String, Integer> words;

    public WordsCounter() {
        words = new ConcurrentHashMap<>();
    }

    public void addWord(String word) {
        //words.merge(word, 1, (oldCount, count) -> oldCount + count);
        words.put(word, 1);
    }

    public synchronized int getWordCountReset() {
        int count = words.size();
        words.clear();
        return count;
    }
}
