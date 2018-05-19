package zahalto1;

import java.util.concurrent.ConcurrentHashMap;

public class WordsCounter {

    //private final ConcurrentMap<String, Integer> words;
    private final ConcurrentHashMap.KeySetView<String, Boolean> wordsSet;

    public WordsCounter() {
        //this.words = new ConcurrentHashMap<>();
        this.wordsSet = ConcurrentHashMap.newKeySet(300000);
    }

    public void addWord(String word) {
        //words.merge(word, 1, (oldCount, count) -> oldCount + count);
        //words.put(word, 1);
        wordsSet.add(word);
    }

    public synchronized int getWordCountReset() {
        //int count = words.size();
        //words.clear();
        int count = wordsSet.size();
        wordsSet.clear();
        return count;
    }
}
