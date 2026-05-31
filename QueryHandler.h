#ifndef QUERY_HANDLER_H
#define QUERY_HANDLER_H

#include "TfIdfIndex.h"
#include "TextProcessor.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <numeric>

class QueryHandler {
private:
    const TfIdfIndex& index;

public:
    explicit QueryHandler(const TfIdfIndex& idx) : index(idx) {}

    void processQuery(const std::string& queryLine) const {
        std::istringstream iss(queryLine);
        std::vector<std::string> tokens((std::istream_iterator<std::string>(iss)), std::istream_iterator<std::string>());
        
        if (tokens.empty()) return;

        std::string cmd = tokens[0];
        std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);

        if (cmd == "WORD" && tokens.size() >= 2) {
            handleWordQuery(TextProcessor::cleanWord(tokens[1]));
        } else if (cmd == "WORD_IN_DOC" && tokens.size() >= 3) {
            handleWordInDocQuery(TextProcessor::cleanWord(tokens[1]), tokens[2]);
        } else if (cmd == "DOC" && tokens.size() >= 2) {
            handleDocQuery(tokens[1]);
        } else if (cmd == "QUERY" && tokens.size() >= 2) {
            std::vector<std::string> queryWords(tokens.begin() + 1, tokens.end());
            std::transform(queryWords.begin(), queryWords.end(), queryWords.begin(), TextProcessor::cleanWord);
            handleSearchQuery(queryWords);
        } else {
            std::cout << "Invalid query format or not enough arguments.\n";
        }
    }

private:
    void handleWordQuery(const std::string& word) const {
        std::cout << "Word: " << word << "\nDocuments total: " << index.getTotalDocs() << "\n";
        auto docs = index.getDocsWithWord(word);
        int docCount = docs ? docs->size() : 0;
        
        std::cout << "Documents with word: " << docCount << "\n";
        std::cout << "IDF: " << std::fixed << std::setprecision(4) << index.getIDF(word) << "\nAppears in:\n";
        
        if (docs) {
            std::vector<std::string> docNames;
            std::transform(docs->begin(), docs->end(), std::back_inserter(docNames), [](const auto& p){ return p.first; });
            std::sort(docNames.begin(), docNames.end());
            std::for_each(docNames.begin(), docNames.end(), [](const std::string& d) { std::cout << " - " << d << "\n"; });
        }
    }

    void handleWordInDocQuery(const std::string& word, const std::string& docName) const {
        std::cout << "Word: " << word << "\nDocument: " << docName << "\n";
        std::cout << "Count: " << index.getWordCountInDoc(word, docName) << "\n";
        std::cout << "TF: " << std::fixed << std::setprecision(4) << index.getTF(word, docName) << "\n";
        std::cout << "TF-IDF: " << std::fixed << std::setprecision(4) << index.getTFIDF(word, docName) << "\n";
    }

    void handleDocQuery(const std::string& docName) const {
        std::cout << "Document: " << docName << "\nTotal words: " << index.getDocTotalWords(docName) << "\n";
        auto uniqueWords = index.getUniqueWordsInDoc(docName);
        std::cout << "Unique words: " << uniqueWords.size() << "\nTop words:\n";

        std::sort(uniqueWords.begin(), uniqueWords.end(), [&](const std::string& a, const std::string& b) {
            double tfidfA = index.getTFIDF(a, docName);
            double tfidfB = index.getTFIDF(b, docName);
            if (std::abs(tfidfA - tfidfB) > 1e-9) return tfidfA > tfidfB;
            return a < b;
        });

        int count = 1;
        auto endIt = uniqueWords.size() > 5 ? uniqueWords.begin() + 5 : uniqueWords.end();
        std::for_each(uniqueWords.begin(), endIt, [&](const std::string& w) {
            std::cout << count++ << ". " << w << " (" << std::fixed << std::setprecision(4) << index.getTFIDF(w, docName) << ")\n";
        });
    }

    void handleSearchQuery(const std::vector<std::string>& queryWords) const {
        std::cout << "Query:";
        std::for_each(queryWords.begin(), queryWords.end(), [](const std::string& w) { std::cout << " " << w; });
        std::cout << "\nResults:\n";

        std::vector<std::pair<std::string, double>> docScores;
        const auto& allDocs = index.getAllDocs();
        
        std::transform(allDocs.begin(), allDocs.end(), std::back_inserter(docScores), [&](const std::string& doc) {
            double score = std::accumulate(queryWords.begin(), queryWords.end(), 0.0, [&](double sum, const std::string& w) {
                return sum + index.getTFIDF(w, doc);
            });
            return std::make_pair(doc, score);
        });

        docScores.erase(std::remove_if(docScores.begin(), docScores.end(), [](const std::pair<std::string, double>& p) {
            return p.second <= 1e-9;
        }), docScores.end());

        std::sort(docScores.begin(), docScores.end(), [](const auto& a, const auto& b) {
            if (std::abs(a.second - b.second) > 1e-9) return a.second > b.second;
            return a.first < b.first;
        });

        int count = 1;
        std::for_each(docScores.begin(), docScores.end(), [&](const auto& pair) {
            std::cout << count++ << ". " << pair.first << " (" << std::fixed << std::setprecision(4) << pair.second << ")\n";
        });
    }
};

#endif