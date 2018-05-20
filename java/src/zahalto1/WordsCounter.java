package zahalto1;

import java.util.concurrent.ConcurrentHashMap;

public class WordsCounter {

    private final ConcurrentHashMap.KeySetView<String, Boolean> wordsSet;

    public WordsCounter() {
        this.wordsSet = ConcurrentHashMap.newKeySet(500000);
    }

    public void addWord(String word) {
        if (!wordsSet.contains(word))
            wordsSet.add(word);
    }

    public synchronized int getWordCountReset() {
        int count = wordsSet.size();
        wordsSet.clear();
        return count;
    }
}
