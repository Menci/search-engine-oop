#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <set>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <readline/readline.h>
#include <readline/history.h>

#include "HTMLParser/HTMLDocument.h"

#include "Arguments.h"
#include "Analysis/Analysis.h"
#include "Document/Document.h"
#include "Document/Field.h"
#include "Document/FieldType.h"
#include "Index/Index.h"
#include "Search/BooleanQuery.h"
#include "Search/PhraseQuery.h"
#include "Search/IndexSearcher.h"

void initializeReadline() {
    constexpr size_t MAX_HISTORY = 100;
    stifle_history(MAX_HISTORY);
}

bool prompt(std::string &command, std::string &argument) {
    std::string input;

    while (input.empty()) {
        char *buffer = readline(">>> ");
        if (!buffer) {
            // EOF
            return false;
        }

        input = buffer;
        boost::trim(input);
        free(buffer);
    }

    add_history(input.c_str());

    size_t spacePos = input.find(' ');

    if (spacePos != std::string::npos) {
        command = input.substr(0, spacePos);
        argument = input.substr(spacePos + 1);
    } else {
        command = input;
        argument = "";
    }

    boost::to_upper(command);

    return true;
}

void addIndex(Index &index, std::filesystem::path path) {
    std::vector<std::filesystem::path> fileList;
    for (const std::filesystem::directory_entry &entry : std::filesystem::recursive_directory_iterator(path)) {
        if (entry.is_regular_file()) {
            std::filesystem::path filePath = entry.path();
            fileList.push_back(filePath);
        }
    }

    size_t i = 0;
    for (const std::filesystem::path &filePath : fileList) {
        HTMLDocument htmlDocument = HTMLDocument(std::ifstream(filePath));
        std::string title = htmlDocument.getTitle(),
                    content = htmlDocument.getTextContent();

        Document document;
        document.add(Field("path", FieldType::Stored, filePath));
        document.add(Field("title", FieldType::Stored | FieldType::Indexed, title));
        document.add(Field("content", FieldType::Stored | FieldType::Indexed, content));

        index.addDocument(document);

        std::cout << boost::format("(%d/%d) Added: %s") % ++i % fileList.size() % filePath << std::endl;
    }
}

int main(int argc, char *argv[]) {
    Arguments arguments = parseArguments(argc, argv);

    std::set<std::string> stopWords;
    for (std::ifstream stopWordsStream(arguments.stopWordsFile); !stopWordsStream.eof(); ) {
        std::string line;
        std::getline(stopWordsStream, line);
        stopWords.insert(line);
    }

    Analysis::setStopWords(stopWords);

    Index index(arguments.indexDirectory);

    initializeReadline();
    for (std::string command, argument; prompt(command, argument); ) {
        if (command == "HELP") {
            std::cout << "ADD-INDEX [path]" << std::endl;
            std::cout << "QUERY [keywords]" << std::endl;
        } else if (command == "ADD-INDEX") {
            addIndex(index, argument);
        } else if (command == "QUERY") {
            std::vector<std::string> queryPhrases;
            boost::iter_split(queryPhrases, argument, boost::first_finder(" "));

            std::shared_ptr<Query> query;
            for (const std::string& phrase : queryPhrases) {
                if (phrase.empty()) continue;

                std::shared_ptr<Query> newPhraseQuery = std::make_shared<PhraseQuery>("content", phrase);
                if (!query) {
                    query = newPhraseQuery;
                } else {
                    query = std::make_shared<BooleanAndQuery>(query, newPhraseQuery);
                }
            }

            IndexSearcher indexSearcher(index);
            std::vector<SearchResultItem> result = indexSearcher.search(*query, 100);

            size_t i = 0;
            for (const SearchResultItem &item : result) {
                Document document = index.getDocument(item.docIDGlobal);
                std::cout << boost::format("#%d (score = %.6lf) title = %s, path = %s")
                             % ++i
                             % item.score
                             % document.getField("title")->getValue()
                             % document.getField("path")->getValue()
                          << std::endl;
            }
        } else {
            std::cout << "Unknown command: " << command << ", enter 'HELP' for help." << std::endl;
        }
    }

    std::cout << std::endl;
}
