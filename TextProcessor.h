#ifndef TEXT_PROCESSOR_H
#define TEXT_PROCESSOR_H

#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iterator>

namespace TextProcessor {
    inline std::string cleanWord(std::string word) {
        word.erase(std::remove_if(word.begin(), word.end(), [](unsigned char c) { return std::ispunct(c); }), word.end());
        std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c) { return std::tolower(c); });
        return word;
    }

    inline std::vector<std::string> extractWords(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) return {};

        std::vector<std::string> words((std::istream_iterator<std::string>(file)), std::istream_iterator<std::string>());
        std::transform(words.begin(), words.end(), words.begin(), cleanWord);
        words.erase(std::remove_if(words.begin(), words.end(), [](const std::string& w){ return w.empty(); }), words.end());
        return words;
    }
}

#endif