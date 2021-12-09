#pragma once

#include <istream>
#include <list>
#include <map>
#include <unordered_map>
#include <ostream>
#include <set>
#include <string>
#include <vector>
#include <future>
#include "synchronized.h"
using namespace std;

class InvertedIndex {
public:
    InvertedIndex(std::istream& document_input);

    InvertedIndex() = default;
    const vector<pair<size_t, size_t>> &Lookup(const string &word) const;

    size_t GetSize() const{
        return docs_count;
    }

private:
    unordered_map<string, vector<pair<size_t, size_t>>> index;
    size_t docs_count = 0;
};

class SearchServer {
public:
    SearchServer() = default;
    explicit SearchServer(istream &document_input);
    void UpdateDocumentBase(istream &document_input);
    void AddQueriesStream(istream &query_input, ostream &search_results_output);

    void UpdateDocumentBaseSingleThread(istream &document_input);
    void AddQueriesStreamSingleThread(istream &query_input, ostream &search_results_output);
private:
    Synchronized<InvertedIndex> index;
    vector<future<void>> futures;
};
