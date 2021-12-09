#include "search_server.h"
#include "iterator_range.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>

vector<string> SplitIntoWords(const string& line) {
    istringstream words_input(line);
    return {istream_iterator<string>(words_input), istream_iterator<string>()};
}

InvertedIndex::InvertedIndex(std::istream& document_input)
{
    index.reserve(15000);
    docs_count = 0;
    std::string line;
    while(std::getline(document_input, line)){
        ++docs_count;
        size_t id = docs_count - 1;

        for (std::string& word : SplitIntoWords(line)){
            auto& id_to_count = index[move(word)];
            if (!id_to_count.empty() && id_to_count.back().first == id){
                ++id_to_count.back().second;
            }
            else{
                id_to_count.emplace_back(id, 1);
            }
        }
    }
}

SearchServer::SearchServer(istream &document_input) {

    UpdateDocumentBaseSingleThread(document_input);
}

void SearchServer::UpdateDocumentBase(istream &document_input) {

    auto func = [this, d = ref(document_input)]{
        UpdateDocumentBaseSingleThread(d);
    };
    futures.push_back(async(func));
    //func();
}

void SearchServer::AddQueriesStream(istream &query_input,
                                    ostream &search_results_output) {

    futures.push_back(async(&SearchServer::AddQueriesStreamSingleThread, this, ref(query_input), ref(search_results_output)));

    //func();
}



void SearchServer::UpdateDocumentBaseSingleThread(istream &document_input) {
    InvertedIndex new_index(document_input);
    index.GetAccess().ref_to_value = move(new_index);
}

void SearchServer::AddQueriesStreamSingleThread(istream &query_input, ostream &search_results_output){


    std::vector<size_t> ids_to_count;
    std::vector<int64_t> ids;
    for (string current_query; getline(query_input, current_query);) {
        const auto words = SplitIntoWords(current_query);
        {
            auto accessor = index.GetAccess();
            ids_to_count.assign(accessor.ref_to_value.GetSize(), 0);
            ids.resize(accessor.ref_to_value.GetSize());
            for (const auto &word: words) {
                const vector<pair<size_t, size_t>>* items;
                items = &accessor.ref_to_value.Lookup(word);
                for (auto &[docid, value]: *items) {
                    ids_to_count[docid] += value;
                }
            }
        }

        iota(ids.begin(), ids.end(), 0);
        partial_sort(std::begin(ids), Head(ids, 5).end(), std::end(ids),
                     [&ids_to_count](int64_t lhs, int64_t rhs)
                     {
                         return std::pair(ids_to_count[lhs], -lhs) > std::pair(ids_to_count[rhs], -rhs);
                     });

        search_results_output << current_query << ':';
        for (size_t docid: Head(ids, 5)) {
            size_t hitcount = ids_to_count[docid];
            if(hitcount == 0){
                break;
            }
            search_results_output << " {"
                                  << "docid: " << docid << ", "
                                  << "hitcount: " << hitcount << '}';
        }
        search_results_output << endl;
    }
}

const vector<pair<size_t, size_t>> &
InvertedIndex::Lookup(const string &word) const {
    static const std::vector<std::pair<size_t, size_t>> result;
    if (const auto it = index.find(word); it != index.end())
    {
        return it->second;
    }
    return result;
}
