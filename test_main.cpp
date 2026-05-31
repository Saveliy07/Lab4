#include "TextProcessor.h"
#include "TfIdfIndex.h"
#include <iostream>
#include <cassert>
#include <cmath>

bool isAlmostEqual(double a, double b, double epsilon = 1e-6) {
    return std::abs(a - b) < epsilon;
}

void testTextProcessor() {
    std::cout << "Running TextProcessor tests...\n";
    
    assert(TextProcessor::cleanWord("Hello!") == "hello");
    assert(TextProcessor::cleanWord("WORD") == "word");
    assert(TextProcessor::cleanWord("comma,") == "comma");
    assert(TextProcessor::cleanWord("multi-word") == "multiword");
    assert(TextProcessor::cleanWord("123.") == "123");
    
    std::cout << "TextProcessor tests passed!\n\n";
}

void testTfIdfCalculations() {
    std::cout << "Running TfIdfIndex tests...\n";
    
    TfIdfIndex index;
    
    std::vector<std::string> doc1 = {"apple", "banana", "apple"};
    index.addDocument("doc1.txt", doc1);
    
    std::vector<std::string> doc2 = {"banana", "orange"};
    index.addDocument("doc2.txt", doc2);

    assert(isAlmostEqual(index.getTF("apple", "doc1.txt"), 2.0 / 3.0));
    assert(isAlmostEqual(index.getTF("banana", "doc1.txt"), 1.0 / 3.0));
    assert(isAlmostEqual(index.getTF("orange", "doc1.txt"), 0.0));
    assert(isAlmostEqual(index.getTF("banana", "doc2.txt"), 0.5));

    assert(isAlmostEqual(index.getIDF("apple"), std::log(2.0 / 1.0)));
    assert(isAlmostEqual(index.getIDF("banana"), std::log(2.0 / 2.0)));
    assert(isAlmostEqual(index.getIDF("orange"), std::log(2.0 / 1.0)));
    assert(isAlmostEqual(index.getIDF("grape"), 0.0));

    double expectedTfIdfApple = (2.0 / 3.0) * std::log(2.0);
    assert(isAlmostEqual(index.getTFIDF("apple", "doc1.txt"), expectedTfIdfApple));
    assert(isAlmostEqual(index.getTFIDF("banana", "doc1.txt"), 0.0));

    assert(index.getTotalDocs() == 2);
    assert(index.getDocTotalWords("doc1.txt") == 3);
    assert(index.getDocTotalWords("doc2.txt") == 2);
    assert(index.getWordCountInDoc("apple", "doc1.txt") == 2);
    
    std::cout << "TfIdfIndex tests passed!\n\n";
}

int main() {
    std::cout << "=== STARTING TESTS ===\n\n";
    
    testTextProcessor();
    testTfIdfCalculations();
    
    std::cout << "=== ALL TESTS PASSED SUCCESSFULLY ===\n";
    return 0;
}

//g++ -std=c++14 test_main.cpp -o run_tests
//./run_tests