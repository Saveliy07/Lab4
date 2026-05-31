#ifndef TFIDF_INDEX_H
#define TFIDF_INDEX_H

#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <cmath>
#include <algorithm>

class TfIdfIndex {
private:
    std::unordered_map<std::string, std::unordered_map<std::string, int>> termDocCount;
    std::unordered_map<std::string, int> docTotalWords;
    std::set<std::string> documents;

public:
    void addDocument(const std::string& docName, const std::vector<std::string>& words) {
        documents.insert(docName);
        docTotalWords[docName] = words.size();
        std::for_each(words.begin(), words.end(), [&](const std::string& word) { termDocCount[word][docName]++; });
    }

    double getTF(const std::string& word, const std::string& docName) const {
        if (docTotalWords.at(docName) == 0) return 0.0;
        auto it = termDocCount.find(word);
        if (it == termDocCount.end()) return 0.0;
        auto docIt = it->second.find(docName);
        if (docIt == it->second.end()) return 0.0;
        return static_cast<double>(docIt->second) / docTotalWords.at(docName);
    }

    double getIDF(const std::string& word) const {
        if (documents.empty()) return 0.0;
        auto it = termDocCount.find(word);
        int df = (it != termDocCount.end()) ? it->second.size() : 0;
        if (df == 0) return 0.0;
        return std::log(static_cast<double>(documents.size()) / df);
    }

    double getTFIDF(const std::string& word, const std::string& docName) const {
        return getTF(word, docName) * getIDF(word);
    }

    size_t getTotalDocs() const { return documents.size(); }
    
    const std::set<std::string>& getAllDocs() const { return documents; }

    const std::unordered_map<std::string, int>* getDocsWithWord(const std::string& word) const {
        auto it = termDocCount.find(word);
        return (it != termDocCount.end()) ? &(it->second) : nullptr;
    }

    int getWordCountInDoc(const std::string& word, const std::string& docName) const {
        auto it = termDocCount.find(word);
        if (it != termDocCount.end()) {
            auto docIt = it->second.find(docName);
            if (docIt != it->second.end()) return docIt->second;
        }
        return 0;
    }

    int getDocTotalWords(const std::string& docName) const {
        auto it = docTotalWords.find(docName);
        return (it != docTotalWords.end()) ? it->second : 0;
    }

    std::vector<std::string> getUniqueWordsInDoc(const std::string& docName) const {
        std::vector<std::string> uniqueWords;
        std::for_each(termDocCount.begin(), termDocCount.end(), [&](const auto& pair) {
            if (pair.second.count(docName) > 0) {
                uniqueWords.push_back(pair.first);
            }
        });
        return uniqueWords;
    }
};

#endif