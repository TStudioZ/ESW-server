package zahalto1;

import java.util.concurrent.ConcurrentHashMap;

public class WordsCounter {

    private final ConcurrentHashMap.KeySetView<String, Boolean>[] wordSets
            = new ConcurrentHashMap.KeySetView[100];

    public WordsCounter() {
        for (int i = 0; i < wordSets.length; i++) {
            wordSets[i] = ConcurrentHashMap.newKeySet(50000);
        }
    }

    public void addWord(String word) {
        int index = word.length() - 1;
        if (!wordSets[index].contains(word))
            wordSets[index].add(word);
    }

    public synchronized int getWordCountReset() {
        int count = 0;
        for (int i = 0; i < wordSets.length; i++) {
            count += wordSets[i].size();
            wordSets[i].clear();
        }
        return count;
    }
}
