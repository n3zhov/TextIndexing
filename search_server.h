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

    const string& GetDocument(size_t id) const {
        return docs[id];
    }

    size_t GetSize() const{
        return docs.size();
    }

private:
    unordered_map<string, vector<pair<size_t, size_t>>> index;
    vector<string> docs;
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
