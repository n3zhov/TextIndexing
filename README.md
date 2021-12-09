# Text_Indexing
Программа для многопоточного текстового поиска по словам.

## Алгоритм работы

Для реализации поиска был написан класс [SearchServer](search_server.h)

```c++
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
```

Также для задачи были написаны вспомогательные классы:

## Synchronized
Шаблонный класс [Synchronized](synchronized.h), который был 
создан для упрощения контроля доступа к конкретному элементу (в нашем случае - к построенному индексу). В качестве 
подходящего примитива синхронизации был использован mutex.

## InvertedIndex
Он описан в том же файле, что и [SearchServer](search_server.h). В данной программе реализован вариант инвертированного индекса без позиции
слова в названии документа.

## Методы класса SearchServer и InvertedIndex
Реализация методов классов InvertedIndex и SearchServer находятся [тут](search_server.cpp).
```c++
    SearchServer() = default;
    explicit SearchServer(istream &document_input);
    void UpdateDocumentBase(istream &document_input);
    void AddQueriesStream(istream &query_input, ostream &search_results_output);

    void UpdateDocumentBaseSingleThread(istream &document_input);
    void AddQueriesStreamSingleThread(istream &query_input, ostream &search_results_output);
```
По порядку - конструктор сервера создаёт индекс по документам из входного потока, если такой был дан. 

Методы UpdateDocumentBase и AddQueriesStream являются обёрткой для выполнения соответствующих функций в отдельном потоке. 
Соотвествующие реализации методов запускаются при помощи async.
```c++
void SearchServer::UpdateDocumentBase(istream &document_input) {
    futures.push_back(async(&SearchServer::UpdateDocumentBaseSingleThread, this, ref(document_input)));
}

void SearchServer::AddQueriesStream(istream &query_input, ostream &search_results_output) {
    futures.push_back(async(&SearchServer::AddQueriesStreamSingleThread, this, ref(query_input), 
                            ref(search_results_output)));
}
```
- UpdateDocumentBase - создаёт новый ивертированный индекс из входного потока.
- AddQueriesStream - принимает запрос на поиск и выводит результаты в формате query: {docid, hitcount}, где docid - номер 
документа, а hitcount - кол-во совпадений слов из query в названии документа docid.

Выводятся первые 5 результатов поиска,
которые сортируются по кол-ву совпадений в порядке убывания (то есть - первый результат, это номер наиболее подходящего 
названия документа). Получить название самого документа можно при помощи функции GetDocument(size_t id).


#### Проект был выполнен в рамках курса "Основы разработки на C++: красный пояс"
