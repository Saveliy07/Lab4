#include "TextProcessor.h"
#include "TfIdfIndex.h"
#include "QueryHandler.h"
#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <algorithm>

struct Line {
    std::string value;
    operator std::string() const { return value; }
};

std::istream& operator>>(std::istream& is, Line& line) {
    return std::getline(is, line.value);
}

int main() {
    std::ifstream docListFile("documents.txt");
    if (!docListFile) {
        std::cerr << "Error: Could not open documents.txt\n";
        return 1;
    }

    TfIdfIndex index;
    
    std::vector<std::string> docNames((std::istream_iterator<std::string>(docListFile)), std::istream_iterator<std::string>());
    
    std::for_each(docNames.begin(), docNames.end(), [&index](const std::string& docName) {
        std::vector<std::string> words = TextProcessor::extractWords(docName);
        index.addDocument(docName, words);
    });

    QueryHandler handler(index);

    (void)std::find_if(std::istream_iterator<Line>(std::cin), std::istream_iterator<Line>(), [&handler](const Line& line) {
        std::string query = line.value;
        if (query.empty() || query == "EXIT") return true; 
        
        handler.processQuery(query);
        std::cout << std::endl;
        
        return false; 
    });

    return 0;
}

//g++ -std=c++14 main.cpp -o LABA4
//./LABA4